#include "QUIC_Common.h"


#include <iostream>

void StatusPrint(QUIC_STATUS status)
{
	switch (status) {
	case QUIC_STATUS_SUCCESS:
#ifdef PLOG
		PLOG_ERROR << "The operation completed successfully.";
#else
		std::cout << "The operation completed successfully." << std::endl;
#endif
		break;
	case QUIC_STATUS_PENDING:
#ifdef PLOG
		PLOG_ERROR << "The operation is pending.";
#else
		std::cout << "The operation is pending." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONTINUE:
#ifdef PLOG
		PLOG_ERROR << "The operation will continue.";
#else
		std::cout << "The operation will continue." << std::endl;
#endif
		break;
	case QUIC_STATUS_OUT_OF_MEMORY:
#ifdef PLOG
		PLOG_ERROR << "Allocation of memory failed.";
#else
		std::cout << "Allocation of memory failed." << std::endl;
#endif
		break;
	case QUIC_STATUS_INVALID_PARAMETER:
#ifdef PLOG
		PLOG_ERROR << "An invalid parameter was encountered.";
#else
		std::cout << "An invalid parameter was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_INVALID_STATE:
#ifdef PLOG
		PLOG_ERROR << "The current state was not valid for this operation.";
#else
		std::cout << "The current state was not valid for this operation." << std::endl;
#endif
		break;
	case QUIC_STATUS_NOT_SUPPORTED:
#ifdef PLOG
		PLOG_ERROR << "The operation was not supported.";
#else
		std::cout << "The operation was not supported." << std::endl;
#endif
		break;
	case QUIC_STATUS_NOT_FOUND:
#ifdef PLOG
		PLOG_ERROR << "The object was not found.";
#else
		std::cout << "The object was not found." << std::endl;
#endif
		break;
	case QUIC_STATUS_BUFFER_TOO_SMALL:
#ifdef PLOG
		PLOG_ERROR << "The buffer was too small for the operation.";
#else
		std::cout << "The buffer was too small for the operation." << std::endl;
#endif
		break;
	case QUIC_STATUS_HANDSHAKE_FAILURE:
#ifdef PLOG
		PLOG_ERROR << "The connection handshake failed.";
#else
		std::cout << "The connection handshake failed." << std::endl;
#endif
		break;
	case QUIC_STATUS_ABORTED:
#ifdef PLOG
		PLOG_ERROR << "The connection or stream was aborted.";
#else
		std::cout << "The connection or stream was aborted." << std::endl;
#endif
		break;
	case QUIC_STATUS_ADDRESS_IN_USE:
#ifdef PLOG
		PLOG_ERROR << "The local address is already in use.";
#else
		std::cout << "The local address is already in use." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONNECTION_TIMEOUT:
#ifdef PLOG
		PLOG_ERROR << "The connection timed out waiting for a response from the peer.";
#else
		std::cout << "The connection timed out waiting for a response from the peer." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONNECTION_IDLE:
#ifdef PLOG
		PLOG_ERROR << "The connection timed out from inactivity.";
#else
		std::cout << "The connection timed out from inactivity." << std::endl;
#endif
		break;
	case QUIC_STATUS_INTERNAL_ERROR:
#ifdef PLOG
		PLOG_ERROR << "An internal error was encountered.";
#else
		std::cout << "An internal error was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_UNREACHABLE:
#ifdef PLOG
		PLOG_ERROR << "The server is currently unreachable.";
#else
		std::cout << "The server is currently unreachable." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONNECTION_REFUSED:
#ifdef PLOG
		PLOG_ERROR << "The server refused the connection.";
#else
		std::cout << "The server refused the connection." << std::endl;
#endif
		break;
	case QUIC_STATUS_PROTOCOL_ERROR:
#ifdef PLOG
		PLOG_ERROR << "A protocol error was encountered.";
#else
		std::cout << "A protocol error was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_VER_NEG_ERROR:
#ifdef PLOG
		PLOG_ERROR << "A version negotiation error was encountered.";
#else
		std::cout << "A version negotiation error was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_USER_CANCELED:
#ifdef PLOG
		PLOG_ERROR << "The peer app/user canceled the connection during the handshake.";
#else
		std::cout << "The peer app/user canceled the connection during the handshake." << std::endl;
#endif
		break;
	case QUIC_STATUS_ALPN_NEG_FAILURE:
#ifdef PLOG
		PLOG_ERROR << "The connection handshake failed to negotiate a common ALPN.";
#else
		std::cout << "The connection handshake failed to negotiate a common ALPN." << std::endl;
#endif
		break;
	case QUIC_STATUS_STREAM_LIMIT_REACHED:
#ifdef PLOG
		PLOG_ERROR << "A stream failed to start because the peer doesn't allow any more to be open at this time.";
#else
		std::cout << "A stream failed to start because the peer doesn't allow any more to be open at this time." << std::endl;
#endif
		break;
	default:
#ifdef PLOG
		PLOG_ERROR.printf("unknown : 0x%x", status);
#else
		printf("unknown : 0x%x\n", status);
#endif
		break;
	}
}
