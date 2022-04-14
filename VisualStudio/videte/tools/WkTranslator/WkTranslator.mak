# Microsoft Developer Studio Generated NMAKE File, Based on WkTranslator.dsp
!IF "$(CFG)" == ""
CFG=WkTranslator - Win32 Release
!MESSAGE No configuration specified. Defaulting to WkTranslator - Win32\
 Release.
!ENDIF 

!IF "$(CFG)" != "WkTranslator - Win32 Release" && "$(CFG)" !=\
 "WkTranslator - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WkTranslator.mak" CFG="WkTranslator - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WkTranslator - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WkTranslator - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WkTranslator.exe"

!ELSE 

ALL : "$(OUTDIR)\WkTranslator.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TranslateDialog.obj"
	-@erase "$(INTDIR)\TranslateString.obj"
	-@erase "$(INTDIR)\TSArrayArray.obj"
	-@erase "$(INTDIR)\WkTranslator.obj"
	-@erase "$(INTDIR)\WkTranslator.pch"
	-@erase "$(INTDIR)\WkTranslator.res"
	-@erase "$(INTDIR)\WkTranslatorDoc.obj"
	-@erase "$(INTDIR)\WkTranslatorView.obj"
	-@erase "$(OUTDIR)\WkTranslator.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)\WkTranslator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\WkTranslator.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WkTranslator.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\WkTranslator.pdb" /machine:I386\
 /out:"$(OUTDIR)\WkTranslator.exe" 
LINK32_OBJS= \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TranslateDialog.obj" \
	"$(INTDIR)\TranslateString.obj" \
	"$(INTDIR)\TSArrayArray.obj" \
	"$(INTDIR)\WkTranslator.obj" \
	"$(INTDIR)\WkTranslator.res" \
	"$(INTDIR)\WkTranslatorDoc.obj" \
	"$(INTDIR)\WkTranslatorView.obj"

"$(OUTDIR)\WkTranslator.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WkTranslator.exe"

!ELSE 

ALL : "$(OUTDIR)\WkTranslator.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TranslateDialog.obj"
	-@erase "$(INTDIR)\TranslateString.obj"
	-@erase "$(INTDIR)\TSArrayArray.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\WkTranslator.obj"
	-@erase "$(INTDIR)\WkTranslator.pch"
	-@erase "$(INTDIR)\WkTranslator.res"
	-@erase "$(INTDIR)\WkTranslatorDoc.obj"
	-@erase "$(INTDIR)\WkTranslatorView.obj"
	-@erase "$(OUTDIR)\WkTranslator.exe"
	-@erase "$(OUTDIR)\WkTranslator.ilk"
	-@erase "$(OUTDIR)\WkTranslator.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)\WkTranslator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\WkTranslator.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WkTranslator.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\WkTranslator.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\WkTranslator.exe" 
LINK32_OBJS= \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TranslateDialog.obj" \
	"$(INTDIR)\TranslateString.obj" \
	"$(INTDIR)\TSArrayArray.obj" \
	"$(INTDIR)\WkTranslator.obj" \
	"$(INTDIR)\WkTranslator.res" \
	"$(INTDIR)\WkTranslatorDoc.obj" \
	"$(INTDIR)\WkTranslatorView.obj"

"$(OUTDIR)\WkTranslator.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "WkTranslator - Win32 Release" || "$(CFG)" ==\
 "WkTranslator - Win32 Debug"
SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\StdAfx.h"\
	".\WkTranslator.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)\WkTranslator.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\WkTranslator.pch" : $(SOURCE)\
 $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\WkTranslator.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\WkTranslator.pch" : $(SOURCE)\
 $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TranslateDialog.cpp
DEP_CPP_TRANS=\
	".\StdAfx.h"\
	".\TranslateDialog.h"\
	".\WkTranslator.h"\
	

"$(INTDIR)\TranslateDialog.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


SOURCE=..\WKWizard\TranslateString.cpp
DEP_CPP_TRANSL=\
	"..\WKWizard\stdafx.h"\
	"..\WKWizard\TranslateString.h"\
	

"$(INTDIR)\TranslateString.obj" : $(SOURCE) $(DEP_CPP_TRANSL) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\TSArrayArray.cpp
DEP_CPP_TSARR=\
	"..\WkWizard\\TranslateString.h"\
	".\StdAfx.h"\
	".\TSArrayArray.h"\
	

"$(INTDIR)\TSArrayArray.obj" : $(SOURCE) $(DEP_CPP_TSARR) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


SOURCE=.\WkTranslator.cpp

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

DEP_CPP_WKTRA=\
	"..\WkWizard\\TranslateString.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	".\TSArrayArray.h"\
	".\WkTranslator.h"\
	".\WkTranslatorDoc.h"\
	".\WkTranslatorView.h"\
	

"$(INTDIR)\WkTranslator.obj" : $(SOURCE) $(DEP_CPP_WKTRA) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

DEP_CPP_WKTRA=\
	"..\WkWizard\\TranslateString.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	".\TSArrayArray.h"\
	".\WkTranslator.h"\
	".\WkTranslatorDoc.h"\
	".\WkTranslatorView.h"\
	

"$(INTDIR)\WkTranslator.obj" : $(SOURCE) $(DEP_CPP_WKTRA) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


!ENDIF 

SOURCE=.\WkTranslator.rc
DEP_RSC_WKTRAN=\
	".\res\Toolbar.bmp"\
	".\res\WkTranslator.ico"\
	".\res\WkTranslator.rc2"\
	".\res\WkTranslatorDoc.ico"\
	

"$(INTDIR)\WkTranslator.res" : $(SOURCE) $(DEP_RSC_WKTRAN) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\WkTranslatorDoc.cpp

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

DEP_CPP_WKTRANS=\
	"..\WkWizard\\TranslateString.h"\
	".\StdAfx.h"\
	".\TranslateDialog.h"\
	".\TSArrayArray.h"\
	".\WkTranslator.h"\
	".\WkTranslatorDoc.h"\
	

"$(INTDIR)\WkTranslatorDoc.obj" : $(SOURCE) $(DEP_CPP_WKTRANS) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

DEP_CPP_WKTRANS=\
	"..\WkWizard\\TranslateString.h"\
	".\StdAfx.h"\
	".\TranslateDialog.h"\
	".\TSArrayArray.h"\
	".\WkTranslator.h"\
	".\WkTranslatorDoc.h"\
	

"$(INTDIR)\WkTranslatorDoc.obj" : $(SOURCE) $(DEP_CPP_WKTRANS) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


!ENDIF 

SOURCE=.\WkTranslatorView.cpp

!IF  "$(CFG)" == "WkTranslator - Win32 Release"

DEP_CPP_WKTRANSL=\
	"..\WkWizard\\TranslateString.h"\
	".\StdAfx.h"\
	".\TSArrayArray.h"\
	".\WkTranslator.h"\
	".\WkTranslatorDoc.h"\
	".\WkTranslatorView.h"\
	

"$(INTDIR)\WkTranslatorView.obj" : $(SOURCE) $(DEP_CPP_WKTRANSL) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


!ELSEIF  "$(CFG)" == "WkTranslator - Win32 Debug"

DEP_CPP_WKTRANSL=\
	"..\WkWizard\\TranslateString.h"\
	".\StdAfx.h"\
	".\TSArrayArray.h"\
	".\WkTranslator.h"\
	".\WkTranslatorDoc.h"\
	".\WkTranslatorView.h"\
	

"$(INTDIR)\WkTranslatorView.obj" : $(SOURCE) $(DEP_CPP_WKTRANSL) "$(INTDIR)"\
 "$(INTDIR)\WkTranslator.pch"


!ENDIF 


!ENDIF 

