# Microsoft Developer Studio Project File - Name="USBCamUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=USBCamUnit - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "USBCamUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "USBCamUnit.mak" CFG="USBCamUnit - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "USBCamUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "USBCamUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "USBCamUnit - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "USBCamUnit - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/USBCAM/USBCamUnit", HJICAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "USBCamUnit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\DirectShow\Filters\\" /I "..\..\..\DirectShow\include" /I "..\..\..\DirectShow\Filters\BaseClasses" /I "..\..\..\DirectShow\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Dxerr8.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib DeviceDetect.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "USBCamUnit - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\DirectShow\Filters\\" /I "..\..\..\DirectShow\include" /I "..\..\..\DirectShow\Filters\BaseClasses" /I "..\..\..\DirectShow\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CIPCDebug.lib OemGuiD.lib WKClassesDebug.lib atl.lib strmiids.lib comctl32.lib Dxerr8.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib DeviceDetectD.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "USBCamUnit - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "USBCamUnit___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "USBCamUnit___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "USBCamUnit___Win32_DebugUnicode"
# PROP Intermediate_Dir "USBCamUnit___Win32_DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\DirectShow\Filters\\" /I "..\..\..\DirectShow\include" /I "..\..\..\DirectShow\Filters\BaseClasses" /I "..\..\..\DirectShow\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\DirectShow\Filters\\" /I "..\..\..\DirectShow\include" /I "..\..\..\DirectShow\Filters\BaseClasses" /I "..\..\..\DirectShow\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPCDebug.lib OemGuiD.lib WKClassesDebug.lib atl.lib strmiids.lib comctl32.lib Dxerr8.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib DeviceDetectD.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CIPCDebug.lib OemGuiD.lib WKClassesDebug.lib atl.lib strmiids.lib comctl32.lib Dxerr8.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib DeviceDetectD.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "USBCamUnit - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "USBCamUnit___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "USBCamUnit___Win32_ReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "USBCamUnit___Win32_ReleaseUnicode"
# PROP Intermediate_Dir "USBCamUnit___Win32_ReleaseUnicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\DirectShow\Filters\\" /I "..\..\..\DirectShow\include" /I "..\..\..\DirectShow\Filters\BaseClasses" /I "..\..\..\DirectShow\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\DirectShow\Filters\\" /I "..\..\..\DirectShow\include" /I "..\..\..\DirectShow\Filters\BaseClasses" /I "..\..\..\DirectShow\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Dxerr8.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib DeviceDetect.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 CIPC.lib OemGui.lib WKClasses.lib atl.lib strmiids.lib comctl32.lib Dxerr8.lib Strmbasd.lib Winmm.lib ole32.lib oleaut32.lib uuid.lib DeviceDetect.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "USBCamUnit - Win32 Release"
# Name "USBCamUnit - Win32 Debug"
# Name "USBCamUnit - Win32 DebugUnicode"
# Name "USBCamUnit - Win32 ReleaseUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CipcInputUSBcamUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcInputUSBcamUnitMdAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcOutputUSBcamUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdconfigdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdconfigdlgbase.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdpoints.cpp
# End Source File
# Begin Source File

SOURCE=.\cmotiondetection.cpp
# End Source File
# Begin Source File

SOURCE=.\memcopy.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\USBCamUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\USBCamUnit.rc
# End Source File
# Begin Source File

SOURCE=.\USBCamUnitDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CipcInputUSBcamunit.h
# End Source File
# Begin Source File

SOURCE=.\CipcInputUSBcamUnitMdAlarm.h
# End Source File
# Begin Source File

SOURCE=.\CipcOutputUSBcamUnit.h
# End Source File
# Begin Source File

SOURCE=.\cmdconfigdlg.h
# End Source File
# Begin Source File

SOURCE=.\cmdconfigdlgbase.h
# End Source File
# Begin Source File

SOURCE=.\cmdpoints.h
# End Source File
# Begin Source File

SOURCE=.\cmotiondetection.h
# End Source File
# Begin Source File

SOURCE=.\memcopy.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\USBCamUnit.h
# End Source File
# Begin Source File

SOURCE=.\USBCamUnitDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\green.bmp
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

SOURCE=.\MDInlay.bmp
# End Source File
# Begin Source File

SOURCE=.\MDmask.bmp
# End Source File
# Begin Source File

SOURCE=.\red.bmp
# End Source File
# Begin Source File

SOURCE=.\res\START.ico
# End Source File
# Begin Source File

SOURCE=.\res\USBCamUnit.ico
# End Source File
# Begin Source File

SOURCE=.\res\USBCamUnit.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
