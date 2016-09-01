#!/bin/bash

#
# read the configure help carefully before you want to change the following options
#
./configure \
    --disable-asm \
    --disable-pthreads \
    --disable-doc \
    --disable-encoders \
    --disable-muxers \
    --disable-decoders \
    --enable-decoder=h264 \
    --disable-parsers \
    --enable-parser=h264 \
    --disable-demuxers \
    --enable-demuxer=h264 \
    --disable-filters \
    --disable-programs \
    --enable-ffmpeg \
