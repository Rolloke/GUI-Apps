# Microsoft Developer Studio Project File - Name="AudioUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AudioUnit - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AudioUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AudioUnit.mak" CFG="AudioUnit - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AudioUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AudioUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AudioUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "AudioUnit - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "AudioUnit - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "AudioUnit - Win32 EnuUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/Audio/AudioUnit", GVHCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AudioUnit - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Strmbase.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShow.lib DeviceDetect.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCMT" /libpath:"..\..\..\DirectShow\lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=.\Release\AudioUnit.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV\ /R /C /F /Y	xcopy $(TargetPath) c:\Security\ /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /Gi /GX /ZI /Od /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CIPCDebug.lib OemGuiD.lib WKClassesDebug.lib atl.lib strmiids.lib comctl32.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShowD.lib DeviceDetectD.lib Dsound.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\DirectShow\lib"

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AudioUnit___Win32_Enu"
# PROP BASE Intermediate_Dir "AudioUnit___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Enu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Strmbase.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShow.lib DeviceDetect.lib /nologo /subsystem:windows /map /machine:I386 /libpath:"..\..\..\DirectShow\lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Strmbase.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShow.lib DeviceDetect.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCMT" /libpath:"..\..\..\DirectShow\lib"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AudioUnit___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "AudioUnit___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /Gm /Gi /GX /ZI /Od /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /Gi /GX /ZI /Od /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPCDebug.lib OemGuiD.lib WKClassesDebug.lib atl.lib strmiids.lib comctl32.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShowD.lib DeviceDetectD.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\DirectShow\lib"
# ADD LINK32 CIPCDebugU.lib OemGuiDU.lib WKClassesDebugU.lib atl.lib strmiids.lib comctl32.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShowDU.lib DeviceDetectDU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\DirectShow\lib" /libpath:"V:\project\Cipc\DebugUnicode"

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AudioUnit___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "AudioUnit___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Strmbase.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShow.lib DeviceDetect.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCMT" /libpath:"..\..\..\DirectShow\lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 CIPCU.lib OemGuiU.lib WKClassesU.lib atl.lib strmiids.lib comctl32.lib Strmbase.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShowU.lib DeviceDetectU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCMT" /libpath:"..\..\..\DirectShow\lib" /libpath:"V:\project\Cipc\ReleaseUnicode"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=.\ReleaseUnicode\AudioUnit.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV\ /R /C /F /Y	xcopy $(TargetPath) c:\Security\ /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 EnuUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AudioUnit___Win32_EnuUnicode"
# PROP BASE Intermediate_Dir "AudioUnit___Win32_EnuUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EnuUnicode"
# PROP Intermediate_Dir "EnuUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\..\..\DirectShow\\" /I "..\..\..\DirectShow\include\\" /I "..\..\..\DirectShow\filters\baseclasses\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Strmbase.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShow.lib DeviceDetect.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCMT" /libpath:"..\..\..\DirectShow\lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 CIPCU.lib OemGuiU.lib WKClassesU.lib atl.lib strmiids.lib comctl32.lib Strmbase.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib CommonDirectShowU.lib DeviceDetectU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCMT" /libpath:"..\..\..\DirectShow\lib" /libpath:"V:\project\Cipc\ReleaseUnicode"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "AudioUnit - Win32 Release"
# Name "AudioUnit - Win32 Debug"
# Name "AudioUnit - Win32 Enu"
# Name "AudioUnit - Win32 DebugUnicode"
# Name "AudioUnit - Win32 ReleaseUnicode"
# Name "AudioUnit - Win32 EnuUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AudioUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioUnit.rc

!IF  "$(CFG)" == "AudioUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 ReleaseUnicode"

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AudioUnitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioUnitEnu.rc

!IF  "$(CFG)" == "AudioUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AudioUnit - Win32 EnuUnicode"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CipcInputAudioUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCMediaAudioUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AudioUnit.h
# End Source File
# Begin Source File

SOURCE=.\AudioUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\CipcInputAudioUnit.h
# End Source File
# Begin Source File

SOURCE=.\IPCMediaAudioUnit.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AudioUnit.ico
# End Source File
# Begin Source File

SOURCE=.\res\AudioUnit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\idc_paus.ico
# End Source File
# Begin Source File

SOURCE=.\res\idc_reco.ico
# End Source File
# Begin Source File

SOURCE=.\res\idc_stop.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_runi.ico
# End Source File
# Begin Source File

SOURCE=.\res\START.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\audiounitenu.lng
# End Source File
# Begin Source File

SOURCE=.\history.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
