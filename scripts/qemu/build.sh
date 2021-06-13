#!/usr/bin/env bash

#######################################################################
#
# This script builds the whole project for the qemu system.
#
#######################################################################

set -e
if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
  set -x
fi

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
ME=$(basename "$0")

usage() {
  cat <<EOF
Usage: $ME [OPTIONS] KERNEL-VERSION

Cross-build for qemu x86_64 with linux KERNEL-VERSION target system.

Options:
    -o BUILD-DIR          use BUILD-DIR as output build directory
    -h                    show this help and exit
EOF
}

BUILDDIR=$BASEDIR/build

while getopts ":ho:" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  o) BUILDDIR=$OPTARG ;;
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

KERNEL_VERSION="$1"

[ -z ${KERNEL_VERSION} ] && (
  usage >&2
  exit 1
)

SDK_NAME=x86_64-buildroot-linux-gnu_sdk-buildroot
SDK_PATH=$BUILDDIR/images/$SDK_NAME

[ ! -f "$BASEDIR/configure" ] && $BASEDIR/autogen.sh

$BASEDIR/scripts/qemu/setup-sdk.sh -k $KERNEL_VERSION -o $BUILDDIR
source $SDK_PATH/environment-setup
$SDK_PATH/relocate-sdk.sh
CONFIGURE_FLAGS="KERNELDIR=${KERNELDIR} $CONFIGURE_FLAGS"

mkdir -p $BUILDDIR
cd $BUILDDIR

$BASEDIR/configure $CONFIGURE_FLAGS --enable-examples

make -j $(nproc) V=1
