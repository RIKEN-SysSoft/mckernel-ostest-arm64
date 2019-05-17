#!/usr/bin/awk -f

function append_testscript(filename) {
    command = "cat " filename;
    while ((command | getline var) > 0) {
	print var >> testscript;
    }
    close(command);
}


BEGIN { 
    "pwd -P" | getline cwd;
    testlistfile = sprintf("%s/testlist", cwd);
    testcasedir = sprintf("%s/%s", cwd, "testcase"); 
    outputdir = sprintf("%s/%s", cwd, "output");
    system("rm -f " testlistfile);
    system("rm -rf " testcasedir);
    system("rm -rf " outputdir);
    system("mkdir -p " testcasedir);
    system("mkdir -p " outputdir);
    testname = "#";
}

/^##/ {
    testname = $2;
    count = 0;
    if (testname == "mem_stack_limits") {
	existScript = 1;
    } else {
	existScript = 0;
    }
    initialized = 1;
}

!/^##/ {
    testscript = sprintf("%s/%s.%03d", testcasedir, testname, count);
    outputfile = sprintf("%s/%s.%03d", outputdir, testname, count);
    count++;
    print "#!/bin/sh"  > testscript;
    append_testscript("before_run_testcase.sh");
    if (existScript) {
	append_testscript("before_" testname ".sh");
    }
    printf("%s > %s\n", $0, outputfile)  >> testscript;
    if (existScript) {
	append_testscript("after_" testname ".sh");
    }
    append_testscript("after_run_testcase.sh");
    system("chmod +x " testscript);

    print testscript >> testlistfile;
}
