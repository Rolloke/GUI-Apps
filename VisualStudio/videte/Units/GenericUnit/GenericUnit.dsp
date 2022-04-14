# Microsoft Developer Studio Project File - Name="GenericUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GenericUnit - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GenericUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GenericUnit.mak" CFG="GenericUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GenericUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GenericUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GenericUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/GenericUnit", YXBCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GenericUnit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Fp".\Release/JaCobUnit.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 cipc.lib oemgui.lib ddraw.lib Jpeglib.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "GenericUnit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W4 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CIPCDebug.lib OemGuiD.lib ddraw.lib Jpeglibd.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "GenericUnit - Win32 Enu"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GenericUnit___Win32_Enu"
# PROP BASE Intermediate_Dir "GenericUnit___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Fp".\Release/JaCobUnit.pch" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Fp".\Release/JaCobUnit.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OemGui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 cipc.lib OemGui.lib ddraw.lib jpeglib.lib /nologo /subsystem:windows /map /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\GenericUnit.exe D:\Dvrt
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "GenericUnit - Win32 Release"
# Name "GenericUnit - Win32 Debug"
# Name "GenericUnit - Win32 Enu"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\CInstanceCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcInputGenericUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CIPCInputGenericUnitMDAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcOutputGenericUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CMotionDetection.cpp
# End Source File
# Begin Source File

SOURCE=.\directdraw.cpp
# End Source File
# Begin Source File

SOURCE=.\directdraw.h
# End Source File
# Begin Source File

SOURCE=.\GenericUnitApp.cpp
# End Source File
# Begin Source File

SOURCE=.\GenericUnitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CCodec.h
# End Source File
# Begin Source File

SOURCE=.\CInstanceCounter.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputGenericUnit.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputGenericUnitMDAlarm.h
# End Source File
# Begin Source File

SOURCE=.\CipcOutputGenericUnit.h
# End Source File
# Begin Source File

SOURCE=.\CMDConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\CMotionDetection.h
# End Source File
# Begin Source File

SOURCE=.\GenericDefs.h
# End Source File
# Begin Source File

SOURCE=.\GenericUnitApp.h
# End Source File
# Begin Source File

SOURCE=.\GenericUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\GenericUnit.rc
# End Source File
# Begin Source File

SOURCE=.\res\GenericUnit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\GenericUnit1.ico
# End Source File
# Begin Source File

SOURCE=.\res\GenericUnit2.ico
# End Source File
# Begin Source File

SOURCE=.\res\GenericUnit3.ico
# End Source File
# Begin Source File

SOURCE=.\GenericUnitenu.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\res\green.bmp
# End Source File
# Begin Source File

SOURCE=.\res\inlay.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MDInlay.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MDmask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\red.bmp
# End Source File
# Begin Source File

SOURCE=.\Testbild1.jpg
# End Source File
# Begin Source File

SOURCE=.\Testbild2.jpg
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\History.txt
# End Source File
# End Group
# Begin Group "Jpeg Files"

# PROP Default_Filter "jpg"
# Begin Source File

SOURCE=.\Camera00.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera01.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera02.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera03.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera04.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera05.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera06.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera07.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera08.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera09.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera10.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera11.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera12.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera13.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera14.jpg
# End Source File
# Begin Source File

SOURCE=.\Camera15.jpg
# End Source File
# End Group
# End Target
# End Project
