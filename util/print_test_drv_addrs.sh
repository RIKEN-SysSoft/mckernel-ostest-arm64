#!/bin/sh
m=test_mck
s=add-test_drv-symbol-file
f=file-test_drv

this_dir="$(cd $(dirname $0); pwd)"
"$this_dir/print_addrs.sh" -m $m -s $s -f $f

