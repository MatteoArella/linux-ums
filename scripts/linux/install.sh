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

cd $BUILDDIR
sudo make install
sudo ldconfig
