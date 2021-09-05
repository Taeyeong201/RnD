
#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include <libavdevice/avdevice.h>
};

#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER

int main(int argc, char* argv[])
{
	AVOutputFormat *ofmt = NULL;
	//Input AVFormatContext and Output AVFormatContext
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVCodec *encoder;
	AVCodecContext *encoder_ctx;
	AVPacket pkt;
	const char *in_filename, *out_filename;
	unsigned int ret, i;
	int videoindex = -1;
	int frame_index = 0;
	int64_t start_time = 0;
	//in_filename  = "cuc_ieschool.mov";
	//in_filename  = "cuc_ieschool.mkv";
	//in_filename  = "cuc_ieschool.ts";
	//in_filename  = "cuc_ieschool.mp4";
	//in_filename  = "cuc_ieschool.h264";
	in_filename = "output.h264";
	//in_filename  = "shanghai03_p.h264";

	//out_filename = "rtmp://localhost/publishlive/livestream";//URL£¨Output URL£©[RTMP]
	out_filename = "rtp://192.168.0.25:5006";//URL£¨Output URL£©[UDP]

	//Network
	avformat_network_init();
	//Input
	if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		printf("Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		goto end;
	}

	//for (i = 0; i < ifmt_ctx->nb_streams; i++)
	//	if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
	//		videoindex = i;
	//		break;
	//	}
	ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &encoder, 0);
	if (ret < 0) {
		fprintf(stderr, "Cannot find a video stream in the input file\n");
		goto end;
	}
	videoindex = ret;

	if (!(encoder_ctx = avcodec_alloc_context3(encoder))) {
		ret = AVERROR(ENOMEM);
		goto end;
	}

	av_dump_format(ifmt_ctx, 0, in_filename, 0);

	//Output
	avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtp", out_filename); //RTMP
	//avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP

	if (!ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	ofmt = ofmt_ctx->oformat;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		//Create output AVStream according to input AVStream
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVCodecParameters *in_codecpar = in_stream->codecpar;

		AVStream *out_stream = avformat_new_stream(ofmt_ctx, encoder);
		if (!out_stream) {
			printf("Failed allocating output stream\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		//Copy the settings of AVCodecContext
		ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
		if (ret < 0) {
			fprintf(stderr, "Failed to copy codec parameters\n");
			goto end;
		}
		out_stream->codecpar->codec_tag = 0;
	}

	//Dump Format------------------
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	//Open output URL
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			printf("Could not open output URL '%s'", out_filename);
			goto end;
		}
	}
	//Write file header
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		printf("Error occurred when opening output URL\n");
		goto end;
	}

	start_time = av_gettime();
	while (1) {
		AVStream *in_stream, *out_stream;
		//Get an AVPacket
		ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
			break;
		//FIX£ºNo PTS (Example: Raw H.264)
		//Simple Write PTS
		if (pkt.pts == AV_NOPTS_VALUE) {
			//Write PTS
			AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
			//Duration between 2 frames (us)
			int64_t calc_duration = (int64_t)((double)AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate));
			//Parameters
			pkt.pts = (int64_t)((double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE));
			pkt.dts = pkt.pts;
			pkt.duration = (int64_t)((double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE));
		}
		//Important:Delay
		if (pkt.stream_index == videoindex) {
			AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
			AVRational time_base_q = { 1,AV_TIME_BASE };
			int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
			int64_t now_time = av_gettime() - start_time;
			if (pts_time > now_time)
				av_usleep((unsigned int)(pts_time - now_time));

		}

		in_stream = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];
		/* copy packet */
		//Convert PTS/DTS
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		//Print to Screen
		if (pkt.stream_index == videoindex) {
			printf("Send %8d video frames to output URL\n", frame_index);
			frame_index++;
		}
		//ret = av_write_frame(ofmt_ctx, &pkt);
		ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

		if (ret < 0) {
			printf("Error muxing packet\n");
			break;
		}

		av_packet_unref(&pkt);

	}
	//Write file trailer
	av_write_trailer(ofmt_ctx);
end:
	avformat_close_input(&ifmt_ctx);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred.\n");
		return -1;
	}
	return 0;
}