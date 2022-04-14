# Microsoft Developer Studio Project File - Name="MiCoDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MiCoDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MiCoDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MiCoDemo.mak" CFG="MiCoDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MiCoDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MiCoDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Project/Units/Hisec/MiCoDemo", QQUAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /Zp1 /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /incremental:no /map /debug /debugtype:cv /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "MiCoDemo - Win32 Release"
# Name "MiCoDemo - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\CAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\CBT829.cpp
# End Source File
# Begin Source File

SOURCE=.\cbt856.cpp
# End Source File
# Begin Source File

SOURCE=.\CI2C.cpp
# End Source File
# Begin Source File

SOURCE=.\CIo.cpp
# End Source File
# Begin Source File

SOURCE=.\CMico.cpp
# End Source File
# Begin Source File

SOURCE=.\crelay.cpp
# End Source File
# Begin Source File

SOURCE=.\CZoran.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\MiCoUnitApp.cpp
# End Source File
# Begin Source File

SOURCE=.\MiCoUnitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\StandAlone.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\zoranio.cpp
# End Source File
# Begin Source File

SOURCE=.\Zr050.cpp
# End Source File
# End Group
# Begin Group "Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\CAlarm.h
# End Source File
# Begin Source File

SOURCE=.\CBT829.h
# End Source File
# Begin Source File

SOURCE=.\cbt856.h
# End Source File
# Begin Source File

SOURCE=.\CI2C.h
# End Source File
# Begin Source File

SOURCE=.\CIO.h
# End Source File
# Begin Source File

SOURCE=.\CMico.h
# End Source File
# Begin Source File

SOURCE=.\CRelay.h
# End Source File
# Begin Source File

SOURCE=.\CZoran.h
# End Source File
# Begin Source File

SOURCE=.\Dlgcolor.h
# End Source File
# Begin Source File

SOURCE=.\H55_tbls.h
# End Source File
# Begin Source File

SOURCE=.\MicoDefs.h
# End Source File
# Begin Source File

SOURCE=.\MiCoReg.h
# End Source File
# Begin Source File

SOURCE=.\MiCoUnitApp.h
# End Source File
# Begin Source File

SOURCE=.\MiCoUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=V:\Project\Include\WK_Version.h
# End Source File
# Begin Source File

SOURCE=.\ZoranIo.h
# End Source File
# Begin Source File

SOURCE=.\Zr050.h
# End Source File
# Begin Source File

SOURCE=.\Zr050_io.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\res\MiCoUnit.ico
# End Source File
# Begin Source File

SOURCE=.\MicoUnit.rc
# End Source File
# Begin Source File

SOURCE=.\res\MicoUnit.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\MiCoDemo.mak
# End Source File
# End Target
# End Project
