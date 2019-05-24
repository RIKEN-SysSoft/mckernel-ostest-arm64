num_system_service_cpus()
{
	num_cpus=`lscpu | awk -F ': *' '/^CPU[(]s[)]/ {print $2}'`

	if [ $num_cpus -eq 56 ]; then
		host_core="0,14,28,42"
		ikc_map="1-13:0+15-27:14+29-41:28+43-55:42"
	elif [ $num_cpus -eq 48 ]; then
		host_core="0,12,24,36"
		ikc_map="1-11:0+13-23:12+25-35:24+37-47:36"
	elif [ $num_cpus -eq 16 ]; then
		host_core="0,8"
		ikc_map="1-7:0+9-15:8"
	else
		host_core="0"
		ikc_map="1-${num_cpus}:$host_core"
	fi
	host_core_num=`echo $host_core | perl -ne '@cpus = split /,/; $ncpus = $#cpus + 1; print $ncpus . "\n";'`
}

if [ -z $AUTOTEST_HOME ]; then
	# Invoked from <ostest>/util/run_testset_postk.sh
	utildir=`pwd`
else
	# Invoked from autotest
	utildir=${AUTOTEST_HOME}/ostest/util
fi

# Define ihkosctl etc.
. ${utildir}/config

export TEST_HOME=${TEST_HOME:-$app_dir} # run_rt_*.sh could define this

temp=${recorddir}/tempfile
link=${recorddir}/templink
mmapfile_name=${recorddir}/mmapfile
ostype_name=${recorddir}/ostype
org_pid_max=/proc/sys/kernel/pid_max
pid_max_name=${recorddir}/pid_max
lv07_tmp=${recorddir}/lv07.txt
lv07_tmp_before=${recorddir}/lv07_before.txt

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
  echo "    -H  Generate Linux outputs"
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
binfmt_prefix_comm=
mck_max_mem_size=
mck_max_node_mem_size=
boot_numa_opt=""
sep_run_num=0
linux_run="no"
pidof_mcexec="pidof mcexec"
DRYRUN=
DRYRUN_WAIT=
linux_exec=

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
      binfmt_prefix_comm="$execve_comm"
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
      DRYRUN_WAIT="wait"
      mcexec='echo ${mcexec} '
      linux_exec='echo '
      trap ":" USR1

      # Switch kill target
      pidof_mcexec="pidof test_mck"

      # Evaluate variables with environment dependent values at run-time
      app_dir='${app_dir}'

      # We don't use execve_comm and execve_arg_end in autotest

      binfmt_prefix_comm='${binfmt_prefix_comm}'

      mck_max_cpus='${mck_max_cpus}'
      getaff_cpus='${getaff_cpus}'
      mck_ap_num='${mck_ap_num}'
      mck_ap_num_even='${mck_ap_num_even}'

      mck_max_mem_size='${mck_max_mem_size}'
      mck_max_mem_size_95p='${mck_max_mem_size_95p}'
      mck_max_mem_size_110p='${mck_max_mem_size_110p}'

      gid='${gid}'
      uid='${uid}'

      temp='${temp}'
      link='${link}'
      mmapfile_name='${mmapfile_name}'
      ostype_name='${ostype_name}'

      org_pid_max='${org_pid_max}'
      pid_max_name='${pid_max_name}'

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

# Boot McKernel when invoked from <ostest>/util/run_testset_postk.sh
if [ -z $AUTOTEST_HOME ]; then
	if [ "$DRYRUN" != ":" ] && [ "$linux_run" == "no" ]; then
	sh "$shutdown_mck_sh"
	echo "boot McKernel, HOST core is #$host_core"
	sh $run_mck_sh -a $host_core $boot_numa_opt -v -r $ikc_map
	fi
fi

# For tests using the same number of CPUs for both McKernel and Linux cases

num_system_service_cpus
num_app_cpus=$(($num_cpus - $host_core_num))

# For tests filling all CPUs

if [ "$linux_run" == "yes" ]; then
	host_core=""
	ikc_map=""
	host_core_num=0
else
	num_system_service_cpus
fi

if [ "$DRYRUN" != ":" ]; then
	mck_max_cpus=`expr $num_cpus - $host_core_num`

	getaff_cpus=`expr $mck_max_cpus + 5`

	mck_ap_num=`expr $mck_max_cpus - 1`
	mck_ap_num_even=$mck_ap_num
	if [ `expr $mck_ap_num_even % 2` != 0 ]; then
	mck_ap_num_even=`expr $mck_ap_num_even - 1`
	fi
fi

# run regression
#while :
#do
	if [ "$DRYRUN" != ":" ]; then
	#### initialize ####
	addusr=0
	$E useradd $test_user_name
	if [ "$?" -eq 0 ]; then
		uid=`id -u $test_user_name`
		gid=`id -g $test_user_name`
		addusr=1
	else
		uid=1000
		gid=1050
	fi
	echo "use uid:$uid gid:$gid"

	#### console output setting ####
	orig_printk_setting=`cat /proc/sys/kernel/printk`
	echo "set 4 4 1 7 => /proc/sys/kernel/printk"
	$E bash -c "echo '4 4 1 7' > /proc/sys/kernel/printk"

	#### host output corefile-name setting ####
	orig_core_pattern=`cat /proc/sys/kernel/core_pattern`
	echo "set core.host.%p => /proc/sys/kernel/core_pattern"
	$E bash -c "echo 'core.host.%p' > /proc/sys/kernel/core_pattern"

	#### host output corefile limitsize setting ####
	orig_core_rlimit=`ulimit -S -c`
	echo "set ulimit -S -c 0"
	ulimit -S -c 0
	fi

	# resolve memory size
	if [ "$DRYRUN" != ":" ]; then
	if [ "$linux_run" == "no" ]; then
	echo "get McKernel memory size."
	query_mem_str=`"$ihkosctl" 0 query mem`
	mck_max_mem_size=`echo $query_mem_str | sed -e 's/@[0-9]*,/ + /g' | sed -e 's/@[0-9]*$//g' | xargs expr`
	mck_max_mem_size_5p=`expr $mck_max_mem_size / 20`
	mck_max_mem_size_110p=`expr $mck_max_mem_size_5p \* 22`
	mck_max_mem_size_95p=`expr $mck_max_mem_size_5p \* 19`

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
		echo "get total usable RAM and the max among NUMA-nodes"
		memtotals=`find /sys/devices/system/node/ -name meminfo | xargs -r grep MemTotal | awk '{ print $4 * 1024; }'`
		mck_max_mem_size=`echo $memtotals | perl -ne 'foreach $memtotal (split) { $sum += $memtotal; } print $sum;'`
		mck_max_mem_size_5p=`expr $mck_max_mem_size / 20`
		mck_max_mem_size_110p=`expr $mck_max_mem_size_5p \* 22`
		mck_max_mem_size_95p=`expr $mck_max_mem_size_5p \* 19`
	
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
	fi

	#### insmod test driver ####
	if [ "$DRYRUN" != ":" ]; then
	echo "insmod test_drv"
	sh "$insmod_test_drv_sh"
	fi

	if [ "$DRYRUN" != ":" ]; then
		echo a > $mmapfile_name
		dd if=/dev/zero of=${temp} bs=1M count=10
		ln -s ${temp} ${link}

		echo $ostype_str > $ostype_name
		cat $org_pid_max > $pid_max_name
	fi