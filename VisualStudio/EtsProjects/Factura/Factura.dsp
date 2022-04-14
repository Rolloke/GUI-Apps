# Microsoft Developer Studio Project File - Name="Factura" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Factura - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Factura.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Factura.mak" CFG="Factura - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Factura - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Factura - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Factura - Win32 ReleaseEngl" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Factura - Win32 Release"

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
# ADD CPP /nologo /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\CARAPREV" /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\Release\EtsPrev.lib CEtsDebug.lib EtsBind.lib CEtsHelp.lib /nologo /subsystem:windows /map /machine:I386 /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ELSEIF  "$(CFG)" == "Factura - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Daten\MyProjects\CARAPREV" /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\Debug\EtsPrev.lib CEtsDebug.lib EtsBind.lib CEtsHelp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /pdbtype:sept /libpath:"C:\Daten\MyProjects\Libraries\Debug"

!ELSEIF  "$(CFG)" == "Factura - Win32 ReleaseEngl"

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
# ADD BASE CPP /nologo /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\CARAPREV" /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O1 /I "C:\Daten\MyProjects\CARAPREV" /I "C:\Daten\MyProjects\Libraries" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "AFX_TARG_NEU" /d "AFX_TARG_DEU" /d "AFX_RESOURCE_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 C:\Daten\MyProjects\CaraPrev\Release\EtsPrev.lib /nologo /subsystem:windows /machine:I386 /libpath:"C:\Daten\MyProjects\Libraries\Release"
# ADD LINK32 C:\Daten\MyProjects\CaraPrev\ReleaseEngl\EtsPrev.lib /nologo /subsystem:windows /machine:I386 /libpath:"C:\Daten\MyProjects\Libraries\Release"

!ENDIF 

# Begin Target

# Name "Factura - Win32 Release"
# Name "Factura - Win32 Debug"
# Name "Factura - Win32 ReleaseEngl"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BaseData.cpp
# End Source File
# Begin Source File

SOURCE=.\CurrencyUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\Customer.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomerList.cpp
# End Source File
# Begin Source File

SOURCE=.\EditFormDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Factura.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\Factura.hpj

!IF  "$(CFG)" == "Factura - Win32 Release"

USERDEP__FACTU="$(ProjDir)\hlp\AfxCore.rtf"	"$(ProjDir)\hlp\AfxPrint.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Release
ProjDir=.
TargetName=Factura
InputPath=.\hlp\Factura.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "Factura - Win32 Debug"

USERDEP__FACTU="$(ProjDir)\hlp\AfxCore.rtf"	"$(ProjDir)\hlp\AfxPrint.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\Debug
ProjDir=.
TargetName=Factura
InputPath=.\hlp\Factura.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "Factura - Win32 ReleaseEngl"

USERDEP__FACTU="$(ProjDir)\hlp\AfxCore.rtf"	"$(ProjDir)\hlp\AfxPrint.rtf"	
# Begin Custom Build - Hilfedatei wird erzeugt...
OutDir=.\ReleaseEngl
ProjDir=.
TargetName=Factura
InputPath=.\hlp\Factura.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Factura.rc
# End Source File
# Begin Source File

SOURCE=.\FacturaDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\FacturaView.cpp
# End Source File
# Begin Source File

SOURCE=.\FindCustomer.cpp
# End Source File
# Begin Source File

SOURCE=.\Invoice.cpp
# End Source File
# Begin Source File

SOURCE=.\InvoiceFind.cpp
# End Source File
# Begin Source File

SOURCE=.\LogOn.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\PayMode.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintAdr.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintAscDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintBankKl.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintFromTo.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\Product.cpp
# End Source File
# Begin Source File

SOURCE=.\ProductFind.cpp
# End Source File
# Begin Source File

SOURCE=.\SendEmail.cpp
# End Source File
# Begin Source File

SOURCE=.\States.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BaseData.h
# End Source File
# Begin Source File

SOURCE=.\CurrencyUnit.h
# End Source File
# Begin Source File

SOURCE=.\Customer.h
# End Source File
# Begin Source File

SOURCE=.\CustomerList.h
# End Source File
# Begin Source File

SOURCE=.\EditFormDlg.h
# End Source File
# Begin Source File

SOURCE=.\Factura.h
# End Source File
# Begin Source File

SOURCE=.\FacturaDoc.h
# End Source File
# Begin Source File

SOURCE=.\FacturaView.h
# End Source File
# Begin Source File

SOURCE=.\FindCustomer.h
# End Source File
# Begin Source File

SOURCE=.\Invoice.h
# End Source File
# Begin Source File

SOURCE=.\InvoiceFind.h
# End Source File
# Begin Source File

SOURCE=.\LogOn.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\PayMode.h
# End Source File
# Begin Source File

SOURCE=.\PrintAdr.h
# End Source File
# Begin Source File

SOURCE=.\PrintAscDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrintBankKl.h
# End Source File
# Begin Source File

SOURCE=.\PrintLabel.h
# End Source File
# Begin Source File

SOURCE=.\Product.h
# End Source File
# Begin Source File

SOURCE=.\ProductFind.h
# End Source File
# Begin Source File

SOURCE=.\ResourceF.h
# End Source File
# Begin Source File

SOURCE=.\SendEmail.h
# End Source File
# Begin Source File

SOURCE=.\States.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\CaraSdb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Factura.ico
# End Source File
# Begin Source File

SOURCE=.\res\Factura.rc2
# End Source File
# Begin Source File

SOURCE=.\res\FacturaDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxPrint.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
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

SOURCE=.\hlp\Factura.cnt
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

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
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

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\CCaraInfo.lib
# End Source File
# End Target
# End Project
