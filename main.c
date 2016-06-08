#include <stdio.h>
#include <stdlib.h>
#include "libavcodec/avcodec.h"

int main()
{
	FILE *in_file = fopen("test/352x288Foreman.264", "rb");
	if (!in_file) {
		printf("Open input file error\n");
		return -1;
	}
	
	FILE *out_file = fopen("test/352x288Foreman.yuv", "wb");
	if (!out_file) {
		printf("Open output file error\n");
	}
 
 	AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);;
 	if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }
    struct AVCodecContext *c = avcodec_alloc_context3(codec); 
	
	if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }
	
    struct AVFrame *frame = av_frame_alloc();
	if (!frame) {
		return -1;
	}

	char* buf = (char*) calloc(1000000, sizeof(char));
	int nal_len = 0;
	int got_picture = 0;
	while (!feof(in_file)) {
		int consumed_bytes = h264_decode_frame(c, frame, &got_picture, buf, nal_len); 
		if (consumed_bytes > 0) {
			printf("consumed %d bytes\n", consumed_bytes);
			if (!out_file) {
				int i;
				for (i=0; i<c->height; i++) {
					fwrite(frame->data[0] + i * frame->linesize[0], 1, c->width, out_file);
				}
				for (i=0; i<c->height/2; i++) {
					fwrite(frame->data[1] + i * frame->linesize[1], 1, c->width/2, out_file);
				}
				for (i=0; i<c->height/2; i++) {
					fwrite(frame->data[2] + i * frame->linesize[2], 1, c->width/2, out_file);
				}
			}
		}
	}

	return 0;		
}
