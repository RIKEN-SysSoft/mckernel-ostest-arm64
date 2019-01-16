#!/bin/sh
m=ihk_smp_arm64
s=add-ihk-smp-symbol-file
f=file-ihk-smp

this_dir="$(cd $(dirname $0); pwd)"
"$this_dir/print_addrs.sh" -m $m -s $s -f $f
