#!/usr/bin/env sh

# get the initial version
sh scripts/build/version.sh > /dev/null

# add post-commit git hook that updates version number
GIT_HOOK_CMD="sh ./scripts/build/version.sh > /dev/null"
GIT_HOOK=".git/hooks/post-commit"

grep -qxF "$GIT_HOOK_CMD" $GIT_HOOK || echo $GIT_HOOK_CMD >> $GIT_HOOK

autoreconf -i -f
