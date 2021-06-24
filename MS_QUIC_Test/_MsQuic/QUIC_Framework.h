#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#include "QUIC_Common.h"

#include "QUIC_Stream.h"

class QuicFramework
{
public:
	QuicFramework();
	QuicFramework(const std::string form);

	//only once in the program
	static int QuicOpen();
	static int QuicClose();

	MsQuicSettings quicSettings_;

	//Server, Clinet Common
	QuicStream stream_;
	 
	//void setStreamList(std::unique_ptr<QuicSessionList> list);
	int initializeConfig();

	bool allShutdown();
	bool close();

	//only Client
	int connection(const std::string& ip, const unsigned short port);

	//only Server
	int startListener(int port);
	void getLocalAddress(QuicAddr& addr);
private:
	//Server, Clinet Common
	MsQuicRegistration* quicRegist_ = nullptr;
	MsQuicConfiguration* quicConfig_ = nullptr;
	MsQuicConnection* quicConnection_ = nullptr;

	//std::unique_ptr<QuicSessionList> quicSessions_;

	MsQuicCredentialConfig credConfig_;
	MsQuicAlpn alpn_;

	bool isServer = false;

	//only Client
	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_CONNECTION_CALLBACK)
		QUIC_STATUS
		ClientConnCallback(
			_In_ MsQuicConnection* Connection,
			_In_opt_ void* Context,
			_Inout_ QUIC_CONNECTION_EVENT* Event
		);

	//only Server
	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_CONNECTION_CALLBACK)
		QUIC_STATUS
		ServerConnCallback(
			_In_ MsQuicConnection* Connection,
			_In_opt_ void* Context,
			_Inout_ QUIC_CONNECTION_EVENT* Event
		);

	

	MsQuicAutoAcceptListener* quicListener_ = nullptr;
	QUIC_CERTIFICATE_FILE crtFile_;
};

