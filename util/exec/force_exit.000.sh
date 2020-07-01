. ${AUTOTEST_HOME}/ostest/util/exec/background.sh
sleep 3
echo "send SIGKILL for mcexec."
kill -9 `$pidof_mcexec`
