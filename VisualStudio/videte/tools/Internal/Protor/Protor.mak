# Microsoft Developer Studio Generated NMAKE File, Based on Protor.dsp
!IF "$(CFG)" == ""
CFG=Protor - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Protor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Protor - Win32 Release" && "$(CFG)" != "Protor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Protor.mak" CFG="Protor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Protor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Protor - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Protor - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Protor.exe"

!ELSE 

ALL : "$(OUTDIR)\Protor.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Protor.obj"
	-@erase "$(INTDIR)\Protor.pch"
	-@erase "$(INTDIR)\Protor.res"
	-@erase "$(INTDIR)\ProtorDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Protor.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\Protor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\Protor.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Protor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Cipc.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\Protor.pdb" /machine:I386 /out:"$(OUTDIR)\Protor.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Protor.obj" \
	"$(INTDIR)\Protor.res" \
	"$(INTDIR)\ProtorDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Protor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Protor - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Protor.exe" "$(OUTDIR)\Protor.bsc"

!ELSE 

ALL : "$(OUTDIR)\Protor.exe" "$(OUTDIR)\Protor.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Protor.obj"
	-@erase "$(INTDIR)\Protor.pch"
	-@erase "$(INTDIR)\Protor.res"
	-@erase "$(INTDIR)\Protor.sbr"
	-@erase "$(INTDIR)\ProtorDlg.obj"
	-@erase "$(INTDIR)\ProtorDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Protor.bsc"
	-@erase "$(OUTDIR)\Protor.exe"
	-@erase "$(OUTDIR)\Protor.ilk"
	-@erase "$(OUTDIR)\Protor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Protor.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\Protor.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Protor.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Protor.sbr" \
	"$(INTDIR)\ProtorDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Protor.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=CipcDebug.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\Protor.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Protor.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Protor.obj" \
	"$(INTDIR)\Protor.res" \
	"$(INTDIR)\ProtorDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Protor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "Protor - Win32 Release" || "$(CFG)" == "Protor - Win32 Debug"
SOURCE=.\Protor.cpp

!IF  "$(CFG)" == "Protor - Win32 Release"

DEP_CPP_PROTO=\
	".\Protor.h"\
	".\ProtorDlg.h"\
	".\WK_String.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\Protor.obj" : $(SOURCE) $(DEP_CPP_PROTO) "$(INTDIR)"\
 "$(INTDIR)\Protor.pch"


!ELSEIF  "$(CFG)" == "Protor - Win32 Debug"

DEP_CPP_PROTO=\
	".\Protor.h"\
	".\ProtorDlg.h"\
	".\WK_String.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\Protor.obj"	"$(INTDIR)\Protor.sbr" : $(SOURCE) $(DEP_CPP_PROTO)\
 "$(INTDIR)" "$(INTDIR)\Protor.pch"


!ENDIF 

SOURCE=.\Protor.rc
DEP_RSC_PROTOR=\
	".\res\Protor.ico"\
	".\res\Protor.rc2"\
	

"$(INTDIR)\Protor.res" : $(SOURCE) $(DEP_RSC_PROTOR) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ProtorDlg.cpp

!IF  "$(CFG)" == "Protor - Win32 Release"

DEP_CPP_PROTORD=\
	".\Protor.h"\
	".\ProtorDlg.h"\
	".\WK_String.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\ProtorDlg.obj" : $(SOURCE) $(DEP_CPP_PROTORD) "$(INTDIR)"\
 "$(INTDIR)\Protor.pch"


!ELSEIF  "$(CFG)" == "Protor - Win32 Debug"

DEP_CPP_PROTORD=\
	".\Protor.h"\
	".\ProtorDlg.h"\
	".\WK_String.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\ProtorDlg.obj"	"$(INTDIR)\ProtorDlg.sbr" : $(SOURCE)\
 $(DEP_CPP_PROTORD) "$(INTDIR)" "$(INTDIR)\Protor.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Protor - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\include\wk.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\Protor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Protor.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Protor - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\include\wk.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Protor.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Protor.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

