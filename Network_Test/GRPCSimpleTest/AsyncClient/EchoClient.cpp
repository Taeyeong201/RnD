#pragma once
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

class EchoClient
{
public:
    explicit EchoClient(std::shared_ptr<Channel> channel)
        : stub_(EchoService::NewStub(channel)) {}

    // 클라이언트의 페이로드를 조립하고 전송한 후 서버에서 응답을 다시 제공합니다.
    EchoMessage Echo(const std::string& text) {
        // 서버로 보내는 데이터.
        Message request;
        request.set_text(text);
        request.set_size(text.length());

        // 서버에서 예상되는 데이터를 저장할 컨테이너입니다.
        EchoMessage reply;

        // 클라이언트의 컨텍스트입니다. 추가 정보를 서버에 전달하거나 특정 RPC 동작을 수정하는 데 사용할 수 있습니다.
        ClientContext context;

        // gRPC 런타임과 비동기적으로 통신하는 데 사용하는 생산자-소비자 대기열입니다.
        CompletionQueue cq;

        // 완료 시 RPC 상태에 대한 저장소입니다.
        Status status;

        /* 
        stub_->PrepareAsyncSayHello()가 RPC 개체를 생성하여 
        "call"에 저장할 인스턴스를 반환하지만 실제로는 RPC를 시작하지 않습니다. 
        비동기 API를 사용하고 있기 때문에 진행 중인 
        RPC에 대한 업데이트를 받으려면 "call" 인스턴스를 유지해야 합니다. 
        */
        std::unique_ptr<ClientAsyncResponseReader<EchoMessage> > rpc(
            stub_->PrepareAsyncEcho(&context, request, &cq));

        // StartCall이 RPC 호출을 시작합니다.
        rpc->StartCall();

        /*
        RPC가 완료되면 서버의 응답에 대해 "reply"을 업데이트하도록 요청하고, 
        작업 성공 여부를 나타내는 "상태"를 요청합니다. 요청에 정수 1을 태그합니다. 
        */
        rpc->Finish(&reply, &status, (void*)1);
        void* got_tag;
        bool ok = false;
        /*
        다음 결과를 완료 대기열 "cq"에서 사용할 수 있을 때까지 차단합니다.
        Next(다음)의 반환 값은 항상 선택해야 합니다.
        이 반환 값은 이벤트가 있는지 또는 cq_이(가) 종료되는지 여부를 알려줍니다. 
        */
        GPR_ASSERT(cq.Next(&got_tag, &ok));

        // "cq"의 결과가 태그별로 이전 요청에 해당되는지 확인합니다.
        GPR_ASSERT(got_tag == (void*)1);
        /*
        ... 요청이 성공적으로 완료되었는지 확인합니다. 
        "확인"은 Finish()에서 도입한 업데이트 요청에만 해당합니다. 
        */
        GPR_ASSERT(ok);

        // 실제 RPC의 상태에 따라 작업합니다.
        if (status.ok()) {
            return reply;
        }
        else {
            return reply;
        }
    }
private:
    // 전달된 채널에서 서버 노출 서비스에 대한 보기가 여기에 저장되어 있는 스텁이 나옵니다.
	std::unique_ptr<EchoService::Stub> stub_;
};

