# Expecting 128 + SIGUSR1
if [ $exit_status -le 128 ]; then
    rc=1
fi
