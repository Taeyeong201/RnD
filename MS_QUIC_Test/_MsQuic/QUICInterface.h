#pragma once

#include <msquic.hpp>

class QUICInterface
{
public:
	QUICInterface();
	~QUICInterface();

	static void StatusPrint(QUIC_STATUS status);

	//only once in the program
	static int MsQuicOpen();
	static int MsQuicClose();

    MsQuicStream* ServerStream{ nullptr };

	int onStream = 0;

	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_STREAM_CALLBACK)
		QUIC_STATUS StreamCallback(
		_In_ MsQuicStream* Stream, 
		_In_opt_ void* Context, 
		_Inout_ QUIC_STREAM_EVENT* Event
	);

	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_CONNECTION_CALLBACK)
		QUIC_STATUS 
		ConnCallback(
		_In_ MsQuicConnection* Connection, 
		_In_opt_ void* Context, 
		_Inout_ QUIC_CONNECTION_EVENT* Event
	);

	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_STREAM_CALLBACK)
		QUIC_STATUS ClientStreamCallback(
			_In_ MsQuicStream* Stream,
			_In_opt_ void* Context,
			_Inout_ QUIC_STREAM_EVENT* Event
		);

	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_CONNECTION_CALLBACK)
		QUIC_STATUS
		ClientConnCallback(
			_In_ MsQuicConnection* Connection,
			_In_opt_ void* Context,
			_Inout_ QUIC_CONNECTION_EVENT* Event
		);

private:

};