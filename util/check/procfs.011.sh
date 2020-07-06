awk '/ARGS/, /RESULT/ {print}' $recordfile | tail -2 | head -1 > $recorddir/cmdline.txt
grep -x $AUTOTEST_HOME/ostest/bin/test_mck $recorddir/cmdline.txt || rc=1
