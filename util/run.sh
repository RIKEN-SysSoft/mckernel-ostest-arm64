#!/bin/sh

# It's sourced from /work/mcktest/data/script/ostests-* scripts.

SCRIPT_PATH=$(readlink -m "${BASH_SOURCE[0]}")
AUTOTEST_HOME="${SCRIPT_PATH%/*/*/*}"
OSTEST_INSTALL="${SCRIPT_PATH%/*/*}"

echo "## $testname ##"
rm -rf $recorddir && mkdir -p $recorddir && pushd $recorddir

# Use recorddir and define mcexec, runHOST etc.
. $OSTEST_INSTALL/bin/init.sh

if [ -f $OSTEST_INSTALL/bin/init/${testname}.sh ]; then
	. $OSTEST_INSTALL/bin/init/${testname}.sh
fi

# To check if kmsg will be empty
if [ "${linux_run}" != "yes" ]; then
	$ihkosctl 0 clear_kmsg
fi
	
if [ -f $OSTEST_INSTALL/bin/exec/${testcase}.sh ]; then
	. $OSTEST_INSTALL/bin/exec/${testcase}.sh 2>&1 | tee $recordfile
else
	eval $command_line 2>&1 | tee $recordfile
	exit_status=${PIPESTATUS[0]}
fi

if [ -f $OSTEST_INSTALL/bin/fini/${testname}.sh ]; then
	. $OSTEST_INSTALL/bin/fini/${testname}.sh
fi

. $OSTEST_INSTALL/bin/fini.sh

popd

# OK/NG decision
if [ "${linux_run}" != "yes" ]; then
	rc=0

	if [ -f $OSTEST_INSTALL/bin/check/${testcase}.sh ]; then
		. $OSTEST_INSTALL/bin/check/${testcase}.sh
	else
		. $OSTEST_INSTALL/bin/check/result_is_ok.sh
	fi

	# Check if kmsg is empty
	nlines=$($ihkosctl 0 kmsg | wc -l)
	(( nlines > 1 )) && echo "$(basename $0): WARNING: kmsg isn't empty."

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
fi

exit $rc
