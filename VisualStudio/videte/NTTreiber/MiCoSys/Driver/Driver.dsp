# Microsoft Developer Studio Project File - Name="Driver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Driver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Driver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Driver.mak" CFG="Driver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Driver - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Driver - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Project/NTTreiber/MiCoSys/Driver", QPVAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Driver - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Driver.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Driver.exe"
# PROP BASE Bsc_Name "Driver.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "d:\Project\NTTreiber\MiCoSys\BuildRelease d:\Project\NTTreiber\MiCoSys"
# PROP Rebuild_Opt "-c"
# PROP Target_File "Driver.exe"
# PROP Bsc_Name "Driver.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Driver - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Driver.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Driver.exe"
# PROP BASE Bsc_Name "Driver.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "d:\Project\NTTreiber\MiCoSys\BuildDebug d:\Project\NTTreiber\MiCoSys"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Driver.exe"
# PROP Bsc_Name "Driver.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Driver - Win32 Release"
# Name "Driver - Win32 Debug"

!IF  "$(CFG)" == "Driver - Win32 Release"

!ELSEIF  "$(CFG)" == "Driver - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=.\mico.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\cbt829.h
# End Source File
# Begin Source File

SOURCE=.\cbt856.h
# End Source File
# Begin Source File

SOURCE=.\cmico.h
# End Source File
# Begin Source File

SOURCE=.\h55_tbls.h
# End Source File
# Begin Source File

SOURCE=.\hardware.h
# End Source File
# Begin Source File

SOURCE=..\INC\mico.h
# End Source File
# Begin Source File

SOURCE=.\zr050.h
# End Source File
# Begin Source File

SOURCE=.\zr050_io.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\BuildDebug.bat
# End Source File
# Begin Source File

SOURCE=..\BuildRelease.bat
# End Source File
# Begin Source File

SOURCE=..\History.txt
# End Source File
# Begin Source File

SOURCE=..\OEMSETUP.INF
# End Source File
# Begin Source File

SOURCE=.\SOURCES
# End Source File
# End Group
# Begin Group "VCKernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\vckernel\VCDISP.C
# End Source File
# Begin Source File

SOURCE=..\vckernel\VCINIT.C
# End Source File
# Begin Source File

SOURCE=..\vckernel\VCISR.C
# End Source File
# Begin Source File

SOURCE=..\vckernel\VCKPRIV.H
# End Source File
# Begin Source File

SOURCE=..\vckernel\VCLIB.C
# End Source File
# Begin Source File

SOURCE=..\vckernel\VCQUEUE.C
# End Source File
# End Group
# End Target
# End Project
