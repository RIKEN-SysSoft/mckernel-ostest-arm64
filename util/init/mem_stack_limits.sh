# mcexec -s <stack_premap_size>:<max_stack_size>
if [ "$linux_run" == "yes" ]; then
    ulimit_s_org=`ulimit -s`

    case $testno in
	0)
	    ulimit -S -s $((10485760 / 1024))
	    ;;
	1)
	    ulimit -S -s $((2147483648 / 1024))
	    ;;
	2 | 3)
	    ulimit -S -s $((mck_max_mem_size_110p / 1024))
	    ;;
    esac

    sudo swapoff -a

    command_line=$(echo $command_line | sed 's/-s [,_${a-zA-Z0-9_}]* //')
fi
