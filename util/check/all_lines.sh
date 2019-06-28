if ! diff $recordfile $answerfile; then
    echo "output files did not match"
    rc=1
fi
