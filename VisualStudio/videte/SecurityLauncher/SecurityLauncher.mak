# Microsoft Developer Studio Generated NMAKE File, Based on SecurityLauncher.dsp
!IF "$(CFG)" == ""
CFG=SecurityLauncher - Win32 Release
!MESSAGE No configuration specified. Defaulting to SecurityLauncher - Win32\
 Release.
!ENDIF 

!IF "$(CFG)" != "SecurityLauncher - Win32 Release" && "$(CFG)" !=\
 "SecurityLauncher - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SecurityLauncher.mak" CFG="SecurityLauncher - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SecurityLauncher - Win32 Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "SecurityLauncher - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SecurityLauncher.exe" "$(OUTDIR)\SecurityLauncher.bsc"

!ELSE 

ALL : "$(OUTDIR)\SecurityLauncher.exe" "$(OUTDIR)\SecurityLauncher.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AfterBackupDialog.obj"
	-@erase "$(INTDIR)\AfterBackupDialog.sbr"
	-@erase "$(INTDIR)\Application.obj"
	-@erase "$(INTDIR)\Application.sbr"
	-@erase "$(INTDIR)\Applications.obj"
	-@erase "$(INTDIR)\Applications.sbr"
	-@erase "$(INTDIR)\AutoRun.obj"
	-@erase "$(INTDIR)\AutoRun.sbr"
	-@erase "$(INTDIR)\BackGroundWnd.obj"
	-@erase "$(INTDIR)\BackGroundWnd.sbr"
	-@erase "$(INTDIR)\BeforeBackupDialog.obj"
	-@erase "$(INTDIR)\BeforeBackupDialog.sbr"
	-@erase "$(INTDIR)\ErrorDialog.obj"
	-@erase "$(INTDIR)\ErrorDialog.sbr"
	-@erase "$(INTDIR)\LauncherControl.obj"
	-@erase "$(INTDIR)\LauncherControl.sbr"
	-@erase "$(INTDIR)\LauncherError.obj"
	-@erase "$(INTDIR)\LauncherError.sbr"
	-@erase "$(INTDIR)\LauncherLog.obj"
	-@erase "$(INTDIR)\LauncherLog.sbr"
	-@erase "$(INTDIR)\LauncherUpdate.obj"
	-@erase "$(INTDIR)\LauncherUpdate.sbr"
	-@erase "$(INTDIR)\LoginDialog.obj"
	-@erase "$(INTDIR)\LoginDialog.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MeszMez.obj"
	-@erase "$(INTDIR)\MeszMez.sbr"
	-@erase "$(INTDIR)\OemGuiApi2.obj"
	-@erase "$(INTDIR)\OemGuiApi2.sbr"
	-@erase "$(INTDIR)\RunService.obj"
	-@erase "$(INTDIR)\RunService.sbr"
	-@erase "$(INTDIR)\ScanChk.obj"
	-@erase "$(INTDIR)\ScanChk.sbr"
	-@erase "$(INTDIR)\SecurityLauncher.obj"
	-@erase "$(INTDIR)\SecurityLauncher.pch"
	-@erase "$(INTDIR)\SecurityLauncher.res"
	-@erase "$(INTDIR)\SecurityLauncher.sbr"
	-@erase "$(INTDIR)\ServiceDialog.obj"
	-@erase "$(INTDIR)\ServiceDialog.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\SystemDialog.obj"
	-@erase "$(INTDIR)\SystemDialog.sbr"
	-@erase "$(INTDIR)\UpdateRecord.obj"
	-@erase "$(INTDIR)\UpdateRecord.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\WatchDog.obj"
	-@erase "$(INTDIR)\WatchDog.sbr"
	-@erase "$(OUTDIR)\SecurityLauncher.bsc"
	-@erase "$(OUTDIR)\SecurityLauncher.exe"
	-@erase "$(OUTDIR)\SecurityLauncher.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SecurityLauncher.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\SecurityLauncher.res" /d "NDEBUG" /d "_AFXDLL"\
 /d "_SECURITY" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SecurityLauncher.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AfterBackupDialog.sbr" \
	"$(INTDIR)\Application.sbr" \
	"$(INTDIR)\Applications.sbr" \
	"$(INTDIR)\AutoRun.sbr" \
	"$(INTDIR)\BackGroundWnd.sbr" \
	"$(INTDIR)\BeforeBackupDialog.sbr" \
	"$(INTDIR)\ErrorDialog.sbr" \
	"$(INTDIR)\LauncherControl.sbr" \
	"$(INTDIR)\LauncherError.sbr" \
	"$(INTDIR)\LauncherLog.sbr" \
	"$(INTDIR)\LauncherUpdate.sbr" \
	"$(INTDIR)\LoginDialog.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\MeszMez.sbr" \
	"$(INTDIR)\OemGuiApi2.sbr" \
	"$(INTDIR)\RunService.sbr" \
	"$(INTDIR)\ScanChk.sbr" \
	"$(INTDIR)\SecurityLauncher.sbr" \
	"$(INTDIR)\ServiceDialog.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SystemDialog.sbr" \
	"$(INTDIR)\UpdateRecord.sbr" \
	"$(INTDIR)\WatchDog.sbr"

"$(OUTDIR)\SecurityLauncher.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=version.lib cipc.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\SecurityLauncher.pdb" /map:"$(INTDIR)\SecurityLauncher.map"\
 /machine:I386 /out:"$(OUTDIR)\SecurityLauncher.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AfterBackupDialog.obj" \
	"$(INTDIR)\Application.obj" \
	"$(INTDIR)\Applications.obj" \
	"$(INTDIR)\AutoRun.obj" \
	"$(INTDIR)\BackGroundWnd.obj" \
	"$(INTDIR)\BeforeBackupDialog.obj" \
	"$(INTDIR)\ErrorDialog.obj" \
	"$(INTDIR)\LauncherControl.obj" \
	"$(INTDIR)\LauncherError.obj" \
	"$(INTDIR)\LauncherLog.obj" \
	"$(INTDIR)\LauncherUpdate.obj" \
	"$(INTDIR)\LoginDialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MeszMez.obj" \
	"$(INTDIR)\OemGuiApi2.obj" \
	"$(INTDIR)\RunService.obj" \
	"$(INTDIR)\ScanChk.obj" \
	"$(INTDIR)\SecurityLauncher.obj" \
	"$(INTDIR)\SecurityLauncher.res" \
	"$(INTDIR)\ServiceDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SystemDialog.obj" \
	"$(INTDIR)\UpdateRecord.obj" \
	"$(INTDIR)\WatchDog.obj"

"$(OUTDIR)\SecurityLauncher.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SecurityLauncher.exe" "$(OUTDIR)\SecurityLauncher.bsc"

!ELSE 

ALL : "$(OUTDIR)\SecurityLauncher.exe" "$(OUTDIR)\SecurityLauncher.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AfterBackupDialog.obj"
	-@erase "$(INTDIR)\AfterBackupDialog.sbr"
	-@erase "$(INTDIR)\Application.obj"
	-@erase "$(INTDIR)\Application.sbr"
	-@erase "$(INTDIR)\Applications.obj"
	-@erase "$(INTDIR)\Applications.sbr"
	-@erase "$(INTDIR)\AutoRun.obj"
	-@erase "$(INTDIR)\AutoRun.sbr"
	-@erase "$(INTDIR)\BackGroundWnd.obj"
	-@erase "$(INTDIR)\BackGroundWnd.sbr"
	-@erase "$(INTDIR)\BeforeBackupDialog.obj"
	-@erase "$(INTDIR)\BeforeBackupDialog.sbr"
	-@erase "$(INTDIR)\ErrorDialog.obj"
	-@erase "$(INTDIR)\ErrorDialog.sbr"
	-@erase "$(INTDIR)\LauncherControl.obj"
	-@erase "$(INTDIR)\LauncherControl.sbr"
	-@erase "$(INTDIR)\LauncherError.obj"
	-@erase "$(INTDIR)\LauncherError.sbr"
	-@erase "$(INTDIR)\LauncherLog.obj"
	-@erase "$(INTDIR)\LauncherLog.sbr"
	-@erase "$(INTDIR)\LauncherUpdate.obj"
	-@erase "$(INTDIR)\LauncherUpdate.sbr"
	-@erase "$(INTDIR)\LoginDialog.obj"
	-@erase "$(INTDIR)\LoginDialog.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MeszMez.obj"
	-@erase "$(INTDIR)\MeszMez.sbr"
	-@erase "$(INTDIR)\OemGuiApi2.obj"
	-@erase "$(INTDIR)\OemGuiApi2.sbr"
	-@erase "$(INTDIR)\RunService.obj"
	-@erase "$(INTDIR)\RunService.sbr"
	-@erase "$(INTDIR)\ScanChk.obj"
	-@erase "$(INTDIR)\ScanChk.sbr"
	-@erase "$(INTDIR)\SecurityLauncher.obj"
	-@erase "$(INTDIR)\SecurityLauncher.pch"
	-@erase "$(INTDIR)\SecurityLauncher.res"
	-@erase "$(INTDIR)\SecurityLauncher.sbr"
	-@erase "$(INTDIR)\ServiceDialog.obj"
	-@erase "$(INTDIR)\ServiceDialog.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\SystemDialog.obj"
	-@erase "$(INTDIR)\SystemDialog.sbr"
	-@erase "$(INTDIR)\UpdateRecord.obj"
	-@erase "$(INTDIR)\UpdateRecord.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\WatchDog.obj"
	-@erase "$(INTDIR)\WatchDog.sbr"
	-@erase "$(OUTDIR)\SecurityLauncher.bsc"
	-@erase "$(OUTDIR)\SecurityLauncher.exe"
	-@erase "$(OUTDIR)\SecurityLauncher.map"
	-@erase "$(OUTDIR)\SecurityLauncher.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /Ob1 /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\SecurityLauncher.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\SecurityLauncher.res" /d "_DEBUG" /d "_AFXDLL"\
 /d "_SECURITY" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SecurityLauncher.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AfterBackupDialog.sbr" \
	"$(INTDIR)\Application.sbr" \
	"$(INTDIR)\Applications.sbr" \
	"$(INTDIR)\AutoRun.sbr" \
	"$(INTDIR)\BackGroundWnd.sbr" \
	"$(INTDIR)\BeforeBackupDialog.sbr" \
	"$(INTDIR)\ErrorDialog.sbr" \
	"$(INTDIR)\LauncherControl.sbr" \
	"$(INTDIR)\LauncherError.sbr" \
	"$(INTDIR)\LauncherLog.sbr" \
	"$(INTDIR)\LauncherUpdate.sbr" \
	"$(INTDIR)\LoginDialog.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\MeszMez.sbr" \
	"$(INTDIR)\OemGuiApi2.sbr" \
	"$(INTDIR)\RunService.sbr" \
	"$(INTDIR)\ScanChk.sbr" \
	"$(INTDIR)\SecurityLauncher.sbr" \
	"$(INTDIR)\ServiceDialog.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SystemDialog.sbr" \
	"$(INTDIR)\UpdateRecord.sbr" \
	"$(INTDIR)\WatchDog.sbr"

"$(OUTDIR)\SecurityLauncher.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=version.lib cipcdebug.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\SecurityLauncher.pdb"\
 /map:"$(INTDIR)\SecurityLauncher.map" /debug /machine:I386\
 /out:"$(OUTDIR)\SecurityLauncher.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AfterBackupDialog.obj" \
	"$(INTDIR)\Application.obj" \
	"$(INTDIR)\Applications.obj" \
	"$(INTDIR)\AutoRun.obj" \
	"$(INTDIR)\BackGroundWnd.obj" \
	"$(INTDIR)\BeforeBackupDialog.obj" \
	"$(INTDIR)\ErrorDialog.obj" \
	"$(INTDIR)\LauncherControl.obj" \
	"$(INTDIR)\LauncherError.obj" \
	"$(INTDIR)\LauncherLog.obj" \
	"$(INTDIR)\LauncherUpdate.obj" \
	"$(INTDIR)\LoginDialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MeszMez.obj" \
	"$(INTDIR)\OemGuiApi2.obj" \
	"$(INTDIR)\RunService.obj" \
	"$(INTDIR)\ScanChk.obj" \
	"$(INTDIR)\SecurityLauncher.obj" \
	"$(INTDIR)\SecurityLauncher.res" \
	"$(INTDIR)\ServiceDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SystemDialog.obj" \
	"$(INTDIR)\UpdateRecord.obj" \
	"$(INTDIR)\WatchDog.obj"

"$(OUTDIR)\SecurityLauncher.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "SecurityLauncher - Win32 Release" || "$(CFG)" ==\
 "SecurityLauncher - Win32 Debug"
SOURCE=.\AfterBackupDialog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_AFTER=\
	".\AfterBackupDialog.h"\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\AfterBackupDialog.obj"	"$(INTDIR)\AfterBackupDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_AFTER) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_AFTER=\
	".\AfterBackupDialog.h"\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\AfterBackupDialog.obj"	"$(INTDIR)\AfterBackupDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_AFTER) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\Application.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_APPLI=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\oemguiapi.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\Application.obj"	"$(INTDIR)\Application.sbr" : $(SOURCE)\
 $(DEP_CPP_APPLI) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_APPLI=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\oemguiapi.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\Application.obj"	"$(INTDIR)\Application.sbr" : $(SOURCE)\
 $(DEP_CPP_APPLI) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\Applications.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_APPLIC=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\Applications.obj"	"$(INTDIR)\Applications.sbr" : $(SOURCE)\
 $(DEP_CPP_APPLIC) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_APPLIC=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\Applications.obj"	"$(INTDIR)\Applications.sbr" : $(SOURCE)\
 $(DEP_CPP_APPLIC) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\AutoRun.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_AUTOR=\
	".\AutoRun.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\AutoRun.obj"	"$(INTDIR)\AutoRun.sbr" : $(SOURCE) $(DEP_CPP_AUTOR)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_AUTOR=\
	".\AutoRun.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\AutoRun.obj"	"$(INTDIR)\AutoRun.sbr" : $(SOURCE) $(DEP_CPP_AUTOR)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\BackGroundWnd.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_BACKG=\
	".\Application.h"\
	".\Applications.h"\
	".\BackGroundWnd.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\BackGroundWnd.obj"	"$(INTDIR)\BackGroundWnd.sbr" : $(SOURCE)\
 $(DEP_CPP_BACKG) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_BACKG=\
	".\Application.h"\
	".\Applications.h"\
	".\BackGroundWnd.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\BackGroundWnd.obj"	"$(INTDIR)\BackGroundWnd.sbr" : $(SOURCE)\
 $(DEP_CPP_BACKG) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\BeforeBackupDialog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_BEFOR=\
	".\Application.h"\
	".\Applications.h"\
	".\BeforeBackupDialog.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\BeforeBackupDialog.obj"	"$(INTDIR)\BeforeBackupDialog.sbr" : \
$(SOURCE) $(DEP_CPP_BEFOR) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_BEFOR=\
	".\Application.h"\
	".\Applications.h"\
	".\BeforeBackupDialog.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\BeforeBackupDialog.obj"	"$(INTDIR)\BeforeBackupDialog.sbr" : \
$(SOURCE) $(DEP_CPP_BEFOR) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\ErrorDialog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_ERROR=\
	".\Application.h"\
	".\Applications.h"\
	".\ErrorDialog.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\ErrorDialog.obj"	"$(INTDIR)\ErrorDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_ERROR) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_ERROR=\
	".\Application.h"\
	".\Applications.h"\
	".\ErrorDialog.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\ErrorDialog.obj"	"$(INTDIR)\ErrorDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_ERROR) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\LauncherControl.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_LAUNC=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\UpdateRecord.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_trace.h"\
	

"$(INTDIR)\LauncherControl.obj"	"$(INTDIR)\LauncherControl.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNC) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_LAUNC=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\UpdateRecord.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\LauncherControl.obj"	"$(INTDIR)\LauncherControl.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNC) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\LauncherError.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_LAUNCH=\
	".\Application.h"\
	".\Applications.h"\
	".\ErrorDialog.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_trace.h"\
	

"$(INTDIR)\LauncherError.obj"	"$(INTDIR)\LauncherError.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNCH) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_LAUNCH=\
	".\Application.h"\
	".\Applications.h"\
	".\ErrorDialog.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\LauncherError.obj"	"$(INTDIR)\LauncherError.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNCH) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\LauncherLog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_LAUNCHE=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\LauncherLog.obj"	"$(INTDIR)\LauncherLog.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNCHE) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_LAUNCHE=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\LauncherLog.obj"	"$(INTDIR)\LauncherLog.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNCHE) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\LauncherUpdate.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_LAUNCHER=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\ServiceDialog.h"\
	".\UpdateRecord.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_trace.h"\
	

"$(INTDIR)\LauncherUpdate.obj"	"$(INTDIR)\LauncherUpdate.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNCHER) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_LAUNCHER=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\ServiceDialog.h"\
	".\StdAfx.h"\
	".\UpdateRecord.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\LauncherUpdate.obj"	"$(INTDIR)\LauncherUpdate.sbr" : $(SOURCE)\
 $(DEP_CPP_LAUNCHER) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\LoginDialog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_LOGIN=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\LoginDialog.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\LoginDialog.obj"	"$(INTDIR)\LoginDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_LOGIN) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_LOGIN=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\LoginDialog.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\LoginDialog.obj"	"$(INTDIR)\LoginDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_LOGIN) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_MAINF=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MainFrm.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_MAINF=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MainFrm.h"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\MeszMez.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_MESZM=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\MeszMez.obj"	"$(INTDIR)\MeszMez.sbr" : $(SOURCE) $(DEP_CPP_MESZM)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_MESZM=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\MeszMez.obj"	"$(INTDIR)\MeszMez.sbr" : $(SOURCE) $(DEP_CPP_MESZM)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\OemGuiApi2.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_OEMGU=\
	".\AutoRun.h"\
	".\oemguiapi.h"\
	".\res\ApplicationDefines.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\OemGuiApi2.obj"	"$(INTDIR)\OemGuiApi2.sbr" : $(SOURCE)\
 $(DEP_CPP_OEMGU) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_OEMGU=\
	".\AutoRun.h"\
	".\oemguiapi.h"\
	".\res\ApplicationDefines.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\OemGuiApi2.obj"	"$(INTDIR)\OemGuiApi2.sbr" : $(SOURCE)\
 $(DEP_CPP_OEMGU) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\RunService.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_RUNSE=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\RunService.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\RunService.obj"	"$(INTDIR)\RunService.sbr" : $(SOURCE)\
 $(DEP_CPP_RUNSE) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_RUNSE=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\RunService.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\RunService.obj"	"$(INTDIR)\RunService.sbr" : $(SOURCE)\
 $(DEP_CPP_RUNSE) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\ScanChk.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_SCANC=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\ScanChk.obj"	"$(INTDIR)\ScanChk.sbr" : $(SOURCE) $(DEP_CPP_SCANC)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_SCANC=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\ScanChk.obj"	"$(INTDIR)\ScanChk.sbr" : $(SOURCE) $(DEP_CPP_SCANC)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\SecurityLauncher.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_SECUR=\
	".\AfterBackupDialog.h"\
	".\Application.h"\
	".\Applications.h"\
	".\BackGroundWnd.h"\
	".\BeforeBackupDialog.h"\
	".\ErrorDialog.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\LoginDialog.h"\
	".\MainFrm.h"\
	".\MeszMez.h"\
	".\oemguiapi.h"\
	".\RunService.h"\
	".\SecurityLauncher.h"\
	".\SystemDialog.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_trace.h"\
	

"$(INTDIR)\SecurityLauncher.obj"	"$(INTDIR)\SecurityLauncher.sbr" : $(SOURCE)\
 $(DEP_CPP_SECUR) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_SECUR=\
	".\AfterBackupDialog.h"\
	".\Application.h"\
	".\Applications.h"\
	".\BackGroundWnd.h"\
	".\BeforeBackupDialog.h"\
	".\ErrorDialog.h"\
	".\launcher.inl"\
	".\LauncherControl.h"\
	".\LoginDialog.h"\
	".\MainFrm.h"\
	".\MeszMez.h"\
	".\oemguiapi.h"\
	".\RunService.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\SystemDialog.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcservercontrol.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\SecurityLauncher.obj"	"$(INTDIR)\SecurityLauncher.sbr" : $(SOURCE)\
 $(DEP_CPP_SECUR) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\SecurityLauncher.rc
DEP_RSC_SECURI=\
	".\res\ApplicationDefines.h"\
	".\res\ApplicationNames.str"\
	".\res\lbggarny.bmp"\
	".\res\lbgist.bmp"\
	".\res\lbgncr.bmp"\
	".\res\SecurityLauncher.ico"\
	".\res\SecurityLauncher.rc2"\
	".\res\Toolbar.bmp"\
	".\res\ToolbarGarny.bmp"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\SecurityLauncher.res" : $(SOURCE) $(DEP_RSC_SECURI) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\SecurityLauncherEng.rc
SOURCE=.\ServiceDialog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_SERVI=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\ServiceDialog.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\ServiceDialog.obj"	"$(INTDIR)\ServiceDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_SERVI) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_SERVI=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\ServiceDialog.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\ServiceDialog.obj"	"$(INTDIR)\ServiceDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_SERVI) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	
CPP_SWITCHES=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\SecurityLauncher.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\SecurityLauncher.pch"\
 : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /Ob1 /D "WIN32" /D "_DEBUG"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\SecurityLauncher.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\SecurityLauncher.pch"\
 : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SystemDialog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_SYSTE=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\oemguiapi.h"\
	".\SecurityLauncher.h"\
	".\SystemDialog.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\SystemDialog.obj"	"$(INTDIR)\SystemDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_SYSTE) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_SYSTE=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\oemguiapi.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\SystemDialog.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\SystemDialog.obj"	"$(INTDIR)\SystemDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_SYSTE) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\UpdateRecord.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_UPDAT=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\UpdateRecord.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\UpdateRecord.obj"	"$(INTDIR)\UpdateRecord.sbr" : $(SOURCE)\
 $(DEP_CPP_UPDAT) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_UPDAT=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\UpdateRecord.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\UpdateRecord.obj"	"$(INTDIR)\UpdateRecord.sbr" : $(SOURCE)\
 $(DEP_CPP_UPDAT) "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 

SOURCE=.\WatchDog.cpp

!IF  "$(CFG)" == "SecurityLauncher - Win32 Release"

DEP_CPP_WATCH=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_profile.h"\
	

"$(INTDIR)\WatchDog.obj"	"$(INTDIR)\WatchDog.sbr" : $(SOURCE) $(DEP_CPP_WATCH)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ELSEIF  "$(CFG)" == "SecurityLauncher - Win32 Debug"

DEP_CPP_WATCH=\
	".\Application.h"\
	".\Applications.h"\
	".\launcher.inl"\
	".\MeszMez.h"\
	".\SecurityLauncher.h"\
	".\StdAfx.h"\
	".\WatchDog.h"\
	{$(INCLUDE)}"afximpl.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"permission.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"updatedefines.h"\
	{$(INCLUDE)}"user.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_profile.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_runtimeerrors.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\WatchDog.obj"	"$(INTDIR)\WatchDog.sbr" : $(SOURCE) $(DEP_CPP_WATCH)\
 "$(INTDIR)" "$(INTDIR)\SecurityLauncher.pch"


!ENDIF 


!ENDIF 

