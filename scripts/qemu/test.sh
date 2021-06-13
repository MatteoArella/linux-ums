#!/usr/bin/env bash

set -Eeo pipefail
if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
  set -x
fi

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
ME=$(basename "$0")

usage() {
  cat <<EOF
Usage: $ME [OPTIONS]...

Options:
    -b BUILD-DIR          source build directory
    -l LOG-DIR            output directory to place logfile
    -t TIMEOUT            test timeout in seconds
    -h                    show this help and exit
EOF
}

BUILDDIR=$BASEDIR/build
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

TESTDIR=$BUILDDIR/examples
LOGDIR=$BUILDDIR/test
SDK_NAME=x86_64-buildroot-linux-gnu
SDK_PATH="$BUILDDIR/images/${SDK_NAME}_sdk-buildroot"
SYSROOT="$SDK_PATH/$SDK_NAME/sysroot"
TARGET_LIBDIR="/lib64"
TARGET_TESTDIR="/root"

export PATH="$SDK_PATH/bin:$SDK_PATH/sbin:$BASEDIR/scripts/qemu:$PATH"

qemu-bridge.sh start $BUILDDIR

trap "qemu-bridge.sh kill" EXIT

qemu-bridge.sh shell "uname -a"

install.sh -i $BUILDDIR

mkdir -p $LOGDIR

readarray -d '' tests < <(find $TESTDIR/*/.libs -type f -print0)

for t in "${tests[@]}"; do
  TEST_NAME=$(basename $t)
  TEST_TIMESTAMP=$(date +%s)
  LOGPREFIX=$LOGDIR/$TEST_NAME-$TEST_TIMESTAMP
  GDB_LOGFILE=$LOGPREFIX.gdb.log

  qemu-bridge.sh push $t $TARGET_TESTDIR 1>/dev/null

  set +e
  printf "\033[1;32m[TEST] %s\033[0m\n" $TEST_NAME

  timeout -s SIGINT $TEST_TIMEOUT x86_64-buildroot-linux-gnu-gdb -batch -q -n \
    -ex "target remote | qemu-bridge.sh shell 'gdbserver - $TARGET_TESTDIR/$TEST_NAME'" \
    -ex "set pagination off" \
    -ex "set logging file $GDB_LOGFILE" \
    -ex "set logging on" \
    -ex "tb main" \
    -ex "c" \
    -ex "set solib-search-path $BUILDDIR/src/lib/.libs" \
    -ex "set sysroot $SYSROOT" \
    -ex "info sharedlibrary" \
    -ex "show solib-search-path" \
    -ex "show sysroot" \
    -ex "c" \
    -ex "thread apply all bt full" &> /dev/null
  EXIT_STATUS=$?
  set -e; set +x

  if [ $EXIT_STATUS != 0 ]; then
    printf "\033[1;32mGDB:\033[0m\n"
    cat $GDB_LOGFILE

    printf "\n\033[1;32mDMESG:\033[0m\n"
    qemu-bridge.sh shell "dmesg | grep ums"

    exit $EXIT_STATUS
  fi

  if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
    set -x
  fi
done
