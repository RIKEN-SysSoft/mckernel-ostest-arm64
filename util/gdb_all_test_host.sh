#!/bin/sh
# gdb_all_test_host.sh COPYRIGHT FUJITSU LIMITED 2016
this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

#
# check path
#
if [ ! -f "${gdb}" ]; then
  echo "gdb not found.(${gdb})"
  exit 1
fi
if [ ! -f "${gdb_host_sh}" ]; then
  echo "gdb_host.sh not found.(${gdb_host_sh})"
  exit 1
fi
if [ ! -f "${gdb_test_case_list}" ]; then
  echo "test case list not found.(${gdb_test_case_list})"
  exit 1
fi


###### Run GDB test cases ###################################
cd $gdb_dir
while read exp
do
	if [[ ${#exp} -eq 0 ]]; then
		continue
	fi

	echo $exp | grep -v '#.*' > /dev/null
	if [[ $? -eq 0 ]]; then
		echo "###### Start  testcase:${exp}"
		make check RUNTESTFLAGS="--verbose GDB=${gdb_host_sh} gdb.base/${exp}.exp"
		echo "###### Finish testcase:${exp}"
	fi
done < ${gdb_test_case_list}
