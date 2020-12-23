#pragma once
#include <thread>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "echo.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

using simple::Message;
using simple::EchoMessage;
using simple::EchoService;

class EchoClient {
public:
    explicit EchoClient(std::shared_ptr<Channel> channel)
        : stub_(EchoService::NewStub(channel)) {}

    // 클라이언트의 페이로드를 조립하여 서버로 보냅니다
    void Echo(const std::string& text) {
        // 서버로 보내는 데이터.
        Message request;
        request.set_text(text);
        request.set_size(text.length());

        // rpc 데이터를 저장할 객체 호출
        AsyncClientCall* call = new AsyncClientCall;


        /*
        stub_->PrepareAsyncSayHello()가 RPC 개체를 생성하여
        "call"에 저장할 인스턴스를 반환하지만 실제로는 RPC를 시작하지 않습니다.
        비동기 API를 사용하고 있기 때문에 진행 중인
        RPC에 대한 업데이트를 받으려면 "call" 인스턴스를 유지해야 합니다.
        */
        call->response_reader =
            stub_->PrepareAsyncEcho(&call->context, request, &cq_);

        // StartCall이 RPC 호출을 시작합니다.
        call->response_reader->StartCall();

        /*
        RPC가 완료되면 "Response"이 서버의 응답으로 업데이트되도록 요청합니다.
        작업의 성공 여부를 나타내는 "Status" 호출 객체의 메모리 주소로 요청에 태그를 지정합니다. */
        call->response_reader->Finish(&call->reply, &call->status, (void*)call);

    }


    // 완료된 응답을 수신하는 동안 반복합니다.
    // 서버에서 응답을 출력합니다.
    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        // 완료 대기열 "cq"에서 다음 결과를 사용할 수있을 때까지 Block합니다.
        while (cq_.Next(&got_tag, &ok)) {
            //이 예제의 태그는 호출 객체의 메모리 위치입니다.
            AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);


            // 요청이 성공적으로 완료되었는지 확인합니다. 
            // "ok"는 Finish()에 의해 도입 된 업데이트 요청에만 해당합니다.

            GPR_ASSERT(ok);

            if (call->status.ok())
                std::cout << "recv : " << call->reply.echo()
                << '(' << call->reply.size() << ','
                << call->reply.ByteSizeLong() << ')' << std::endl;
            else
                std::cout << "RPC failed" << std::endl;

            // 완료되면 호출 객체를 할당 해제합니다.
            delete call;
        }
    }

private:
    // 상태 및 데이터 정보를 유지하기위한 구조체
    struct AsyncClientCall {
        // 서버에서 예상되는 데이터를 저장할 컨테이너입니다.
        EchoMessage reply;


        // 클라이언트에 대한 컨텍스트입니다. 
        // 추가 정보를 서버에 전달하거나 특정 RPC 동작을 조정하는 데 사용할 수 있습니다.
        ClientContext context;

        // 완료 시 RPC 상태에 대한 저장소입니다.
        Status status;

        std::unique_ptr<ClientAsyncResponseReader<EchoMessage>> response_reader;
    };

    // 전달된 채널에서 서버 노출 서비스에 대한 보기가 여기에 저장되어 있는 스텁이 나옵니다.
    std::unique_ptr<EchoService::Stub> stub_;

    // gRPC 런타임과 비동기적으로 통신하는 데 사용하는 생산자-소비자 대기열입니다.
    CompletionQueue cq_;
};