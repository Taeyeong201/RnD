// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: echo.proto

#include "echo.pb.h"
#include "echo.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace simple {

static const char* EchoService_method_names[] = {
  "/simple.EchoService/Echo",
};

std::unique_ptr< EchoService::Stub> EchoService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< EchoService::Stub> stub(new EchoService::Stub(channel));
  return stub;
}

EchoService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Echo_(EchoService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status EchoService::Stub::Echo(::grpc::ClientContext* context, const ::simple::Message& request, ::simple::EchoMessage* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Echo_, context, request, response);
}

void EchoService::Stub::experimental_async::Echo(::grpc::ClientContext* context, const ::simple::Message* request, ::simple::EchoMessage* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Echo_, context, request, response, std::move(f));
}

void EchoService::Stub::experimental_async::Echo(::grpc::ClientContext* context, const ::simple::Message* request, ::simple::EchoMessage* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Echo_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::simple::EchoMessage>* EchoService::Stub::PrepareAsyncEchoRaw(::grpc::ClientContext* context, const ::simple::Message& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::simple::EchoMessage>::Create(channel_.get(), cq, rpcmethod_Echo_, context, request, false);
}

::grpc::ClientAsyncResponseReader< ::simple::EchoMessage>* EchoService::Stub::AsyncEchoRaw(::grpc::ClientContext* context, const ::simple::Message& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncEchoRaw(context, request, cq);
  result->StartCall();
  return result;
}

EchoService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      EchoService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< EchoService::Service, ::simple::Message, ::simple::EchoMessage>(
          [](EchoService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::simple::Message* req,
             ::simple::EchoMessage* resp) {
               return service->Echo(ctx, req, resp);
             }, this)));
}

EchoService::Service::~Service() {
}

::grpc::Status EchoService::Service::Echo(::grpc::ServerContext* context, const ::simple::Message* request, ::simple::EchoMessage* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace simple

