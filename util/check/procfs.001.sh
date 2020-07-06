i=0;
while read hex; do
    [[ $hex =~ ^#|^[[:space:]]*$ ]] && continue

    hex=0x$hex
    (( hex != i)) && rc=1
    i=$((i + 1))
done < <(awk '/0x[0-9a-f]*:/ {print}' $recordfile | sed 's/[ ]*0x[0-9a-f]*://' | tr ' ' '\n')
