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
    -b BUILD-DIR          use BUILD-DIR as input build directory
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

INSTALLDIR=$BUILDDIR/install
EXAMPLESDIR=$BUILDDIR/examples
LOGDIR=${LOGDIR:-"$BUILDDIR/test"}
SDK_NAME=x86_64-buildroot-linux-gnu
SDK_PATH="$BUILDDIR/images/${SDK_NAME}_sdk-buildroot"
SYSROOT="$SDK_PATH/$SDK_NAME/sysroot"

export PATH="$SDK_PATH/bin:$SDK_PATH/sbin:$BASEDIR/scripts/qemu:$PATH"

qemu-bridge.sh start $BUILDDIR

trap "qemu-bridge.sh kill" EXIT

qemu-bridge.sh shell "uname -a"

install.sh -b $BUILDDIR -i $INSTALLDIR

mkdir -p $LOGDIR

readarray -d '' examples < <(find $EXAMPLESDIR/*/.libs -type f -print0)

for e in "${examples[@]}"; do
  EXAMPLE_NAME=$(basename $e)
  EXAMPLE_TIMESTAMP=$(date +%s)
  LOGPREFIX=$LOGDIR/$EXAMPLE_NAME-$EXAMPLE_TIMESTAMP
  GDB_LOGFILE=$LOGPREFIX.gdb.log

  set +e
  printf "\033[1;32m[TEST] %s\033[0m\n" $EXAMPLE_NAME

  timeout -s SIGINT $TEST_TIMEOUT x86_64-buildroot-linux-gnu-gdb -batch -q -n \
    -ex "target remote | qemu-bridge.sh shell 'gdbserver - $EXAMPLE_NAME'" \
    -ex "set pagination off" \
    -ex "set logging file $GDB_LOGFILE" \
    -ex "set logging on" \
    -ex "tb main" \
    -ex "c" \
    -ex "set solib-search-path $INSTALLDIR/usr/lib" \
    -ex "set sysroot $SYSROOT" \
    -ex "info sharedlibrary" \
    -ex "show solib-search-path" \
    -ex "show sysroot" \
    -ex "c" \
    -ex "thread apply all bt full" &> /dev/null
  EXIT_STATUS=$?
  set -e; set +x

  printf "\033[1;32mGDB:\033[0m\n"
  cat $GDB_LOGFILE

  printf "\n\033[1;32mDMESG:\033[0m\n"
  qemu-bridge.sh shell "dmesg | grep ums"

  if [ $EXIT_STATUS != 0 ]; then
    exit $EXIT_STATUS
  fi

  if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
    set -x
  fi
done
