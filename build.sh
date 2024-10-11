#!/bin/bash
pushd "$(dirname "$0")"
# set -xe
set -eu

for arg in "$@"; do declare $arg='1'; done

if [ -v run ]; then
  RUN=1
else
  RUN=0
fi

NAME=http_server
CCFLAGS=-std=c11

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
