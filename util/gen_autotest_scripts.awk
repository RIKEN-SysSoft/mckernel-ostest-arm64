#!/usr/bin/awk -f

# Generate autotest related scripts and expected results
# Usage:
# ./run_testset.sh -d > /tmp/dryrun.out
# ./gen_autotest_scripts.awk /tmp/dryrun.out
# ./gen_expected_results.sh
#
# How to run individual tests:
#   McKernel run: bash -x /work/mcktest/data/script/ostest-<test_name>.<test_num>
#   Linux run: bash -x /work/mcktest/data/script/ostest-<test_name>.<test_num> -H

BEGIN { 
    "pwd -P" | getline cwd;
    "dirname " ARGV[0] | getline dir;
    "cd " dir "/../.. && pwd -P" | getline autotest_home;
    "cd " dir "/.. && pwd -P" | getline ostest_install;

    scriptdir = sprintf("%s/data/scripts", autotest_home);
    system("rm -f " scriptdir "/ostest-*");

    testname = "#";
}

/^##/ {
    testname = $2;
    testno = 0;
    initialized = 1;
}

!/^##/ {
    command_line = $0;
    script_bn = sprintf("ostest-%s.%03d", testname, testno);
    script = sprintf("%s/%s", scriptdir, script_bn);

    # print script;

    print "#!/bin/sh\n"  > script;

    print "# Define linux_run" >> script;
    printf(". %s/bin/linux_run.sh\n", ostest_install) >> script;

    print "# Define WORKDIR, DATADIR, MCKINSTALL etc." >> script;
    printf(". %s/bin/config.sh\n", autotest_home) >> script;

    # Switch recorddir and answerdir for McKernel run and Linux run

    printf("if [ \"${linux_run}\" != \"yes\" ]; then\n") >> script;

    recorddir_base = "$WORKDIR/output/mckernel";
    answerdir_base = "$DATADIR/linux";
    printf("\trecordfile=%s/ostest-%s.%03d.output\n", recorddir_base, testname, testno) >> script;
    printf("\trecorddir=%s/ostest-%s.%03d\n", recorddir_base, testname, testno) >> script;
    printf("\tanswerfile=%s/ostest-%s.%03d.output\n", answerdir_base, testname, testno) >> script;
    printf("\tanswerdir=%s/ostest-%s.%03d\n", answerdir_base, testname, testno) >> script;

    printf("else\n") >> script;

    recorddir_base = sprintf("%s/data/linux", autotest_home);
    printf("\trecordfile=%s/ostest-%s.%03d.output\n", recorddir_base, testname, testno) >> script;
    printf("\trecorddir=%s/ostest-%s.%03d\n", recorddir_base, testname, testno) >> script;
    
    printf("fi\n\n") >> script;


    printf("testname=%s\n", testname) >> script;
    printf("testcase=%s.%03d\n", testname, testno) >> script;
    printf("testno=%d\n", testno) >> script;


    if (match($command_line, /[-]n [0-9]+/)) {
	tp_num = substr($0, RSTART +  3, RLENGTH - 3);
	printf("tp_num=%s\n\n", tp_num) >> script;
    }

    printf("command_line='%s'\n\n", command_line) >> script;

    printf(". %s/bin/run.sh", ostest_install) >> script;

    system("chmod +x " script);

    testno++;
}
