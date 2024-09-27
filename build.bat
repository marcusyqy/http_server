@echo off
setlocal
cd /D "%~dp0"

if not exist .build mkdir .build

set RUN=1
set NAME=http_server
set CCFLAGS=
REM -Werror -Wall -pedantic

pushd .build
clang ../src/main.c %CCFLAGS% -o %NAME%.exe -I../src
if "%RUN%" == "1" %NAME%.exe
popd

