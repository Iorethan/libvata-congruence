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
    ${VATA} -r expl -t equiv "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  bisim-up)
    ${VATA} -r expl -t equiv -o "alg=bisimulation" "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  bisim-up-cached)
    ${VATA} -r expl -t equiv -o "alg=bisimulation,cache=yes" "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  congr-up)
    ${VATA} -r expl -t equiv -o "alg=bisimulation,congr=yes" "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  congr-up-cached)
    ${VATA} -r expl -t equiv -o "alg=bisimulation,congr=yes,cache=yes" "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  congr-up-cached-lax)
    ${VATA} -r expl -t equiv -o "alg=bisimulation,congr=yes,cache=yes,lax=yes" "${FILE_LHS}" "${FILE_RHS}"
    RETVAL="$?"
    ;;
  *) die "Invalid option ${OPERATION}"
    ;;
esac

exit ${RETVAL}
