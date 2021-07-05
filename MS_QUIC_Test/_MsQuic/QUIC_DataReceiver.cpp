#include "QUIC_DataReceiver.h"

#include <iostream>

//BlockingConcurrentQueue<DataPacket> QuicDataReceiver::queue_(100);
//bool QuicDataReceiver::stopRecv = false;

QuicDataReceiver::QuicDataReceiver()
#if QUEUE
	: queue_(300), remainQueue_(10)
#endif
{
}
unsigned long long GetMicroCounter123()
{
	unsigned long long Counter = 0;

#if defined(_WIN32)
	unsigned long long Frequency = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
	Counter = 1000000 * Counter / Frequency;
#elif defined(__linux__)
	struct timeval t;
	gettimeofday(&t, 0);
	Counter = 1000000 * t.tv_sec + t.tv_usec;
#endif

	return Counter;
}
bool QuicDataReceiver::getData(DataPacket& data)
{
	DataPacket packet = { 0, };
	DataPacket remainPacket = { 0, };
	unsigned long long start, end;
	static int asd = 0;

	if (!stopRecv) {
#if QUEUE
		if (remainQueue_.try_dequeue(packet));
		else
			queue_.wait_dequeue(packet);
#else
		while (deque_.empty()) Sleep(1);

		recvMutex.lock();
		packet = deque_.front();
		deque_.pop_front();
		recvMutex.unlock();
#endif
		if (!packet.data) 
			return false;

		// Packet Size가 Payload Size 보다 작을때
		if (packet.size < sizeof(DataPayload)) {
#if QUEUE
			queue_.wait_dequeue(remainPacket);
#else
			while (deque_.empty()) Sleep(1);

			recvMutex.lock();
			remainPacket = deque_.front();
			deque_.pop_front();
			recvMutex.unlock();
#endif
			auto tmp = std::shared_ptr<unsigned char>(
				new unsigned char[packet.size + remainPacket.size],
				[](unsigned char* ptr) { delete[]ptr; }
			);
			memcpy(tmp.get(), packet.data.get(), packet.size);
			memcpy(tmp.get() + packet.size, remainPacket.data.get(), remainPacket.size);
			packet.data.swap(tmp);
			packet.size += remainPacket.size;
		}

		auto payload = (DataPayload*)packet.data.get();
		auto payloadBuf = (uint8_t*)packet.data.get() + sizeof(DataPayload);
	
		data.size = payload->size;
		data.data = std::shared_ptr<unsigned char>(
			new unsigned char[payload->size],
			[](unsigned char* ptr) { delete[]ptr; }
		);
		memset(data.data.get(), 0, payload->size);

		//payload의 DataPayload 크기 뺀 packet size
		uint32_t totalRecvSize = packet.size - sizeof(DataPayload);
		uint32_t bufferPos = packet.size - sizeof(DataPayload);

		//PacketSize와 Payload Size가 일치할때
		if (totalRecvSize == payload->size) {
			memcpy(data.data.get(), payloadBuf, payload->size);
		}
		else {
			//PakcetSize가 PayloadSize보다 작을때 나머지 데이터를 받기
			if (totalRecvSize < payload->size) {
				memcpy(data.data.get(), (uint8_t*)payloadBuf, totalRecvSize);

				while (true) {
#if QUEUE
					queue_.wait_dequeue(remainPacket);
#else
					while (deque_.empty()) Sleep(1);

					recvMutex.lock();
					remainPacket = deque_.front();
					deque_.pop_front();
					recvMutex.unlock();
#endif
					if (!remainPacket.data) 
						return false;

					totalRecvSize += remainPacket.size;

					if (totalRecvSize < payload->size) {
						memcpy(data.data.get() + bufferPos, remainPacket.data.get(), remainPacket.size);
						bufferPos += remainPacket.size;
						t5++;
					}
					else if (totalRecvSize > payload->size) {

						auto remainPacketSize = totalRecvSize - payload->size;
						auto remainPayloadSize = remainPacket.size - remainPacketSize;

						memcpy(data.data.get() + bufferPos, (uint8_t*)remainPacket.data.get(), remainPayloadSize);

						remainingPacket(remainPacket.data.get() + remainPayloadSize, remainPacketSize);
						break;
					}
					else {
						memcpy(data.data.get() + bufferPos, remainPacket.data.get(), remainPacket.size);
						break;
					}
				}
			}
			// totalRecvSize > payload->size
			// PacketSize가 PayloadSize보다 크면 뒤에 다른 데이터가 붙어 들어온 것으로 짤라냄
			else {
				memcpy(data.data.get(), (uint8_t*)payloadBuf, payload->size);

				auto remainPacketSize = totalRecvSize - payload->size;
				remainingPacket(payloadBuf + payload->size, remainPacketSize);
			}
		}

		//remainPacket.data.reset();
		//packet.data.reset();
		return true;
	}
	return false;
}

bool QuicDataReceiver::remainingPacket(uint8_t* packetBuf, uint32_t remainPacketSize)
{
	//Payload 이외 데이터를 나머지 Queue 삽입
	DataPacket remainPacket;

	remainPacket.size = remainPacketSize;
	remainPacket.data = std::shared_ptr<unsigned char>(
		new unsigned char[remainPacketSize],
		[](unsigned char* ptr) { delete[]ptr; }
	);
	memcpy(remainPacket.data.get(), packetBuf, remainPacketSize);
#if QUEUE
	remainQueue_.enqueue(std::move(remainPacket));
#else
	recvMutex.lock();
	deque_.push_front(std::move(remainPacket));
	recvMutex.unlock();
#endif
	return true;
}

void QuicDataReceiver::queueBuffer(uint8_t* buffer, unsigned int size)
{
	DataPacket packet = { 0, };
	packet.data = std::shared_ptr<unsigned char>(
		new unsigned char[size],
		[](unsigned char* ptr) { delete[]ptr; }
	);
	packet.size = size;

	memcpy(packet.data.get(), buffer, size);

#if QUEUE
	queue_.enqueue(std::move(packet));
#else
	recvMutex.lock();
	deque_.push_back(std::move(packet));
	recvMutex.unlock();
#endif
	//if (queue_.size_approx() > 30)
	//	std::cout << "warning recv queue overflow" << std::endl;
}

void QuicDataReceiver::shutdownGetData()
{
	DataPacket packet = { 0, };

#if QUEUE
	queue_.enqueue(std::move(packet));
#else
	recvMutex.lock();
	deque_.push_back(std::move(packet));
	recvMutex.unlock();
#endif
}


DataPacket::~DataPacket()
{
	if (data) {
		data.reset();
	}
}
