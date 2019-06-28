# Wait until memory release finishes and the thread is released
sleep $((${mck_max_mem_size_95p} / (5 * 1024 * 1024 * 1024)))

if [ "${linux_run}" != "yes" ]; then
	rc=0

	result_mck=`awk -F ': *' '$1=="RESULT" {print $2}' $recordfile`

	if [ "$result_mck" != "ok" ]; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
