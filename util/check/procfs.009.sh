awk '/cpu[0-9]*/ {print}' $recordfile | sed 's/cpu\([0-9]*\)/\1/' > $recorddir/result.txt
seq 0 $(($(ncpus) - 1)) | tr ' ' '\n' > $recorddir/expected.txt
diff $recorddir/{result.txt,expected.txt} || rc=1
