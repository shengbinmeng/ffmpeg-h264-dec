SRCS := $(shell find . -name "*.c" -not -name "*_template.c")

CFLAGS := -I./ffmpeg-src -O2 -w -std=c99 -Wno-implicit-function-declaration
LDFLAGS := -lm

OBJS := $(SRCS:%.c=%.o)

h264dec: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# The following targets are expected to be used with Emscripten (by calling `emmake make`).

h264dec.bc: $(OBJS)
	emcc -o $@ $^ $(LDFLAGS)

h264dec.html: h264dec.bc
	emcc -O2 h264dec.bc --preload-file test/352x288Foreman.264 -o h264dec.html
	
h264dec.node.js: h264dec.bc
	emcc -O2 h264dec.bc --embed-file test/352x288Foreman.264 -o h264dec.node.js

.PHONY: clean
clean:
	rm -f h264dec h264dec.* $(OBJS)