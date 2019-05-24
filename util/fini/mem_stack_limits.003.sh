ulimit -S -c unlimited

if [ "$linux_run" == "yes" ]; then
	swapon -a
fi
