declare -A status
awk '/ARGS/, /RESULT/ {print}' $recordfile > $recorddir/status.txt
while IFS=: read key value; do
    status[$key]=$value
done < $recorddir/status.txt

[[ "${status[Pid]}" =~ [0-9]+ ]] || rc=1
[[ "${status[Uid]}" =~ [0-9]+[[:space:]]+[0-9]+[[:space:]]+[0-9]+[[:space:]]+[0-9]+ ]] || rc=1
[[ "${status[Gid]}" =~ [0-9]+[[:space:]]+[0-9]+[[:space:]]+[0-9]+[[:space:]]+[0-9]+ ]] || rc=1
[[ "${status[State]}" =~ running ]] || rc=1
[[ "${status[VmLck]}" =~ [[:space:]]0[[:space:]]kB$ ]] || rc=1
[[ "${status[Threads]}" =~ [[:space:]]1$ ]] || rc=1

compare() {
    > $recorddir/${1}_result.txt
    while IFS=- read begin end; do
	[[ $end == "" ]] && end=$begin
	seq $begin $end >> $recorddir/${1}_result.txt
    done < <(echo ${status[$1]} | tr ',' '\n')
    diff $recorddir/${1}_{result,expected}.txt || rc=1
}

seq 0 $(($(ncpus) - 1)) | tr ' ' '\n' > $recorddir/Cpus_allowed_list_expected.txt
compare Cpus_allowed_list

numa_list > $recorddir/Mems_allowed_list_expected.txt
compare Mems_allowed_list
