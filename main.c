#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "libavcodec/avcodec.h"

#define READ_SIZE 4096
#define BUFFER_CAPACITY 4096*4

extern AVCodec ff_h264_decoder;
extern AVCodecParser ff_h264_parser;

static void yuv_save(unsigned char *buf[], int wrap[], int xsize,int ysize, FILE *f)
{
	int i;	
	for (i = 0; i < ysize; i++) {
		fwrite(buf[0] + i * wrap[0], 1, xsize, f);
	}

	for (i = 0; i < ysize / 2; i++) {
		fwrite(buf[1] + i * wrap[1], 1, xsize/2, f);
	}
	for (i = 0; i < ysize / 2; i++) {
		fwrite(buf[2] + i * wrap[2], 1, xsize/2, f);
	}
}


static int decode_write_frame(FILE *file, AVCodecContext *avctx,
							  AVFrame *frame, int *frame_index, AVPacket *pkt, int flush)
{
	int got_frame = 0;
	do {
		int len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
		if (len < 0) {
			fprintf(stderr, "Error while decoding frame %d\n", *frame_index);
			return len;
		}
		if (got_frame) {
			printf("Got frame %d\n", *frame_index);
			if (file) {
				yuv_save(frame->data, frame->linesize, avctx->width, avctx->height, file);
			}
			(*frame_index)++;
		}
	} while (flush && got_frame);
	return 0;
}

static void h264_video_decode(const char *filename, const char *outfilename)
{
	printf("Decode file '%s' to '%s'\n", filename, outfilename);

	FILE *file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Could not open '%s'\n", filename);
		exit(1);
	}
	
	FILE *outfile = fopen(outfilename, "wb");
	if (!outfile) {
		fprintf(stderr, "Could not open '%s'\n", outfilename);
		exit(1);
	}
	
	avcodec_register(&ff_h264_decoder);
	av_register_codec_parser(&ff_h264_parser);
	
	AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}

	AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	
	AVCodecParserContext* parser = av_parser_init(AV_CODEC_ID_H264);
	if(!parser) {
		fprintf(stderr, "Could not create H264 parser\n");
		exit(1);
	}
	
	if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}

	AVFrame *frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}
	
	int ending = 0;
	int need_more = 1;
	int frame_index = 0;
	int buf_size = 0;
	uint8_t buffer[BUFFER_CAPACITY];
	AVPacket packet;
	
	struct timeval tv_start, tv_end;
	gettimeofday(&tv_start, NULL);
	while (!ending) {
		if (need_more == 1 && buf_size + READ_SIZE <= BUFFER_CAPACITY) {
			int bytes_read = fread(buffer + buf_size, 1, READ_SIZE, file);
			if (bytes_read == 0) {
				ending = 1;
			} else {
				buf_size += bytes_read;
				need_more = 0;
			}
		}
		
		uint8_t* data = NULL;
  		int size = 0;
		int bytes_used = av_parser_parse2(parser, codec_ctx, &data, &size, buffer, buf_size, 0, 0, AV_NOPTS_VALUE);
		if (size == 0) {
			need_more = 1;
			continue;
		}
		if (bytes_used > 0 || ending == 1) {
			// We have data of one packet, decode it; or decode whatever when ending
			av_init_packet(&packet);
			packet.data = data;
			packet.size = size;
			int ret = decode_write_frame(outfile, codec_ctx, frame, &frame_index, &packet, 0);
			if (ret < 0) {
				fprintf(stderr, "Decode or write frame error\n");
				exit(1);
			}
			
			// Move unused data in buffer to front
			buf_size = buf_size - bytes_used;
			memcpy(buffer, buffer + bytes_used, buf_size);
		}		
	}

	// Flush the decoder
	packet.data = NULL;
	packet.size = 0;
	decode_write_frame(outfile, codec_ctx, frame, &frame_index, &packet, 1);

	gettimeofday(&tv_end, NULL);

	fclose(file);
	fclose(outfile);

	avcodec_close(codec_ctx);
	av_free(codec_ctx);
	av_frame_free(&frame);
	printf("Done\n");

	float time = (tv_end.tv_sec + (tv_end.tv_usec / 1000000.0)) - (tv_start.tv_sec + (tv_start.tv_usec / 1000000.0));
	float speed = (frame_index + 1) / time;
	printf("Decoding time: %.3fs, speed: %.1f FPS\n", time, speed);
}


int main(int argc, char* argv[])
{
	if (argc == 1) {
		h264_video_decode("test/352x288Foreman.264", "test/352x288Foreman.yuv");
	} else if (argc == 3) {
		h264_video_decode(argv[1], argv[2]);
	} else {
		printf("Usage: %s input_file output_file\n", argv[0]);
	}
	return 0;		
}
