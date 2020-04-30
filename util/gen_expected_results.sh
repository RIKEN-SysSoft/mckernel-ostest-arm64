#!/bin/bash

# Generate autotest related scripts and expected results
# Usage:
# ./run_testset.sh -d > /tmp/dryrun.out
# ./gen_autotest_scripts.awk /tmp/dryrun.out
# ./gen_expected_results.sh

SCRIPT_PATH=$(readlink -m "${BASH_SOURCE[0]}")
AUTOTEST_HOME="${SCRIPT_PATH%/*/*/*}"
. $AUTOTEST_HOME/bin/config.sh

while read file; do
    echo $file
    bash $DATADIR/scripts/$file -H
done < <(find $DATADIR/scripts -type f -name "ostest-*" | sed 's|.*/||' | grep -Fv -f blacklist)
