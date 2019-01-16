#!/bin/sh
# gdb_host.sh COPYRIGHT FUJITSU LIMITED 2016
this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

exec $gdb "$@"
