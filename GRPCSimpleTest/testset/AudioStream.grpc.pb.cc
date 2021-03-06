// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: AudioStream.proto

#include "AudioStream.pb.h"
#include "AudioStream.grpc.pb.h"

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
namespace LiveStream {

static const char* AudioStream_method_names[] = {
  "/LiveStream.AudioStream/AudioStreamReqeust",
};

std::unique_ptr< AudioStream::Stub> AudioStream::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< AudioStream::Stub> stub(new AudioStream::Stub(channel));
  return stub;
}

AudioStream::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_AudioStreamReqeust_(AudioStream_method_names[0], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  {}

::grpc::ClientReader< ::LiveStream::DataStream>* AudioStream::Stub::AudioStreamReqeustRaw(::grpc::ClientContext* context, const ::LiveStream::AudioFormat& request) {
  return ::grpc::internal::ClientReaderFactory< ::LiveStream::DataStream>::Create(channel_.get(), rpcmethod_AudioStreamReqeust_, context, request);
}

void AudioStream::Stub::experimental_async::AudioStreamReqeust(::grpc::ClientContext* context, ::LiveStream::AudioFormat* request, ::grpc::experimental::ClientReadReactor< ::LiveStream::DataStream>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::LiveStream::DataStream>::Create(stub_->channel_.get(), stub_->rpcmethod_AudioStreamReqeust_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::LiveStream::DataStream>* AudioStream::Stub::AsyncAudioStreamReqeustRaw(::grpc::ClientContext* context, const ::LiveStream::AudioFormat& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::LiveStream::DataStream>::Create(channel_.get(), cq, rpcmethod_AudioStreamReqeust_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::LiveStream::DataStream>* AudioStream::Stub::PrepareAsyncAudioStreamReqeustRaw(::grpc::ClientContext* context, const ::LiveStream::AudioFormat& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::LiveStream::DataStream>::Create(channel_.get(), cq, rpcmethod_AudioStreamReqeust_, context, request, false, nullptr);
}

AudioStream::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AudioStream_method_names[0],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< AudioStream::Service, ::LiveStream::AudioFormat, ::LiveStream::DataStream>(
          [](AudioStream::Service* service,
             ::grpc::ServerContext* ctx,
             const ::LiveStream::AudioFormat* req,
             ::grpc::ServerWriter<::LiveStream::DataStream>* writer) {
               return service->AudioStreamReqeust(ctx, req, writer);
             }, this)));
}

AudioStream::Service::~Service() {
}

::grpc::Status AudioStream::Service::AudioStreamReqeust(::grpc::ServerContext* context, const ::LiveStream::AudioFormat* request, ::grpc::ServerWriter< ::LiveStream::DataStream>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace LiveStream

