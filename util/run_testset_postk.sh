#!/bin/sh
# run_testset_postk.sh COPYRIGHT FUJITSU LIMITED 2015-2018

source ./before_run_testcase.sh

# RT_BLOCK 1 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 1 ]; then
	#### other than test_mck tp case ####
	echo "## hello_world ##"
	${mcexec} $execve_comm "${app_dir}/exit_group"
	${mcexec} $execve_comm "${app_dir}/hello_world"
	${mcexec} $execve_comm "${app_dir}/glibc_hello_world"

	echo "## lv07 ##"
	${mcexec} $execve_comm "${app_dir}/lv07-st" $execve_arg_end $ostype_name
	${mcexec} $execve_comm "${app_dir}/lv07-pth" $execve_arg_end $ostype_name

	count=1
	${DRYRUN} touch ${lv07_tmp_before}
	while [ $count -le $mck_max_cpus ]
	do
		${mcexec} $execve_comm "${app_dir}/lv07-pth" $execve_arg_end $ostype_name $count ${DRYRUN_QUOTE}| tee ${lv07_tmp}${DRYRUN_QUOTE}
		if [ "$DRYRUN" != ":" ]; then
		work_str=`cat ${lv07_tmp}`
		echo ${work_str} | grep -o ".:${ostype_str}" | sort > ${lv07_tmp}
		diff ${lv07_tmp_before} ${lv07_tmp}
		mv ${lv07_tmp} ${lv07_tmp_before}
		fi
		count=`expr $count + 1`
	done
	${DRYRUN} rm ${lv07_tmp_before}
	count=0

	echo "## lv09 ##"
	${mcexec} $execve_comm "${app_dir}/lv09-pgf" $execve_arg_end w $temp aaabbbcccdddeeefffggghhh\\n
	${mcexec} $execve_comm "${app_dir}/lv09-pgf" $execve_arg_end r $temp

	echo "## lv11 ##"
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end w rp   $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end w rwp  $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end w rep  $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end w rwep $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end w wp   $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end w wep  $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end w ep   $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end r rp   $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end r rwp  $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end r rep  $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end r rwep $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end r wp   $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end r wep  $temp
	${mcexec} $execve_comm "${app_dir}/lv11" $execve_arg_end r ep   $temp

	echo "## lv12 ##"
	${mcexec} $execve_comm "${app_dir}/lv12-kill"
	${mcexec} $execve_comm "${app_dir}/lv12-kill-single"
	${mcexec} $execve_comm "${app_dir}/lv12-segv"
fi # RT_BLOCK 1 end

# RT_BLOCK 2 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 2 ]; then
#	echo "## lv13 ##"
#SKIP	${mcexec} $execve_comm "${app_dir}/lv13"

	echo "## lv14 ##"
	${mcexec} $execve_comm "${app_dir}/lv14" $execve_arg_end 0
	${mcexec} $execve_comm "${app_dir}/lv14" $execve_arg_end 1
	${mcexec} $execve_comm "${app_dir}/lv14" $execve_arg_end 2

	if [ $mck_max_mem_size -ge 1181116006 ]; then
		${mcexec} $execve_comm "${app_dir}/lv14" $execve_arg_end 3
		${mcexec} $execve_comm "${app_dir}/lv14" $execve_arg_end 4
		${mcexec} $execve_comm "${app_dir}/lv14" $execve_arg_end 5
	else
		echo "## lv14 03-05 SKIP ##"
	fi

	echo "## lv15 ##"
	count=0
	while [ $count -le 10 ]
	do
		${mcexec} $execve_comm "${app_dir}/lv15-kill"
		count=`expr $count + 1`
	done
	count=0
	while [ $count -le 10 ]
	do
		${mcexec} $execve_comm "${app_dir}/lv15-manon"
		count=`expr $count + 1`
	done
	count=0
	while [ $count -le 10 ]
	do	
		${mcexec} $execve_comm "${app_dir}/lv15-mfile" $execve_arg_end $ostype_name
		count=`expr $count + 1`
	done

	echo "## socket ##"
	${mcexec} $execve_comm "${app_dir}/single_node"
#MANUAL	${mcexec} $execve_comm "${app_dir}/2node_recv"
#MANUAL	${mcexec} $execve_comm "${app_dir}/2node_send" $execve_arg_end $ipaddress

	#### test_mck case ####
	echo "## siginfo ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s siginfo -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s siginfo -n 1 ${DRYRUN_QUOTE}&${DRYRUN_QUOTE}
	if [ "$DRYRUN" != ":" ]; then
	sleep 3
	siginfo_send_signal `$pidof_mcexec`
	sleep 1
	fi

	echo "## wait4 ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s wait4 -n 0 
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s wait4 -n 1 -- -f $pid_max_name
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s wait4 -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s wait4 -n 3

	echo "## env ##"
	env_opt="-e AAA -e USER= -e a -e b -e ARCH=x86 -e ARCH=postk"
	${mcexec} $env_opt $execve_comm "${app_dir}/test_mck" $execve_arg_end -s env -n 0

	echo "## rt_sigsuspend ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigsuspend -n 0

	echo "## cpu_thread_limits ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s cpu_thread_limits -n 0 -- -t $mck_ap_num
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s cpu_thread_limits -n 1 -- -t $mck_ap_num

	echo "## gettid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s gettid -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s gettid -n 1

	echo "## mprotect ##"
	for tp_num in `seq 0 7`
	do	
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mprotect -n $tp_num
	done

	echo "## mem_stack_limits ##"
	${mcexec} -s 10485760,10485760 $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s 9961472

	if [ $mck_max_mem_size -ge 2244120412 ]; then
		${mcexec} -s 2147483648,2147483648 $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s 2040109466
	else
		echo "## mem_stack_limits 2GiB SKIP ##"
	fi
fi # RT_BLOCK 2 end

# RT_BLOCK 3 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 3 ]; then
	if [ $sep_run_num -eq 3 ]; then
		echo "## mem_stack_limits ##"
	fi
	${DRYRUN} echo "MCKERNEL_RLIMIT_STACK=mckernel_max_memory_size x 110%"
	${mcexec} -s ${DRYRUN_QUOTE}${mck_max_mem_size_110p},${mck_max_mem_size_110p}${DRYRUN_QUOTE} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size_95p
fi # RT_BLOCK 3 end

# RT_BLOCK 4 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 4 ]; then
	if [ $sep_run_num -eq 4 ]; then
		echo "## mem_stack_limits ##"
		${DRYRUN} echo "MCKERNEL_RLIMIT_STACK=mckernel_max_memory_size x 110%"
		${mcexec} -s ${DRYRUN_QUOTE}${mck_max_mem_size_110p},${mck_max_mem_size_110p}${DRYRUN_QUOTE} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size_110p
	else
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size_110p
	fi
fi # RT_BLOCK 4 end

# RT_BLOCK 5 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 5 ]; then
	echo "## munlock ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s munlock -n 0

	echo "## rt_sigaction ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigaction -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigaction -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigaction -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigaction -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigaction -n 4
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigaction -n 5

	echo "## fork ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fork -n 0

	echo "## pause ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s pause -n 0

	echo "## sigaltstack ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sigaltstack -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sigaltstack -n 1

	echo "## ptrace ##"
	for tp_num in `seq 0 21`
	do	
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s ptrace -n $tp_num
	done

	echo "## mmap_dev ##"
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_dev -n 0 -- -d /dev/test_mck/mmap_dev -s 8192
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_dev -n 1 -- -d /dev/test_mck/mmap_dev2 -s 8192
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_dev -n 1 -- -d /dev/test_mck/mmap_dev2 -s $(( ($page_size/8) * $page_size + $page_size ))
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_dev -n 2 -- -d /dev/test_mck/mmap_dev2 -s 8192
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_dev -n 2 -- -d /dev/test_mck/mmap_dev2 -s $(( ($page_size/8) * $page_size + $page_size ))

	echo "## tgkill ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tgkill -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tgkill -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tgkill -n 2

	echo "## rt_sigpending ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigpending -n 0

	echo "## rt_sigqueueinfo ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigqueueinfo -n 0

	echo "## rt_sigprocmask ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigprocmask -n 0

	echo "## mmap_populate ##"
	if [ "$DRYRUN" != ":" ]; then
	echo a > $mmapfile_name
	fi
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_populate -n 0 -- -f $mmapfile_name

	echo "## mem_large_page ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 1
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 2
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 4
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 5
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 6
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 7
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 8
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 9
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_large_page -n 10

	echo "## tls ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tls -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tls -n 1 -- -t $mck_ap_num

	echo "## mmap_file ##"
	${DRYRUN} rm -f $mmapfile_name
	for tp_num in `seq 0 48`
	do	
		if [ "$DRYRUN" != ":" ]; then
		echo a > $mmapfile_name
		fi
		${DRYRUN} cat $mmapfile_name
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_file -n $tp_num -- -f $mmapfile_name
		${DRYRUN} cat $mmapfile_name
	done

	echo "## execve ##"
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 0 -- -f "$app_dir/execve_app"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/execve_app"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/test_mck"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/test_mck" -- -s env -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 3 -- -f "$app_dir/execve_app"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 4 -- -f "$app_dir/execve_app"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s execve -n 5 -- -f "$app_dir/execve_app"

	echo "## madvise ##"
	for tp_num in `seq 0 7`
	do	
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s madvise -n $tp_num -- -f $mmapfile_name
	done
#REPEAL madvise#8,9
	for tp_num in `seq 10 15`
	do	
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s madvise -n $tp_num -- -f $mmapfile_name
	done
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s madvise -n 16
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s madvise -n 17
	for tp_num in `seq 18 19`
	do
		${DRYRUN} ulimit -S -c unlimited
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s madvise -n $tp_num -- -f $mmapfile_name
		if [ "$DRYRUN" != ":" ]; then
		ulimit -S -c 0
		mv core core.madvise$tp_num.$$
		echo "generate corefile: core.madvise$tp_num.$$"
		gdb -x $app_dir/madvise$tp_num.inf $app_dir/test_mck core.madvise$tp_num.$$
		readelf -l core.madvise$tp_num.$$ | tail -16
		fi
	done

	echo "## cpu_proc_limits ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s cpu_proc_limits -n 0 -- -p $mck_ap_num

	echo "## nfo ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nfo -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nfo -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nfo -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nfo -n 3

	echo "## getrlimit ##"
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 1
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 3
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 4
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 5
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 6
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 7
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 8
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 9
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 10
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 11
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 12
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 13
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 14
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 15
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 16
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 17
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrlimit -n 18

#	echo "## rt_sigtimedwait ##"
#	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s rt_sigtimedwait -n 0

	echo "## mlock ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mlock -n 0

	echo "## mmap_locked ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mmap_locked -n 0 -- -f $mmapfile_name

	echo "## remap_file_pages ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s remap_file_pages -n 0 -- -s $((1024*16))
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s remap_file_pages -n 0 -- -s $((256*1024*1024))

	echo "## mem_limits ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $((1024*1024)) -c 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $mck_max_mem_size_95p -c 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $mck_max_mem_size -c 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -S mmap -c 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $((1024*1024)) -c 1
fi # RT_BLOCK 5 end

# RT_BLOCK 6 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 6 ]; then
	if [ $sep_run_num -eq 6 ]; then
		echo "## mem_limits ##"
	fi

	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $mck_max_mem_size_95p -c 1
fi # RT_BLOCK 6 end

# RT_BLOCK 7 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 7 ]; then
	if [ $sep_run_num -eq 7 ]; then
		echo "## mem_limits ##"
	fi

	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $mck_max_mem_size -c 1
fi # RT_BLOCK 7 end

# RT_BLOCK 8 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 8 ]; then
	if [ $mck_max_node_mem_size -ge 2684354560 ]; then
		echo "## large_bss ##"
		${mcexec} $execve_comm "${app_dir}/large_bss"
	else
		echo "## large_bss SKIP ##"
	fi

#	echo "## tofu ##"
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu -n 0

#	echo "## tofu_mmap_check ##"
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_mmap_check -n 0 -- -f $temp
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_mmap_check -n 1
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_mmap_check -n 2
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_mmap_check -n 3
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_mmap_check -n 4
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_mmap_check -n 5

#	echo "## tofu_unmap_notify ##"
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_unmap_notify -n 0 -- -f $temp
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_unmap_notify -n 1 -- -f $temp
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tofu_unmap_notify -n 2 -- -f $temp

	echo "## system ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s system -n 0

	echo "## vfork ##"
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s vfork -n 0 -- -f "$app_dir/execve_app"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s vfork -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s vfork -n 2 -- -f "$app_dir/execve_app"

	echo "## coredump ##"
	${DRYRUN} echo "This test case setting corefile rlimit unlimited."
	${DRYRUN} ulimit -S -c unlimited
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s coredump -n 0
	if [ "$DRYRUN" != ":" ]; then
	ulimit -S -c 0
	mv core core.$$
	echo "generate corefile: core.$$"
	readelf -a core.$$
	file core.$$
	gdb -x $app_dir/autorun.inf $app_dir/test_mck core.$$
	fi

	echo "## popen ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s popen -n 0

	echo "## procfs ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 4
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 5
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 6
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 7
#	sh "$kmsgcat_sh" | grep "TPLOG" | grep "procfs_maps"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 8
#	sh "$kmsgcat_sh" | grep "TPLOG" | grep "procfs_pagemap"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 9
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 10
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 11
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 12
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 13

	echo "## fork_execve ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fork_execve -n 0 -- -f "$app_dir/execve_app"

	echo "## shellscript ##"
	${mcexec} $execve_comm "${app_dir}/test_shell.sh"

	echo "## mremap_mmap_anon ##"
	for tp_num in `seq 0 16`
	do
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done
	for tp_num in `seq 100 116`
	do
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done
	for tp_num in `seq 200 216`
	do
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done
	for tp_num in `seq 300 316`
	do
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done

	echo "## get_cpu_id ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_cpu_id -n 0
	if [ "$DRYRUN" != ":" ]; then
	sh "$kmsgcat_sh" | grep "TPLOG:sys_get_cpu_id"
	fi

	echo "## setpgid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setpgid -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setpgid -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setpgid -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setpgid -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setpgid -n 4 -- -f "$app_dir/execve_app"

	echo "## kill ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s kill -n 0 -- -p $mck_ap_num
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s kill -n 1 -- -p $mck_ap_num
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s kill -n 2 -- -p $mck_ap_num

	echo "## sched_setaffinity ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 0 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 1 -- -p $mck_max_cpus
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 2 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 3 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 4 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 5
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 6 -- -p $mck_max_cpus
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 7 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 8 -- -p $mck_max_cpus
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 9 -- -p $mck_max_cpus
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 10 -- -p $mck_max_cpus
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 11 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setaffinity -n 12 -- -p $mck_max_cpus

	getaff_cpus=`expr $mck_max_cpus + 5`
	echo "## sched_getaffinity ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getaffinity -n 0 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getaffinity -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getaffinity -n 2 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getaffinity -n 3 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getaffinity -n 4 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getaffinity -n 5 -- -p $mck_max_cpus
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getaffinity -n 6 -- -p $getaff_cpus -f "$app_dir/show_affinity" -- -p $getaff_cpus

	echo "## pthread_setaffinity ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s pthread_setaffinity -n 0 -- -p $mck_max_cpus

	echo "## pthread_getaffinity ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s pthread_getaffinity -n 0 -- -p $mck_max_cpus

	echo "## enosys ##"
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s enosys -n 0
#REPEAL	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s enosys -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s enosys -n 2

	echo "## getcpu ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getcpu -n 0

	echo "## getegid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getegid -n 0 -- -e $gid

	echo "## geteuid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s geteuid -n 0 -- -e $uid

	echo "## getgid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getgid -n 0 -- -g $gid

	echo "## getppid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getppid -n 0
fi # RT_BLOCK 8 end

# RT_BLOCK 9 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 9 ]; then
	echo "## getresgid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getresgid -n 0 -- -r $gid -e $gid -s $gid

	echo "## getresuid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getresuid -n 0 -- -r $uid -e $uid -s $uid

	echo "## getuid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getuid -n 0 -- -u $uid

	echo "## ipc ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s ipc -n 0

	echo "## mincore ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mincore -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mincore -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mincore -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mincore -n 3

	echo "## mlockall ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mlockall -n 0

	echo "## msync ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s msync -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s msync -n 1

	echo "## munlockall ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s munlockall -n 0

	echo "## page_fault_forwording ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s page_fault_forwording -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s page_fault_forwording -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s page_fault_forwording -n 2

	echo "## process_vm_readv ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s process_vm_readv -n 0

	echo "## process_vm_writev ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s process_vm_writev -n 0

	echo "## sched_get_priority_max ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_max -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_max -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_max -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_max -n 3

	echo "## sched_get_priority_min ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_min -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_min -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_min -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_get_priority_min -n 3

	echo "## sched_getparam ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getparam -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getparam -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getparam -n 2

	echo "## sched_getscheduler ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getscheduler -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_getscheduler -n 1

	echo "## sched_rr_get_interval ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_rr_get_interval -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_rr_get_interval -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_rr_get_interval -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_rr_get_interval -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_rr_get_interval -n 4

	echo "## sched_setparam ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setparam -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setparam -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setparam -n 2

	echo "## sched_setscheduler ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setscheduler -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setscheduler -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setscheduler -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setscheduler -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_setscheduler -n 4

	echo "## setfsgid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setfsgid -n 0 -- -f $gid

	echo "## setfsuid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setfsuid -n 0 -- -f $uid

	echo "## setgid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setgid -n 0 -- -g $gid

	echo "## setregid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setregid -n 0 -- -r $gid -e $gid

	echo "## setresgid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setresgid -n 0 -- -r $gid -e $gid -s $gid

	echo "## setresuid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setresuid -n 0 -- -r $uid -e $uid -s $uid

	echo "## setreuid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setreuid -n 0 -- -r $uid -e $uid

	echo "## setrlimit ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 4
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 5
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 6
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 7
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 8
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 9
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 10
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 11
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 12
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 13
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 14
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setrlimit -n 15

	echo "## setuid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setuid -n 0 -- -u $uid

	echo "## waitid ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s waitid -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s waitid -n 1 -- -p $mck_ap_num_even
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s waitid -n 2 -- -p $mck_ap_num

	echo "## signalfd4 ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s signalfd4 -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s signalfd4 -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s signalfd4 -n 2

	echo "## gettimeofday ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s gettimeofday -n 0

	echo "## sched_yield ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s sched_yield -n 0

	echo "## set_tid_address ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_tid_address -n 0

	echo "## getrusage ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrusage -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrusage -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrusage -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrusage -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getrusage -n 4
fi # RT_BLOCK 9 end

# RT_BLOCK 10 start
if [ $sep_run_num -eq 0 -o $sep_run_num -eq 10 ]; then
	echo "## tkill ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tkill -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s tkill -n 1

	echo "## times ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s times -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s times -n 1

	echo "## nanosleep ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nanosleep -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nanosleep -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nanosleep -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s nanosleep -n 3

	echo "## getitimer ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getitimer -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getitimer -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s getitimer -n 2

	echo "## setitimer ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setitimer -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setitimer -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setitimer -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s setitimer -n 3

	echo "## clock_gettime ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s clock_gettime -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s clock_gettime -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s clock_gettime -n 2

	echo "## clock_getres ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s clock_getres -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s clock_getres -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s clock_getres -n 2

	echo "## readlinkat ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s readlinkat -n 0 -- -f ${temp} -l ${link}

	echo "## fpregs ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fpregs -n 0
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fpregs -n 1
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fpregs -n 2
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fpregs -n 3
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fpregs -n 4
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fpregs -n 5 -- -p $mck_max_cpus

	echo "## binfmt ##"
	binfmt_prefix_comm=
	if [ "${execve_comm}" != "" ]; then
		binfmt_prefix_comm="$execve_comm"
	fi
	MCEXEC_WL=${DRYRUN_QUOTE}$(cd ${app_dir}; pwd -P)${DRYRUN_QUOTE} $binfmt_prefix_comm "$app_dir/test_mck" $execve_arg_end -s get_cpu_id -n 0

	echo "## time_sharing ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s time_sharing -n 0

#	echo "## evtstrm ##"
#SKIP	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s evtstrm -n 0

	echo "## writecombine ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s writecombine -n 0 -- -d /dev/test_mck/writecombine

	mems_allowed=`${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s procfs -n 10 | grep "Mems_allowed:"`
	max_node=`echo $mems_allowed | awk '{print $2}' | sed 's|,||g' |tr '[a-z]' '[A-Z]'`
	max_node=`echo "obase=2; ibase=16; $max_node" | bc | wc -c`
	max_node=`expr $max_node - 1`

	if [ $max_node -ge 2 ]; then
		echo "## get_mempolicy ##"
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 0
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 1 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 2
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 3 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 4 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 5 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 6 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 7 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 8
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 9 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 10 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 11 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 12 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 13
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 14
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 15
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 16
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 17 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 18 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 19
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s get_mempolicy -n 20

		echo "## set_mempolicy ##"
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 0 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 1 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 2 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 3 -- -n $max_node
		if [ $max_node -ge 4 ]; then
#SKIP			${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 4 -- -n $max_node
			${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 5 -- -n $max_node
		else
			echo "## set_mempolicy 4-5 SKIP(max_node < 4) ##"
		fi
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 6 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 7 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 8
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 9 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 10 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 11 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 12 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 13 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 14 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 15 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 16 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s set_mempolicy -n 17

		echo "## mbind ##"
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 0 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 1 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 2 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 3 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 4 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 5 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 6 -- -n $max_node
#		if [ $max_node -ge 4 ]; then
#SKIP			${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 7 -- -n $max_node
#SKIP			${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 8 -- -n $max_node
#		else
#			echo "## mbind 7-8 SKIP(max_node < 4) ##"
#		fi
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 9 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 10 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 11 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 12 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 13 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 14 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 15 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 16 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 17 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 18 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 19 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 20 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 21 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 22 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 23 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 24 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 25 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 26 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 27 -- -n $max_node
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 28 -- -n $max_node
#SKIP		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 29
        	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s mbind -n 30
	else
		echo "## get_mempolicy set_mempoliacy mbind SKIP(max_node < 2) ##"
	fi

	echo "## track syscalls ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s track_syscalls -n 0
	if [ "$DRYRUN" != ":" ]; then
	"$ihkosctl" 0 kmsg | grep -E "\(271, *\"process_vm_writev\"\):"
	fi

	echo "## msr instruction emulation  ##"
	for tp_num in `seq 0 33`
	do
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s iemul -n $tp_num
	done

	echo "## fstatat  ##"
	for tp_num in `seq 0 2`
	do
		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s fstatat -n $tp_num
	done

	if [ "$linux_run" == "no" ]; then
	echo "## freeze_thaw ##"
	echo "## freeze_thaw test#0 (a.out -> freeze -> thaw) ##"
	${mcexec} $execve_comm "${app_dir}/single_node" &
	if [ "$DRYRUN" != ":" ]; then
	sleep 1

	"${app_dir}/freeze_thaw" 0 freeze
	for i in `seq 1 3`
	do
		sleep 1
		echo "sleep ${i} second elapsed."
	done
	"${app_dir}/freeze_thaw" 0 thaw
	wait `$pidof_mcexec`
	fi

#SKIP	echo "## freeze_thaw test#1 (freeze -> a.out -> thaw) ##"
#SKIP	"${app_dir}/freeze_thaw" 0 freeze
#SKIP	${mcexec} $execve_comm "${app_dir}/glibc_hello_world" &
#SKIP	for i in `seq 1 3`
#SKIP	do
#SKIP		sleep 1
#SKIP		echo "sleep ${i} second elapsed."
#SKIP	done
#SKIP	"${app_dir}/freeze_thaw" 0 thaw
#SKIP	wait `$pidof_mcexec`

#SKIP	echo "## freeze_thaw test#2 (freeze -> a.out -> kill a.out -> thaw) ##"
#SKIP	"${app_dir}/freeze_thaw" 0 freeze
#SKIP	${mcexec} $execve_comm "${app_dir}/glibc_hello_world" &
#SKIP	kill `$pidof_mcexec`
#SKIP	"${app_dir}/freeze_thaw" 0 thaw
#SKIP	${mcexec} $execve_comm "${app_dir}/glibc_hello_world"

	echo "## cpu_pa_info ##"
	echo "## cpu_pa_info test#0 ##"
	"${app_dir}/cpu_pa_info" 0 0

#REPEAL	echo "## cpu_pa_info test#1 ##"
#	"${app_dir}/cpu_pa_info" 0 1

	echo "## cpu_pa_info test#2 ##"
	"${app_dir}/cpu_pa_info" 0 2

	echo "## cpu_pa_info test#3 ##"
	"${app_dir}/cpu_pa_info" 0 3

	echo "## cpu_pa_info test#4 ##"
	"${app_dir}/cpu_pa_info" 0 4

	echo "## cpu_pa_info test#5 ##"
	"${app_dir}/cpu_pa_info" 0 5

	echo "## get_rusage ##"
	echo "## get_rusage test#0 (no user process) ##"
	"${app_dir}/get_rusage" 0

	echo "## get_rusage test#1 (user multi thread process rinning) ##"
	${mcexec} $execve_comm "${app_dir}/single_node" &
	if [ "$DRYRUN" != ":" ]; then
	sleep 1
	"${app_dir}/get_rusage" 0
	wait `$pidof_mcexec`
	fi
	fi

#	echo "## uti ##"
#	for tp_num in `seq 0 17`
#	do
#		${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s uti -n $tp_num
#	done

fi # RT_BLOCK 10 end

	echo "## force_exit ##"
	${mcexec} $execve_comm "${app_dir}/test_mck" $execve_arg_end -s force_exit -n 0 -- -f $mmapfile_name -d /dev/test_mck/mmap_dev &
	if [ "$DRYRUN" != ":" ]; then
	sleep 3
	echo "send SIGKILL for mcexec."
	kill -9 `$pidof_mcexec`
	fi

	if [ "$DRYRUN" != ":" ] && [ "$linux_run" == "no" ]; then
	echo "shutdown_mck... (mcctrl.ko no unload, and reboot mck.)"
	sh "$shutdown_mck_sh" -m
	sh $run_mck_sh -a $host_core $boot_numa_opt -v -r $ikc_map
	fi

	if [ "$DRYRUN" != ":" ]; then
	${mcexec} $execve_comm "${app_dir}/glibc_hello_world"
	fi

	source ./after_run_testcase.sh

#	if [ -e ${sh_base}/continue_end ]; then
#		echo "find continue_end file."
#		break
#	fi
#done

