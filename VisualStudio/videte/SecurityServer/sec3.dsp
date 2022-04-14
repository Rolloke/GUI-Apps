# Microsoft Developer Studio Project File - Name="sec3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sec3 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sec3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sec3.mak" CFG="sec3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sec3 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "sec3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sec3 - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "sec3 - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/SecurityServer", UCEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sec3 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\WinDebug\Output"
# PROP Intermediate_Dir ".\WinDebug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\sec3.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 CipcDebug.lib OemGuiD.lib WKClassesDebug.lib vimaged.lib version.lib ijl15.lib /nologo /subsystem:windows /debug /machine:I386 /out:".\WinDebug\sec3.exe"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "sec3 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\sec3.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Cipc.lib OemGui.lib WKClasses.lib vimage.lib version.lib ijl15.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=.\Release\sec3.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) c:\security
# End Special Build Tool

!ELSEIF  "$(CFG)" == "sec3 - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "sec3___Win32_Enu"
# PROP BASE Intermediate_Dir "sec3___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\sec3.bsc"
# ADD BSC32 /nologo /o".\sec3.bsc"
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib version.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 Cipc.lib OemGui.lib WKClasses.lib vimage.lib version.lib ijl15.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "sec3 - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "sec3___Win32_Ita"
# PROP BASE Intermediate_Dir "sec3___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\sec3.bsc"
# ADD BSC32 /nologo /o".\sec3.bsc"
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib version.lib WKClasses.lib h263enc.lib jpeglib.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 Cipc.lib OemGui.lib WKClasses.lib h263enc.lib jpeglib.lib version.lib ijl15.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "sec3 - Win32 Debug"
# Name "sec3 - Win32 Release"
# Name "sec3 - Win32 Enu"
# Name "sec3 - Win32 Ita"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "CIPC Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CipcDatabaseClientServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcInputServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcInputServerClient.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcOutputServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcOutputServerClient.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcServerControlServerSide.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Activation.cpp
# End Source File
# Begin Source File

SOURCE=.\ActualImageProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupThread.cpp
# End Source File
# Begin Source File

SOURCE=.\CallProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\CallThread.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseServerBackup.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCInputServerCall.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputActualImage.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputServerCall.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientEncodingProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientOverlayProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientRelayProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\EncoderThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GuardTourProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\GuardTourThread.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\inputlist.cpp
# End Source File
# Begin Source File

SOURCE=.\IOGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCAudioServer.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCAudioServerClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCServerToVision.cpp
# End Source File
# Begin Source File

SOURCE=.\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MDProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\media.cpp
# End Source File
# Begin Source File

SOURCE=.\MultipleCallProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\outputlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Process.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessListDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessListView.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessMacro.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessScheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessSchedulerAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\processschedulervideo.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordingProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestCollectors.cpp
# End Source File
# Begin Source File

SOURCE=.\sec3.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TimerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\TranscoderThread.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoOutProcess.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "CIPC Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CipcDatabaseClientServer.h
# End Source File
# Begin Source File

SOURCE=.\CipcInputServer.h
# End Source File
# Begin Source File

SOURCE=.\CipcInputServerClient.h
# End Source File
# Begin Source File

SOURCE=.\CipcOutputServer.h
# End Source File
# Begin Source File

SOURCE=.\CipcOutputServerClient.h
# End Source File
# Begin Source File

SOURCE=.\CipcServerControlServerSide.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Activation.h
# End Source File
# Begin Source File

SOURCE=.\activation.inl
# End Source File
# Begin Source File

SOURCE=.\ActualImageProcess.h
# End Source File
# Begin Source File

SOURCE=.\AudioProcess.h
# End Source File
# Begin Source File

SOURCE=.\BackupProcess.h
# End Source File
# Begin Source File

SOURCE=.\BackupThread.h
# End Source File
# Begin Source File

SOURCE=.\CallProcess.h
# End Source File
# Begin Source File

SOURCE=.\CallThread.h
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseServerBackup.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputServerCall.h
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputActualImage.h
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputServerCall.h
# End Source File
# Begin Source File

SOURCE=.\ClientEncodingProcess.h
# End Source File
# Begin Source File

SOURCE=.\ClientOverlayProcess.h
# End Source File
# Begin Source File

SOURCE=.\ClientProcess.h
# End Source File
# Begin Source File

SOURCE=.\ClientRelayProcess.h
# End Source File
# Begin Source File

SOURCE=.\EncoderThread.h
# End Source File
# Begin Source File

SOURCE=.\GuardTourProcess.h
# End Source File
# Begin Source File

SOURCE=.\GuardTourThread.h
# End Source File
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\inputlist.h
# End Source File
# Begin Source File

SOURCE=.\IOGroup.h
# End Source File
# Begin Source File

SOURCE=.\IPCAudioServer.h
# End Source File
# Begin Source File

SOURCE=.\IPCAudioServerClient.h
# End Source File
# Begin Source File

SOURCE=.\IPCServerToVision.h
# End Source File
# Begin Source File

SOURCE=.\mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\MDProcess.h
# End Source File
# Begin Source File

SOURCE=.\media.h
# End Source File
# Begin Source File

SOURCE=.\MultipleCallProcess.h
# End Source File
# Begin Source File

SOURCE=.\output.h
# End Source File
# Begin Source File

SOURCE=.\Output.inl
# End Source File
# Begin Source File

SOURCE=.\outputlist.h
# End Source File
# Begin Source File

SOURCE=.\Process.h
# End Source File
# Begin Source File

SOURCE=.\process.inl
# End Source File
# Begin Source File

SOURCE=.\ProcessListDoc.h
# End Source File
# Begin Source File

SOURCE=.\ProcessListView.h
# End Source File
# Begin Source File

SOURCE=.\ProcessMacro.h
# End Source File
# Begin Source File

SOURCE=.\processmacro.inl
# End Source File
# Begin Source File

SOURCE=.\ProcessScheduler.h
# End Source File
# Begin Source File

SOURCE=.\ProcessSchedulerAudio.h
# End Source File
# Begin Source File

SOURCE=.\processschedulervideo.h
# End Source File
# Begin Source File

SOURCE=.\RecordingProcess.h
# End Source File
# Begin Source File

SOURCE=.\RelayProcess.h
# End Source File
# Begin Source File

SOURCE=.\RequestCollector.h
# End Source File
# Begin Source File

SOURCE=.\RequestCollectors.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sec3.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\TimerThread.h
# End Source File
# Begin Source File

SOURCE=.\TranscoderThread.h
# End Source File
# Begin Source File

SOURCE=.\VideoOutProcess.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\sec3.ico
# End Source File
# Begin Source File

SOURCE=.\sec3.rc

!IF  "$(CFG)" == "sec3 - Win32 Debug"

!ELSEIF  "$(CFG)" == "sec3 - Win32 Release"

!ELSEIF  "$(CFG)" == "sec3 - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "sec3 - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\sec3.rc2
# End Source File
# Begin Source File

SOURCE=.\sec3enu.rc

!IF  "$(CFG)" == "sec3 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "sec3 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "sec3 - Win32 Enu"

!ELSEIF  "$(CFG)" == "sec3 - Win32 Ita"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sec3Ita.rc

!IF  "$(CFG)" == "sec3 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "sec3 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "sec3 - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "sec3 - Win32 Ita"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\test352288.263
# End Source File
# Begin Source File

SOURCE=.\test720280.263
# End Source File
# End Group
# Begin Group "Doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActionsDoc.txt
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\Server.txt
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sec3enu.lng
# End Source File
# Begin Source File

SOURCE=.\sec3Ita.lng
# End Source File
# End Group
# End Target
# End Project
# Section sec3 : {03EB4294-339F-11CF-B026-080000199489}
# 	1:17:CG_IDS_DISK_SPACE:104
# 	1:19:CG_IDS_PHYSICAL_MEM:103
# 	1:25:CG_IDS_DISK_SPACE_UNAVAIL:105
# 	2:10:SysInfoKey:1234
# End Section
