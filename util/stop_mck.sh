#!/bin/sh
# stop_mck.sh COPYRIGHT FUJITSU LIMITED 2015
this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

usage()
{
  echo "$0 [-h] [-n]"
  echo "  options:"
  echo "    -n  dry run."
  echo "    -h  show usage."
  echo "    -m  no unload mcctrl.ko & ihk.ko"
}

mcctrl_no_unload=
while getopts nhm OPT
do
    case $OPT in
        n)  dry_run=1
            ;;
        h)  usage; exit 0
            ;;
        m)  mcctrl_no_unload="yes"
            ;;
        \?) usage; exit 1
            ;;
    esac
done

shutdown
release
