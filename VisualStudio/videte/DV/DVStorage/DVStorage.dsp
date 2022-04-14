# Microsoft Developer Studio Project File - Name="DVStorage" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DVStorage - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DVStorage.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DVStorage.mak" CFG="DVStorage - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DVStorage - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DVStorage - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DVStorage - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "DVStorage - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "DVStorage - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "DVStorage - Win32 EnuUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DV/DVStorage", YBYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DVStorage - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib c4lib.lib wkclasses.lib vimage.lib /nologo /subsystem:windows /map /machine:I386
# Begin Special Build Tool
TargetPath=.\Release\DVStorage.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) c:\dv
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVStorage - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebug.lib c4libd.lib wkclassesdebug.lib vimaged.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "DVStorage - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVStorage___Win32_Enu"
# PROP BASE Intermediate_Dir "DVStorage___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib c4lib.lib wkclasses.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib c4lib.lib wkclasses.lib vimage.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DVStorage - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DVStorage___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "DVStorage___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcdebug.lib c4libd.lib wkclassesdebug.lib jpeglibd.lib h263encd.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebugU.lib c4libd.lib wkclassesdebugU.lib vimagedU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"V:\project\Cipc\DebugUnicode"

!ELSEIF  "$(CFG)" == "DVStorage - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVStorage___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "DVStorage___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib c4lib.lib wkclasses.lib jpeglib.lib h263enc.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipcU.lib c4lib.lib wkclassesU.lib vimageU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# Begin Special Build Tool
TargetPath=.\ReleaseUnicode\DVStorage.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) c:\dvX
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVStorage - Win32 EnuUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVStorage___Win32_EnuUnicode"
# PROP BASE Intermediate_Dir "DVStorage___Win32_EnuUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EnuUnicode"
# PROP Intermediate_Dir "EnuUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcU.lib c4lib.lib wkclassesU.lib jpeglibU.lib h263encU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# ADD LINK32 cipcU.lib c4lib.lib wkclassesU.lib vimageU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"

!ENDIF 

# Begin Target

# Name "DVStorage - Win32 Release"
# Name "DVStorage - Win32 Debug"
# Name "DVStorage - Win32 Enu"
# Name "DVStorage - Win32 DebugUnicode"
# Name "DVStorage - Win32 ReleaseUnicode"
# Name "DVStorage - Win32 EnuUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Backup.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupScanThread.cpp
# End Source File
# Begin Source File

SOURCE=.\backupthread.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcdatabasestorage.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrolstorage.cpp
# End Source File
# Begin Source File

SOURCE=.\CLIENT.CPP
# End Source File
# Begin Source File

SOURCE=.\Collection.cpp
# End Source File
# Begin Source File

SOURCE=.\Collections.cpp
# End Source File
# Begin Source File

SOURCE=.\DRIVES.CPP
# End Source File
# Begin Source File

SOURCE=.\DVStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\DVStorage.rc

!IF  "$(CFG)" == "DVStorage - Win32 Release"

!ELSEIF  "$(CFG)" == "DVStorage - Win32 Debug"

!ELSEIF  "$(CFG)" == "DVStorage - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVStorage - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "DVStorage - Win32 ReleaseUnicode"

!ELSEIF  "$(CFG)" == "DVStorage - Win32 EnuUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DVStorageDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DVStorageEnu.rc

!IF  "$(CFG)" == "DVStorage - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVStorage - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVStorage - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVStorage - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVStorage - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVStorage - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DVStorageView.cpp
# End Source File
# Begin Source File

SOURCE=.\finddata.cpp
# End Source File
# Begin Source File

SOURCE=.\Imagedata.cpp
# End Source File
# Begin Source File

SOURCE=.\InitializeThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\savethread.cpp
# End Source File
# Begin Source File

SOURCE=.\search.cpp
# End Source File
# Begin Source File

SOURCE=.\searchthread.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceHashEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\Sequences.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Tape.cpp
# End Source File
# Begin Source File

SOURCE=.\VerifyThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Backup.h
# End Source File
# Begin Source File

SOURCE=.\BackupScanThread.h
# End Source File
# Begin Source File

SOURCE=.\backupthread.h
# End Source File
# Begin Source File

SOURCE=.\cipcdatabasestorage.h
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrolstorage.h
# End Source File
# Begin Source File

SOURCE=.\CLIENT.H
# End Source File
# Begin Source File

SOURCE=.\Collection.h
# End Source File
# Begin Source File

SOURCE=.\Collections.h
# End Source File
# Begin Source File

SOURCE=.\DRIVES.H
# End Source File
# Begin Source File

SOURCE=.\DVStorage.h
# End Source File
# Begin Source File

SOURCE=.\DVStorageDoc.h
# End Source File
# Begin Source File

SOURCE=.\DVStorageView.h
# End Source File
# Begin Source File

SOURCE=.\finddata.h
# End Source File
# Begin Source File

SOURCE=.\Imagedata.h
# End Source File
# Begin Source File

SOURCE=.\InitializeThread.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\savethread.h
# End Source File
# Begin Source File

SOURCE=.\search.h
# End Source File
# Begin Source File

SOURCE=.\searchthread.h
# End Source File
# Begin Source File

SOURCE=.\SequenceHashEntry.h
# End Source File
# Begin Source File

SOURCE=.\Sequences.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Storageid.h
# End Source File
# Begin Source File

SOURCE=.\Tape.h
# End Source File
# Begin Source File

SOURCE=.\VerifyThread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DVStorage.ico
# End Source File
# Begin Source File

SOURCE=.\res\DVStorage.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DVStorageDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\DVStorageenu.lng
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\History.txt
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
