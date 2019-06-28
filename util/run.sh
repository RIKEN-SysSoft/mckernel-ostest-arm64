#!/bin/sh

# It's sourced from /work/mcktest/data/script/ostest-* scripts.

echo "## $testname ##"
rm -rf $recorddir && mkdir -p $recorddir && pushd $recorddir

# Use recorddir and define mcexec, runHOST etc.
. ${AUTOTEST_HOME}/ostest/util/init.sh

if [ -f ${AUTOTEST_HOME}/ostest/util/init/${testname}.sh ]; then
	. ${AUTOTEST_HOME}/ostest/util/init/${testname}.sh
fi

if [ -f ${AUTOTEST_HOME}/ostest/util/loop/${testname}.sh ]; then
	. ${AUTOTEST_HOME}/ostest/util/loop/${testname}.sh
elif [ "$testname" == "siginfo" ] && [ $testno -eq 1 ] ||
    [ "$testname" == "force_exit" ] && $testno -eq 0 ]; then
	eval $command_line > $recordfile &
else
	eval $command_line > $recordfile
	exit_status=$?
fi

if [ -f ${AUTOTEST_HOME}/ostest/util/fini/${testname}.sh ]; then
	. ${AUTOTEST_HOME}/ostest/util/fini/${testname}.sh
fi

. ${AUTOTEST_HOME}/ostest/util/fini.sh

popd

# Define rc
if [ "${linux_run}" != "yes" ]; then
	rc=0

	if [ -f ${AUTOTEST_HOME}/ostest/util/check/${testcase}.sh ]; then
		. ${AUTOTEST_HOME}/ostest/util/check/${testcase}.sh
	else
		. ${AUTOTEST_HOME}/ostest/util/check/default.sh
	fi

	echo $rc > $WORKDIR/result.log

fi

exit $rc
