@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by ISDNUNIT.HPJ. >"hlp\ISDNUnit.hm"
echo. >>"hlp\ISDNUnit.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\ISDNUnit.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\ISDNUnit.hm"
echo. >>"hlp\ISDNUnit.hm"
echo // Prompts (IDP_*) >>"hlp\ISDNUnit.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\ISDNUnit.hm"
echo. >>"hlp\ISDNUnit.hm"
echo // Resources (IDR_*) >>"hlp\ISDNUnit.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\ISDNUnit.hm"
echo. >>"hlp\ISDNUnit.hm"
echo // Dialogs (IDD_*) >>"hlp\ISDNUnit.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\ISDNUnit.hm"
echo. >>"hlp\ISDNUnit.hm"
echo // Frame Controls (IDW_*) >>"hlp\ISDNUnit.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\ISDNUnit.hm"
REM -- Make help for Project ISDNUNIT


echo Building Win32 Help files
start hcrtf -x "hlp\ISDNUnit.hpj"
echo Copying Help files to output directory
if exist Debug\Output\nul copy "hlp\ISDNUnit.hlp" Debug\Output
if exist Debug\Output\nul copy "hlp\ISDNUnit.cnt" Debug\Output
if exist Release\Output\nul copy "hlp\ISDNUnit.hlp" Release\Output
if exist Release\Output\nul copy "hlp\ISDNUnit.cnt" Release\Output
echo.


