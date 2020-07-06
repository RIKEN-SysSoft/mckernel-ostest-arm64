awk 'NR != 1 {print}' $recordfile > $recorddir/result.txt
awk 'NR != 1 {print}' $answerfile > $recorddir/expected.txt
diff $recorddir/{result,expected}.txt || rc=1
