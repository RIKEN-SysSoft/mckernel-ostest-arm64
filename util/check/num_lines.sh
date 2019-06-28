nl_linux=`wc -l $answerfile | cut -d ' ' -f 1`
nl_mck=`wc -l $recordfile | cut -d ' ' -f 1`

if [ "$nl_linux" != "$nl_mck" ]; then
    echo "Numbers of lines of log did not match ($nl_linux vs. $nl_mck)"
    rc=1
fi
