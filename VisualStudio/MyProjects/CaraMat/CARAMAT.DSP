# Microsoft Developer Studio Project File - Name="CARAMAT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CARAMAT - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "CARAMAT.MAK".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "CARAMAT.MAK" CFG="CARAMAT - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "CARAMAT - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARAMAT - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CARAMAT - Win32 EngRelease" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CARAMAT - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\CaraMat.Dll C:\Daten\MyProjects\Libraries\ReleaseDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARAMAT - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_DEU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib CEtsHelp.lib ETSBIND.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\CaraMat.Dll C:\Daten\MyProjects\Libraries\DebugDll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CARAMAT - Win32 EngRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CARAMAT___Win32_EngRelease"
# PROP BASE Intermediate_Dir "CARAMAT___Win32_EngRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EngRelease"
# PROP Intermediate_Dir "EngRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AFX_RESOURCE_DLL" /d "AFX_TARG_ENU" /d "AFX_TARG_NEU"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\EngRelease
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\CaraMat.Dll C:\Daten\MyProjects\Libraries\ReleaseEnglDll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CARAMAT - Win32 Release"
# Name "CARAMAT - Win32 Debug"
# Name "CARAMAT - Win32 EngRelease"
# Begin Group "Bitmaps"

# PROP Default_Filter "bmp"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap10.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap11.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap12.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap13.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap14.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap15.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap16.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap17.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap18.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap19.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap20.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap21.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap22.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap23.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap24.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap25.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap26.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap27.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap28.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap29.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap30.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap31.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap32.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap33.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap34.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap35.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap36.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap37.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap38.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap39.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap40.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap41.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap42.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap43.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap44.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap45.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap46.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap47.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap48.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap49.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap50.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap51.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap52.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap53.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap54.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap55.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap56.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap57.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap58.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap59.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap6.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap60.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap61.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap62.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap63.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap64.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap65.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap66.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap67.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap68.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap69.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap7.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap70.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap71.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap72.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap73.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap74.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap75.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap76.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap77.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap78.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap79.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap8.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap80.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap81.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap82.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap83.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap84.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap85.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap86.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap87.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap88.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap89.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap9.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap90.bmp
# End Source File
# Begin Source File

SOURCE=.\NoBMP08.bmp
# End Source File
# Begin Source File

SOURCE=.\NoBMP32.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\CARAMAT.cpp
# End Source File
# Begin Source File

SOURCE=.\Caramat.def
# End Source File
# Begin Source File

SOURCE=.\CEtsHelp.h
# End Source File
# Begin Source File

SOURCE=.\FileHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\FileHeader.h
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Skript1.rc
# End Source File
# Begin Source File

SOURCE=.\CEtsHelp.lib
# End Source File
# Begin Source File

SOURCE=.\ETSBIND.lib
# End Source File
# End Target
# End Project
