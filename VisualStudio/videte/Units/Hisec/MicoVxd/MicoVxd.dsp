# Microsoft Developer Studio Project File - Name="MicoVxd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=MicoVxd - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MicoVxd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MicoVxd.mak" CFG="MicoVxd - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MicoVxd - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "MicoVxd - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Project/Units/Micounit/MicoVxd", QHLAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "MicoVxd - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Cmd_Line "NMAKE /f Makefile"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Makefile.exe"
# PROP BASE Bsc_Name "Makefile.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Cmd_Line "NMAKE /f Makefile CFG=Release"
# PROP Rebuild_Opt "/a"
# PROP Target_File "MicoVxd.exe"
# PROP Bsc_Name "MicoVxd.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "MicoVxd - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Cmd_Line "NMAKE /f Makefile"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Makefile.exe"
# PROP BASE Bsc_Name "Makefile.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Cmd_Line "NMAKE /f Makefile CFG=Debug"
# PROP Rebuild_Opt "/a"
# PROP Target_File "MicoVxd.exe"
# PROP Bsc_Name "MicoVxd.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "MicoVxd - Win32 Release"
# Name "MicoVxd - Win32 Debug"

!IF  "$(CFG)" == "MicoVxd - Win32 Release"

!ELSEIF  "$(CFG)" == "MicoVxd - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\MicoVxd.asm
# End Source File
# Begin Source File

SOURCE=.\MicoVxd.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\MiCoReg.h
# End Source File
# Begin Source File

SOURCE=.\MicoVxd.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\Mico.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\Makefile
# End Source File
# Begin Source File

SOURCE=.\MicoVxd.def
# End Source File
# Begin Source File

SOURCE=.\MicoVxd.mak
# End Source File
# End Target
# End Project
