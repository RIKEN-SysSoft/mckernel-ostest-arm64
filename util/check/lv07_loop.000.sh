if [ "${linux_run}" != "yes" ]; then
	rc=0

	if ! diff -q $recordfile $answerfile; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
