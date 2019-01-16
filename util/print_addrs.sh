#!/bin/sh

usage()
{
	echo "$0 -m <module_name> -f <gdb-add-sym-function> -f <gdb-file-function>"
}

#
# parse args
#
mod=
sym=
file=
while getopts m:s:f: OPT
do
    case $OPT in
        m)  mod=$OPTARG
            ;;
        s)  sym=$OPTARG
            ;;
        f)  file=$OPTARG
            ;;
		h)  usage; exit 0
			;;
        \?) usage; exit 1
            ;;
    esac
done

if [ -z "$mod" -o -z "sym" -o -z "$file" ]; then
	usage
	exit 1
fi

#
# check
#
section_dir="/sys/module/$mod/sections"
if [ ! -d "$section_dir" ]; then
	echo "$section_dir is not found." >&2
	exit 1
fi

#
# get
#
text=`/bin/cat "$section_dir/.text"`
data=`/bin/cat "$section_dir/.data"`
bss=`/bin/cat "$section_dir/.bss"`

#
# print 
#
echo "== sections =="
echo ".text=$text"
echo ".data=$data"
echo ".bss =$bss"
echo "== GDB command =="
echo "$sym $text $bss $data"
echo "$file"
