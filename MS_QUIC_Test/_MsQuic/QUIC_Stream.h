#pragma once

#include "QUIC_Common.h"

#include "QUIC_DataReceiver.h"

class QuicStream {
public:
	//QuicStream(int id);
	QuicStream() noexcept;
	QuicStream(std::string id) noexcept;
	//QuicStream(const QuicStream& other) noexcept;

	//int id_;
	MsQuicStream* stream_ = nullptr;

	bool InitializeSend(const char* id);

	bool isBroken();
	bool StopStream();

	bool receiveData(DataPacket& data);
	bool Send(const uint8_t* buf, uint32_t size);
	bool Send(const char* buf, uint32_t size);

	void RegistDeleteCallback(std::function<void(const char*)> callback);

	//bool InitializeSend();
	//bool InitializeReceive();

	//QuicStream& operator=(QuicStream&& other) noexcept;

	//std::function<> 

	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_STREAM_CALLBACK)
		QUIC_STATUS StreamCallback(
			_In_ MsQuicStream* Stream,
			_In_opt_ void* Context,
			_Inout_ QUIC_STREAM_EVENT* Event
		);


	std::string id_;
private:

	bool brokenStream = false;

	QuicDataReceiver receiver_;
	std::function<void(const char*)> deleteCallback;
};



