#!/bin/sh

if [ "$1" == "" ]; then
    list=/work/mcktest/data/ostest-testlist
else
    list=$1
fi

while read line
do
    AUTOTEST_HOME=/work/mcktest bash -x data/linux/script/$line -H
done < $list
