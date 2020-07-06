awk 'NF >= 44 {print}' $recordfile > $recorddir/stat.txt
stat=($(cat $recorddir/stat.txt))
[[ ${stat[0]} =~ [0-9]+ ]] || rc=1
[[ ${stat[1]} =~ test_mck ]] || rc=1
[[ ${stat[2]} == S ]] || rc=1
(( ${stat[3]} == 1 )) || rc=1
[[ ${stat[4]} =~ [0-9]+ ]] || rc=1
(( stat[19] > 0 )) || rc=1
(( stat[38] > 0 )) || rc=1
