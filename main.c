#include <stdio.h>
#include <stdlib.h>
#include "libavcodec/avcodec.h"

#define READ_SIZE 8192
#define BUFFER_CAPACITY 8192*10

extern AVCodec ff_h264_decoder;

static void yuv_save(unsigned char *buf[], int wrap[], int xsize,int ysize, const char *filename)
{
    FILE *f;
    int i;	

	f = fopen(filename, "w");
	
	for (i = 0; i < ysize; i++) {
		fwrite(buf[0] + i * wrap[0], 1, xsize, f);
	}

	for (i = 0; i < ysize / 2; i++) {
		fwrite(buf[1] + i * wrap[1], 1, xsize/2, f);
	}
	for (i = 0; i < ysize / 2; i++) {
		fwrite(buf[2] + i * wrap[2], 1, xsize/2, f);
	}

    fclose(f);
}


static int decode_write_frame(const char *outfilename, AVCodecContext *avctx,
                              AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;

    len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", *frame_count);
        return len;
    }
    printf("decode return %d\n", len);
    if (got_frame) {
        printf("Got frame %d\n", *frame_count);
		if (outfilename) {
			yuv_save(frame->data, frame->linesize, avctx->width, avctx->height, outfilename);
		}
		(*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}

static void video_decode_example(const char *filename, const char *outfilename)
{
    AVCodec *codec;
    AVCodecContext *c = NULL;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t buffer[BUFFER_CAPACITY];
    int buf_size = 0;
    AVPacket avpkt;
    AVCodecParserContext* parser; 

    av_init_packet(&avpkt);


    printf("Decode video file %s to %s\n", filename, outfilename);

    /* find the mpeg1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
    	// we do not send complete frames
    	printf("we do not send complete frames\n");
        c->flags |= AV_CODEC_FLAG_TRUNCATED;
    } else {
    	printf("we must parse and send complete frames\n");
    }
    
    parser = av_parser_init(AV_CODEC_ID_H264);

	printf("debug point: 1\n");
	
    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
	
    frame_count = 0;
    int need_more = 1;
    for (;;) {
    	if (need_more == 1 && buf_size + READ_SIZE <= BUFFER_CAPACITY) {
    		int bytes_read = fread(buffer + buf_size, 1, READ_SIZE, f);
			if (bytes_read == 0) {
				printf("Read nothing. feof: %d, ferror: %d\n", feof(f), ferror(f));
				break;
			}
			buf_size += bytes_read;
			need_more = 0;
    	}
        
		int bytes_used = av_parser_parse2(parser, c, &avpkt.data, &avpkt.size, buffer, buf_size, 0, 0, AV_NOPTS_VALUE);
		printf("bytes_used: %d\n", bytes_used);
		if (avpkt.size == 0) {
			printf("need more input bytes\n");
			need_more = 1;
			continue;
		}
		if (bytes_used > 0) {
			buf_size = buf_size - bytes_used;
			memcpy(buffer + bytes_used, buffer, buf_size);
		
			// we have a packet, decode it
			while (avpkt.size > 0) {
				if (decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 0) < 0) {
					fprintf(stderr, "decode or write error\n");
					exit(1);
				}
			}
		}
                
        
    }

    /* some codecs, such as MPEG, transmit the I and P frame with a
       latency of one frame. You must do the following to have a
       chance to get the last frame of the video */
    avpkt.data = NULL;
    avpkt.size = 0;
    decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 1);

    fclose(f);

    avcodec_close(c);
    av_free(c);
    av_frame_free(&frame);
    printf("Done\n");
}


int main()
{
	avcodec_register(&ff_h264_decoder);
	video_decode_example("test/352x288Foreman.264", "test/352x288Foreman.yuv");
	return 0;		
}
