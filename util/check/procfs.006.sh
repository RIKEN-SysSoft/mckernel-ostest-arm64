awk '/processor[^:]:/ {print}' $recordfile | sed 's/processor[^:]:[^0-9]*//' > $recorddir/result.txt
cpu_list > $recorddir/expected.txt
diff $recorddir/{result.txt,expected.txt} || rc=1
