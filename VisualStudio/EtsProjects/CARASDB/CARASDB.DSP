# Microsoft Developer Studio Project File - Name="CARASDB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CARASDB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CARASDB.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CARASDB.MAK" CFG="CARASDB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CARASDB - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARASDB - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARASDB - Win32 ReleaseEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARASDB - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /GB /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_DEU" /D "AFX_TARG_NEU" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib Comctl32.lib shell32.lib CEtsDlg.lib CCaraWalk.lib CFileHeader.lib CEtsHelp.lib CCaramat.lib CEtsLsmb.lib EtsBind.lib CEtsDebug.lib advapi32.lib CEtsList.lib CCaraUnzip.lib CEtsDiv.lib /nologo /subsystem:windows /dll /map /machine:I386 /nodefaultlib:"libmmd" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der DLL-Datei
PostBuild_Cmds=copy $(OutDir)\CARASDB.DLL c:\Daten\MyProjects\Libraries\ReleaseDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARASDB - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_DEU" /D "AFX_TARG_NEU" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib Comctl32.lib shell32.lib CEtsDlg.lib CCaraWalk.lib CFileHeader.lib CEtsHelp.lib CCaramat.lib CEtsLsmb.lib ETSBIND.lib CEtsDebug.lib advapi32.lib CEtsList.lib CCaraUnzip.lib CEtsDiv.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der DLL-Datei
PostBuild_Cmds=copy $(OutDir)\CARASDB.DLL c:\Daten\MyProjects\Libraries\DebugDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARASDB - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_ENU" /D "AFX_TARG_NEU" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib Comctl32.lib shell32.lib CEtsDlg.lib CCaraWalk.lib CFileHeader.lib CEtsHelp.lib CCaramat.lib CEtsLsmb.lib EtsBind.lib /nologo /subsystem:windows /dll /map /machine:I386 /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 comdlg32.lib Comctl32.lib CEtsDlg.lib CCaraWalk.lib CFileHeader.lib CEtsHelp.lib CCaramat.lib CEtsLsmb.lib CEtsList.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib CEtsDebug.lib EtsBind.lib CCaraSdb.lib CCaraUnzip.lib CEtsDiv.lib /nologo /subsystem:windows /dll /map /machine:I386 /nodefaultlib:"libmmd" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\ReleaseEngl
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der DLL-Datei
PostBuild_Cmds=copy $(OutDir)\CARASDB.DLL c:\Daten\MyProjects\Libraries\ReleaseEnglDll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CARASDB - Win32 Release"
# Name "CARASDB - Win32 Debug"
# Name "CARASDB - Win32 ReleaseEngl"
# Begin Source File

SOURCE=.\CaraSdb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CaraSdb.bmp
# End Source File
# Begin Source File

SOURCE=.\CARASDB.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraSdb.def
# End Source File
# Begin Source File

SOURCE=.\CaraSDB.rc
# End Source File
# Begin Source File

SOURCE=..\Libraries\CCaraSDB.h
# End Source File
# Begin Source File

SOURCE=.\CCaraSdbDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CCaraSdbDlg.h
# End Source File
# Begin Source File

SOURCE=.\CTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\CTreeView.h
# End Source File
# Begin Source File

SOURCE=".\dödel.txt"
# End Source File
# Begin Source File

SOURCE=..\Libraries\LsBoxDb_cfg.h
# End Source File
# Begin Source File

SOURCE=.\MyDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\SdbFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SdbFileDlg.h
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
# End Target
# End Project
