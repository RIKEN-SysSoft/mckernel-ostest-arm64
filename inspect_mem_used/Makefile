# Makefile COPYRIGHT FUJITSU LIMITED 2016
TARGET := inspect_mem_used

CC := $(CROSS_COMPILE)gcc

CFLAGS += -g
CFLAGS += -O2
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -funsigned-char
CPPFLAGS += -D_GNU_SOURCE
LDFLAGS  += -lpthread


default:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) a.c -o $(TARGET)

clean:
	rm -f a.o $(TARGET) *~
