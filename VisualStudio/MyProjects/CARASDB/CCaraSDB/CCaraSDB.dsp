# Microsoft Developer Studio Project File - Name="CCaraSDB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CCaraSDB - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "CCaraSDB.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "CCaraSDB.mak" CFG="CCaraSDB - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "CCaraSDB - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "CCaraSDB - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CCaraSDB - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
OutDir=.\Release
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Bibliothek
PostBuild_Cmds=copy $(OutDir)\CCARASDB.lib c:\Daten\MyProjects\Libraries\Release	copy $(ProjDir)\CCARASDB.h c:\Daten\MyProjects\Libraries
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CCaraSDB - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
OutDir=.\Debug
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Desc=Kopieren der Bibliothek
PostBuild_Cmds=copy $(OutDir)\CCARASDB.lib c:\Daten\MyProjects\Libraries\Debug	copy $(ProjDir)\CCARASDB.h c:\Daten\MyProjects\Libraries
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CCaraSDB - Win32 Release"
# Name "CCaraSDB - Win32 Debug"
# Begin Source File

SOURCE=.\CCaraSdb.cpp
# End Source File
# Begin Source File

SOURCE=.\CCaraSDB.h
# End Source File
# End Target
# End Project
