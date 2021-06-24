#include "QUIC_Stream.h"

#include <iostream>

QuicStream::QuicStream()
{
}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_STREAM_CALLBACK)
QUIC_STATUS QuicStream::StreamCallback(
	_In_ MsQuicStream* Stream,
	_In_opt_ void* Context,
	_Inout_ QUIC_STREAM_EVENT* Event)
{
	auto ctx = (QuicStream*)Context;

	switch (Event->Type) {
	case QUIC_STREAM_EVENT_START_COMPLETE:
		printf("[strm][%p] Start Complete\n", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_SEND_SHUTDOWN_COMPLETE:
		printf("[strm][%p] Send Shutdown Complete\n", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_SEND_COMPLETE:
		//
		// A previous StreamSend call has completed, and the context is being
		// returned back to the app.
		//
		free(Event->SEND_COMPLETE.ClientContext);
		printf("[strm][%p] Data sent\n", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_RECEIVE:
		//
		// Data was received from the peer on the stream.
		//
		printf("[strm][%p] --- Data received\n", Stream->Handle);
		printf("AbsoluteOffset %llu\n", Event->RECEIVE.AbsoluteOffset);
		printf("TotalBufferLength %llu\n", Event->RECEIVE.TotalBufferLength);
		printf("BufferCount %u\n", Event->RECEIVE.BufferCount);
		printf("Buffers->Length %u\n", Event->RECEIVE.Buffers->Length);
		printf("Flags %d\n", Event->RECEIVE.Flags);

		//for (uint32_t i = 0; i < Event->RECEIVE.Buffers->Length; i++) {
		//	printf("%.2X", (uint8_t)Event->RECEIVE.Buffers->Buffer[i]);
		//}
		//printf("\n");

		ctx->receiver_.queueBuffer(Event->RECEIVE.Buffers->Buffer, Event->RECEIVE.Buffers->Length);

		printf("[strm][%p] --- Data received\n", Stream->Handle);

		break;

	case QUIC_STREAM_EVENT_IDEAL_SEND_BUFFER_SIZE:
		printf("[strm][%p] Ideal Send Buffer Size(%llu)\n", Stream->Handle, Event->IDEAL_SEND_BUFFER_SIZE.ByteCount);

		break;
	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
		printf("[strm][%p] Peer shut down\n", Stream->Handle);
		// 정상적인 종료는 Shutdown을 보내줌
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL);
		ctx->receiver_.stopRecv = true;
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		printf("[strm][%p] Peer Send aborted, ", Stream->Handle);
		StatusPrint(Event->PEER_SEND_ABORTED.ErrorCode);
		Stream->Shutdown(Event->PEER_SEND_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_SEND);
		ctx->receiver_.stopRecv = true;
		//Stream->ConnectionShutdown(Event->PEER_SEND_ABORTED.ErrorCode);
		break;
	case QUIC_STREAM_EVENT_PEER_RECEIVE_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		printf("[strm][%p] Peer Receive aborted, ", Stream->Handle);
		StatusPrint(Event->PEER_RECEIVE_ABORTED.ErrorCode);
		Stream->Shutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_RECEIVE);
		ctx->receiver_.stopRecv = true;
		//Stream->ConnectionShutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode);
		break;
	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
		//
		// Both directions of the stream have been shut down and MsQuic is done
		// with the stream. It can now be safely cleaned up.
		//
		printf("[strm][%p] All done\n", Stream->Handle);

		ctx->receiver_.stopRecv = true;
		//Stream->ConnectionShutdown(0);
		break;
	default:
		printf("[strm][%p] Unknown Event %d\n", Stream->Handle, Event->Type);
		break;
	}

	return QUIC_STATUS_SUCCESS;
}

bool QuicStream::receiveData(DataPacket& data)
{
	DataPacket packet = { 0, };

	if (receiver_.getData(packet)) {
		data.data = std::move(packet.data);
		data.size = packet.size;
	}
	else
		return false;

	return true;
}

bool QuicStream::Send(const uint8_t* buf, uint32_t size)
{
	if (stream_->Handle) {
		QUIC_STATUS Status;

		auto packetSize = size + 4;

		auto SendBufferRaw = malloc((size_t)(sizeof(QUIC_BUFFER) + packetSize));
		if (SendBufferRaw == nullptr) {
			printf("SendBuffer allocation failed!\n");
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
		memset(SendBufferRaw, 0, sizeof(QUIC_BUFFER) + packetSize);

		auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
		SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
		SendBuffer->Length = packetSize;


		auto payload = (QUIC_BUFFER*)SendBuffer->Buffer;
		payload->Buffer = SendBuffer->Buffer + sizeof(uint32_t);
		payload->Length = size;

		memcpy(payload->Buffer, buf, size);

		if (QUIC_FAILED(Status = stream_->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
			printf("StreamSend failed, 0x%x!\n", Status);
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
	if (stream_->Handle) {
		QUIC_STATUS Status;

		auto packetSize = size + sizeof(DataPayload);

		auto SendBufferRaw = malloc((sizeof(QUIC_BUFFER) + packetSize));
		if (SendBufferRaw == nullptr) {
			printf("SendBuffer allocation failed!\n");
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
			printf("StreamSend failed, 0x%x!\n", Status);
			free(SendBufferRaw);
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
	}
	else
		return false;

	return true;
}

bool QuicStream::InitializeSend()
{

	if (stream_->Handle) {
		QUIC_STATUS Status;

		auto packetSize = 32;

		auto SendBufferRaw = malloc((sizeof(QUIC_BUFFER) + packetSize));
		if (SendBufferRaw == nullptr) {
			printf("SendBuffer allocation failed!\n");
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
		memset(SendBufferRaw, 0, sizeof(QUIC_BUFFER) + packetSize);

		auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
		SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
		SendBuffer->Length = packetSize;

		auto payload = (DataPayload*)SendBuffer->Buffer;
		payload->buf = (uint8_t*)SendBuffer->Buffer + sizeof(DataPayload);
		payload->size = packetSize - sizeof(DataPayload);

		if (QUIC_FAILED(Status = stream_->Send(SendBuffer, 1, QUIC_SEND_FLAG_START, SendBuffer))) {
			printf("StreamSend failed, 0x%x!\n", Status);
			free(SendBufferRaw);
			stream_->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
			return false;
		}
	}
	else
		return false;

	return true;

}

bool QuicStream::InitializeReceive()
{
	DataPacket packet = { 0, };

	if (receiver_.getData(packet)) {
		packet.data.reset();
	}
	else
		return false;
}

