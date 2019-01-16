#!/bin/sh
# run_app.sh COPYRIGHT FUJITSU LIMITED 2013-2015
this_dir="$(cd $(dirname $0); pwd)"
source "${this_dir}/config"

usage()
{
  if [ -e ${ld_lib_path} ]; then
    echo "$0 [-e ENV=VALUE ...] program [program_args ...]"
  else
    echo "$0 program [program_args ...]"
  fi
  echo "$0 [-h]"
  echo "  options:"
  if [ -e ${ld_lib_path} ]; then
    echo "    -e  for mcexec add environ."
  fi
  echo "    -h  show usage."
}

if [ -e ${ld_lib_path} ]; then
  envopt="-e LD_LIBRARY_PATH=${ld_lib_path}"
else
  envopt=
fi

while getopts e:h OPT
do
  case $OPT in
    e)
      envopt="${envopt} -e ${OPTARG}"
      ;;
    h)
      usage
      exit 0
      ;;
  esac
done
shift `expr $OPTIND - 1`

if [ $# -eq 0 ]; then
  usage
  exit 1
fi

#
# check path
#
if [ ! -f "$mcexec" ]; then
  echo "mcexec not found.($mcexec)"
  exit 1
fi

if [ ! -d "$app_dir" ]; then
  echo "app directory not found.($app_dir)"
  exit 1
fi

#
# upload and execute a binary
#
"$mcexec" -c 0 ${envopt} $app_dir/$@
