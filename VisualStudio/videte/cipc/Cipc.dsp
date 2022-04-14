# Microsoft Developer Studio Project File - Name="cipc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cipc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cipc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cipc.mak" CFG="cipc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cipc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cipc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/CIPC", XDEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cipc - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cipc___Win32_Release"
# PROP BASE Intermediate_Dir "cipc___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /Op /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdcipc.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 version.lib Winmm.lib wkclasses.lib /nologo /base:"0x11100000" /subsystem:windows /dll /pdb:none /map /machine:I386 /mapinfo:lines
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
TargetPath=.\release\cipc.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=v:\bin\tools\syscopy.exe $(TargetPath)	copy $(TargetPath) c:\security
# End Special Build Tool

!ELSEIF  "$(CFG)" == "cipc - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cipc___Win32_Debug"
# PROP BASE Intermediate_Dir "cipc___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdcipc.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib Winmm.lib wkclassesdebug.lib /nologo /base:"0x11100000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"Debug/cipcdebug.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\debug\cipcdebug.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=v:\bin\tools\syscopy.exe $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "cipc - Win32 Release"
# Name "cipc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "CIPC Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cipc.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCArchivRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcchannel.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcdatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseClient.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCDrive.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCDrives.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcextramemory.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcfetchresult.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCField.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcinput.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcinputclient.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCInt64.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcoutput.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcoutputclient.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCSequenceRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrolclientside.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCFetch.cpp
# End Source File
# End Group
# Begin Group "WK Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wk_debugoptions.cpp
# End Source File
# Begin Source File

SOURCE=.\wk_dongle.cpp
# End Source File
# Begin Source File

SOURCE=.\wk_timer.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\archivinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\autologout.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraControl.cpp
# End Source File
# Begin Source File

SOURCE=.\cipc.def

!IF  "$(CFG)" == "cipc - Win32 Release"

!ELSEIF  "$(CFG)" == "cipc - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cipc.rc
# End Source File
# Begin Source File

SOURCE=.\cipcdebug.def

!IF  "$(CFG)" == "cipc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cipc - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cipcdebugU.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\cipcdll.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCMedia.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCMediaClient.cpp
# End Source File
# Begin Source File

SOURCE=.\cipcU.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\connectionrecord.cpp
# End Source File
# Begin Source File

SOURCE=.\host.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCActivityMask.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCDataCarrier.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCInputFileUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\MediaSampleRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\notificationmessage.cpp
# End Source File
# Begin Source File

SOURCE=.\permission.cpp
# End Source File
# Begin Source File

SOURCE=.\picturerecord.cpp
# End Source File
# Begin Source File

SOURCE=.\secid.cpp
# End Source File
# Begin Source File

SOURCE=.\secidarray.cpp
# End Source File
# Begin Source File

SOURCE=.\sectimer.cpp
# End Source File
# Begin Source File

SOURCE=.\sectimespan.cpp
# End Source File
# Begin Source File

SOURCE=.\stdcipc.cpp
# ADD CPP /Yc"stdcipc.h"
# End Source File
# Begin Source File

SOURCE=.\systemtime.cpp
# End Source File
# Begin Source File

SOURCE=.\timedmessage.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeZoneInformation.cpp
# End Source File
# Begin Source File

SOURCE=.\user.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "CIPC Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cipc.h
# End Source File
# Begin Source File

SOURCE=.\CIPCArchivRecord.h
# End Source File
# Begin Source File

SOURCE=.\CIPCArchivRecord.inl
# End Source File
# Begin Source File

SOURCE=.\cipcchannel.h
# End Source File
# Begin Source File

SOURCE=.\cipcdatabase.h
# End Source File
# Begin Source File

SOURCE=.\cipcdatabase.inl
# End Source File
# Begin Source File

SOURCE=.\CIPCDatabaseClient.h
# End Source File
# Begin Source File

SOURCE=.\CIPCDrive.h
# End Source File
# Begin Source File

SOURCE=.\CIPCDrives.h
# End Source File
# Begin Source File

SOURCE=.\cipcextramemory.h
# End Source File
# Begin Source File

SOURCE=.\cipcfetchresult.h
# End Source File
# Begin Source File

SOURCE=.\CIPCField.h
# End Source File
# Begin Source File

SOURCE=.\CIPCField.inl
# End Source File
# Begin Source File

SOURCE=.\cipcinput.h
# End Source File
# Begin Source File

SOURCE=.\cipcinputclient.h
# End Source File
# Begin Source File

SOURCE=.\cipcinputrecord.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInt64.h
# End Source File
# Begin Source File

SOURCE=.\cipcoutput.h
# End Source File
# Begin Source File

SOURCE=.\cipcoutputclient.h
# End Source File
# Begin Source File

SOURCE=.\cipcoutputrecord.h
# End Source File
# Begin Source File

SOURCE=.\CIPCSequenceRecord.h
# End Source File
# Begin Source File

SOURCE=.\CIPCSequenceRecord.inl
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrol.h
# End Source File
# Begin Source File

SOURCE=.\cipcservercontrolclientside.h
# End Source File
# Begin Source File

SOURCE=.\cipcstringdefs.h
# End Source File
# Begin Source File

SOURCE=.\IPCFetch.h
# End Source File
# End Group
# Begin Group "WK Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wk.h
# End Source File
# Begin Source File

SOURCE=.\WK_DebugOptions.h
# End Source File
# Begin Source File

SOURCE=.\wk_dongle.h
# End Source File
# Begin Source File

SOURCE=.\WK_Magic.h
# End Source File
# Begin Source File

SOURCE=.\WK_Names.h
# End Source File
# Begin Source File

SOURCE=.\wk_template.h
# End Source File
# Begin Source File

SOURCE=.\wk_timer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\alldongletransfers.h
# End Source File
# Begin Source File

SOURCE=.\analyse.h
# End Source File
# Begin Source File

SOURCE=.\ArchivDefines.h
# End Source File
# Begin Source File

SOURCE=.\archivinfo.h
# End Source File
# Begin Source File

SOURCE=.\autologout.h
# End Source File
# Begin Source File

SOURCE=.\cameracontrol.h
# End Source File
# Begin Source File

SOURCE=.\CIPCMedia.h
# End Source File
# Begin Source File

SOURCE=.\CIPCMediaClient.h
# End Source File
# Begin Source File

SOURCE=.\CipcMediaRecord.h
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcOutputUnit.h
# End Source File
# Begin Source File

SOURCE=.\clogfile.h
# End Source File
# Begin Source File

SOURCE=.\connectionrecord.h
# End Source File
# Begin Source File

SOURCE=.\ErrorMessage.h
# End Source File
# Begin Source File

SOURCE=.\fastapi.h
# End Source File
# Begin Source File

SOURCE=.\getlongpathname.h
# End Source File
# Begin Source File

SOURCE=.\hlapi_c.h
# End Source File
# Begin Source File

SOURCE=.\host.h
# End Source File
# Begin Source File

SOURCE=.\inifile.h
# End Source File
# Begin Source File

SOURCE=.\IPCActivityMask.h
# End Source File
# Begin Source File

SOURCE=.\IPCDataCarrier.h
# End Source File
# Begin Source File

SOURCE=.\IPCInputFileUpdate.h
# End Source File
# Begin Source File

SOURCE=.\MediaSampleRecord.h
# End Source File
# Begin Source File

SOURCE=.\notificationmessage.h
# End Source File
# Begin Source File

SOURCE=.\permission.h
# End Source File
# Begin Source File

SOURCE=.\picturedef.h
# End Source File
# Begin Source File

SOURCE=.\picturerecord.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\secid.h
# End Source File
# Begin Source File

SOURCE=.\secidarray.h
# End Source File
# Begin Source File

SOURCE=.\sectimer.h
# End Source File
# Begin Source File

SOURCE=.\sectimespan.h
# End Source File
# Begin Source File

SOURCE=.\stdcipc.h
# End Source File
# Begin Source File

SOURCE=.\systemtime.h
# End Source File
# Begin Source File

SOURCE=.\timedmessage.h
# End Source File
# Begin Source File

SOURCE=.\TimeZoneInformation.h
# End Source File
# Begin Source File

SOURCE=.\UpdateDefines.h
# End Source File
# Begin Source File

SOURCE=.\updatefns.h
# End Source File
# Begin Source File

SOURCE=.\updatehandle.h
# End Source File
# Begin Source File

SOURCE=.\user.h
# End Source File
# Begin Source File

SOURCE=.\videojob.h
# End Source File
# Begin Source File

SOURCE=.\WK_New.inl
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cipc.rc2
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\release\cipc.dll
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\debug\cipcdebug.dll
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\release\cipc.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\debug\cipcdebug.lib
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Hlapi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlapi_c.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\hardlock\nt_call.obj
# End Source File
# Begin Source File

SOURCE=.\hardlock\netstuff.obj
# End Source File
# Begin Source File

SOURCE=.\hardlock\api_3nnm.obj
# End Source File
# End Group
# Begin Source File

SOURCE=.\history.txt
# End Source File
# End Target
# End Project
