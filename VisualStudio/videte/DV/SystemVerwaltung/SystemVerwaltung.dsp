# Microsoft Developer Studio Project File - Name="SystemVerwaltung" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SystemVerwaltung - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SystemVerwaltung.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SystemVerwaltung.mak" CFG="SystemVerwaltung - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SystemVerwaltung - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SystemVerwaltung - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SystemVerwaltung - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "SystemVerwaltung - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DV/SystemVerwaltung", MQDCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 cipcdebug.lib oemguid.lib WKClassesDebug.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SystemVerwaltung___Win32_Enu"
# PROP BASE Intermediate_Dir "SystemVerwaltung___Win32_Enu"
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
# ADD BASE LINK32 cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SystemVerwaltung___Win32_Ita"
# PROP BASE Intermediate_Dir "SystemVerwaltung___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
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
# ADD BASE LINK32 cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386

!ENDIF 

# Begin Target

# Name "SystemVerwaltung - Win32 Release"
# Name "SystemVerwaltung - Win32 Debug"
# Name "SystemVerwaltung - Win32 Enu"
# Name "SystemVerwaltung - Win32 Ita"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "Pages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\apppage.cpp
# End Source File
# Begin Source File

SOURCE=.\ArchivPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DrivePage.cpp
# End Source File
# Begin Source File

SOURCE=.\HostPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ISDNPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketPage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AppPage.h
# End Source File
# Begin Source File

SOURCE=.\ArchivPage.h
# End Source File
# Begin Source File

SOURCE=.\DrivePage.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\HostPage.h
# End Source File
# Begin Source File

SOURCE=.\ISDNPage.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SocketPage.h
# End Source File
# Begin Source File

SOURCE=.\statuslist.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SVCoolBar.h
# End Source File
# Begin Source File

SOURCE=.\SVDoc.h
# End Source File
# Begin Source File

SOURCE=.\SVPage.h
# End Source File
# Begin Source File

SOURCE=.\SVTree.h
# End Source File
# Begin Source File

SOURCE=.\SVView.h
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltung.h
# End Source File
# Begin Source File

SOURCE=.\treelist.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\archive.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cold.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drives.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\images.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SVDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\SystemVerwaltung.ico
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltung.rc

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\SystemVerwaltung.rc2
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltungenu.rc

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltungIta.rc

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP Intermediate_Dir "Release"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Up.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\statuslist.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SVCoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SVDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SVPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SVTree.cpp
# End Source File
# Begin Source File

SOURCE=.\SVView.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltung.cpp

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\treelist.cpp
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter "txt;lng;csv"
# Begin Source File

SOURCE=.\Deu_Enu.csv
# End Source File
# Begin Source File

SOURCE=.\Enu_Ita.csv
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltungenu.lng
# End Source File
# Begin Source File

SOURCE=.\systemverwaltungIta.lng
# End Source File
# End Group
# End Target
# End Project
