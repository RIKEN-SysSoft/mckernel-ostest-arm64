grep -v range $recordfile | grep -oE '[0-9a-f]+[-][0-9a-f]+ [^ ]*' > $recorddir/procfs.txt
awk '/range: / {print}' $recordfile | sed 's/range: //' > $recorddir/mmap.txt
grep -xF -f $recorddir/mmap.txt $recorddir/procfs.txt > $recorddir/union.txt
diff $recorddir/{union.txt,mmap.txt} || rc=1
