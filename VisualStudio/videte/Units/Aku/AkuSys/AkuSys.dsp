# Microsoft Developer Studio Project File - Name="AkuSys" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=AkuSys - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AkuSys.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AkuSys.mak" CFG="AkuSys - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AkuSys - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "AkuSys - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Project/Units/Aku/AkuSys", LWCBAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "AkuSys - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f AkuSys.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "AkuSys.exe"
# PROP BASE Bsc_Name "AkuSys.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "d:\Project\Units\Aku\AkuSys\BuildRelease d:\Project\Units\Aku\AkuSys"
# PROP Rebuild_Opt "-c"
# PROP Target_File "AkuSys.exe"
# PROP Bsc_Name "AkuSys.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "AkuSys - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f AkuSys.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "AkuSys.exe"
# PROP BASE Bsc_Name "AkuSys.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "d:\Project\Units\Aku\AkuSys\BuildDebug d:\Project\Units\Aku\AkuSys"
# PROP Rebuild_Opt "-c"
# PROP Target_File "AkuSys.exe"
# PROP Bsc_Name "AkuSys.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "AkuSys - Win32 Release"
# Name "AkuSys - Win32 Debug"

!IF  "$(CFG)" == "AkuSys - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuSys - Win32 Debug"

!ENDIF 

# Begin Group "Source"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=.\AkuISAStuff.c
# End Source File
# Begin Source File

SOURCE=.\AkuSys.c
# End Source File
# End Group
# Begin Group "Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\AkuIOctl.H
# End Source File
# Begin Source File

SOURCE=.\AkuSys.h
# End Source File
# Begin Source File

SOURCE=.\Driver.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\AkuSys.rc
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bin\Aku.sys
# End Source File
# Begin Source File

SOURCE=.\BuildDebug.bat
# End Source File
# Begin Source File

SOURCE=.\BuildRelease.bat
# End Source File
# Begin Source File

SOURCE=.\SOURCES
# End Source File
# End Group
# End Target
# End Project
