if [ "$linux_run" == "yes" ]; then
	swapoff -a
fi

# SEGV is detected by exit status
ulimit -S -c 0
