# Microsoft Developer Studio Project File - Name="DVStarter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DVStarter - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DVStarter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DVStarter.mak" CFG="DVStarter - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DVStarter - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DVStarter - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DVStarter - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "DVStarter - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DV/DVStarter", YGYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DVStarter - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib skins.lib wkclasses.lib Version.lib HardwareInfo.lib /nologo /subsystem:windows /map /machine:I386
# Begin Special Build Tool
WkspDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy /R /Y $(WkspDir)\DVDongle.dcf v:\bin\DV	xcopy /R /Y $(WkspDir)\*.bmp v:\bin\DV
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVStarter - Win32 Debug"

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
# ADD LINK32 cipcdebug.lib skinsd.lib wkclassesdebug.lib Version.lib HardwareInfoD.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
OutDir=.\Debug
WkspDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=copy dvdongle.dcf
PostBuild_Cmds=xcopy                           $(WkspDir)\dvdongle.dcf                           $(OutDir)\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVStarter - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DVStarter___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "DVStarter___Win32_DebugUnicode"
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
# ADD BASE LINK32 cipcdebug.lib skinsd.lib wkclassesdebug.lib Version.lib HardwareInfoD.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebugU.lib skinsdU.lib wkclassesdebugU.lib Version.lib HardwareInfoDU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /debug /machine:I386 /pdbtype:sept /libpath:"V:\project\Cipc\DebugUnicode"
# Begin Special Build Tool
OutDir=.\DebugUnicode
WkspDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=copy dvdongle.dcf
PostBuild_Cmds=xcopy                           $(WkspDir)\dvdongle.dcf                           $(OutDir)\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVStarter - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVStarter___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "DVStarter___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib skins.lib wkclasses.lib Version.lib HardwareInfo.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipcu.lib skinsu.lib wkclassesu.lib Version.lib HardwareInfou.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# Begin Special Build Tool
WkspDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy /R /Y $(WkspDir)\DVDongle.dcf v:\bin\DVX	xcopy /R /Y $(WkspDir)\*.bmp v:\bin\DVX
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DVStarter - Win32 Release"
# Name "DVStarter - Win32 Debug"
# Name "DVStarter - Win32 DebugUnicode"
# Name "DVStarter - Win32 ReleaseUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CAboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\CInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\CRTErrorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CWatchDog.cpp
# End Source File
# Begin Source File

SOURCE=.\DVStarter.cpp
# End Source File
# Begin Source File

SOURCE=.\DVStarterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Piezo.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanChk.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CAboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\CApplication.h
# End Source File
# Begin Source File

SOURCE=.\CRTErrorDlg.h
# End Source File
# Begin Source File

SOURCE=.\CWatchDog.h
# End Source File
# Begin Source File

SOURCE=.\DVStarter.h
# End Source File
# Begin Source File

SOURCE=.\DVStarterDlg.h
# End Source File
# Begin Source File

SOURCE=.\Piezo.h
# End Source File
# Begin Source File

SOURCE=.\PSAPI.H
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\alarmcom.ico
# End Source File
# Begin Source File

SOURCE=.\res\arrow.cur
# End Source File
# Begin Source File

SOURCE=.\res\confirm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DVStarter.ico
# End Source File
# Begin Source File

SOURCE=.\DVStarter.rc
# End Source File
# Begin Source File

SOURCE=.\res\DVStarter.rc2
# End Source File
# Begin Source File

SOURCE=.\res\rterror.bmp
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DVDongle.dcf
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\TimeZonesNT.reg
# End Source File
# Begin Source File

SOURCE=.\TimeZonesRegXP.reg
# End Source File
# End Group
# Begin Group "OemBmp"

# PROP Default_Filter "bmp"
# Begin Source File

SOURCE=.\Alarmcom9x.bmp
# End Source File
# Begin Source File

SOURCE=.\AlarmcomNT.bmp
# End Source File
# Begin Source File

SOURCE=.\DResearch9x.bmp
# End Source File
# Begin Source File

SOURCE=.\DResearchNT.bmp
# End Source File
# Begin Source File

SOURCE=.\DTS9x.bmp
# End Source File
# Begin Source File

SOURCE=.\DTSNT.bmp
# End Source File
# Begin Source File

SOURCE=.\Santec9x.bmp
# End Source File
# Begin Source File

SOURCE=.\SantecNT.bmp
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=.\PSAPI.LIB
# End Source File
# End Group
# End Target
# End Project
