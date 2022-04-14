# Microsoft Developer Studio Project File - Name="DBS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DBS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DBS.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DBS.MAK" CFG="DBS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DBS - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DBS - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DBS - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "DBS - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/DBS", FWMBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DBS - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib oemgui.lib c4lib.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile /debug
# Begin Special Build Tool
TargetPath=.\Release\DBS.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) c:\security
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DBS - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebug.lib oemguid.lib c4libd.lib wkclassesdebug.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "DBS - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DBS___Win32_Enu"
# PROP BASE Intermediate_Dir "DBS___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib c4lib.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile /debug
# ADD LINK32 cipc.lib oemgui.lib c4lib.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile /debug

!ELSEIF  "$(CFG)" == "DBS - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DBS___Win32_Ita"
# PROP BASE Intermediate_Dir "DBS___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib c4lib.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile /debug
# ADD LINK32 cipc.lib oemgui.lib c4lib.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile /debug

!ENDIF 

# Begin Target

# Name "DBS - Win32 Release"
# Name "DBS - Win32 Debug"
# Name "DBS - Win32 Enu"
# Name "DBS - Win32 Ita"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Archiv.cpp
# End Source File
# Begin Source File

SOURCE=.\Archivs.cpp
# End Source File
# Begin Source File

SOURCE=.\Backup.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupThread.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCServerControlDBS.cpp
# End Source File
# Begin Source File

SOURCE=.\Client.cpp
# End Source File
# Begin Source File

SOURCE=.\Clients.cpp
# End Source File
# Begin Source File

SOURCE=.\Conversion.cpp
# End Source File
# Begin Source File

SOURCE=.\ConversionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Converter.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertThread.cpp
# End Source File
# Begin Source File

SOURCE=.\copyreadonlyversion.cpp
# End Source File
# Begin Source File

SOURCE=.\DBS.cpp
# End Source File
# Begin Source File

SOURCE=.\DBS.rc

!IF  "$(CFG)" == "DBS - Win32 Release"

!ELSEIF  "$(CFG)" == "DBS - Win32 Debug"

!ELSEIF  "$(CFG)" == "DBS - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DBS - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DBSDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DBSId.cpp
# End Source File
# Begin Source File

SOURCE=.\DBSView.cpp
# End Source File
# Begin Source File

SOURCE=.\Drives.cpp
# End Source File
# Begin Source File

SOURCE=.\InitializeThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\oldpictdata.cpp
# End Source File
# Begin Source File

SOURCE=.\Query.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryThread.cpp
# End Source File
# Begin Source File

SOURCE=.\RHDDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveData.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Sequence.cpp
# End Source File
# Begin Source File

SOURCE=.\Sequences.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VDBCameraMap.cpp
# End Source File
# Begin Source File

SOURCE=.\VDBSequenceHashEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\VerifyThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32FindData.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Archiv.h
# End Source File
# Begin Source File

SOURCE=.\Archivs.h
# End Source File
# Begin Source File

SOURCE=.\Backup.h
# End Source File
# Begin Source File

SOURCE=.\BackupThread.h
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseServer.h
# End Source File
# Begin Source File

SOURCE=.\CIPCServerControlDBS.h
# End Source File
# Begin Source File

SOURCE=.\Client.h
# End Source File
# Begin Source File

SOURCE=.\Clients.h
# End Source File
# Begin Source File

SOURCE=.\Conversion.h
# End Source File
# Begin Source File

SOURCE=.\ConversionDialog.h
# End Source File
# Begin Source File

SOURCE=.\Converter.h
# End Source File
# Begin Source File

SOURCE=.\ConvertThread.h
# End Source File
# Begin Source File

SOURCE=.\copyreadonlyversion.h
# End Source File
# Begin Source File

SOURCE=.\DBS.h
# End Source File
# Begin Source File

SOURCE=.\DBSDoc.h
# End Source File
# Begin Source File

SOURCE=.\DBSId.h
# End Source File
# Begin Source File

SOURCE=.\DBSView.h
# End Source File
# Begin Source File

SOURCE=.\Drives.h
# End Source File
# Begin Source File

SOURCE=.\InitializeThread.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\oldpictdata.h
# End Source File
# Begin Source File

SOURCE=.\Query.h
# End Source File
# Begin Source File

SOURCE=.\QueryThread.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RHDDialog.h
# End Source File
# Begin Source File

SOURCE=.\SaveData.h
# End Source File
# Begin Source File

SOURCE=.\SaveThread.h
# End Source File
# Begin Source File

SOURCE=.\Sequence.h
# End Source File
# Begin Source File

SOURCE=.\Sequences.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VDBCameraMap.h
# End Source File
# Begin Source File

SOURCE=.\VDBSequenceHashEntry.h
# End Source File
# Begin Source File

SOURCE=.\VerifyThread.h
# End Source File
# Begin Source File

SOURCE=.\Win32FindData.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DBS.ico
# End Source File
# Begin Source File

SOURCE=.\res\DBS.rc2
# End Source File
# Begin Source File

SOURCE=.\DBSenu.rc

!IF  "$(CFG)" == "DBS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DBS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DBS - Win32 Enu"

# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409

!ELSEIF  "$(CFG)" == "DBS - Win32 Ita"

# PROP Exclude_From_Build 1
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DBSIta.rc

!IF  "$(CFG)" == "DBS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DBS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DBS - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DBS - Win32 Ita"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\DBSenu.lng
# End Source File
# Begin Source File

SOURCE=.\dbsIta.lng
# End Source File
# Begin Source File

SOURCE=.\history.txt
# End Source File
# End Target
# End Project
