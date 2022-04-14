@echo off
set TargetDir=%1
set TargetFile=%2

call D:\DevStudio\VC\Bin\VCVARS32.BAT x86
call D:\DDK\BIN\SETENV.BAT D:\DDK

set INCLUDE=%INCLUDE%;D:\DDK\INC

cd %TargetDir%

build

copy d:\ddk\lib\i386\free\%TargetFile% c:\winnt\system32\drivers


