#!/usr/bin/env sh

#######################################################################
#
# This script uploads the buildroot sdk to an AWS S3 bucket.
#
# Note: aws cli must be installed and configured on the system!
#
#######################################################################

set -e
if [ "$CI" = "true" ] || [ ! -z "${CI+x}" ]; then
  set -x
fi

SDK_PATH="$1"
SDK_TARBALL=$(basename $SDK_PATH)
BUCKET_NAME="aosv-buildroot-sdks"

# upload sdk tarball
aws s3 cp $SDK_PATH s3://$BUCKET_NAME

# update object ACL for public read
aws s3api put-object-acl --bucket $BUCKET_NAME --key $SDK_TARBALL --acl public-read
