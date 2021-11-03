# Microsoft Developer Studio Project File - Name="CARABOX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CARABOX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CARABOX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CARABOX.mak" CFG="CARABOX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CARABOX - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARABOX - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARABOX - Win32 ReleaseEngl" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARABOX - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "CARABOX_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "INCLUDE_ETS_HELP" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib CCaraboxView.lib CCurve.lib CEtsLSMB.lib CCarasdb.lib ETSBIND.lib CEtsHelp.lib CEtsList.lib Cara3Dtr.lib CEtsDebug.lib CEtsDiv.lib wininet.lib CEts3DGL.lib opengl32.lib CEtsDlg.lib ETSRegistry.lib version.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc.lib" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ELSEIF  "$(CFG)" == "CARABOX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CARABOX_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "INCLUDE_ETS_HELP" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib CCaraboxView.lib CCurve.lib CEtsLSMB.lib CCarasdb.lib ETSBIND.lib CEtsHelp.lib CEtsList.lib Cara3Dtr.lib CEtsDebug.lib CEtsDiv.lib wininet.lib CEts3DGL.lib opengl32.lib CEtsDlg.lib ETSRegistry.lib version.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"bcinterf.lib" /nodefaultlib:"libm.lib" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\CaraBox.Dll C:\Daten\MyProjects\Libraries\DebugDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARABOX - Win32 ReleaseEngl"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseEngl"
# PROP BASE Intermediate_Dir "ReleaseEngl"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseEngl"
# PROP Intermediate_Dir "ReleaseEngl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "CARABOX_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_DEU" /D "INCLUDE_ETS_HELP" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\Libraries\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "CARABOX_EXPORTS" /D "AFX_RESOURCE_DLL" /D "AFX_TARG_NEU" /D "AFX_TARG_ENU" /D "INCLUDE_ETS_HELP" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_NEU" /d "AFX_TARG_ENU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib CCaraboxView.lib CCurve.lib CEtsLSMB.lib CCarasdb.lib ETSBIND.lib CEtsHelp.lib CEtsList.lib Cara3Dtr.lib CEtsDebug.lib CEtsDiv.lib wininet.lib CEts3DGL.lib opengl32.lib CEtsDlg.lib ETSRegistry.lib version.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc.lib" /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib CCaraboxView.lib CCurve.lib CEtsLSMB.lib CCarasdb.lib ETSBIND.lib CEtsHelp.lib CEtsList.lib Cara3Dtr.lib CEtsDebug.lib CEtsDiv.lib wininet.lib CEts3DGL.lib opengl32.lib CEtsDlg.lib ETSRegistry.lib version.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc.lib" /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ENDIF 

# Begin Target

# Name "CARABOX - Win32 Release"
# Name "CARABOX - Win32 Debug"
# Name "CARABOX - Win32 ReleaseEngl"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BoxBaseData.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxCabinetDesign.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxChassisData.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxCrossOver.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\BoxStructures.cpp
# End Source File
# Begin Source File

SOURCE=.\C3DViewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CARABOX.cpp
# End Source File
# Begin Source File

SOURCE=.\CARABOX.def
# End Source File
# Begin Source File

SOURCE=.\CCabinet.cpp
# End Source File
# Begin Source File

SOURCE=.\CFilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\COpenGLView.cpp
# End Source File
# Begin Source File

SOURCE=.\CProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CScaleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CSortFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\CSPLViewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Skript1.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BoxBaseData.h
# End Source File
# Begin Source File

SOURCE=.\BoxCabinetDesign.h
# End Source File
# Begin Source File

SOURCE=.\BoxChassisData.h
# End Source File
# Begin Source File

SOURCE=.\BoxCrossOver.h
# End Source File
# Begin Source File

SOURCE=.\BoxDescription.h
# End Source File
# Begin Source File

SOURCE=.\BoxFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\Boxglobal.h
# End Source File
# Begin Source File

SOURCE=.\BoxPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\BoxStructures.h
# End Source File
# Begin Source File

SOURCE=.\C3DViewDlg.h
# End Source File
# Begin Source File

SOURCE=.\CCabinet.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\CEtsList.h
# End Source File
# Begin Source File

SOURCE=.\CFilterDlg.h
# End Source File
# Begin Source File

SOURCE=.\COpenGLView.h
# End Source File
# Begin Source File

SOURCE=.\CProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\CScaleDlg.h
# End Source File
# Begin Source File

SOURCE=.\CSortFunctions.h
# End Source File
# Begin Source File

SOURCE=.\CSPLViewDlg.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\Curve.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\ETS3DGL.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\FileHeader.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\MathCurve.h
# End Source File
# Begin Source File

SOURCE=..\Libraries\Plot.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\alphablend.ico
# End Source File
# Begin Source File

SOURCE=.\res\bandpass.ico
# End Source File
# Begin Source File

SOURCE=.\res\bkcolor.ico
# End Source File
# Begin Source File

SOURCE=.\res\btncolor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cab_deu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cab_deu1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\colbox.ico
# End Source File
# Begin Source File

SOURCE=.\res\colchassis.ico
# End Source File
# Begin Source File

SOURCE=.\res\colscale.ico
# End Source File
# Begin Source File

SOURCE=.\res\frqu_0.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hipass.ico
# End Source File
# Begin Source File

SOURCE=.\res\Icon.ico
# End Source File
# Begin Source File

SOURCE=.\res\idc_3d_p.ico
# End Source File
# Begin Source File

SOURCE=.\res\liteblue.ico
# End Source File
# Begin Source File

SOURCE=.\res\litecuto.ico
# End Source File
# Begin Source File

SOURCE=.\res\litedst.ico
# End Source File
# Begin Source File

SOURCE=.\res\liteexp.ico
# End Source File
# Begin Source File

SOURCE=.\res\litegreen.ico
# End Source File
# Begin Source File

SOURCE=.\res\liteonoff.ico
# End Source File
# Begin Source File

SOURCE=.\res\litephi.ico
# End Source File
# Begin Source File

SOURCE=.\res\litered.ico
# End Source File
# Begin Source File

SOURCE=.\res\liteteta.ico
# End Source File
# Begin Source File

SOURCE=.\res\lopass.ico
# End Source File
# Begin Source File

SOURCE=.\res\setproj.ico
# End Source File
# Begin Source File

SOURCE=.\res\shinines.ico
# End Source File
# Begin Source File

SOURCE=.\res\tran_x.ico
# End Source File
# Begin Source File

SOURCE=.\res\tran_y.ico
# End Source File
# Begin Source File

SOURCE=.\res\tran_z.ico
# End Source File
# Begin Source File

SOURCE=.\res\turnphi.ico
# End Source File
# Begin Source File

SOURCE=.\res\turnteta.ico
# End Source File
# Begin Source File

SOURCE=.\res\view_xy.ico
# End Source File
# Begin Source File

SOURCE=.\res\view_z.ico
# End Source File
# Begin Source File

SOURCE=.\res\zoom.ico
# End Source File
# End Group
# End Target
# End Project
