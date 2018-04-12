#!/bin/bash

################################# CONSTANTS ##################################

OPERATION=$1
FILE_LHS=$2
FILE_RHS=$3

# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=`readlink -f $0`

# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=`dirname $SCRIPT`

# VATA executable
VATA="${SCRIPTPATH}/../build/cli/vata"

# VATA executable
OLDVATA="${SCRIPTPATH}/old-vata"

# SFTA executable
#SFTA="${SCRIPTPATH}/sfta"
SFTA="${SCRIPTPATH}/sfta_wrapper.sh"

# TAlib executable
TALIB="${SCRIPTPATH}/talib"

################################# FUNCTIONS ##################################

# Function that terminates the script with a message
function die {
  echo "$1";
  exit -1;
}

################################## PROGRAM ###################################

if [ "$#" -ne 3 ]; then
  die "usage: $0 <method> <file1> <file2>"
fi

RETVAL="?"

ulimit -s 1000000

case "${OPERATION}" in
  double-incl)
    ${VATA} -r expl -t equiv -o order=breadth "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  congrup)
    ${VATA} -r expl -t equiv -o order=depth "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  *) die "Invalid option ${OPERATION}"
    ;;
esac

exit ${RETVAL}

