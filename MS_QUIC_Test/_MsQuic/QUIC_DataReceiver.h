#pragma once

#include <thread>
#include <functional>

#include <deque>
#include <mutex>

#include "QUIC_Common.h"

#define QUEUE 0

#if QUEUE
#include "blockingconcurrentqueue.h"

using namespace moodycamel;
#endif


struct DataPacket {
	~DataPacket();

	unsigned int size = 0;
	std::shared_ptr<unsigned char> data;
};

struct DataPayload {
	uint32_t size;
	uint8_t* buf;
};

class QuicDataReceiver
{
public:
	QuicDataReceiver();

	void queueBuffer(uint8_t* buffer, unsigned int size);
	bool getData(DataPacket& data);

	bool stopRecv = false;

	void shutdownGetDataFunc();
private:
	bool remainingPacket(uint8_t* packetBuf, uint32_t remainPacketSize);

#if QUEUE
	BlockingConcurrentQueue<DataPacket> queue_;
	BlockingConcurrentQueue<DataPacket> remainQueue_;
#else
	std::deque<DataPacket> deque_;
	std::mutex recvMutex;
#endif
};

