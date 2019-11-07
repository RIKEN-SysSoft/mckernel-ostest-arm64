linux_run="no"

while getopts H OPT
do
    case $OPT in
	H)
	    linux_run="yes"
	    ;;
    esac
done

# We need to feed -H to init.sh as well
OPTIND=1 
