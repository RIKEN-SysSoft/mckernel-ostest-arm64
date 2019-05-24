if [ "${linux_run}" != "yes" ]; then
	rc=0

	nl_linux=`wc -l $answerfile | cut -d ' ' -f 1`
	nl_mck=`wc -l $recordfile | cut -d ' ' -f 1`
	result_linux=`awk -F ': *' '$1=="RESULT" {print $2}' $answerfile`
	result_mck=`awk -F ': *' '$1=="RESULT" {print $2}' $recordfile`
	core_linux=`find $answerdir -name "core*" | wc -l`
	core_mck=`find $recorddir -name "core*" | wc -l`

	if [ $core_linux -ne $core_mck ]; then
		echo "Core file counts did not match ($core_linux vs. $core_mck)"
		rc=1
	fi

	if [ $nl_linux -ne $nl_mck ]; then
		echo "Numbers of lines of log did not match ($nl_linux vs. $nl_mck)"
		rc=1
	fi

	if [ "$result_linux" != "$result_mck" ]; then
		echo "RESULTs did not match ($result_linux vs. $result_mck)"
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
