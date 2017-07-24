	if [ "$DRYRUN" != ":" ]; then
	echo "send SIGKILL for mcexec."
	kill -9 `$pidof_mcexec`

	#### rmmod test driver ####    
	echo "rmmod test_drv"
	sh "$rmmod_test_drv_sh"
	fi

	if [ "$DRYRUN" != ":" ] && [ "$linux_run" == "no" ]; then
	#### shutdown mckernel ####
	sh "$shutdown_mck_sh"
	fi

	if [ "$DRYRUN" != ":" ]; then
	#### host output corefile limitsize setting restore ####
	echo "restore ulimit -S -c $orig_core_rlimit"
	ulimit -S -c $orig_core_rlimit

	#### host output corefile-name setting restore ####
	echo "restore $orig_core_pattern => /proc/sys/kernel/core_pattern"
	echo $orig_core_pattern > /proc/sys/kernel/core_pattern

	#### console output setting restore ####
	echo "restore $orig_printk_setting => /proc/sys/kernel/printk"
	echo $orig_printk_setting > /proc/sys/kernel/printk

	if [ "$addusr" -eq 1 ]; then
		userdel $test_user_name
	fi
	fi

cd $ostest_util_dir

exit $rc
