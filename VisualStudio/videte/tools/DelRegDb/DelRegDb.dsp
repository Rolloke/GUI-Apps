# Microsoft Developer Studio Project File - Name="DelRegDb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DelRegDb - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DelRegDb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DelRegDb.mak" CFG="DelRegDb - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DelRegDb - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DelRegDb - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DelRegDb - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "DelRegDb - Win32 Dts" (based on "Win32 (x86) Application")
!MESSAGE "DelRegDb - Win32 DtsEnu" (based on "Win32 (x86) Application")
!MESSAGE "DelRegDb - Win32 DtsIta" (based on "Win32 (x86) Application")
!MESSAGE "DelRegDb - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Tools/DelRegDb", TTRAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DelRegDb - Win32 Release"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wkclasses.lib cipc.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wkclassesdebug.lib cipcdebug.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DelRegDb___Win32_Enu"
# PROP BASE Intermediate_Dir "DelRegDb___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wkclasses.lib cipc.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Dts"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DelRegDb___Win32_Dts"
# PROP BASE Intermediate_Dir "DelRegDb___Win32_Dts"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Dts"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wkclasses.lib cipc.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsEnu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DelRegDb___Win32_DtsEnu"
# PROP BASE Intermediate_Dir "DelRegDb___Win32_DtsEnu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsEnu"
# PROP Intermediate_Dir "DtsEnu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wkclasses.lib cipc.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsIta"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DelRegDb___Win32_DtsIta"
# PROP BASE Intermediate_Dir "DelRegDb___Win32_DtsIta"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsIta"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wkclasses.lib cipc.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DelRegDb___Win32_Ita"
# PROP BASE Intermediate_Dir "DelRegDb___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wkclasses.lib cipc.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "DelRegDb - Win32 Release"
# Name "DelRegDb - Win32 Debug"
# Name "DelRegDb - Win32 Enu"
# Name "DelRegDb - Win32 Dts"
# Name "DelRegDb - Win32 DtsEnu"
# Name "DelRegDb - Win32 DtsIta"
# Name "DelRegDb - Win32 Ita"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\DelRegDb.cpp
# End Source File
# Begin Source File

SOURCE=.\DelRegDb.rc

!IF  "$(CFG)" == "DelRegDb - Win32 Release"

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Debug"

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Dts"

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsEnu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsIta"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DelRegDbenu.rc

!IF  "$(CFG)" == "DelRegDb - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Intermediate_Dir "Re"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Dts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsEnu"

# PROP BASE Intermediate_Dir "Re"
# PROP Intermediate_Dir "Re"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsIta"

# PROP BASE Intermediate_Dir "Re"
# PROP Intermediate_Dir "Re"
# PROP Exclude_From_Build 1
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Ita"

# PROP BASE Intermediate_Dir "Re"
# PROP Intermediate_Dir "Re"
# PROP Exclude_From_Build 1
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DelRegDbIta.rc

!IF  "$(CFG)" == "DelRegDb - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Dts"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 DtsIta"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DelRegDb - Win32 Ita"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\DelRegDb.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DelRegDb.ico
# End Source File
# Begin Source File

SOURCE=.\res\DelRegDb.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\delregdbIta.lng
# End Source File
# Begin Source File

SOURCE=.\history.txt
# End Source File
# End Target
# End Project
