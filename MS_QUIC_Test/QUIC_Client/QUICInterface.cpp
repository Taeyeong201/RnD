#include "QUICInterface.h"

#include <iostream>

const MsQuicApi* MsQuic;

QUICInterface::QUICInterface()
{
}

QUICInterface::~QUICInterface()
{
}

void QUICInterface::StatusPrint(QUIC_STATUS status)
{
	switch (status) {
	case QUIC_STATUS_SUCCESS:
		std::cout << "The operation completed successfully." << std::endl;
		break;
	case QUIC_STATUS_PENDING:
		std::cout << "The operation is pending." << std::endl;
		break;
	case QUIC_STATUS_CONTINUE:
		std::cout << "The operation will continue." << std::endl;
		break;
	case QUIC_STATUS_OUT_OF_MEMORY:
		std::cout << "Allocation of memory failed." << std::endl;
		break;
	case QUIC_STATUS_INVALID_PARAMETER:
		std::cout << "An invalid parameter was encountered." << std::endl;
		break;
	case QUIC_STATUS_INVALID_STATE:
		std::cout << "The current state was not valid for this operation." << std::endl;
		break;
	case QUIC_STATUS_NOT_SUPPORTED:
		std::cout << "The operation was not supported." << std::endl;
		break;
	case QUIC_STATUS_NOT_FOUND:
		std::cout << "The object was not found." << std::endl;
		break;
	case QUIC_STATUS_BUFFER_TOO_SMALL:
		std::cout << "The buffer was too small for the operation." << std::endl;
		break;
	case QUIC_STATUS_HANDSHAKE_FAILURE:
		std::cout << "The connection handshake failed." << std::endl;
		break;
	case QUIC_STATUS_ABORTED:
		std::cout << "The connection or stream was aborted." << std::endl;
		break;
	case QUIC_STATUS_ADDRESS_IN_USE:
		std::cout << "The local address is already in use." << std::endl;
		break;
	case QUIC_STATUS_CONNECTION_TIMEOUT:
		std::cout << "The connection timed out waiting for a response from the peer." << std::endl;
		break;
	case QUIC_STATUS_CONNECTION_IDLE:
		std::cout << "The connection timed out from inactivity." << std::endl;
		break;
	case QUIC_STATUS_INTERNAL_ERROR:
		std::cout << "An internal error was encountered." << std::endl;
		break;
	case QUIC_STATUS_UNREACHABLE:
		std::cout << "The server is currently unreachable." << std::endl;
		break;
	case QUIC_STATUS_CONNECTION_REFUSED:
		std::cout << "The server refused the connection." << std::endl;
		break;
	case QUIC_STATUS_PROTOCOL_ERROR:
		std::cout << "A protocol error was encountered." << std::endl;
		break;
	case QUIC_STATUS_VER_NEG_ERROR:
		std::cout << "A version negotiation error was encountered." << std::endl;
		break;
	case QUIC_STATUS_USER_CANCELED:
		std::cout << "The peer app/user canceled the connection during the handshake." << std::endl;
		break;
	case QUIC_STATUS_ALPN_NEG_FAILURE:
		std::cout << "The connection handshake failed to negotiate a common ALPN." << std::endl;
		break;
	case QUIC_STATUS_STREAM_LIMIT_REACHED:
		std::cout << "A stream failed to start because the peer doesn't allow any more to be open at this time." << std::endl;
		break;
	default:
		printf("unknown : 0x%x\n", status);
		break;
	}
}


int QUICInterface::MsQuicOpen()
{
	QUIC_STATUS Status;

	MsQuic = new (std::nothrow) MsQuicApi();
	if (MsQuic == nullptr) {
		std::cout << "MsQuic alloc out of memory" << std::endl;
		return -1;
	}
	if (QUIC_FAILED(Status = MsQuic->GetInitStatus())) {
		printf("MsQuic Failed To Initialize: 0x%x\n", Status);
		return -1;
	}
	return 0;
}

int QUICInterface::MsQuicClose()
{
	if (MsQuic != nullptr) {
		delete MsQuic;
		MsQuic = nullptr;
	}
	return 0;
}

void
ServerSend(
	_In_ MsQuicStream* Stream
)
{
	QUIC_STATUS Status;


	//
	// Allocates and builds the buffer to send over the stream.
	//
	auto SendBufferRaw = malloc(sizeof(QUIC_BUFFER) + 3000);
	if (SendBufferRaw == nullptr) {
		printf("SendBuffer allocation failed!\n");
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
		return;
	}

	auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
	SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
	SendBuffer->Length = 3000;

	printf("[strm][%p] Sending data...\n", Stream);
	memset(SendBuffer->Buffer, 0, 3000);
	memcpy(SendBuffer->Buffer,
		"DataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataData",
		strlen("DataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataData"));

	//
	// Sends the buffer over the stream. Note the FIN flag is passed along with
	// the buffer. This indicates this is the last buffer on the stream and the
	// the stream is shut down (in the send direction) immediately after.
	//
	if (QUIC_FAILED(Status = Stream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
		printf("StreamSend failed, 0x%x!\n", Status);
		free(SendBufferRaw);
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	}
}

void ClientSend(_In_ MsQuicStream* Stream) {
	QUIC_STATUS Status;


	//
	// Allocates and builds the buffer to send over the stream.
	//
	auto SendBufferRawTest = malloc(sizeof(QUIC_BUFFER) + 12);
	if (SendBufferRawTest == nullptr) {
		printf("SendBuffer allocation failed!\n");
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
		return;
	}

	auto SendBufferTest = (QUIC_BUFFER*)SendBufferRawTest;
	SendBufferTest->Buffer = (uint8_t*)SendBufferRawTest + sizeof(QUIC_BUFFER);
	SendBufferTest->Length = 12;

	memset(SendBufferTest->Buffer, 0, 12);

	auto testbuf = (QUIC_BUFFER*)SendBufferTest->Buffer;
	testbuf->Buffer = SendBufferTest->Buffer + sizeof(uint32_t);
	testbuf->Length = 8;
	memcpy(testbuf->Buffer, "11111111", strlen("11111111"));

	auto SendBufferRaw2 = malloc(sizeof(QUIC_BUFFER) + 10000);
	if (SendBufferRaw2 == nullptr) {
		printf("SendBuffer allocation failed!\n");
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
		return;
	}

	auto SendBuffer2 = (QUIC_BUFFER*)SendBufferRaw2;
	SendBuffer2->Buffer = (uint8_t*)SendBufferRaw2 + sizeof(QUIC_BUFFER);
	SendBuffer2->Length = 10000;

	memset(SendBuffer2->Buffer, 0, 10000);

	auto testbuf3 = (QUIC_BUFFER*)SendBuffer2->Buffer;
	testbuf3->Buffer = SendBuffer2->Buffer + sizeof(uint32_t);
	testbuf3->Length = 9996;

	memcpy(testbuf3->Buffer,
		"DataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataData",
		strlen("DataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataData"));

	auto SendBufferRaw = malloc(sizeof(QUIC_BUFFER) + 100);
	if (SendBufferRaw == nullptr) {
		printf("SendBuffer allocation failed!\n");
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
		return;
	}

	auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
	SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
	SendBuffer->Length = 100;

	memset(SendBuffer->Buffer, 0, 100);

	auto testbuf2 = (QUIC_BUFFER*)SendBuffer->Buffer;
	testbuf2->Buffer = SendBuffer->Buffer + sizeof(uint32_t);
	testbuf2->Length = 96;

	memcpy(testbuf2->Buffer,
		"DataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataData",
		strlen("DataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataDataData"));

	printf("[strm][%p] Sending data...\n", Stream);

	if (QUIC_FAILED(Status = Stream->Send(SendBufferTest, 1, QUIC_SEND_FLAG_NONE))) {
		printf("StreamSend failed, 0x%x!\n", Status);
		free(SendBufferRawTest);
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	}

	if (QUIC_FAILED(Status = Stream->Send(SendBuffer2, 1, QUIC_SEND_FLAG_NONE))) {
		printf("StreamSend failed, 0x%x!\n", Status);
		free(SendBufferRaw2);
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	}


	//
	// Sends the buffer over the stream. Note the FIN flag is passed along with
	// the buffer. This indicates this is the last buffer on the stream and the
	// the stream is shut down (in the send direction) immediately after.
	//
	if (QUIC_FAILED(Status = Stream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
		printf("StreamSend failed, 0x%x!\n", Status);
		free(SendBufferRaw);
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	}
}

void ClientInitSend(_In_ MsQuicStream* Stream) {
	QUIC_STATUS Status;


	auto packetSize = 24;

	auto SendBufferRaw = malloc((sizeof(QUIC_BUFFER) + packetSize));
	if (SendBufferRaw == nullptr) {
		printf("SendBuffer allocation failed!\n");
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	}
	memset(SendBufferRaw, 0, sizeof(QUIC_BUFFER) + packetSize);

	auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
	SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
	SendBuffer->Length = packetSize;

	if (QUIC_FAILED(Status = Stream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
		printf("StreamSend failed, 0x%x!\n", Status);
		free(SendBufferRaw);
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	}
}


_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_STREAM_CALLBACK)
QUIC_STATUS QUICInterface::StreamCallback(
	_In_ MsQuicStream* Stream,
	_In_opt_ void* Context,
	_Inout_ QUIC_STREAM_EVENT* Event)
{
	auto ctx = (QUICInterface*)Context;
	//if (Event->Type == QUIC_STREAM_EVENT_RECEIVE) {
	//    //ctx->ServerStreamRecv.Set();
	//    return QUIC_STATUS_PENDING;
	//}
	//else if (Event->Type == QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE) {
	//    //ctx->ServerStreamHasShutdown = true;
	//    //ctx->ServerStreamShutdown.Set();
	//    Stream->ConnectionShutdown(1);
	//}

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
		printf("[strm][%p] --- Data received\n", Stream->Handle);

		break;

	case QUIC_STREAM_EVENT_IDEAL_SEND_BUFFER_SIZE:
		printf("[strm][%p] Ideal Send Buffer Size(%d)\n", Stream->Handle, Event->IDEAL_SEND_BUFFER_SIZE.ByteCount);

		break;
	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
	{
		printf("[strm][%p] Peer shut down\n", Stream->Handle);
		// 정상적인 종료는 Shutdown을 보내줌
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL);

		break;
	}
	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		printf("[strm][%p] Peer Send aborted, ", Stream->Handle);
		StatusPrint(Event->PEER_SEND_ABORTED.ErrorCode);
		Stream->Shutdown(Event->PEER_SEND_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_SEND);
		//Stream->ConnectionShutdown(Event->PEER_SEND_ABORTED.ErrorCode);
		break;
	case QUIC_STREAM_EVENT_PEER_RECEIVE_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		printf("[strm][%p] Peer Receive aborted, ", Stream->Handle);
		StatusPrint(Event->PEER_RECEIVE_ABORTED.ErrorCode);
		Stream->Shutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_RECEIVE);
		//Stream->ConnectionShutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode);
		break;
	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
		//
		// Both directions of the stream have been shut down and MsQuic is done
		// with the stream. It can now be safely cleaned up.
		//
		printf("[strm][%p] All done\n", Stream->Handle);
		//Stream->ConnectionShutdown(0);
		break;
	default:
		printf("[strm][%p] Unknown Event %d\n", Stream->Handle, Event->Type);
		break;
	}

	return QUIC_STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_CONNECTION_CALLBACK)
QUIC_STATUS QUICInterface::ConnCallback(
	_In_ MsQuicConnection* Connection,
	_In_opt_ void* Context,
	_Inout_ QUIC_CONNECTION_EVENT* Event)
{
	auto ctx = (QUICInterface*)Context;
	switch (Event->Type) {
	case QUIC_CONNECTION_EVENT_CONNECTED:
		//
		// The handshake has completed for the connection.
		//
		printf("[conn][%p] Connected\n", Connection->Handle);
		//Connection->SendResumptionTicket(QUIC_SEND_RESUMPTION_FLAG_NONE, 0, NULL);
		//ctx->ServerStream =
		//	new(std::nothrow) MsQuicStream(*Connection, QUIC_STREAM_OPEN_FLAG_NONE, CleanUpAutoDelete, StreamCallback, Context);
		//ctx->ServerStream->Start(QUIC_STREAM_START_FLAG_NONE);
		//ServerSend(ctx->ServerStream);

		//ctx->ServerStream =
		//	new(std::nothrow) MsQuicStream(*Connection, QUIC_STREAM_OPEN_FLAG_NONE, CleanUpAutoDelete, StreamCallback, Context);
		//ctx->ServerStream->Start(QUIC_STREAM_START_FLAG_NONE);
		//ServerSend(ctx->ServerStream);

		//ctx->ServerStream =
		//	new(std::nothrow) MsQuicStream(*Connection, QUIC_STREAM_OPEN_FLAG_NONE, CleanUpAutoDelete, StreamCallback, Context);
		//ctx->ServerStream->Start(QUIC_STREAM_START_FLAG_NONE);
		//ServerSend(ctx->ServerStream);

		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		//
		// The connection has been shut down by the transport. Generally, this
		// is the expected way for the connection to shut down with this
		// protocol, since we let idle timeout kill the connection.
		//
		if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
			printf("[conn][%p] Successfully shut down on idle.\n", Connection->Handle);
		}
		else {
			printf("[conn][%p] Shut down by transport, ", Connection->Handle);
			StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		//
		// The connection was explicitly shut down by the peer.
		//
		printf("[conn][%p] Shut down by peer ", Connection->Handle);
		StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		//
		// The connection has completed the shutdown process and is ready to be
		// safely cleaned up.
		//
		printf("[conn][%p] All done\n", Connection->Handle);
		ctx->ServerStream = nullptr;
		break;
	case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
		//
		// The peer has started/created a new stream. The app MUST set the
		// callback handler before returning.
		//
		printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
		ctx->ServerStream =
			new(std::nothrow) MsQuicStream(Event->PEER_STREAM_STARTED.Stream, CleanUpAutoDelete, StreamCallback, Context);
		ServerSend(ctx->ServerStream);
		break;
	case QUIC_CONNECTION_EVENT_RESUMED:
		//
		// The connection succeeded in doing a TLS resumption of a previous
		// connection's session.
		//
		printf("[conn][%p] Connection resumed!\n", Connection->Handle);
		break;
	case QUIC_CONNECTION_EVENT_IDEAL_PROCESSOR_CHANGED:
		printf("[conn][%p] ideal processor chaged\n", Connection->Handle);

		break;
	default:
		printf("[conn][%p] Unknown Event %d\n", Connection->Handle, Event->Type);
		break;
	}
	return QUIC_STATUS_SUCCESS;
}
_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_STREAM_CALLBACK)
QUIC_STATUS QUICInterface::ClientStreamCallback(
	_In_ MsQuicStream* Stream,
	_In_opt_ void* Context,
	_Inout_ QUIC_STREAM_EVENT* Event)
{
	auto ctx = (QUICInterface*)Context;

	switch (Event->Type) {
	case QUIC_STREAM_EVENT_START_COMPLETE:
		printf("[strm][%p] Start Complete\n", Stream->Handle);

		ctx->onStream++;
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
		printf("[strm][%p] --- Data received\n", Stream->Handle);

		break;
	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
		//
		// The peer gracefully shut down its send direction of the stream.
		//
		printf("[strm][%p] Peer aborted\n", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
		//
		// The peer aborted its send direction of the stream.
		//
		printf("[strm][%p] Peer shut down\n", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_SEND_SHUTDOWN_COMPLETE:
		printf("[strm][%p] Send Shutdown Complete\n", Stream->Handle);
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL);
		break;
	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
		//
		// Both directions of the stream have been shut down and MsQuic is done
		// with the stream. It can now be safely cleaned up.
		//
		printf("[strm][%p] All done\n", Stream->Handle);
		ctx->onStream--;
		break;

	default:
		printf("[strm][%p] Unknown Event %d\n", Stream->Handle, Event->Type);

		break;
	}
	return QUIC_STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_CONNECTION_CALLBACK)
QUIC_STATUS QUICInterface::ClientConnCallback(
	_In_ MsQuicConnection* Connection,
	_In_opt_ void* Context,
	_Inout_ QUIC_CONNECTION_EVENT* Event)
{

	auto ctx = (QUICInterface*)Context;

	switch (Event->Type) {
	case QUIC_CONNECTION_EVENT_CONNECTED:
		//
		// The handshake has completed for the connection.
		//
		printf("[conn][%p] Connected\n", Connection->Handle);
		ctx->ServerStream =
			new(std::nothrow) MsQuicStream(*Connection, QUIC_STREAM_OPEN_FLAG_NONE, CleanUpAutoDelete, ClientStreamCallback, Context);
		ctx->ServerStream->Start(QUIC_STREAM_START_FLAG_NONE);
		ClientInitSend(ctx->ServerStream);
		//ClientSend(ctx->ServerStream);
		break;
	case QUIC_CONNECTION_EVENT_STREAMS_AVAILABLE:
		printf("[conn][%p] Streams Available\n", Connection->Handle);
		break;
	case QUIC_CONNECTION_EVENT_DATAGRAM_STATE_CHANGED:
		printf("[conn][%p] Datagram State Changed\n", Connection->Handle);
		break;
	case QUIC_CONNECTION_EVENT_IDEAL_PROCESSOR_CHANGED:
		printf("[conn][%p] Ideal Processor Changed\n", Connection->Handle);
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		//
		// The connection has been shut down by the transport. Generally, this
		// is the expected way for the connection to shut down with this
		// protocol, since we let idle timeout kill the connection.
		//
		if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
			printf("[conn][%p] Successfully shut down on idle.\n", Connection->Handle);
		}
		else {
			printf("[conn][%p] Shut down by transport, ", Connection->Handle);
			StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		//
		// The connection was explicitly shut down by the peer.
		//
		printf("[conn][%p] Shut down by peer, ", Connection->Handle );
		StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		//
		// The connection has completed the shutdown process and is ready to be
		// safely cleaned up.
		//
		printf("[conn][%p] All done\n", Connection->Handle);
		if (!Event->SHUTDOWN_COMPLETE.AppCloseInProgress) {
			ctx->ServerStream = nullptr;
		}
		break;
	case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
		//
		// The peer has started/created a new stream. The app MUST set the
		// callback handler before returning.
		//
		printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
		//ctx->ServerStream =
		//	new(std::nothrow) MsQuicStream(Event->PEER_STREAM_STARTED.Stream, CleanUpAutoDelete, ClientStreamCallback, Context);
		//ServerSend(ctx->ServerStream);
		break;
	case QUIC_CONNECTION_EVENT_RESUMPTION_TICKET_RECEIVED:
		//
		// A resumption ticket (also called New Session Ticket or NST) was
		// received from the server.
		//
		printf("[conn][%p] Resumption ticket received (%u bytes):\n", Connection->Handle, Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength);
		for (uint32_t i = 0; i < Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength; i++) {
			printf("%.2X", (uint8_t)Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicket[i]);
		}
		printf("\n");
		break;
	default:
		printf("[conn][%p] Unknown Event %d\n", Connection->Handle, Event->Type);
		break;
	}
	return QUIC_STATUS_SUCCESS;
}