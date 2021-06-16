#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>
#include <functional>
#include <string>

#include "QUICInterface.h"

int main() {
	if (QUIC_FAILED(QUICInterface::MsQuicOpen())) {
		return -1;
	}
	QUIC_STATUS Status;


	{

		MsQuicRegistration QUICRegist;
		if (!QUICRegist.IsValid()) {
			std::cout << "Registration Failed" << std::endl;
			return -1;
		}
		MsQuicAlpn Alpn("test");

		QUIC_ADDR Address = {};
		QuicAddrSetFamily(&Address, QUIC_ADDRESS_FAMILY_UNSPEC);
		QuicAddrSetPort(&Address, 12154);

		QuicAddr Addr(QUIC_ADDRESS_FAMILY_UNSPEC, (uint16_t)12154);

		MsQuicSettings Settings;
		Settings.GetGlobal();
		Settings.SetIdleTimeoutMs(6000);
		const uint32_t Version = 0xff00001dU; // IETF draft 29
		Settings.SetDesiredVersionsList(&Version, 1);
		Settings.SetMinimumMtu(3000);
		Settings.SetMaximumMtu(4000);
		//Settings.SetDatagramReceiveEnabled(true);

		Settings.KeepAliveIntervalMs = 5000;
		Settings.IsSet.KeepAliveIntervalMs = TRUE;

		Settings.SetPeerBidiStreamCount(5);

		if (Settings.SetGlobal() < 0) {
			std::cout << "Global Setting Failed" << std::endl;
		};

		MsQuicCredentialConfig CredentialConfig(QUIC_CREDENTIAL_FLAG_CLIENT | QUIC_CREDENTIAL_FLAG_NO_CERTIFICATE_VALIDATION);
		CredentialConfig.Type = QUIC_CREDENTIAL_TYPE_NONE;
		//QUIC_CERTIFICATE_FILE crtFile{ "rootCA.key", "rootCA.pem" };
		//CredentialConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE;
		//CredentialConfig.CertificateFile = &crtFile;

		MsQuicConfiguration ClientConfiguration(QUICRegist, Alpn, Settings, CredentialConfig);

		if (!ClientConfiguration.IsValid()) {
			QUICInterface::StatusPrint(ClientConfiguration.GetInitStatus());
			return -1;
		}

		QUICInterface* quicInterface;
		quicInterface = new QUICInterface();

		MsQuicConnection* connection = new MsQuicConnection(QUICRegist, CleanUpAutoDelete, QUICInterface::ClientConnCallback, quicInterface);
		if (!connection->IsValid()) {
			QUICInterface::StatusPrint(connection->GetInitStatus());
			return -1;
		}
		if (QUIC_FAILED(Status = connection->Start(ClientConfiguration, "192.168.0.35", 12154))) {
			//if (QUIC_FAILED(Status = connection.StartLocalhost(ClientConfiguration, Addr))) {
			printf("ConnectionStart failed, 0x%x!\n", Status);
			//connection.StartLocalhost()
		}

		std::string input_string;

		while (true) {
			std::cin >> input_string;


			auto SendBufferRaw = malloc(sizeof(QUIC_BUFFER) + 100000);
			//uint8_t SendBufferRaw[sizeof(QUIC_BUFFER) + 100000];
			if (SendBufferRaw == nullptr) {
				printf("SendBuffer allocation failed!\n");
				quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
				break;
			}

			auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
			SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof(QUIC_BUFFER);

			printf("[strm][%p] Sending data...\n", quicInterface->ServerStream);

			memset(SendBuffer->Buffer, 0, 100000);
			memcpy(SendBuffer->Buffer, input_string.c_str(), input_string.length());
			SendBuffer->Length = 100000;

			//
			// Sends the buffer over the stream. Note the FIN flag is passed along with
			// the buffer. This indicates this is the last buffer on the stream and the
			// the stream is shut down (in the send direction) immediately after.
			//
			QUIC_STATUS Status;
			if (input_string.compare("exit") == 0) {
				if (quicInterface->ServerStream) {
					if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_FIN, SendBuffer))) {
						printf("StreamSend failed, ");
						QUICInterface::StatusPrint(Status);
						free(SendBufferRaw);
						quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
					}
					connection->Shutdown(0);
				}
				else {
					printf("Stream null\n");
					free(SendBufferRaw);
				}
				break;
			}
			else if (input_string.compare("reconnect") == 0) {

				if (quicInterface->ServerStream) {
					if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_FIN, SendBuffer))) {
						printf("StreamSend failed, ");
						QUICInterface::StatusPrint(Status);
						free(SendBufferRaw);
						quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
						break;
					}
				}

				printf("Stream Reconnect\n");
				quicInterface->ServerStream =
					new(std::nothrow) MsQuicStream(
						*connection, 
						QUIC_STREAM_OPEN_FLAG_NONE,
						CleanUpAutoDelete, 
						QUICInterface::ClientStreamCallback, quicInterface);
				quicInterface->ServerStream->Start(QUIC_STREAM_START_FLAG_NONE);
				//while (true) {
				//	Sleep(40);
				//	if (quicInterface->onStream > 1) break;
				//}

				//auto SendBufferRaw1 = malloc(sizeof(QUIC_BUFFER) + 1000);
				//if (SendBufferRaw1 == nullptr) {
				//	printf("SendBuffer allocation failed!\n");
				//	quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
				//	break;
				//}

				//auto SendBuffer1 = (QUIC_BUFFER*)SendBufferRaw1;
				//SendBuffer1->Buffer = (uint8_t*)SendBufferRaw1 + sizeof(QUIC_BUFFER);

				//printf("[strm][%p] Sending data...\n", quicInterface->ServerStream);

				//memset(SendBuffer1->Buffer, 0, 1000);
				//memcpy(SendBuffer1->Buffer, input_string.c_str(), input_string.length());
				//SendBuffer1->Length = input_string.length() + 1;

				//if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer1, 1, QUIC_SEND_FLAG_NONE, SendBuffer1))) {
				//	printf("StreamSend failed, ");
				//	QUICInterface::StatusPrint(Status);
				//	free(SendBufferRaw1);
				//	quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
				//	break;
				//}
				continue;
			}

			if (quicInterface->ServerStream) {
				
				if (QUIC_FAILED(Status = quicInterface->ServerStream->Send(SendBuffer, 1, QUIC_SEND_FLAG_NONE, SendBuffer))) {
					printf("StreamSend failed, ");
					QUICInterface::StatusPrint(Status);
					free(SendBufferRaw);
					quicInterface->ServerStream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_ABORT);
					break;
				}
			}
			else {
				printf("Stream null\n");
				free(SendBufferRaw);
				break;
			}
		}
		printf("exit.\n\n");
		getchar();
	}
	QUICInterface::MsQuicClose();
	return 0;
}