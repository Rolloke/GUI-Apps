# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

!IF "$(CFG)" == ""
CFG=MAKEFILE - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MAKEFILE - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MAKEFILE - Win32 Release" && "$(CFG)" !=\
 "MAKEFILE - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAKEFILE.MAK" CFG="MAKEFILE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAKEFILE - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "MAKEFILE - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "MAKEFILE - Win32 Debug"

!IF  "$(CFG)" == "MAKEFILE - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f MAKEFILE"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "MAKEFILE.EXE"
# PROP BASE Bsc_Name "MAKEFILE.BSC"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /f MAKEFILE"
# PROP Rebuild_Opt "/a"
# PROP Target_File "MAKEFILE.EXE"
# PROP Bsc_Name "MAKEFILE.BSC"
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "MAKEFILE - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f MAKEFILE"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "MAKEFILE.EXE"
# PROP BASE Bsc_Name "MAKEFILE.BSC"
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /f MAKEFILE"
# PROP Rebuild_Opt "/a"
# PROP Target_File "MAKEFILE.EXE"
# PROP Bsc_Name "MAKEFILE.BSC"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ENDIF 

################################################################################
# Begin Target

# Name "MAKEFILE - Win32 Release"
# Name "MAKEFILE - Win32 Debug"

!IF  "$(CFG)" == "MAKEFILE - Win32 Release"

".\MAKEFILE.EXE" : 
   CD D:\Project\PortAccess\sys
   NMAKE /f MAKEFILE

!ELSEIF  "$(CFG)" == "MAKEFILE - Win32 Debug"

".\MAKEFILE.EXE" : 
   CD D:\Project\PortAccess\sys
   NMAKE /f MAKEFILE

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\GENPORT.C

!IF  "$(CFG)" == "MAKEFILE - Win32 Release"

!ELSEIF  "$(CFG)" == "MAKEFILE - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GENPORT.H

!IF  "$(CFG)" == "MAKEFILE - Win32 Release"

!ELSEIF  "$(CFG)" == "MAKEFILE - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Project\PortAccess\GPIOCTL.H

!IF  "$(CFG)" == "MAKEFILE - Win32 Release"

!ELSEIF  "$(CFG)" == "MAKEFILE - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
