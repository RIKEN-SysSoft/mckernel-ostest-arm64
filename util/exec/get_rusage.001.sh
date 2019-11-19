. ${AUTOTEST_HOME}/ostest/util/exec/background.sh
sleep 1
${linux_exec} "${app_dir}/get_rusage" 0
. ${AUTOTEST_HOME}/ostest/util/exec/wait.sh
