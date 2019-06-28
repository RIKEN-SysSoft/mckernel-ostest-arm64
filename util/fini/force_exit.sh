sleep 3
echo "send SIGKILL for mcexec."
kill -9 `${pidofcomm}`

. ${AUTOTEST_HOME}/ostest/util/init/mmapfile.sh
sh "$rmmod_test_drv_sh"
