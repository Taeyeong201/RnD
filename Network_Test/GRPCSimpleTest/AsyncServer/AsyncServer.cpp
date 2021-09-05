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
        // �׻� ���� �Ŀ� �Ϸ� ��⿭�� �����մϴ�.
		cq_->Shutdown();
	}

    // �� �ڵ忡�� ���� ó���� �����ϴ�.
    void Run() {
        std::string server_address("0.0.0.0:50051");

        ServerBuilder builder;
        // ���� ��Ŀ���� ���� ������ �ּҸ� �����մϴ�.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Ŭ���̾�Ʈ�� ����� �ν��Ͻ��� "service_"�� ����մϴ�. �� ��� *�񵿱��* ���񽺿� �ش��մϴ�.
        builder.RegisterService(&service_);
        // gRPC ��Ÿ�Ӱ� �񵿱� ��ſ� ���Ǵ� �Ϸ� ť�� ����ϴ�.
        cq_ = builder.AddCompletionQueue();
        // ���������� ������ �����մϴ�.
        server_ = builder.BuildAndStart();

        std::cout << "Server listening on " << server_address << std::endl;

        // ������ �⺻ ������ �����մϴ�.
        HandleRpcs();
    }

private:
    // ��û�� ó���ϴ� �� �ʿ��� ���¿� ���� �����ϴ� Ŭ�����Դϴ�.
    class CallData {
    public:
        /* 
        "����" �ν��Ͻ�(�� ��� �񵿱� ������ ��Ÿ��)�� 
        gRPC ��Ÿ�Ӱ��� �񵿱� ��ſ� ���Ǵ� �Ϸ� ��⿭ "cq"�� ����մϴ�. 
        */
        CallData(EchoService::AsyncService* service, ServerCompletionQueue* cq)
            : service_(service), cq_(cq), responder_(&ctx_), status_(CallStatus::CREATE) {
            // ���� ������ ��� ȣ���մϴ�.
            Proceed();
        }

        void Proceed() {
            if (status_ == CallStatus::CREATE) {
                // �� �ν��Ͻ��� PROGE ���·� ��ȯ�մϴ�.
                status_ = CallStatus::PROCESS;

                /*
                �ʱ� CREATE ������ �Ϻη� Echo ��û�� �ý��ۿ��� ó���ϵ��� *Request* �մϴ�. 
                �� ��û���� "this" ������ ��û�� �����ϰ� �ĺ��ϴ� �±��Դϴ�
                (�ٸ� ȣ�� ������ �ν��Ͻ��� �ٸ� ��û�� ���ÿ� ó���� �� �ֵ���). 
                �� ��� �� ȣ�� ������ �ν��Ͻ��� �޸� �ּ��Դϴ�. 
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
                �� CallData�� ���� �ν��Ͻ��� ó���ϴ� ���� 
                �� Ŭ���̾�Ʈ�� ���񽺸� �����ϱ� ���� 
                �� ��ȭ ������ �ν��Ͻ��� �����մϴ�. 
                �ν��Ͻ��� FINGINE ������ �Ϻη� ��ü �Ҵ� �����˴ϴ�. 
                */
                new CallData(service_, cq_);
                // ���� ó��
                std::string prefix("Echo ");
                prefix.append(request_.text());

                reply_.set_echo(prefix);
                reply_.set_size(prefix.length());

                std::cout << "size : " << request_.size() << std::endl;

                /* ���� �������ϴ�! 
                �� �ν��Ͻ��� �޸� �ּҸ� �̺�Ʈ�� ���� 
                ���� �ĺ� �±׷� ����Ͽ� gRPC ��Ÿ�ӿ� �Ϸ�Ǿ����� �˸��ϴ�. 
                */
                status_ = CallStatus::FINISH;
                responder_.Finish(reply_, Status::OK, this);
            }
            else {
                GPR_ASSERT(status_ == CallStatus::FINISH);
                // FINISH ���°� �Ǹ� ��ü �Ҵ��� ����մϴ�(CallData)..
                delete this;
            }
        }
    private:
        EchoService::AsyncService* service_;
        // �񵿱� ���� ������ ���� ������-�Һ��� ��⿭.
        ServerCompletionQueue* cq_;
        // ����, ���� ���� ���� ������ �����ϰ� ��Ÿ�����͸� Ŭ���̾�Ʈ�� �ٽ� ���� �� �ִ� rpc ���ؽ�Ʈ�Դϴ�.
        ServerContext ctx_;

        // Ŭ���̾�Ʈ�κ��� ���� �����Դϴ�.
        Message request_;
        // Ŭ���̾�Ʈ���� �ٽ� ������ ����
        EchoMessage reply_;

        // Ŭ���̾�Ʈ�� ���ư��� ���� �����Դϴ�.
        ServerAsyncResponseWriter<EchoMessage> responder_;

        // ���� ���¸� ���� ���� ���� �ý����� ������ ���ڽ��ϴ�.
        enum class CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;  // ���� ���� �����Դϴ�.
    };
    void HandleRpcs() {
        // �� Ŭ���̾�Ʈ�� ���񽺸� ������ �� CallData �ν��Ͻ��� �����մϴ�.
        new CallData(&service_, cq_.get());
        void* tag;  // uniquely identifies a request.
        bool ok;
        while (true) {
            /*
            �Ϸ� ��⿭���� ���� �̺�Ʈ�� �б� ���� ����ϴ� ���� �����մϴ�. 
            �̺�Ʈ�� �±׷� �����ϰ� �ĺ��Ǹ�, �� ��� CallData �ν��Ͻ��� �޸� �ּ��Դϴ�. 
            Next(����)�� ��ȯ ���� �׻� �����ؾ� �մϴ�. 
            �� ��ȯ ���� �̺�Ʈ�� �ִ��� �Ǵ� cq_��(��) ����Ǵ��� ���θ� �˷��ݴϴ�. 
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