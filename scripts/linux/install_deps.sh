#!/usr/bin/env bash

set -e
if [ ! -z "${CI+x}" ] || [ "$CI" = "true" ]; then
  set -x
fi

sudo apt-get update
sudo apt-get install -y \
  linux-headers-$(uname -r) \
  autotools-dev \
  autoconf \
  automake \
  libtool \
  pkg-config \
  gdb \
  cppcheck
