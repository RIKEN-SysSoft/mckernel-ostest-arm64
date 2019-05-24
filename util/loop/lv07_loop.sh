count=1
rm -f $recordfile
touch ${lv07_tmp_before}
while [ $count -le $num_app_cpus ]
do
	eval $command_line | grep -o ".:${ostype_str}" | sort > ${lv07_tmp}
	diff ${lv07_tmp_before} ${lv07_tmp} >> $recordfile
	mv ${lv07_tmp} ${lv07_tmp_before}
	count=`expr $count + 1`
done
rm -f ${lv07_tmp_before}
