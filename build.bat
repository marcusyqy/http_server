@echo off
setlocal
REM pushd /D "%~dp0"
pushd %~dp0

for %%a in (%*) do set "%%a=1"

if not exist .build mkdir .build

if "%run%"=="1" set RUN=1

set NAME=http_server
set CCFLAGS=-std=c11
REM -Werror -Wall -pedantic

pushd .build
if exist %NAME%.exe (
  del %NAME%.exe
)
echo --[[COMPILING]]--
clang ../build.c %CCFLAGS% -o %NAME%.exe -I../src
popd

if exist .build\%NAME%.exe (
pushd server
if "%RUN%" == "1" (
  echo --[[RUN]]--
  ..\.build\%NAME%.exe
)
popd
)

popd
