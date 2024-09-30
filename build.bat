@echo off
setlocal
REM pushd /D "%~dp0"
pushd %~dp0

if not exist .build mkdir .build

set RUN=1
set NAME=http_server
set CCFLAGS=
REM -Werror -Wall -pedantic

pushd .build
if exist %NAME%.exe (
  del %NAME%.exe
)
echo --[[COMPILING]]--
clang ../src/main.c %CCFLAGS% -o %NAME%.exe -I../src
if "%RUN%" == "1" if exist %NAME%.exe (
  echo --[[RUN]]--
  %NAME%.exe
)
popd
popd
