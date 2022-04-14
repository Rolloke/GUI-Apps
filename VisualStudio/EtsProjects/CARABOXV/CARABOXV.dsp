# Microsoft Developer Studio Project File - Name="CARABOXV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CARABOXV - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CARABOXV.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CARABOXV.mak" CFG="CARABOXV - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CARABOXV - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARABOXV - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARABOXV - Win32 ReleaseEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARABOXV - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARABOXV_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /GB /MD /W3 /GX /O1 /I "C:\Daten\Myprojects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARABOXV_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_DEU" /D "AFX_TARG_NEU" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib CEtsDlg.lib CCaraWalk.lib EtsBind.lib CEtsHelp.lib CEtsDiv.lib /nologo /dll /map /machine:I386 /nodefaultlib:"libmmd" /libpath:"C:\Daten\Myprojects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der DLL
PostBuild_Cmds=copy $(OutDir)\CARABOXV.dll C:\Daten\MyProjects\Libraries\ReleaseDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARABOXV - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARABOXV_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\Myprojects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARABOXV_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_DEU" /D "AFX_TARG_NEU" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib CEtsDlg.lib CCaraWalk.lib ETSBIND.lib CEtsHelp.lib CEtsDiv.lib /nologo /dll /map /debug /machine:I386 /nodefaultlib:"LIBCMTD" /pdbtype:sept /libpath:"C:\Daten\Myprojects\Libraries\Debug"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der DLL
PostBuild_Cmds=copy $(OutDir)\CARABOXV.dll C:\Daten\MyProjects\Libraries\DebugDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARABOXV - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "C:\Daten\Myprojects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARABOXV_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\Myprojects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARABOXV_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_ENU" /D "AFX_TARG_NEU" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib CEtsDlg.lib CCaraWalk.lib EtsBind.lib CEtsHelp.lib /nologo /dll /map /machine:I386 /libpath:"C:\Daten\Myprojects\Libraries\Release"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib CEtsDlg.lib CCaraWalk.lib EtsBind.lib CEtsHelp.lib CEtsDiv.lib /nologo /dll /map /machine:I386 /nodefaultlib:"libmmd" /libpath:"C:\Daten\Myprojects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\ReleaseEngl
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der DLL
PostBuild_Cmds=copy $(OutDir)\CARABOXV.dll C:\Daten\MyProjects\Libraries\ReleaseEnglDll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CARABOXV - Win32 Release"
# Name "CARABOXV - Win32 Debug"
# Name "CARABOXV - Win32 ReleaseEngl"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CARABOXV.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraBoxV.def
# End Source File
# Begin Source File

SOURCE=.\CARABOXV.rc
# End Source File
# Begin Source File

SOURCE=.\CaraBoxViewDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Libraries\CEtsDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CaraBoxViewDlg.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\CCaraWalk.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BigIcon.ico
# End Source File
# Begin Source File

SOURCE=.\res\bigicon1.ico
# End Source File
# Begin Source File

SOURCE=.\Small.ico
# End Source File
# Begin Source File

SOURCE=.\res\SOPHLEFT.ico
# End Source File
# Begin Source File

SOURCE=.\res\SOPHRIGHT.ico
# End Source File
# Begin Source File

SOURCE=.\res\SOPSLEFT.ico
# End Source File
# Begin Source File

SOURCE=.\res\SOPSRIGHT.ico
# End Source File
# Begin Source File

SOURCE=.\res\SOTHDOWN.ico
# End Source File
# Begin Source File

SOURCE=.\res\SOTHUP.ico
# End Source File
# Begin Source File

SOURCE=.\TestApp.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
