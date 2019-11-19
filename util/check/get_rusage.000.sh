if [ "${linux_run}" != "yes" ]; then
	rc=0

	cmd='
		/memory_stat_rss is all 0x0/ { found++; }
		/memory_stat_mapped_file is all 0x0/ { found++; }
		/memory_max_usage/ { print $2;if ($2 != "0x0") { rc=1; } }


		/memory_numa_stat is all 0x0/ { found++; }

		/cpuacct_stat_user/ { if ($2 != "0x0") { rc=1; } }
		/cpuacct_usage / { if ($2 != "0x0") { rc=1; } }
		/cpuacct_usage_percpu is all 0x0/ { found++; } 
		/num_threads/ { if ($2 != "0x0") { rc=1; } }
		/max_num_threads/ { if ($2 != "0x0") { rc=1; } }
		END {
			print("rc: " rc ", found: " found);
			exit (rc == 0 && found == 4);
		}
	'

	if awk -F ' : ' "$cmd" $recordfile; then
	    rc=1
	fi
	
	echo $rc > $WORKDIR/result.log
fi
