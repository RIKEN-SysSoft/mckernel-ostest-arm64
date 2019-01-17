#!/bin/sh
# run_mck.sh COPYRIGHT FUJITSU LIMITED 2015
this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

#
# default value
#
h_num_cpu_def="0,1"
m_num_cpu_def="all"
total_mem=`free -m | grep Mem: | awk '{print $2}'`
mem_size_def=$(( $total_mem -  ($total_mem * (100 - 45) / 100)))

usage()
{
	echo "$0 [-a h_num_cpu] [-i interval] [-k logmode] [-o owner_name]"
	echo "   [-m mem_size] [-f facility] [-v] [-n] [-p NUMA-preset num] [-h]"
	echo "  options:"
	echo "    -f  facility of logmode.(default=LOG_LOCAL6)"
	echo "    -o  /dev/mcd*, /dev/mcos* owner name.(default=\`logname\`)"
	echo "    -k  logmode."
	echo "    -a  number of host cpu core."
	echo "    -m  McKernel memory size.(MiB)"
	echo "    -r  ikc_map setting.(<mck-cores>:<host-core>+<mck-cores>:<host-core>...)"
	echo "          default=<mck-cores>:0"
	echo "    -q  ihk_start_irq.(default=60)"
	echo "    -i  interval of logmode.(Seconds, default=1)"
	echo "    -w  ihk_nr_irq.(default=4, (1 < x <= 32))"
	echo "    -v  mcoverlay enable."
	echo "    -n  dry run."
	echo "    -p  use NUMA-preset setting. (-m option is ignore)"
	echo "          1 = 4 nodes, 512 MiB separation"
	echo "          2 = 4 nodes,   1 GiB separation"
	echo "          3 = 4 nodes,   2 GiB separation"
	echo "          4 = 4 nodes,   3 GiB separation"
	echo "          5 = 4 nodes,   4 GiB separation"
	echo "    -h  show usage."
}

h_num_cpu=$h_num_cpu_def
m_num_cpu=$m_num_cpu_def
mem_size=$mem_size_def
enable_mcoverlay=
ikc_map=""
ihk_irq="60"
nr_irq="4"
while getopts a:i:k:o:m:f:vnp:hr:q:w: OPT
do
	case $OPT in
		a)  h_num_cpu=$OPTARG
		    ;;
		i)  INTERVAL=${OPTARG}
		    expr "${INTERVAL}" + 1 > /dev/null 2>&1
		    if [ $? -ge 2 ]; then
			    echo "invalid -i value" >&2
			    exit 1
		    fi
		    if [ ${INTERVAL} -le 0 ]; then
			    echo "invalid -i value" >&2
			    exit 1
		    fi
		    ;;
		k)  LOGMODE=${OPTARG}
		    expr "${LOGMODE}" + 1 > /dev/null 2>&1
		    if [ $? -ge 2 ]; then
			    echo "invalid -k value" >&2
			    exit 1
		    fi
		    if [ ${LOGMODE} -lt 0 -o ${LOGMODE} -gt 2 ]; then
			    echo "invalid -k value" >&2
			    exit 1
		    fi
		    ;;
		o)  chown_option=${OPTARG}
		    ;;
		m)  mem_size=$OPTARG
		    ;;
		f)  facility=${OPTARG}
		    ;;
		n)  dry_run=1
		    ;;
		v)  enable_mcoverlay="yes"
		    ;;
		p)  preset_num=${OPTARG}
                    ;;
		h)  usage; exit 0
		    ;;
		r)  ikc_map=${OPTARG}
		    ;;
		q)  ihk_irq=${OPTARG}
		    ;;
		w)  nr_irq=${OPTARG}
		    ;;
	\?) usage; exit 1
	    ;;
	esac
done

if [ -n "$enable_mcoverlay" ]; then
	release=`uname -r`
	major=`echo ${release} | sed -e 's/^\([0-9]*\).*/\1/'`
	minor=`echo ${release} | sed -e 's/^[0-9]*.\([0-9]*\).*/\1/'`
	patch=`echo ${release} | sed -e 's/^[0-9]*.[0-9]*.\([0-9]*\).*/\1/'`
	linux_version_code=`expr \( ${major} \* 65536 \) + \( ${minor} \* 256 \) + ${patch}`
	rhel_release=`echo ${release} | sed -e 's/^[0-9]*.[0-9]*.[0-9]*-\([0-9]*\).*/\1/'`
	enable_mcoverlay=

	if [ "${release}" == "${rhel_release}" ]; then
		rhel_release=""
	fi

	if [ "${rhel_release}" == "" ]; then
		if [ ${linux_version_code} -ge 262144 -a ${linux_version_code} -lt 262400 ]; then
			enable_mcoverlay="yes"
		fi
		if [ ${linux_version_code} -ge 263680 -a ${linux_version_code} -lt 263936 ]; then
			enable_mcoverlay="yes"
		fi
	else
		if [ ${linux_version_code} -eq 199168 -a ${rhel_release} -ge 327 ]; then
			enable_mcoverlay="yes"
		fi
		if [ ${linux_version_code} -eq 266752 -a ${rhel_release} -ge 32 ]; then
			enable_mcoverlay="yes"
		fi
	fi
fi

printf "mcoverlayfs is "
if [ -n "$enable_mcoverlay" ]; then
    echo "enabled"
else
    echo "disabled"
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

have_host_cpu_list=0
max_h_num_cpu=0
lines=`cat /proc/cpuinfo | grep processor | cut -d ' ' -f 2`
while read line
do
	if [ $max_h_num_cpu -le $line ]; then
		max_h_num_cpu=$line
	fi
	have_host_cpu_list=$(( $have_host_cpu_list | (1 << $line) ))
done << END
$lines
END

for i in `seq 0 $max_h_num_cpu`
do
	if [ $(( ($h_cpu_list >> $i) & 1 )) -eq 0 ]; then
		if [ $(( ($have_host_cpu_list >> $i) & 1 )) -eq 1 ]; then
			p_num_cpu="${p_num_cpu}${i},"
		fi
	fi
done
p_num_cpu=`echo "$p_num_cpu" | sed 's/,$//'`

reserve "$p_num_cpu" $mem_size
max_chunk_size=0
if [ $dry_run -eq 0 ]; then
	old_ifs="$IFS"
	IFS=$'\n';

	while read line
	do
		size=`echo "$line" | sed -E 's/^\[.*\][\t ]+//g' | cut -d\  -f 7 | sed 's/)//g'`
		if [ $max_chunk_size -lt $size ]; then
			max_chunk_size=$size
		fi
	done < "$smp_resource_info"

	if [ $max_chunk_size -le $large_page_size ]; then
	    echo "memory size ($max_chunk_size) must be greater than $large_page_size. Increse the value in ./config"
	    exit 1
	fi

	max_chunk_size=$(( $max_chunk_size / $MiB ))
	IFS="$old_ifs"
else
	max_chunk_size="(memory-size)"
fi
if [ "$m_num_cpu" = "all" ]; then
	m_num_cpu="$p_num_cpu"
fi
boot "$m_num_cpu" $max_chunk_size
