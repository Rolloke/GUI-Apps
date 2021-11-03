# Microsoft Developer Studio Project File - Name="ETS3DGLDLG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ETS3DGLDLG - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "ETS3DGLDLG.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ETS3DGLDLG.mak" CFG="ETS3DGLDLG - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ETS3DGLDLG - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "ETS3DGLDLG - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "ETS3DGLDLG - Win32 ReleaseEngl" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ETS3DGLDLG - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /I "c:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_DEU" /D "AFX_TARG_NEU" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib etsbind.lib CCaraInfo.lib CCaraMat.lib CEtsCDPlayer.lib /nologo /subsystem:windows /machine:I386 /out:"Release/GLSetup.exe" /libpath:"c:\Daten\MyProjects\Libraries\Release"

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "AFX_RESOURCE_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU" /d "AFX_TARG_DES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib etsbind.lib CCaraInfo.lib CCaraMat.lib CEtsCDPlayer.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/GLSetup.exe" /pdbtype:sept /libpath:"c:\Daten\MyProjects\Libraries\Debug"

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "c:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "c:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /D "AFX_RESOURCE_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL" /d "AFX_TARGET_DEU"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_TARG_ENU" /d "AFX_TARG_NEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib etsbind.lib /nologo /subsystem:windows /machine:I386 /out:"Release/GLSetup.exe" /libpath:"c:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 version.lib etsbind.lib CCaraInfo.lib CCaraMat.lib CEtsCDPlayer.lib /nologo /subsystem:windows /machine:I386 /out:"ReleaseEngl/GLSetup.exe" /libpath:"c:\Daten\MyProjects\Libraries\Release"

!ENDIF 

# Begin Target

# Name "ETS3DGLDLG - Win32 Release"
# Name "ETS3DGLDLG - Win32 Debug"
# Name "ETS3DGLDLG - Win32 ReleaseEngl"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CarawalkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorMessageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ETS3DGLDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\ETS3DGLDLG.rc
# End Source File
# Begin Source File

SOURCE=.\ETS3DGLDLGDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\GLSetup.hpj

!IF  "$(CFG)" == "ETS3DGLDLG - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Release
InputPath=.\hlp\GLSetup.hpj
InputName=GLSetup

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

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Debug
InputPath=.\hlp\GLSetup.hpj
InputName=GLSetup

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

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 ReleaseEngl"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\ReleaseEngl
InputPath=.\hlp\GLSetup.hpj
InputName=GLSetup

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlpEngl\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlpEngl\$(InputName).hlp" goto :Error 
	copy "hlpEngl\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlpEngl\$(InputName).hpj(1) : error: 
	type "hlpEngl\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MaterialComponents.cpp
# End Source File
# Begin Source File

SOURCE=.\mci.cpp
# End Source File
# Begin Source File

SOURCE=.\PathDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\PresentationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TimerDlg.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\AnimationDlg.h
# End Source File
# Begin Source File

SOURCE=.\CarawalkDlg.h
# End Source File
# Begin Source File

SOURCE=.\CopyFolder.h
# End Source File
# Begin Source File

SOURCE=.\ErrorMessageDlg.h
# End Source File
# Begin Source File

SOURCE=.\ETS3DGLDLG.h
# End Source File
# Begin Source File

SOURCE=.\ETS3DGLDLGDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\MaterialComponents.h
# End Source File
# Begin Source File

SOURCE=.\mci.h
# End Source File
# Begin Source File

SOURCE=.\PathDlg.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\PresentationDlg.h
# End Source File
# Begin Source File

SOURCE=.\ReportDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "ETS3DGLDLG - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Erstellen von Include-Datei für Hilfe...
TargetName=GLSetup
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

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Erstellen von Include-Datei für Hilfe...
TargetName=GLSetup
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

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 ReleaseEngl"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Erstellen von Include-Datei für Hilfe...
TargetName=GLSetup
InputPath=.\Resource.h

"hlpEngl\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlpEngl\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlpEngl\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlpEngl\$(TargetName).hm" 
	echo. >>"hlpEngl\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlpEngl\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlpEngl\$(TargetName).hm" 
	echo. >>"hlpEngl\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlpEngl\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlpEngl\$(TargetName).hm" 
	echo. >>"hlpEngl\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlpEngl\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlpEngl\$(TargetName).hm" 
	echo. >>"hlpEngl\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlpEngl\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlpEngl\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TimerDlg.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ETS3DGLDLG.ico
# End Source File
# Begin Source File

SOURCE=.\res\ETS3DGLDLG.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# End Group
# Begin Group "Hilfedateien"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=.\hlp\AfxDlg.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\GLSetup.cnt

!IF  "$(CFG)" == "ETS3DGLDLG - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Kopieren der Inhaltsdatei...
OutDir=.\Release
InputPath=.\hlp\GLSetup.cnt
InputName=GLSetup

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Kopieren der Inhaltsdatei...
OutDir=.\Debug
InputPath=.\hlp\GLSetup.cnt
InputName=GLSetup

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "ETS3DGLDLG - Win32 ReleaseEngl"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Kopieren der Inhaltsdatei...
OutDir=.\ReleaseEngl
InputPath=.\hlp\GLSetup.cnt
InputName=GLSetup

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlpEngl\$(InputName).cnt" $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section ETS3DGLDLG : {B7ABC220-DF71-11CF-8E74-00A0C90F26F8}
# 	2:5:Class:Cmci
# 	2:10:HeaderFile:mci.h
# 	2:8:ImplFile:mci.cpp
# End Section
# Section ETS3DGLDLG : {B09DE713-87C1-11D1-8BE3-0000F8754DA1}
# 	2:5:Class:CAnimation
# 	2:10:HeaderFile:animation.h
# 	2:8:ImplFile:animation.cpp
# End Section
# Section ETS3DGLDLG : {C1A8AF25-1257-101B-8FB0-0020AF039CA3}
# 	2:21:DefaultSinkHeaderFile:mci.h
# 	2:16:DefaultSinkClass:Cmci
# End Section
# Section ETS3DGLDLG : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section ETS3DGLDLG : {B09DE715-87C1-11D1-8BE3-0000F8754DA1}
# 	2:21:DefaultSinkHeaderFile:animation.h
# 	2:16:DefaultSinkClass:CAnimation
# End Section
