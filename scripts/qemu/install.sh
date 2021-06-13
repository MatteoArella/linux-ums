#!/usr/bin/env bash

set -e
if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
  set -x
fi

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
ME=$(basename "$0")

usage() {
  cat <<EOF
Usage: $ME [OPTIONS]

Options:
    -i BUILD-DIR          use BUILD-DIR as input build directory
    -h                    show this help and exit
EOF
}

BUILDDIR=$BASEDIR/build

while getopts ":hi:" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  i) BUILDDIR=$OPTARG ;;
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

SDK_NAME=x86_64-buildroot-linux-gnu
SDK_PATH="$BUILDDIR/images/${SDK_NAME}_sdk-buildroot"
SYSROOT="$SDK_PATH/$SDK_NAME/sysroot"
TARGET_LIBDIR="/lib64"
TARGET_TESTDIR="/root"

export PATH=$PATH:$BASEDIR/scripts/qemu

# extract library SONAME from the symlink
LIBUMS_SYMLINK=$(readlink $BUILDDIR/src/lib/.libs/libums.so)
LIBUMS=$(readelf -d $BUILDDIR/src/lib/.libs/$LIBUMS_SYMLINK | grep -oP 'SONAME.*\K(?<=\[).*(?=\])')

qemu-bridge.sh push $BUILDDIR/src/lib/.libs/$LIBUMS $TARGET_LIBDIR 1>/dev/null
qemu-bridge.sh push $BUILDDIR/src/module/src/ums.ko $TARGET_TESTDIR 1>/dev/null
qemu-bridge.sh shell "insmod $TARGET_TESTDIR/ums.ko"
