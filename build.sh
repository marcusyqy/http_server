#!/bin/sh

# Change to the directory of the script
cd "$(dirname "$0")" || exit 1
# set -xe
set -eu

# Declare variables based on arguments
for arg in "$@"; do
  eval "$arg=1"
done

# Check if the variable 'run' is set
if [ -n "${run+x}" ]; then
  RUN=1
else
  RUN=0
fi

NAME=http_server
CCFLAGS=-std=c11

# Create the .build directory if it doesn't exist
[ ! -d .build ] && mkdir .build

# Change to the .build directory
cd .build || exit 1
cc ../build.c $CCFLAGS -o $NAME -I../src

# Run the server if RUN is set to 1 and the executable exists
if [ "$RUN" -eq 1 ] && [ -e "$NAME" ]; then
  cd ../server || exit 1
  ./../.build/$NAME
fi

# Change back to the original directory
cd - || exit 1

