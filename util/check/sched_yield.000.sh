awk '/parent/ && /End/ { p = NR } /child/ && /End/ { c = NR } END { exit c < p ? 0 : 1; }' $recordfile || rc=1
