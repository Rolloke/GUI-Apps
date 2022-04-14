# Microsoft Developer Studio Generated NMAKE File, Based on WkWizard.dsp
!IF "$(CFG)" == ""
CFG=WkWizard - Win32 Release
!MESSAGE No configuration specified. Defaulting to WkWizard - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "WkWizard - Win32 Release" && "$(CFG)" !=\
 "WkWizard - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WkWizard.mak" CFG="WkWizard - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WkWizard - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WkWizard - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "WkWizard - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WkWizard.exe" "$(OUTDIR)\WkWizard.pch"

!ELSE 

ALL : "$(OUTDIR)\WkWizard.exe" "$(OUTDIR)\WkWizard.pch"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\DataView.obj"
	-@erase "$(INTDIR)\InsertDialog.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\ProjectView.obj"
	-@erase "$(INTDIR)\Scanner.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TranslateString.obj"
	-@erase "$(INTDIR)\WkWizard.obj"
	-@erase "$(INTDIR)\WkWizard.pch"
	-@erase "$(INTDIR)\WkWizard.res"
	-@erase "$(INTDIR)\WkWizardDoc.obj"
	-@erase "$(OUTDIR)\WkWizard.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\WkWizard.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\WkWizard.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WkWizard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /profile /machine:I386\
 /out:"$(OUTDIR)\WkWizard.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DataView.obj" \
	"$(INTDIR)\InsertDialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\ProjectView.obj" \
	"$(INTDIR)\Scanner.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TranslateString.obj" \
	"$(INTDIR)\WkWizard.obj" \
	"$(INTDIR)\WkWizard.res" \
	"$(INTDIR)\WkWizardDoc.obj"

"$(OUTDIR)\WkWizard.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WkWizard - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WkWizard.exe" "$(OUTDIR)\WkWizard.pch"

!ELSE 

ALL : "$(OUTDIR)\WkWizard.exe" "$(OUTDIR)\WkWizard.pch"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\DataView.obj"
	-@erase "$(INTDIR)\InsertDialog.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\ProjectView.obj"
	-@erase "$(INTDIR)\Scanner.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TranslateString.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\WkWizard.obj"
	-@erase "$(INTDIR)\WkWizard.pch"
	-@erase "$(INTDIR)\WkWizard.res"
	-@erase "$(INTDIR)\WkWizardDoc.obj"
	-@erase "$(OUTDIR)\WkWizard.exe"
	-@erase "$(OUTDIR)\WkWizard.ilk"
	-@erase "$(OUTDIR)\WkWizard.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\WkWizard.pch" /YX"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\WkWizard.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WkWizard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\WkWizard.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\WkWizard.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DataView.obj" \
	"$(INTDIR)\InsertDialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\ProjectView.obj" \
	"$(INTDIR)\Scanner.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TranslateString.obj" \
	"$(INTDIR)\WkWizard.obj" \
	"$(INTDIR)\WkWizard.res" \
	"$(INTDIR)\WkWizardDoc.obj"

"$(OUTDIR)\WkWizard.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "WkWizard - Win32 Release" || "$(CFG)" ==\
 "WkWizard - Win32 Debug"
SOURCE=.\DataView.cpp
DEP_CPP_DATAV=\
	".\DataView.h"\
	".\Scanner.h"\
	".\stdafx.h"\
	".\TranslateString.h"\
	".\WkWizard.h"\
	

"$(INTDIR)\DataView.obj" : $(SOURCE) $(DEP_CPP_DATAV) "$(INTDIR)"


SOURCE=.\InsertDialog.cpp
DEP_CPP_INSER=\
	".\InsertDialog.h"\
	".\stdafx.h"\
	".\WkWizard.h"\
	

"$(INTDIR)\InsertDialog.obj" : $(SOURCE) $(DEP_CPP_INSER) "$(INTDIR)"


SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\DataView.h"\
	".\MainFrm.h"\
	".\ProjectView.h"\
	".\Scanner.h"\
	".\stdafx.h"\
	".\TranslateString.h"\
	".\WkWizard.h"\
	".\WkWizardDoc.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


SOURCE=.\Parser.cpp
DEP_CPP_PARSE=\
	".\Parser.h"\
	".\Scanner.h"\
	".\stdafx.h"\
	".\TranslateString.h"\
	

"$(INTDIR)\Parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"


SOURCE=.\ProjectView.cpp
DEP_CPP_PROJE=\
	".\InsertDialog.h"\
	".\ProjectView.h"\
	".\Scanner.h"\
	".\stdafx.h"\
	".\TranslateString.h"\
	".\WkWizard.h"\
	".\WkWizardDoc.h"\
	

"$(INTDIR)\ProjectView.obj" : $(SOURCE) $(DEP_CPP_PROJE) "$(INTDIR)"


SOURCE=.\Scanner.cpp
DEP_CPP_SCANN=\
	".\Scanner.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Scanner.obj" : $(SOURCE) $(DEP_CPP_SCANN) "$(INTDIR)"


SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "WkWizard - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\WkWizard.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\WkWizard.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WkWizard - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\WkWizard.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\WkWizard.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TranslateString.cpp
DEP_CPP_TRANS=\
	".\stdafx.h"\
	".\TranslateString.h"\
	

"$(INTDIR)\TranslateString.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


SOURCE=.\WkWizard.cpp
DEP_CPP_WKWIZ=\
	".\MainFrm.h"\
	".\ProjectView.h"\
	".\Scanner.h"\
	".\stdafx.h"\
	".\TranslateString.h"\
	".\WkWizard.h"\
	".\WkWizardDoc.h"\
	

"$(INTDIR)\WkWizard.obj" : $(SOURCE) $(DEP_CPP_WKWIZ) "$(INTDIR)"


SOURCE=.\WkWizard.rc
DEP_RSC_WKWIZA=\
	".\res\Toolbar.bmp"\
	".\res\WkWizard.ico"\
	".\res\WkWizard.rc2"\
	".\res\WkWizardDoc.ico"\
	

"$(INTDIR)\WkWizard.res" : $(SOURCE) $(DEP_RSC_WKWIZA) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\WkWizardDoc.cpp
DEP_CPP_WKWIZAR=\
	".\Parser.h"\
	".\Scanner.h"\
	".\stdafx.h"\
	".\TranslateString.h"\
	".\WkWizard.h"\
	".\WkWizardDoc.h"\
	

"$(INTDIR)\WkWizardDoc.obj" : $(SOURCE) $(DEP_CPP_WKWIZAR) "$(INTDIR)"



!ENDIF 

