# Microsoft Developer Studio Project File - Name="acdc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=acdc - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "acdc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "acdc.mak" CFG="acdc - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "acdc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "acdc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "acdc - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "acdc - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "acdc - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "acdc - Win32 EnuUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DV/acdc", UCGCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "acdc - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 NeroAPIGlue.lib cipc.lib wkclasses.lib oemgui.lib devicedetect.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"Libcmt.lib" /libpath:"f:\project\dv\ac\nerosrc"

!ELSEIF  "$(CFG)" == "acdc - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 NeroAPIGlue.lib cipcdebug.lib wkclassesdebug.lib oemguid.lib devicedetectd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt.lib" /pdbtype:sept /libpath:"f:\project\dv\ac\nerosrc"

!ELSEIF  "$(CFG)" == "acdc - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "acdc___Win32_Enu"
# PROP BASE Intermediate_Dir "acdc___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 NeroAPIGlue.lib cipc.lib wkclasses.lib oemgui.lib /nologo /subsystem:windows /map /machine:I386 /libpath:"f:\project\dv\ac\nerosrc"
# ADD LINK32 NeroAPIGlue.lib cipc.lib wkclasses.lib oemgui.lib devicedetect.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"Libcmt.lib" /libpath:"f:\project\dv\ac\nerosrc"

!ELSEIF  "$(CFG)" == "acdc - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "acdc___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "acdc___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 NeroAPIGlue.lib cipcdebug.lib wkclassesdebug.lib oemguid.lib devicedetectd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt.lib" /pdbtype:sept /libpath:"f:\project\dv\ac\nerosrc"
# ADD LINK32 NeroAPIGlue.lib cipcdebugU.lib wkclassesdebugU.lib oemguidU.lib devicedetectdU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt.lib" /pdbtype:sept /libpath:"f:\project\dv\ac\nerosrc" /libpath:"V:\project\Cipc\DebugUnicode"

!ELSEIF  "$(CFG)" == "acdc - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "acdc___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "acdc___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 NeroAPIGlue.lib cipc.lib wkclasses.lib oemgui.lib devicedetect.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"Libcmt.lib" /libpath:"f:\project\dv\ac\nerosrc"
# ADD LINK32 NeroAPIGlue.lib cipcU.lib wkclassesU.lib oemguiU.lib devicedetectU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"Libcmt.lib" /libpath:"f:\project\dv\ac\nerosrc" /libpath:"V:\project\Cipc\ReleaseUnicode"

!ELSEIF  "$(CFG)" == "acdc - Win32 EnuUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "acdc___Win32_EnuUnicode"
# PROP BASE Intermediate_Dir "acdc___Win32_EnuUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EnuUnicode"
# PROP Intermediate_Dir "EnuUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 NeroAPIGlue.lib cipcU.lib wkclassesU.lib oemguiU.lib devicedetectU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"Libcmt.lib" /libpath:"f:\project\dv\ac\nerosrc" /libpath:"V:\project\Cipc\ReleaseUnicode"
# ADD LINK32 NeroAPIGlue.lib cipcU.lib wkclassesU.lib oemguiU.lib devicedetectU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"Libcmt.lib" /libpath:"f:\project\dv\ac\nerosrc" /libpath:"V:\project\Cipc\ReleaseUnicode"

!ENDIF 

# Begin Target

# Name "acdc - Win32 Release"
# Name "acdc - Win32 Debug"
# Name "acdc - Win32 Enu"
# Name "acdc - Win32 DebugUnicode"
# Name "acdc - Win32 ReleaseUnicode"
# Name "acdc - Win32 EnuUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\acdc.cpp
# End Source File
# Begin Source File

SOURCE=.\acdc.rc

!IF  "$(CFG)" == "acdc - Win32 Release"

!ELSEIF  "$(CFG)" == "acdc - Win32 Debug"

!ELSEIF  "$(CFG)" == "acdc - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "acdc - Win32 ReleaseUnicode"

!ELSEIF  "$(CFG)" == "acdc - Win32 EnuUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\acdcDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\acdcEnu.rc

!IF  "$(CFG)" == "acdc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AcdcException.cpp
# End Source File
# Begin Source File

SOURCE=.\BurnContext.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckMediaType.cpp
# End Source File
# Begin Source File

SOURCE=.\ExitCode.cpp
# End Source File
# Begin Source File

SOURCE=.\exitcodetranslator.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCBurnDataCarrier.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCServerControlACDC.cpp
# End Source File
# Begin Source File

SOURCE=.\isotrack.cpp
# End Source File
# Begin Source File

SOURCE=.\MainCommands.cpp
# End Source File
# Begin Source File

SOURCE=.\NeroBurn.cpp
# End Source File
# Begin Source File

SOURCE=.\NeroCallbacks.cpp
# End Source File
# Begin Source File

SOURCE=.\NeroThread.cpp
# End Source File
# Begin Source File

SOURCE=.\PARAMETERS.cpp
# End Source File
# Begin Source File

SOURCE=.\Producer.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WriteCommands.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\acdc.h
# End Source File
# Begin Source File

SOURCE=.\acdcDlg.h
# End Source File
# Begin Source File

SOURCE=.\AcdcEnums.h
# End Source File
# Begin Source File

SOURCE=.\AcdcException.h
# End Source File
# Begin Source File

SOURCE=.\BurnContext.h
# End Source File
# Begin Source File

SOURCE=.\ExitCode.h
# End Source File
# Begin Source File

SOURCE=.\FindFiles.h
# End Source File
# Begin Source File

SOURCE=.\IPCBurnDataCarrier.h
# End Source File
# Begin Source File

SOURCE=.\IPCServerControlACDC.h
# End Source File
# Begin Source File

SOURCE=.\NeroAPI.h
# End Source File
# Begin Source File

SOURCE=.\NeroAPIGlue.h
# End Source File
# Begin Source File

SOURCE=.\NeroBurn.h
# End Source File
# Begin Source File

SOURCE=.\NeroThread.h
# End Source File
# Begin Source File

SOURCE=.\NeroUserDialog.h
# End Source File
# Begin Source File

SOURCE=.\PARAMETERS.h
# End Source File
# Begin Source File

SOURCE=.\Producer.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SimpleStringArray.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\acdc.ico
# End Source File
# Begin Source File

SOURCE=.\res\acdc.rc2
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\History.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\acdcEnu.lng
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NeroAPIGlue.lib

!IF  "$(CFG)" == "acdc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "acdc - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
