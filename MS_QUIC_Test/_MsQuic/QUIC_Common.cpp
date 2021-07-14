#include "QUIC_Common.h"

#include <iostream>
#ifdef PLOG
const char* StatusPrint(QUIC_STATUS status)
#else
void StatusPrint(QUIC_STATUS status)
#endif
{
	switch (status) {
	case QUIC_STATUS_SUCCESS:
#ifdef PLOG
		return "The operation completed successfully.";
#else
		std::cout << "The operation completed successfully." << std::endl;
#endif
		break;
	case QUIC_STATUS_PENDING:
#ifdef PLOG
		return "The operation is pending.";
#else
		std::cout << "The operation is pending." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONTINUE:
#ifdef PLOG
		return "The operation will continue.";
#else
		std::cout << "The operation will continue." << std::endl;
#endif
		break;
	case QUIC_STATUS_OUT_OF_MEMORY:
#ifdef PLOG
		return "Allocation of memory failed.";
#else
		std::cout << "Allocation of memory failed." << std::endl;
#endif
		break;
	case QUIC_STATUS_INVALID_PARAMETER:
#ifdef PLOG
		return "An invalid parameter was encountered.";
#else
		std::cout << "An invalid parameter was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_INVALID_STATE:
#ifdef PLOG
		return "The current state was not valid for this operation.";
#else
		std::cout << "The current state was not valid for this operation." << std::endl;
#endif
		break;
	case QUIC_STATUS_NOT_SUPPORTED:
#ifdef PLOG
		return "The operation was not supported.";
#else
		std::cout << "The operation was not supported." << std::endl;
#endif
		break;
	case QUIC_STATUS_NOT_FOUND:
#ifdef PLOG
		return "The object was not found.";
#else
		std::cout << "The object was not found." << std::endl;
#endif
		break;
	case QUIC_STATUS_BUFFER_TOO_SMALL:
#ifdef PLOG
		return "The buffer was too small for the operation.";
#else
		std::cout << "The buffer was too small for the operation." << std::endl;
#endif
		break;
	case QUIC_STATUS_HANDSHAKE_FAILURE:
#ifdef PLOG
		return "The connection handshake failed.";
#else
		std::cout << "The connection handshake failed." << std::endl;
#endif
		break;
	case QUIC_STATUS_ABORTED:
#ifdef PLOG
		return "The connection or stream was aborted.";
#else
		std::cout << "The connection or stream was aborted." << std::endl;
#endif
		break;
	case QUIC_STATUS_ADDRESS_IN_USE:
#ifdef PLOG
		return "The local address is already in use.";
#else
		std::cout << "The local address is already in use." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONNECTION_TIMEOUT:
#ifdef PLOG
		return "The connection timed out waiting for a response from the peer.";
#else
		std::cout << "The connection timed out waiting for a response from the peer." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONNECTION_IDLE:
#ifdef PLOG
		return "The connection timed out from inactivity.";
#else
		std::cout << "The connection timed out from inactivity." << std::endl;
#endif
		break;
	case QUIC_STATUS_INTERNAL_ERROR:
#ifdef PLOG
		return "An internal error was encountered.";
#else
		std::cout << "An internal error was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_UNREACHABLE:
#ifdef PLOG
		return "The server is currently unreachable.";
#else
		std::cout << "The server is currently unreachable." << std::endl;
#endif
		break;
	case QUIC_STATUS_CONNECTION_REFUSED:
#ifdef PLOG
		return "The server refused the connection.";
#else
		std::cout << "The server refused the connection." << std::endl;
#endif
		break;
	case QUIC_STATUS_PROTOCOL_ERROR:
#ifdef PLOG
		return "A protocol error was encountered.";
#else
		std::cout << "A protocol error was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_VER_NEG_ERROR:
#ifdef PLOG
		return "A version negotiation error was encountered.";
#else
		std::cout << "A version negotiation error was encountered." << std::endl;
#endif
		break;
	case QUIC_STATUS_USER_CANCELED:
#ifdef PLOG
		return "The peer app/user canceled the connection during the handshake.";
#else
		std::cout << "The peer app/user canceled the connection during the handshake." << std::endl;
#endif
		break;
	case QUIC_STATUS_ALPN_NEG_FAILURE:
#ifdef PLOG
		return "The connection handshake failed to negotiate a common ALPN.";
#else
		std::cout << "The connection handshake failed to negotiate a common ALPN." << std::endl;
#endif
		break;
	case QUIC_STATUS_STREAM_LIMIT_REACHED:
#ifdef PLOG
		return "A stream failed to start because the peer doesn't allow any more to be open at this time.";
#else
		std::cout << "A stream failed to start because the peer doesn't allow any more to be open at this time." << std::endl;
#endif
		break;
	default:
#ifdef PLOG
		return "unknown";
#else
		printf("unknown : 0x%x\n", status);
#endif
		break;
	}
}
