. ${AUTOTEST_HOME}/ostest/util/exec/background.sh

sleep 3
siginfo_send_signal `$pidof_mcexec`
sleep 1
