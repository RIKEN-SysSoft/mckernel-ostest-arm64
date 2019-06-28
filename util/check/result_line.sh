result_linux=`grep '^RESULT:.*' $answerfile`
result_mck=`grep '^RESULT:.*' $recordfile`

if [ "$result_linux" != "$result_mck" ]; then
    echo "result lines did not match:"
    echo "Linux: $result_linux"
    echo "McKernel: $result_mck"
    rc=1
fi
