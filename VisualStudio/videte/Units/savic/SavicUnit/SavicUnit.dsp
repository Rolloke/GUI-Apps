# Microsoft Developer Studio Project File - Name="SavicUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SavicUnit - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SavicUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SavicUnit.mak" CFG="SavicUnit - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SavicUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SavicUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SavicUnit - Win32 Enu" (based on "Win32 (x86) Application")
!MESSAGE "SavicUnit - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "SavicUnit - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Units/Savic/SavicUnit", UDRBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Fp".\Release/SavicUnit.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 CIPC.lib OemGui.lib WKClasses.lib Jpeglib.lib /nologo /subsystem:windows /map /machine:I386
# Begin Special Build Tool
TargetPath=.\Release\SavicUnit.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV /R /C /F /Y	xcopy $(TargetPath) c:\Security /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

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
# ADD LINK32 CIPCDebug.lib OemGuiD.lib WKClassesDebug.lib jpeglibd.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\Debug\SavicUnit.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV /R /C /F /Y	xcopy $(TargetPath) c:\Security /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SavicUnit___Win32_Enu"
# PROP BASE Intermediate_Dir "SavicUnit___Win32_Enu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Enu"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Fp".\Release/SavicUnit.pch" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Fp".\Release/SavicUnit.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OemGui.lib WKClasses.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 CIPC.lib OemGui.lib WKClasses.lib Jpeglib.lib /nologo /subsystem:windows /map /machine:I386
# Begin Special Build Tool
TargetPath=.\Enu\SavicUnit.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DV /R /C /F /Y	xcopy $(TargetPath) c:\Security /R /C /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SavicUnit___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "SavicUnit___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MDd /W4 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W4 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPCDebug.lib OemGuiD.lib WKClassesDebug.lib jpeglibd.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CIPCDebugU.lib OemGuiDU.lib WKClassesDebugU.lib jpeglibdU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /debug /machine:I386 /pdbtype:sept /libpath:"v:\project\cipc\debugunicode"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SavicUnit___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "SavicUnit___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Fp".\Release/SavicUnit.pch" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Fp".\Release/SavicUnit.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CIPC.lib OemGui.lib WKClasses.lib Jpeglib.lib /nologo /subsystem:windows /map /machine:I386
# ADD LINK32 CIPCU.lib OemGuiU.lib WKClassesU.lib JpeglibU.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /libpath:"v:\project\cipc\ReleaseUnicode"
# Begin Special Build Tool
TargetPath=.\ReleaseUnicode\SavicUnit.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) c:\DVX /R /C /F /Y	xcopy $(TargetPath) c:\SecurityU /R /C /F /Y
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "SavicUnit - Win32 Release"
# Name "SavicUnit - Win32 Debug"
# Name "SavicUnit - Win32 Enu"
# Name "SavicUnit - Win32 DebugUnicode"
# Name "SavicUnit - Win32 ReleaseUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CInstanceCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcInputSavicUnit.cpp

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CIPCInputSavicUnitMDAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\CipcOutputSavicUnit.cpp

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CMyDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\CMySavic.cpp
# End Source File
# Begin Source File

SOURCE=.\CSavicException.cpp

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CVideoControl.cpp

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SavicUnitApp.cpp

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SavicUnitDlg.cpp

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VideoInlayWindow.cpp

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CInstanceCounter.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputSavicUnit.h
# End Source File
# Begin Source File

SOURCE=.\CIPCInputSavicUnitMDAlarm.h
# End Source File
# Begin Source File

SOURCE=.\CipcOutputSavicUnit.h
# End Source File
# Begin Source File

SOURCE=.\CMyDebug.h
# End Source File
# Begin Source File

SOURCE=.\CMySavic.h
# End Source File
# Begin Source File

SOURCE=.\CSavicException.h
# End Source File
# Begin Source File

SOURCE=.\CVideoControl.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SavicUnitApp.h
# End Source File
# Begin Source File

SOURCE=.\SavicUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\VideoInlayWindow.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\SavicUnit.rc

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\SavicUnit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SavicUnit1.ico
# End Source File
# Begin Source File

SOURCE=.\res\SavicUnit2.ico
# End Source File
# Begin Source File

SOURCE=.\res\SavicUnit3.ico
# End Source File
# Begin Source File

SOURCE=.\SavicUnitenu.rc

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

# PROP Intermediate_Dir "Release"
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Testbild1.jpg
# End Source File
# Begin Source File

SOURCE=.\Testbild2.jpg
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=..\SavicDll\Debug\SavicD.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SavicDll\Release\Savic.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SavicDA\Release\SavicDA.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SavicDA\Debug\SavicDAD.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SavicDADU.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SavicDAU.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\savicdll\DebugUnicode\SavicDU.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\savicdll\ReleaseUnicode\SavicU.lib

!IF  "$(CFG)" == "SavicUnit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 Enu"

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 DebugUnicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SavicUnit - Win32 ReleaseUnicode"

!ENDIF 

# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\SavicUnitenu.lng
# End Source File
# End Group
# End Target
# End Project
