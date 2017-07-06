this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

# test file
temp=$this_dir/tempfile
link=/tmp/templink
mmapfile_name=$this_dir/mmapfile
ostype_name=$this_dir/ostype
org_pid_max=/proc/sys/kernel/pid_max
pid_max_name=$this_dir/pid_max
cpuinfo_path=/proc/mcos0/cpuinfo
lv07_tmp=$this_dir/lv07.txt
lv07_tmp_before=$this_dir/lv07_before.txt

# ostype string
ostype_str="Linux"

# loop counter
count=0

# test user
test_user_name="mck_test"

# for siginfo TP send signal.
signal_name="HUP INT QUIT ILL TRAP ABRT EMT FPE KILL BUS SEGV SYS PIPE ALRM TERM URG STOP TSTP CONT CHLD TTIN TTOU IO XCPU XFSZ VTALRM PROF WINCH PWR USR1 USR2"

siginfo_send_signal() {
  local pid=$1
  for sig in $signal_name ; do
    if [ ${sig} == "KILL" ] ; then
      continue
    elif [ ${sig} == "STOP" ] ; then
      continue
    elif [ ${sig} == "TSTP" ] ; then
      continue
    elif [ ${sig} == "CONT" ] ; then
      continue
    elif [ ${sig} == "CHLD" ] ; then
      continue
    elif [ ${sig} == "TTIN" ] ; then
      continue
    elif [ ${sig} == "TTOU" ] ; then
      continue
    fi
    echo -e "send SIG${sig} to ${pid}"
    kill -${sig} ${pid}
    sleep 1
  done
}

# parse parameter
usage()
{
  echo "$0 [-e] [-p <presetnum>] [-s <separationnum>] [-d] [-h]"
  echo "  options:"
  echo "    -H  Generate Linux reference results"
  echo "    -e  use execve regression."
  echo "    -p  use NUMA-preset setting."
  echo "      1: 4 nodes, 512 MiB separation"
  echo "      2: 4 nodes,   1 GiB separation"
  echo "      3: 4 nodes,   2 GiB separation"
  echo "      4: 4 nodes,   3 GiB separation"
  echo "      5: 4 nodes,   4 GiB separation"
  echo "    -s  separation running number."
  echo "      0 or none: run all."
  echo "      1-10     : on the way running."
  echo "      any      : setting nothing, print usage."
  echo "    -d  Print the commands that would be executed, but do not execute them"
  echo "    -h  show usage."
}

# option check
execve_comm=
execve_arg_end=
mck_max_mem_size=
mck_max_node_mem_size=
boot_numa_opt=""
sep_run_num=0
linux_run="no"
pidof_mcexec="pidof mcexec"
DRYRUN=
DRYRUN_QUOTE=
while getopts Hep:s:dh OPT
do
  case $OPT in
    H)
      mcexec=""
      linux_run="yes"
      pidof_mcexec="pidof test_mck"
      trap ":" USR1
      ;;
    e)
      execve_comm="${app_dir}/test_mck -s execve -n 1 -- -f"
      execve_arg_end="--"
      ;;
    p)
      boot_numa_opt="-p $OPTARG"
      ;;
    s)
      sep_run_num=$OPTARG
      if [ $sep_run_num -lt 0 -o 10 -lt $sep_run_num ]; then
        usage
        exit 1
      fi
      ;;
    d)
      # Don't show commands surrounding mcexec and
      # supress thier side-effects
      DRYRUN=":"
      DRYRUN_QUOTE="'"
      mcexec='echo ${mcexec} '
      trap ":" USR1

      # Switch kill target
      pidof_mcexec="pidof test_mck"

      # Evaluate variables with environment dependent values at run-time
      app_dir='${app_dir}'
      mck_max_cpus='${mck_max_cpus}'
      mck_max_mem_size='${mck_max_mem_size}'
      mck_max_mem_size_95p='${mck_max_mem_size_95p}'
      mck_max_mem_size_110p='${mck_max_mem_size_110p}'
      mck_ap_num='${mck_ap_num}'
      mck_ap_num_even='${mck_ap_num_even}'
      gid='${gid}'
      uid='${uid}'

      this_dir='${this_dir}'
      temp='${temp}'
      link='${link}'
      mmapfile_name='${mmapfile_name}'
      ostype_name='${ostype_name}'
      org_pid_max='${org_pid_max}'
      pid_max_name='${pid_max_name}'
      cpuinfo_path='${cpuinfo_path}'
      lv07_tmp='${lv07_tmp}'
      lv07_tmp_before='${lv07_tmp_before}'
      ;;
    h)
      usage
      exit 0
      ;;
  esac
done
shift `expr $OPTIND - 1`

host_core=0
mck_core=""
ikc_map=""
max_core=0
min_core=`cat /proc/cpuinfo | grep processor | cut -d ' ' -f 2 | head -1`
lines=`cat /proc/cpuinfo | grep processor | cut -d ' ' -f 2`
while read line
do
	if [ $max_core -lt $line ]; then
		max_core=$line
	fi

	if [ $min_core -gt $line ]; then
		min_core=$line
	fi
done << END
$lines
END

# check full configuration
mck_max_cpus=`cat /proc/cpuinfo | grep -c "processor"`
if [ "$linux_run" == "no" ]; then
if [ $max_core -eq 59 ]; then
	host_core="0-3"
	mck_core="12-59"
	ikc_map="12-23:0+24-35:1+36-47:2+48-59:3"
	mck_max_cpus=`expr $mck_max_cpus - 3`
elif [ $max_core -eq 51 ]; then
	host_core="0,13,26,39"
	mck_core="1-12,14-25,27-38,40-51"
	ikc_map="1-12:0+14-25:13+27-38:26+40-51:39"
	mck_max_cpus=`expr $mck_max_cpus - 3`
elif [ $max_core -eq 15 ]; then
	host_core="0-3"
	mck_core="4-15"
	ikc_map="4,8,12:0+5,9,13:1+6,10,14:2+7,11,15:3"
	mck_max_cpus=`expr $mck_max_cpus - 3`
else
	host_core=$min_core
	mck_core="1-${max_core}"
	ikc_map="1-${max_core}:0"
fi
fi

# get mck ap num
mck_ap_num=`expr $mck_max_cpus - 1`
mck_ap_num_even=$mck_ap_num

if [ `expr $mck_ap_num_even % 2` != 0 ]; then
  mck_ap_num_even=`expr $mck_ap_num_even - 1`
fi

# run regression
#while :
#do
	if [ "$DRYRUN" != ":" ]; then
	#### initialize ####
	addusr=0
	useradd $test_user_name
	if [ "$?" -eq 0 ]; then
		uid=`id -u $test_user_name`
		gid=`id -g $test_user_name`
		addusr=1
	else
		uid=1000
		gid=1050
	fi
	echo "use uid:$uid gid:$gid"

	echo a > $mmapfile_name
	dd if=/dev/zero of=${temp} bs=1M count=10
	ln -s ${temp} ${link}

	echo $ostype_str > $ostype_name
	cat $org_pid_max > $pid_max_name

	#### console output setting ####
	orig_printk_setting=`cat /proc/sys/kernel/printk`
	echo "set 4 4 1 7 => /proc/sys/kernel/printk"
	echo "4 4 1 7" > /proc/sys/kernel/printk

	#### host output corefile-name setting ####
	orig_core_pattern=`cat /proc/sys/kernel/core_pattern`
	echo "set core.host.%p => /proc/sys/kernel/core_pattern"
	echo "core.host.%p" > /proc/sys/kernel/core_pattern

	#### host output corefile limitsize setting ####
	orig_core_rlimit=`ulimit -S -c`
	echo "set ulimit -S -c 0"
	ulimit -S -c 0
	fi

	if [ "$DRYRUN" != ":" ] && [ "$linux_run" == "no" ]; then
	#### boot McKernel ####
	sh "$shutdown_mck_sh"
	echo "boot McKernel, HOST core is #$host_core"
	sh $run_mck_sh -a $host_core $boot_numa_opt -v -r $ikc_map
	fi

	if [ "$linux_run" == "no" ]; then
	#### get McKernel memory size ####
	echo "get McKernel memory size."
	query_mem_str=`"$ihkosctl" 0 query mem`
	mck_max_mem_size=`echo $query_mem_str | sed -e 's/@[0-9]*,/ + /g' | sed -e 's/@[0-9]*$//g' | xargs expr`
	mck_max_mem_size_95p=`expr $mck_max_mem_size / 20`
	mck_max_mem_size_110p=`expr $mck_max_mem_size_95p \* 22`
	mck_max_mem_size_95p=`expr $mck_max_mem_size_95p \* 19`

	query_mem_sep_str=`echo $query_mem_str | sed -e 's/@[0-9]*,/,/g' | sed -e 's/@[0-9]*$//g'`
	mck_max_node_mem_size=`echo $query_mem_sep_str | cut -d ',' -f 1`
	query_mem_sep_str=`echo $query_mem_sep_str | sed -e 's/,/ /g'`
	for i in $query_mem_sep_str
	do
		if [ $mck_max_node_mem_size -lt $i ]; then
			mck_max_node_mem_size=$i
		fi
	done
	echo "mck_max_mem_size     :$mck_max_mem_size"
	echo "mck_max_node_mem_size:$mck_max_node_mem_size"
	else
		#### get Linux memory size ####
		echo "get total usable RAM and the max among NUMA-nodes"
		memtotals=`find /sys/devices/system/node/ -name meminfo | xargs -r grep MemTotal | awk '{ print $4 * 1024; }'`
		mck_max_mem_size=`echo $memtotals | perl -ne 'foreach $memtotal (split) { $sum += $memtotal; } print $sum;'`
		mck_max_mem_size_95p=`expr $mck_max_mem_size / 20`
		mck_max_mem_size_110p=`expr $mck_max_mem_size_95p \* 22`
		mck_max_mem_size_95p=`expr $mck_max_mem_size_95p \* 19`
	
		mck_max_node_mem_size=-1
		for i in $memtotals
		do
			if [ $mck_max_node_mem_size -lt $i ]; then
				mck_max_node_mem_size=$i
			fi
		done
		echo "mck_max_mem_size     :$mck_max_mem_size"
		echo "mck_max_node_mem_size:$mck_max_node_mem_size"
	fi

	#### insmod test driver ####
	if [ "$DRYRUN" != ":" ]; then
	echo "insmod test_drv"
	sh "$insmod_test_drv_sh"
	fi
