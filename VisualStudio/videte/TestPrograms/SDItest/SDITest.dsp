# Microsoft Developer Studio Project File - Name="SDITest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SDITest - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SDITest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SDITest.mak" CFG="SDITest - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SDITest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SDITest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Tools/TstTools/SDITest", UKZAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SDITest - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib oemgui.lib wkclasses.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "SDITest - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cipcdebug.lib oemguid.lib wkclassesdebug.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SDITest - Win32 Release"
# Name "SDITest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ascii.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\comconfigurationdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ComDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DocAscom.cpp
# End Source File
# Begin Source File

SOURCE=.\DocEngelMultipass.cpp
# End Source File
# Begin Source File

SOURCE=.\DocFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DocGermanParcel.cpp
# End Source File
# Begin Source File

SOURCE=.\DocIbm.cpp
# End Source File
# Begin Source File

SOURCE=.\DocKebaPasador.cpp
# End Source File
# Begin Source File

SOURCE=.\DocKebaPc2000.cpp
# End Source File
# Begin Source File

SOURCE=.\DocLandauDtp.cpp
# End Source File
# Begin Source File

SOURCE=.\DocNcr_1_1.cpp
# End Source File
# Begin Source File

SOURCE=.\DocNcr_1_2.cpp
# End Source File
# Begin Source File

SOURCE=.\DocSniCom.cpp
# End Source File
# Begin Source File

SOURCE=.\DocSniDos.cpp
# End Source File
# Begin Source File

SOURCE=.\DocSniIbmEmulation.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIMultiDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIRS232.cpp
# End Source File
# Begin Source File

SOURCE=.\SDITest.cpp
# End Source File
# Begin Source File

SOURCE=.\SDITest.rc
# End Source File
# Begin Source File

SOURCE=.\SDITestCommandLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SDITestDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SDITestView.cpp
# End Source File
# Begin Source File

SOURCE=.\StandardEntriesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ViewAscom.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewEngelMultipass.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewGermanParcel.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewIbm.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewKebaPasador.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewKebaPc2000.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewLandauDtp.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewNcr_1_1.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewNcr_1_2.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewSniCom.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewSniDos.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewSniIbmEmulation.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=".\!ToDo.h"
# End Source File
# Begin Source File

SOURCE=.\ascii.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\comconfigurationdialog.h
# End Source File
# Begin Source File

SOURCE=.\ComDlg.h
# End Source File
# Begin Source File

SOURCE=.\DocAscom.h
# End Source File
# Begin Source File

SOURCE=.\DocEngelMultipass.h
# End Source File
# Begin Source File

SOURCE=.\DocFile.h
# End Source File
# Begin Source File

SOURCE=.\DocGermanParcel.h
# End Source File
# Begin Source File

SOURCE=.\DocIbm.h
# End Source File
# Begin Source File

SOURCE=.\DocKebaPasador.h
# End Source File
# Begin Source File

SOURCE=.\DocKebaPc2000.h
# End Source File
# Begin Source File

SOURCE=.\DocLandauDtp.h
# End Source File
# Begin Source File

SOURCE=.\DocNcr_1_1.h
# End Source File
# Begin Source File

SOURCE=.\DocNcr_1_2.h
# End Source File
# Begin Source File

SOURCE=.\DocSniCom.h
# End Source File
# Begin Source File

SOURCE=.\DocSniDos.h
# End Source File
# Begin Source File

SOURCE=.\DocSniIbmEmulation.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SDIMultiDocTemplate.h
# End Source File
# Begin Source File

SOURCE=.\SDIRS232.h
# End Source File
# Begin Source File

SOURCE=.\SDITest.h
# End Source File
# Begin Source File

SOURCE=.\SDITestCommandLineInfo.h
# End Source File
# Begin Source File

SOURCE=.\SDITestDoc.h
# End Source File
# Begin Source File

SOURCE=.\SDITestView.h
# End Source File
# Begin Source File

SOURCE=.\StandardEntriesDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ViewAscom.h
# End Source File
# Begin Source File

SOURCE=.\ViewEngelMultipass.h
# End Source File
# Begin Source File

SOURCE=.\ViewFile.h
# End Source File
# Begin Source File

SOURCE=.\ViewGermanParcel.h
# End Source File
# Begin Source File

SOURCE=.\ViewIbm.h
# End Source File
# Begin Source File

SOURCE=.\ViewKebaPasador.h
# End Source File
# Begin Source File

SOURCE=.\ViewKebaPc2000.h
# End Source File
# Begin Source File

SOURCE=.\ViewLandauDtp.h
# End Source File
# Begin Source File

SOURCE=.\ViewNcr_1_1.h
# End Source File
# Begin Source File

SOURCE=.\ViewNcr_1_2.h
# End Source File
# Begin Source File

SOURCE=.\ViewSniCom.h
# End Source File
# Begin Source File

SOURCE=.\ViewSniDos.h
# End Source File
# Begin Source File

SOURCE=.\ViewSniIbmEmulation.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Ascom.ico
# End Source File
# Begin Source File

SOURCE=.\res\AVM.ico
# End Source File
# Begin Source File

SOURCE=.\res\Dialogbank.ico
# End Source File
# Begin Source File

SOURCE=.\res\dtp.ico
# End Source File
# Begin Source File

SOURCE=.\res\engel.ico
# End Source File
# Begin Source File

SOURCE=.\res\GermanParcel.ico
# End Source File
# Begin Source File

SOURCE=.\res\ibm.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_dtp.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_file.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_keba.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_ncr.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_ncr_new.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_sni_ibm_emu.ico
# End Source File
# Begin Source File

SOURCE=.\res\Rondo.ico
# End Source File
# Begin Source File

SOURCE=.\res\SDITest.ico
# End Source File
# Begin Source File

SOURCE=.\res\SDITest.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SDITestDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\sni.ico
# End Source File
# Begin Source File

SOURCE=.\res\sni_com.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=C:\log\SDITest.log
# End Source File
# Begin Source File

SOURCE=.\SDITest.reg
# End Source File
# End Target
# End Project
