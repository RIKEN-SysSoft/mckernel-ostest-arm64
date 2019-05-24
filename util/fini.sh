	if [ "$DRYRUN" != ":" ] && [ "$linux_run" == "no" ]; then
	echo "send SIGKILL for mcexec."
	pidof mcexec | xargs -r kill -9
	fi

	#### rmmod test driver ####    
	echo "rmmod test_drv"
	sh "$rmmod_test_drv_sh"

	if [ "$DRYRUN" != ":" ] && [ "$linux_run" == "no" ]; then
		# Shutdown McKernel when invoked from run_testset_postk.sh
		if [ -z $AUTOTEST_HOME ]; then
			sh "$shutdown_mck_sh"
		fi
	fi

	if [ "$DRYRUN" != ":" ]; then
	#### host output corefile limitsize setting restore ####
	echo "restore ulimit -S -c $orig_core_rlimit"
	ulimit -S -c $orig_core_rlimit

	#### host output corefile-name setting restore ####
	echo "restore $orig_core_pattern => /proc/sys/kernel/core_pattern"
	$E bash -c "echo $orig_core_pattern > /proc/sys/kernel/core_pattern"

	#### console output setting restore ####
	echo "restore $orig_printk_setting => /proc/sys/kernel/printk"
	$E bash -c "echo $orig_printk_setting > /proc/sys/kernel/printk"

	if [ "$addusr" -eq 1 ]; then
		$E userdel $test_user_name
	fi
	fi
