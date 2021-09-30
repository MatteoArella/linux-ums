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
    -o BUILD-DIR          use BUILD-DIR as output build directory
    -p PUBLISH-DIR        use PUBLISH-DIR as publish directory
    -h                    show this help and exit
EOF
}

BUILDDIR=$BASEDIR/build
PUBLISHDIR=$BUILDDIR/public
VERSION_NUMBER=$($BASEDIR/scripts/build/version.sh)

while getopts ":ho:p:" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  o) BUILDDIR=$OPTARG ;;
  p) PUBLISHDIR=$OPTARG ;;
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

[ ! -f "$BASEDIR/configure" ] && $BASEDIR/autogen.sh

mkdir -p $BUILDDIR

# make pdf two times for preventing any issues
docker run -t --rm -u $UID -v $BASEDIR:$BASEDIR -w $BUILDDIR arella/aosv-latexpdf /bin/bash -c \
  "$BASEDIR/configure --enable-docs && \
  make -j $(nproc) V=1 html && \
  make -j $(nproc) V=1 pdf && \
  make -j $(nproc) V=1 pdf"

cp -R $BUILDDIR/doc/html $PUBLISHDIR
cp $BUILDDIR/doc/latex/linuxusermodescheduling.pdf $PUBLISHDIR

cd $BUILDDIR/doc/html
tar cvf $BUILDDIR/linux-ums-$VERSION_NUMBER-html.tar.gz .
cd $BUILDDIR/doc/latex
tar cvf $BUILDDIR/linux-ums-$VERSION_NUMBER-pdf.tar.gz linuxusermodescheduling.pdf
