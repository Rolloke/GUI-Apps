# Microsoft Developer Studio Project File - Name="ISDNUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ISDNUnit - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ISDNUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ISDNUnit.mak" CFG="ISDNUnit - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ISDNUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 PTUnitDebug" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 PTUnitRelease" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 PTEnu" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 Dts" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 Dts Debug" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 DtsUnicode" (based on "Win32 (x86) Application")
!MESSAGE "ISDNUnit - Win32 DtsDebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Capi/ISDNUnit", YJHAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ISDNUnit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /U "_DEBUG" /U "RUN_AS_PTUNIT" /U "NO_PERMISSION_CHECK" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Cipc.lib OemGui.lib DeviceDetect.Lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /U "NDEBUG" /U "RUN_AS_PTUNIT" /U "NO_PERMISSION_CHECK" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 CipcDebug.lib OemGuiD.lib DeviceDetectD.Lib /nologo /version:3.0 /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /profile /incremental:no

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ISDNUnit"
# PROP BASE Intermediate_Dir "ISDNUnit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PTUnitDebug"
# PROP Intermediate_Dir "PTUnitDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /Gf /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /U "NDEBUG" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /Gf /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "RUN_AS_PTUNIT" /U "NDEBUG" /U "NO_PERMISSION_CHECK" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"PTUnitDebug/PTUnitUnit.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /version:3.0 /subsystem:windows /incremental:no /debug /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 CipcDebug.lib OemGuiD.lib DeviceDetectD.Lib /nologo /version:3.0 /subsystem:windows /incremental:no /debug /machine:I386 /out:"PTUnitDebug/PTUnit.exe"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitRelease"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ISDNUnit"
# PROP BASE Intermediate_Dir "ISDNUnit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PTUnitRelease"
# PROP Intermediate_Dir "PTUnitRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /U "_DEBUG" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "RUN_AS_PTUNIT" /U "_DEBUG" /U "NO_PERMISSION_CHECK" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=1
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 Cipc.lib OemGui.lib DeviceDetect.Lib /nologo /subsystem:windows /map /machine:I386 /out:"PTUnitRelease/PTUnit.exe"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Enu"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /I ALL_WARNINGS=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D RUN_AS_PTUNIT=0 /U "_DEBUG" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /U "_DEBUG" /U "RUN_AS_PTUNIT" /U "NO_PERMISSION_CHECK" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 Cipc.lib OemGui.lib DeviceDetect.Lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTEnu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PTEnu"
# PROP BASE Intermediate_Dir "PTUnitRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PTEnu"
# PROP Intermediate_Dir "PTUnitRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /I ALL_WARNINGS=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D RUN_AS_PTUNIT=1 /U "_DEBUG" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "RUN_AS_PTUNIT" /U "_DEBUG" /U "NO_PERMISSION_CHECK" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=1
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=1
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib /nologo /subsystem:windows /map /machine:I386 /out:"PTUnitRelease/PTUnit.exe"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 Cipc.lib OemGui.lib DeviceDetect.Lib /nologo /subsystem:windows /map /machine:I386 /out:"PTEnu/PTUnit.exe"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ISDNUnit___Win32_Dts"
# PROP BASE Intermediate_Dir "ISDNUnit___Win32_Dts"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Dts"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /I ALL_WARNINGS=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D RUN_AS_PTUNIT=0 /U "_DEBUG" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_PERMISSION_CHECK" /D "_DTS" /U "_DEBUG" /U "RUN_AS_PTUNIT" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 Cipc.lib OemGui.lib DeviceDetect.Lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ISDNUnit___Win32_Dts_Debug"
# PROP BASE Intermediate_Dir "ISDNUnit___Win32_Dts_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsDebug"
# PROP Intermediate_Dir "DtsDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_PERMISSION_CHECK" /D "_DTS" /U "_DEBUG" /U "RUN_AS_PTUNIT" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_PERMISSION_CHECK" /D "_DTS" /U "NDEBUG" /U "RUN_AS_PTUNIT" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 CipcDebug.lib OemGuiD.lib DeviceDetectD.Lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ISDNUnit___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "ISDNUnit___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /U "NDEBUG" /U "RUN_AS_PTUNIT" /U "NO_PERMISSION_CHECK" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /U "NDEBUG" /U "RUN_AS_PTUNIT" /U "NO_PERMISSION_CHECK" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CipcDebug.lib OemGuiD.lib DeviceDetectD.Lib /nologo /version:3.0 /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /profile /incremental:no
# ADD LINK32 CipcDebugU.lib OemGuiDU.lib DeviceDetectDU.Lib wkclassesDebugU.lib /nologo /version:3.0 /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /libpath:"V:\project\Cipc\DebugUnicode"
# SUBTRACT LINK32 /profile /incremental:no

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ISDNUnit___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "ISDNUnit___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /U "_DEBUG" /U "RUN_AS_PTUNIT" /U "NO_PERMISSION_CHECK" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /U "_DEBUG" /U "RUN_AS_PTUNIT" /U "NO_PERMISSION_CHECK" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib DeviceDetect.Lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 CipcU.lib OemGuiU.lib DeviceDetectU.Lib wkclassesU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"mfcs42.lib" /libpath:"V:\project\Cipc\ReleaseUnicode"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ISDNUnit___Win32_DtsUnicode"
# PROP BASE Intermediate_Dir "ISDNUnit___Win32_DtsUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsUnicode"
# PROP Intermediate_Dir "DtsUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_PERMISSION_CHECK" /D "_DTS" /U "_DEBUG" /U "RUN_AS_PTUNIT" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_PERMISSION_CHECK" /D "_DTS" /D "_UNICODE" /U "_DEBUG" /U "RUN_AS_PTUNIT" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib DeviceDetect.Lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 CipcU.lib OemGuiU.lib DeviceDetectU.Lib wkclassesU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsDebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ISDNUnit___Win32_DtsDebugUnicode"
# PROP BASE Intermediate_Dir "ISDNUnit___Win32_DtsDebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsDebugUnicode"
# PROP Intermediate_Dir "DtsDebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_PERMISSION_CHECK" /D "_DTS" /U "NDEBUG" /U "RUN_AS_PTUNIT" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NO_PERMISSION_CHECK" /D "_DTS" /D "_UNICODE" /U "NDEBUG" /U "RUN_AS_PTUNIT" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d RUN_AS_PTUNIT=0
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CipcDebug.lib OemGuiD.lib DeviceDetectD.Lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /profile /map
# ADD LINK32 CipcDebugU.lib OemGuiDU.lib DeviceDetectDU.Lib wkclassesDebugU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /libpath:"V:\project\Cipc\DebugUnicode"
# SUBTRACT LINK32 /profile /map

!ENDIF 

# Begin Target

# Name "ISDNUnit - Win32 Release"
# Name "ISDNUnit - Win32 Debug"
# Name "ISDNUnit - Win32 PTUnitDebug"
# Name "ISDNUnit - Win32 PTUnitRelease"
# Name "ISDNUnit - Win32 Enu"
# Name "ISDNUnit - Win32 PTEnu"
# Name "ISDNUnit - Win32 Dts"
# Name "ISDNUnit - Win32 Dts Debug"
# Name "ISDNUnit - Win32 DebugUnicode"
# Name "ISDNUnit - Win32 ReleaseUnicode"
# Name "ISDNUnit - Win32 DtsUnicode"
# Name "ISDNUnit - Win32 DtsDebugUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "PT Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CipcPipeInputPT.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcPipeOutputPT.cpp
# End Source File
# Begin Source File

SOURCE=.\ISDNConnectionPT.cpp
# End Source File
# Begin Source File

SOURCE=.\PTAdvancedSettingsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PTBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ptcmd.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Aboutbox.cpp
# End Source File
# Begin Source File

SOURCE=.\CapiQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\CapiQueueRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\CapiThread.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCPipeIsdn.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCPipeIST.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcServerControlIsdn.cpp
# End Source File
# Begin Source File

SOURCE=.\HiddenCapiWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\IsdnConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\ISDNConnectionMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\ISDNUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\ISDNUnitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ISDNUnitWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\KnockKnockBox.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "PT Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CipcPipeInputPT.h
# End Source File
# Begin Source File

SOURCE=.\CipcPipeOutputPT.h
# End Source File
# Begin Source File

SOURCE=.\pt.h
# End Source File
# Begin Source File

SOURCE=.\PTAdvancedSettingsDialog.h
# End Source File
# Begin Source File

SOURCE=.\PTAlarm.h
# End Source File
# Begin Source File

SOURCE=.\PTBox.h
# End Source File
# Begin Source File

SOURCE=.\PTCamera.h
# End Source File
# Begin Source File

SOURCE=.\RawDataArray.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AboutBox.h
# End Source File
# Begin Source File

SOURCE=.\CapiQueue.h
# End Source File
# Begin Source File

SOURCE=.\CapiQueueRecord.h
# End Source File
# Begin Source File

SOURCE=.\CapiThread.h
# End Source File
# Begin Source File

SOURCE=.\CIPCPipeIsdn.h
# End Source File
# Begin Source File

SOURCE=.\CIPCPipeIST.h
# End Source File
# Begin Source File

SOURCE=.\CipcServerControlIsdn.h
# End Source File
# Begin Source File

SOURCE=.\CIPCType.h
# End Source File
# Begin Source File

SOURCE=.\HiddenCapiWnd.h
# End Source File
# Begin Source File

SOURCE=.\IsdnConnection.h
# End Source File
# Begin Source File

SOURCE=.\ISDNUnit.h
# End Source File
# Begin Source File

SOURCE=.\ISDNUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\IsdnUnitWnd.h
# End Source File
# Begin Source File

SOURCE=.\KnockKnockBox.h
# End Source File
# Begin Source File

SOURCE=.\Net_Messages.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ISDNUnit.ico
# End Source File
# Begin Source File

SOURCE=.\ISDNUnit.rc

!IF  "$(CFG)" == "ISDNUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitDebug"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitRelease"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 ReleaseUnicode"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsDebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\ISDNUnit.rc2
# End Source File
# Begin Source File

SOURCE=.\ISDNUnitEnu.rc

!IF  "$(CFG)" == "ISDNUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTEnu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsUnicode"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsDebugUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\NoDevice.ico
# End Source File
# Begin Source File

SOURCE=.\res\pticon.ico
# End Source File
# Begin Source File

SOURCE=.\res\ptnodev.ico
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\capi4\Release\Capi4_32.lib

!IF  "$(CFG)" == "ISDNUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitRelease"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTEnu"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsDebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\capi4\Debug\Capi4_32_Debug.lib

!IF  "$(CFG)" == "ISDNUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitDebug"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTEnu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts Debug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsDebugUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\capi4\DebugUnicode\Capi4_32_DebugU.lib

!IF  "$(CFG)" == "ISDNUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsDebugUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\capi4\ReleaseUnicode\capi4_32U.lib

!IF  "$(CFG)" == "ISDNUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTUnitRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 PTEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 Dts Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Intermediate_Dir "ReleaseUnicode"

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsUnicode"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ISDNUnit - Win32 DtsDebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\ISDNMainPage.txt
# End Source File
# Begin Source File

SOURCE=.\isdnunit.htm
# End Source File
# Begin Source File

SOURCE=.\protokol.txt
# End Source File
# Begin Source File

SOURCE=.\ptdoc.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\ISDNUnitenu.lng
# End Source File
# End Target
# End Project
