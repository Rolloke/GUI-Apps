# Microsoft Developer Studio Project File - Name="SocketUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SocketUnit - Win32 DtsDebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SocketUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SocketUnit.mak" CFG="SocketUnit - Win32 DtsDebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SocketUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 Dts" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 DtsDebug" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 DtsDebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "SocketUnit - Win32 DtsUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/SocketUnit", AQJAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SocketUnit - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib oemgui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 CipcDebug.lib OemGuid.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SocketUnit___Win32_Enu"
# PROP BASE Intermediate_Dir "SocketUnit___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Enu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 Cipc.lib OemGui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Dts"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SocketUnit___Win32_Dts"
# PROP BASE Intermediate_Dir "SocketUnit___Win32_Dts"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Dts"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_DTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Cipc.lib OemGui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 Cipc.lib OemGui.lib Netcomm.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SocketUnit___Win32_DtsDebug"
# PROP BASE Intermediate_Dir "SocketUnit___Win32_DtsDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DtsDebug"
# PROP Intermediate_Dir "DtsDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CipcDebug.lib OemGuid.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386
# ADD LINK32 CipcDebug.lib OemGuid.lib NetcommDebug.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SocketUnit___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "SocketUnit___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CipcDebug.lib OemGuid.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386
# ADD LINK32 CipcDebugU.lib OemGuidU.lib wkclassesDebugU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /map /debug /machine:I386 /libpath:"v:\project\cipc\debugunicode"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SocketUnit___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "SocketUnit___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /fo"ReleaseUnicode/SocketUnit.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 cipcU.lib oemguiU.lib wkclassesU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SocketUnit___Win32_DtsDebugUnicode"
# PROP BASE Intermediate_Dir "SocketUnit___Win32_DtsDebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DtsDebugUnicode"
# PROP Intermediate_Dir "DtsDebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CipcDebugU.lib OemGuidU.lib wkclassesDebugU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /map /debug /machine:I386 /libpath:"v:\project\cipc\debugunicode"
# ADD LINK32 CipcDebugU.lib OemGuidU.lib wkclassesDebugU.lib NetcommDebug.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /map /debug /machine:I386

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SocketUnit___Win32_DtsUnicode"
# PROP BASE Intermediate_Dir "SocketUnit___Win32_DtsUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsUnicode"
# PROP Intermediate_Dir "DtsUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /D "_DTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /fo"ReleaseUnicode/SocketUnit.res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /fo"ReleaseUnicode/SocketUnit.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipcU.lib oemguiU.lib wkclassesU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 cipcU.lib oemguiU.lib wkclassesU.lib Netcomm.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"V:\project\Cipc\ReleaseUnicode"
# SUBTRACT LINK32 /incremental:yes

!ENDIF 

# Begin Target

# Name "SocketUnit - Win32 Release"
# Name "SocketUnit - Win32 Debug"
# Name "SocketUnit - Win32 Enu"
# Name "SocketUnit - Win32 Dts"
# Name "SocketUnit - Win32 DtsDebug"
# Name "SocketUnit - Win32 DebugUnicode"
# Name "SocketUnit - Win32 ReleaseUnicode"
# Name "SocketUnit - Win32 DtsDebugUnicode"
# Name "SocketUnit - Win32 DtsUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\CIPCPipeSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectionThread.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\DataSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\IPBook.cpp

!IF  "$(CFG)" == "SocketUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Dts"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebug"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebugUnicode"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IPInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerControlSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketUnitDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketUnitView.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CIPCPipeSocket.h
# End Source File
# Begin Source File

SOURCE=.\ConnectionThread.h
# End Source File
# Begin Source File

SOURCE=.\ControlSocket.h
# End Source File
# Begin Source File

SOURCE=.\DataSocket.h
# End Source File
# Begin Source File

SOURCE=.\IPBook.h

!IF  "$(CFG)" == "SocketUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Dts"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebug"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebugUnicode"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IPInterface.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ServerControlSocket.h
# End Source File
# Begin Source File

SOURCE=.\SocketCmdRecord.h
# End Source File
# Begin Source File

SOURCE=.\SocketUnit.h
# End Source File
# Begin Source File

SOURCE=.\SocketUnitDoc.h
# End Source File
# Begin Source File

SOURCE=.\SocketUnitView.h
# End Source File
# Begin Source File

SOURCE=.\SocketUtil.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\SocketUnit.ico
# End Source File
# Begin Source File

SOURCE=.\SocketUnit.rc

!IF  "$(CFG)" == "SocketUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Dts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebug"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebugUnicode"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\SocketUnit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SocketUnitDoc.ico
# End Source File
# Begin Source File

SOURCE=.\SocketUnitenu.rc

!IF  "$(CFG)" == "SocketUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 Dts"

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsDebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SocketUnit - Win32 DtsUnicode"

!ENDIF 

# End Source File
# End Group
# Begin Group "Doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\SocketUnitMainPage.txt
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SocketUnitenu.lng
# End Source File
# End Group
# End Target
# End Project
