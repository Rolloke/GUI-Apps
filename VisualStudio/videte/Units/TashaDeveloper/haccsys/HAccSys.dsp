# Microsoft Developer Studio Project File - Name="HAccSys" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=HAccSys - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HAccSys.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HAccSys.mak" CFG="HAccSys - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HAccSys - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "HAccSys - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/TashaDeveloper/haccsys", FNJCAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "HAccSys - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f HAccSys.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "HAccSys.exe"
# PROP BASE Bsc_Name "HAccSys.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "d:\Project\Units\TashaDeveloper\HAccSys\BuildDebug d:\Project\Units\TashaDeveloper\HAccSys"
# PROP Rebuild_Opt "-c"
# PROP Bsc_Name "HAccSys.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "HAccSys - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f HAccSys.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "HAccSys.exe"
# PROP BASE Bsc_Name "HAccSys.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "d:\Project\Units\TashaDeveloper\HAccSys\BuildDebug d:\Project\Units\TashaDeveloper\HAccSys"
# PROP Rebuild_Opt "-c"
# PROP Target_File "HAccSys.exe"
# PROP Bsc_Name "HAccSys.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "HAccSys - Win32 Release"
# Name "HAccSys - Win32 Debug"

!IF  "$(CFG)" == "HAccSys - Win32 Release"

!ELSEIF  "$(CFG)" == "HAccSys - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=.\Eventlog.c
# End Source File
# Begin Source File

SOURCE=.\HAccCommon.c
# End Source File
# Begin Source File

SOURCE=.\HAccISAStuff.c
# End Source File
# Begin Source File

SOURCE=.\HAccPcistuff.c
# End Source File
# Begin Source File

SOURCE=.\HAccSys.c
# End Source File
# Begin Source File

SOURCE=.\perf.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Driver.h
# End Source File
# Begin Source File

SOURCE=.\Eventlog.h
# End Source File
# Begin Source File

SOURCE=.\HAccCommon.h
# End Source File
# Begin Source File

SOURCE=.\HAccIOctl.H
# End Source File
# Begin Source File

SOURCE=.\HAccSys.h
# End Source File
# Begin Source File

SOURCE=.\PERF.H
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\HAccSys.rc
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BuildDebug.bat
# End Source File
# Begin Source File

SOURCE=.\BuildRelease.bat
# End Source File
# Begin Source File

SOURCE=.\Bin\HAcc.sys
# End Source File
# Begin Source File

SOURCE=.\HAccMSG.mc
# End Source File
# Begin Source File

SOURCE=.\MAKEFILE
# End Source File
# Begin Source File

SOURCE=.\SOURCES
# End Source File
# End Group
# End Target
# End Project
