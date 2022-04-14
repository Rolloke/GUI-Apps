# Microsoft Developer Studio Project File - Name="CCaraBox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CCaraBox - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CCaraBox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CCaraBox.mak" CFG="CCaraBox - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CCaraBox - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CCaraBox - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "CCaraBox - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\CCaraBox.lib c:\Daten\MyProjects\Libraries\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CCaraBox - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\CCaraBox.lib c:\Daten\MyProjects\Libraries\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CCaraBox - Win32 Release"
# Name "CCaraBox - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CCaraBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Skript1.rc
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CCaraBox.h
# End Source File
# Begin Source File

SOURCE=.\ETSBIND.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\res\bandpass.ico
# End Source File
# Begin Source File

SOURCE=..\res\hipass.ico
# End Source File
# Begin Source File

SOURCE=..\res\Icon.ico
# End Source File
# Begin Source File

SOURCE=..\res\idc_3d_p.ico
# End Source File
# Begin Source File

SOURCE=..\res\liteblue.ico
# End Source File
# Begin Source File

SOURCE=..\res\litecuto.ico
# End Source File
# Begin Source File

SOURCE=..\res\litedst.ico
# End Source File
# Begin Source File

SOURCE=..\res\liteexp.ico
# End Source File
# Begin Source File

SOURCE=..\res\litegreen.ico
# End Source File
# Begin Source File

SOURCE=..\res\litephi.ico
# End Source File
# Begin Source File

SOURCE=..\res\litered.ico
# End Source File
# Begin Source File

SOURCE=..\res\liteteta.ico
# End Source File
# Begin Source File

SOURCE=..\res\lopass.ico
# End Source File
# Begin Source File

SOURCE=..\res\tran_x.ico
# End Source File
# Begin Source File

SOURCE=..\res\tran_y.ico
# End Source File
# Begin Source File

SOURCE=..\res\tran_z.ico
# End Source File
# Begin Source File

SOURCE=..\res\turnphi.ico
# End Source File
# Begin Source File

SOURCE=..\res\turnteta.ico
# End Source File
# End Target
# End Project
