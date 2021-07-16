
#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>
#include <functional>
#include <string>

#include <thread>

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

	QuicFramework quicFramework("server");

	quicFramework.quicSettings_.SetIdleTimeoutMs(13000);
	quicFramework.quicSettings_.SetServerResumptionLevel(QUIC_SERVER_RESUME_AND_ZERORTT);
	quicFramework.quicSettings_.SetDisconnectTimeoutMs(1000);
	quicFramework.quicSettings_.SetPeerBidiStreamCount(1);


	//quicFramework.quicSettings_;
	//quicFramework.quicSettings_.SetIdleTimeoutMs(50000);
	//quicFramework.quicSettings_.SetServerResumptionLevel(QUIC_SERVER_RESUME_AND_ZERORTT);
	//quicFramework.quicSettings_.SetDisconnectTimeoutMs(1000);
	//quicFramework.quicSettings_.SetPeerBidiStreamCount(5);
	////quicFramework.quicSettings_.SetDatagramReceiveEnabled(true);
	//quicFramework.quicSettings_.SetMinimumMtu(3000);
	//quicFramework.quicSettings_.SetMaximumMtu(4000);

	//printf("%d\n", strlen("111"));

	quicFramework.initializeConfig();
	quicFramework.startListener(12155);

	std::string input_string;

	quicFramework.streamManager_.WaitForCreateStream();
	//quicFramework.stream_.InitializeReceive();
	printf("connect\n");

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
			//for (int i = 0; i < 20; i++)
				//quicFramework.stream_.Send(input_string.c_str(), input_string.length());
		}
	}
	printf("Press Enter to exit.\n\n");
	getchar();

	//{
	//	MsQuicRegistration QUICRegist;
	//	if (!QUICRegist.IsValid()) {
	//		std::cout << "Registration Failed" << std::endl;
	//		return -1;
	//	}

	//	MsQuicAlpn Alpn("test");



	//	QUIC_ADDR Address = {};
	//	QuicAddrSetFamily(&Address, QUIC_ADDRESS_FAMILY_UNSPEC);
	//	QuicAddrSetPort(&Address, 12154);

	//	MsQuicSettings Settings;
	//	Settings.SetIdleTimeoutMs(6000);
	//	Settings.SetServerResumptionLevel(QUIC_SERVER_RESUME_AND_ZERORTT);
	//	Settings.SetDisconnectTimeoutMs(1000);
	//	Settings.SetPeerBidiStreamCount(5);
	//	//Settings.SetDatagramReceiveEnabled(true);
	//	Settings.SetMinimumMtu(3000);
	//	Settings.SetMaximumMtu(4000);
	//	
	//	if (Settings.SetGlobal() < 0) {
	//		std::cout << "Global Setting Failed" << std::endl;
	//	};

	//	MsQuicCredentialConfig CredentialConfig(
	//		QUIC_CREDENTIAL_FLAG_REQUIRE_CLIENT_AUTHENTICATION
	//		| QUIC_CREDENTIAL_FLAG_USE_TLS_BUILTIN_CERTIFICATE_VALIDATION
	//	);

	//	QUIC_CERTIFICATE_FILE crtFile{ "server.key", "server.pem" };
	//	CredentialConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE;
	//	CredentialConfig.CertificateFile = &crtFile;

	//	MsQuicConfiguration ServerConfiguration(QUICRegist, Alpn, Settings, CredentialConfig);
	//	if (!ServerConfiguration.IsValid()) {
	//		QUICInterface::StatusPrint(ServerConfiguration.GetInitStatus());
	//		return -1;
	//	}

	//	QUICInterface* quicInterface;
	//	quicInterface = new QUICInterface();

	//	MsQuicAutoAcceptListener litener(QUICRegist, ServerConfiguration, QUICInterface::ConnCallback, quicInterface);
	//	if (!litener.IsValid()) {
	//		QUICInterface::StatusPrint(litener.GetInitStatus());
	//		return -1;
	//	}
	//	litener.Start(Alpn, &Address);
	//	QuicAddr ServerLocalAddr;
	//	litener.GetLocalAddr(ServerLocalAddr);

	//	char localIP[INET_ADDRSTRLEN] = { 0, };
	//	inet_ntop(AF_INET, &(ServerLocalAddr.SockAddr.Ipv4.sin_addr), localIP, INET_ADDRSTRLEN);

	//	std::cout << std::endl;
	//	std::cout << "Server Address - " << localIP << ":" << ServerLocalAddr.SockAddr.Ipv4.sin_port << std::endl;
	//	std::cout << std::endl;

	//	//getchar();
	//	//printf("Press Enter to send data.\n\n");
	//	std::string input_string;
	//	while(true)
	//	{
	//		std::cin >> input_string;
	//		if (input_string.compare("exit") == 0) {
	//			break;
	//		}

	//		auto SendBufferRaw = malloc(sizeof(QUIC_BUFFER) + 1000);
	//		if (SendBufferRaw == nullptr) {
	//			printf("SendBuffer allocation failed!\n");
	//			quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//			return -1;
	//		}


	//		uint8_t test[sizeof(QUIC_BUFFER) + 1000];

	//		auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
	//		SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);
	//		SendBuffer->Length = input_string.length() + 1;

	//		printf("[strm][%p] Sending data...\n", quicInterface->ServerStream);

	//		memset(SendBuffer->Buffer, 0, 1000);
	//		memcpy(SendBuffer->Buffer, input_string.c_str(), input_string.length());

	//		//
	//		// Sends the buffer over the stream. Note the FIN flag is passed along with
	//		// the buffer. This indicates this is the last buffer on the stream and the
	//		// the stream is shut down (in the send direction) immediately after.
	//		//
	//		if (quicInterface->ServerStream) {
	//			QUIC_STATUS Status;
	//			if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
	//				printf("StreamSend failed, 0x%x!\n", Status);
	//				free(SendBufferRaw);
	//				quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
	//			}
	//		}
	//		else {
	//			printf("Stream null\n");
	//			free(SendBufferRaw);
	//			break;
	//		}
	//	}

	//	printf("Press Enter to exit.\n\n");
	//	getchar();
	//}

	quicFramework.close();

	QuicFramework::QuicClose();


	return 0;
}