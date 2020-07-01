noks=$(grep -c pause000_sig_handle $recordfile)
(( noks == 0 )) && rc=1
