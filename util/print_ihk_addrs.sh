#!/bin/sh
m=ihk
s=add-ihk-symbol-file
f=file-ihk

this_dir="$(cd $(dirname $0); pwd)"
"$this_dir/print_addrs.sh" -m $m -s $s -f $f
