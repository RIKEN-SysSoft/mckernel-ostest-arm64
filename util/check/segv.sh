# Expecting SEGV
if [ $exit_status -ne 139 ]; then
    rc=1
fi
