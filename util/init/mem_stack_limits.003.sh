if [ "$linux_run" == "yes" ]; then
	swapoff -a
fi

ulimit -S -c 0 # Check SEGV by checking if exit status is 139
