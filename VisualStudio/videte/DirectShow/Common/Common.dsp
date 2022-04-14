# Microsoft Developer Studio Project File - Name="Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Common - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Common - Win32 DebugUnicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Common - Win32 ReleaseUnicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/DirectShow/Common", BCICAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\Filters\BaseClasses" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 amstrmid.lib dmoguids.lib dsound.lib strmbase.lib dinput.lib dmoguids.lib dplayx.lib dsetup.lib DxErr8.lib dxguid.lib dxtrans.lib ksproxy.lib ksuser.lib msdmo.lib quartz.lib strmiids.lib Winmm.lib wmstub.lib /nologo /subsystem:windows /dll /map /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\lib/CommonDirectShow.dll" /libpath:"..\lib" /libpath:"..\Filters\BaseClasses\Debug"
# Begin Special Build Tool
TargetPath=\Project\DirectShow\lib\CommonDirectShow.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy Files
PostBuild_Cmds=v:\bin\tools\syscopy.exe $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /GX /Zi /Od /I "..\Filters\BaseClasses" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 amstrmid.lib dmoguids.lib dsound.lib strmbasd.lib dinput.lib dmoguids.lib dplayx.lib dsetup.lib DxErr8.lib dxguid.lib dxtrans.lib ksproxy.lib ksuser.lib msdmo.lib quartz.lib strmiids.lib Winmm.lib wmstub.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"Debug/CommonD.pdb" /debug /machine:I386 /out:"..\lib\CommonDirectShowD.dll" /pdbtype:sept /libpath:"..\lib" /libpath:"..\Filters\BaseClasses\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Project\DirectShow\lib\CommonDirectShowD.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy Files
PostBuild_Cmds=v:\bin\tools\syscopy.exe $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Common - Win32 DebugUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Common___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "Common___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Filters\BaseClasses" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /GX /Zi /Od /I "..\Filters\BaseClasses" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 amstrmid.lib dmoguids.lib dsound.lib strmbasd.lib dinput.lib dmoguids.lib dplayx.lib dsetup.lib DxErr8.lib dxguid.lib dxtrans.lib ksproxy.lib ksuser.lib msdmo.lib quartz.lib strmiids.lib Winmm.lib wmstub.lib /nologo /subsystem:windows /dll /pdb:"Debug/CommonD.pdb" /debug /machine:I386 /out:"..\lib\CommonDirectShowD.dll" /pdbtype:sept /libpath:"..\lib" /libpath:"..\Filters\BaseClasses\Debug"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 amstrmid.lib dmoguids.lib dsound.lib strmbasd.lib dinput.lib dmoguids.lib dplayx.lib dsetup.lib DxErr8.lib dxguid.lib dxtrans.lib ksproxy.lib ksuser.lib msdmo.lib quartz.lib strmiids.lib Winmm.lib wmstub.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"DebugUnicode/CommonD.pdb" /debug /machine:I386 /out:"..\lib\CommonDirectShowDU.dll" /pdbtype:sept /libpath:"..\lib" /libpath:"..\Filters\BaseClasses\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Project\DirectShow\lib\CommonDirectShowDU.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy Files
PostBuild_Cmds=v:\bin\tools\syscopy.exe $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Common - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Common___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "Common___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\Filters\BaseClasses" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\Filters\BaseClasses" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 amstrmid.lib dmoguids.lib dsound.lib strmbase.lib dinput.lib dmoguids.lib dplayx.lib dsetup.lib DxErr8.lib dxguid.lib dxtrans.lib ksproxy.lib ksuser.lib msdmo.lib quartz.lib strmiids.lib Winmm.lib wmstub.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\lib/CommonDirectShow.dll" /libpath:"..\lib" /libpath:"..\Filters\BaseClasses\Debug"
# ADD LINK32 amstrmid.lib dmoguids.lib dsound.lib strmbase.lib dinput.lib dmoguids.lib dplayx.lib dsetup.lib DxErr8.lib dxguid.lib dxtrans.lib ksproxy.lib ksuser.lib msdmo.lib quartz.lib strmiids.lib Winmm.lib wmstub.lib /nologo /subsystem:windows /dll /map /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\lib/CommonDirectShowU.dll" /libpath:"..\lib" /libpath:"..\Filters\BaseClasses\Debug"
# Begin Special Build Tool
TargetPath=\Project\DirectShow\lib\CommonDirectShowU.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Copy Files
PostBuild_Cmds=v:\bin\tools\syscopy.exe $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Common - Win32 Release"
# Name "Common - Win32 Debug"
# Name "Common - Win32 DebugUnicode"
# Name "Common - Win32 ReleaseUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Common.cpp
# End Source File
# Begin Source File

SOURCE=.\Common.def

!IF  "$(CFG)" == "Common - Win32 Release"

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common.rc
# End Source File
# Begin Source File

SOURCE=.\CommonD.def

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

!ELSEIF  "$(CFG)" == "Common - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CommonDU.def

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "Common - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CommonU.def

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Common - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dshowutil.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyProvider.cpp
# End Source File
# Begin Source File

SOURCE=.\mfcdmoutil.cpp
# End Source File
# Begin Source File

SOURCE=.\mfcutil.cpp
# End Source File
# Begin Source File

SOURCE=.\namedguid.cpp
# End Source File
# Begin Source File

SOURCE=.\seekutil.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dshowasf.h
# End Source File
# Begin Source File

SOURCE=.\dshowutil.h
# End Source File
# Begin Source File

SOURCE=.\KeyProvider.h
# End Source File
# Begin Source File

SOURCE=.\mfcdmoutil.h
# End Source File
# Begin Source File

SOURCE=.\mfcutil.h
# End Source File
# Begin Source File

SOURCE=.\namedguid.h
# End Source File
# Begin Source File

SOURCE=.\nserror.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\seekutil.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\wmsbuffer.h
# End Source File
# Begin Source File

SOURCE=.\wmsdk.h
# End Source File
# Begin Source File

SOURCE=.\wmsdkidl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Common.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\history.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
