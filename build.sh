#!/bin/bash
pushd "$(dirname "$0")"
set -xe

RUN=1
NAME=http_server
CCFLAGS=

[[ ! -d .build ]] && mkdir .build

pushd .build
clang ../src/main.c $CCFLAGS -o $NAME.exe -I../src

if [ $RUN -eq 1 ] && [ -e $NAME.exe ]; then
  ./$NAME.exe
fi

popd
popd
