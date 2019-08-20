pgsize=`getconf PAGE_SIZE`

if [ $pgsize != 65536 ]; then
    . $AUTOTEST_HOME/ostest/util/result_is_ok.sh
fi
