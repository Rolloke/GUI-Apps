# Microsoft Developer Studio Project File - Name="CARA2DV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CARA2DV - Win32 DebugEngl
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Cara2dv.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Cara2dv.mak" CFG="CARA2DV - Win32 DebugEngl"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "CARA2DV - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "CARA2DV - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "CARA2DV - Win32 ReleaseEngl" (basierend auf  "Win32 (x86) Application")
!MESSAGE "CARA2DV - Win32 DebugEngl" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARA2DV - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "CARA_MFC_HEADER" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /i "C:\Daten\MyProjects\CaraPrev\resource.h" /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\Release\EtsPrev.lib CEtsDiv.lib CEtsLsmb.lib CEtsDebug.lib EtsBind.lib CEtsHelp.lib CDibSection.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libmmd.lib" /out:"Release/ETS2DV.exe" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "CARA_MFC_HEADER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\Debug\EtsPrev.lib CEtsDiv.lib CEtsLsmb.lib CEtsDebug.lib ETSBIND.lib CEtsHelp.lib CDibsection.lib Kernel32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCD" /nodefaultlib:"libm" /nodefaultlib:"libmmd" /out:"Debug/ETS2DV.exe" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# SUBTRACT LINK32 /incremental:no /nodefaultlib

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /D "CARA_MFC_HEADER" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /i "C:\Daten\MyProjects\CaraPrev\resource.h" /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
# ADD RSC /l 0x409 /i "C:\Daten\MyProjects\CaraPrev\resource.h" /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_ENU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 C:\Daten\MyProjects\CaraPrev\Release\EtsPrev.lib CEtsDiv.lib CEtsLsmb.lib CCaraInfo.lib CEtsDebug.lib EtsBind.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release/ETS2DV.exe" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\ReleaseEngl\EtsPrev.lib CEtsDiv.lib CEtsLsmb.lib CEtsDebug.lib EtsBind.lib CEtsHelp.lib CDibSection.lib /nologo /subsystem:windows /map /machine:I386 /out:"ReleaseEngl/ETS2DV.exe" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 DebugEngl"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugEngl"
# PROP BASE Intermediate_Dir "DebugEngl"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugEngl"
# PROP Intermediate_Dir "DebugEngl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "AFX_TARG_NEU" /d "AFX_TARG_ENU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 C:\Daten\MyProjects\CaraPrev\Debug\EtsPrev.lib CEtsDiv.lib CEtsLsmb.lib CCaraInfo.lib CEtsDebug.lib EtsBind.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCD" /out:"Debug/ETS2DV.exe" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# SUBTRACT BASE LINK32 /incremental:no /nodefaultlib
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\DebugEngl\EtsPrev.lib CEtsDiv.lib CEtsLsmb.lib CCaraInfo.lib CEtsDebug.lib EtsBind.lib CDibSection.lib CEtsHelp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCD" /out:"DebugEngl/ETS2DV.exe" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# SUBTRACT LINK32 /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "CARA2DV - Win32 Release"
# Name "CARA2DV - Win32 Debug"
# Name "CARA2DV - Win32 ReleaseEngl"
# Name "CARA2DV - Win32 DebugEngl"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ArithmaEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Aritmetic.cpp

!IF  "$(CFG)" == "CARA2DV - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 DebugEngl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AverageDlg.cpp

!IF  "$(CFG)" == "CARA2DV - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 ReleaseEngl"

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 DebugEngl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CARA2DV.cpp
# End Source File
# Begin Source File

SOURCE=.\CARA2DV.rc
# End Source File
# Begin Source File

SOURCE=.\CARA2DVDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CARA2DVView.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Derive.cpp
# End Source File
# Begin Source File

SOURCE=.\EditDoubleCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\ETS2DV.hpj

!IF  "$(CFG)" == "CARA2DV - Win32 Release"

USERDEP__ETS2D="$(ProjDir)\hlp\AfxCore.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Release
ProjDir=.
TargetName=ETS2DV
InputPath=.\hlp\ETS2DV.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 Debug"

USERDEP__ETS2D="$(ProjDir)\hlp\AfxCore.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Debug
ProjDir=.
TargetName=ETS2DV
InputPath=.\hlp\ETS2DV.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 ReleaseEngl"

USERDEP__ETS2D="$(ProjDir)\hlpEngl\AfxCore.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\ReleaseEngl
ProjDir=.
TargetName=ETS2DV
InputPath=.\hlp\ETS2DV.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelpEngl.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "CARA2DV - Win32 DebugEngl"

USERDEP__ETS2D="$(ProjDir)\hlp\AfxCore.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\DebugEngl
ProjDir=.
TargetName=ETS2DV
InputPath=.\hlp\ETS2DV.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ETS2DV.odl
# End Source File
# Begin Source File

SOURCE=.\FitCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\Integrate.cpp
# End Source File
# Begin Source File

SOURCE=.\IpFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroEditorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveCurveAsWave.cpp
# End Source File
# Begin Source File

SOURCE=.\SplineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ArithmaEdit.h
# End Source File
# Begin Source File

SOURCE=.\Aritmetic.h
# End Source File
# Begin Source File

SOURCE=.\AverageDlg.h
# End Source File
# Begin Source File

SOURCE=.\CARA2DV.h
# End Source File
# Begin Source File

SOURCE=.\CARA2DVDoc.h
# End Source File
# Begin Source File

SOURCE=.\CARA2DVView.h
# End Source File
# Begin Source File

SOURCE=.\CreateCurve.h
# End Source File
# Begin Source File

SOURCE=.\CurveDialog.h
# End Source File
# Begin Source File

SOURCE=.\CurveFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\Derive.h
# End Source File
# Begin Source File

SOURCE=.\EditDoubleCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FitCurve.h
# End Source File
# Begin Source File

SOURCE=.\Integrate.h
# End Source File
# Begin Source File

SOURCE=.\IpFrame.h
# End Source File
# Begin Source File

SOURCE=.\MacroEditorDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SaveCurveAsWave.h
# End Source File
# Begin Source File

SOURCE=.\SplineDlg.h
# End Source File
# Begin Source File

SOURCE=.\SrvrItem.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CARA2DV.ico
# End Source File
# Begin Source File

SOURCE=.\res\CARA2DV.rc2
# End Source File
# Begin Source File

SOURCE=.\res\CARA2DVDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\CARA2DVIEW.ico
# End Source File
# Begin Source File

SOURCE=.\res\CARA2DVIEWDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\cara_cur.ico
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\CaraView.cnt
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\AfxPrint.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\arrow.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Average.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Contexthelp.rtf
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\Contexthelp.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\CreateDlg.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CRegDlg.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Derivedlg.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Dialogs.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\dontdrop.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditDel.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FarbDlg.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\fist.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\fistplus.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\funcref.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\funcrefs.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\hand.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpPTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpQuMrk.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\IntDlg.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Kurven.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\magnify.bmp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\hlp\makroed.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Plot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlotBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Plots.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\pltcref.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltdeflt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltdelone.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltdelzo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltedit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltline.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltminus.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltnezo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltnumst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltpick.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltplus.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltprzo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltrefon.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\pltzoom.bmp
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\Properties.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\SchriftDlg.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\sizeall.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Spline.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\stdabwfc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\StdBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\titlebar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\updown.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewaxis.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewbox.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewdlg.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewfase.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewfull.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewgrid.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewlite.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\viewlstn.bmp
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\WinDlg.rtf
# End Source File
# End Group
# Begin Source File

SOURCE=.\bmpformat.txt
# End Source File
# Begin Source File

SOURCE=.\CARA2DV.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
