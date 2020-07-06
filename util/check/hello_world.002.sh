tail -1 $recordfile > $recorddir/stdout.txt
grep -x 'Hello World' $recorddir/stdout.txt || rc=1
