if [ "$linux_run" == "no" ]; then
    echo "send SIGKILL for mcexec."
    pidof mcexec | xargs -r kill -9
fi

# Shutdown McKernel when invoked from run_testset_postk.sh
if [ "$linux_run" == "no" ] && [ -z $AUTOTEST_HOME ]; then
    sh "$shutdown_mck_sh"
fi

#### host output corefile limitsize setting restore ####
echo "restore ulimit -S -c $orig_core_rlimit"
ulimit -S -c $orig_core_rlimit

#### host output corefile-name setting restore ####
echo "restore $orig_core_pattern => /proc/sys/kernel/core_pattern"
$E bash -c "echo $orig_core_pattern > /proc/sys/kernel/core_pattern"

#### console output setting restore ####
echo "restore $orig_printk_setting => /proc/sys/kernel/printk"
$E bash -c "echo $orig_printk_setting > /proc/sys/kernel/printk"

rm -f $ostype_name

if [ "$addusr" -eq 1 ]; then
    if ! $E userdel $test_user_name; then
	echo "ERROR: userdel failed"
    fi
fi
