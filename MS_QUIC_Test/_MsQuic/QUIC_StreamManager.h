#pragma once

#include "QUIC_Common.h"
#include "QUIC_Stream.h"

#include <unordered_map>
class QuicFramework;

class QuicStreamManager {
	friend QuicFramework;
public:
	QuicStreamManager();

	// Client Only
	template <typename String, typename ...Strings>
	void initStreamName(String name, Strings&&... strings);

	void NewStream(const char* name);
	void DeleteStream(const char* name);

	bool WaitForCreateStream();
	bool WaitForCreateConnection();

	void SetConnection(MsQuicConnection* connection);

	std::vector<std::string> getStreamList();

	QuicStream* operator[](const char* key);
private:
	static
		_IRQL_requires_max_(PASSIVE_LEVEL)
		_Function_class_(QUIC_STREAM_CALLBACK)
		QUIC_STATUS TempStreamCallback(
			_In_ MsQuicStream* Stream,
			_In_opt_ void* Context,
			_Inout_ QUIC_STREAM_EVENT* Event
		);

	void initStreamName() {};

	// Framework Use
	void EventPeerStreamStarted(HQUIC handle);
	void EventConnected(MsQuicConnection* _connection);
	void DeleteAllStream();

	// Stream Use
	void SetPeerStreamName(const char* name, std::shared_ptr<QuicStream> stream);
	std::shared_ptr<QuicStream> findTempStream(MsQuicStream* nativeStream);

	std::unordered_map<std::string, std::shared_ptr<QuicStream>> streamMap_;
	std::vector<std::shared_ptr<QuicStream>> tmpStreams;

	std::mutex mutex_;
	std::condition_variable cv_;

	uint8_t streamCount = 0;

	bool brokenConnection = false;

	// Stream 생성시 사용
	MsQuicConnection* connection_ = nullptr;
};

template<typename String, typename ...Strings>
inline void QuicStreamManager::initStreamName(String name, Strings && ...strings)
{
	if (streamMap_.count(name) > 0)
		PLOG_WARNING << "\"" << name << "\" Already Same Name! Override QuicStream";

	streamMap_[name] = std::make_shared<QuicStream>(name);

	initStreamName(strings...);
}