# Microsoft Developer Studio Project File - Name="ETSCDPL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ETSCDPL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ETSCDPL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ETSCDPL.mak" CFG="ETSCDPL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ETSCDPL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ETSCDPL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ETSCDPL - Win32 ReleaseEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ETSCDPL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETSCDPL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETSCDPL_EXPORTS" /D "INCLUDE_ETS_HELP" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib Winmm.lib CEtsHelp.lib CEtsDiv.lib ETSBIND.lib CEtsDlg.lib CEtsList.lib version.lib ole32.lib comctl32.lib EtsRegistry.lib CEtsDebug.lib /nologo /dll /map /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libmmd" /nodefaultlib:"libm" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Dll
PostBuild_Cmds=copy $(OutDir)\ETSCDPL.Dll C:\Daten\MyProjects\Libraries\ReleaseDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ETSCDPL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETSCDPL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETSCDPL_EXPORTS" /D "INCLUDE_ETS_HELP" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib Winmm.lib CEtsHelp.lib CEtsDiv.lib ETSBIND.lib CEtsDlg.lib CEtsList.lib Advapi32.lib version.lib ole32.lib comctl32.lib Msacm32.lib EtsRegistry.lib CEtsDebug.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libmmd" /nodefaultlib:"libm" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Dll
PostBuild_Cmds=copy $(OutDir)\ETSCDPL.Dll C:\Daten\MyProjects\Libraries\DebugDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ETSCDPL - Win32 ReleaseEngl"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseEngl"
# PROP BASE Intermediate_Dir "ReleaseEngl"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseEngl"
# PROP Intermediate_Dir "ReleaseEngl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETSCDPL_EXPORTS" /D "INCLUDE_ETS_HELP" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_USRDLL" /D "_WINDOWS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_ENU" /D "AFX_TARG_NEU" /D "INCLUDE_ETS_HELP" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib Winmm.lib CEtsHelp.lib CEtsDiv.lib ETSBIND.lib CEtsDlg.lib CEtsList.lib version.lib ole32.lib comctl32.lib /nologo /dll /machine:I386 /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib Winmm.lib CEtsHelp.lib CEtsDiv.lib ETSBIND.lib CEtsDlg.lib CEtsList.lib version.lib ole32.lib comctl32.lib EtsRegistry.lib CEtsDebug.lib /nologo /dll /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libmmd" /nodefaultlib:"libm" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# SUBTRACT LINK32 /map
# Begin Special Build Tool
OutDir=.\ReleaseEngl
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Dll
PostBuild_Cmds=copy $(OutDir)\ETSCDPL.Dll C:\Daten\MyProjects\Libraries\ReleaseEnglDll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ETSCDPL - Win32 Release"
# Name "ETSCDPL - Win32 Debug"
# Name "ETSCDPL - Win32 ReleaseEngl"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CCDPLDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CDPlayerCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CDTitleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ETSCDPL.cpp
# End Source File
# Begin Source File

SOURCE=.\ETSCDPL.def
# End Source File
# Begin Source File

SOURCE=.\FormatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MixerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MixerOptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Skript1.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CCDPLDlg.h
# End Source File
# Begin Source File

SOURCE=.\CDPlayerCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CDTitleDlg.h
# End Source File
# Begin Source File

SOURCE=.\FormatDlg.h
# End Source File
# Begin Source File

SOURCE=.\MixerDlg.h
# End Source File
# Begin Source File

SOURCE=.\MixerOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BtnEject.ico
# End Source File
# Begin Source File

SOURCE=.\res\CDInfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\idc_ac_b.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_btn_.ico
# End Source File
# Begin Source File

SOURCE=.\res\NTCHECK.BMP
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
