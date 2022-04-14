# Microsoft Developer Studio Generated NMAKE File, Based on UpdateHandler.dsp
!IF "$(CFG)" == ""
CFG=UpdateHandler - Win32 Release
!MESSAGE No configuration specified. Defaulting to UpdateHandler - Win32\
 Release.
!ENDIF 

!IF "$(CFG)" != "UpdateHandler - Win32 Release" && "$(CFG)" !=\
 "UpdateHandler - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UpdateHandler.mak" CFG="UpdateHandler - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UpdateHandler - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "UpdateHandler - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\UpdateHandler.exe"

!ELSE 

ALL : "$(OUTDIR)\UpdateHandler.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ConnectionDialog.obj"
	-@erase "$(INTDIR)\CoolBar.obj"
	-@erase "$(INTDIR)\CreateServiceDialog.obj"
	-@erase "$(INTDIR)\DirContent.obj"
	-@erase "$(INTDIR)\DirDialog.obj"
	-@erase "$(INTDIR)\DirectoryView.obj"
	-@erase "$(INTDIR)\FileView.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ReportView.obj"
	-@erase "$(INTDIR)\SendDialog.obj"
	-@erase "$(INTDIR)\SettingsDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\SystemInfoDialog.obj"
	-@erase "$(INTDIR)\UpdateDoc.obj"
	-@erase "$(INTDIR)\UpdateHandler.obj"
	-@erase "$(INTDIR)\UpdateHandler.pch"
	-@erase "$(INTDIR)\UpdateHandler.res"
	-@erase "$(INTDIR)\UpdateHandlerInput.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\UpdateHandler.exe"
	-@erase "$(OUTDIR)\UpdateHandler.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\UpdateHandler.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\UpdateHandler.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UpdateHandler.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\UpdateHandler.pdb" /map:"$(INTDIR)\UpdateHandler.map"\
 /machine:I386 /out:"$(OUTDIR)\UpdateHandler.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ConnectionDialog.obj" \
	"$(INTDIR)\CoolBar.obj" \
	"$(INTDIR)\CreateServiceDialog.obj" \
	"$(INTDIR)\DirContent.obj" \
	"$(INTDIR)\DirDialog.obj" \
	"$(INTDIR)\DirectoryView.obj" \
	"$(INTDIR)\FileView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ReportView.obj" \
	"$(INTDIR)\SendDialog.obj" \
	"$(INTDIR)\SettingsDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SystemInfoDialog.obj" \
	"$(INTDIR)\UpdateDoc.obj" \
	"$(INTDIR)\UpdateHandler.obj" \
	"$(INTDIR)\UpdateHandler.res" \
	"$(INTDIR)\UpdateHandlerInput.obj" \
	"V:\Project\Cipc\Release\Cipc.lib"

"$(OUTDIR)\UpdateHandler.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\UpdateHandler.exe" "$(OUTDIR)\UpdateHandler.bsc"

!ELSE 

ALL : "$(OUTDIR)\UpdateHandler.exe" "$(OUTDIR)\UpdateHandler.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ConnectionDialog.obj"
	-@erase "$(INTDIR)\ConnectionDialog.sbr"
	-@erase "$(INTDIR)\CoolBar.obj"
	-@erase "$(INTDIR)\CoolBar.sbr"
	-@erase "$(INTDIR)\CreateServiceDialog.obj"
	-@erase "$(INTDIR)\CreateServiceDialog.sbr"
	-@erase "$(INTDIR)\DirContent.obj"
	-@erase "$(INTDIR)\DirContent.sbr"
	-@erase "$(INTDIR)\DirDialog.obj"
	-@erase "$(INTDIR)\DirDialog.sbr"
	-@erase "$(INTDIR)\DirectoryView.obj"
	-@erase "$(INTDIR)\DirectoryView.sbr"
	-@erase "$(INTDIR)\FileView.obj"
	-@erase "$(INTDIR)\FileView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\ReportView.obj"
	-@erase "$(INTDIR)\ReportView.sbr"
	-@erase "$(INTDIR)\SendDialog.obj"
	-@erase "$(INTDIR)\SendDialog.sbr"
	-@erase "$(INTDIR)\SettingsDlg.obj"
	-@erase "$(INTDIR)\SettingsDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\SystemInfoDialog.obj"
	-@erase "$(INTDIR)\SystemInfoDialog.sbr"
	-@erase "$(INTDIR)\UpdateDoc.obj"
	-@erase "$(INTDIR)\UpdateDoc.sbr"
	-@erase "$(INTDIR)\UpdateHandler.obj"
	-@erase "$(INTDIR)\UpdateHandler.pch"
	-@erase "$(INTDIR)\UpdateHandler.res"
	-@erase "$(INTDIR)\UpdateHandler.sbr"
	-@erase "$(INTDIR)\UpdateHandlerInput.obj"
	-@erase "$(INTDIR)\UpdateHandlerInput.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\UpdateHandler.bsc"
	-@erase "$(OUTDIR)\UpdateHandler.exe"
	-@erase "$(OUTDIR)\UpdateHandler.ilk"
	-@erase "$(OUTDIR)\UpdateHandler.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /Gi /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\UpdateHandler.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\UpdateHandler.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UpdateHandler.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ConnectionDialog.sbr" \
	"$(INTDIR)\CoolBar.sbr" \
	"$(INTDIR)\CreateServiceDialog.sbr" \
	"$(INTDIR)\DirContent.sbr" \
	"$(INTDIR)\DirDialog.sbr" \
	"$(INTDIR)\DirectoryView.sbr" \
	"$(INTDIR)\FileView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\ReportView.sbr" \
	"$(INTDIR)\SendDialog.sbr" \
	"$(INTDIR)\SettingsDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SystemInfoDialog.sbr" \
	"$(INTDIR)\UpdateDoc.sbr" \
	"$(INTDIR)\UpdateHandler.sbr" \
	"$(INTDIR)\UpdateHandlerInput.sbr"

"$(OUTDIR)\UpdateHandler.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\UpdateHandler.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\UpdateHandler.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ConnectionDialog.obj" \
	"$(INTDIR)\CoolBar.obj" \
	"$(INTDIR)\CreateServiceDialog.obj" \
	"$(INTDIR)\DirContent.obj" \
	"$(INTDIR)\DirDialog.obj" \
	"$(INTDIR)\DirectoryView.obj" \
	"$(INTDIR)\FileView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ReportView.obj" \
	"$(INTDIR)\SendDialog.obj" \
	"$(INTDIR)\SettingsDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SystemInfoDialog.obj" \
	"$(INTDIR)\UpdateDoc.obj" \
	"$(INTDIR)\UpdateHandler.obj" \
	"$(INTDIR)\UpdateHandler.res" \
	"$(INTDIR)\UpdateHandlerInput.obj" \
	"V:\Project\Cipc\Debug\CipcDebug.lib"

"$(OUTDIR)\UpdateHandler.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "UpdateHandler - Win32 Release" || "$(CFG)" ==\
 "UpdateHandler - Win32 Debug"
SOURCE=.\ConnectionDialog.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_CONNE=\
	".\ConnectionDialog.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\ConnectionDialog.obj" : $(SOURCE) $(DEP_CPP_CONNE) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_CONNE=\
	".\ConnectionDialog.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\ConnectionDialog.obj"	"$(INTDIR)\ConnectionDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_CONNE) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\CoolBar.cpp
DEP_CPP_COOLB=\
	".\CoolBar.h"\
	

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"


"$(INTDIR)\CoolBar.obj" : $(SOURCE) $(DEP_CPP_COOLB) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"


"$(INTDIR)\CoolBar.obj"	"$(INTDIR)\CoolBar.sbr" : $(SOURCE) $(DEP_CPP_COOLB)\
 "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\CreateServiceDialog.cpp
DEP_CPP_CREAT=\
	".\CreateServiceDialog.h"\
	".\UpdateHandler.h"\
	

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"


"$(INTDIR)\CreateServiceDialog.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"


"$(INTDIR)\CreateServiceDialog.obj"	"$(INTDIR)\CreateServiceDialog.sbr" : \
$(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\DirContent.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_DIRCO=\
	".\DirContent.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\DirContent.obj" : $(SOURCE) $(DEP_CPP_DIRCO) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_DIRCO=\
	".\DirContent.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\DirContent.obj"	"$(INTDIR)\DirContent.sbr" : $(SOURCE)\
 $(DEP_CPP_DIRCO) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\DirDialog.cpp
DEP_CPP_DIRDI=\
	".\DirDialog.h"\
	".\UpdateHandler.h"\
	

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"


"$(INTDIR)\DirDialog.obj" : $(SOURCE) $(DEP_CPP_DIRDI) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"


"$(INTDIR)\DirDialog.obj"	"$(INTDIR)\DirDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_DIRDI) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\DirectoryView.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_DIREC=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\DirectoryView.h"\
	".\MainFrm.h"\
	".\UpdateDoc.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\DirectoryView.obj" : $(SOURCE) $(DEP_CPP_DIREC) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_DIREC=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\DirectoryView.h"\
	".\MainFrm.h"\
	".\UpdateDoc.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\DirectoryView.obj"	"$(INTDIR)\DirectoryView.sbr" : $(SOURCE)\
 $(DEP_CPP_DIREC) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\FileView.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_FILEV=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\FileView.h"\
	".\MainFrm.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\FileView.obj" : $(SOURCE) $(DEP_CPP_FILEV) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_FILEV=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\FileView.h"\
	".\MainFrm.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\FileView.obj"	"$(INTDIR)\FileView.sbr" : $(SOURCE) $(DEP_CPP_FILEV)\
 "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_MAINF=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\DirectoryView.h"\
	".\FileView.h"\
	".\MainFrm.h"\
	".\ReportView.h"\
	".\UpdateDoc.h"\
	".\UpdateHandler.h"\
	".\UpdateHandlerInput.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_MAINF=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\DirectoryView.h"\
	".\FileView.h"\
	".\MainFrm.h"\
	".\ReportView.h"\
	".\UpdateDoc.h"\
	".\UpdateHandler.h"\
	".\UpdateHandlerInput.h"\
	

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\ReportView.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_REPOR=\
	".\CoolBar.h"\
	".\MainFrm.h"\
	".\ReportView.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\ReportView.obj" : $(SOURCE) $(DEP_CPP_REPOR) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_REPOR=\
	".\CoolBar.h"\
	".\MainFrm.h"\
	".\ReportView.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\ReportView.obj"	"$(INTDIR)\ReportView.sbr" : $(SOURCE)\
 $(DEP_CPP_REPOR) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\SendDialog.cpp
DEP_CPP_SENDD=\
	".\SendDialog.h"\
	".\UpdateHandler.h"\
	

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"


"$(INTDIR)\SendDialog.obj" : $(SOURCE) $(DEP_CPP_SENDD) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"


"$(INTDIR)\SendDialog.obj"	"$(INTDIR)\SendDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_SENDD) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\SettingsDlg.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_SETTI=\
	".\SettingsDlg.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\SettingsDlg.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_SETTI=\
	".\SettingsDlg.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\SettingsDlg.obj"	"$(INTDIR)\SettingsDlg.sbr" : $(SOURCE)\
 $(DEP_CPP_SETTI) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcextramemory.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\host.h"\
	"v:\project\cipc\permission.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\user.h"\
	"v:\project\cipc\util.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\logzip.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\UpdateHandler.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\UpdateHandler.pch" : $(SOURCE)\
 $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcextramemory.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\host.h"\
	"v:\project\cipc\permission.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\user.h"\
	"v:\project\cipc\util.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\logzip.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /Gi /GX /Zi /Od /D "WIN32" /D "_DEBUG"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\UpdateHandler.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\UpdateHandler.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SystemInfoDialog.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_SYSTE=\
	".\SystemInfoDialog.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\SystemInfoDialog.obj" : $(SOURCE) $(DEP_CPP_SYSTE) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_SYSTE=\
	".\SystemInfoDialog.h"\
	".\UpdateHandler.h"\
	

"$(INTDIR)\SystemInfoDialog.obj"	"$(INTDIR)\SystemInfoDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_SYSTE) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\UpdateDoc.cpp
DEP_CPP_UPDAT=\
	".\UpdateDoc.h"\
	".\UpdateHandler.h"\
	

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"


"$(INTDIR)\UpdateDoc.obj" : $(SOURCE) $(DEP_CPP_UPDAT) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"


"$(INTDIR)\UpdateDoc.obj"	"$(INTDIR)\UpdateDoc.sbr" : $(SOURCE)\
 $(DEP_CPP_UPDAT) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\UpdateHandler.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_UPDATE=\
	".\ConnectionDialog.h"\
	".\CoolBar.h"\
	".\CreateServiceDialog.h"\
	".\DirDialog.h"\
	".\DirectoryView.h"\
	".\MainFrm.h"\
	".\SendDialog.h"\
	".\SettingsDlg.h"\
	".\SystemInfoDialog.h"\
	".\UpdateDoc.h"\
	".\UpdateHandler.h"\
	".\UpdateHandlerInput.h"\
	

"$(INTDIR)\UpdateHandler.obj" : $(SOURCE) $(DEP_CPP_UPDATE) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_UPDATE=\
	".\ConnectionDialog.h"\
	".\CoolBar.h"\
	".\CreateServiceDialog.h"\
	".\DirDialog.h"\
	".\DirectoryView.h"\
	".\MainFrm.h"\
	".\SendDialog.h"\
	".\SettingsDlg.h"\
	".\SystemInfoDialog.h"\
	".\UpdateDoc.h"\
	".\UpdateHandler.h"\
	".\UpdateHandlerInput.h"\
	

"$(INTDIR)\UpdateHandler.obj"	"$(INTDIR)\UpdateHandler.sbr" : $(SOURCE)\
 $(DEP_CPP_UPDATE) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 

SOURCE=.\UpdateHandler.rc
DEP_RSC_UPDATEH=\
	".\res\back.bmp"\
	".\res\dialog_b.bmp"\
	".\res\eye.bmp"\
	".\res\symbol.bmp"\
	".\res\Toolbar.bmp"\
	".\res\update.bmp"\
	".\res\UpdateHandler.ico"\
	".\res\UpdateHandler.rc2"\
	

"$(INTDIR)\UpdateHandler.res" : $(SOURCE) $(DEP_RSC_UPDATEH) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\UpdateHandlerInput.cpp

!IF  "$(CFG)" == "UpdateHandler - Win32 Release"

DEP_CPP_UPDATEHA=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\MainFrm.h"\
	".\UpdateHandler.h"\
	".\UpdateHandlerInput.h"\
	

"$(INTDIR)\UpdateHandlerInput.obj" : $(SOURCE) $(DEP_CPP_UPDATEHA) "$(INTDIR)"\
 "$(INTDIR)\UpdateHandler.pch"


!ELSEIF  "$(CFG)" == "UpdateHandler - Win32 Debug"

DEP_CPP_UPDATEHA=\
	".\CoolBar.h"\
	".\DirContent.h"\
	".\MainFrm.h"\
	".\UpdateHandler.h"\
	".\UpdateHandlerInput.h"\
	

"$(INTDIR)\UpdateHandlerInput.obj"	"$(INTDIR)\UpdateHandlerInput.sbr" : \
$(SOURCE) $(DEP_CPP_UPDATEHA) "$(INTDIR)" "$(INTDIR)\UpdateHandler.pch"


!ENDIF 


!ENDIF 

