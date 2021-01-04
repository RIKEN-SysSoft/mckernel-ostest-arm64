# show page[1-3]
corefile=`ls $recorddir | grep core | grep -v host | head -n1`
gdb -x $app_dir/madvise$tp_num.inf $app_dir/test_mck $recorddir/$corefile > $recorddir/gdb.out 2>&1

# addresses of page[1-3]
awk -F ' *' --non-decimal-data '/^[$]. = .*$/ { printf("%d\n", $3); }' $recorddir/gdb.out > $recorddir/dec_addrs
IFS=$'\r\n' GLOBIGNORE='*' command eval "dec_addrs=($(cat $recorddir/dec_addrs))"

# addresses of page[1-3]
awk -F ' *' --non-decimal-data '/^[$]. = .*$/ { printf("0x%016x\n", $3); }' $recorddir/gdb.out > $recorddir/hex_addrs
IFS=$'\r\n' GLOBIGNORE='*' command eval "hex_addrs=($(cat $recorddir/hex_addrs))"

# List mmap areas included in the dump
readelf -l $recorddir/$corefile | grep LOAD > $recorddir/readelf.out

# 3 pages are grouped when madvise is disabled
readelf -l $recorddir/$corefile | awk 'c > 0 { print $2; c--; } /LOAD/ && $3 ~ /'"${hex_addrs[0]}"'/ { c=1 }' > $recorddir/sizeofbuf.out
