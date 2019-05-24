if [ "${linux_run}" != "yes" ]; then
	rc=0

	result_mck=`awk -F ': *' '$1=="RESULT" {print $2}' $recordfile`

	if [ "$result_mck" != "ok" ]; then
		rc=1
	fi

	if ! diff -q ${recorddir}/cpuset_forked ${workdir}/cpuset_execed >/dev/null; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
