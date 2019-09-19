#!/bin/sh

# It's sourced from /work/mcktest/data/script/ostest-* scripts.

echo "## $testname ##"
rm -rf $recorddir && mkdir -p $recorddir && pushd $recorddir

# Use recorddir and define mcexec, runHOST etc.
. ${AUTOTEST_HOME}/ostest/util/init.sh

if [ -f ${AUTOTEST_HOME}/ostest/util/init/${testname}.sh ]; then
	. ${AUTOTEST_HOME}/ostest/util/init/${testname}.sh
fi

# To check if kmsg will be empty
if [ "${linux_run}" != "yes" ]; then
	$ihkosctl 0 clear_kmsg
fi
	
if [ -f ${AUTOTEST_HOME}/ostest/util/exec/${testcase}.sh ]; then
	. ${AUTOTEST_HOME}/ostest/util/exec/${testcase}.sh > $recordfile 2>&1
else
	eval $command_line > $recordfile 2>&1
	exit_status=$?
fi

if [ -f ${AUTOTEST_HOME}/ostest/util/fini/${testname}.sh ]; then
	. ${AUTOTEST_HOME}/ostest/util/fini/${testname}.sh
fi

. ${AUTOTEST_HOME}/ostest/util/fini.sh

popd

# OK/NG decision
if [ "${linux_run}" != "yes" ]; then
	rc=0

	if [ -f ${AUTOTEST_HOME}/ostest/util/check/${testcase}.sh ]; then
		. ${AUTOTEST_HOME}/ostest/util/check/${testcase}.sh
	else
		. ${AUTOTEST_HOME}/ostest/util/check/result_is_ok.sh
	fi

	# Check if kmsg is empty
	$ihkosctl 0 kmsg > $recorddir/kmsg.log
	if [ "`cat $recorddir/kmsg.log | wc -l`" -ne 1 ]; then
	    echo "$(basename $0): WARNING: kmsg isn't empty."
	fi

	# Check if process/thread structs remain
	show_struct_process_or_thread="$recorddir/show_struct_process_or_thread.log"
	$ihkosctl 0 clear_kmsg
	$ihkosctl 0 ioctl 40000000 1
	$ihkosctl 0 ioctl 40000000 2
	$ihkosctl 0 kmsg > $show_struct_process_or_thread

	nprocs=`awk '$4=="processes"{print $3}' $show_struct_process_or_thread`
	if [ -n $nprocs ] && [ "$nprocs" != "0" ]; then
	    echo "$(basename $0): INFO: $nprocs process(es) remaining"
	    rc=1
	fi

	nthreads=`awk '$4=="threads"{print $3}' $show_struct_process_or_thread`
	if [ -n $nthreads ] && [ "$nthreads" != "0" ]; then
	    echo "$(basename $0): INFO: $nprocs thread(s) remaining"
	    rc=1
	fi

	# Pass OK/NG to runtestlist.sh
	echo $rc > $WORKDIR/result.log
fi

exit $rc
