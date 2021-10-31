# Microsoft Developer Studio Project File - Name="AC_COM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AC_COM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AC_COM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AC_COM.mak" CFG="AC_COM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AC_COM - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AC_COM - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AC_COM - Win32 ReleaseEngl" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AC_COM - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_TARG_DEU" /D "AFX_RESOURCE_DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ETSBIND.lib CCaraInfo.lib CCurve.lib version.lib Winmm.lib CEtsDebug.lib CEtsCDPlayer.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libmmd" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ELSEIF  "$(CFG)" == "AC_COM - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_TARG_DEU" /D "AFX_RESOURCE_DLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ETSBIND.lib CCaraInfo.lib CCurve.lib version.lib Winmm.lib CEtsDebug.lib CEtsCDPlayer.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libmmd" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"

!ELSEIF  "$(CFG)" == "AC_COM - Win32 ReleaseEngl"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseEngl"
# PROP BASE Intermediate_Dir "ReleaseEngl"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseEngl"
# PROP Intermediate_Dir "ReleaseEngl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_TARG_DEU" /D "AFX_RESOURCE_DLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_TARG_ENU" /D "AFX_RESOURCE_DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "AFX_TARG_ENU" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 ETSBIND.lib CCaraInfo.lib CCurve.lib version.lib Winmm.lib /nologo /subsystem:windows /machine:I386 /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 ETSBIND.lib CCaraInfo.lib CCurve.lib version.lib Winmm.lib CEtsDebug.lib CEtsCDPlayer.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libmmd" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ENDIF 

# Begin Target

# Name "AC_COM - Win32 Release"
# Name "AC_COM - Win32 Debug"
# Name "AC_COM - Win32 ReleaseEngl"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AC_COM.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\AC_COM.hpj

!IF  "$(CFG)" == "AC_COM - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Release
InputPath=.\hlp\AC_COM.hpj
InputName=AC_COM

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AC_COM - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Debug
InputPath=.\hlp\AC_COM.hpj
InputName=AC_COM

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AC_COM - Win32 ReleaseEngl"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\ReleaseEngl
InputPath=.\hlp\AC_COM.hpj
InputName=AC_COM

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AC_COM.rc
# End Source File
# Begin Source File

SOURCE=.\AC_COMDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioControlData.cpp
# End Source File
# Begin Source File

SOURCE=.\BasicParamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CommPort.cpp
# End Source File
# Begin Source File

SOURCE=.\EvaluateMeasurements.cpp
# End Source File
# Begin Source File

SOURCE=.\IRcodeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Measurement.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgBoxBehaviour.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AC_COM.h
# End Source File
# Begin Source File

SOURCE=.\AC_COMDlg.h
# End Source File
# Begin Source File

SOURCE=.\AudioControlData.h
# End Source File
# Begin Source File

SOURCE=.\BasicParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommPort.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\Curve.h
# End Source File
# Begin Source File

SOURCE=.\EvaluateMeasurements.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\FileHeader.h
# End Source File
# Begin Source File

SOURCE=.\IRcodeDlg.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\MathCurve.h
# End Source File
# Begin Source File

SOURCE=.\Measurement.h
# End Source File
# Begin Source File

SOURCE=.\MsgBoxBehaviour.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\Plot.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "AC_COM - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Erstellen von Include-Datei für Hilfe...
TargetName=AC_COM
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AC_COM - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Erstellen von Include-Datei für Hilfe...
TargetName=AC_COM
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AC_COM - Win32 ReleaseEngl"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Erstellen von Include-Datei für Hilfe...
TargetName=AC_COM
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AC_COM.ico
# End Source File
# Begin Source File

SOURCE=.\res\AC_COM.rc2
# End Source File
# Begin Source File

SOURCE=.\res\BtnEject.ico
# End Source File
# Begin Source File

SOURCE=.\res\CaraSdb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CDInfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\FileType.ico
# End Source File
# Begin Source File

SOURCE=".\res\Hand-m.cur"
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\list_ima.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Ntcheck.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Remote.ico
# End Source File
# End Group
# Begin Group "Hilfedateien"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=.\hlp\AC_COM.cnt

!IF  "$(CFG)" == "AC_COM - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Kopieren der Inhaltsdatei...
OutDir=.\Release
InputPath=.\hlp\AC_COM.cnt
InputName=AC_COM

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "AC_COM - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Kopieren der Inhaltsdatei...
OutDir=.\Debug
InputPath=.\hlp\AC_COM.cnt
InputName=AC_COM

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "AC_COM - Win32 ReleaseEngl"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Kopieren der Inhaltsdatei...
OutDir=.\ReleaseEngl
InputPath=.\hlp\AC_COM.cnt
InputName=AC_COM

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\ACDLG.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\ACManual.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxDlg.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\WinDlg.rtf
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section AC_COM : {B7ABC220-DF71-11CF-8E74-00A0C90F26F8}
# 	2:5:Class:Cmci
# 	2:10:HeaderFile:mci.h
# 	2:8:ImplFile:mci.cpp
# End Section
# Section AC_COM : {C1A8AF25-1257-101B-8FB0-0020AF039CA3}
# 	2:21:DefaultSinkHeaderFile:mci.h
# 	2:16:DefaultSinkClass:Cmci
# End Section
