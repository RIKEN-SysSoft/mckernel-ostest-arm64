# Makefile COPYRIGHT FUJITSU LIMITED 2015-2017
obj-m += test_mck.o
test_mck-objs := test_mck_drv.o test_mck_drv-mmap_dev.o test_mck_drv-mmap_dev2.o test_mck_drv-writecombine.o

#TEST_MCK_KERNEL_SOURCE ?= /lib/modules/$(shell uname -r)/build
TEST_MCK_KERNEL_SOURCE ?= /usr/src/kernels/4.18.0-32.el8.aarch64+debug

all:
	make -C $(TEST_MCK_KERNEL_SOURCE) M=$(PWD) modules

clean:
	make -C $(TEST_MCK_KERNEL_SOURCE) M=$(PWD) clean

