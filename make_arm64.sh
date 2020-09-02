# make_arm64.sh COPYRIGHT FUJITSU LIMITED 2019
#!/bin/sh

MCK_DIR=/work/mcktest/work/mck

export ARCH=$(case $(uname -p) in aarch64) echo arm64; ;; x86_64) echo x86; ;; esac)
export LIBIHK_DIR=${MCK_DIR}/lib64
export IHKUSR_INCLUDE_DIR=${MCK_DIR}/include
export LIBDIR=${MCK_DIR}/lib64

MAKECMD="make"
MAKEOPT="-j"

page_size=`getconf PAGE_SIZE`
case $page_size in
    65536)
	MAKEOPT+=" CPPFLAGS=\"-DCONFIG_ARM64_64K_PAGES=1\""
	;;
    4096)
	;;
    *)
	echo "$(basename $0): ERROR: unsupported page size ($page_size)"
	exit 1
	;;
esac

if [ "$1" == "clean" ]; then
	MAKECMD="make clean"
fi

while read line
do
	[[ $ARCH == x86 ]] && [[ $line =~ "test_usr" ]] && continue
	pushd ${line}

	ADDITIONALOPT=
	[[ -e Makefile.$ARCH ]] && ADDITIONALOPT="-f Makefile.$ARCH"

	$MAKECMD $MAKEOPT $ADDITIONALOPT
	if [ $? != 0 ]; then
		exit 1
	fi
	popd
done < ./util/tp_list.txt

if [ "$1" == "clean" ]; then
	rm -f ./bin/*
	exit 0
fi

if [ ! -d ./bin ]; then
    mkdir bin
fi

while read line
do
	[[ $ARCH == x86 ]] && [[ $line =~ "test_usr" ]] && continue
	cp ${line} ./bin/
done < ./util/bin_list.txt

for i in init exec fini check; do
    install -d bin/$i
    install util/$i/*.sh bin/$i/
done
