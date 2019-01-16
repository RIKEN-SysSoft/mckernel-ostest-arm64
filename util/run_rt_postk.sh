#!/bin/sh
# run_rt_postk.sh COPYRIGHT FUJITSU LIMITED 2015-2017

this_dir="$(cd $(dirname $0); pwd)"
source ${this_dir}/config

export TEST_HOME=$app_base

usage()
{
  echo "$0 [-n|-e|-b] [-p <presetnum>] [-s <separationnum>] [-h]"
  echo "  options:"
  echo "    -n (normal RT)"
  echo "    -e (execve used RT)"
  echo "    -b (both RT)"
  echo "    -p use NUMA-preset setting."
  echo "      1: 4 nodes, 512 MiB separation"
  echo "      2: 4 nodes,   1 GiB separation"
  echo "      3: 4 nodes,   2 GiB separation"
  echo "      4: 4 nodes,   3 GiB separation"
  echo "      5: 4 nodes,   4 GiB separation"
  echo "    -s separation running number."
  echo "      0 or none: run all."
  echo "      1-10     : on the way running."
  echo "      any      : setting nothing, print usage."
  echo "    -h show usage."
}

on_execve=0
on_normal=0
numa_opt=""
sep_opt=""
testset_shell="run_testset_postk.sh"
while getopts nebp:s:qth OPT
do
  case $OPT in
    n)
      on_normal=1
      ;;
    e)
      on_execve=1
      ;;
    b)
      on_normal=1
      on_execve=1
      ;;
    p)
      numa_opt="-p $OPTARG"
      ;;
    s)
      sep_opt="-s $OPTARG"
      ;;
    h)
      usage
      exit 0
      ;;
    *)
      usage
      exit 1
      ;;
  esac
done

if [ $on_normal -eq 0 -a $on_execve -eq 0 ]; then
  usage
  exit 1
fi

if [ $on_normal -eq 1 ]; then
  sh ${this_dir}/${testset_shell} $numa_opt $sep_opt
fi

if [ $on_execve -eq 1 ]; then
  sh ${this_dir}/${testset_shell} -e $numa_opt $sep_opt
fi
