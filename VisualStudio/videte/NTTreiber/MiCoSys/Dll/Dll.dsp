# Microsoft Developer Studio Project File - Name="Dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dll.mak" CFG="Dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Dll - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Dll - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Project/NTTreiber/MiCoSys/Dll", DQVAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Dll - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Dll.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Dll.exe"
# PROP BASE Bsc_Name "Dll.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "d:\Project\NTTreiber\MiCoSys\BuildRelease d:\Project\NTTreiber\MiCoSys"
# PROP Rebuild_Opt "-c"
# PROP Target_File "Dll.exe"
# PROP Bsc_Name "Dll.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Dll - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Dll.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Dll.exe"
# PROP BASE Bsc_Name "Dll.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "d:\Project\NTTreiber\MiCoSys\BuildDebug d:\Project\NTTreiber\MiCoSys"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Dll.exe"
# PROP Bsc_Name "Dll.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Dll - Win32 Release"
# Name "Dll - Win32 Debug"

!IF  "$(CFG)" == "Dll - Win32 Release"

!ELSEIF  "$(CFG)" == "Dll - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=.\DIALOG.C
# End Source File
# Begin Source File

SOURCE=.\DRVPROC.C
# End Source File
# Begin Source File

SOURCE=.\INSTALL.C
# End Source File
# Begin Source File

SOURCE=.\PALETTE.C
# End Source File
# Begin Source File

SOURCE=.\VMSG.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\DIALOG.H
# End Source File
# Begin Source File

SOURCE=.\micou.h
# End Source File
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\mico.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\SOURCES
# End Source File
# End Target
# End Project
