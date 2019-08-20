#!/bin/sh
# rmmod_test_drv.sh COPYRIGHT FUJITSU LIMITED 2015

if [ -z $AUTOTEST_HOME ]; then
	# Invoked from <ostest>/util/run_testset_postk.sh
	this_dir="$(cd $(dirname $0); pwd)"
else
	# Invoked from autotest
	this_dir=${AUTOTEST_HOME}/ostest/util
fi

# E and test_mck_ko are defined in <ostest>/util/config

usage()
{
  echo "$0[-h]"
  echo "  options:"
  echo "    -h  show usage."
}

while getopts i:h OPT
do
    case $OPT in
        h)  usage; exit 0
            ;;
        \?) usage; exit 1
            ;;
    esac
done

#
# delete device files
#
$E rm -rf "$test_mck_device_dir"
echo "remove $test_mck_device_dir"

#
# rmmod
#
$E rmmod "$test_mck_ko_name"
echo "rmmod $test_mck_ko"
