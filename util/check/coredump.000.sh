pushd $recorddir

corefiles=$(ls | grep core)

while true; do
    for corefile in $corefiles; do
	if ! (readelf -a $corefile | grep mcexec > /dev/null); then
	    break 2
	fi
    done
    rc=1
    break
done

# core file
type=$(readelf -a $corefile | awk -F " *" '/Type:/ {print $3}')
if [ "$type" != "CORE" ]; then
    rc=1
fi

if  ! (file $corefile | grep 'core file' > /dev/null); then
    rc=1
fi


gdb -x $app_dir/autorun.inf $app_dir/test_mck $corefile > gdb.out 2>&1

# buf_1
buf_1=$(awk -F " *" '/^buf_1/ {print $3}' $recordfile)
first_val=$(awk "/$buf_1/ "'{print $2}' gdb.out)
if [ $first_val != 0x0000000000000000 ]; then
    rc=1
fi

last_row=$(printf "0x%x" $((buf_1+240)))
last_val=$(awk "/$last_row/ "'{print $3}' gdb.out)
if [ $last_val != 0x000000000000001f ]; then
    rc=1
fi

# buf_2
first_row=$(grep '^$1 = ' gdb.out)
if ! (echo $first_row | grep '0xffffffffffffffff, 0xfffffffffffffffe' > /dev/null); then
    rc=1
fi

# back trace
first_row=$(grep '^#0 ' gdb.out)
if ! (echo $first_row | grep 'coredump0' > /dev/null); then
    rc=1
fi

# register values
x0=$(awk '/^x0/ {print $3}' gdb.out)
if [ "$x0" != "0" ]; then
    rc=1
fi

x28=$(awk '/^x28/ {print $3}' gdb.out)
if [ "$x28" != "28" ]; then
    rc=1
fi

popd
