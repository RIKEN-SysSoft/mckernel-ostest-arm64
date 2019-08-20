. ${AUTOTEST_HOME}/ostest/util/check/result_is_ok.sh

if ! diff -q $cpuset_forked $cpuset_execed >/dev/null; then
	rc=1
fi
