#!/usr/bin/env bash

#######################################################################
#
# This script builds the buildroot sdk for cross-compiling the
# project and the images (kernel + rootfs) for the qemu system target.
#
#######################################################################

set -e
if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
  set -x
fi

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
WORKDIR=$(pwd)
ME=$(basename "$0")

BUILDROOT_HOME=$BASEDIR/buildroot
BUILDROOT_TARGET=qemu_x86_64_defconfig

usage() {
  cat <<EOF
Usage: $ME [OPTIONS]... -k KERNEL-VERSION [-o OUTPUT-DIR]

Build rootfs image and linux kernel image for x86_64 qemu target.
Additionally, build a SDK for cross-compiling for that target.
The images and the SDK are compatible for the specified linux KERNEL-VERSION.
All outputs are placed to OUTPUT-DIR.

Options:
    -h                    show this help and exit
EOF
}

OUTPUT_DIR=$BASEDIR/targets

while getopts ":hk:o:" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  k) KERNEL_VERSION=$OPTARG ;;
  o) OUTPUT_DIR=$OPTARG ;;
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

[ -z ${KERNEL_VERSION+x} ] && (
  usage >&2
  exit 1
)

SDK_NAME=x86_64-buildroot-linux-gnu_sdk-buildroot
COMPRESSED_TARGET=x86_64-buildroot-linux-$KERNEL_VERSION.tar.xz

if [ -z "$(ls -A $BUILDROOT_HOME)" ]; then
  git submodule update --init
fi

cd $BUILDROOT_HOME

make distclean
make $BUILDROOT_TARGET

$BASEDIR/qemu/configs/$BUILDROOT_TARGET $KERNEL_VERSION >.config-fragment
support/kconfig/merge_config.sh -m .config .config-fragment
make olddefconfig

# Build & package build environment
make -j $(nproc)
make -j $(nproc) sdk

cd output/images

# edit environment setup
tar xzf $SDK_NAME.tar.gz
rm $SDK_NAME.tar.gz

sed -i "/alias configure=/d" $SDK_NAME/environment-setup
sed -i "/export \"KERNELDIR=/c\export \"KERNELDIR=\$(dirname \$(dirname \$SDK_PATH))/linux-$KERNEL_VERSION\"" \
  $SDK_NAME/environment-setup
rm start-qemu.sh

cd $WORKDIR && mkdir -p $OUTPUT_DIR

tar cJf $OUTPUT_DIR/$COMPRESSED_TARGET \
  -C $BUILDROOT_HOME/output/build linux-$KERNEL_VERSION \
  -C $BUILDROOT_HOME/output images
