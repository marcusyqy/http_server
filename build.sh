#!/bin/bash
pushd "$(dirname "$0")"
set -xe

RUN=1
NAME=http_server
CCFLAGS=

[[ ! -d .build ]] && mkdir .build

pushd .build
clang ../src/main.c $CCFLAGS -o $NAME -I../src
popd

if [ $RUN -eq 1 ] && [ -e .build/$NAME ]; then
pushd server
  ./../.build/$NAME
popd
fi

popd
