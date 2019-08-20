if [ "$linux_run" == "yes" ]; then
    sudo swapon -a
    ulimit -S -s $ulimit_s_org
fi

# Wait until memory release finishes and the thread is released
sleep $((mck_max_mem_size_110p / (5 * 1024 * 1024 * 1024)))
