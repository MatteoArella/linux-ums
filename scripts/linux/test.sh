#!/usr/bin/env bash

set -eo pipefail
if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
  set -x
fi

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
ME=$(basename "$0")

usage() {
  cat <<EOF
Usage: $ME [OPTIONS]...

Options:
    -b BUILD-DIR          use BUILD-DIR as input build directory
    -l LOG-DIR            output directory to place logfile
    -t TIMEOUT            test timeout in seconds
    -h                    show this help and exit
EOF
}

BUILDDIR=$BASEDIR/build
TESTDIR=$BUILDDIR/examples
LOGDIR=$BUILDDIR/test
TEST_TIMEOUT=120

while getopts ":hb:l:t:" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  b) BUILDDIR=$OPTARG ;;
  l) LOGDIR=$OPTARG ;;
  t) TEST_TIMEOUT=$OPTARG ;;
  \?)
    echo "unknown option $OPTARG!" >&2
    usage >&2
    exit 1
    ;;
  :)
    echo "missing value for option [-$OPTARG]" >&2
    usage >&2
    exit 1
    ;;
  esac
done
shift $(($OPTIND - 1))

$BASEDIR/scripts/linux/install.sh -i $BUILDDIR

mkdir -p $LOGDIR

readarray -d '' tests < <(find $TESTDIR/*/.libs -type f -print0)

for t in "${tests[@]}"; do
  TEST_NAME=$(basename $t)
  TEST_TIMESTAMP=$(date +%s)
  LOGPREFIX=$LOGDIR/$TEST_NAME-$TEST_TIMESTAMP
  GDB_LOGFILE=$LOGPREFIX.gdb.log
  TEST_LOGFILE=$LOGPREFIX.out.log
  set +e
  printf "\033[1;32m[TEST] %s\033[0m\n" $TEST_NAME

  timeout -s SIGINT $TEST_TIMEOUT gdb -batch -q -n \
    -ex "set pagination off" \
    -ex "set logging file $GDB_LOGFILE" \
    -ex "set logging on" \
    -ex "file $t" \
    -ex "run > $TEST_LOGFILE" \
    -ex "call setvbuf(stdout, 0, 2, 0)" \
    -ex "thread apply all bt full" &> /dev/null
  EXIT_STATUS=$?
  set -e; set +x

  printf "\033[1;32mOUTPUT:\033[0m\n"
  cat $TEST_LOGFILE

  if [ $EXIT_STATUS != 0 ]; then
    printf "\n\033[1;32mGDB:\033[0m\n"
    cat $GDB_LOGFILE

    printf "\n\033[1;32mDMESG:\033[0m\n"
    dmesg | grep ums

    exit $EXIT_STATUS
  fi

  if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
    set -x
  fi
done
