#!/bin/sh
# insmod_test_drv.sh COPYRIGHT FUJITSU LIMITED 2015

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
  echo "$0 [-i ident] [-h]"
  echo "  options:"
  echo "    -i  identifier for the syslog."
  echo "    -h  show usage."
}

ident=
while getopts i:h OPT
do
    case $OPT in
        i)  ident=$OPTARG
            ;;
        h)  usage; exit 0
            ;;
        \?) usage; exit 1
            ;;
    esac
done

#
# check path
#
if [ ! -d "$app_dir" ]; then
  echo "test app directory not found.($app_dir)"
  exit 1
fi

#
# insmod
#
$E insmod "$test_mck_ko" ident="$ident"
echo "insmod $test_mck_ko"

#
# create device files
#
devnos=`cat "$test_mck_proc_devno"`
devno_ents=`echo $devnos | awk '{print NF}'`

names=`cat "$test_mck_proc_tp_names"`
names_ents=`echo $names | awk '{print NF}'`

$E mkdir -p "$test_mck_device_dir"
for i in `seq 1 $devno_ents`
do
	maj=`echo $devnos | cut -d' ' -f$i | sed 's/:[0-9]*//g'`
	min=`echo $devnos | cut -d' ' -f$i | sed 's/[0-9]*://g'`
	if [ $i -gt  $names_ents ]; then
		devname="test_mck$min"
	else
		devname=`echo $names | cut -d' ' -f$i`
	fi
	path="$test_mck_device_dir/$devname"
	$E mknod "$path" c $maj $min
	$E chown `id -u`:`id -g` "$path"
	echo "create charcter device $path($maj:$min)"
done
