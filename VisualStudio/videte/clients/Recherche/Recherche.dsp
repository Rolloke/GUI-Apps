# Microsoft Developer Studio Project File - Name="Recherche" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Recherche - Win32 DebugDts
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Recherche.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Recherche.mak" CFG="Recherche - Win32 DebugDts"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Recherche - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Recherche - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Recherche - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "Recherche - Win32 Dts" (based on "Win32 (x86) Application")
!MESSAGE "Recherche - Win32 DtsEnu" (based on "Win32 (x86) Application")
!MESSAGE "Recherche - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE "Recherche - Win32 DtsIta" (based on "Win32 (x86) Application")
!MESSAGE "Recherche - Win32 DebugDts" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Clients/Recherche", SHZAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Recherche - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
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
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
TargetPath=.\Release\Recherche.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) c:\security
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vimaged.lib cipcdebug.lib oemguid.lib wkclassesdebug.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Recherche___Win32_Enu"
# PROP BASE Intermediate_Dir "Recherche___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib IMAGN32.lib ptdecoder.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Recherche___Win32_Dts"
# PROP BASE Intermediate_Dir "Recherche___Win32_Dts"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Dts"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib Jpeglib.lib ptdecoder.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Recherche___Win32_DtsEnu"
# PROP BASE Intermediate_Dir "Recherche___Win32_DtsEnu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsEnu"
# PROP Intermediate_Dir "DtsEnu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib Jpeglib.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Recherche___Win32_Ita"
# PROP BASE Intermediate_Dir "Recherche___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib Jpeglib.lib ptdecoder.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 cipc.lib oemgui.lib Jpeglib.lib ptdecoder.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Recherche___Win32_DtsIta"
# PROP BASE Intermediate_Dir "Recherche___Win32_DtsIta"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsIta"
# PROP Intermediate_Dir "DtsIta"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib Jpeglib.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 cipc.lib oemgui.lib Jpeglib.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Recherche___Win32_DebugDts"
# PROP BASE Intermediate_Dir "Recherche___Win32_DebugDts"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDts"
# PROP Intermediate_Dir "DebugDts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vimaged.lib cipcdebug.lib oemguid.lib wkclassesdebug.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vimaged.lib cipcdebug.lib oemguid.lib wkclassesdebug.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Recherche - Win32 Release"
# Name "Recherche - Win32 Debug"
# Name "Recherche - Win32 Enu"
# Name "Recherche - Win32 Dts"
# Name "Recherche - Win32 DtsEnu"
# Name "Recherche - Win32 Ita"
# Name "Recherche - Win32 DtsIta"
# Name "Recherche - Win32 DebugDts"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AlarmListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmListWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupDialogNew.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupDialogOld.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseRecherche.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCServerControlRecherche.cpp
# End Source File
# Begin Source File

SOURCE=.\CommentDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectingDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyReadOnlyVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\Custsite.cpp
# End Source File
# Begin Source File

SOURCE=.\databaseinfodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DibData.cpp
# End Source File
# Begin Source File

SOURCE=.\DibWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\displaywindow.cpp
# End Source File
# Begin Source File

SOURCE=.\FieldDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Idispimp.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCControlAudioUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCDataCarrierRecherche.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\NavigationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectView.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectViewTreeStuff.cpp
# End Source File
# Begin Source File

SOURCE=.\playrealtime.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryFieldCharDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryFieldDateDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryFieldDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryFieldNumDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryFieldTimeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryParameter.cpp
# End Source File
# Begin Source File

SOURCE=.\queryrecttracker.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryResult.cpp
# End Source File
# Begin Source File

SOURCE=.\Recherche.cpp
# End Source File
# Begin Source File

SOURCE=.\Recherche.rc

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RechercheDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Rechercheenu.rc

!IF  "$(CFG)" == "Recherche - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RechercheIta.rc

!IF  "$(CFG)" == "Recherche - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RechercheView.cpp
# End Source File
# Begin Source File

SOURCE=.\Save.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchResultDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectBackupDriveDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\Servers.cpp
# End Source File
# Begin Source File

SOURCE=.\smalltoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\smallwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\StationNameDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SyncCoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AlarmListDialog.h
# End Source File
# Begin Source File

SOURCE=.\AlarmListWindow.h
# End Source File
# Begin Source File

SOURCE=.\BackupDialog.h
# End Source File
# Begin Source File

SOURCE=.\BackupDialogNew.h
# End Source File
# Begin Source File

SOURCE=.\BackupDialogOld.h
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseRecherche.h
# End Source File
# Begin Source File

SOURCE=.\CIPCServerControlRecherche.h
# End Source File
# Begin Source File

SOURCE=.\CommentDialog.h
# End Source File
# Begin Source File

SOURCE=.\ConnectingDialog.h
# End Source File
# Begin Source File

SOURCE=.\CopyReadOnlyVersion.h
# End Source File
# Begin Source File

SOURCE=.\Custsite.h
# End Source File
# Begin Source File

SOURCE=.\databaseinfodlg.h
# End Source File
# Begin Source File

SOURCE=.\DibData.h
# End Source File
# Begin Source File

SOURCE=.\DibWindow.h
# End Source File
# Begin Source File

SOURCE=.\displaywindow.h
# End Source File
# Begin Source File

SOURCE=.\FieldDialog.h
# End Source File
# Begin Source File

SOURCE=.\HtmlWindow.h
# End Source File
# Begin Source File

SOURCE=.\Idispimp.h
# End Source File
# Begin Source File

SOURCE=.\images.h
# End Source File
# Begin Source File

SOURCE=.\ImageWindow.h
# End Source File
# Begin Source File

SOURCE=.\IPCControlAudioUnit.h
# End Source File
# Begin Source File

SOURCE=.\IPCDataCarrierRecherche.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NavigationDialog.h
# End Source File
# Begin Source File

SOURCE=.\ObjectView.h
# End Source File
# Begin Source File

SOURCE=.\playrealtime.h
# End Source File
# Begin Source File

SOURCE=.\QueryDialog.h
# End Source File
# Begin Source File

SOURCE=.\QueryFieldCharDialog.h
# End Source File
# Begin Source File

SOURCE=.\QueryFieldDateDialog.h
# End Source File
# Begin Source File

SOURCE=.\QueryFieldDialog.h
# End Source File
# Begin Source File

SOURCE=.\QueryFieldNumDialog.h
# End Source File
# Begin Source File

SOURCE=.\QueryFieldTimeDialog.h
# End Source File
# Begin Source File

SOURCE=.\QueryParameter.h
# End Source File
# Begin Source File

SOURCE=.\QueryRectTracker.h
# End Source File
# Begin Source File

SOURCE=.\QueryResult.h
# End Source File
# Begin Source File

SOURCE=.\Recherche.h
# End Source File
# Begin Source File

SOURCE=.\RechercheDoc.h
# End Source File
# Begin Source File

SOURCE=.\RechercheView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchResultDialog.h
# End Source File
# Begin Source File

SOURCE=.\SelectBackupDriveDialog.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\server.inl
# End Source File
# Begin Source File

SOURCE=.\Servers.h
# End Source File
# Begin Source File

SOURCE=.\smalltoolbar.h
# End Source File
# Begin Source File

SOURCE=.\smallwindow.h
# End Source File
# Begin Source File

SOURCE=.\StationNameDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SyncCoolBar.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\alarm_li.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\arrow.cur

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\back.gif

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\cold.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\dib.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\display.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\display_dts.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\drives.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\glass.cur

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\HOSTicon.ico
# End Source File
# Begin Source File

SOURCE=.\res\hot.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\html.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\images.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\Recherche.ico

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\Recherche.rc2

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\RechercheDoc.ico

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\rect.cur

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\search.avi

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\states.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\sync_hot.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\toolbarm.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\white.bmp

!IF  "$(CFG)" == "Recherche - Win32 Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Debug"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Enu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Dts"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Recherche - Win32 Ita"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DtsIta"

!ELSEIF  "$(CFG)" == "Recherche - Win32 DebugDts"

!ENDIF 

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

SOURCE=.\history.txt
# End Source File
# Begin Source File

SOURCE=.\res\nostatement.htm
# End Source File
# Begin Source File

SOURCE=.\Rechercheenu.lng
# End Source File
# Begin Source File

SOURCE=.\rechercheIta.lng
# End Source File
# End Group
# End Target
# End Project
