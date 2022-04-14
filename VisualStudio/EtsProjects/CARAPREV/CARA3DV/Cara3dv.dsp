# Microsoft Developer Studio Project File - Name="CARA3DV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CARA3DV - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cara3dv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cara3dv.mak" CFG="CARA3DV - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CARA3DV - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CARA3DV - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CARA3DV - Win32 ReleaseEngl" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "AFX_RESOURCE_DLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\Release\EtsPrev.lib opengl32.lib glu32.lib CDibSection.lib CEtsDiv.lib CCaraInfo.lib CEtsLsmb.lib CEtsDebug.lib CEts3DGL.lib EtsBind.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release/ETS3DV.exe" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\Debug\EtsPrev.lib opengl32.lib glu32.lib CDibSection.lib CEtsDiv.lib CCaraInfo.lib CETSLsmb.lib CEtsDebug.lib CEts3DGL.lib ETSBIND.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /nodefaultlib:"LIBCD" /nodefaultlib:"libm" /nodefaultlib:"libmmd" /out:"Debug/ETS3DV.exe" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_ENU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 C:\Daten\MyProjects\CaraPrev\Release\EtsPrev.lib opengl32.lib glu32.lib CDibSection.lib CEtsDiv.lib CCaraInfo.lib CEtsLsmb.lib CEtsDebug.lib CEts3DGL.lib EtsBind.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release/ETS3DV.exe" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\ReleaseEngl\EtsPrev.lib opengl32.lib glu32.lib CDibSection.lib CEtsDiv.lib CCaraInfo.lib CEtsLsmb.lib CEtsDebug.lib CEts3DGL.lib EtsBind.lib /nologo /subsystem:windows /map /machine:I386 /out:"ReleaseEngl/ETS3DV.exe" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ENDIF 

# Begin Target

# Name "CARA3DV - Win32 Release"
# Name "CARA3DV - Win32 Debug"
# Name "CARA3DV - Win32 ReleaseEngl"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\3DGrid.cpp

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BaseClass3D.cpp

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

# ADD CPP /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

# ADD CPP /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CARA3DDocFriend.cpp
# End Source File
# Begin Source File

SOURCE=.\Cara3DGL.cpp

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CARA3DV.cpp
# End Source File
# Begin Source File

SOURCE=.\CARA3DV.rc
# End Source File
# Begin Source File

SOURCE=.\CARA3DVDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CARA3DVView.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorView.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\ETS3DV.hpj

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

USERDEP__ETS3D="$(ProjDir)\hlp\AfxCore.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Release
ProjDir=.
TargetName=ETS3DV
InputPath=.\hlp\ETS3DV.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

USERDEP__ETS3D="$(ProjDir)\hlp\AfxCore.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Debug
ProjDir=.
TargetName=ETS3DV
InputPath=.\hlp\ETS3DV.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

USERDEP__ETS3D="$(ProjDir)\hlp\AfxCore.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\ReleaseEngl
ProjDir=.
TargetName=ETS3DV
InputPath=.\hlp\ETS3DV.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelpEngl.bat"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InfoToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\IpFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelColor.cpp

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

# ADD CPP /G6 /O2 /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LightDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\NonModalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectTransformation.cpp
# End Source File
# Begin Source File

SOURCE=.\Pins.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanGraph.cpp

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

# ADD CPP /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

# ADD CPP /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PolarGraph.cpp

!IF  "$(CFG)" == "CARA3DV - Win32 Release"

# ADD CPP /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 Debug"

!ELSEIF  "$(CFG)" == "CARA3DV - Win32 ReleaseEngl"

# ADD CPP /D "_USE_INTEL_COMPILER"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ScaleView.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\StandardDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ViewParams.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\3DGrid.h
# End Source File
# Begin Source File

SOURCE=.\BaseClass3D.h
# End Source File
# Begin Source File

SOURCE=.\CARA3DDocFriend.h
# End Source File
# Begin Source File

SOURCE=.\Cara3DGL.h
# End Source File
# Begin Source File

SOURCE=.\CARA3DV.h
# End Source File
# Begin Source File

SOURCE=.\CARA3DVDoc.h
# End Source File
# Begin Source File

SOURCE=.\CARA3DVView.h
# End Source File
# Begin Source File

SOURCE=..\CMargindialogparams.h
# End Source File
# Begin Source File

SOURCE=.\ColorView.h
# End Source File
# Begin Source File

SOURCE=.\InfoToolBar.h
# End Source File
# Begin Source File

SOURCE=.\IpFrame.h
# End Source File
# Begin Source File

SOURCE=.\LevelColor.h
# End Source File
# Begin Source File

SOURCE=.\LightDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NonModalDlg.h
# End Source File
# Begin Source File

SOURCE=.\ObjectTransformation.h
# End Source File
# Begin Source File

SOURCE=.\Pins.h
# End Source File
# Begin Source File

SOURCE=.\PlanGraph.h
# End Source File
# Begin Source File

SOURCE=.\PolarGraph.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\resource.hm
# End Source File
# Begin Source File

SOURCE=.\ScaleView.h
# End Source File
# Begin Source File

SOURCE=.\SrvrItem.h
# End Source File
# Begin Source File

SOURCE=.\StandardDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ViewParams.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CARA3DV.ico
# End Source File
# Begin Source File

SOURCE=.\res\CARA3DV.rc2
# End Source File
# Begin Source File

SOURCE=.\res\CARA3DVDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\CARA3DVIEW.ico
# End Source File
# Begin Source File

SOURCE=.\res\CARA3DVIEWDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\CaraColor.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
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

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\Contexthelp.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Dialog.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\res\FilmBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\FuncBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\hcontext.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\Properties.rtf
# End Source File
# Begin Source File

SOURCE=.\resource.hm
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# Begin Source File

SOURCE=.\res\StdBar.bmp
# End Source File
# Begin Source File

SOURCE=..\PrevHelp\WinDlg.rtf
# End Source File
# End Group
# Begin Source File

SOURCE=.\CARA3DV.reg
# End Source File
# Begin Source File

SOURCE=..\CARAPREV.dsw
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
