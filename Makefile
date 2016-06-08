#
# source files
#

SRCS := $(shell find . -name "*.c" -not -name "*_template.c")

CFLAGS = -I. -O2 -w -std=c99
LDFLAGS = -lm

OBJS := $(SRCS:%.c=%.o)

h264dec : $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f h264dec $(OBJS)
