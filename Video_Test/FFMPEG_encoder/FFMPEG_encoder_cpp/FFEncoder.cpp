#include "FFEncoder.h"

std::string FFEncoder::CODEC_NAME = "h264_mf";

FFEncoder::FFEncoder()
{
}
#pragma warning( push ) // 현재의 warning 상태 저장 
#pragma warning( disable : 26812 )
bool FFEncoder::Init(EncodeParams& params)
{
    int ret;

    if (params.codec == "h264") {
        // 0 : mf, 1 : qsv, 2 : nvenc, 3 : amf
        CODEC_NAME = CODEC_LIST[0];
    }
    else {
        CODEC_NAME = CODEC_LIST[4];
    }
    codec_ = avcodec_find_encoder_by_name(CODEC_NAME.c_str());
    codec_ctx_ = avcodec_alloc_context3(codec_);
    if (!codec_ctx_) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return false;
    }

    setCodecCtxOpt(params);

    ret = avcodec_open2(codec_ctx_, codec_, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec: %d", ret);
        return false;
    }

    if (!allocateBuf()) {
        fprintf(stderr, "Could not allocate video packet\n");
        return false;
    }

    frame_->format = codec_ctx_->pix_fmt;
    frame_->width = codec_ctx_->width;
    frame_->height = codec_ctx_->height;

    ret = av_frame_get_buffer(frame_, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        return false;
    }
    is_initialized_ = true;
    return true;
}
#pragma warning( pop ) // 이전의 warning 상태 복원

void FFEncoder::Destroy()
{
    is_initialized_ = false;
    freeBuf();
    freeEncoder();
}

int FFEncoder::EncodeFrame(
    const uint8_t* nv12_image, uint32_t width, uint32_t height, 
    uint8_t* out_buf, uint32_t out_buf_size, 
    int movCount, 
    int movSrcX, int movSrcY, 
    int movTop, int movBottom, int movLeft, int movRight, 
    int dirCount, 
    int dirTop, int dirBottom, int dirLeft, int dirRight)
{
    int ret = 0;
    int y_size = width * height;
    int uv_size = width * height / 2;
    
    frame_->pts = pts_++;

    ret = av_frame_make_writable(frame_);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame\n");
        return -1;
    }


    uint8_t* y_ptr = (uint8_t*)nv12_image;
    uint8_t* uv_ptr = y_ptr + y_size;

    if (memcpy_s(frame_->data[0], y_size, y_ptr, y_size))
    {
        printf("There is a problem transferring the luminance data to NV12 texture.\n");
    }

    if (memcpy_s(frame_->data[1], uv_size, uv_ptr, uv_size))
    {
        printf("There is a problem transferring the luminance data to NV12 texture.\n");
    }

    return encode(codec_ctx_, frame_, pkt_, out_buf);
}

int FFEncoder::FlushEncoder()
{
    int ret;

    ret = avcodec_send_frame(codec_ctx_, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        return -0;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(codec_ctx_, pkt_);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            return -1;
        }
        //memcpy_s(out_buf, pkt->size, pkt->data, pkt->size);
        //printf("Write packet %ld (size=%5d)\n", pkt->pts, pkt->size);
        //fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt_);
    }
    return 0;
}

int FFEncoder::FlushEncoder(uint8_t* out_buf)
{
    int ret;

    ret = avcodec_send_frame(codec_ctx_, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        return -0;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(codec_ctx_, pkt_);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            return -1;
        }
        memcpy_s(out_buf, pkt_->size, pkt_->data, pkt_->size);
        //printf("Write packet %ld (size=%5d)\n", pkt->pts, pkt->size);
        //fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt_);
    }
    return pkt_->size;
}

bool FFEncoder::ResetEncoder(EncodeParams& params)
{
    freeBuf();
    freeEncoder();
    pts_ = 0;
    int ret = 0;
    codec_ctx_ = avcodec_alloc_context3(codec_);
    if (!codec_ctx_) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return false;
    }

    setCodecCtxOpt(params);

    ret = avcodec_open2(codec_ctx_, codec_, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec: %d", ret);
        return false;
    }

    if (allocateBuf()) {
        fprintf(stderr, "Could not allocate video packet\n");
        return false;
    }

    frame_->format = codec_ctx_->pix_fmt;
    frame_->width = codec_ctx_->width;
    frame_->height = codec_ctx_->height;

    ret = av_frame_get_buffer(frame_, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        return false;
    }

    return true;
}

int FFEncoder::encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt, uint8_t* out_buf)
{
    int ret;
    int frame_size = 0;

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        return -2;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            return -3;
        }
        frame_size = pkt->size;
        memcpy_s(out_buf, pkt->size, pkt->data, pkt->size);
        //printf("Write packet %ld (size=%5d)\n", pkt->pts, pkt->size);
        //fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }

    return frame_size;
}

void FFEncoder::setCodecCtxOpt(EncodeParams& params)
{
    codec_ctx_->flags |= AV_CODEC_FLAG_LOW_DELAY;
    //codec_ctx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    codec_ctx_->flags |= AV_CODEC_FLAG_CLOSED_GOP;
    codec_ctx_->flags2 |= AV_CODEC_FLAG2_FAST;
    //codec_ctx_->flags2 |= AV_CODEC_FLAG2_CHUNKS;


    codec_ctx_->bit_rate = 8 * 1000 * 1000 * 2; // 2mb
    //codec_ctx_->bit_rate_tolerance = 512 * 1024 * 2;
    codec_ctx_->gop_size = 10;
    codec_ctx_->width = params.width;
    codec_ctx_->height = params.height;
    codec_ctx_->pix_fmt = AV_PIX_FMT_NV12;

    codec_ctx_->time_base = AVRational{ 1, (int)24 };
    codec_ctx_->framerate = AVRational{ (int)24, 1 };

    codec_ctx_->thread_count = 1;
    //codec_ctx_->global_quality = 51;
    //codec_ctx_->compression_level = 0;
    codec_ctx_->profile = FF_PROFILE_H264_HIGH;
    codec_ctx_->level = 4;
    //av_opt_set(codec_ctx_->priv_data, "preset", "fast", 0);
    av_opt_set(codec_ctx_->priv_data, "profile", "main", 0);
    //av_opt_set(codec_ctx_->priv_data, "tune", "zerolatency", 0);
    //av_opt_set(codec_ctx_->priv_data, "preset", "veryslow", 0);
}

bool FFEncoder::allocateBuf()
{
    pkt_ = av_packet_alloc();
    if (!pkt_) return false;

    frame_ = av_frame_alloc();
    if (!frame_) return false;

    return true;
}

void FFEncoder::freeBuf()
{
    av_frame_free(&frame_);
    frame_ = nullptr;
    av_packet_free(&pkt_);
    pkt_ = nullptr;
}

void FFEncoder::freeEncoder()
{
    avcodec_free_context(&codec_ctx_);
    codec_ctx_ = nullptr;
}

