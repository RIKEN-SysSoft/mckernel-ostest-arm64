. ${AUTOTEST_HOME}/ostest/util/exec/background.sh
sleep 1

sudo "${app_dir}/freeze_thaw" 0 freeze
for i in `seq 1 3`
do
    sleep 1
    echo "sleep ${i} second elapsed."
done
sudo "${app_dir}/freeze_thaw" 0 thaw

. ${AUTOTEST_HOME}/ostest/util/exec/wait.sh
