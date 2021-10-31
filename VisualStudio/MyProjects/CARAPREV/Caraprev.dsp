# Microsoft Developer Studio Project File - Name="CARAPREV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CARAPREV - Win32 DebugEngl
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Caraprev.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Caraprev.mak" CFG="CARAPREV - Win32 DebugEngl"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CARAPREV - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARAPREV - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARAPREV - Win32 ReleaseEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARAPREV - Win32 DebugEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARAPREV - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "CARA_MFC_HEADER" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 version.lib CDibSection.lib CEtsDiv.lib CEtsLsmb.lib CEtsPrtd.lib CEts3DGL.lib CEtsDebug.lib EtsBind.lib CEtsMapi.lib CCaraInfo.lib CEtsHelp.lib /nologo /subsystem:windows /dll /map /machine:I386 /nodefaultlib:"libmmd.lib" /out:"Release/ETSPREV.dll" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren von ETSPREV.DLL
PostBuild_Cmds=$(OutDir)\CopyDll.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "CARA_MFC_HEADER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib CEtsDiv.lib CDibSection.lib CEtsLsmb.lib CEtsPrtd.lib CEts3DGL.lib CEtsDebug.lib ETSBIND.lib CEtsMapi.lib CCaraInfo.lib CEtsHelp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCMTD" /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCD" /nodefaultlib:"libm" /nodefaultlib:"libmmd" /out:"Debug/ETSPREV.dll" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren von ETSPREV.DLL
PostBuild_Cmds=$(OutDir)\CopyDll.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /D "CARA_MFC_HEADER" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_ENU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 version.lib CDibSection.lib CEtsDiv.lib CEtsLsmb.lib CEtsPrtd.lib CEts3DGL.lib CEtsDebug.lib EtsBind.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"Release/ETSPREV.dll" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 version.lib CDibSection.lib CEtsDiv.lib CEtsLsmb.lib CEtsPrtd.lib CEts3DGL.lib CEtsDebug.lib EtsBind.lib CEtsMapi.lib CCaraInfo.lib CEtsHelp.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"ReleaseEngl/ETSPREV.dll" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# Begin Special Build Tool
OutDir=.\ReleaseEngl
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren von ETSPREV.DLL
PostBuild_Cmds=$(OutDir)\CopyDll.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 DebugEngl"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /D "CARA_MFC_HEADER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_TARG_NEU" /d "AFX_TARG_ENU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 version.lib CEtsDiv.lib CDibSection.lib CEtsLsmb.lib CEtsPrtd.lib CEts3DGL.lib CEtsDebug.lib EtsBind.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCD" /out:"Debug/ETSPREV.dll" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# ADD LINK32 version.lib CEtsDiv.lib CDibSection.lib CEtsLsmb.lib CEtsPrtd.lib CEts3DGL.lib CEtsDebug.lib EtsBind.lib CEtsMapi.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCD" /out:"DebugEngl/ETSPREV.dll" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\DebugEngl
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren von ETSPREV.DLL
PostBuild_Cmds=$(OutDir)\CopyDll.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CARAPREV - Win32 Release"
# Name "CARAPREV - Win32 Debug"
# Name "CARAPREV - Win32 ReleaseEngl"
# Name "CARAPREV - Win32 DebugEngl"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BezierLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\BrushListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\BrushStylePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CARADoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\caramenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CARAPREV.cpp
# End Source File
# Begin Source File

SOURCE=.\CARAPREV.def
# End Source File
# Begin Source File

SOURCE=.\CARAPREV.rc
# End Source File
# Begin Source File

SOURCE=.\CARAPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraPropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraView.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraViewProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\CaraWinApp.cpp
# End Source File
# Begin Source File

SOURCE=.\CatchGridDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CircleLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorArray.cpp

!IF  "$(CFG)" == "CARAPREV - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 DebugEngl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ColorLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\ColScalePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\CopyDll.bat
# End Source File
# Begin Source File

SOURCE=.\Release\CopyDll.bat
# End Source File
# Begin Source File

SOURCE=.\Curve.cpp

!IF  "$(CFG)" == "CARAPREV - Win32 Release"

# ADD CPP /G6 /W4 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 DebugEngl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CurveColorPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveValuePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FontPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.cpp

!IF  "$(CFG)" == "CARAPREV - Win32 Release"

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 ReleaseEngl"

# ADD CPP /GB /O1 /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 DebugEngl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LabelContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\LineLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\LineListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\LineStylePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\MathCurve.cpp

!IF  "$(CFG)" == "CARAPREV - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 DebugEngl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PasteSelectivDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PictureLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\PicturePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotRangePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotViewPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PreviewFileHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\RectLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\ScalePainter.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TextEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\TextLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\Transformation.cpp

!IF  "$(CFG)" == "CARAPREV - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER" /FAs
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 Debug"

# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 ReleaseEngl"

# ADD BASE CPP /FAs
# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER" /FAs
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARAPREV - Win32 DebugEngl"

# ADD BASE CPP /FAcs
# ADD CPP /FAcs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TreeDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BezierLabel.h
# End Source File
# Begin Source File

SOURCE=.\BrushListBox.h
# End Source File
# Begin Source File

SOURCE=.\BrushStylePropPage.h
# End Source File
# Begin Source File

SOURCE=.\CARADoc.h
# End Source File
# Begin Source File

SOURCE=.\CaraFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\caramenu.h
# End Source File
# Begin Source File

SOURCE=.\CARAPREV.h
# End Source File
# Begin Source File

SOURCE=.\CARAPreview.h
# End Source File
# Begin Source File

SOURCE=.\CaraPropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\CaraToolbar.h
# End Source File
# Begin Source File

SOURCE=.\CaraView.h
# End Source File
# Begin Source File

SOURCE=.\CaraViewProperties.h
# End Source File
# Begin Source File

SOURCE=.\CaraWinApp.h
# End Source File
# Begin Source File

SOURCE=.\CatchGridDlg.h
# End Source File
# Begin Source File

SOURCE=.\CCaraFile.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\CEtsDiv.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\CEtsPrtD.h
# End Source File
# Begin Source File

SOURCE=.\CircleLabel.h
# End Source File
# Begin Source File

SOURCE=.\CMargindialogparams.h
# End Source File
# Begin Source File

SOURCE=.\ColorArray.h
# End Source File
# Begin Source File

SOURCE=.\ColorLabel.h
# End Source File
# Begin Source File

SOURCE=.\ColScalePropPage.h
# End Source File
# Begin Source File

SOURCE=.\Curve.h
# End Source File
# Begin Source File

SOURCE=.\CurveColorPropPage.h
# End Source File
# Begin Source File

SOURCE=.\CurveLabel.h
# End Source File
# Begin Source File

SOURCE=.\CurveValuePropPage.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\DibSection.h
# End Source File
# Begin Source File

SOURCE=.\FontPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Label.h
# End Source File
# Begin Source File

SOURCE=.\LabelContainer.h
# End Source File
# Begin Source File

SOURCE=.\LineLabel.h
# End Source File
# Begin Source File

SOURCE=.\LineListBox.h
# End Source File
# Begin Source File

SOURCE=.\LineStylePropPage.h
# End Source File
# Begin Source File

SOURCE=.\ListDlg.h
# End Source File
# Begin Source File

SOURCE=.\MathCurve.h
# End Source File
# Begin Source File

SOURCE=.\PasteSelectivDlg.h
# End Source File
# Begin Source File

SOURCE=.\PictureLabel.h
# End Source File
# Begin Source File

SOURCE=.\PicturePropPage.h
# End Source File
# Begin Source File

SOURCE=.\PlotLabel.h
# End Source File
# Begin Source File

SOURCE=.\PlotRangePropPage.h
# End Source File
# Begin Source File

SOURCE=.\PlotViewPropPage.h
# End Source File
# Begin Source File

SOURCE=.\PreviewFileHeader.h
# End Source File
# Begin Source File

SOURCE=.\PropertyPreview.h
# End Source File
# Begin Source File

SOURCE=.\RectLabel.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScalePainter.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TextEdit.h
# End Source File
# Begin Source File

SOURCE=.\TextLabel.h
# End Source File
# Begin Source File

SOURCE=.\Transformation.h
# End Source File
# Begin Source File

SOURCE=.\TreeDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00021.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CaraLogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CARAPREV.rc2
# End Source File
# Begin Source File

SOURCE=.\res\CaraSdb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\copycurs.cur
# End Source File
# Begin Source File

SOURCE=.\res\copycurv.cur
# End Source File
# Begin Source File

SOURCE=.\res\CopyFist.cur
# End Source File
# Begin Source File

SOURCE=.\res\copyplot.cur
# End Source File
# Begin Source File

SOURCE=.\res\CrBezierCirc.cur
# End Source File
# Begin Source File

SOURCE=.\res\create_l.cur
# End Source File
# Begin Source File

SOURCE=.\res\CreateBezier.cur
# End Source File
# Begin Source File

SOURCE=.\res\CreateCircle.cur
# End Source File
# Begin Source File

SOURCE=.\res\CreateLine.cur
# End Source File
# Begin Source File

SOURCE=.\res\CreateRect.cur
# End Source File
# Begin Source File

SOURCE=.\res\CreateText.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\Edit.cur
# End Source File
# Begin Source File

SOURCE=.\res\handcurs.cur
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

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Magnify.cur
# End Source File
# Begin Source File

SOURCE=.\res\NoDrop.cur
# End Source File
# Begin Source File

SOURCE=.\res\nodropcu.cur
# End Source File
# Begin Source File

SOURCE=.\res\nodroppl.cur
# End Source File
# Begin Source File

SOURCE=.\res\openhand.cur
# End Source File
# Begin Source File

SOURCE=.\res\Pen_l.cur
# End Source File
# Begin Source File

SOURCE=.\res\preview_.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\prt_setu.ico
# End Source File
# Begin Source File

SOURCE=.\res\radiosel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\select.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TrackFist.cur
# End Source File
# Begin Source File

SOURCE=.\res\tracPlot.cur
# End Source File
# Begin Source File

SOURCE=.\res\traCurve.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoom_out.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Dinosaur.ani
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
