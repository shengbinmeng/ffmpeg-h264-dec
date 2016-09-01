SRCS := $(shell find ../ffmpeg-src/libavcodec -name "*.c" -not -name "*_template.c")
SRCS += $(shell find ../ffmpeg-src/libavutil -name "*.c" -not -name "*_template.c")
SRCS += ../decoder.c

CFLAGS := -I../ffmpeg-src/ -O2 -w -std=c99 -Wno-implicit-function-declaration
LDFLAGS := -lm

OBJS := $(SRCS:%.c=%.o)

ARGS := -O3 --memory-init-file 0 --llvm-opts 3 --llvm-lto 3 \
		-s EXPORTED_FUNCTIONS="['HEAP8', 'HEAP16', 'HEAP32', '_broadwayGetMajorVersion', '_broadwayGetMinorVersion', '_broadwayInit', '_broadwayExit', '_broadwayCreateStream', '_broadwayPlayStream', '_broadwayOnHeadersDecoded', '_broadwayOnPictureDecoded']" \
		-s NO_EXIT_RUNTIME=1 -s NO_FILESYSTEM=1 -s NO_BROWSER=1 -s INVOKE_RUN=0 -s DOUBLE_MODE=0 -s PRECISE_I64_MATH=0 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s ALIASING_FUNCTION_POINTERS=1 -s DISABLE_EXCEPTION_CATCHING=1 \
		--js-library library.js \
		--pre-js DecoderPre.js \
		--post-js DecoderPost.js

Decoder.js: $(OBJS)
	emcc $(ARGS) -o $@ $^
	
.PHONY: clean
clean:
	rm -f Decoder.js $(OBJS)