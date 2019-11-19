if [ "${linux_run}" != "yes" ]; then
	rc=0

	cmd='
		/memory_stat_rss/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/memory_stat_mapped_file/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/memory_max_usage/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/memory_kmem_usage/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/memory_kmem_max_usage/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/memory_numa_stat/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/cpuacct_stat_system/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/cpuacct_stat_user/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/cpuacct_usage / { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } }
		/cpuacct_usage_percpu/ { if ($2 == "0x0" || rshift(strtonum($2), 63) != 0) { rc=1; } } 
		/num_threads/ { if ($2 == "0x0" || rshift(strtonum($2), 31) != 0) { rc=1; } }
		/max_num_threads/ { if ($2 == "0x0" || rshift(strtonum($2), 31) != 0) { rc=1; } }
		END {
			print("rc: " rc);
			exit (rc == 0);
		}
	'

	if awk -F ' : ' "$cmd" $recordfile; then
	    rc=1
	fi
	
	echo $rc > $WORKDIR/result.log
fi
