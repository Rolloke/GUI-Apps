# Microsoft Developer Studio Project File - Name="CARAWALK" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CARAWALK - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CARAWALK.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CARAWALK.mak" CFG="CARAWALK - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CARAWALK - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARAWALK - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARAWALK - Win32 ReleaseEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARAWALK - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARAWALK_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARAWALK_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_DEU" /D "AFX_TARG_NEU" /FR /Yu"stdafx.h" /FD /G7 -Qip -O3 -Qipo "_USE_INTEL_COMPILER" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib winmm.lib advapi32.lib shell32.lib opengl32.lib glu32.lib CEtsDlg.lib CFileHeader.lib Cara3DTr.lib CEtsHelp.lib CCaraMat.lib CEtsList.lib CEts3DGL.lib CDibSection.lib CEtsDebug.lib version.lib EtsBind.lib CEtsDiv.lib EtsRegistry.lib /nologo /version:1.1 /dll /map /machine:I386 /libpath:"c:\Daten\MyProjects\Libraries\Release\\"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Carawalk.Dll
PostBuild_Cmds=copy $(OutDir)\CaraWalk.Dll c:\Daten\MyProjects\Libraries\ReleaseDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARAWALK_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARAWALK_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_DEU" /D "AFX_TARG_NEU" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib winmm.lib advapi32.lib shell32.lib opengl32.lib glu32.lib CEtsDlg.lib CFileHeader.lib Cara3DTr.lib CEtsHelp.lib CCaraMat.lib CEtsList.lib CEts3DGL.lib CDibSection.lib CEtsDebug.lib version.lib ETSBIND.lib CEtsDiv.lib EtsRegistry.lib /nologo /version:1.1 /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libmmd" /pdbtype:sept /libpath:"c:\Daten\MyProjects\Libraries\Debug\\"
# SUBTRACT LINK32 /force
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Carawalk.Dll
PostBuild_Cmds=copy $(OutDir)\CaraWalk.Dll c:\Daten\MyProjects\Libraries\DebugDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARAWALK_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARAWALK_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_ENU" /D "AFX_TARG_NEU" /FR /Yu"stdafx.h" /FD /G7 -Qip -O3 -Qipo "_USE_INTEL_COMPILER" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib winmm.lib advapi32.lib shell32.lib opengl32.lib glu32.lib CEtsDlg.lib CFileHeader.lib Cara3DTr.lib CEtsHelp.lib CCaraMat.lib CEtsList.lib CEts3DGL.lib CDibSection.lib CEtsDebug.lib version.lib EtsBind.lib /nologo /version:1.1 /dll /map /machine:I386 /libpath:"c:\Daten\MyProjects\Libraries\Release\\"
# ADD LINK32 comctl32.lib comdlg32.lib winmm.lib opengl32.lib glu32.lib CEtsDlg.lib CFileHeader.lib Cara3DTr.lib CEtsHelp.lib CCaraMat.lib CEtsList.lib CEts3DGL.lib CDibSection.lib version.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib CEtsDebug.lib EtsBind.lib CCaraSdb.lib CEtsDiv.lib EtsRegistry.lib /nologo /version:1.1 /dll /map /machine:I386 /libpath:"c:\Daten\MyProjects\Libraries\Release\\"
# Begin Special Build Tool
OutDir=.\ReleaseEngl
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Carawalk.Dll
PostBuild_Cmds=copy $(OutDir)\CaraWalk.Dll c:\Daten\MyProjects\Libraries\ReleaseEnglDll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CARAWALK - Win32 Release"
# Name "CARAWALK - Win32 Debug"
# Name "CARAWALK - Win32 ReleaseEngl"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BoxDsrc.cpp

!IF  "$(CFG)" == "CARAWALK - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BoxProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxPsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CARAWALK.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraWalk.def
# End Source File
# Begin Source File

SOURCE=.\CARAWALK.rc
# End Source File
# Begin Source File

SOURCE=.\carawalkdll.cpp
# End Source File
# Begin Source File

SOURCE=.\DllError.cpp
# End Source File
# Begin Source File

SOURCE=.\LightDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Material.cpp

!IF  "$(CFG)" == "CARAWALK - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RoomDscr.cpp

!IF  "$(CFG)" == "CARAWALK - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARAWALK - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BoxDscr.h
# End Source File
# Begin Source File

SOURCE=.\BoxProperties.h
# End Source File
# Begin Source File

SOURCE=.\BoxPsDlg.h
# End Source File
# Begin Source File

SOURCE=.\CaraWalkDll.h
# End Source File
# Begin Source File

SOURCE=.\DllError.h
# End Source File
# Begin Source File

SOURCE=..\Ets3dgl\CEts3DGL\ETS3DGL.h
# End Source File
# Begin Source File

SOURCE=.\LightDlg.h
# End Source File
# Begin Source File

SOURCE=.\Material.h
# End Source File
# Begin Source File

SOURCE=.\RoomDscr.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\LookAround.cur
# End Source File
# Begin Source File

SOURCE=.\room_wal.bmp
# End Source File
# Begin Source File

SOURCE=.\turnlight.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
