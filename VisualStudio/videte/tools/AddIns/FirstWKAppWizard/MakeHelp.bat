@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by WKAPPWIZARD.HPJ. >"hlp\WKAppWizard.hm"
echo. >>"hlp\WKAppWizard.hm"
echo // Dialogs (IDD_*) >>"hlp\WKAppWizard.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\WKAppWizard.hm"
echo. >>"hlp\WKAppWizard.hm"

REM -- Make help for Project WKAPPWIZARD

start /wait hcw /C /E /M "WKAppWizard.hpj"
if %errorlevel% == 1 goto :Error
if exist Debug\nul copy "WKAppWizard.hlp" Debug
if exist Release\nul copy "WKAppWizard.hlp" Release
goto :done

:Error
echo WKAppWizard.hpj(1) : error: Problem encountered creating help file

:done
echo.
