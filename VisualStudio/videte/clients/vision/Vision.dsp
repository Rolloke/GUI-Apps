# Microsoft Developer Studio Project File - Name="Vision" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Vision - Win32 Dts Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Vision.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Vision.mak" CFG="Vision - Win32 Dts Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Vision - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 Dts" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 DtsEnu" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 Dts Debug" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 DtsIta" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Clients/Vision", EOXAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Vision - Win32 Release"

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
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
TargetPath=.\Release\Vision.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy         $(TargetPath)         c:\security\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 oemguid.lib vimaged.lib cipcdebug.lib wkclassesdebug.lib skinsd.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Vision___Win32_Enu"
# PROP BASE Intermediate_Dir "Vision___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X /Fr
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib IMAGN32.lib ptdecoder.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Vision___Win32_Dts"
# PROP BASE Intermediate_Dir "Vision___Win32_Dts"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Dts"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib jpeglib.lib ptdecoder.lib h263dec.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Vision___Win32_DtsEnu"
# PROP BASE Intermediate_Dir "Vision___Win32_DtsEnu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsEnu"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib jpeglib.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 cipc.lib oemgui.lib vimage.lib wkclasses.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Vision___Win32_Dts_Debug"
# PROP BASE Intermediate_Dir "Vision___Win32_Dts_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Dts_Debug"
# PROP Intermediate_Dir "Dts_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oemguid.lib jpeglibd.lib cipcdebug.lib ptdecoderd.lib h263decd.lib wkclassesdebug.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 oemguid.lib vimaged.lib cipcdebug.lib wkclassesdebug.lib skinsd.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Vision___Win32_Ita"
# PROP BASE Intermediate_Dir "Vision___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib jpeglib.lib ptdecoder.lib h263dec.lib wkclasses.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 cipc.lib oemgui.lib jpeglib.lib ptdecoder.lib h263dec.lib wkclasses.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Vision___Win32_DtsIta"
# PROP BASE Intermediate_Dir "Vision___Win32_DtsIta"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DtsIta"
# PROP Intermediate_Dir "Dts"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DTS" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib jpeglib.lib wkclasses.lib h263dec.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 cipc.lib oemgui.lib jpeglib.lib wkclasses.lib h263dec.lib skins.lib version.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug

!ENDIF 

# Begin Target

# Name "Vision - Win32 Release"
# Name "Vision - Win32 Debug"
# Name "Vision - Win32 Enu"
# Name "Vision - Win32 Dts"
# Name "Vision - Win32 DtsEnu"
# Name "Vision - Win32 Dts Debug"
# Name "Vision - Win32 Ita"
# Name "Vision - Win32 DtsIta"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AudioInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioOutputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCControlAudioUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCDataBaseVision.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCInputVision.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCInputVisionCommData.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputVision.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputVisionDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCServerControlVision.cpp
# End Source File
# Begin Source File

SOURCE=.\CocoWindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConnectingDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\csbdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Custsite.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ExternalFrameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HTMLlinkProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\HTMLmapProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Idispimp.cpp
# End Source File
# Begin Source File

SOURCE=.\InputDeactivationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCControl.cpp
# End Source File
# Begin Source File

SOURCE=.\loadsavesequencerdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\mdconfigdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MicoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectView.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\picturelengthstatistic.cpp
# End Source File
# Begin Source File

SOURCE=.\ptwindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReferenceWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\Servers.cpp
# End Source File
# Begin Source File

SOURCE=.\SmallToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SmallWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SNConfigDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TextWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\videosndialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Vision.cpp
# End Source File
# Begin Source File

SOURCE=.\Vision.rc

!IF  "$(CFG)" == "Vision - Win32 Release"

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VisionCoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\VisionDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Visionenu.rc

!IF  "$(CFG)" == "Vision - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VisionIta.rc

!IF  "$(CFG)" == "Vision - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

# PROP Intermediate_Dir "Release"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x410

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VisionView.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AudioInputDlg.h
# End Source File
# Begin Source File

SOURCE=.\AudioOutputDlg.h
# End Source File
# Begin Source File

SOURCE=.\AudioPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\CIPCControlAudioUnit.h
# End Source File
# Begin Source File

SOURCE=.\CIPCDataBaseVision.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputVision.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputVisionCommData.h
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputVision.h
# End Source File
# Begin Source File

SOURCE=.\CIPCOutputVisionDecoder.h
# End Source File
# Begin Source File

SOURCE=.\CIPCServerControlVision.h
# End Source File
# Begin Source File

SOURCE=.\CocoWindow.h
# End Source File
# Begin Source File

SOURCE=.\ConnectingDialog.h
# End Source File
# Begin Source File

SOURCE=.\csbdialog.h
# End Source File
# Begin Source File

SOURCE=.\Custsite.h
# End Source File
# Begin Source File

SOURCE=.\DisplayWindow.h
# End Source File
# Begin Source File

SOURCE=.\ExternalFrameDlg.h
# End Source File
# Begin Source File

SOURCE=.\HTMLlinkProperties.h
# End Source File
# Begin Source File

SOURCE=.\HTMLmapProperties.h
# End Source File
# Begin Source File

SOURCE=.\HtmlWindow.h
# End Source File
# Begin Source File

SOURCE=.\Idispimp.h
# End Source File
# Begin Source File

SOURCE=.\images.h
# End Source File
# Begin Source File

SOURCE=.\InputDeactivationDialog.h
# End Source File
# Begin Source File

SOURCE=.\IPCControl.h
# End Source File
# Begin Source File

SOURCE=.\loadsavesequencerdialog.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\mdconfigdlg.h
# End Source File
# Begin Source File

SOURCE=.\MicoWindow.h
# End Source File
# Begin Source File

SOURCE=.\ObjectView.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\picturelengthstatistic.h
# End Source File
# Begin Source File

SOURCE=.\ptwindow.h
# End Source File
# Begin Source File

SOURCE=.\ReferenceWindow.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\server.inl
# End Source File
# Begin Source File

SOURCE=.\Servers.h
# End Source File
# Begin Source File

SOURCE=.\SmallToolBar.h
# End Source File
# Begin Source File

SOURCE=.\SmallWindow.h
# End Source File
# Begin Source File

SOURCE=.\SNConfigDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TextWindow.h
# End Source File
# Begin Source File

SOURCE=.\videosndialog.h
# End Source File
# Begin Source File

SOURCE=.\Vision.h
# End Source File
# Begin Source File

SOURCE=.\VisionCoolBar.h
# End Source File
# Begin Source File

SOURCE=.\VisionDoc.h
# End Source File
# Begin Source File

SOURCE=.\VisionView.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\alarm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\back.gif
# End Source File
# Begin Source File

SOURCE=.\res\center.ico
# End Source File
# Begin Source File

SOURCE=.\res\cold.bmp
# End Source File
# Begin Source File

SOURCE=.\res\display.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down.ico
# End Source File
# Begin Source File

SOURCE=.\res\glass.cur
# End Source File
# Begin Source File

SOURCE=.\res\hot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\html.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HTMLimg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\images.bmp
# End Source File
# Begin Source File

SOURCE=.\res\left.ico
# End Source File
# Begin Source File

SOURCE=.\res\nostatement.htm
# End Source File
# Begin Source File

SOURCE=.\res\panleft.cur
# End Source File
# Begin Source File

SOURCE=.\res\panright.cur
# End Source File
# Begin Source File

SOURCE=.\res\right.ico
# End Source File
# Begin Source File

SOURCE=.\res\small_st.bmp
# End Source File
# Begin Source File

SOURCE=.\res\smalstop.ico
# End Source File
# Begin Source File

SOURCE=.\res\states.bmp
# End Source File
# Begin Source File

SOURCE=.\res\text.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tiltdown.cur
# End Source File
# Begin Source File

SOURCE=.\res\tiltup.cur
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbarm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\turn180.ico
# End Source File
# Begin Source File

SOURCE=.\res\up.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up.ico
# End Source File
# Begin Source File

SOURCE=.\res\Vision.rc2
# End Source File
# Begin Source File

SOURCE=.\res\VisionDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\VisionIST.ico
# End Source File
# Begin Source File

SOURCE=.\res\zoomin.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoomout.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoomout1.cur
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter "txt;lng;csv"
# Begin Source File

SOURCE=.\Deu_Enu.csv

!IF  "$(CFG)" == "Vision - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Enu_Ita.csv

!IF  "$(CFG)" == "Vision - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\History.txt

!IF  "$(CFG)" == "Vision - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Visionenu.lng

!IF  "$(CFG)" == "Vision - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\visionita.lng

!IF  "$(CFG)" == "Vision - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Enu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsEnu"

!ELSEIF  "$(CFG)" == "Vision - Win32 Dts Debug"

!ELSEIF  "$(CFG)" == "Vision - Win32 Ita"

!ELSEIF  "$(CFG)" == "Vision - Win32 DtsIta"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
# Section Vision : {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}
# 	2:5:Class:CWebBrowser2
# 	2:10:HeaderFile:webbrowser2.h
# 	2:8:ImplFile:webbrowser2.cpp
# End Section
# Section Vision : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowser2.h
# 	2:16:DefaultSinkClass:CWebBrowser2
# End Section
