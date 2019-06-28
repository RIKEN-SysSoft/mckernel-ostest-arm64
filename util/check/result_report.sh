result_linux=`awk -F ': *' '$1=="RESULT" {print $2}' $answerfile`
result_mck=`awk -F ': *' '$1=="RESULT" {print $2}' $recordfile`

if [ "$result_linux" != "$result_mck" ]; then
    echo "result reports did not match ($result_linux vs. $result_mck)"
    rc=1
fi
