# madvise/014.c
result_mck=$(tail -n 1 $recordfile)

# phys != 0, meaning mapping present because it's copied on fork
if [[ "$result_mck" != "RESULT: WEXITSTATUS error." ]]; then
    rc=1
fi
