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

    // Ŭ���̾�Ʈ�� ���̷ε带 �����ϰ� ������ �� �������� ������ �ٽ� �����մϴ�.
    EchoMessage Echo(const std::string& text) {
        // ������ ������ ������.
        Message request;
        request.set_text(text);
        request.set_size(text.length());

        // �������� ����Ǵ� �����͸� ������ �����̳��Դϴ�.
        EchoMessage reply;

        // Ŭ���̾�Ʈ�� ���ؽ�Ʈ�Դϴ�. �߰� ������ ������ �����ϰų� Ư�� RPC ������ �����ϴ� �� ����� �� �ֽ��ϴ�.
        ClientContext context;

        // gRPC ��Ÿ�Ӱ� �񵿱������� ����ϴ� �� ����ϴ� ������-�Һ��� ��⿭�Դϴ�.
        CompletionQueue cq;

        // �Ϸ� �� RPC ���¿� ���� ������Դϴ�.
        Status status;

        /* 
        stub_->PrepareAsyncSayHello()�� RPC ��ü�� �����Ͽ� 
        "call"�� ������ �ν��Ͻ��� ��ȯ������ �����δ� RPC�� �������� �ʽ��ϴ�. 
        �񵿱� API�� ����ϰ� �ֱ� ������ ���� ���� 
        RPC�� ���� ������Ʈ�� �������� "call" �ν��Ͻ��� �����ؾ� �մϴ�. 
        */
        std::unique_ptr<ClientAsyncResponseReader<EchoMessage> > rpc(
            stub_->PrepareAsyncEcho(&context, request, &cq));

        // StartCall�� RPC ȣ���� �����մϴ�.
        rpc->StartCall();

        /*
        RPC�� �Ϸ�Ǹ� ������ ���信 ���� "reply"�� ������Ʈ�ϵ��� ��û�ϰ�, 
        �۾� ���� ���θ� ��Ÿ���� "����"�� ��û�մϴ�. ��û�� ���� 1�� �±��մϴ�. 
        */
        rpc->Finish(&reply, &status, (void*)1);
        void* got_tag;
        bool ok = false;
        /*
        ���� ����� �Ϸ� ��⿭ "cq"���� ����� �� ���� ������ �����մϴ�.
        Next(����)�� ��ȯ ���� �׻� �����ؾ� �մϴ�.
        �� ��ȯ ���� �̺�Ʈ�� �ִ��� �Ǵ� cq_��(��) ����Ǵ��� ���θ� �˷��ݴϴ�. 
        */
        GPR_ASSERT(cq.Next(&got_tag, &ok));

        // "cq"�� ����� �±׺��� ���� ��û�� �ش�Ǵ��� Ȯ���մϴ�.
        GPR_ASSERT(got_tag == (void*)1);
        /*
        ... ��û�� ���������� �Ϸ�Ǿ����� Ȯ���մϴ�. 
        "Ȯ��"�� Finish()���� ������ ������Ʈ ��û���� �ش��մϴ�. 
        */
        GPR_ASSERT(ok);

        // ���� RPC�� ���¿� ���� �۾��մϴ�.
        if (status.ok()) {
            return reply;
        }
        else {
            return reply;
        }
    }
private:
    // ���޵� ä�ο��� ���� ���� ���񽺿� ���� ���Ⱑ ���⿡ ����Ǿ� �ִ� ������ ���ɴϴ�.
	std::unique_ptr<EchoService::Stub> stub_;
};

