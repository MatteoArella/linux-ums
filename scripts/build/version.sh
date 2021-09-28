#!/usr/bin/env sh

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
VERSION_NUMBER=$(git describe --abbrev=7 --dirty --always --tags)

echo "Updating version."
echo "m4_define([VERSION_NUMBER], [$VERSION_NUMBER])" > $BASEDIR/m4/version.m4
