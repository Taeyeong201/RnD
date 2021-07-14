#include "QUIC_Common.h"

#include <iostream>
const char* StatusPrint(QUIC_STATUS status)
{
	switch (status) {
	case QUIC_STATUS_SUCCESS:
		return "The operation completed successfully.";
	case QUIC_STATUS_PENDING:
		return "The operation is pending.";
	case QUIC_STATUS_CONTINUE:
		return "The operation will continue.";
	case QUIC_STATUS_OUT_OF_MEMORY:
		return "Allocation of memory failed.";
	case QUIC_STATUS_INVALID_PARAMETER:
		return "An invalid parameter was encountered.";
	case QUIC_STATUS_INVALID_STATE:
		return "The current state was not valid for this operation.";
	case QUIC_STATUS_NOT_SUPPORTED:
		return "The operation was not supported.";
	case QUIC_STATUS_NOT_FOUND:
		return "The object was not found.";
	case QUIC_STATUS_BUFFER_TOO_SMALL:
		return "The buffer was too small for the operation.";
	case QUIC_STATUS_HANDSHAKE_FAILURE:
		return "The connection handshake failed.";
	case QUIC_STATUS_ABORTED:
		return "The connection or stream was aborted.";
	case QUIC_STATUS_ADDRESS_IN_USE:
		return "The local address is already in use.";
	case QUIC_STATUS_CONNECTION_TIMEOUT:
		return "The connection timed out waiting for a response from the peer.";
	case QUIC_STATUS_CONNECTION_IDLE:
		return "The connection timed out from inactivity.";
	case QUIC_STATUS_INTERNAL_ERROR:
		return "An internal error was encountered.";
	case QUIC_STATUS_UNREACHABLE:
		return "The server is currently unreachable.";
	case QUIC_STATUS_CONNECTION_REFUSED:
		return "The server refused the connection.";
	case QUIC_STATUS_PROTOCOL_ERROR:
		return "A protocol error was encountered.";
	case QUIC_STATUS_VER_NEG_ERROR:
		return "A version negotiation error was encountered.";
	case QUIC_STATUS_USER_CANCELED:
		return "The peer app/user canceled the connection during the handshake.";
	case QUIC_STATUS_ALPN_NEG_FAILURE:
		return "The connection handshake failed to negotiate a common ALPN.";
	case QUIC_STATUS_STREAM_LIMIT_REACHED:
		return "A stream failed to start because the peer doesn't allow any more to be open at this time.";
	default:
		return "unknown";
	}
}
