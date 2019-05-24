#!/bin/sh

# It's sourced from /work/mcktest/data/script/ostest-* scripts.

rm -rf $recorddir && mkdir -p $recorddir && pushd $recorddir

echo "## $testname ##"

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
if [ -f ${AUTOTEST_HOME}/ostest/util/check/${testcase}.sh ]; then
	. ${AUTOTEST_HOME}/ostest/util/check/${testcase}.sh
else
	. ${AUTOTEST_HOME}/ostest/util/check/default.sh
fi

exit $rc
