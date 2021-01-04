result_mck=$(tail -n 1 $recordfile)

# mlockall is not supported
if [[ "$result_mck" != "RESULT: mlockall error." ]]; then
    rc=1
fi
