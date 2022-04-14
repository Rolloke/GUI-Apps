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

mc -c -v HACCmsg.mc 

build -w -b %2

goto %OS_VERSION%
:WinNT4
xcopy %TARGETPATH%\i386\checked\HAcc.sys %WINDIR%\system32\drivers /R /C /F	/Y
xcopy %TARGETPATH%\i386\checked\HAcc.sys .\Bin	/R /C /F /Y
goto end

:WinNT5
:WinXP
xcopy %TARGETPATH%\i386\HAcc.sys %WINDIR%\system32\drivers\ /R /C /F /Y
xcopy %TARGETPATH%\i386\HAcc.sys .\Bin\	/R /C /F /Y
rem xcopy %TARGETPATH%\i386\Savic.sys \\Martin02\Drivers	/R /C /F /Y
%SOFT_ICE% /TRANSLATE:PACKAGE,ALWAYS %TARGETPATH%\i386\HAcc.sys	/SOURCE:%_TargetFile% /SYMLOAD:%TARGETPATH%\i386\HAcc.sys,BREAK

:end






