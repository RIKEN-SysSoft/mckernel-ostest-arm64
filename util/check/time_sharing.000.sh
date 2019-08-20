for i in {0..4}; do
    scheduled=($(awk "/loop count=$i/ {print \$1}" $recordfile| xargs -r));
    if [ ${scheduled[0]} != "[child]" ] || [ ${scheduled[1]} != "[parent]" ]; then
	rc=1
    fi
done
