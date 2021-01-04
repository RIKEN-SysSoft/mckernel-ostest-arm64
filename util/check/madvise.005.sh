result_mck=$(tail -n 1 $recordfile)

# madvise is not supported with Fugaku hacks
if [[ "$result_mck" != "RESULT: madvise error." ]]; then
    rc=1
fi
