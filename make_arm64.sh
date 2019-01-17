# make_arm64.sh COPYRIGHT FUJITSU LIMITED 2019
#!/bin/sh

. ${HOME}/.mck_test_config

export ARCH=arm64
export LIBIHK_DIR=${MCK_DIR}/lib
export IHKUSR_INCLUDE_DIR=${MCK_DIR}/include
export LIBDIR=${MCK_DIR}/lib

MAKECMD="make"
if [ "$1" == "clean" ]; then
	MAKECMD="make clean"
fi

while read line
do
	pushd ${line}
	${MAKECMD}
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
	cp ${line} ./bin/
done < ./util/bin_list.txt
