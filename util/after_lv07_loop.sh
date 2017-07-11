work_str=`cat ${lv07_tmp}`
echo ${work_str} | grep -o ".:${ostype_str}" | sort > ${lv07_tmp}
diff ${lv07_tmp_before} ${lv07_tmp}
mv ${lv07_tmp} ${lv07_tmp_before}

rm -rf /tmp/ostest
rm -f ${lv07_tmp_before}
