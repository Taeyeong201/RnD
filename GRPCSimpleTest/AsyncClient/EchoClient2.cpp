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

    // Ŭ���̾�Ʈ�� ���̷ε带 �����Ͽ� ������ �����ϴ�
    void Echo(const std::string& text) {
        // ������ ������ ������.
        Message request;
        request.set_text(text);
        request.set_size(text.length());

        // rpc �����͸� ������ ��ü ȣ��
        AsyncClientCall* call = new AsyncClientCall;


        /*
        stub_->PrepareAsyncSayHello()�� RPC ��ü�� �����Ͽ�
        "call"�� ������ �ν��Ͻ��� ��ȯ������ �����δ� RPC�� �������� �ʽ��ϴ�.
        �񵿱� API�� ����ϰ� �ֱ� ������ ���� ����
        RPC�� ���� ������Ʈ�� �������� "call" �ν��Ͻ��� �����ؾ� �մϴ�.
        */
        call->response_reader =
            stub_->PrepareAsyncEcho(&call->context, request, &cq_);

        // StartCall�� RPC ȣ���� �����մϴ�.
        call->response_reader->StartCall();

        /*
        RPC�� �Ϸ�Ǹ� "Response"�� ������ �������� ������Ʈ�ǵ��� ��û�մϴ�.
        �۾��� ���� ���θ� ��Ÿ���� "Status" ȣ�� ��ü�� �޸� �ּҷ� ��û�� �±׸� �����մϴ�. */
        call->response_reader->Finish(&call->reply, &call->status, (void*)call);

    }


    // �Ϸ�� ������ �����ϴ� ���� �ݺ��մϴ�.
    // �������� ������ ����մϴ�.
    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        // �Ϸ� ��⿭ "cq"���� ���� ����� ����� ������ ������ Block�մϴ�.
        while (cq_.Next(&got_tag, &ok)) {
            //�� ������ �±״� ȣ�� ��ü�� �޸� ��ġ�Դϴ�.
            AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);


            // ��û�� ���������� �Ϸ�Ǿ����� Ȯ���մϴ�. 
            // "ok"�� Finish()�� ���� ���� �� ������Ʈ ��û���� �ش��մϴ�.

            GPR_ASSERT(ok);

            if (call->status.ok())
                std::cout << "recv : " << call->reply.echo()
                << '(' << call->reply.size() << ','
                << call->reply.ByteSizeLong() << ')' << std::endl;
            else
                std::cout << "RPC failed" << std::endl;

            // �Ϸ�Ǹ� ȣ�� ��ü�� �Ҵ� �����մϴ�.
            delete call;
        }
    }

private:
    // ���� �� ������ ������ �����ϱ����� ����ü
    struct AsyncClientCall {
        // �������� ����Ǵ� �����͸� ������ �����̳��Դϴ�.
        EchoMessage reply;


        // Ŭ���̾�Ʈ�� ���� ���ؽ�Ʈ�Դϴ�. 
        // �߰� ������ ������ �����ϰų� Ư�� RPC ������ �����ϴ� �� ����� �� �ֽ��ϴ�.
        ClientContext context;

        // �Ϸ� �� RPC ���¿� ���� ������Դϴ�.
        Status status;

        std::unique_ptr<ClientAsyncResponseReader<EchoMessage>> response_reader;
    };

    // ���޵� ä�ο��� ���� ���� ���񽺿� ���� ���Ⱑ ���⿡ ����Ǿ� �ִ� ������ ���ɴϴ�.
    std::unique_ptr<EchoService::Stub> stub_;

    // gRPC ��Ÿ�Ӱ� �񵿱������� ����ϴ� �� ����ϴ� ������-�Һ��� ��⿭�Դϴ�.
    CompletionQueue cq_;
};