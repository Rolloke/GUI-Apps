# Microsoft Developer Studio Project File - Name="SavicDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SavicDll - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SavicDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SavicDll.mak" CFG="SavicDll - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SavicDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SavicDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SavicDll - Win32 Enu" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SavicDll - Win32 DebugUnicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SavicDll - Win32 ReleaseUnicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/Savic/SavicDll", BPRBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SavicDll - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 cipc.lib ddraw.lib wkclasses.lib ijl15.lib /nologo /base:"0x11300000" /subsystem:windows /dll /map /machine:I386 /out:"Release/Savic.dll"
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
TargetPath=.\release\Savic.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV /R /C /F /Y	xcopy $(TargetPath) c:\Security /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebug.lib ddraw.lib wkclassesdebug.lib ijl15.lib /nologo /base:"0x11300000" /subsystem:windows /dll /map /debug /machine:I386 /out:"Debug/SavicD.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\debug\SavicD.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) $(WINDIR)\System32 /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SavicDll___Win32_Enu"
# PROP BASE Intermediate_Dir "SavicDll___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib ddraw.lib wkclasses.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"Release/Savic.dll"
# ADD LINK32 cipc.lib ddraw.lib wkclasses.lib ijl15.lib /nologo /base:"0x11300000" /subsystem:windows /dll /map /machine:I386 /out:"Enu/Savic.dll"
# Begin Special Build Tool
TargetPath=.\Enu\Savic.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV /R /C /F /Y	xcopy $(TargetPath) c:\Security /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SavicDll___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "SavicDll___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcdebug.lib ddraw.lib wkclassesdebug.lib ijl15.lib /nologo /base:"0x11300000" /subsystem:windows /dll /map /debug /machine:I386 /out:"Debug/SavicD.dll" /pdbtype:sept
# ADD LINK32 cipcdebugU.lib ddraw.lib wkclassesdebugU.lib ijl15.lib /nologo /base:"0x11300000" /subsystem:windows /dll /map /debug /machine:I386 /out:"DebugUnicode/SavicDU.dll" /pdbtype:sept /libpath:"v:\project\cipc\debugunicode"
# Begin Special Build Tool
TargetPath=.\DebugUnicode\SavicDU.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) $(WINDIR)\System32 /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SavicDll___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "SavicDll___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib ddraw.lib wkclasses.lib ijl15.lib /nologo /base:"0x11300000" /subsystem:windows /dll /map /machine:I386 /out:"Release/Savic.dll"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 cipcu.lib ddraw.lib wkclassesu.lib ijl15.lib /nologo /base:"0x11300000" /subsystem:windows /dll /map /machine:I386 /out:"ReleaseUnicode/SavicU.dll" /libpath:"v:\project\cipc\ReleaseUnicode"
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
TargetPath=.\ReleaseUnicode\SavicU.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV /R /C /F /Y	xcopy $(TargetPath) c:\Security /R /C /F /Y
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "SavicDll - Win32 Release"
# Name "SavicDll - Win32 Debug"
# Name "SavicDll - Win32 Enu"
# Name "SavicDll - Win32 DebugUnicode"
# Name "SavicDll - Win32 ReleaseUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\Cbt878.cpp
# End Source File
# Begin Source File

SOURCE=.\CCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\CDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\Ci2c.cpp
# End Source File
# Begin Source File

SOURCE=.\CIo.cpp
# End Source File
# Begin Source File

SOURCE=.\CIOMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\CIOPort.cpp
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CMDPoints.cpp
# End Source File
# Begin Source File

SOURCE=.\CMotionDetection.cpp
# End Source File
# Begin Source File

SOURCE=.\CMyJpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\CPCF8563.cpp
# End Source File
# Begin Source File

SOURCE=.\CPCF8582.cpp
# End Source File
# Begin Source File

SOURCE=.\CPci.cpp
# End Source File
# Begin Source File

SOURCE=.\CRelay.cpp
# End Source File
# Begin Source File

SOURCE=.\CRisc.cpp
# End Source File
# Begin Source File

SOURCE=.\CVideoInlay.cpp
# End Source File
# Begin Source File

SOURCE=.\CWatchDog.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\MemCopy.cpp
# End Source File
# Begin Source File

SOURCE=.\SavicDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SavicTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CAlarm.h
# End Source File
# Begin Source File

SOURCE=.\Cbt878.h
# End Source File
# Begin Source File

SOURCE=.\CCodec.h
# End Source File
# Begin Source File

SOURCE=.\CDebug.h
# End Source File
# Begin Source File

SOURCE=.\CI2C.h
# End Source File
# Begin Source File

SOURCE=.\Cio.h
# End Source File
# Begin Source File

SOURCE=.\CIOMemory.h
# End Source File
# Begin Source File

SOURCE=.\CIOPort.h
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlgBase.h
# End Source File
# Begin Source File

SOURCE=.\CMDPoints.h
# End Source File
# Begin Source File

SOURCE=.\CMotionDetection.h
# End Source File
# Begin Source File

SOURCE=.\CMyJpeg.h
# End Source File
# Begin Source File

SOURCE=.\CPCF8563.h
# End Source File
# Begin Source File

SOURCE=.\CPCF8582.h
# End Source File
# Begin Source File

SOURCE=.\CPci.h
# End Source File
# Begin Source File

SOURCE=.\CRelay.h
# End Source File
# Begin Source File

SOURCE=.\CRisc.h
# End Source File
# Begin Source File

SOURCE=.\CVideoInlay.h
# End Source File
# Begin Source File

SOURCE=.\CWatchDog.h
# End Source File
# Begin Source File

SOURCE=.\DirectDraw.h
# End Source File
# Begin Source File

SOURCE=.\Dlgcolor.h
# End Source File
# Begin Source File

SOURCE=.\MemCopy.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SavicDefs.h
# End Source File
# Begin Source File

SOURCE=.\SavicLib.h
# End Source File
# Begin Source File

SOURCE=.\SavicProfile.h
# End Source File
# Begin Source File

SOURCE=.\SavicReg.h
# End Source File
# Begin Source File

SOURCE=.\SavicTrace.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\green.bmp
# End Source File
# Begin Source File

SOURCE=.\Mask.bmp
# End Source File
# Begin Source File

SOURCE=.\MDInlay.bmp
# End Source File
# Begin Source File

SOURCE=.\MDmask.bmp
# End Source File
# Begin Source File

SOURCE=.\red.bmp
# End Source File
# Begin Source File

SOURCE=.\SavicDll.rc

!IF  "$(CFG)" == "SavicDll - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\SavicDll.rc2
# End Source File
# Begin Source File

SOURCE=.\SavicDllenu.rc

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter "Lib"
# Begin Source File

SOURCE=.\release\Savic.dll
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\debug\SavicD.dll
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DebugUnicode\SavicDU.dll
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ReleaseUnicode\SavicU.dll
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\SavicDA\Debug\SavicDAD.lib

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SavicDA\Release\SavicDA.lib

!IF  "$(CFG)" == "SavicDll - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Release\Savic.lib

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Debug\SavicD.lib

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\savicda\DebugUnicode\SavicDADU.lib

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\savicda\ReleaseUnicode\SavicDAU.lib

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DebugUnicode\SavicDU.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ReleaseUnicode\SavicU.lib
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\Savic.def

!IF  "$(CFG)" == "SavicDll - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SavicD.def

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SavicDllenu.lng
# End Source File
# Begin Source File

SOURCE=.\savicdu.def

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\savicu.def

!IF  "$(CFG)" == "SavicDll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicDll - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
