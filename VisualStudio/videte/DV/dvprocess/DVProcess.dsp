# Microsoft Developer Studio Project File - Name="DVProcess" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DVProcess - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DVProcess.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DVProcess.mak" CFG="DVProcess - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DVProcess - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DVProcess - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DVProcess - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "DVProcess - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "DVProcess - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "DVProcess - Win32 EnuUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DV/DVProcess", BAYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DVProcess - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib wkclasses.lib vimage.lib HardwareInfo.lib ijl15.lib /nologo /subsystem:windows /map /machine:I386
# Begin Special Build Tool
TargetPath=.\Release\DVProcess.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVProcess - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_H263" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebug.lib wkclassesdebug.lib vimaged.lib HardwareInfod.lib ijl15.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "DVProcess - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVProcess___Win32_Enu"
# PROP BASE Intermediate_Dir "DVProcess___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipc.lib wkclasses.lib vimage.lib HardwareInfo.lib ijl15.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "DVProcess - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DVProcess___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "DVProcess___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_H263" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_H263" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcdebug.lib wkclassesdebug.lib jpeglibd.lib h263encd.lib HardwareInfod.lib ijl15.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcDebugU.lib wkclassesDebugU.lib vimageDU.lib HardwareInfoDU.lib ijl15.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"V:\project\Cipc\DebugUnicode"

!ELSEIF  "$(CFG)" == "DVProcess - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVProcess___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "DVProcess___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib wkclasses.lib jpeglib.lib h263enc.lib HardwareInfo.lib ijl15.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipcU.lib wkclassesU.lib vimageU.lib HardwareInfoU.lib ijl15.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# Begin Special Build Tool
TargetPath=.\ReleaseUnicode\DVProcess.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DVX\ /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DVProcess - Win32 EnuUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DVProcess___Win32_EnuUnicode"
# PROP BASE Intermediate_Dir "DVProcess___Win32_EnuUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EnuUnicode"
# PROP Intermediate_Dir "EnuUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcU.lib wkclassesU.lib jpeglibU.lib h263encU.lib HardwareInfoU.lib ijl15.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# ADD LINK32 cipcU.lib wkclassesU.lib vimageU.lib HardwareInfoU.lib ijl15.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"

!ENDIF 

# Begin Target

# Name "DVProcess - Win32 Release"
# Name "DVProcess - Win32 Debug"
# Name "DVProcess - Win32 Enu"
# Name "DVProcess - Win32 DebugUnicode"
# Name "DVProcess - Win32 ReleaseUnicode"
# Name "DVProcess - Win32 EnuUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AudioClient.cpp
# End Source File
# Begin Source File

SOURCE=.\callthread.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrolprocess.cpp
# End Source File
# Begin Source File

SOURCE=.\DVProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\DVProcessDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DVProcessView.cpp
# End Source File
# Begin Source File

SOURCE=.\EncoderThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Input.cpp
# End Source File
# Begin Source File

SOURCE=.\InputClient.cpp
# End Source File
# Begin Source File

SOURCE=.\InputGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\InputList.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcaudioserver.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcaudioserverclient.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCDatabaseProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCInputDVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCOutputDVClient.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\media.cpp
# End Source File
# Begin Source File

SOURCE=.\Output.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputClient.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputList.cpp
# End Source File
# Begin Source File

SOURCE=.\Relay.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Request.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SystemInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TranscoderThread.cpp
# End Source File
# Begin Source File

SOURCE=.\VOutThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AudioClient.h
# End Source File
# Begin Source File

SOURCE=.\callthread.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\CameraGroup.h
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrolprocess.h
# End Source File
# Begin Source File

SOURCE=.\DVProcess.h
# End Source File
# Begin Source File

SOURCE=.\DVProcessDoc.h
# End Source File
# Begin Source File

SOURCE=.\DVProcessView.h
# End Source File
# Begin Source File

SOURCE=.\EncoderThread.h
# End Source File
# Begin Source File

SOURCE=.\Input.h
# End Source File
# Begin Source File

SOURCE=.\InputClient.h
# End Source File
# Begin Source File

SOURCE=.\InputGroup.h
# End Source File
# Begin Source File

SOURCE=.\InputList.h
# End Source File
# Begin Source File

SOURCE=.\ipcaudioserver.h
# End Source File
# Begin Source File

SOURCE=.\ipcaudioserverclient.h
# End Source File
# Begin Source File

SOURCE=.\IPCDatabaseProcess.h
# End Source File
# Begin Source File

SOURCE=.\IPCInputDVClient.h
# End Source File
# Begin Source File

SOURCE=.\IPCOutputDVClient.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\media.h
# End Source File
# Begin Source File

SOURCE=.\Output.h
# End Source File
# Begin Source File

SOURCE=.\OutputClient.h
# End Source File
# Begin Source File

SOURCE=.\OutputGroup.h
# End Source File
# Begin Source File

SOURCE=.\OutputList.h
# End Source File
# Begin Source File

SOURCE=.\Relay.h
# End Source File
# Begin Source File

SOURCE=.\RelayGroup.h
# End Source File
# Begin Source File

SOURCE=.\Request.h
# End Source File
# Begin Source File

SOURCE=.\RequestThread.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SystemInfo.h
# End Source File
# Begin Source File

SOURCE=.\TranscoderThread.h
# End Source File
# Begin Source File

SOURCE=.\VOutThread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DVProcess.ico
# End Source File
# Begin Source File

SOURCE=.\DVProcess.rc

!IF  "$(CFG)" == "DVProcess - Win32 Release"

!ELSEIF  "$(CFG)" == "DVProcess - Win32 Debug"

!ELSEIF  "$(CFG)" == "DVProcess - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVProcess - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "DVProcess - Win32 ReleaseUnicode"

!ELSEIF  "$(CFG)" == "DVProcess - Win32 EnuUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\DVProcess.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DVProcessDoc.ico
# End Source File
# Begin Source File

SOURCE=.\DVProcessEnu.rc

!IF  "$(CFG)" == "DVProcess - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVProcess - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVProcess - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVProcess - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVProcess - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DVProcess - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DVProcessenu.lng
# End Source File
# Begin Source File

SOURCE=.\history.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
