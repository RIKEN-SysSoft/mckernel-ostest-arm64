result_linux=`tail -n 1 $answerfile`
result_mck=`tail -n 1 $recordfile`

if [ "$result_linux" != "$result_mck" ]; then
    echo "last lines did not match:"
    echo "Linux: $result_linux"
    echo "McKernel: $result_mck"
    rc=1
fi
