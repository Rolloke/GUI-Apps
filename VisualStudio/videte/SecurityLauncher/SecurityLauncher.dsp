# Microsoft Developer Studio Project File - Name="SecurityLauncher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SecurityLauncher - Win32 Ita
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SecurityLauncher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SecurityLauncher.mak" CFG="SecurityLauncher - Win32 Ita"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SecurityLauncher - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SecurityLauncher - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SecurityLauncher - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "SecurityLauncher - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/SecurityLauncher", ZDKAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "_SECURITY"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib cipc.lib wkclasses.lib oemgui.lib /nologo /subsystem:windows /pdb:none /map /machine:I386
# Begin Special Build Tool
TargetPath=.\Release\SecurityLauncher.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) c:\security
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /Ob1 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "_SECURITY"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 version.lib cipcdebug.lib wkclassesdebug.lib oemguid.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SecurityLauncher___Win32_Enu"
# PROP BASE Intermediate_Dir "SecurityLauncher___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "_SECURITY"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL" /d "_SECURITY"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib cipc.lib wkclasses.lib oemgui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 version.lib cipc.lib wkclasses.lib oemgui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SecurityLauncher___Win32_Ita"
# PROP BASE Intermediate_Dir "SecurityLauncher___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /Ob1 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "_SECURITY"
# ADD RSC /l 0x410 /d "_DEBUG" /d "_AFXDLL" /d "_SECURITY"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib cipcdebug.lib wkclassesdebug.lib oemguid.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386
# ADD LINK32 version.lib cipc.lib wkclasses.lib oemgui.lib /nologo /subsystem:windows /incremental:no /map /machine:I386
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "SecurityLauncher - Win32 Release"
# Name "SecurityLauncher - Win32 Debug"
# Name "SecurityLauncher - Win32 Enu"
# Name "SecurityLauncher - Win32 Ita"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AfterBackupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\APPBAR.CPP
# End Source File
# Begin Source File

SOURCE=.\Application.cpp
# End Source File
# Begin Source File

SOURCE=.\Applications.cpp
# End Source File
# Begin Source File

SOURCE=.\BackGroundWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\BeforeBackupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapTextButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCInputVirtualAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LauncherControl.cpp
# End Source File
# Begin Source File

SOURCE=.\LauncherError.cpp
# End Source File
# Begin Source File

SOURCE=.\LauncherUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\LauncherWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MeszMez.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\ScanChk.cpp
# End Source File
# Begin Source File

SOURCE=.\SecurityLauncher.cpp
# End Source File
# Begin Source File

SOURCE=.\ServiceDialog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Enu"

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UpdateRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\WatchDog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AfterBackupDialog.h
# End Source File
# Begin Source File

SOURCE=.\APPBAR.H
# End Source File
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=.\Applications.h
# End Source File
# Begin Source File

SOURCE=.\BackGroundWnd.h
# End Source File
# Begin Source File

SOURCE=.\BeforeBackupDialog.h
# End Source File
# Begin Source File

SOURCE=.\BitmapTextButton.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputVirtualAlarm.h
# End Source File
# Begin Source File

SOURCE=.\ErrorDialog.h
# End Source File
# Begin Source File

SOURCE=.\launcher.inl
# End Source File
# Begin Source File

SOURCE=.\LauncherControl.h
# End Source File
# Begin Source File

SOURCE=.\LauncherWnd.h
# End Source File
# Begin Source File

SOURCE=.\LoginDialog.h
# End Source File
# Begin Source File

SOURCE=.\MeszMez.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SecurityLauncher.h
# End Source File
# Begin Source File

SOURCE=.\ServiceDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UpdateRecord.h
# End Source File
# Begin Source File

SOURCE=.\WatchDog.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\alarmcom.ico
# End Source File
# Begin Source File

SOURCE=.\res\button_s.bmp
# End Source File
# Begin Source File

SOURCE=.\res\error_sy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\lbggarny.bmp
# End Source File
# Begin Source File

SOURCE=.\res\menu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SecurityLauncher.ico
# End Source File
# Begin Source File

SOURCE=.\SecurityLauncher.rc

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Ita"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\SecurityLauncher.rc2
# End Source File
# Begin Source File

SOURCE=.\SecurityLauncherEnu.rc

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SecurityLauncherIta.rc

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Ita"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarGarny.bmp
# End Source File
# Begin Source File

SOURCE=.\videte_logo_invers_transparent.bmp
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter "txt; lng"
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\SecurityLauncherEnu.lng
# End Source File
# Begin Source File

SOURCE=.\securitylauncherIta.lng
# End Source File
# End Group
# End Target
# End Project
