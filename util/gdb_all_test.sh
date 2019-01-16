#!/bin/sh
# gdb_all_test.sh COPYRIGHT FUJITSU LIMITED 2016
this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

#
# default value
#
mck_max_cpus=`cat /proc/cpuinfo | grep -c "processor"`
h_num_cpu_def=`expr $mck_max_cpus - 1`
m_num_cpu_def="all"
total_mem=`free -m | grep Mem: | awk '{print $2}'`
mem_size_def=$(( $total_mem -  ($total_mem * (100 - 45) / 100)))

usage()
{
  echo "$0 [-a h_num_cpu] [-c m_num_cpu] [-m mem_size] [-h] [-n]"
  echo "  options:"
  echo "    -a  number of host cpu core."
  echo "    -c  number of McKernel cpu core."
  echo "    -i  interval of logmode"
  echo "    -k  logmode"
  echo "    -m  McKernel memory size.(MiB)"
  echo "    -n  dry run."
  echo "    -h  show usage."
}

#
# check path
#
if [ ! -f "${mcexec}" ]; then
  echo "mcexec not found.(${mcexec})"
  exit 1
fi
if [ ! -f "${gdb}" ]; then
  echo "gdb not found.(${gdb})"
  exit 1
fi
if [ ! -f "${gdb_mcexec_sh}" ]; then
  echo "gdb_mcexec.sh not found.(${gdb_mcexec_sh})"
  exit 1
fi
if [ ! -f "${gdb_test_case_list}" ]; then
  echo "test case list not found.(${gdb_test_case_list})"
  exit 1
fi

h_num_cpu=$h_num_cpu_def
m_num_cpu=$m_num_cpu_def
mem_size=$mem_size_def
enable_mcoverlay=
while getopts a:c:i:k:m:noh OPT
do
  case $OPT in
    a)  h_num_cpu=$OPTARG
        ;;
    c)  m_num_cpu=$OPTARG
        ;;
    i)  INTERVAL=${OPTARG}
        expr "${INTERVAL}" + 1 > /dev/null 2>&1
        if [ $? -ge 2 ]; then
          echo "invalid -i value"
          exit 1
        fi
        if [ ${INTERVAL} -le 0 ]; then
          echo "invalid -i value"
          exit 1
        fi
        ;;
    k)  LOGMODE=${OPTARG}
        expr "${LOGMODE}" + 1 > /dev/null 2>&1
        if [ $? -ge 2 ]; then
          echo "invalid -k value"
          exit 1
        fi
        if [ ${LOGMODE} -lt 0 -o ${LOGMODE} -gt 2 ]; then
          echo "invalid -k value"
          exit 1
        fi
        ;;
    m)  mem_size=$OPTARG
        ;;
    n)  dry_run=1
        ;;
    o)  enable_mcoverlay="yes"
        ;;
    h)  usage; exit 0
        ;;
  \?) usage; exit 1
      ;;
  esac
done

#### console output setting ####
orig_printk_setting=`cat /proc/sys/kernel/printk`
#echo "set 4 4 1 7 => /proc/sys/kernel/printk"
echo "4 4 1 7" > /proc/sys/kernel/printk

if [ -n "$enable_mcoverlay" ]; then
  release=`uname -r`
  major=`echo ${release} | sed -e 's/^\([0-9]*\).*/\1/'`
  minor=`echo ${release} | sed -e 's/^[0-9]*.\([0-9]*\).*/\1/'`
  patch=`echo ${release} | sed -e 's/^[0-9]*.[0-9]*.\([0-9]*\).*/\1/'`
  linux_version_code=`expr \( ${major} \* 65536 \) + \( ${minor} \* 256 \) + ${patch}`
  rhel_release=`echo ${release} | sed -e 's/^[0-9]*.[0-9]*.[0-9]*-\([0-9]*\).*/\1/'`

  if [ "${release}" == "${rhel_release}" ]; then
    rhel_release=""
  fi

  if [ ${linux_version_code} -ge 262144 ] && [ ${linux_version_code} -lt 262400 ]; then
    enable_mcoverlay="yes"
  else
    enable_mcoverlay=
  fi
fi

mem_size=$(( $mem_size ))

field_num=$(( `echo "$h_num_cpu" | awk -F ',' '{print NF}'` ))
h_cpu_list=0
p_num_cpu=
for i in `seq 1 $field_num`
do
  field=`echo "$h_num_cpu" | cut -d ',' -f $i`
  echo "$field" | grep -q "^[0-9]*$"
  if [ $? -eq 0 ]; then
    h_cpu_list=$(( $h_cpu_list | (1 << $field) ))
  else
    top=$(( `echo "$field" | cut -d '-' -f 1` ))
    btm=$(( `echo "$field" | cut -d '-' -f 2` ))
    for j in `seq $top $btm`
    do
      h_cpu_list=$(( $h_cpu_list | (1 << j) ))
    done
  fi
done
max_h_num_cpu=$(( `cat /proc/cpuinfo | grep -c "processor"` - 1 ))
for i in `seq 0 $max_h_num_cpu`
do
  if [ $(( ($h_cpu_list >> $i) & 1 )) -eq 0 ]; then
    p_num_cpu="${p_num_cpu}${i},"
  fi
done
p_num_cpu=`echo "$p_num_cpu" | sed 's/,$//'`

reserve "$p_num_cpu" $mem_size
max_chunk_size=0
if [ $dry_run -eq 0 ]; then
  old_ifs="$IFS"
  IFS=$'\n';
  chunks=`grep "IHK-SMP: chunk" "$smp_resource_info"`
  for chunk in $chunks
  do
    size=`echo $chunk | sed -E 's/^\[.*\][\t ]+//g' | cut -d\  -f 7 | sed 's/)//g'`
    if [ $max_chunk_size -lt $size ]; then
      max_chunk_size=$size
    fi
  done
  max_chunk_size=$(( $max_chunk_size / $MiB ))
  IFS="$old_ifs"
else
  max_chunk_size="(memory-size)"
fi
if [ "$m_num_cpu" = "all" ]; then
  m_num_cpu="$p_num_cpu"
fi
boot "$m_num_cpu" $max_chunk_size


###### Run GDB test cases ###################################
cd $gdb_dir
while read exp
do
	if [[ ${#exp} -eq 0 ]]; then
		continue
	fi

	echo $exp | grep -v '#.*' > /dev/null
	if [[ $? -eq 0 ]]; then
    echo "###### Start  testcase:${exp}"
		make check RUNTESTFLAGS="--verbose GDB=${gdb_mcexec_sh} gdb.base/${exp}.exp"
    echo "###### Finish testcase:${exp}"

		#### reboot McKernel ####
		shutdown
		boot "$m_num_cpu" $max_chunk_size
	fi
done < ${gdb_test_case_list}

#### shutdown McKernel ####
shutdown
release

#### console output setting restore ####
#echo "restore $orig_printk_setting => /proc/sys/kernel/printk"
echo $orig_printk_setting > /proc/sys/kernel/printk

