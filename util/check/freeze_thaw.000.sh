grep '\[client\] sending...\[9\]' $recordfile || rc=1
grep '\[server\] received\[9\]' $recordfile || rc=1
