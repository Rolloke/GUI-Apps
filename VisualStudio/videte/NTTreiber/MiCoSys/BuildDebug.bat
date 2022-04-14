@echo off
set _TargetFile=%1

call D:\WINNTDDK\BIN\SETENV.BAT D:\WINNTDDK	checked

cd %_TargetFile%

build %2

copy d:\WINNTDDK\lib\i386\checked\mico.sys .\Kit
copy d:\WINNTDDK\lib\i386\checked\mico.dll .\Kit	



