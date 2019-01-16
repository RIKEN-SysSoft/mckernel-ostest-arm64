#!/bin/sh
m=mcctrl
s=add-mcctrl-symbol-file
f=file-mcctrl

this_dir="$(cd $(dirname $0); pwd)"
"$this_dir/print_addrs.sh" -m $m -s $s -f $f

