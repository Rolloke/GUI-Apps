# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

!IF "$(CFG)" == ""
CFG=AkuDrv - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to AkuDrv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AkuDrv - Win32 Release" && "$(CFG)" != "AkuDrv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "AkuDrv.mak" CFG="AkuDrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AkuDrv - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "AkuDrv - Win32 Debug" (based on "Win32 (x86) External Target")
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
# PROP Target_Last_Scanned "AkuDrv - Win32 Debug"

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

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
# PROP Target_File "AkuDrv.exe"
# PROP Bsc_Name "AkuDrv.bsc"
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

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
# PROP Target_File "AkuDrv.exe"
# PROP Bsc_Name "AkuDrv.bsc"
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

# Name "AkuDrv - Win32 Release"
# Name "AkuDrv - Win32 Debug"

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

".\AkuDrv.exe" : 
   CD D:\Project\NTTreiber\AkuDrv\sys
   NMAKE /f MAKEFILE

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

".\AkuDrv.exe" : 
   CD D:\Project\NTTreiber\AkuDrv\sys
   NMAKE /f MAKEFILE

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\MAKEFILE

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Akudrv.c

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Genport.reg

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Akudrv.rc

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Akudrv.h

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Project\NTTreiber\Driver.bat

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Project\NTTreiber\AkuDrv\AkuIoctl.h

!IF  "$(CFG)" == "AkuDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "AkuDrv - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
