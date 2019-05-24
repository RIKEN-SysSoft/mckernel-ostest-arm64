#!/bin/sh

if [ "$1" == "" ]; then
    list=data/linux/ostest-testlist
else
    list=$1
fi

while read line
do
    sudo bash -c "AUTOTEST_HOME=/work/mcktest bash data/linux/script/$line -H"
done < $list

