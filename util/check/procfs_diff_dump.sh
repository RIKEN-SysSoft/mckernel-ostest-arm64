awk '/0x[0-9a-f]*:/ {print}' $recordfile | sed 's/[ ]*0x[0-9a-f]*://' | tr ' ' '\n' > $recorddir/result.txt
awk '/0x[0-9a-f]*:/ {print}' $answerfile | sed 's/[ ]*0x[0-9a-f]*://' | tr ' ' '\n' > $recorddir/expected.txt
diff $recorddir/{result.txt,expected.txt} || rc=1
