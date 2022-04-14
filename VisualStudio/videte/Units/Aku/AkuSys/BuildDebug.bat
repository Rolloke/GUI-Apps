@echo off
set _TargetFile=%1

call D:\WINNTDDK\BIN\SETENV.BAT D:\WINNTDDK	checked

cd %_TargetFile%

build %2

copy d:\WINNTDDK\lib\i386\checked\aku.sys c:\winnt\system32\drivers
copy d:\WINNTDDK\lib\i386\checked\aku.sys d:\dvrt
copy d:\WINNTDDK\lib\i386\checked\aku.sys .\Bin






