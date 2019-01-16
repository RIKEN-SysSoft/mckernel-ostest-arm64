#!/bin/sh
# reboot_mck.sh COPYRIGHT FUJITSU LIMITED 2015
this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

usage()
{
  echo "$0 [-c num_cpu] [-m mem_size] [-h] [-n]"
  echo "  options:"
  echo "    -c  number of McKernel cpu core."
  echo "    -m  McKernel memory size.(MiB)"
  echo "    -n  dry run."
  echo "    -h  show usage."
}

num_cpu=
mem_size=
while getopts c:m:nh OPT
do
    case $OPT in
        c)  num_cpu=$OPTARG
            ;;
        m)  mem_size=$OPTARG
            ;;
        n)  dry_run=1
            ;;
        h)  usage; exit 0
            ;;
        \?) usage; exit 1
            ;;
    esac
done

if [ -z "$num_cpu" ]; then
  num_cpu=`grep cpu "$mcos_info" | cut -d\  -f 2`
fi
if [ -z "$mem_size" ]; then
  mem_size=`grep mem "$mcos_info" | cut -d\  -f 2`
fi

mem_size=$(( $mem_size ))

shutdown
boot $num_cpu $mem_size
