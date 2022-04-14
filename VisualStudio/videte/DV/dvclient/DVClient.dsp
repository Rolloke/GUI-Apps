# Microsoft Developer Studio Project File - Name="DVClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DVClient - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DVClient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DVClient.mak" CFG="DVClient - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DVClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DVClient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DVClient - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "DVClient - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DV/DVClient", NDYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DVClient - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib vimage.lib skins.lib wkclasses.lib Version.lib DeviceDetect.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
TargetPath=.\Release\DVClient.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy /R /Y *.jpg v:\bin\DV\	xcopy $(TargetPath) c:\DV\ /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVClient - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebug.lib vimaged.lib skinsd.lib wkclassesdebug.lib Version.lib DeviceDetectD.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "DVClient - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DVClient___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "DVClient___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcdebug.lib vimaged.lib skinsd.lib wkclassesdebug.lib Version.lib DeviceDetectD.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebugU.lib vimagedU.lib skinsdU.lib wkclassesdebugU.lib Version.lib DeviceDetectDU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /map /debug /machine:I386 /pdbtype:sept /libpath:"v:\project\cipc\debugUnicode"

!ELSEIF  "$(CFG)" == "DVClient - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVClient___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "DVClient___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib vimage.lib skins.lib wkclasses.lib Version.lib DeviceDetect.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 cipcU.lib vimageU.lib skinsU.lib wkclassesU.lib Version.lib DeviceDetectU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"v:\project\cipc\ReleaseUnicode"
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
TargetPath=.\ReleaseUnicode\DVClient.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy /R /Y *.jpg v:\bin\DV\	xcopy $(TargetPath) c:\DV\ /R /C /F /Y
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DVClient - Win32 Release"
# Name "DVClient - Win32 Debug"
# Name "DVClient - Win32 DebugUnicode"
# Name "DVClient - Win32 ReleaseUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\alarmlistdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmObject.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupEraseMedium.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\CExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCAudioDVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\CKeyboardDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CMinimizedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\copyreadonlyversion.cpp
# End Source File
# Begin Source File

SOURCE=.\COSMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CRTEDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CStatisticDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CUpdateSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectCD.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\DVUIThread.cpp
# End Source File
# Begin Source File

SOURCE=.\HostsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InitPacketCD.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCDatabaseClientAlarmlist.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCDatabaseDVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCDataCarrierClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCInputDVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCOutputDVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCServerControlDVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\LangDllInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\LiveWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayRealTime.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryRectTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerAlarmlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\alarmlistdlg.h
# End Source File
# Begin Source File

SOURCE=.\AlarmObject.h
# End Source File
# Begin Source File

SOURCE=.\BackupEraseMedium.h
# End Source File
# Begin Source File

SOURCE=.\BackupWindow.h
# End Source File
# Begin Source File

SOURCE=.\CExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\CIPCAudioDVClient.h
# End Source File
# Begin Source File

SOURCE=.\CKeyboardDlg.h
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\CMinimizedDlg.h
# End Source File
# Begin Source File

SOURCE=.\copyreadonlyversion.h
# End Source File
# Begin Source File

SOURCE=.\COSMenu.h
# End Source File
# Begin Source File

SOURCE=.\CPanel.h
# End Source File
# Begin Source File

SOURCE=.\CRTEDlg.h
# End Source File
# Begin Source File

SOURCE=.\CStatisticDlg.h
# End Source File
# Begin Source File

SOURCE=.\CUpdateSystem.h
# End Source File
# Begin Source File

SOURCE=.\DirectCD.h
# End Source File
# Begin Source File

SOURCE=.\DisplayWindow.h
# End Source File
# Begin Source File

SOURCE=.\DVClient.h
# End Source File
# Begin Source File

SOURCE=.\DVUIThread.h
# End Source File
# Begin Source File

SOURCE=.\Enumerations.h
# End Source File
# Begin Source File

SOURCE=.\HostsDlg.h
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\InitPacketCD.h
# End Source File
# Begin Source File

SOURCE=.\IPCDatabaseClientAlarmlist.h
# End Source File
# Begin Source File

SOURCE=.\IPCDatabaseDVClient.h
# End Source File
# Begin Source File

SOURCE=.\IPCDataCarrierClient.h
# End Source File
# Begin Source File

SOURCE=.\IPCInputDVClient.h
# End Source File
# Begin Source File

SOURCE=.\IPCOutputDVClient.h
# End Source File
# Begin Source File

SOURCE=.\IPCServerControlDVClient.h
# End Source File
# Begin Source File

SOURCE=.\LangDllInfo.h
# End Source File
# Begin Source File

SOURCE=.\LiveWindow.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\PlayRealTime.h
# End Source File
# Begin Source File

SOURCE=.\PlayWindow.h
# End Source File
# Begin Source File

SOURCE=.\QueryRectTracker.h
# End Source File
# Begin Source File

SOURCE=.\RequestThread.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\LangDll\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\server.inl
# End Source File
# Begin Source File

SOURCE=.\ServerAlarmlist.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "Bitmaps"

# PROP Default_Filter "bmp"
# Begin Source File

SOURCE=.\res\adaptiveMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\alarmlis.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListActivity.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListDown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListDown2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListEnter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListExtern1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListExtern2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListPlay2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListPrint.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListUp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AlarmListUp2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\back2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\backspace.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clock.bmp
# End Source File
# Begin Source File

SOURCE=.\res\confirm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\del.bmp
# End Source File
# Begin Source File

SOURCE=.\res\del_all.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down_end.bmp
# End Source File
# Begin Source File

SOURCE=.\res\eject2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\enter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\enter2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\export.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ff2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\invert.bmp
# End Source File
# Begin Source File

SOURCE=.\res\left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minimize.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minimize_ac.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minimize_po.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minimized.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minimized_ac.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minimized_po.bmp
# End Source File
# Begin Source File

SOURCE=.\res\new.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pause2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\play2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\power.bmp
# End Source File
# Begin Source File

SOURCE=.\res\power2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\print.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rew2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\search.bmp
# End Source File
# Begin Source File

SOURCE=.\res\shift.bmp
# End Source File
# Begin Source File

SOURCE=.\res\single_b2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\single_f2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\skip_bac2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\skip_for2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stop2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\symbols.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up_start.bmp
# End Source File
# End Group
# Begin Group "Cursors"

# PROP Default_Filter "cur"
# Begin Source File

SOURCE=.\res\arrow.cur
# End Source File
# Begin Source File

SOURCE=.\res\arrow1.cur
# End Source File
# Begin Source File

SOURCE=.\res\arrow2.cur
# End Source File
# Begin Source File

SOURCE=.\Res\Arrow3.cur
# End Source File
# Begin Source File

SOURCE=.\res\arrow4.cur
# End Source File
# Begin Source File

SOURCE=.\res\glass.cur
# End Source File
# Begin Source File

SOURCE=.\res\panleft.cur
# End Source File
# Begin Source File

SOURCE=.\res\panright.cur
# End Source File
# Begin Source File

SOURCE=.\res\rect.cur
# End Source File
# Begin Source File

SOURCE=.\res\tiltdown.cur
# End Source File
# Begin Source File

SOURCE=.\res\tiltup.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoomin.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoomout.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\DVClient.ico
# End Source File
# Begin Source File

SOURCE=.\DVClient.rc
# End Source File
# Begin Source File

SOURCE=.\res\DVClient.rc2
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\Oem.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Group
# Begin Group "OemJpegs"

# PROP Default_Filter "jpg"
# Begin Source File

SOURCE=.\Alarmcom.jpg
# End Source File
# Begin Source File

SOURCE=.\dresearch.jpg
# End Source File
# Begin Source File

SOURCE=.\DTS.jpg
# End Source File
# Begin Source File

SOURCE=.\ProtectionOne.jpg
# End Source File
# Begin Source File

SOURCE=.\Santec.jpg
# End Source File
# End Group
# End Target
# End Project
