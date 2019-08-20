sleep 1
$ihkosctl 0 kmsg > $recorddir/kmsg
if ! grep page_fault_handler $recorddir/kmsg > /dev/null; then
    rc=1
fi
