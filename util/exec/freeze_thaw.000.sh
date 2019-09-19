eval $command_line &
pid=$!

sleep 1
sudo "${app_dir}/freeze_thaw" 0 freeze
sleep 3
sudo "${app_dir}/freeze_thaw" 0 thaw

timeout=10
while [ -e /proc/$pid ]; do
    sleep 1
    timeout=$((timeout - 1))
    if [ $timeout -le 0 ]; then
	kill $pid
	echo "RESULT: ng"
	break;
    fi
done

if [ $timeout -gt 0 ]; then
    echo "RESULT: ok"
fi
