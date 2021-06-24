#pragma once

#include <thread>
#include <functional>

#include "QUIC_Common.h"

#include "blockingconcurrentqueue.h"

using namespace moodycamel;

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

private:
	void shutdownGetData();
	bool remainingPacket(uint8_t* packetBuf, uint32_t remainPacketSize);

	BlockingConcurrentQueue<DataPacket> queue_;
	BlockingConcurrentQueue<DataPacket> remainQueue_;
};

