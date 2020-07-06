nents=$(grep -c AT_ $recordfile)
(( nents != 10 )) && rc=1
