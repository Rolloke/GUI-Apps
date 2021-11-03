# Microsoft Developer Studio Project File - Name="ETS3DGL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ETS3DGL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ETS3DGL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ETS3DGL.mak" CFG="ETS3DGL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ETS3DGL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ETS3DGL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ETS3DGL - Win32 ReleaseEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ETS3DGL - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETS3DGL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETS3DGL_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "_USE_INTEL_COMPILER" /YX /FD /G7 -Qip -O3 -Qipo /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib uuid.lib opengl32.lib glu32.lib CDibSection.lib CEtsDebug.lib EtsBind.lib dinput.lib Dxguid.lib /nologo /version:1.0 /dll /map /machine:I386 /libpath:"c:\Daten\MyProjects\Libraries\Release" -Qipo
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren von ETS3DGL.DLL
PostBuild_Cmds=copy $(OutDir)\ETS3DGL.DLL c:\Daten\MyProjects\Libraries\ReleaseDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ETS3DGL - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETS3DGL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETS3DGL_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib opengl32.lib glu32.lib CEtsDebug.lib CDibsection.lib ETSBIND.lib dinput.lib Dxguid.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"c:\Daten\MyProjects\Libraries\Debug"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren von ETS3DGL.DLL
PostBuild_Cmds=copy $(OutDir)\ETS3DGL.DLL c:\Daten\MyProjects\Libraries\DebugDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ETS3DGL - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETS3DGL_EXPORTS" /FR /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ETS3DGL_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /D "_USE_INTEL_COMPILER" /YX /FD /G7 -Qip -O3 -Qipo /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib uuid.lib opengl32.lib glu32.lib CDibSection.lib CEtsDebug.lib EtsBind.lib /nologo /version:1.0 /dll /map /machine:I386 /libpath:"c:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 uuid.lib opengl32.lib glu32.lib CDibSection.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib CEtsDebug.lib EtsBind.lib CCaraSdb.lib dinput.lib Dxguid.lib /nologo /version:1.0 /dll /map /machine:I386 /libpath:"c:\Daten\MyProjects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\ReleaseEngl
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren von ETS3DGL.DLL
PostBuild_Cmds=copy $(OutDir)\ETS3DGL.DLL c:\Daten\MyProjects\Libraries\ReleaseEnglDll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ETS3DGL - Win32 Release"
# Name "ETS3DGL - Win32 Debug"
# Name "ETS3DGL - Win32 ReleaseEngl"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ETS3DGL.cpp
# End Source File
# Begin Source File

SOURCE=.\ETS3DGL.def
# End Source File
# Begin Source File

SOURCE=.\ETS3DGL.rc
# End Source File
# Begin Source File

SOURCE=.\ETS3DGL1.cpp
# End Source File
# Begin Source File

SOURCE=.\ETS3DGL2.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\diError.h
# End Source File
# Begin Source File

SOURCE=.\ETS3DGL.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
