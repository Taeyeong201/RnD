#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#include <iostream>
#include <string>

struct EncodeParams
{
	std::string codec;
	uint32_t width;
	uint32_t height;
	uint32_t bitrate_kbps;
	uint32_t framerate;
	uint32_t gop;

	uint8_t monitor_count;
};

class FFEncoder
{
	constexpr static const char* CODEC_LIST[8] = {
	"h264_mf", "h264_qsv", "h264_nvenc", "h264_amf",
	"hevc_mf", "hevc_qsv", "hevc_nvenc", "hevc_amf" };

public:
	FFEncoder();
	virtual bool Init(EncodeParams& params);
	virtual void Destroy();

	virtual bool IsInitialized() const
	{
		return is_initialized_;
	}

	virtual int EncodeFrame(const uint8_t* nv12_image, uint32_t width, uint32_t height,
		uint8_t* out_buf, uint32_t out_buf_size,
		int movCount, int movSrcX, int movSrcY, int movTop, int movBottom, int movLeft, int movRight,
		int dirCount, int dirTop, int dirBottom, int dirLeft, int dirRight);

	virtual int FlushEncoder();
	virtual int FlushEncoder(uint8_t* out_buf);
	virtual bool ResetEncoder(EncodeParams& params);

private:
	static std::string CODEC_NAME;

	int encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt, uint8_t* out_buf);

	void setCodecCtxOpt(EncodeParams& params);
	bool allocateBuf();
	void freeBuf();
	void freeEncoder();

	bool is_initialized_ = false;

	int64_t pts_ = 0;

	const AVCodec* codec_ = nullptr;
	AVCodecContext* codec_ctx_ = nullptr;
	AVPacket* pkt_ = nullptr;
	AVFrame* frame_ = nullptr;
};

