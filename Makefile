#
# source files
#

SRCS := $(shell find . -name "*.c" -not -name "*_template.c")

CFLAGS = -I. -O2 -w -std=c99 -Wno-implicit-function-declaration
LDFLAGS = -lm

OBJS := $(SRCS:%.c=%.o)

h264dec.bc: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

js: h264dec.bc
	emcc -O2 h264dec.bc --preload-file test/352x288Foreman.264 -o h264dec.html

.PHONY: clean
clean:
	rm -f h264dec.* $(OBJS)