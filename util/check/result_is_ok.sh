result_mck=`awk -F ': *' '$1=="RESULT" {print $2}' $recordfile | xargs -r`

if [ -z "$result_mck" ]; then
    rc=1
fi

if ! echo $result_mck | grep SKIP >/dev/null; then
    for i in $result_mck; do
	if [ "$i" != "ok" ] && [ "$i" != "OK." ]; then
	    echo "[ NG ] RESULTs: $result_mck (expecting all ok)"
	    rc=1
	    break
	fi
    done
fi
