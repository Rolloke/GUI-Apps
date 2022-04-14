# Microsoft Developer Studio Generated NMAKE File, Based on ProductView.dsp
!IF "$(CFG)" == ""
CFG=ProductView - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ProductView - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ProductView - Win32 Release" && "$(CFG)" !=\
 "ProductView - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProductView.mak" CFG="ProductView - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProductView - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ProductView - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "ProductView - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ProductView.exe"

!ELSE 

ALL : "$(OUTDIR)\ProductView.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\DialogDongle.obj"
	-@erase "$(INTDIR)\ErrorDialog.obj"
	-@erase "$(INTDIR)\ProductView.obj"
	-@erase "$(INTDIR)\ProductView.pch"
	-@erase "$(INTDIR)\ProductView.res"
	-@erase "$(INTDIR)\ProductViewDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\ProductView.exe"
	-@erase "$(OUTDIR)\ProductView.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\ProductView.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ProductView.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ProductView.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Cipc.lib OemGui.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\ProductView.pdb" /map:"$(INTDIR)\ProductView.map" /machine:I386\
 /out:"$(OUTDIR)\ProductView.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DialogDongle.obj" \
	"$(INTDIR)\ErrorDialog.obj" \
	"$(INTDIR)\ProductView.obj" \
	"$(INTDIR)\ProductView.res" \
	"$(INTDIR)\ProductViewDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\ProductView.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ProductView - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ProductView.exe" "$(OUTDIR)\ProductView.bsc"

!ELSE 

ALL : "$(OUTDIR)\ProductView.exe" "$(OUTDIR)\ProductView.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\DialogDongle.obj"
	-@erase "$(INTDIR)\DialogDongle.sbr"
	-@erase "$(INTDIR)\ErrorDialog.obj"
	-@erase "$(INTDIR)\ErrorDialog.sbr"
	-@erase "$(INTDIR)\ProductView.obj"
	-@erase "$(INTDIR)\ProductView.pch"
	-@erase "$(INTDIR)\ProductView.res"
	-@erase "$(INTDIR)\ProductView.sbr"
	-@erase "$(INTDIR)\ProductViewDlg.obj"
	-@erase "$(INTDIR)\ProductViewDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\ProductView.bsc"
	-@erase "$(OUTDIR)\ProductView.exe"
	-@erase "$(OUTDIR)\ProductView.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ProductView.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ProductView.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ProductView.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DialogDongle.sbr" \
	"$(INTDIR)\ErrorDialog.sbr" \
	"$(INTDIR)\ProductView.sbr" \
	"$(INTDIR)\ProductViewDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\ProductView.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=CipcDebug.lib OemGuiD.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\ProductView.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\ProductView.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\DialogDongle.obj" \
	"$(INTDIR)\ErrorDialog.obj" \
	"$(INTDIR)\ProductView.obj" \
	"$(INTDIR)\ProductView.res" \
	"$(INTDIR)\ProductViewDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\ProductView.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "ProductView - Win32 Release" || "$(CFG)" ==\
 "ProductView - Win32 Debug"
SOURCE=.\DialogDongle.cpp

!IF  "$(CFG)" == "ProductView - Win32 Release"

DEP_CPP_DIALO=\
	".\DialogDongle.h"\
	".\ProductView.h"\
	".\ProductViewDlg.h"\
	"v:\project\cipc\alldongletransfers.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\DialogDongle.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ProductView.pch"


!ELSEIF  "$(CFG)" == "ProductView - Win32 Debug"

DEP_CPP_DIALO=\
	".\DialogDongle.h"\
	".\ProductView.h"\
	".\ProductViewDlg.h"\
	"v:\project\cipc\alldongletransfers.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\DialogDongle.obj"	"$(INTDIR)\DialogDongle.sbr" : $(SOURCE)\
 $(DEP_CPP_DIALO) "$(INTDIR)" "$(INTDIR)\ProductView.pch"


!ENDIF 

SOURCE=.\ErrorDialog.cpp

!IF  "$(CFG)" == "ProductView - Win32 Release"

DEP_CPP_ERROR=\
	".\ErrorDialog.h"\
	".\ProductView.h"\
	

"$(INTDIR)\ErrorDialog.obj" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"\
 "$(INTDIR)\ProductView.pch"


!ELSEIF  "$(CFG)" == "ProductView - Win32 Debug"

DEP_CPP_ERROR=\
	".\ErrorDialog.h"\
	".\ProductView.h"\
	

"$(INTDIR)\ErrorDialog.obj"	"$(INTDIR)\ErrorDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_ERROR) "$(INTDIR)" "$(INTDIR)\ProductView.pch"


!ENDIF 

SOURCE=.\ProductView.cpp

!IF  "$(CFG)" == "ProductView - Win32 Release"

DEP_CPP_PRODU=\
	".\DialogDongle.h"\
	".\ErrorDialog.h"\
	".\ProductView.h"\
	".\ProductViewDlg.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductView.obj" : $(SOURCE) $(DEP_CPP_PRODU) "$(INTDIR)"\
 "$(INTDIR)\ProductView.pch"


!ELSEIF  "$(CFG)" == "ProductView - Win32 Debug"

DEP_CPP_PRODU=\
	".\DialogDongle.h"\
	".\ErrorDialog.h"\
	".\ProductView.h"\
	".\ProductViewDlg.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductView.obj"	"$(INTDIR)\ProductView.sbr" : $(SOURCE)\
 $(DEP_CPP_PRODU) "$(INTDIR)" "$(INTDIR)\ProductView.pch"


!ENDIF 

SOURCE=.\ProductView.rc
DEP_RSC_PRODUC=\
	".\res\ProductView.ico"\
	".\res\ProductView.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\ProductView.res" : $(SOURCE) $(DEP_RSC_PRODUC) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ProductViewDlg.cpp

!IF  "$(CFG)" == "ProductView - Win32 Release"

DEP_CPP_PRODUCT=\
	".\DialogDongle.h"\
	".\ProductView.h"\
	".\ProductViewDlg.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductViewDlg.obj" : $(SOURCE) $(DEP_CPP_PRODUCT) "$(INTDIR)"\
 "$(INTDIR)\ProductView.pch"


!ELSEIF  "$(CFG)" == "ProductView - Win32 Debug"

DEP_CPP_PRODUCT=\
	".\DialogDongle.h"\
	".\ProductView.h"\
	".\ProductViewDlg.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductViewDlg.obj"	"$(INTDIR)\ProductViewDlg.sbr" : $(SOURCE)\
 $(DEP_CPP_PRODUCT) "$(INTDIR)" "$(INTDIR)\ProductView.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ProductView - Win32 Release"

CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\ProductView.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ProductView.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ProductView - Win32 Debug"

CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ProductView.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\ProductView.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

