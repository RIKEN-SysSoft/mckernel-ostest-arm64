if [ "${linux_run}" != "yes" ]; then
	rc=0

	sleep 3
	$MCKINSTALL/sbin/ihkosctl 0 kmsg > /tmp/autotest-kmsg.txt
	fgrep page_fault_handler /tmp/autotest-kmsg.txt > /dev/null

	if [ $? -ne 0 ]; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
