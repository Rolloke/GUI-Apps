# Microsoft Developer Studio Project File - Name="LangDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LangDll - Win32 RusDebugU
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LangDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LangDll.mak" CFG="LangDll - Win32 RusDebugU"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LangDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Enu" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 EnuDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Esp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Fra" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Ita" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Deu" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Csy" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Nld" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Plk" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Sve" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 CsyDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Nor" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Wls" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 Ptg" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 DeuU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 ChsU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 DebugU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 ChsDebugU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 EnuDebugU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 EnuU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 EspU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 FraU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 ItaU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 CsyU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 NldU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 PlkU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 SveU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 NorU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 PtgU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 RusU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LangDll - Win32 RusDebugU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DV/LangDll", FWZBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LangDll - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"Debug/DVDeuDebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\Debug\DVDeuDebug.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\LibsD\_and_to_..\DVClient\Debug\ 
PostBuild_Cmds=copy $(TargetPath) ..\dvclient\Debug	copy $(TargetPath) .\libsD
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Enu"
# PROP BASE Intermediate_Dir "Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Enu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Release/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Enu/DVEnu.dll"
# Begin Special Build Tool
TargetPath=.\Enu\DVEnu.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "EnuDebug"
# PROP BASE Intermediate_Dir "EnuDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "EnuDebug"
# PROP Intermediate_Dir "EnuDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"Debug/DVDeuDebug.dll" /pdbtype:sept
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"EnuDebug/DVEnuDebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\EnuDebug\DVEnuDebug.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\LibsD\_and_to_..\DVClient\Debug\ 
PostBuild_Cmds=copy $(TargetPath) ..\dvclient\Debug	copy $(TargetPath) .\libsD
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Esp"
# PROP BASE Intermediate_Dir "LangDll___Win32_Esp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Esp"
# PROP Intermediate_Dir "Esp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ESP" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc0a /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Release/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Esp/DVEsp.dll"
# Begin Special Build Tool
TargetPath=.\Esp\DVEsp.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Fra"
# PROP BASE Intermediate_Dir "LangDll___Win32_Fra"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Fra"
# PROP Intermediate_Dir "Fra"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_FRA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Release/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Fra/DVFra.dll"
# Begin Special Build Tool
TargetPath=.\Fra\DVFra.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Ita"
# PROP BASE Intermediate_Dir "LangDll___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Ita"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ITA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Release/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Ita/DVIta.dll"
# Begin Special Build Tool
TargetPath=.\Ita\DVIta.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Deu"
# PROP BASE Intermediate_Dir "LangDll___Win32_Deu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Deu"
# PROP Intermediate_Dir "Deu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Release/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Deu/DVDeu.dll"
# Begin Special Build Tool
TargetPath=.\Deu\DVDeu.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Csy"
# PROP BASE Intermediate_Dir "LangDll___Win32_Csy"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Csy"
# PROP Intermediate_Dir "Csy"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_CSY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x405 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Deu/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Csy/DVCsy.dll"
# Begin Special Build Tool
TargetPath=.\Csy\DVCsy.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Nld"
# PROP BASE Intermediate_Dir "LangDll___Win32_Nld"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Nld"
# PROP Intermediate_Dir "Nld"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_NLD" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Deu/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Nld/DVNld.dll"
# Begin Special Build Tool
TargetPath=.\Nld\DVNld.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Plk"
# PROP BASE Intermediate_Dir "LangDll___Win32_Plk"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Plk"
# PROP Intermediate_Dir "Plk"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_PLK" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x14000000" /subsystem:windows /dll /map /machine:I386 /out:"Deu/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Plk/DVPlk.dll"
# Begin Special Build Tool
TargetPath=.\Plk\DVPlk.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Sve"
# PROP BASE Intermediate_Dir "LangDll___Win32_Sve"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Sve"
# PROP Intermediate_Dir "Sve"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_SVE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x41d /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Deu/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Sve/DVSve.dll"
# Begin Special Build Tool
TargetPath=.\Sve\DVSve.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LangDll___Win32_CsyDebug"
# PROP BASE Intermediate_Dir "LangDll___Win32_CsyDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "CsyDebug"
# PROP Intermediate_Dir "CsyDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_CSY" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x405 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"EnuDebug/DVEnuDebug.dll" /pdbtype:sept
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"CsyDebug/DVCsyDebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\CsyDebug\DVCsyDebug.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\LibsD\_and_to_..\DVClient\Debug\ 
PostBuild_Cmds=copy $(TargetPath) ..\dvclient\Debug	copy $(TargetPath) .\libsD
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Nor"
# PROP BASE Intermediate_Dir "LangDll___Win32_Nor"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Nor"
# PROP Intermediate_Dir "Nor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_NOR" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Deu/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Nor/DVNor.dll"
# Begin Special Build Tool
TargetPath=.\Nor\DVNor.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Wls"
# PROP BASE Intermediate_Dir "LangDll___Win32_Wls"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Wls"
# PROP Intermediate_Dir "Wls"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_WLS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Enu/DVEnu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"WLS/DVWls.dll"
# Begin Special Build Tool
TargetPath=.\WLS\DVWls.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_Ptg"
# PROP BASE Intermediate_Dir "LangDll___Win32_Ptg"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ptg"
# PROP Intermediate_Dir "Ptg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_PTG" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x816 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Enu/DVEnu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Ptg/DVPtg.dll"
# Begin Special Build Tool
TargetPath=.\Ptg\DVPtg.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DV\ 
PostBuild_Cmds=copy $(TargetPath) c:\dv\*.*	copy $(TargetPath) .\libs\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_DeuU"
# PROP BASE Intermediate_Dir "LangDll___Win32_DeuU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DeuU"
# PROP Intermediate_Dir "DeuU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Deu/DVDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"DeuU/DVUDeu.dll"
# Begin Special Build Tool
TargetPath=.\DeuU\DVUDeu.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_ChsU"
# PROP BASE Intermediate_Dir "LangDll___Win32_ChsU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ChsU"
# PROP Intermediate_Dir "ChsU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_CHS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_CHS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Chs/DVUChs.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"ChsU/DVUChs.dll"
# Begin Special Build Tool
TargetPath=.\ChsU\DVUChs.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DVX\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LangDll___Win32_DebugU"
# PROP BASE Intermediate_Dir "LangDll___Win32_DebugU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /fo"Debug/LangDllU.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"Debug/DVDeuDebug.dll" /pdbtype:sept
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"DebugU/DVUDeuDebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\DebugU\DVUDeuDebug.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_..\DVClient\Debug\ 
PostBuild_Cmds=copy $(TargetPath) .\libsDU\*.*	copy $(TargetPath) ..\dvclient\debugunicode\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LangDll___Win32_ChsDebugU"
# PROP BASE Intermediate_Dir "LangDll___Win32_ChsDebugU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ChsDebugU"
# PROP Intermediate_Dir "ChsDebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_CHS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /fo"ChsDebug/LangDllchsU.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"Debug/DVUDeuDebug.dll" /pdbtype:sept
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"ChsDebugU/DVUChsDebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\ChsDebugU\DVUChsDebug.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_..\DVClient\Debug\ 
PostBuild_Cmds=copy $(TargetPath) .\libsDU\*.*	copy $(TargetPath) ..\dvclient\debugunicode\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LangDll___Win32_EnuDebugU"
# PROP BASE Intermediate_Dir "LangDll___Win32_EnuDebugU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "EnuDebugU"
# PROP Intermediate_Dir "EnuDebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"EnuDebug/LangDllEnuU.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"EnuDebug/DVEnuDebug.dll" /pdbtype:sept
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"EnuDebugU/DVUEnuDebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\EnuDebugU\DVUEnuDebug.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_..\DVClient\Debug\ 
PostBuild_Cmds=copy $(TargetPath) .\libsDU\*.*	copy $(TargetPath) ..\dvclient\debugunicode\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_EnuU"
# PROP BASE Intermediate_Dir "LangDll___Win32_EnuU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EnuU"
# PROP Intermediate_Dir "EnuU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Enu/DVEnu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"EnuU/DVUEnu.dll"
# Begin Special Build Tool
TargetPath=.\EnuU\DVUEnu.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_c:\DVX\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_EspU"
# PROP BASE Intermediate_Dir "LangDll___Win32_EspU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EspU"
# PROP Intermediate_Dir "EspU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ESP" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ESP" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc0a /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Esp/DVEsp.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"EspU/DVUEsp.dll"
# Begin Special Build Tool
TargetPath=.\EspU\DVUEsp.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_FraU"
# PROP BASE Intermediate_Dir "LangDll___Win32_FraU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "FraU"
# PROP Intermediate_Dir "FraU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_FRA" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_FRA" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Fra/DVFra.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"FraU/DVUFra.dll"
# Begin Special Build Tool
TargetPath=.\FraU\DVUFra.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_ItaU"
# PROP BASE Intermediate_Dir "LangDll___Win32_ItaU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ItaU"
# PROP Intermediate_Dir "ItaU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ITA" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ITA" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Ita/DVIta.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"ItaU/DVUIta.dll"
# Begin Special Build Tool
TargetPath=.\ItaU\DVUIta.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_CsyU"
# PROP BASE Intermediate_Dir "LangDll___Win32_CsyU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "CsyU"
# PROP Intermediate_Dir "CsyU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_CSY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_CSY" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x405 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Csy/DVCsy.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"CsyU/DVUCsy.dll"
# Begin Special Build Tool
TargetPath=.\CsyU\DVUCsy.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_NldU"
# PROP BASE Intermediate_Dir "LangDll___Win32_NldU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "NldU"
# PROP Intermediate_Dir "NldU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_NLD" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_NLD" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Nld/DVNld.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"NldU/DVUNld.dll"
# Begin Special Build Tool
TargetPath=.\NldU\DVUNld.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_PlkU"
# PROP BASE Intermediate_Dir "LangDll___Win32_PlkU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PlkU"
# PROP Intermediate_Dir "PlkU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_PLK" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_PLK" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Plk/DVPlk.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"PlkU/DVUPlk.dll"
# Begin Special Build Tool
TargetPath=.\PlkU\DVUPlk.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_SveU"
# PROP BASE Intermediate_Dir "LangDll___Win32_SveU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SveU"
# PROP Intermediate_Dir "SveU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_SVE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_SVE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x41d /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Sve/DVSve.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"SveU/DVUSve.dll"
# Begin Special Build Tool
TargetPath=.\SveU\DVUSve.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_NorU"
# PROP BASE Intermediate_Dir "LangDll___Win32_NorU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "NorU"
# PROP Intermediate_Dir "NorU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_NOR" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_NOR" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Nor/DVNor.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"NorU/DVUNor.dll"
# Begin Special Build Tool
TargetPath=.\NorU\DVUNor.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_PtgU"
# PROP BASE Intermediate_Dir "LangDll___Win32_PtgU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PtgU"
# PROP Intermediate_Dir "PtgU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_DEU" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_PTG" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"DeuU/DVUDeu.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"PtgU/DVUPtg.dll"
# Begin Special Build Tool
TargetPath=.\PtgU\DVUPtg.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LangDll___Win32_RusU"
# PROP BASE Intermediate_Dir "LangDll___Win32_RusU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RusU"
# PROP Intermediate_Dir "RusU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_RUS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_RUS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"Rus/DVRus.dll"
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /map /machine:I386 /out:"RusU/DVURus.dll"
# Begin Special Build Tool
TargetPath=.\RusU\DVURus.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\ 
PostBuild_Cmds=copy $(TargetPath) .\libsU\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LangDll___Win32_RusDebugU"
# PROP BASE Intermediate_Dir "LangDll___Win32_RusDebugU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "RusDebugU"
# PROP Intermediate_Dir "RusDebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "DV_ENU" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"EnuDebug/DVEnuDebug.dll" /pdbtype:sept
# ADD LINK32 /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"RusDebugU/DVURusDebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\RusDebugU\DVURusDebug.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_.\Libs\_and_to_..\DVClient\Debug\ 
PostBuild_Cmds=copy $(TargetPath) .\libsDU\*.*	copy $(TargetPath) ..\dvclient\debugunicode\*.*
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "LangDll - Win32 Debug"
# Name "LangDll - Win32 Enu"
# Name "LangDll - Win32 EnuDebug"
# Name "LangDll - Win32 Esp"
# Name "LangDll - Win32 Fra"
# Name "LangDll - Win32 Ita"
# Name "LangDll - Win32 Deu"
# Name "LangDll - Win32 Csy"
# Name "LangDll - Win32 Nld"
# Name "LangDll - Win32 Plk"
# Name "LangDll - Win32 Sve"
# Name "LangDll - Win32 CsyDebug"
# Name "LangDll - Win32 Nor"
# Name "LangDll - Win32 Wls"
# Name "LangDll - Win32 Ptg"
# Name "LangDll - Win32 DeuU"
# Name "LangDll - Win32 ChsU"
# Name "LangDll - Win32 DebugU"
# Name "LangDll - Win32 ChsDebugU"
# Name "LangDll - Win32 EnuDebugU"
# Name "LangDll - Win32 EnuU"
# Name "LangDll - Win32 EspU"
# Name "LangDll - Win32 FraU"
# Name "LangDll - Win32 ItaU"
# Name "LangDll - Win32 CsyU"
# Name "LangDll - Win32 NldU"
# Name "LangDll - Win32 PlkU"
# Name "LangDll - Win32 SveU"
# Name "LangDll - Win32 NorU"
# Name "LangDll - Win32 PtgU"
# Name "LangDll - Win32 RusU"
# Name "LangDll - Win32 RusDebugU"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LangDll.cpp
# End Source File
# Begin Source File

SOURCE=.\LangDll.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllchsDebugU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllChsU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllCsy.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllCsyDebug.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllCsyU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllDebug.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdlldebugU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllEnu.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllEnuDebug.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllEnuDebugU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllEnuU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllEsp.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllEspU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllFra.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllFraU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllIta.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllItaU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllNld.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllNldU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllNor.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllNorU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllPlk.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllPlkU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllPtg.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Intermediate_Dir "Ptg"

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllPtgU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllRus.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllRusDebugU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllrusU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllSve.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Intermediate_Dir "CsyDebug"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllSveU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllU.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllwls.def

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LangDll.h
# End Source File
# Begin Source File

SOURCE=..\DVClient\resource.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\LangDll.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\LangDll.rc2
# End Source File
# Begin Source File

SOURCE=.\LangDllchs.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllCsy.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllEnu.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllEsp.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllFra.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllIta.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllNld.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllNor.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllPlk.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllptg.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllRus.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllSve.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllwls.rc

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Intermediate_Dir "Wls"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Language"

# PROP Default_Filter "lng"
# Begin Source File

SOURCE=.\langdllchs.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllCsy.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllEnu.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllEsp.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllFra.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllIta.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllNld.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllNor.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LangDllPlk.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllPtg.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllRus.lng
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LangDllSve.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\langdllwls.lng

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Glossary"

# PROP Default_Filter "csv"
# Begin Source File

SOURCE=.\Deu_Enu.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Enu_Chs.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\enu_csy.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\enu_esp.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Enu_Fra.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Enu_Ita.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\enu_nld.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\enu_ptg.csv

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\enu_rus.csv
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\History.txt

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\language.txt

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "LangDll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Esp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Fra"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Deu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Csy"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nld"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Plk"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Sve"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Nor"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Wls"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 Ptg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DeuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 DebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ChsDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EnuU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 EspU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 FraU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 ItaU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 CsyU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NldU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PlkU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 SveU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 NorU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 PtgU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LangDll - Win32 RusDebugU"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
