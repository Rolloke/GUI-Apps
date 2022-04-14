# Microsoft Developer Studio Project File - Name="AkuUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AkuUnit - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AkuUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AkuUnit.mak" CFG="AkuUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AkuUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AkuUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AkuUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/AkuUnit", BHNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

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
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib OEMGUI.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

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
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 CIPCDEBUG.lib OEMGUID.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AkuUnit___Win32_Enu"
# PROP BASE Intermediate_Dir "AkuUnit___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OEMGUI.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 CIPC.lib OEMGUI.lib /nologo /subsystem:windows /map /machine:I386

!ENDIF 

# Begin Target

# Name "AkuUnit - Win32 Release"
# Name "AkuUnit - Win32 Debug"
# Name "AkuUnit - Win32 Enu"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AkuUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\AkuUnitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CAku.cpp
# End Source File
# Begin Source File

SOURCE=.\CAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\CIo.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcInputAkuUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcOutputAkuUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\crelay.cpp
# End Source File
# Begin Source File

SOURCE=.\DAAku.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AkuDef.h
# End Source File
# Begin Source File

SOURCE=..\AkuSys\AkuIOctl.H
# End Source File
# Begin Source File

SOURCE=.\AkuReg.h
# End Source File
# Begin Source File

SOURCE=.\AkuUnit.h
# End Source File
# Begin Source File

SOURCE=.\AkuUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\CAku.h
# End Source File
# Begin Source File

SOURCE=.\CAlarm.h
# End Source File
# Begin Source File

SOURCE=.\Cio.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputAkuUnit.h
# End Source File
# Begin Source File

SOURCE=.\CipcOutputAkuUnit.h
# End Source File
# Begin Source File

SOURCE=.\CRelay.h
# End Source File
# Begin Source File

SOURCE=.\DAAku.h
# End Source File
# Begin Source File

SOURCE=.\Ini2Reg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AkuUnit.ico
# End Source File
# Begin Source File

SOURCE=.\AkuUnit.rc

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\AkuUnit.rc2
# End Source File
# Begin Source File

SOURCE=.\AkuUnitenu.rc

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AkuUnit.mak
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\AkuUnitenu.lng
# End Source File
# End Target
# End Project
