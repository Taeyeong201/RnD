#include "QUIC_Framework.h"

#include <iostream>

const MsQuicApi* MsQuic;

QuicFramework::QuicFramework()
	: alpn_("cloudein"), credConfig_(), crtFile_()
{
	credConfig_.Flags = QUIC_CREDENTIAL_FLAG_CLIENT | QUIC_CREDENTIAL_FLAG_NO_CERTIFICATE_VALIDATION;
	credConfig_.Type = QUIC_CREDENTIAL_TYPE_NONE;
	QUIC_STATUS Status;
	if (QUIC_FAILED(Status = quicSettings_.GetGlobal())) {
#ifdef PLOG
		PLOG_ERROR << "GetSetting Failed";
		StatusPrint(Status);
#else
		std::cout << "GetSetting Failed, ";
		StatusPrint(Status);
#endif
	}
}

QuicFramework::QuicFramework(const std::string form)
	: alpn_("cloudein"), credConfig_(), crtFile_()
{
	if (form.compare("server") == 0) {
		isServer = true;

		crtFile_.PrivateKeyFile = "server.key";
		crtFile_.CertificateFile = "server.pem";
		credConfig_.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE;
		credConfig_.Flags = QUIC_CREDENTIAL_FLAG_NONE;
		credConfig_.CertificateFile = &crtFile_;
	}
	QUIC_STATUS Status;
	if (QUIC_FAILED(Status = quicSettings_.GetGlobal())) {
#ifdef PLOG
		PLOG_ERROR << "GetSetting Failed";
		StatusPrint(Status);
#else
		std::cout << "GetSetting Failed, ";
		StatusPrint(Status);
#endif
	}
}


int QuicFramework::QuicOpen()
{
	QUIC_STATUS Status;

	MsQuic = new (std::nothrow) MsQuicApi();
	if (MsQuic == nullptr) {
		std::cout << "MsQuic alloc out of memory" << std::endl;
		return -1;
	}
	if (QUIC_FAILED(Status = MsQuic->GetInitStatus())) {
#ifdef PLOG
		PLOG_ERROR.printf("MsQuic Failed To Initialize: 0x%x\n", Status);
#else
		printf("MsQuic Failed To Initialize: 0x%x\n", Status);
#endif
		return -1;
	}
	return 0;
}

int QuicFramework::QuicClose()
{
	if (MsQuic != nullptr) {
		delete MsQuic;
		MsQuic = nullptr;
	}
	return 0;
}

//void QuicFramework::setStreamList(std::unique_ptr<QuicSessionList> list)
//{
//
//}

int QuicFramework::initializeConfig()
{
	QUIC_STATUS Status;

	quicRegist_ = new MsQuicRegistration();
	if (!quicRegist_->IsValid()) {
#ifdef PLOG
		PLOG_ERROR << "Registration Failed";
		StatusPrint(quicRegist_->GetInitStatus());
#else
		std::cout << "Registration Failed, ";
		StatusPrint(quicRegist_->GetInitStatus());
#endif
		return -1;
	}
	//	if (QUIC_FAILED(Status = quicSettings_.GetGlobal())) {
	//#ifdef PLOG
	//		PLOG_ERROR << "GetSetting Failed";
	//		StatusPrint(Status);
	//#else
	//		std::cout << "GetSetting Failed, ";
	//		StatusPrint(Status);
	//#endif
	//		return -1;
	//	}

	quicConfig_ = new MsQuicConfiguration(*quicRegist_, alpn_, quicSettings_, credConfig_);
	if (!quicConfig_->IsValid()) {
#ifdef PLOG
		PLOG_ERROR << "Configuration Failed";
		StatusPrint(quicConfig_->GetInitStatus());
#else
		std::cout << "Configuration Failed, ";
		StatusPrint(quicConfig_->GetInitStatus());
#endif
		return -1;
	}

	return 0;
}

bool QuicFramework::allShutdown()
{
	if (!brokenConnection) {
		if (quicConnection_)
			quicConnection_->Shutdown(0, QUIC_CONNECTION_SHUTDOWN_FLAG_NONE);
		if (quicRegist_)
			quicRegist_->Shutdown(QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
	}
	return false;
}

bool QuicFramework::close()
{
	allShutdown();

	if (quicListener_) {
		delete quicListener_;
		quicListener_ = nullptr;
	}
	if (quicConfig_) {
		delete quicConfig_;
		quicConfig_ = nullptr;
	}
	if (quicRegist_) {
		delete quicRegist_;
		quicRegist_ = nullptr;
	}

	return true;
}

int QuicFramework::connection(const std::string& ip,const unsigned short port)
{
	QUIC_STATUS Status;

	quicConnection_ = new MsQuicConnection(*quicRegist_, CleanUpAutoDelete, QuicFramework::ClientConnCallback, this);
	if (!quicConnection_->IsValid()) {
#ifdef PLOG
		PLOG_ERROR << "Connection Failed";
		StatusPrint(quicListener_->GetInitStatus());
#else
		std::cout << "Connection Failed, ";
		StatusPrint(quicListener_->GetInitStatus());
#endif
		return -1;
	}
	if (QUIC_FAILED(Status = quicConnection_->Start(*quicConfig_, ip.c_str(), port))) {
		printf("ConnectionStart failed, 0x%x!\n", Status);
	}

	return 0;
}

int QuicFramework::startListener(int port)
{
	QUIC_STATUS Status;

	quicListener_ = new MsQuicAutoAcceptListener(*quicRegist_, *quicConfig_, ServerConnCallback, this);
	if (!quicListener_->IsValid()) {
#ifdef PLOG
		PLOG_ERROR << "Listener Failed";
		StatusPrint(quicListener_->GetInitStatus());
#else
		std::cout << "Listener Failed, ";
		StatusPrint(quicListener_->GetInitStatus());
#endif
		return -1;
	}
	QUIC_ADDR addr = {};
	QuicAddrSetFamily(&addr, QUIC_ADDRESS_FAMILY_UNSPEC);
	QuicAddrSetPort(&addr, port);

	if (QUIC_FAILED(Status = quicListener_->Start(alpn_, &addr))) {
#ifdef PLOG
		PLOG_ERROR << "Listener Start Failed";
		StatusPrint(Status);
#else
		std::cout << "Listener Start Failed, ";
		StatusPrint(Status);
#endif
		return -1;
	};
	return 0;
}

void QuicFramework::stopListener()
{
}

void QuicFramework::getLocalAddress(QuicAddr& addr)
{
	quicListener_->GetLocalAddr(addr);
}

#ifdef PLOG
_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_CONNECTION_CALLBACK)
QUIC_STATUS QuicFramework::ServerConnCallback(
	_In_ MsQuicConnection* Connection,
	_In_opt_ void* Context,
	_Inout_ QUIC_CONNECTION_EVENT* Event)
{
	auto ctx = (QuicFramework*)Context;
	switch (Event->Type) {
	case QUIC_CONNECTION_EVENT_CONNECTED:
		PLOG_INFO.printf("[conn][%p] Connected\n", Connection->Handle);
		if (ctx) ctx->quicConnection_ = Connection;
		Connection->SendResumptionTicket(QUIC_SEND_RESUMPTION_FLAG_FINAL, 0, nullptr);
		break;
	case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
		PLOG_INFO.printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
		//if (ctx) 
		//	ctx->quicSessions_->newSession(Event->PEER_STREAM_STARTED.Stream);
		//else
		//	return QUIC_STATUS_OUT_OF_MEMORY;
		if (ctx) {
			ctx->quicConnection_ = Connection;
			ctx->stream_.stream_ =
				new(std::nothrow) MsQuicStream(
					*Connection,
					QUIC_STREAM_OPEN_FLAG_NONE,
					CleanUpAutoDelete,
					QuicStream::StreamCallback,
					&ctx->stream_
				);
			ctx->stream_.InitializeSend();
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
			PLOG_INFO.printf("[conn][%p] Successfully shut down on idle.\n", Connection->Handle);
		}
		else {
			PLOG_INFO.printf("[conn][%p] Shut down by transport, ", Connection->Handle);
			StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		PLOG_INFO.printf("[conn][%p] Shut down by peer ", Connection->Handle);
		StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
		break;

	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		PLOG_INFO.printf("[conn][%p] All done\n", Connection->Handle);
		//if (ctx) {
		//	for (auto elem : ctx->quicSessions_->sessionListMap_) {
		//		elem.second.reset();
		//	}
		//	ctx->quicSessions_->sessionListMap_.clear();
		//}
		if (ctx) {
			ctx->brokenConnection = true;
			ctx->stream_.stream_ = nullptr;
		}
		break;
#ifdef DEBUG_
	case QUIC_CONNECTION_EVENT_RESUMED:
		PLOG_INFO.printf("[conn][%p] Connection resumed!\n", Connection->Handle);
		break;
	case QUIC_CONNECTION_EVENT_IDEAL_PROCESSOR_CHANGED:
		PLOG_INFO.printf("[conn][%p] ideal processor chaged\n", Connection->Handle);
		break;
	default:
		PLOG_INFO.printf("[conn][%p] Unknown Event %d\n", Connection->Handle, Event->Type);
		break;
#else
	default:
		break;
#endif
	}
	return QUIC_STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_CONNECTION_CALLBACK)
QUIC_STATUS QuicFramework::ClientConnCallback(
	_In_ MsQuicConnection* Connection,
	_In_opt_ void* Context,
	_Inout_ QUIC_CONNECTION_EVENT* Event)
{
	auto ctx = (QuicFramework*)Context;
	switch (Event->Type) {
	case QUIC_CONNECTION_EVENT_CONNECTED:
		PLOG_INFO.printf("[conn][%p] Connected\n", Connection->Handle);
		if (ctx) ctx->quicConnection_ = Connection;

		if (ctx) {
			ctx->quicConnection_ = Connection;
			ctx->stream_.stream_ =
				new(std::nothrow) MsQuicStream(
					*Connection,
					QUIC_STREAM_OPEN_FLAG_NONE,
					CleanUpAutoDelete,
					QuicStream::StreamCallback,
					&ctx->stream_
				);
			ctx->stream_.InitializeSend();
		}

		break;
	case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
		PLOG_INFO.printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
		//if (ctx) 
		//	ctx->quicSessions_->newSession(Event->PEER_STREAM_STARTED.Stream);
		//else
		//	return QUIC_STATUS_OUT_OF_MEMORY;
		if (ctx) {
			ctx->stream_.stream_ =
				new(std::nothrow) MsQuicStream(
					Event->PEER_STREAM_STARTED.Stream,
					CleanUpAutoDelete,
					QuicStream::StreamCallback,
					&ctx->stream_);
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
			PLOG_INFO.printf("[conn][%p] Successfully shut down on idle.\n", Connection->Handle);
		}
		else {
			PLOG_INFO.printf("[conn][%p] Shut down by transport, ", Connection->Handle);
			StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		PLOG_INFO.printf("[conn][%p] Shut down by peer ", Connection->Handle);
		StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
		break;

	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		PLOG_INFO.printf("[conn][%p] All done\n", Connection->Handle);
		//if (ctx) {
		//	for (auto elem : ctx->quicSessions_->sessionListMap_) {
		//		elem.second.reset();
		//	}
		//	ctx->quicSessions_->sessionListMap_.clear();
		//}
		if (ctx) {
			ctx->brokenConnection = true;
			ctx->stream_.stream_ = nullptr;
		}
		break;
#ifdef DEBUG_
	case QUIC_CONNECTION_EVENT_RESUMED:
		PLOG_INFO.printf("[conn][%p] Connection resumed!\n", Connection->Handle);
		break;
	case QUIC_CONNECTION_EVENT_IDEAL_PROCESSOR_CHANGED:
		PLOG_INFO.printf("[conn][%p] ideal processor chaged\n", Connection->Handle);
		break;
	default:
		PLOG_INFO.printf("[conn][%p] Unknown Event %d\n", Connection->Handle, Event->Type);
		break;
#else
	default:
		break;
#endif
	}
	return QUIC_STATUS_SUCCESS;
}
#else

_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_CONNECTION_CALLBACK)
QUIC_STATUS QuicFramework::ServerConnCallback(
	_In_ MsQuicConnection* Connection,
	_In_opt_ void* Context,
	_Inout_ QUIC_CONNECTION_EVENT* Event)
{
	auto ctx = (QuicFramework*)Context;
	switch (Event->Type) {
	case QUIC_CONNECTION_EVENT_CONNECTED:
		printf("[conn][%p] Connected\n", Connection->Handle);
		if (ctx) ctx->quicConnection_ = Connection;
		Connection->SendResumptionTicket(QUIC_SEND_RESUMPTION_FLAG_FINAL, 0, nullptr);
		break;
	case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
		printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
		//if (ctx) 
		//	ctx->quicSessions_->newSession(Event->PEER_STREAM_STARTED.Stream);
		//else
		//	return QUIC_STATUS_OUT_OF_MEMORY;

		if (ctx) {
			ctx->stream_.stream_ =
				new(std::nothrow) MsQuicStream(
					Event->PEER_STREAM_STARTED.Stream,
					CleanUpAutoDelete,
					QuicStream::StreamCallback,
					&ctx->stream_);
		}

		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
			printf("[conn][%p] Successfully shut down on idle.\n", Connection->Handle);
		}
		else {
			printf("[conn][%p] Shut down by transport, ", Connection->Handle);
			StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		printf("[conn][%p] Shut down by peer ", Connection->Handle);
		StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		printf("[conn][%p] All done\n", Connection->Handle);
		//if (ctx) {
		//	for (auto elem : ctx->quicSessions_->sessionListMap_) {
		//		elem.second.reset();
		//	}
		//	ctx->quicSessions_->sessionListMap_.clear();
		//}
		if (ctx) {
			ctx->brokenConnection = true;
			ctx->stream_.stream_ = nullptr;
		}
		break;
	case QUIC_CONNECTION_EVENT_RESUMED:
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
_Function_class_(QUIC_CONNECTION_CALLBACK)
QUIC_STATUS QuicFramework::ClientConnCallback(
	_In_ MsQuicConnection* Connection,
	_In_opt_ void* Context,
	_Inout_ QUIC_CONNECTION_EVENT* Event)
{
	auto ctx = (QuicFramework*)Context;
	switch (Event->Type) {
	case QUIC_CONNECTION_EVENT_CONNECTED:
		printf("[conn][%p] Connected\n", Connection->Handle);
		if (ctx) {
			ctx->quicConnection_ = Connection;
			ctx->stream_.stream_ =
				new(std::nothrow) MsQuicStream(
					*Connection,
					QUIC_STREAM_OPEN_FLAG_NONE, 
					CleanUpAutoDelete, 
					QuicStream::StreamCallback, 
					&ctx->stream_
				);
			ctx->stream_.InitializeSend();
		}

		break;
	case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
		printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
			printf("[conn][%p] Successfully shut down on idle.\n", Connection->Handle);
		}
		else {
			printf("[conn][%p] Shut down by transport, ", Connection->Handle);
			StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
		}
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		printf("[conn][%p] Shut down by peer ", Connection->Handle);
		StatusPrint((unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		printf("[conn][%p] All done\n", Connection->Handle);
		//if (ctx) {
		//	for (auto elem : ctx->quicSessions_->sessionListMap_) {
		//		elem.second.reset();
		//	}
		//	ctx->quicSessions_->sessionListMap_.clear();
		//}
		if (ctx) {
			ctx->brokenConnection = true;
			ctx->stream_.stream_ = nullptr;
		}
		break;
	case QUIC_CONNECTION_EVENT_RESUMED:
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
#endif