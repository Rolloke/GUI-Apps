# Microsoft Developer Studio Project File - Name="CoCoUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CoCoUnit - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CoCoUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CoCoUnit.mak" CFG="CoCoUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CoCoUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CoCoUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CoCoUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/CoCoUnit", QCNAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 CIPC.lib OEMGUI.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 cipcdebug.lib oemguid.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CoCoUnit___Win32_Enu"
# PROP BASE Intermediate_Dir "CoCoUnit___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 CIPC.lib OEMGUI.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 CIPC.lib OEMGUI.lib /nologo /subsystem:windows /map /machine:I386

!ENDIF 

# Begin Target

# Name "CoCoUnit - Win32 Release"
# Name "CoCoUnit - Win32 Debug"
# Name "CoCoUnit - Win32 Enu"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Ccoco.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CipcInputCoCoUnit.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CipcOutputCoCoUnit.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CoCoUnitApp.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CoCoUnitDlg.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cpydata.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgcolor.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Settings.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CCoCo.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputCoCoUnit.h
# End Source File
# Begin Source File

SOURCE=.\CipcOutputCoCoUnit.h
# End Source File
# Begin Source File

SOURCE=.\cocounitapp.h
# End Source File
# Begin Source File

SOURCE=.\CoCoUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\cpydata.h
# End Source File
# Begin Source File

SOURCE=.\dlgcolor.h
# End Source File
# Begin Source File

SOURCE=.\message.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\CoCoUnit.ico
# End Source File
# Begin Source File

SOURCE=.\CoCoUnit.rc

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\CoCoUnit.rc2
# End Source File
# Begin Source File

SOURCE=.\CoCoUnitenu.rc

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

!ENDIF 

# End Source File
# End Group
# Begin Group "Lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\MegraV2\MegraV2dll\release\megrav2.lib

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\MegraV2\MegraV2dll\Debug\MegraV2D.lib

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CoCoUnit.mak
# End Source File
# Begin Source File

SOURCE=.\CoCoUnitenu.lng
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# End Group
# End Target
# End Project
