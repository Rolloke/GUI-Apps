# Microsoft Developer Studio Project File - Name="CommUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CommUnit - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CommUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CommUnit.mak" CFG="CommUnit - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CommUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CommUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CommUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "CommUnit - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "CommUnit - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "CommUnit - Win32 EnuUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/CommUnit", GILAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CommUnit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib oemgui.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=.\Release\CommUnit.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy        $(TargetPath)        c:\security\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp1 /MD /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 cipcdebug.lib oemguid.lib wkclassesDebug.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Enu"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CommUnit___Win32_Enu"
# PROP BASE Intermediate_Dir "CommUnit___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 cipc.lib oemgui.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "CommUnit - Win32 DebugUnicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CommUnit___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "CommUnit___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcdebug.lib oemguid.lib wkclassesDebug.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 cipcdebugU.lib oemguidU.lib wkclassesDebugU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /debug /machine:I386
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "CommUnit - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CommUnit___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "CommUnit___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 cipcU.lib oemguiU.lib wkclassesU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "CommUnit - Win32 EnuUnicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CommUnit___Win32_EnuUnicode"
# PROP BASE Intermediate_Dir "CommUnit___Win32_EnuUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EnuUnicode"
# PROP Intermediate_Dir "EnuUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 cipcU.lib oemguiU.lib wkclassesU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "CommUnit - Win32 Release"
# Name "CommUnit - Win32 Debug"
# Name "CommUnit - Win32 Enu"
# Name "CommUnit - Win32 DebugUnicode"
# Name "CommUnit - Win32 ReleaseUnicode"
# Name "CommUnit - Win32 EnuUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\CameraControlRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\CommInput.cpp
# End Source File
# Begin Source File

SOURCE=.\CommUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CommUnit.rc

!IF  "$(CFG)" == "CommUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "CommUnit - Win32 ReleaseUnicode"

!ELSEIF  "$(CFG)" == "CommUnit - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CommUnitEnu.lng
# End Source File
# Begin Source File

SOURCE=.\CommUnitEnu.rc

!IF  "$(CFG)" == "CommUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 EnuUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CommWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlRecordBaxall.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlRecordFastrax.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlRecordTOA.cpp
# End Source File
# Begin Source File

SOURCE=.\CRS232.cpp
# End Source File
# Begin Source File

SOURCE=.\MapDwordToDword.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Enu"

# ADD BASE CPP /Yc
# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "CommUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "CommUnit - Win32 ReleaseUnicode"

# ADD BASE CPP /Yc
# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "CommUnit - Win32 EnuUnicode"

# ADD BASE CPP /Yc
# ADD CPP /Yc

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CameraCommandRecord.h
# End Source File
# Begin Source File

SOURCE=.\CameraControlRecord.h
# End Source File
# Begin Source File

SOURCE=.\CommInput.h
# End Source File
# Begin Source File

SOURCE=.\CommUnit.h
# End Source File
# Begin Source File

SOURCE=.\CommWindow.h
# End Source File
# Begin Source File

SOURCE=.\ControlRecord.h
# End Source File
# Begin Source File

SOURCE=.\ControlRecordBaxall.h
# End Source File
# Begin Source File

SOURCE=.\ControlRecordFastrax.h
# End Source File
# Begin Source File

SOURCE=.\ControlRecordTOA.h
# End Source File
# Begin Source File

SOURCE=.\CRS232.h
# End Source File
# Begin Source File

SOURCE=.\MapDwordToDword.h

!IF  "$(CFG)" == "CommUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CommUnit - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\CommUnit.ico
# End Source File
# Begin Source File

SOURCE=.\res\CommUnit.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\history.txt
# End Source File
# End Target
# End Project
