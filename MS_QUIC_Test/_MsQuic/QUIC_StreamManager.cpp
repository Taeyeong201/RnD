#pragma warning(disable: 28182)

#include "QUIC_StreamManager.h"

QuicStreamManager::QuicStreamManager()
{
}

void QuicStreamManager::DeleteAllStream()
{
	streamMap_.clear();
	brokenConnection = true;
	streamCount = 0;
}

bool QuicStreamManager::WaitForCreateStream()
{
	PLOG_INFO << "Waiting for create stream";

	std::unique_lock<std::mutex> lk(mutex_);
	cv_.wait(lk, [&] {return (streamMap_.size() > streamCount) || brokenConnection; });
	lk.unlock();


	if (brokenConnection) {
		return false;
	}
	streamCount++;

	return true;
}

bool QuicStreamManager::WaitForCreateConnection()
{
	PLOG_INFO << "Waiting for create connection";

	std::unique_lock<std::mutex> lk(mutex_);
	cv_.wait(lk, [&] {return (connection_ != nullptr) || brokenConnection; });
	lk.unlock();
	if (brokenConnection)
		return false;

	return true;
}

std::vector<std::string> QuicStreamManager::getStreamList()
{
	std::vector<std::string> tmp;
	for (auto it = streamMap_.begin(); it != streamMap_.end(); it++) {
		tmp.push_back(it->first);
	}

	return std::move(tmp);
}

void QuicStreamManager::SetStreamName(const char* name, std::shared_ptr<QuicStream> stream)
{
	std::string streamName(name);

	if (streamMap_.count(streamName.c_str()) > 0)
		PLOG_WARNING << "\"" << streamName.c_str() << "\" Already Same Name! Override QuicStream";

	stream->stream_->Callback = QuicStream::StreamCallback;
	stream->stream_->Context = stream.get();
	{
		std::lock_guard<std::mutex> lg(mutex_);
		streamMap_[streamName.c_str()] = stream;
	}

	//removeTempStream(stream);
	cv_.notify_one();
}

void QuicStreamManager::EventPeerStreamStarted(HQUIC handle)
{
	std::unique_ptr<QuicStream> tmp = std::make_unique<QuicStream>();
	tmp->stream_ = new(std::nothrow) MsQuicStream(
		handle,
		CleanUpAutoDelete,
		QuicStreamManager::TempStreamCallback,
		this);

	//tmp->stream_ = new(std::nothrow) MsQuicStream(
	//	handle,
	//	CleanUpAutoDelete,
	//	QuicStream::StreamCallback,
	//	tmp.get());
	tmpStreams.push_back(std::move(tmp));
}

void QuicStreamManager::EventConnected(MsQuicConnection* _connection)
{
	{
		std::lock_guard<std::mutex> lg(mutex_);
		this->connection_ = _connection;
	}
	if (streamMap_.size() > 0)
		for (auto it = streamMap_.begin(); it != streamMap_.end(); it++) {
			it->second->stream_ =
				new(std::nothrow) MsQuicStream(
					*connection_,
					QUIC_STREAM_OPEN_FLAG_NONE,
					CleanUpAutoDelete,
					QuicStream::StreamCallback,
					it->second.get()
				);
			it->second->InitializeSend(it->first.c_str());
		}

	cv_.notify_one();
}

std::shared_ptr<QuicStream> QuicStreamManager::findTempStream(MsQuicStream* nativeStream)
{
	for (std::vector<int>::size_type i = 0; i < tmpStreams.size(); i++) {
		if (tmpStreams[i]->stream_ == nativeStream) {
			auto tmp = std::move(tmpStreams[i]);
			tmpStreams.erase(tmpStreams.begin() + i);
			return std::move(tmp);
		}
	}
	return nullptr;
}

QuicStream* QuicStreamManager::operator[](const char* key)
{
	if (streamMap_.count(key) > 0)
		return streamMap_[key].get();
	else
		return nullptr;
}


_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_STREAM_CALLBACK)
QUIC_STATUS QuicStreamManager::TempStreamCallback(
	_In_ MsQuicStream* Stream,
	_In_opt_ void* Context,
	_Inout_ QUIC_STREAM_EVENT* Event)
{
	auto managerCtx = (QuicStreamManager*)Context;
	auto streamCtx = managerCtx->findTempStream(Stream);
	char buf[1024] = { 0, };

	if (streamCtx == nullptr) {
		PLOG_ERROR.printf("[strm][%p] not all the same", Stream->Handle);
		return QUIC_STATUS_OUT_OF_MEMORY;
	}
	static uint64_t nowCall = 0, lastCall = 0;

	switch (Event->Type) {
	case QUIC_STREAM_EVENT_START_COMPLETE:
		PLOG_INFO.printf("[strm][%p] Start Complete", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_SEND_SHUTDOWN_COMPLETE:
		PLOG_INFO.printf("[strm][%p] Send Shutdown Complete", Stream->Handle);
		break;
	case QUIC_STREAM_EVENT_SEND_COMPLETE:
		//
		// A previous StreamSend call has completed, and the context is being
		// returned back to the app.
		//
		free(Event->SEND_COMPLETE.ClientContext);
		break;
	case QUIC_STREAM_EVENT_RECEIVE:
	{
		//
		// Data was received from the peer on the stream.
		//
		PLOG_INFO.printf("[strm][%p] QUIC_STREAM_EVENT_RECEIVE", Stream->Handle);

		auto payload = (DataPayload*)Event->RECEIVE.Buffers[0].Buffer;
		auto payloadBuf = (uint8_t*)Event->RECEIVE.Buffers[0].Buffer + sizeof(DataPayload);
		memcpy(buf, payloadBuf, payload->size);

		managerCtx->SetStreamName(buf, std::move(streamCtx));
	}
	break;
	case QUIC_STREAM_EVENT_IDEAL_SEND_BUFFER_SIZE:
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
		PLOG_INFO.printf("[strm][%p] Peer shut down", Stream->Handle);
		// 정상적인 종료는 Shutdown을 보내줌
		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL);
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		PLOG_INFO.printf("[strm][%p] Peer Send aborted[0x%x], %s",
			Stream->Handle, Event->PEER_SEND_ABORTED.ErrorCode,
			StatusPrint(Event->PEER_SEND_ABORTED.ErrorCode)
		);
		Stream->Shutdown(Event->PEER_SEND_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_SEND);
		break;
	case QUIC_STREAM_EVENT_PEER_RECEIVE_ABORTED:
		//
		// The peer aborted its send direction of the stream.
		//
		PLOG_INFO.printf("[strm][%p] Peer Receive aborted[0x%x], %s",
			Stream->Handle, Event->PEER_SEND_ABORTED.ErrorCode,
			StatusPrint(Event->PEER_RECEIVE_ABORTED.ErrorCode)
		);
		Stream->Shutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_RECEIVE);
		break;
	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
		//
		// Both directions of the stream have been shut down and MsQuic is done
		// with the stream. It can now be safely cleaned up.
		//
		PLOG_INFO.printf("[strm][%p] All done", Stream->Handle);

		for (std::vector<int>::size_type i = 0; i < managerCtx->tmpStreams.size(); i++) {
			if (managerCtx->tmpStreams[i] == streamCtx) {
				managerCtx->tmpStreams.erase(managerCtx->tmpStreams.begin() + i);
				break;
			}
		}
		break;
	default:
		PLOG_INFO.printf("[strm][%p] Unknown Event %d", Stream->Handle, Event->Type);
		break;
	}

	return QUIC_STATUS_SUCCESS;
}