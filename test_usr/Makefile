# Makefile COPYRIGHT FUJITSU LIMITED 2015
CPPFLAGS=
CFLAGS=-g -O0 -Wall
CFLAGS_PIE=$(CFLAGS) -fPIE -pie
LDFLAGS=

default:
	$(CC) $(CFLAGS) a.c -o test_usr
	$(CC) $(CFLAGS_PIE) a.c -o test_usr_pie

clean:
	rm -f test_usr test_usr_pie
