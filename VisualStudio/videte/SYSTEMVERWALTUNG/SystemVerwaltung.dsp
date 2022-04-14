# Microsoft Developer Studio Project File - Name="SystemVerwaltung" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SystemVerwaltung - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SystemVerwaltung.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SystemVerwaltung.mak" CFG="SystemVerwaltung - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SystemVerwaltung - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SystemVerwaltung - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SystemVerwaltung - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "SystemVerwaltung - Win32 Ita" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/SystemVerwaltung", ANTAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\units\ICPCON\ICPCONDll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
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
# ADD LINK32 cipc.lib oemgui.lib WKClasses.lib ICPCONDll.lib i7000.lib uart.lib /nologo /subsystem:windows /map /machine:I386 /libpath:"..\units\ICPCON\ICPCONDll\Release"
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
TargetPath=.\Release\SystemVerwaltung.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                       $(TargetPath)                       c:\security\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /ZI /Od /I "..\units\ICPCON\ICPCONDll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 cipcdebug.lib oemguid.lib WKClassesDebug.lib ICPCONDllD.lib i7000.lib uart.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"..\units\ICPCON\ICPCONDll\Debug"
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SystemVerwaltung___Win32_Enu"
# PROP BASE Intermediate_Dir "SystemVerwaltung___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\units\ICPCON\ICPCONDll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipc.lib oemgui.lib WKClasses.lib ICPCONDll.lib i7000.lib uart.lib /nologo /subsystem:windows /map /machine:I386 /libpath:"..\units\ICPCON\ICPCONDll\Release"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SystemVerwaltung___Win32_Ita"
# PROP BASE Intermediate_Dir "SystemVerwaltung___Win32_Ita"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Ita"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O1 /I "..\units\ICPCON\ICPCONDll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipc.lib oemgui.lib WKClasses.lib ICPCONDll.lib i7000.lib uart.lib /nologo /subsystem:windows /map /machine:I386 /libpath:"..\units\ICPCON\ICPCONDll\Release"

!ENDIF 

# Begin Target

# Name "SystemVerwaltung - Win32 Release"
# Name "SystemVerwaltung - Win32 Debug"
# Name "SystemVerwaltung - Win32 Enu"
# Name "SystemVerwaltung - Win32 Ita"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "Pages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\activationpage.cpp
# End Source File
# Begin Source File

SOURCE=.\ActualImagePage.cpp
# End Source File
# Begin Source File

SOURCE=.\AKUPage.cpp
# End Source File
# Begin Source File

SOURCE=.\AppPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ArchivPage.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupProcessPage.cpp
# End Source File
# Begin Source File

SOURCE=.\callpage.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckCallPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CoCoPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DatabaseFieldPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DefaultTreeKnode.cpp
# End Source File
# Begin Source File

SOURCE=.\DrivePage.cpp
# End Source File
# Begin Source File

SOURCE=.\EMailPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FieldMapPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GemosPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GuardTourPage.cpp
# End Source File
# Begin Source File

SOURCE=.\hardwarepage.cpp
# End Source File
# Begin Source File

SOURCE=.\HostPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ICPCONConfigPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageCallPage.cpp
# End Source File
# Begin Source File

SOURCE=.\inputpage.cpp
# End Source File
# Begin Source File

SOURCE=.\ISDNPage.cpp
# End Source File
# Begin Source File

SOURCE=.\JaCobPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MESZPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MicoPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiMonitorPage.cpp
# End Source File
# Begin Source File

SOURCE=.\NotificationPage.cpp
# End Source File
# Begin Source File

SOURCE=.\NTLoginPage.cpp
# End Source File
# Begin Source File

SOURCE=.\outputpage.cpp
# End Source File
# Begin Source File

SOURCE=.\permissionpage.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageVariable.cpp
# End Source File
# Begin Source File

SOURCE=.\PTUnitPage.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayPage.cpp
# End Source File
# Begin Source File

SOURCE=.\RS232Page.cpp
# End Source File
# Begin Source File

SOURCE=.\savepage.cpp
# End Source File
# Begin Source File

SOURCE=.\SaVicPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SecurityPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SimPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SMSPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SoftwarePage.cpp
# End Source File
# Begin Source File

SOURCE=.\STMPage.cpp
# End Source File
# Begin Source File

SOURCE=.\timereditorpage.cpp
# End Source File
# Begin Source File

SOURCE=.\TOBSPage.cpp
# End Source File
# Begin Source File

SOURCE=.\userpage.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoOutPage.cpp
# End Source File
# Begin Source File

SOURCE=.\YutaPage.cpp
# End Source File
# End Group
# Begin Group "Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\inputgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\inputlist.cpp
# End Source File
# Begin Source File

SOURCE=.\output.cpp
# End Source File
# Begin Source File

SOURCE=.\outputgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\outputlist.cpp
# End Source File
# Begin Source File

SOURCE=.\processlist.cpp
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ComConfigurationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\InformationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\InputDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\passwordcheckdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogACT.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogIBM.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogMicrolock.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogNCR.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogNCTDiva.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIDefaultMicrolockDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\selectinputdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecialCharactersDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\timereditorcombi.cpp
# End Source File
# Begin Source File

SOURCE=.\timereditorspan.cpp
# End Source File
# Begin Source File

SOURCE=.\TimerImportDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\timersetdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TimerTestDialog.cpp
# End Source File
# End Group
# Begin Group "SDI Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SDIAlarmType.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationACTRS232.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationIbmRS232.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationMicrolockRS232.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationNcrRS232.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationRS232.cpp
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationRS232NCTDiva.cpp
# End Source File
# End Group
# Begin Group "Cipc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CIPCDatabaseSV.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCInputSV.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCControlAudioUnit.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AudioChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioChannelDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioChannelPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\media.cpp
# End Source File
# Begin Source File

SOURCE=.\mediagroup.cpp
# End Source File
# Begin Source File

SOURCE=.\medialist.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteThread.cpp
# End Source File
# Begin Source File

SOURCE=.\statuslist.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SVCoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SVDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SVPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SVPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\SVTree.cpp
# End Source File
# Begin Source File

SOURCE=.\SVView.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltung.cpp
# End Source File
# Begin Source File

SOURCE=.\timereditorhourctl.cpp
# End Source File
# Begin Source File

SOURCE=.\treelist.cpp
# End Source File
# Begin Source File

SOURCE=.\USBcamPage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "Pages Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\activationpage.h
# End Source File
# Begin Source File

SOURCE=.\ActualImagePage.h
# End Source File
# Begin Source File

SOURCE=.\AKUPage.h
# End Source File
# Begin Source File

SOURCE=.\AppPage.h
# End Source File
# Begin Source File

SOURCE=.\ArchivPage.h
# End Source File
# Begin Source File

SOURCE=.\AudioPage.h
# End Source File
# Begin Source File

SOURCE=.\BackupProcessPage.h
# End Source File
# Begin Source File

SOURCE=.\callpage.h
# End Source File
# Begin Source File

SOURCE=.\CheckCallPage.h
# End Source File
# Begin Source File

SOURCE=.\CoCoPage.h
# End Source File
# Begin Source File

SOURCE=.\ComConfigurationDialog.h
# End Source File
# Begin Source File

SOURCE=.\DatabaseFieldPage.h
# End Source File
# Begin Source File

SOURCE=.\DefaultTreeKnode.h
# End Source File
# Begin Source File

SOURCE=.\DrivePage.h
# End Source File
# Begin Source File

SOURCE=.\EMailPage.h
# End Source File
# Begin Source File

SOURCE=.\FieldMapPage.h
# End Source File
# Begin Source File

SOURCE=.\GemosPage.h
# End Source File
# Begin Source File

SOURCE=.\GuardTourPage.h
# End Source File
# Begin Source File

SOURCE=.\hardwarepage.h
# End Source File
# Begin Source File

SOURCE=.\HostPage.h
# End Source File
# Begin Source File

SOURCE=.\ICPCONConfigPage.h
# End Source File
# Begin Source File

SOURCE=.\ImageCallPage.h
# End Source File
# Begin Source File

SOURCE=.\Informationdialog.h
# End Source File
# Begin Source File

SOURCE=.\InputDialog.h
# End Source File
# Begin Source File

SOURCE=.\inputpage.h
# End Source File
# Begin Source File

SOURCE=.\ISDNPage.h
# End Source File
# Begin Source File

SOURCE=.\JaCobPage.h
# End Source File
# Begin Source File

SOURCE=.\MESZPage.h
# End Source File
# Begin Source File

SOURCE=.\MicoPage.h
# End Source File
# Begin Source File

SOURCE=.\MultiMonitorPage.h
# End Source File
# Begin Source File

SOURCE=.\NotificationPage.h
# End Source File
# Begin Source File

SOURCE=.\NTLoginPage.h
# End Source File
# Begin Source File

SOURCE=.\OutputDialog.h
# End Source File
# Begin Source File

SOURCE=.\outputpage.h
# End Source File
# Begin Source File

SOURCE=.\passwordcheckdialog.h
# End Source File
# Begin Source File

SOURCE=.\permissionpage.h
# End Source File
# Begin Source File

SOURCE=.\PropPageVariable.h
# End Source File
# Begin Source File

SOURCE=.\PTUnitPage.h
# End Source File
# Begin Source File

SOURCE=.\RelayDialog.h
# End Source File
# Begin Source File

SOURCE=.\RelayPage.h
# End Source File
# Begin Source File

SOURCE=.\RemoteDialog.h
# End Source File
# Begin Source File

SOURCE=.\RS232Page.h
# End Source File
# Begin Source File

SOURCE=.\savepage.h
# End Source File
# Begin Source File

SOURCE=.\SaVicPage.h
# End Source File
# Begin Source File

SOURCE=.\SDIPage.h
# End Source File
# Begin Source File

SOURCE=.\SecurityPage.h
# End Source File
# Begin Source File

SOURCE=.\SimPage.h
# End Source File
# Begin Source File

SOURCE=.\SMSPage.h
# End Source File
# Begin Source File

SOURCE=.\SocketPage.h
# End Source File
# Begin Source File

SOURCE=.\SoftwarePage.h
# End Source File
# Begin Source File

SOURCE=.\STMPage.h
# End Source File
# Begin Source File

SOURCE=.\timereditorpage.h
# End Source File
# Begin Source File

SOURCE=.\TOBSPage.h
# End Source File
# Begin Source File

SOURCE=.\userpage.h
# End Source File
# Begin Source File

SOURCE=.\VideoOutPage.h
# End Source File
# Begin Source File

SOURCE=.\YutaPage.h
# End Source File
# End Group
# Begin Group "Objects Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\inputgroup.h
# End Source File
# Begin Source File

SOURCE=.\inputlist.h
# End Source File
# Begin Source File

SOURCE=.\inputtooutput.h
# End Source File
# Begin Source File

SOURCE=.\output.h
# End Source File
# Begin Source File

SOURCE=.\outputgroup.h
# End Source File
# Begin Source File

SOURCE=.\outputlist.h
# End Source File
# Begin Source File

SOURCE=.\processlist.h
# End Source File
# End Group
# Begin Group "Dialogs Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SDIConfigurationDialog.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogACT.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogIBM.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogMicrolock.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogNCR.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationDialogNCTDiva.h
# End Source File
# Begin Source File

SOURCE=.\SDIDefaultMicrolockDlg.h
# End Source File
# Begin Source File

SOURCE=.\SDIDialog.h
# End Source File
# Begin Source File

SOURCE=.\selectinputdlg.h
# End Source File
# Begin Source File

SOURCE=.\SpecialCharactersDialog.h
# End Source File
# Begin Source File

SOURCE=.\timereditorcombi.h
# End Source File
# Begin Source File

SOURCE=.\timereditorspan.h
# End Source File
# Begin Source File

SOURCE=.\TimerImportDialog.h
# End Source File
# Begin Source File

SOURCE=.\timersetdialog.h
# End Source File
# Begin Source File

SOURCE=.\TimerTestDialog.h
# End Source File
# End Group
# Begin Group "SDI Objects Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SDIAlarmType.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationACTRS232.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationIbmRS232.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationMicrolockRS232.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationNcrRS232.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationRS232.h
# End Source File
# Begin Source File

SOURCE=.\SDIConfigurationRS232NCTDiva.h
# End Source File
# End Group
# Begin Group "CIPC Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CIPCDatabaseSV.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputSV.h
# End Source File
# Begin Source File

SOURCE=.\IPCControlAudioUnit.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AudioChannel.h
# End Source File
# Begin Source File

SOURCE=.\AudioChannelDialog.h
# End Source File
# Begin Source File

SOURCE=.\AudioChannelPage.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\media.h
# End Source File
# Begin Source File

SOURCE=.\mediagroup.h
# End Source File
# Begin Source File

SOURCE=.\medialist.h
# End Source File
# Begin Source File

SOURCE=.\RemoteThread.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\statuslist.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SVCoolBar.h
# End Source File
# Begin Source File

SOURCE=.\SVDoc.h
# End Source File
# Begin Source File

SOURCE=.\SVPage.h
# End Source File
# Begin Source File

SOURCE=.\SVPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\SVTree.h
# End Source File
# Begin Source File

SOURCE=.\SVView.h
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltung.h
# End Source File
# Begin Source File

SOURCE=.\timereditorhourctrl.h
# End Source File
# Begin Source File

SOURCE=.\treelist.h
# End Source File
# Begin Source File

SOURCE=.\USBcamPage.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\3state.bmp
# End Source File
# Begin Source File

SOURCE=.\res\archive.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cold.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down.ico
# End Source File
# Begin Source File

SOURCE=.\res\drives.bmp
# End Source File
# Begin Source File

SOURCE=.\res\garny.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\images.bmp
# End Source File
# Begin Source File

SOURCE=.\res\radio.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ReadSample.avi
# End Source File
# Begin Source File

SOURCE=.\res\ReadSampleStopped.avi
# End Source File
# Begin Source File

SOURCE=.\res\state.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SVDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\SystemVerwaltung.ico
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltung.rc

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\SystemVerwaltung.rc2
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltungenu.rc

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltungIta.rc

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Ita"

# PROP BASE Exclude_From_Build 1
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x410

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\TimerDiv.ico
# End Source File
# Begin Source File

SOURCE=.\res\timerminus.ico
# End Source File
# Begin Source File

SOURCE=.\res\timerplus.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\treestatemult.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Up.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up.ico
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter "lng;csv"
# Begin Source File

SOURCE=.\Deu_Enu.csv
# End Source File
# Begin Source File

SOURCE=.\Enu_Ita.csv
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltungenu.lng
# End Source File
# Begin Source File

SOURCE=.\SystemVerwaltungIta.lng
# End Source File
# Begin Source File

SOURCE=.\Timer.tim
# End Source File
# End Group
# End Target
# End Project
