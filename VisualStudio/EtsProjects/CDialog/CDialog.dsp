# Microsoft Developer Studio Project File - Name="CDialog" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CDialog - Win32 Debug WinHelp
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "CDialog.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "CDialog.mak" CFG="CDialog - Win32 Debug WinHelp"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "CDialog - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "CDialog - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "CDialog - Win32 Debug WinHelp" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "CDialog - Win32 Release WinHelp" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CDialog - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "INCLUDE_ETS_HELP" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\CEtsDlg.lib"
# Begin Special Build Tool
OutDir=.\Release
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Bibliothek
PostBuild_Cmds=copy $(OutDir)\CEtsDlg.lib c:\Daten\MyProjects\Libraries\Release	copy $(ProjDir)\CEtsDlg.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsFileDlg.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertyPage.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertySheet.h c:\Daten\MyProjects\Libraries
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CDialog - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "INCLUDE_ETS_HELP" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\CEtsDlg.lib"
# Begin Special Build Tool
OutDir=.\Debug
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Bibliothek
PostBuild_Cmds=copy $(OutDir)\CEtsDlg.lib c:\Daten\MyProjects\Libraries\Debug	copy $(ProjDir)\CEtsDlg.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertyPage.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertySheet.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsFileDlg.h c:\Daten\MyProjects\Libraries
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CDialog - Win32 Debug WinHelp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_WinHelp"
# PROP BASE Intermediate_Dir "Debug_WinHelp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_WinHelp"
# PROP Intermediate_Dir "Debug_WinHelp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\CEtsDlgW.lib"
# ADD LIB32 /nologo /out:"Debug\CEtsDlgW.lib"
# Begin Special Build Tool
OutDir=.\Debug_WinHelp
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Bibliothek
PostBuild_Cmds=copy $(OutDir)\CEtsDlgW.lib c:\Daten\MyProjects\Libraries\Debug	copy $(ProjDir)\CEtsDlg.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertyPage.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertySheet.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsFileDlg.h c:\Daten\MyProjects\Libraries
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CDialog - Win32 Release WinHelp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_WinHelp"
# PROP BASE Intermediate_Dir "Release_WinHelp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_WinHelp"
# PROP Intermediate_Dir "Release_WinHelp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release\CEtsDlgW.lib"
# ADD LIB32 /nologo /out:"Release\CEtsDlgW.lib"
# Begin Special Build Tool
OutDir=.\Release_WinHelp
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Bibliothek
PostBuild_Cmds=copy $(OutDir)\CEtsDlgW.lib c:\Daten\MyProjects\Libraries\Release	copy $(ProjDir)\CEtsDlg.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsFileDlg.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertyPage.h c:\Daten\MyProjects\Libraries	copy $(ProjDir)\CEtsPropertySheet.h c:\Daten\MyProjects\Libraries
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CDialog - Win32 Release"
# Name "CDialog - Win32 Debug"
# Name "CDialog - Win32 Debug WinHelp"
# Name "CDialog - Win32 Release WinHelp"
# Begin Source File

SOURCE=.\CDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CEtsDlg.h
# End Source File
# Begin Source File

SOURCE=.\CEtsFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CEtsFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\CEtsPropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CEtsPropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\CEtsPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CEtsPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# End Target
# End Project
