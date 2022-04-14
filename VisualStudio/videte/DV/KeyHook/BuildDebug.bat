@echo off

set _TargetFile=%1
set SOFT_ICE="D:\Programme\Developer\NuMega\SoftICE Driver Suite\SoftICE\NMSYM.EXE"
set TARGETPATH=.\objchk

call %BASEDIR%\BIN\SETENV.BAT %BASEDIR% checked

echo ++++++++++++++++++
echo NTDEBUG=%NTDEBUG%
echo NTDEBUGTYPE=%NTDEBUGTYPE%
echo MSC_OPTIMIZATION=%MSC_OPTIMIZATION%
echo ++++++++++++++++++

cd %_TargetFile%

build -w -b %2

goto %OS_VERSION%
:WinNT4
xcopy %TARGETPATH%\i386\free\KeyHook.sys %WINDIR%\system32\drivers /R /C /F
xcopy %TARGETPATH%\i386\free\KeyHook.sys .\Bin	/R /C /F
goto end

:WinNT5
:WinXP
xcopy %TARGETPATH%\i386\KeyHook.sys %WINDIR%\system32\drivers\ /R /C /F /Y
xcopy %TARGETPATH%\i386\KeyHook.sys .\Bin\	/R /C /F /Y

:end





