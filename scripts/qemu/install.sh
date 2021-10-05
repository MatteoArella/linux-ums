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
    -b BUILD-DIR          build directory
    -i INSTALL-DIR        temporary installation directory
    -h                    show this help and exit
EOF
}

BUILDDIR=$BASEDIR/build

while getopts ":hb:i:" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  b) BUILDDIR=$OPTARG ;;
  i) INSTALLDIR=$OPTARG ;;
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

INSTALLDIR=${INSTALLDIR:-"$BUILDDIR/install"}
TARGET_TESTDIR="/"

cd $BUILDDIR
make -j $(nproc) V=1 DESTDIR=$INSTALLDIR install
make -j $(nproc) V=1 DESTDIR=$INSTALLDIR/tests installcheck

$BASEDIR/scripts/qemu/qemu-bridge.sh push $INSTALLDIR/usr $TARGET_TESTDIR 1>/dev/null

if [ -d $INSTALLDIR/tests ]; then
  $BASEDIR/scripts/qemu/qemu-bridge.sh push $INSTALLDIR/tests/usr $TARGET_TESTDIR 1>/dev/null
fi

$BASEDIR/scripts/qemu/qemu-bridge.sh push $INSTALLDIR/ums.ko $TARGET_TESTDIRums.ko 1>/dev/null
$BASEDIR/scripts/qemu/qemu-bridge.sh shell "insmod $TARGET_TESTDIRums.ko"
