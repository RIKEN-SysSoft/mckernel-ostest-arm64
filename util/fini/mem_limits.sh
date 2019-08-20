if [ "$linux_run" == "yes" ]; then
    sudo swapon -a
fi

# Wait until memory release finishes and the thread is released
sleep $((mck_max_mem_size / (5 * 1024 * 1024 * 1024)))
