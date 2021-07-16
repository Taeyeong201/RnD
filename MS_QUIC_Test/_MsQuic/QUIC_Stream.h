#pragma once

#include "QUIC_Common.h"

#include "QUIC_DataReceiver.h"

class QuicStream {
public:
	//QuicStream(int id);
	QuicStream() noexcept;
	QuicStream(const QuicStream& other) noexcept;

	//int id_;
	MsQuicStream* stream_ = nullptr;

	bool receiveData(DataPacket& data);
	bool Send(const uint8_t* buf, uint32_t size);
	bool Send(const char* buf, uint32_t size);

	//bool InitializeSend();
	//bool InitializeReceive();

	QuicStream& operator=(QuicStream&& other) noexcept;


	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_STREAM_CALLBACK)
		QUIC_STATUS StreamCallback(
			_In_ MsQuicStream* Stream,
			_In_opt_ void* Context,
			_Inout_ QUIC_STREAM_EVENT* Event
		);

	bool InitializeSend(const char* id);
private:

	bool brokenStream = false;

	QuicDataReceiver receiver_;
};



