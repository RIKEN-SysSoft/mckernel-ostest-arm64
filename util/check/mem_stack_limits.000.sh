if [ "${linux_run}" != "yes" ]; then
	rc=0

	nl_linux=`wc -l $answerfile | cut -d ' ' -f 1`
	nl_mck=`wc -l $recordfile | cut -d ' ' -f 1`
	result_linux=`awk -F ': *' '$1=="RESULT" {print $2}' $answerfile`
	result_mck=`awk -F ': *' '$1=="RESULT" {print $2}' $recordfile`

	if [ $nl_linux -ne $nl_mck ]; then
		rc=1
	fi

	if [ "$result_linux" != "$result_mck" ]; then
		rc=1
	fi

	if [ $exit_status -ne 139 ]; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
