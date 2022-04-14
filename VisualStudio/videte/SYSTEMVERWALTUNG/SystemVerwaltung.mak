# Microsoft Developer Studio Generated NMAKE File, Based on SystemVerwaltung.dsp
!IF "$(CFG)" == ""
CFG=SystemVerwaltung - Win32 Release
!MESSAGE No configuration specified. Defaulting to SystemVerwaltung - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "SystemVerwaltung - Win32 Release" && "$(CFG)" != "SystemVerwaltung - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SystemVerwaltung.mak" CFG="SystemVerwaltung - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SystemVerwaltung - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SystemVerwaltung - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\SystemVerwaltung.exe" "$(OUTDIR)\SystemVerwaltung.bsc"


CLEAN :
	-@erase "$(INTDIR)\activationpage.obj"
	-@erase "$(INTDIR)\activationpage.sbr"
	-@erase "$(INTDIR)\AKUPage.obj"
	-@erase "$(INTDIR)\AKUPage.sbr"
	-@erase "$(INTDIR)\AppPage.obj"
	-@erase "$(INTDIR)\AppPage.sbr"
	-@erase "$(INTDIR)\callpage.obj"
	-@erase "$(INTDIR)\callpage.sbr"
	-@erase "$(INTDIR)\CoCoPage.obj"
	-@erase "$(INTDIR)\CoCoPage.sbr"
	-@erase "$(INTDIR)\ComConfigurationDialog.obj"
	-@erase "$(INTDIR)\ComConfigurationDialog.sbr"
	-@erase "$(INTDIR)\FieldMapPage.obj"
	-@erase "$(INTDIR)\FieldMapPage.sbr"
	-@erase "$(INTDIR)\GAAPage.obj"
	-@erase "$(INTDIR)\GAAPage.sbr"
	-@erase "$(INTDIR)\hardwarepage.obj"
	-@erase "$(INTDIR)\hardwarepage.sbr"
	-@erase "$(INTDIR)\HostPage.obj"
	-@erase "$(INTDIR)\HostPage.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\InputDialog.obj"
	-@erase "$(INTDIR)\InputDialog.sbr"
	-@erase "$(INTDIR)\inputgroup.obj"
	-@erase "$(INTDIR)\inputgroup.sbr"
	-@erase "$(INTDIR)\inputlist.obj"
	-@erase "$(INTDIR)\inputlist.sbr"
	-@erase "$(INTDIR)\inputpage.obj"
	-@erase "$(INTDIR)\inputpage.sbr"
	-@erase "$(INTDIR)\InstructionDialog.obj"
	-@erase "$(INTDIR)\InstructionDialog.sbr"
	-@erase "$(INTDIR)\ISDNPage.obj"
	-@erase "$(INTDIR)\ISDNPage.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MESZPage.obj"
	-@erase "$(INTDIR)\MESZPage.sbr"
	-@erase "$(INTDIR)\MicoPage.obj"
	-@erase "$(INTDIR)\MicoPage.sbr"
	-@erase "$(INTDIR)\NTLoginPage.obj"
	-@erase "$(INTDIR)\NTLoginPage.sbr"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\output.sbr"
	-@erase "$(INTDIR)\OutputDialog.obj"
	-@erase "$(INTDIR)\OutputDialog.sbr"
	-@erase "$(INTDIR)\outputgroup.obj"
	-@erase "$(INTDIR)\outputgroup.sbr"
	-@erase "$(INTDIR)\outputlist.obj"
	-@erase "$(INTDIR)\outputlist.sbr"
	-@erase "$(INTDIR)\outputpage.obj"
	-@erase "$(INTDIR)\outputpage.sbr"
	-@erase "$(INTDIR)\passwordcheckdialog.obj"
	-@erase "$(INTDIR)\passwordcheckdialog.sbr"
	-@erase "$(INTDIR)\passworddialog.obj"
	-@erase "$(INTDIR)\passworddialog.sbr"
	-@erase "$(INTDIR)\permissionpage.obj"
	-@erase "$(INTDIR)\permissionpage.sbr"
	-@erase "$(INTDIR)\processlist.obj"
	-@erase "$(INTDIR)\processlist.sbr"
	-@erase "$(INTDIR)\PTUnitPage.obj"
	-@erase "$(INTDIR)\PTUnitPage.sbr"
	-@erase "$(INTDIR)\RelayDialog.obj"
	-@erase "$(INTDIR)\RelayDialog.sbr"
	-@erase "$(INTDIR)\RelayPage.obj"
	-@erase "$(INTDIR)\RelayPage.sbr"
	-@erase "$(INTDIR)\RS232Page.obj"
	-@erase "$(INTDIR)\RS232Page.sbr"
	-@erase "$(INTDIR)\savepage.obj"
	-@erase "$(INTDIR)\savepage.sbr"
	-@erase "$(INTDIR)\SDIConfigurationDialog.obj"
	-@erase "$(INTDIR)\SDIConfigurationDialog.sbr"
	-@erase "$(INTDIR)\SDIConfigurationIbmRS232.obj"
	-@erase "$(INTDIR)\SDIConfigurationIbmRS232.sbr"
	-@erase "$(INTDIR)\SDIDialog.obj"
	-@erase "$(INTDIR)\SDIDialog.sbr"
	-@erase "$(INTDIR)\SDIPage.obj"
	-@erase "$(INTDIR)\SDIPage.sbr"
	-@erase "$(INTDIR)\selectinputdlg.obj"
	-@erase "$(INTDIR)\selectinputdlg.sbr"
	-@erase "$(INTDIR)\SimPage.obj"
	-@erase "$(INTDIR)\SimPage.sbr"
	-@erase "$(INTDIR)\SocketPage.obj"
	-@erase "$(INTDIR)\SocketPage.sbr"
	-@erase "$(INTDIR)\SoftwarePage.obj"
	-@erase "$(INTDIR)\SoftwarePage.sbr"
	-@erase "$(INTDIR)\SpecialCharactersDialog.obj"
	-@erase "$(INTDIR)\SpecialCharactersDialog.sbr"
	-@erase "$(INTDIR)\statuslist.obj"
	-@erase "$(INTDIR)\statuslist.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\STMPage.obj"
	-@erase "$(INTDIR)\STMPage.sbr"
	-@erase "$(INTDIR)\SVDoc.obj"
	-@erase "$(INTDIR)\SVDoc.sbr"
	-@erase "$(INTDIR)\SVPage.obj"
	-@erase "$(INTDIR)\SVPage.sbr"
	-@erase "$(INTDIR)\SVTree.obj"
	-@erase "$(INTDIR)\SVTree.sbr"
	-@erase "$(INTDIR)\SVView.obj"
	-@erase "$(INTDIR)\SVView.sbr"
	-@erase "$(INTDIR)\SystemVerwaltung.obj"
	-@erase "$(INTDIR)\SystemVerwaltung.pch"
	-@erase "$(INTDIR)\SystemVerwaltung.res"
	-@erase "$(INTDIR)\SystemVerwaltung.sbr"
	-@erase "$(INTDIR)\timerlist.obj"
	-@erase "$(INTDIR)\timerlist.sbr"
	-@erase "$(INTDIR)\timerpage.obj"
	-@erase "$(INTDIR)\timerpage.sbr"
	-@erase "$(INTDIR)\treelist.obj"
	-@erase "$(INTDIR)\treelist.sbr"
	-@erase "$(INTDIR)\userpage.obj"
	-@erase "$(INTDIR)\userpage.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\SystemVerwaltung.bsc"
	-@erase "$(OUTDIR)\SystemVerwaltung.exe"
	-@erase "$(OUTDIR)\SystemVerwaltung.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SystemVerwaltung.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\SystemVerwaltung.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SystemVerwaltung.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\activationpage.sbr" \
	"$(INTDIR)\AKUPage.sbr" \
	"$(INTDIR)\AppPage.sbr" \
	"$(INTDIR)\callpage.sbr" \
	"$(INTDIR)\CoCoPage.sbr" \
	"$(INTDIR)\GAAPage.sbr" \
	"$(INTDIR)\hardwarepage.sbr" \
	"$(INTDIR)\HostPage.sbr" \
	"$(INTDIR)\inputpage.sbr" \
	"$(INTDIR)\ISDNPage.sbr" \
	"$(INTDIR)\MESZPage.sbr" \
	"$(INTDIR)\MicoPage.sbr" \
	"$(INTDIR)\NTLoginPage.sbr" \
	"$(INTDIR)\outputpage.sbr" \
	"$(INTDIR)\permissionpage.sbr" \
	"$(INTDIR)\PTUnitPage.sbr" \
	"$(INTDIR)\RelayPage.sbr" \
	"$(INTDIR)\RS232Page.sbr" \
	"$(INTDIR)\savepage.sbr" \
	"$(INTDIR)\SDIPage.sbr" \
	"$(INTDIR)\SimPage.sbr" \
	"$(INTDIR)\SocketPage.sbr" \
	"$(INTDIR)\SoftwarePage.sbr" \
	"$(INTDIR)\STMPage.sbr" \
	"$(INTDIR)\timerpage.sbr" \
	"$(INTDIR)\userpage.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\inputgroup.sbr" \
	"$(INTDIR)\inputlist.sbr" \
	"$(INTDIR)\output.sbr" \
	"$(INTDIR)\outputgroup.sbr" \
	"$(INTDIR)\outputlist.sbr" \
	"$(INTDIR)\processlist.sbr" \
	"$(INTDIR)\timerlist.sbr" \
	"$(INTDIR)\ComConfigurationDialog.sbr" \
	"$(INTDIR)\InputDialog.sbr" \
	"$(INTDIR)\InstructionDialog.sbr" \
	"$(INTDIR)\OutputDialog.sbr" \
	"$(INTDIR)\passwordcheckdialog.sbr" \
	"$(INTDIR)\passworddialog.sbr" \
	"$(INTDIR)\RelayDialog.sbr" \
	"$(INTDIR)\SDIConfigurationDialog.sbr" \
	"$(INTDIR)\SDIDialog.sbr" \
	"$(INTDIR)\selectinputdlg.sbr" \
	"$(INTDIR)\FieldMapPage.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\SpecialCharactersDialog.sbr" \
	"$(INTDIR)\statuslist.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SVDoc.sbr" \
	"$(INTDIR)\SVPage.sbr" \
	"$(INTDIR)\SVTree.sbr" \
	"$(INTDIR)\SVView.sbr" \
	"$(INTDIR)\SystemVerwaltung.sbr" \
	"$(INTDIR)\treelist.sbr" \
	"$(INTDIR)\SDIConfigurationIbmRS232.sbr"

"$(OUTDIR)\SystemVerwaltung.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=cipc.lib oemgui.lib WKClasses.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\SystemVerwaltung.pdb" /map:"$(INTDIR)\SystemVerwaltung.map" /machine:I386 /out:"$(OUTDIR)\SystemVerwaltung.exe" 
LINK32_OBJS= \
	"$(INTDIR)\activationpage.obj" \
	"$(INTDIR)\AKUPage.obj" \
	"$(INTDIR)\AppPage.obj" \
	"$(INTDIR)\callpage.obj" \
	"$(INTDIR)\CoCoPage.obj" \
	"$(INTDIR)\GAAPage.obj" \
	"$(INTDIR)\hardwarepage.obj" \
	"$(INTDIR)\HostPage.obj" \
	"$(INTDIR)\inputpage.obj" \
	"$(INTDIR)\ISDNPage.obj" \
	"$(INTDIR)\MESZPage.obj" \
	"$(INTDIR)\MicoPage.obj" \
	"$(INTDIR)\NTLoginPage.obj" \
	"$(INTDIR)\outputpage.obj" \
	"$(INTDIR)\permissionpage.obj" \
	"$(INTDIR)\PTUnitPage.obj" \
	"$(INTDIR)\RelayPage.obj" \
	"$(INTDIR)\RS232Page.obj" \
	"$(INTDIR)\savepage.obj" \
	"$(INTDIR)\SDIPage.obj" \
	"$(INTDIR)\SimPage.obj" \
	"$(INTDIR)\SocketPage.obj" \
	"$(INTDIR)\SoftwarePage.obj" \
	"$(INTDIR)\STMPage.obj" \
	"$(INTDIR)\timerpage.obj" \
	"$(INTDIR)\userpage.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\inputgroup.obj" \
	"$(INTDIR)\inputlist.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\outputgroup.obj" \
	"$(INTDIR)\outputlist.obj" \
	"$(INTDIR)\processlist.obj" \
	"$(INTDIR)\timerlist.obj" \
	"$(INTDIR)\ComConfigurationDialog.obj" \
	"$(INTDIR)\InputDialog.obj" \
	"$(INTDIR)\InstructionDialog.obj" \
	"$(INTDIR)\OutputDialog.obj" \
	"$(INTDIR)\passwordcheckdialog.obj" \
	"$(INTDIR)\passworddialog.obj" \
	"$(INTDIR)\RelayDialog.obj" \
	"$(INTDIR)\SDIConfigurationDialog.obj" \
	"$(INTDIR)\SDIDialog.obj" \
	"$(INTDIR)\selectinputdlg.obj" \
	"$(INTDIR)\FieldMapPage.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\SpecialCharactersDialog.obj" \
	"$(INTDIR)\statuslist.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SVDoc.obj" \
	"$(INTDIR)\SVPage.obj" \
	"$(INTDIR)\SVTree.obj" \
	"$(INTDIR)\SVView.obj" \
	"$(INTDIR)\SystemVerwaltung.obj" \
	"$(INTDIR)\treelist.obj" \
	"$(INTDIR)\SystemVerwaltung.res" \
	"$(INTDIR)\SDIConfigurationIbmRS232.obj"

"$(OUTDIR)\SystemVerwaltung.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\SystemVerwaltung.exe" "$(OUTDIR)\SystemVerwaltung.bsc"


CLEAN :
	-@erase "$(INTDIR)\activationpage.obj"
	-@erase "$(INTDIR)\activationpage.sbr"
	-@erase "$(INTDIR)\AKUPage.obj"
	-@erase "$(INTDIR)\AKUPage.sbr"
	-@erase "$(INTDIR)\AppPage.obj"
	-@erase "$(INTDIR)\AppPage.sbr"
	-@erase "$(INTDIR)\callpage.obj"
	-@erase "$(INTDIR)\callpage.sbr"
	-@erase "$(INTDIR)\CoCoPage.obj"
	-@erase "$(INTDIR)\CoCoPage.sbr"
	-@erase "$(INTDIR)\ComConfigurationDialog.obj"
	-@erase "$(INTDIR)\ComConfigurationDialog.sbr"
	-@erase "$(INTDIR)\FieldMapPage.obj"
	-@erase "$(INTDIR)\FieldMapPage.sbr"
	-@erase "$(INTDIR)\GAAPage.obj"
	-@erase "$(INTDIR)\GAAPage.sbr"
	-@erase "$(INTDIR)\hardwarepage.obj"
	-@erase "$(INTDIR)\hardwarepage.sbr"
	-@erase "$(INTDIR)\HostPage.obj"
	-@erase "$(INTDIR)\HostPage.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\InputDialog.obj"
	-@erase "$(INTDIR)\InputDialog.sbr"
	-@erase "$(INTDIR)\inputgroup.obj"
	-@erase "$(INTDIR)\inputgroup.sbr"
	-@erase "$(INTDIR)\inputlist.obj"
	-@erase "$(INTDIR)\inputlist.sbr"
	-@erase "$(INTDIR)\inputpage.obj"
	-@erase "$(INTDIR)\inputpage.sbr"
	-@erase "$(INTDIR)\InstructionDialog.obj"
	-@erase "$(INTDIR)\InstructionDialog.sbr"
	-@erase "$(INTDIR)\ISDNPage.obj"
	-@erase "$(INTDIR)\ISDNPage.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MESZPage.obj"
	-@erase "$(INTDIR)\MESZPage.sbr"
	-@erase "$(INTDIR)\MicoPage.obj"
	-@erase "$(INTDIR)\MicoPage.sbr"
	-@erase "$(INTDIR)\NTLoginPage.obj"
	-@erase "$(INTDIR)\NTLoginPage.sbr"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\output.sbr"
	-@erase "$(INTDIR)\OutputDialog.obj"
	-@erase "$(INTDIR)\OutputDialog.sbr"
	-@erase "$(INTDIR)\outputgroup.obj"
	-@erase "$(INTDIR)\outputgroup.sbr"
	-@erase "$(INTDIR)\outputlist.obj"
	-@erase "$(INTDIR)\outputlist.sbr"
	-@erase "$(INTDIR)\outputpage.obj"
	-@erase "$(INTDIR)\outputpage.sbr"
	-@erase "$(INTDIR)\passwordcheckdialog.obj"
	-@erase "$(INTDIR)\passwordcheckdialog.sbr"
	-@erase "$(INTDIR)\passworddialog.obj"
	-@erase "$(INTDIR)\passworddialog.sbr"
	-@erase "$(INTDIR)\permissionpage.obj"
	-@erase "$(INTDIR)\permissionpage.sbr"
	-@erase "$(INTDIR)\processlist.obj"
	-@erase "$(INTDIR)\processlist.sbr"
	-@erase "$(INTDIR)\PTUnitPage.obj"
	-@erase "$(INTDIR)\PTUnitPage.sbr"
	-@erase "$(INTDIR)\RelayDialog.obj"
	-@erase "$(INTDIR)\RelayDialog.sbr"
	-@erase "$(INTDIR)\RelayPage.obj"
	-@erase "$(INTDIR)\RelayPage.sbr"
	-@erase "$(INTDIR)\RS232Page.obj"
	-@erase "$(INTDIR)\RS232Page.sbr"
	-@erase "$(INTDIR)\savepage.obj"
	-@erase "$(INTDIR)\savepage.sbr"
	-@erase "$(INTDIR)\SDIConfigurationDialog.obj"
	-@erase "$(INTDIR)\SDIConfigurationDialog.sbr"
	-@erase "$(INTDIR)\SDIConfigurationIbmRS232.obj"
	-@erase "$(INTDIR)\SDIConfigurationIbmRS232.sbr"
	-@erase "$(INTDIR)\SDIDialog.obj"
	-@erase "$(INTDIR)\SDIDialog.sbr"
	-@erase "$(INTDIR)\SDIPage.obj"
	-@erase "$(INTDIR)\SDIPage.sbr"
	-@erase "$(INTDIR)\selectinputdlg.obj"
	-@erase "$(INTDIR)\selectinputdlg.sbr"
	-@erase "$(INTDIR)\SimPage.obj"
	-@erase "$(INTDIR)\SimPage.sbr"
	-@erase "$(INTDIR)\SocketPage.obj"
	-@erase "$(INTDIR)\SocketPage.sbr"
	-@erase "$(INTDIR)\SoftwarePage.obj"
	-@erase "$(INTDIR)\SoftwarePage.sbr"
	-@erase "$(INTDIR)\SpecialCharactersDialog.obj"
	-@erase "$(INTDIR)\SpecialCharactersDialog.sbr"
	-@erase "$(INTDIR)\statuslist.obj"
	-@erase "$(INTDIR)\statuslist.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\STMPage.obj"
	-@erase "$(INTDIR)\STMPage.sbr"
	-@erase "$(INTDIR)\SVDoc.obj"
	-@erase "$(INTDIR)\SVDoc.sbr"
	-@erase "$(INTDIR)\SVPage.obj"
	-@erase "$(INTDIR)\SVPage.sbr"
	-@erase "$(INTDIR)\SVTree.obj"
	-@erase "$(INTDIR)\SVTree.sbr"
	-@erase "$(INTDIR)\SVView.obj"
	-@erase "$(INTDIR)\SVView.sbr"
	-@erase "$(INTDIR)\SystemVerwaltung.obj"
	-@erase "$(INTDIR)\SystemVerwaltung.pch"
	-@erase "$(INTDIR)\SystemVerwaltung.res"
	-@erase "$(INTDIR)\SystemVerwaltung.sbr"
	-@erase "$(INTDIR)\timerlist.obj"
	-@erase "$(INTDIR)\timerlist.sbr"
	-@erase "$(INTDIR)\timerpage.obj"
	-@erase "$(INTDIR)\timerpage.sbr"
	-@erase "$(INTDIR)\treelist.obj"
	-@erase "$(INTDIR)\treelist.sbr"
	-@erase "$(INTDIR)\userpage.obj"
	-@erase "$(INTDIR)\userpage.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SystemVerwaltung.bsc"
	-@erase "$(OUTDIR)\SystemVerwaltung.exe"
	-@erase "$(OUTDIR)\SystemVerwaltung.ilk"
	-@erase "$(OUTDIR)\SystemVerwaltung.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SystemVerwaltung.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\SystemVerwaltung.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SystemVerwaltung.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\activationpage.sbr" \
	"$(INTDIR)\AKUPage.sbr" \
	"$(INTDIR)\AppPage.sbr" \
	"$(INTDIR)\callpage.sbr" \
	"$(INTDIR)\CoCoPage.sbr" \
	"$(INTDIR)\GAAPage.sbr" \
	"$(INTDIR)\hardwarepage.sbr" \
	"$(INTDIR)\HostPage.sbr" \
	"$(INTDIR)\inputpage.sbr" \
	"$(INTDIR)\ISDNPage.sbr" \
	"$(INTDIR)\MESZPage.sbr" \
	"$(INTDIR)\MicoPage.sbr" \
	"$(INTDIR)\NTLoginPage.sbr" \
	"$(INTDIR)\outputpage.sbr" \
	"$(INTDIR)\permissionpage.sbr" \
	"$(INTDIR)\PTUnitPage.sbr" \
	"$(INTDIR)\RelayPage.sbr" \
	"$(INTDIR)\RS232Page.sbr" \
	"$(INTDIR)\savepage.sbr" \
	"$(INTDIR)\SDIPage.sbr" \
	"$(INTDIR)\SimPage.sbr" \
	"$(INTDIR)\SocketPage.sbr" \
	"$(INTDIR)\SoftwarePage.sbr" \
	"$(INTDIR)\STMPage.sbr" \
	"$(INTDIR)\timerpage.sbr" \
	"$(INTDIR)\userpage.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\inputgroup.sbr" \
	"$(INTDIR)\inputlist.sbr" \
	"$(INTDIR)\output.sbr" \
	"$(INTDIR)\outputgroup.sbr" \
	"$(INTDIR)\outputlist.sbr" \
	"$(INTDIR)\processlist.sbr" \
	"$(INTDIR)\timerlist.sbr" \
	"$(INTDIR)\ComConfigurationDialog.sbr" \
	"$(INTDIR)\InputDialog.sbr" \
	"$(INTDIR)\InstructionDialog.sbr" \
	"$(INTDIR)\OutputDialog.sbr" \
	"$(INTDIR)\passwordcheckdialog.sbr" \
	"$(INTDIR)\passworddialog.sbr" \
	"$(INTDIR)\RelayDialog.sbr" \
	"$(INTDIR)\SDIConfigurationDialog.sbr" \
	"$(INTDIR)\SDIDialog.sbr" \
	"$(INTDIR)\selectinputdlg.sbr" \
	"$(INTDIR)\FieldMapPage.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\SpecialCharactersDialog.sbr" \
	"$(INTDIR)\statuslist.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SVDoc.sbr" \
	"$(INTDIR)\SVPage.sbr" \
	"$(INTDIR)\SVTree.sbr" \
	"$(INTDIR)\SVView.sbr" \
	"$(INTDIR)\SystemVerwaltung.sbr" \
	"$(INTDIR)\treelist.sbr" \
	"$(INTDIR)\SDIConfigurationIbmRS232.sbr"

"$(OUTDIR)\SystemVerwaltung.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=cipcdebug.lib oemguid.lib WKClassesDebug.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\SystemVerwaltung.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SystemVerwaltung.exe" 
LINK32_OBJS= \
	"$(INTDIR)\activationpage.obj" \
	"$(INTDIR)\AKUPage.obj" \
	"$(INTDIR)\AppPage.obj" \
	"$(INTDIR)\callpage.obj" \
	"$(INTDIR)\CoCoPage.obj" \
	"$(INTDIR)\GAAPage.obj" \
	"$(INTDIR)\hardwarepage.obj" \
	"$(INTDIR)\HostPage.obj" \
	"$(INTDIR)\inputpage.obj" \
	"$(INTDIR)\ISDNPage.obj" \
	"$(INTDIR)\MESZPage.obj" \
	"$(INTDIR)\MicoPage.obj" \
	"$(INTDIR)\NTLoginPage.obj" \
	"$(INTDIR)\outputpage.obj" \
	"$(INTDIR)\permissionpage.obj" \
	"$(INTDIR)\PTUnitPage.obj" \
	"$(INTDIR)\RelayPage.obj" \
	"$(INTDIR)\RS232Page.obj" \
	"$(INTDIR)\savepage.obj" \
	"$(INTDIR)\SDIPage.obj" \
	"$(INTDIR)\SimPage.obj" \
	"$(INTDIR)\SocketPage.obj" \
	"$(INTDIR)\SoftwarePage.obj" \
	"$(INTDIR)\STMPage.obj" \
	"$(INTDIR)\timerpage.obj" \
	"$(INTDIR)\userpage.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\inputgroup.obj" \
	"$(INTDIR)\inputlist.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\outputgroup.obj" \
	"$(INTDIR)\outputlist.obj" \
	"$(INTDIR)\processlist.obj" \
	"$(INTDIR)\timerlist.obj" \
	"$(INTDIR)\ComConfigurationDialog.obj" \
	"$(INTDIR)\InputDialog.obj" \
	"$(INTDIR)\InstructionDialog.obj" \
	"$(INTDIR)\OutputDialog.obj" \
	"$(INTDIR)\passwordcheckdialog.obj" \
	"$(INTDIR)\passworddialog.obj" \
	"$(INTDIR)\RelayDialog.obj" \
	"$(INTDIR)\SDIConfigurationDialog.obj" \
	"$(INTDIR)\SDIDialog.obj" \
	"$(INTDIR)\selectinputdlg.obj" \
	"$(INTDIR)\FieldMapPage.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\SpecialCharactersDialog.obj" \
	"$(INTDIR)\statuslist.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SVDoc.obj" \
	"$(INTDIR)\SVPage.obj" \
	"$(INTDIR)\SVTree.obj" \
	"$(INTDIR)\SVView.obj" \
	"$(INTDIR)\SystemVerwaltung.obj" \
	"$(INTDIR)\treelist.obj" \
	"$(INTDIR)\SystemVerwaltung.res" \
	"$(INTDIR)\SDIConfigurationIbmRS232.obj"

"$(OUTDIR)\SystemVerwaltung.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("SystemVerwaltung.dep")
!INCLUDE "SystemVerwaltung.dep"
!ELSE 
!MESSAGE Warning: cannot find "SystemVerwaltung.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SystemVerwaltung - Win32 Release" || "$(CFG)" == "SystemVerwaltung - Win32 Debug"
SOURCE=.\activationpage.cpp

"$(INTDIR)\activationpage.obj"	"$(INTDIR)\activationpage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\AKUPage.cpp

"$(INTDIR)\AKUPage.obj"	"$(INTDIR)\AKUPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\AppPage.cpp

"$(INTDIR)\AppPage.obj"	"$(INTDIR)\AppPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\callpage.cpp

"$(INTDIR)\callpage.obj"	"$(INTDIR)\callpage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\CoCoPage.cpp

"$(INTDIR)\CoCoPage.obj"	"$(INTDIR)\CoCoPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\GAAPage.cpp

"$(INTDIR)\GAAPage.obj"	"$(INTDIR)\GAAPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\hardwarepage.cpp

"$(INTDIR)\hardwarepage.obj"	"$(INTDIR)\hardwarepage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\HostPage.cpp

"$(INTDIR)\HostPage.obj"	"$(INTDIR)\HostPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\inputpage.cpp

"$(INTDIR)\inputpage.obj"	"$(INTDIR)\inputpage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\ISDNPage.cpp

"$(INTDIR)\ISDNPage.obj"	"$(INTDIR)\ISDNPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\MESZPage.cpp

"$(INTDIR)\MESZPage.obj"	"$(INTDIR)\MESZPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\MicoPage.cpp

"$(INTDIR)\MicoPage.obj"	"$(INTDIR)\MicoPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\NTLoginPage.cpp

"$(INTDIR)\NTLoginPage.obj"	"$(INTDIR)\NTLoginPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\outputpage.cpp

"$(INTDIR)\outputpage.obj"	"$(INTDIR)\outputpage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\permissionpage.cpp

"$(INTDIR)\permissionpage.obj"	"$(INTDIR)\permissionpage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\PTUnitPage.cpp

"$(INTDIR)\PTUnitPage.obj"	"$(INTDIR)\PTUnitPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\RelayPage.cpp

"$(INTDIR)\RelayPage.obj"	"$(INTDIR)\RelayPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\RS232Page.cpp

"$(INTDIR)\RS232Page.obj"	"$(INTDIR)\RS232Page.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\savepage.cpp

"$(INTDIR)\savepage.obj"	"$(INTDIR)\savepage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SDIPage.cpp

"$(INTDIR)\SDIPage.obj"	"$(INTDIR)\SDIPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SimPage.cpp

"$(INTDIR)\SimPage.obj"	"$(INTDIR)\SimPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SocketPage.cpp

"$(INTDIR)\SocketPage.obj"	"$(INTDIR)\SocketPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SoftwarePage.cpp

"$(INTDIR)\SoftwarePage.obj"	"$(INTDIR)\SoftwarePage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\STMPage.cpp

"$(INTDIR)\STMPage.obj"	"$(INTDIR)\STMPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\timerpage.cpp

"$(INTDIR)\timerpage.obj"	"$(INTDIR)\timerpage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\userpage.cpp

"$(INTDIR)\userpage.obj"	"$(INTDIR)\userpage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\input.cpp

"$(INTDIR)\input.obj"	"$(INTDIR)\input.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\inputgroup.cpp

"$(INTDIR)\inputgroup.obj"	"$(INTDIR)\inputgroup.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\inputlist.cpp

"$(INTDIR)\inputlist.obj"	"$(INTDIR)\inputlist.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\output.cpp

"$(INTDIR)\output.obj"	"$(INTDIR)\output.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\outputgroup.cpp

"$(INTDIR)\outputgroup.obj"	"$(INTDIR)\outputgroup.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\outputlist.cpp

"$(INTDIR)\outputlist.obj"	"$(INTDIR)\outputlist.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\processlist.cpp

"$(INTDIR)\processlist.obj"	"$(INTDIR)\processlist.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\timerlist.cpp

"$(INTDIR)\timerlist.obj"	"$(INTDIR)\timerlist.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\ComConfigurationDialog.cpp

"$(INTDIR)\ComConfigurationDialog.obj"	"$(INTDIR)\ComConfigurationDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\InputDialog.cpp

"$(INTDIR)\InputDialog.obj"	"$(INTDIR)\InputDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\InstructionDialog.cpp

"$(INTDIR)\InstructionDialog.obj"	"$(INTDIR)\InstructionDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\OutputDialog.cpp

"$(INTDIR)\OutputDialog.obj"	"$(INTDIR)\OutputDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\passwordcheckdialog.cpp

"$(INTDIR)\passwordcheckdialog.obj"	"$(INTDIR)\passwordcheckdialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\passworddialog.cpp

"$(INTDIR)\passworddialog.obj"	"$(INTDIR)\passworddialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\RelayDialog.cpp

"$(INTDIR)\RelayDialog.obj"	"$(INTDIR)\RelayDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SDIConfigurationDialog.cpp

"$(INTDIR)\SDIConfigurationDialog.obj"	"$(INTDIR)\SDIConfigurationDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SDIDialog.cpp

"$(INTDIR)\SDIDialog.obj"	"$(INTDIR)\SDIDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\selectinputdlg.cpp

"$(INTDIR)\selectinputdlg.obj"	"$(INTDIR)\selectinputdlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\FieldMapPage.cpp

"$(INTDIR)\FieldMapPage.obj"	"$(INTDIR)\FieldMapPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\MainFrm.cpp

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SDIConfigurationIbmRS232.cpp

"$(INTDIR)\SDIConfigurationIbmRS232.obj"	"$(INTDIR)\SDIConfigurationIbmRS232.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SpecialCharactersDialog.cpp

"$(INTDIR)\SpecialCharactersDialog.obj"	"$(INTDIR)\SpecialCharactersDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\statuslist.cpp

"$(INTDIR)\statuslist.obj"	"$(INTDIR)\statuslist.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "SystemVerwaltung - Win32 Release"

CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SystemVerwaltung.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\SystemVerwaltung.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "SystemVerwaltung - Win32 Debug"

CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SystemVerwaltung.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\SystemVerwaltung.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SVDoc.cpp

"$(INTDIR)\SVDoc.obj"	"$(INTDIR)\SVDoc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SVPage.cpp

"$(INTDIR)\SVPage.obj"	"$(INTDIR)\SVPage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SVTree.cpp

"$(INTDIR)\SVTree.obj"	"$(INTDIR)\SVTree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SVView.cpp

"$(INTDIR)\SVView.obj"	"$(INTDIR)\SVView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SystemVerwaltung.cpp

"$(INTDIR)\SystemVerwaltung.obj"	"$(INTDIR)\SystemVerwaltung.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\treelist.cpp

"$(INTDIR)\treelist.obj"	"$(INTDIR)\treelist.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SystemVerwaltung.pch"


SOURCE=.\SystemVerwaltung.rc

"$(INTDIR)\SystemVerwaltung.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

