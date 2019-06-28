if [ "${linux_run}" != "yes" ]; then
	rc=0

	sleep 3
	$MCKINSTALL/sbin/ihkosctl 0 kmsg > $recorddir/kmsg
	fgrep page_fault_handler $recorddir/kmsg > /dev/null

	if [ $? -ne 0 ]; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
