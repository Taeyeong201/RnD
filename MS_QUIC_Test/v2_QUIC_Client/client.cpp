#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>
#include <functional>
#include <string>

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/DebugOutputAppender.h>

#include "QUIC_Framework.h"

int main() {

	static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
	plog::init(plog::verbose, &debugOutputAppender);

	if (QUIC_FAILED(QuicFramework::QuicOpen())) {
		return -1;
	}

	QuicFramework quicFramework;


	quicFramework.quicSettings_;
	const uint32_t Version = 0xff00001dU; // IETF draft 29
	quicFramework.quicSettings_.SetDesiredVersionsList(&Version, 1);
	quicFramework.quicSettings_.SetIdleTimeoutMs(50000);
	quicFramework.quicSettings_.SetDisconnectTimeoutMs(10000);
	quicFramework.quicSettings_.SetPeerBidiStreamCount(5);
	//quicFramework.quicSettings_.SetDatagramReceiveEnabled(true);
	quicFramework.quicSettings_.SetMinimumMtu(3000);
	quicFramework.quicSettings_.SetMaximumMtu(4000);

	quicFramework.quicSettings_.KeepAliveIntervalMs = 5000;
	quicFramework.quicSettings_.IsSet.KeepAliveIntervalMs = TRUE;

	quicFramework.streamManager_.initStreamName("main");

	quicFramework.initializeConfig();

	quicFramework.connection("192.168.0.201", 12155);
	quicFramework.streamManager_.WaitForCreateConnection();
	printf("connect\n");
	std::string input_string;
	while (true)
	{
		std::cin >> input_string;
		if (input_string.compare("exit") == 0) {
			break;
		}
		else if (input_string.compare("r") == 0) {
			DataPacket data = { 0, };
			quicFramework.streamManager_["main"]->receiveData(data);
		}
		else {
			//for (int i = 0; i < 20; i++) {};
			quicFramework.streamManager_["main"]->Send(input_string.c_str(), input_string.length());
		}
	}

	//while (true) {
	//	std::cin >> input_string;


	//	auto SendBufferRaw = malloc(sizeof(QUIC_BUFFER) + 1000000);
	//	//uint8_t SendBufferRaw[sizeof(QUIC_BUFFER) + 100000];
	//	if (SendBufferRaw == nullptr) {
	//		printf("SendBuffer allocation failed!\n");
	//		quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//		break;
	//	}

	//	auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
	//	SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);

	//	printf("[strm][%p] Sending data...\n", quicInterface->ServerStream);

	//	memset(SendBuffer->Buffer, 0, 1000000);
	//	memcpy(SendBuffer->Buffer, input_string.c_str(), input_string.length());
	//	SendBuffer->Length = 1000000;

	//	//
	//	// Sends the buffer over the stream. Note the FIN flag is passed along with
	//	// the buffer. This indicates this is the last buffer on the stream and the
	//	// the stream is shut down (in the send direction) immediately after.
	//	//
	//	QUIC_STATUS Status;
	//	if (input_string.compare("exit") == 0) {
	//		if (quicInterface->ServerStream) {
	//			if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_FIN, SendBuffer))) {
	//				printf("StreamSend failed, ");
	//				QUICInterface::StatusPrint(Status);
	//				free(SendBufferRaw);
	//				quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//			}
	//			connection->Shutdown(0);
	//		}
	//		else {
	//			printf("Stream null\n");
	//			free(SendBufferRaw);
	//		}
	//		break;
	//	}
	//	else if (input_string.compare("reconnect") == 0) {

	//		if (quicInterface->ServerStream) {
	//			if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_FIN, SendBuffer))) {
	//				printf("StreamSend failed, ");
	//				QUICInterface::StatusPrint(Status);
	//				free(SendBufferRaw);
	//				quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//				break;
	//			}
	//		}

	//		printf("Stream Reconnect\n");
	//		quicInterface->ServerStream =
	//			new(std::nothrow) MsQuicStream(
	//				*connection, 
	//				QUIC_STREAM_OPEN_FLAG_NONE,
	//				CleanUpAutoDelete, 
	//				QUICInterface::ClientStreamCallback, quicInterface);
	//		quicInterface->ServerStream->Start(QUIC_STREAM_START_FLAG_NONE);
	//		//while (true) {
	//		//	Sleep(40);
	//		//	if (quicInterface->onStream > 1) break;
	//		//}

	//		//auto SendBufferRaw1 = malloc(sizeof(QUIC_BUFFER) + 1000);
	//		//if (SendBufferRaw1 == nullptr) {
	//		//	printf("SendBuffer allocation failed!\n");
	//		//	quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//		//	break;
	//		//}

	//		//auto SendBuffer1 = (QUIC_BUFFER*)SendBufferRaw1;
	//		//SendBuffer1->Buffer = (uint8_t*)SendBufferRaw1 + sizeof(QUIC_BUFFER);

	//		//printf("[strm][%p] Sending data...\n", quicInterface->ServerStream);

	//		//memset(SendBuffer1->Buffer, 0, 1000);
	//		//memcpy(SendBuffer1->Buffer, input_string.c_str(), input_string.length());
	//		//SendBuffer1->Length = input_string.length() + 1;

	//		//if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer1, 1, QUIC_SEND_FLAG_NONE, SendBuffer1))) {
	//		//	printf("StreamSend failed, ");
	//		//	QUICInterface::StatusPrint(Status);
	//		//	free(SendBufferRaw1);
	//		//	quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//		//	break;
	//		//}
	//		continue;
	//	}
	//	else if (input_string.compare("1000") == 0) {
	//		for (int i = 0; i < 1000; i++) {
	//			if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, nullptr))) {
	//				printf("StreamSend failed, ");
	//				QUICInterface::StatusPrint(Status);
	//				free(SendBufferRaw);
	//				quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//				break;
	//			}

	//			Sleep(2);
	//		}
	//		if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
	//			printf("StreamSend failed, ");
	//			QUICInterface::StatusPrint(Status);
	//			free(SendBufferRaw);
	//			quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//			break;
	//		}

	//		continue;
	//	}

	//	if (quicInterface->ServerStream) {
	//		
	//		if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
	//			printf("StreamSend failed, ");
	//			QUICInterface::StatusPrint(Status);
	//			free(SendBufferRaw);
	//			quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//			break;
	//		}
	//	}
	//	else {
	//		printf("Stream null\n");
	//		free(SendBufferRaw);
	//		break;
	//	}
	//}
	printf("exit.\n\n");
	getchar();

	quicFramework.close();

	QuicFramework::QuicClose();

	return 0;
}