# Microsoft Developer Studio Project File - Name="SDIConfig" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SDIConfig - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SDIConfig.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SDIConfig.mak" CFG="SDIConfig - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SDIConfig - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SDIConfig - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Clients/SDIConfig", OXVAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SDIConfig - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 WKClasses.lib cipc.lib oemgui.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "SDIConfig - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 WKClassesDebug.lib cipcdebug.lib oemguid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SDIConfig - Win32 Release"
# Name "SDIConfig - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfig.rc
# End Source File
# Begin Source File

SOURCE=.\SDIConfigCoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigInput.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigKebaDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigKebaView.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigView.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIMultiDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ConnectionDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfig.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigCoolBar.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigDoc.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigInput.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigKebaDoc.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigKebaView.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigRecord.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigView.h
# End Source File
# Begin Source File

SOURCE=.\SDIMultiDocTemplate.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cold.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ConfigFailed.avi
# End Source File
# Begin Source File

SOURCE=.\res\ConfigOK.avi
# End Source File
# Begin Source File

SOURCE=.\res\Configure.avi
# End Source File
# Begin Source File

SOURCE=.\res\Connect.avi
# End Source File
# Begin Source File

SOURCE=.\res\ConnectFailed.avi
# End Source File
# Begin Source File

SOURCE=.\res\ExternProgram.avi
# End Source File
# Begin Source File

SOURCE=.\res\FileTransfer.avi
# End Source File
# Begin Source File

SOURCE=.\res\GetInfo.avi
# End Source File
# Begin Source File

SOURCE=.\res\HasInfo.avi
# End Source File
# Begin Source File

SOURCE=.\res\HasNoInfo.avi
# End Source File
# Begin Source File

SOURCE=.\res\hot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Idle.avi
# End Source File
# Begin Source File

SOURCE=.\res\SDIConfig.ico
# End Source File
# Begin Source File

SOURCE=.\res\SDIConfig.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SDIConfigDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar_old.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TryConfig.avi
# End Source File
# End Group
# Begin Source File

SOURCE=.\history.txt
# End Source File
# End Target
# End Project
