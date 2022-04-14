@echo off
set _TargetFile=%1

call D:\MSDEV\BIN\VCVARS32.BAT x86
call D:\DDK\BIN\SETENV.BAT D:\DDK

set INCLUDE=%INCLUDE%;D:\DDK\INC

cd %_TargetFile%

build

copy d:\ddk\lib\i386\free\genport.sys c:\winnt\system32\drivers


