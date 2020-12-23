#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "echo.grpc.pb.h"

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerAsyncResponseWriter;
using grpc::Status;

using namespace simple;

class AsyncEchoImpl final {
public:
	~AsyncEchoImpl() {
		server_->Shutdown();
        // 항상 서버 후에 완료 대기열을 종료합니다.
		cq_->Shutdown();
	}

    // 이 코드에는 종료 처리가 없습니다.
    void Run() {
        std::string server_address("0.0.0.0:50051");

        ServerBuilder builder;
        // 인증 메커니즘 없이 지정된 주소를 수신합니다.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // 클라이언트와 통신할 인스턴스로 "service_"를 등록합니다. 이 경우 *비동기식* 서비스에 해당합니다.
        builder.RegisterService(&service_);
        // gRPC 런타임과 비동기 통신에 사용되는 완료 큐를 잡습니다.
        cq_ = builder.AddCompletionQueue();
        // 마지막으로 서버를 조립합니다.
        server_ = builder.BuildAndStart();

        std::cout << "Server listening on " << server_address << std::endl;

        // 서버의 기본 루프로 진행합니다.
        HandleRpcs();
    }

private:
    // 요청을 처리하는 데 필요한 상태와 논리를 포함하는 클래스입니다.
    class CallData {
    public:
        /* 
        "서비스" 인스턴스(이 경우 비동기 서버를 나타냄)와 
        gRPC 런타임과의 비동기 통신에 사용되는 완료 대기열 "cq"를 사용합니다. 
        */
        CallData(EchoService::AsyncService* service, ServerCompletionQueue* cq)
            : service_(service), cq_(cq), responder_(&ctx_), status_(CallStatus::CREATE) {
            // 제공 로직을 즉시 호출합니다.
            Proceed();
        }

        void Proceed() {
            if (status_ == CallStatus::CREATE) {
                // 이 인스턴스를 PROGE 상태로 전환합니다.
                status_ = CallStatus::PROCESS;

                /*
                초기 CREATE 상태의 일부로 Echo 요청을 시스템에서 처리하도록 *Request* 합니다. 
                이 요청에서 "this" 동작은 요청을 고유하게 식별하는 태그입니다
                (다른 호출 데이터 인스턴스가 다른 요청을 동시에 처리할 수 있도록). 
                이 경우 이 호출 데이터 인스턴스의 메모리 주소입니다. 
                */
                service_->RequestEcho(
                    &ctx_, 
                    &request_, 
                    &responder_, 
                    cq_, cq_,
                    this);
            }
            else if (status_ == CallStatus::PROCESS) {
                /*
                이 CallData에 대한 인스턴스를 처리하는 동안 
                새 클라이언트에 서비스를 제공하기 위해 
                새 통화 데이터 인스턴스를 생성합니다. 
                인스턴스가 FINGINE 상태의 일부로 자체 할당 해제됩니다. 
                */
                new CallData(service_, cq_);
                // 실제 처리
                std::string prefix("Echo ");
                prefix.append(request_.text());

                reply_.set_echo(prefix);
                reply_.set_size(prefix.length());

                std::cout << "size : " << request_.size() << std::endl;

                /* 이제 끝났습니다! 
                이 인스턴스의 메모리 주소를 이벤트에 대한 
                고유 식별 태그로 사용하여 gRPC 런타임에 완료되었음을 알립니다. 
                */
                status_ = CallStatus::FINISH;
                responder_.Finish(reply_, Status::OK, this);
            }
            else {
                GPR_ASSERT(status_ == CallStatus::FINISH);
                // FINISH 상태가 되면 자체 할당을 취소합니다(CallData)..
                delete this;
            }
        }
    private:
        EchoService::AsyncService* service_;
        // 비동기 서버 통지를 위한 생산자-소비자 대기열.
        ServerCompletionQueue* cq_;
        // 압축, 인증 사용과 같은 측면을 수정하고 메타데이터를 클라이언트로 다시 보낼 수 있는 rpc 컨텍스트입니다.
        ServerContext ctx_;

        // 클라이언트로부터 얻은 정보입니다.
        Message request_;
        // 클라이언트에게 다시 보내는 내용
        EchoMessage reply_;

        // 클라이언트로 돌아가기 위한 수단입니다.
        ServerAsyncResponseWriter<EchoMessage> responder_;

        // 다음 상태를 가진 작은 상태 시스템을 구현해 보겠습니다.
        enum class CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;  // 현재 서비스 상태입니다.
    };
    void HandleRpcs() {
        // 새 클라이언트에 서비스를 제공할 새 CallData 인스턴스를 생성합니다.
        new CallData(&service_, cq_.get());
        void* tag;  // uniquely identifies a request.
        bool ok;
        while (true) {
            /*
            완료 대기열에서 다음 이벤트를 읽기 위해 대기하는 것을 차단합니다. 
            이벤트는 태그로 고유하게 식별되며, 이 경우 CallData 인스턴스의 메모리 주소입니다. 
            Next(다음)의 반환 값은 항상 선택해야 합니다. 
            이 반환 값은 이벤트가 있는지 또는 cq_이(가) 종료되는지 여부를 알려줍니다. 
            */
            GPR_ASSERT(cq_->Next(&tag, &ok));
            GPR_ASSERT(ok);
            static_cast<CallData*>(tag)->Proceed();
        }
    }


	std::unique_ptr<ServerCompletionQueue> cq_;
	EchoService::AsyncService service_;
	std::unique_ptr<Server> server_;
};

int main() {
    AsyncEchoImpl server;
    server.Run();

	return 0;
}