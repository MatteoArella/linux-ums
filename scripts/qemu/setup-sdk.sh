#!/usr/bin/env sh

#######################################################################
#
# This script setup the sdk for cross-compilation and qemu testing.
#
#######################################################################

set -e

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
ME=$(basename "$0")

usage() {
  cat <<EOF
Usage: $ME [OPTIONS]... -k KERNEL-VERSION -o OUTPUT-DIR

Setup linux KERNEL-VERSION compatible sdk and install it to OUTPUT-DIR.
Download it if the sdk tarball is not available inside CACHE-DIR.

Options:
    -c CACHE-DIR          use CACHE-DIR as directory for caching sdk tarballs
    -h                    show this help and exit
EOF
}

# aws cloudfront CDN
SDK_DOMAIN_URL="https://d56bcx24ngtij.cloudfront.net"
CACHE_DIR=$BASEDIR/targets

while getopts ":hk:o:c:" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  k) KERNEL_VERSION=$OPTARG ;;
  o) OUTPUT_DIR=$OPTARG ;;
  c) CACHE_DIR=$OPTARG ;;
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
[ -z ${OUTPUT_DIR+x} ] && (
  usage >&2
  exit 1
)

COMPRESSED_TARGET=x86_64-buildroot-linux-$KERNEL_VERSION.tar.xz

# check if sdk is already available
[ -d $OUTPUT_DIR/images ] && [ -d $OUTPUT_DIR/linux-$KERNEL_VERSION ] && exit 0

# download sdk tarball if it is not available locally
if [ ! -f $CACHE_DIR/$COMPRESSED_TARGET ]; then
  mkdir -p $CACHE_DIR
  wget -q -O $CACHE_DIR/$COMPRESSED_TARGET $SDK_DOMAIN_URL/$COMPRESSED_TARGET ||
    (
      echo "unable to download sdk $COMPRESSED_TARGET" >&2
      exit 1
    )
fi

# extract sdk tarball
mkdir -p $OUTPUT_DIR
tar xJf $CACHE_DIR/$COMPRESSED_TARGET -C $OUTPUT_DIR
