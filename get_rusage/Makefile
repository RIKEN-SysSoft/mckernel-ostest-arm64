# Makefile COPYRIGHT FUJITSU LIMITED 2017
CPPFLAGS=
CFLAGS=-g -O0 -Wall
CFLAGS_PIE=$(CFLAGS) -fPIE -pie
LDFLAGS=-lihk

default:
	$(CC) $(CFLAGS) -L$(LIBIHK_DIR) -I$(IHKUSR_INCLUDE_DIR) \
	$(LDFLAGS) get_rusage.c -o get_rusage -Wl,-rpath=$(LIBDIR)

clean:
	rm -f get_rusage
