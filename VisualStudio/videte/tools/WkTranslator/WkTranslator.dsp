# Microsoft Developer Studio Project File - Name="WkTranslator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WkTranslator - Win32 ChsDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WkTranslator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WkTranslator.mak" CFG="WkTranslator - Win32 ChsDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WkTranslator - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WkTranslator - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "WkTranslator - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "WkTranslator - Win32 Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "WkTranslator - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "WkTranslator - Win32 ChsDebug" (based on "Win32 (x86) Application")
!MESSAGE "WkTranslator - Win32 EnuUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Tools/WkTranslator", KXNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /U "_DEBUG" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /map /machine:I386 /out:".\Release/Translator.exe"
# Begin Special Build Tool
TargetPath=.\Release\Translator.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_v:\Bin\Tools\ 
PostBuild_Cmds=copy $(TargetPath) V:\Bin\Tools\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /U "NDEBUG" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:".\Debug/Translator.exe"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WkTranslator___Win32_Enu"
# PROP BASE Intermediate_Dir "WkTranslator___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"Enu/Translator.exe"
# Begin Special Build Tool
TargetPath=.\Enu\Translator.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_v:\Bin\Tools\TranslatorEnu.exe
PostBuild_Cmds=copy $(TargetPath) V:\Bin\Tools\TranslatorEnu.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Unicode"
# PROP BASE Intermediate_Dir "Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /U "NDEBUG" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UNICODE" /U "NDEBUG" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:".\Debug_Unicode/Translator.exe"
# ADD LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /debug /machine:I386 /out:".\Debug_Unicode/Translator.exe"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUnicode"
# PROP BASE Intermediate_Dir "ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /U "_DEBUG" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UNICODE" /U "_DEBUG" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /map /machine:I386 /out:".\Release/Translator.exe"
# ADD LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /out:".\ReleaseUnicode/TranslatorU.exe"
# Begin Special Build Tool
TargetPath=.\ReleaseUnicode\TranslatorU.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_v:\Bin\Tools\ 
PostBuild_Cmds=copy $(TargetPath) V:\Bin\Tools\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WkTranslator___Win32_ChsDebug"
# PROP BASE Intermediate_Dir "WkTranslator___Win32_ChsDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ChsDebug"
# PROP Intermediate_Dir "ChsDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /U "NDEBUG" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UNICODE" /U "NDEBUG" /FR"Debug_Unicode/" /Fp"Debug_Unicode/WkTranslator.pch" /Yu"stdafx.h" /Fo"Debug_Unicode/" /Fd"Debug_Unicode/" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:".\Debug/Translator.exe"
# ADD LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /debug /machine:I386 /out:".\ChsDebug/TranslatorChs.exe"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WkTranslator___Win32_EnuUnicode"
# PROP BASE Intermediate_Dir "WkTranslator___Win32_EnuUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EnuUnicode"
# PROP Intermediate_Dir "EnuUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UNICODE" /U "_DEBUG" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UNICODE" /U "_DEBUG" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /out:".\ReleaseUnicode/TranslatorU.exe"
# ADD LINK32 version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /out:".\EnuUnicode/TranslatorEnuU.exe"
# Begin Special Build Tool
TargetPath=.\EnuUnicode\TranslatorEnuU.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Copy_to_v:\Bin\Tools\ 
PostBuild_Cmds=copy $(TargetPath) V:\Bin\Tools\*.*
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "WkTranslator - Win32 Release"
# Name "WkTranslator - Win32 Debug"
# Name "WkTranslator - Win32 Enu"
# Name "WkTranslator - Win32 Debug Unicode"
# Name "WkTranslator - Win32 ReleaseUnicode"
# Name "WkTranslator - Win32 ChsDebug"
# Name "WkTranslator - Win32 EnuUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\CommandLineInfoWkTranslator.cpp
# End Source File
# Begin Source File

SOURCE=.\EditReplaceDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FileOperations.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GlossaryInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SelectCodePageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TranslateDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\translatestring.cpp
# End Source File
# Begin Source File

SOURCE=.\TSArrayArray.cpp
# End Source File
# Begin Source File

SOURCE=.\WkTranslator.cpp
# End Source File
# Begin Source File

SOURCE=.\WkTranslator.rc

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wktranslatorChs.lng

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WkTranslatorChs.rc

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WkTranslatorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\WkTranslatorenu.lng

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WkTranslatorEnu.rc

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WkTranslatorView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CommandLineInfoWkTranslator.h
# End Source File
# Begin Source File

SOURCE=.\EditReplaceDialog.h
# End Source File
# Begin Source File

SOURCE=.\FileOperations.h
# End Source File
# Begin Source File

SOURCE=.\FindDialog.h
# End Source File
# Begin Source File

SOURCE=.\GlossaryInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SelectCodePageDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TranslateDialog.h
# End Source File
# Begin Source File

SOURCE=.\translatestring.h
# End Source File
# Begin Source File

SOURCE=.\TSArrayArray.h
# End Source File
# Begin Source File

SOURCE=.\WkTranslator.h
# End Source File
# Begin Source File

SOURCE=.\WkTranslatorDoc.h
# End Source File
# Begin Source File

SOURCE=.\WkTranslatorView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WkTranslator.ico
# End Source File
# Begin Source File

SOURCE=.\res\WkTranslator.rc2
# End Source File
# Begin Source File

SOURCE=.\res\WkTranslatorDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\history.txt

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 ChsDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 EnuUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
