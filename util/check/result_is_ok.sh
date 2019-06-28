result_mck=`awk -F ': *' '$1=="RESULT" {print $2}' $recordfile`

if [ "$result_mck" != "ok" ]; then
	echo "RESULT field is not \"ok\" but \"$result_mck\""
	rc=1
fi
