@echo off
set _TargetFile=%1
set SOFT_ICE="C:\Programme\Developer\NuMega\SoftICE Driver Suite\SoftICE\NMSYM.EXE"
set TARGETPATH=.\objchk_wxp_x86

call %BASEDIR%\BIN\SETENV.BAT %BASEDIR% checked

echo ++++++++++++++++++
echo NTDEBUG=%NTDEBUG%
echo NTDEBUGTYPE=%NTDEBUGTYPE%
echo MSC_OPTIMIZATION=%MSC_OPTIMIZATION%
echo ++++++++++++++++++

d:

cd %_TargetFile%

mc -c -v TashaMsg.mc 

build -w -b %2

xcopy %TARGETPATH%\i386\Tasha.sys %WINDIR%\system32\drivers\ /R /C /F /Y
xcopy %TARGETPATH%\i386\Tasha.sys .\Bin\	/F /Y
%SOFT_ICE% /TRANSLATE:PACKAGE,ALWAYS %TARGETPATH%\i386\Tasha.sys	/SOURCE:%_TargetFile% /SYMLOAD:%TARGETPATH%\i386\Tasha.sys,BREAK







