#!/usr/bin/awk -f

function usage() {
    print "Create autotest scripts: awk -v target=\"autotest_scripts\" extestcase.awk"
    print "Create scripts to generate expected results: awk -v target=\"expected_results\" extestcase.awk"
}

BEGIN { 
    "pwd -P" | getline cwd;
    "dirname " ARGV[0] | getline dir;
    "cd " dir "/../.. && pwd -P" | getline autotest_home;

    switch(target) {
    case "autotest_scripts":
	scriptdir = sprintf("%s/data/script", autotest_home); 
	testlistfile = sprintf("%s/data/ostest-testlist", autotest_home);
	recorddir_base = "$WORKDIR/output";
	answerdir_base = "$DATADIR/linux";
	break;
    case "expected_results":
	scriptdir = sprintf("%s/data/linux/script", cwd); 
	system("rm -rf " scriptdir);
	system("mkdir -p " scriptdir);

	testlistfile = sprintf("%s/data/linux/ostest-testlist", cwd);
	system("rm -f " testlistfile);

	recorddir_base = sprintf("%s/data/linux", autotest_home);

	# Not used
	answerdir_base = sprintf("%s/data/linux", autotest_home);

	break;
    default:
	usage();
	break;
    }
    testname = "#";
}

/^##/ {
    testname = $2;
    testno = 0;
    initialized = 1;
}

!/^##/ {
    script_bn = sprintf("ostest-%s.%03d", testname, testno);
    script = sprintf("%s/%s", scriptdir, script_bn);
    recordfile = sprintf("%s/ostest-%s.%03d.output", recorddir_base, testname, testno);
    recorddir = sprintf("%s/ostest-%s.%03d", recorddir_base, testname, testno);
    answerfile = sprintf("%s/ostest-%s.%03d.output", answerdir_base, testname, testno);
    answerdir = sprintf("%s/ostest-%s.%03d", answerdir_base, testname, testno);

    print script;

    print "#!/bin/sh\n"  > script;

    print "# Define WORKDIR, DATADIR, MCKINSTALL etc." >> script;
    print ". ${AUTOTEST_HOME}/bin/config.sh\n" >> script;

    printf("testname=%s\n", testname) >> script;
    printf("testcase=%s.%03d\n", testname, testno) >> script;
    printf("testno=%d\n", testno) >> script;
    printf("recordfile=%s\n", recordfile) >> script;
    printf("recorddir=%s\n", recorddir) >> script;
    printf("answerfile=%s\n", answerfile) >> script;
    printf("answerdir=%s\n\n", answerdir) >> script;

    print "# Use recorddir and define mcexec, runHOST etc." >> script;
    print ". ${AUTOTEST_HOME}/ostest/util/init.sh\n" >> script;

    printf("command_line='%s'\n\n", $0) >> script;

    print(". ${AUTOTEST_HOME}/ostest/util/run.sh") >> script;

    system("chmod +x " script);

    print script_bn >> testlistfile;
    testno++;
}
