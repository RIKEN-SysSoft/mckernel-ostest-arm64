awk '/processor[^:]:/ {print}' $recordfile | sed 's/processor[^:]:[^0-9]*//' > $recorddir/result.txt
seq 0 $(($(ncpus) - 1)) > $recorddir/expected.txt
diff $recorddir/{result.txt,expected.txt} || rc=1
