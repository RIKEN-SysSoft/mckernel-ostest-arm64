# Makefile COPYRIGHT FUJITSU LIMITED 2015
RM=rm
CFLAGS += -g
CFLAGS += -O0
CFLAGS += -Wall

default:
	${AS} -o hello_world.o hello_world.S
	${AS} -o goto_user.o goto_user.S
	${AS} -o exit_group.o exit_group.S
	${LD} -o hello_world -e _start hello_world.o
	${LD} -o goto_user -e _start goto_user.o
	${LD} -o exit_group -e _start exit_group.o
	${CC} ${CFLAGS} glibc_hello_world.c -o glibc_hello_world

clean:
	${RM} -f hello_world hello_world.o
	${RM} -f goto_user goto_user.o
	${RM} -f exit_group exit_group.o
	${RM} -f glibc_hello_world
