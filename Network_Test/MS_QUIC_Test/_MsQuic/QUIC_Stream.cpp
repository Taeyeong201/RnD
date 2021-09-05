#pragma warning(disable: 28182)

#include "QUIC_Stream.h"

#include "QUIC_StreamManager.h"

#include <iostream>

QuicStream::QuicStream() noexcept
	: receiver_()
{
}
QuicStream::QuicStream(std::string id) noexcept
	: receiver_(), id_(id)
{
}

//QuicStream::QuicStream(const QuicStream& other) noexcept
//	: receiver_(other.receiver_)
//{
//	stream_ = other.stream_;
//}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_STREAM_CALLBACK)
QUIC_STATUS QuicStream::StreamCallback(
	_In_ MsQuicStream* Stream,
	_In_opt_ void* Context,
	_Inout_ QUIC_STREAM_EVENT* Event)
{
	auto ctx = (QuicStream*)Context;
	static uint64_t nowCall = 0, lastCall = 0;

	switch (Event->Type) {
	case QUIC_STREAM_EVENT_START_COMPLETE:
		PLOG_INFO.printf("[strm][%p] Start Complete", Stream->Handle);
		ctx->InitializeSend(ctx->id_.c_str());
		break;
	case QUIC_STREAM_EVENT_SEND_SHUTDOWN_COMPLETE:
		PLOG_INFO.printf("[strm][%p] Send Shutdown Complete", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_SEND_COMPLETE:
		//
		// A previous StreamSend call has completed, and the context is being
		// returned back to the app.
		//
		//PLOG_INFO.printf("[strm][%p] Data sent", Stream->Handle);
		free(Event->SEND_COMPLETE.ClientContext);
		//printf("[strm][%p] Data sent\n", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_RECEIVE:
		//
		// Data was received from the peer on the stream.
		//
		//PLOG_INFO.printf("[strm][%p] --- Data received", Stream->Handle);
		//printf("AbsoluteOffset %llu\n", Event->RECEIVE.AbsoluteOffset);
		//printf("TotalBufferLength %llu\n", Event->RECEIVE.TotalBufferLength);
		//	printf("BufferCount %u\n", Event->RECEIVE.BufferCount);
		//printf("Buffers->Length %u\n", Event->RECEIVE.Buffers->Length);
		//printf("Flags %d\n", Event->RECEIVE.Flags);

		//for (uint32_t i = 0; i < Event->RECEIVE.Buffers->Length; i++) {
		//	printf("%.2X", (uint8_t)Event->RECEIVE.Buffers->Buffer[i]);
		//}
		//printf("\n");
		//PLOG_INFO.printf("[strm][%p] QUIC_STREAM_EVENT_RECEIVE", Stream->Handle);

		for (uint32_t i = 0; i < Event->RECEIVE.BufferCount; i++)
			ctx->receiver_.queueBuffer(Event->RECEIVE.Buffers[i].Buffer, Event->RECEIVE.Buffers[i].Length);

		//printf("[strm][%p] --- Data received\n", Stream->Handle);

		break;

	case QUIC_STREAM_EVENT_IDEAL_SEND_BUFFER_SIZE:
		//printf("[strm][%p] Ideal Send Buffer Size(%llu)\n", Stream->Handle, Event->IDEAL_SEND_BUFFER_SIZE.ByteCount);

		break;
	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
		PLOG_INFO.printf("[strm][%p] Peer shut down", Stream->Handle);
		// 정상적인 종료는 Shutdown을 보내줌
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL);
		ctx->brokenStream = true;
		ctx->receiver_.shutdownGetDataFunc();
		ctx->receiver_.stopRecv = true;
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		PLOG_INFO.printf("[strm][%p] Peer Send aborted[0x%x], %s",
			Stream->Handle, Event->PEER_SEND_ABORTED.ErrorCode,
			StatusPrint(Event->PEER_SEND_ABORTED.ErrorCode)
		);
		Stream->Shutdown(Event->PEER_SEND_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_SEND);
		ctx->brokenStream = true;
		ctx->receiver_.shutdownGetDataFunc();
		ctx->receiver_.stopRecv = true;
		//Stream->ConnectionShutdown(Event->PEER_SEND_ABORTED.ErrorCode);
		break;
	case QUIC_STREAM_EVENT_PEER_RECEIVE_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		PLOG_INFO.printf("[strm][%p] Peer Receive aborted[0x%x], %s",
			Stream->Handle, Event->PEER_SEND_ABORTED.ErrorCode,
			StatusPrint(Event->PEER_RECEIVE_ABORTED.ErrorCode)
		);

		Stream->Shutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_RECEIVE);
		ctx->brokenStream = true;
		ctx->receiver_.shutdownGetDataFunc();
		ctx->receiver_.stopRecv = true;
		//Stream->ConnectionShutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode);
		break;
	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
		//
		// Both directions of the stream have been shut down and MsQuic is done
		// with the stream. It can now be safely cleaned up.
		//
		PLOG_INFO.printf("[strm][%p] All done", Stream->Handle);

		ctx->brokenStream = true;
		ctx->receiver_.stopRecv = true;
		ctx->receiver_.shutdownGetDataFunc();
		ctx->deleteCallback(ctx->id_.c_str());
		//Stream->ConnectionShutdown(0);
		break;
	default:
		PLOG_INFO.printf("[strm][%p] Unknown Event %d", Stream->Handle, Event->Type);
		break;
	}

	return QUIC_STATUS_SUCCESS;
}

bool QuicStream::receiveData(DataPacket& data)
{
	if (!brokenStream) {
		if (receiver_.getData(data)) return true;
	}
	return false;
}

bool QuicStream::Send(const uint8_t* buf, uint32_t size)
{
	if (stream_ != nullptr) {
		QUIC_STATUS Status;

		auto packetSize = size + sizeof(DataPayload);

		auto SendBufferRaw = malloc((size_t)(sizeof(QUIC_BUFFER) + packetSize));
		if (SendBufferRaw == nullptr) {
			PLOG_ERROR.printf("SendBuffer allocation failed!\n");
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
		memset(SendBufferRaw, 0, sizeof(QUIC_BUFFER) + packetSize);

		auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
		SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
		SendBuffer->Length = packetSize;


		auto payload = (DataPayload*)SendBuffer->Buffer;
		payload->buf = (uint8_t*)SendBuffer->Buffer + sizeof(DataPayload);
		payload->size = size;

		memcpy(payload->buf, buf, size);

		if (QUIC_FAILED(Status = stream_->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBufferRaw))) {
			PLOG_ERROR.printf("StreamSend failed, [0x%x], %s",
				Status, StatusPrint(Status)
			);
			free(SendBufferRaw);
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
	}
	else
		return false;

	return true;
}

bool QuicStream::Send(const char* buf, uint32_t size)
{
	if (stream_ != nullptr) {
		QUIC_STATUS Status;

		auto packetSize = size + sizeof(DataPayload);

		auto SendBufferRaw = malloc((sizeof(QUIC_BUFFER) + packetSize));
		if (SendBufferRaw == nullptr) {
			PLOG_ERROR.printf("SendBuffer allocation failed!\n");
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
		memset(SendBufferRaw, 0, sizeof(QUIC_BUFFER) + packetSize);

		auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
		SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
		SendBuffer->Length = packetSize;

		auto payload = (DataPayload*)SendBuffer->Buffer;
		payload->buf = (uint8_t*)SendBuffer->Buffer + sizeof(DataPayload);
		payload->size = size;

		memcpy(payload->buf, buf, size);

		if (QUIC_FAILED(Status = stream_->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBufferRaw))) {
			PLOG_ERROR.printf("StreamSend failed, [0x%x], %s",
				Status, StatusPrint(Status)
			);
			free(SendBufferRaw);
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
	}
	else
		return false;

	return true;
}

void QuicStream::RegistDeleteCallback(std::function<void(const char*)> callback)
{
	deleteCallback = callback;
}

bool QuicStream::StopStream()
{
	QUIC_STATUS Status;
	brokenStream = true;
	if (stream_->Handle) {
		if (QUIC_FAILED(Status = stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT))) {
			PLOG_ERROR.printf("Stream Shutdown failed, [0x%x], %s",
				Status, StatusPrint(Status)
			);
			return false;
		}
	}

	return true;
}

bool QuicStream::InitializeSend(const char* id)
{
	if (stream_->Handle) {
		QUIC_STATUS Status;

		auto packetSize = (strlen(id) + 1) + sizeof(DataPayload);

		auto SendBufferRaw = malloc((sizeof(QUIC_BUFFER) + packetSize));
		if (SendBufferRaw == nullptr) {
			PLOG_ERROR << "SendBuffer allocation failed!";
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
		memset(SendBufferRaw, 0, sizeof(QUIC_BUFFER) + packetSize);

		auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
		SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
		SendBuffer->Length = packetSize;

		auto payload = (DataPayload*)SendBuffer->Buffer;
		payload->buf = (uint8_t*)SendBuffer->Buffer + sizeof(DataPayload);
		payload->size = (strlen(id) + 1);

		memcpy(payload->buf, id, strlen(id) + 1);

		if (QUIC_FAILED(Status = stream_->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBufferRaw))) {
			PLOG_ERROR.printf("StreamSend failed, [0x%x], %s",
				Status, StatusPrint(Status)
			);
			free(SendBufferRaw);
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
	}
	else {
		PLOG_ERROR.printf("Init StreamSend failed, [0x%x], %s",
			stream_->GetInitStatus(), StatusPrint(stream_->GetInitStatus())
		);
		return false;
	}

	return true;
}

bool QuicStream::isBroken()
{
	return brokenStream;
}