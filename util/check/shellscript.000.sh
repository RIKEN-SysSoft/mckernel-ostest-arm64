awk -f ${AUTOTEST_HOME}/ostest/util/check/shellscript.000.awk $recordfile > $recorddir/result_filtered.txt
awk -f ${AUTOTEST_HOME}/ostest/util/check/shellscript.000.awk $answerfile > $recorddir/answer_filtered.txt
diff $recorddir/result_filtered.txt $recorddir/answer_filtered.txt || rc=1
