#include "decoder.h"
#include "libavcodec/avcodec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern AVCodec ff_h264_decoder;
extern AVCodecParser ff_h264_parser;

static AVCodecContext *codec_ctx;
static AVFrame *frame;
static AVPacket packet;
static AVCodecParserContext* parser;
static u8* yuv_data;

static u32 picDecodeNumber;
static u32 picDisplayNumber;
static u32 picSize;

typedef struct {
    u32 length;
    u8 *buffer;
    u8 *pos;
    u8 *end;
} Stream;

static Stream broadwayStream;

static void streamInit(Stream *stream, u32 length) {
    stream->buffer = stream->pos = (u8 *)malloc(sizeof(u8) * length);
    stream->length = length;
    stream->end = stream->buffer + length;
}

static void outputFrame (AVFrame *frame) {
    int width = frame->width, height = frame->height;
    int linesize_y = frame->linesize[0], linesize_uv = frame->linesize[1];
    if (yuv_data == NULL) {
        yuv_data = malloc(width * height * 3 / 2);
    }
    u8 *y = yuv_data, *u = y + height * width, *v = u + height/2 * width/2;
    for (int i = 0; i < height; i++) {
        memcpy(y + i * width, frame->data[0] + i * linesize_y, width);
    }
    for (int i = 0; i < height / 2; i++) {
        memcpy(u + i * width / 2, frame->data[1] + i * linesize_uv, width / 2);
    }
    for (int i = 0; i < height / 2; i++) {
        memcpy(v + i * width / 2, frame->data[2] + i * linesize_uv, width / 2);
    }
    printf("Output frame %d\n", picDisplayNumber);
    broadwayOnPictureDecoded(yuv_data, width, height);
}

static int decodePacket(AVPacket *packet, int flush) {
    int got_frame = 0;
    do {
        int len = avcodec_decode_video2(codec_ctx, frame, &got_frame, packet);
        if (len < 0) {
            fprintf(stderr, "Error while decoding frame %d\n", picDecodeNumber);
            return len;
        }
        if (got_frame) {
            printf("Got frame %d\n", picDisplayNumber);
            outputFrame(frame);
            picDisplayNumber++;
        }
    } while (flush && got_frame);
}

static void playStream(Stream *stream) {
    
    uint8_t *buffer = stream->buffer;
    int buf_size = stream->length;

    while (buf_size > 0) {
        uint8_t* data = NULL;
        int size = 0;
        int bytes_used = av_parser_parse2(parser, codec_ctx, &data, &size, buffer, buf_size, 0, 0, AV_NOPTS_VALUE);
        //printf("bytes_used: %d, size: %d, buf_size: %d\n", bytes_used, size, buf_size);
        if (size == 0) {
            if (bytes_used == buf_size) {
                // This is the last chunk of data, so we try to parse again.
                av_parser_parse2(parser, codec_ctx, &data, &size, buffer, buf_size, 0, 0, AV_NOPTS_VALUE);
            }
        }
        if (bytes_used > 0) {
            // We have data of one packet, decode it; or decode whatever when ending.
            av_init_packet(&packet);
            packet.data = data;
            packet.size = size;
            int len = decodePacket(&packet, 0);
            if (len < 0) {
                fprintf(stderr, "Error while decoding frame %d\n", picDecodeNumber);
            }
            picDecodeNumber++;
            
            buffer += bytes_used;
            buf_size -= bytes_used;
        }
    }
    
    // Flush
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    decodePacket(&packet, 1);
}

static void decodeAccessUnit(Stream *stream) {
    av_init_packet(&packet);
    packet.data = stream->buffer;
    packet.size = stream->length;
    int len = decodePacket(&packet, 1);
    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", picDecodeNumber);
    }
    picDecodeNumber++;
}

u32 broadwayInit() {

    avcodec_register(&ff_h264_decoder);
    av_register_codec_parser(&ff_h264_parser);
    
    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    
    parser = av_parser_init(AV_CODEC_ID_H264);
    if(!parser) {
        fprintf(stderr, "Could not create H264 parser\n");
        exit(1);
    }
    
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }


    picDecodeNumber = picDisplayNumber = 1;
    yuv_data = NULL;
    
    return 0;
}

u8* broadwayCreateStream(u32 length) {
    streamInit(&broadwayStream, length);
    return broadwayStream.buffer;
}

void broadwayPlayStream(u32 length) {
    broadwayStream.length = length;
    playStream(&broadwayStream);
}

void broadwayExit() {
    if (broadwayStream.buffer) {
        free(broadwayStream.buffer);
    }
    
    avcodec_close(codec_ctx);
    av_free(codec_ctx);
    av_frame_free(&frame);
    if (yuv_data != NULL) {
        free(yuv_data);
    }
}

u32 broadwayGetMajorVersion() {
    return 1;
}

u32 broadwayGetMinorVersion() {
    return 1;
}
