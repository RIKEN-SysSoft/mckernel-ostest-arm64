. $AUTOTEST_HOME/ostest/util/check/madvise.common.sh

# Expecting SEGV on the line
# gdb -x /work/mcktest/ostest/util/../bin/madvise18.inf /work/mcktest/ostest/util/../bin/test_mck /work/mcktest/data/linux/ostest-madvise.016/core
# ...
# #0  0x0000000000422b8c in madvise18 (tc_num=18, tc_arg=0x481b28 <param>) at src/madvise/018.c:51
# 51		*nullp = 0;
# [Current thread is 1 (LWP 18021520)]
# $1 = 0x100000220000
# $2 = 0x100000230000
# $3 = 0x100000240000

if ! grep '\*nullp = 0;' $recorddir/gdb.out >/dev/null; then
    rc=1
fi

PAGE_SIZE=`getconf PAGE_SIZE`
if [ $((${dec_addrs[1]} - ${dec_addrs[0]})) -ne $PAGE_SIZE ] ||
       [ $((${dec_addrs[2]} - ${dec_addrs[1]})) -ne $PAGE_SIZE ]; then
    rc=1
fi

# Expecting 0x100000230000 to be missing
# readelf -l  /work/mcktest/work_development_development/output/ostest-madvise.016/core
#  LOAD           0x00000000003a0000 0x0000100000220000 0x0000000000000000
#                 0x0000000000010000 0x0000000000010000  RW     0x10000
#  LOAD           0x00000000003b0000 0x0000100000240000 0x0000000000000000
#                 0x0000000000010000 0x0000000000010000  RW     0x10000

# one vma with the size of 3 pages is dumped when madvise is not supported
if (( $(cat recorddir/sizeofbuf.out) != PAGE_SIZE * 3)); then
    rc=1
fi
