# check procfs because eval makes mcexec a grandchild
pid=`$pidof_mcexec`
while true; do
    if [ ! -e /proc/$pid ]; then
	break;
    fi
    sleep 2
done
