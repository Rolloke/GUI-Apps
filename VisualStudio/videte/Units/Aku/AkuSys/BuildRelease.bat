@echo off
set _TargetFile=%1

call D:\WINNTDDK\BIN\SETENV.BAT D:\WINNTDDK free

cd %_TargetFile%

build %2

copy d:\WINNTDDK\lib\i386\free\aku.sys c:\winnt\system32\drivers
copy d:\WINNTDDK\lib\i386\free\aku.sys d:\dvrt
copy d:\WINNTDDK\lib\i386\free\aku.sys .\Bin




