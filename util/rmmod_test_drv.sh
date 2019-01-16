#!/bin/sh
# rmmod_test_drv.sh COPYRIGHT FUJITSU LIMITED 2015
E=

this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

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
rm -rf "$test_mck_device_dir"
echo "remove $test_mck_device_dir"

#
# rmmod
#
rmmod "$test_mck_ko_name"
echo "rmmod $test_mck_ko"
