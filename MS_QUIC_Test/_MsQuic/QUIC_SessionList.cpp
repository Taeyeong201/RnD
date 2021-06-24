//#include "QUIC_Protocol.h"

//QuicSessionList::QuicSessionList()
//{
//}
//
//int QuicSessionList::newSession(HQUIC streamHandle)
//{
//	std::shared_ptr<QuicStream> aa = std::make_shared<QuicStream>(SessionCount);
//	aa->stream_ = new (std::nothrow) MsQuicStream(streamHandle, CleanUpAutoDelete, QuicStream::StreamCallback, this);
//
//	sessionListMap_.insert(std::make_pair(SessionCount, aa));
//
//	return SessionCount++;
//}


//QuicStream::QuicStream(int id)
//	: id_(id), stream_(nullptr)
//{
//}


//_IRQL_requires_max_(PASSIVE_LEVEL)
//_Function_class_(QUIC_STREAM_CALLBACK)
//QUIC_STATUS QuicStream::StreamCallback(
//	_In_ MsQuicStream* Stream,
//	_In_opt_ void* Context,
//	_Inout_ QUIC_STREAM_EVENT* Event)
//{
//	auto ctx = (QuicSessionList*)Context;
//
//	switch (Event->Type) {
//	case QUIC_STREAM_EVENT_START_COMPLETE:
//		printf("[strm][%p] Start Complete\n", Stream->Handle);
//		if (ctx) {
//			for (auto it : ctx->sessionListMap_) {
//				if (it.second->stream_ == Stream) {
//					it.second->online = true;
//					break;
//				}
//			}
//		}
//		break;
//	case QUIC_STREAM_EVENT_SEND_SHUTDOWN_COMPLETE:
//		printf("[strm][%p] Send Shutdown Complete\n", Stream->Handle);
//		break;
//	case QUIC_STREAM_EVENT_SEND_COMPLETE:
//		//
//		// A previous StreamSend call has completed, and the context is being
//		// returned back to the app.
//		//
//		free(Event->SEND_COMPLETE.ClientContext);
//		printf("[strm][%p] Data sent\n", Stream->Handle);
//		break;
//	case QUIC_STREAM_EVENT_RECEIVE:
//		//
//		// Data was received from the peer on the stream.
//		//
//		printf("[strm][%p] --- Data received\n", Stream->Handle);
//		printf("AbsoluteOffset %llu\n", Event->RECEIVE.AbsoluteOffset);
//		printf("TotalBufferLength %llu\n", Event->RECEIVE.TotalBufferLength);
//		printf("BufferCount %u\n", Event->RECEIVE.BufferCount);
//		printf("Buffers->Length %u\n", Event->RECEIVE.Buffers->Length);
//		printf("Flags %d\n", Event->RECEIVE.Flags);
//
//		//for (uint32_t i = 0; i < Event->RECEIVE.Buffers->Length; i++) {
//		//	printf("%.2X", (uint8_t)Event->RECEIVE.Buffers->Buffer[i]);
//		//}
//		//printf("\n");
//		printf("[strm][%p] --- Data received\n", Stream->Handle);
//
//		break;
//
//	case QUIC_STREAM_EVENT_IDEAL_SEND_BUFFER_SIZE:
//		printf("[strm][%p] Ideal Send Buffer Size(%llu)\n", Stream->Handle, Event->IDEAL_SEND_BUFFER_SIZE.ByteCount);
//
//		break;
//	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
//	{
//		printf("[strm][%p] Peer shut down\n", Stream->Handle);
//		// 정상적인 종료는 Shutdown을 보내줌
//		Stream->Shutdown(0, QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL);
//
//		break;
//	}
//	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
//		//
//		// The peer aborted its send direction of the stream.
//		//
//		printf("[strm][%p] Peer Send aborted, ", Stream->Handle);
//		StatusPrint(Event->PEER_SEND_ABORTED.ErrorCode);
//		Stream->Shutdown(Event->PEER_SEND_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_SEND);
//		//Stream->ConnectionShutdown(Event->PEER_SEND_ABORTED.ErrorCode);
//		break;
//	case QUIC_STREAM_EVENT_PEER_RECEIVE_ABORTED:
//		//
//		// The peer aborted its send direction of the stream.
//		//
//		printf("[strm][%p] Peer Receive aborted, ", Stream->Handle);
//		StatusPrint(Event->PEER_RECEIVE_ABORTED.ErrorCode);
//		Stream->Shutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode, QUIC_STREAM_SHUTDOWN_FLAG_ABORT_RECEIVE);
//		//Stream->ConnectionShutdown(Event->PEER_RECEIVE_ABORTED.ErrorCode);
//		break;
//	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
//		//
//		// Both directions of the stream have been shut down and MsQuic is done
//		// with the stream. It can now be safely cleaned up.
//		//
//		printf("[strm][%p] All done\n", Stream->Handle);
//		if (ctx) {
//			int id = -1;
//			for (auto it : ctx->sessionListMap_) {
//				if (it.second->stream_ == Stream) {
//					it.second->online = false;
//					id = it.second->id_;
//					it.second.reset();
//					break;
//				}
//			}
//			if (id > 0) {
//				ctx->sessionListMap_.erase(id);
//			}
//			ctx->SessionCount--;
//		}
//		//Stream->ConnectionShutdown(0);
//		break;
//	default:
//		printf("[strm][%p] Unknown Event %d\n", Stream->Handle, Event->Type);
//		break;
//	}
//
//	return QUIC_STATUS_SUCCESS;
//}