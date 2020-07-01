$ihkosctl 0 kmsg > $recorddir/kmsg
grep process_vm_writev $recorddir/kmsg || rc=1
