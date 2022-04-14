# Microsoft Developer Studio Generated NMAKE File, Based on ProductVersion.dsp
!IF "$(CFG)" == ""
CFG=ProductVersion - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ProductVersion - Win32\
 Debug.
!ENDIF 

!IF "$(CFG)" != "ProductVersion - Win32 Release" && "$(CFG)" !=\
 "ProductVersion - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProductVersion.mak" CFG="ProductVersion - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProductVersion - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ProductVersion - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ProductVersion - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ProductVersion.exe"

!ELSE 

ALL : "$(OUTDIR)\ProductVersion.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ProductVersion.obj"
	-@erase "$(INTDIR)\ProductVersion.pch"
	-@erase "$(INTDIR)\ProductVersion.res"
	-@erase "$(INTDIR)\ProductVersionDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\WK_VersionInfo.obj"
	-@erase "$(OUTDIR)\ProductVersion.exe"
	-@erase "$(OUTDIR)\ProductVersion.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\ProductVersion.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ProductVersion.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ProductVersion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Cipc.lib version.lib OemGui.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\ProductVersion.pdb"\
 /map:"$(INTDIR)\ProductVersion.map" /machine:I386\
 /out:"$(OUTDIR)\ProductVersion.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ProductVersion.obj" \
	"$(INTDIR)\ProductVersion.res" \
	"$(INTDIR)\ProductVersionDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WK_VersionInfo.obj"

"$(OUTDIR)\ProductVersion.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ProductVersion - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ProductVersion.exe" "$(OUTDIR)\ProductVersion.bsc"

!ELSE 

ALL : "$(OUTDIR)\ProductVersion.exe" "$(OUTDIR)\ProductVersion.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ProductVersion.obj"
	-@erase "$(INTDIR)\ProductVersion.pch"
	-@erase "$(INTDIR)\ProductVersion.res"
	-@erase "$(INTDIR)\ProductVersion.sbr"
	-@erase "$(INTDIR)\ProductVersionDlg.obj"
	-@erase "$(INTDIR)\ProductVersionDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\WK_VersionInfo.obj"
	-@erase "$(INTDIR)\WK_VersionInfo.sbr"
	-@erase "$(OUTDIR)\ProductVersion.bsc"
	-@erase "$(OUTDIR)\ProductVersion.exe"
	-@erase "$(OUTDIR)\ProductVersion.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ProductVersion.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ProductVersion.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ProductVersion.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ProductVersion.sbr" \
	"$(INTDIR)\ProductVersionDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\WK_VersionInfo.sbr"

"$(OUTDIR)\ProductVersion.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=CipcDebug.lib version.lib OemGuiD.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\ProductVersion.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\ProductVersion.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ProductVersion.obj" \
	"$(INTDIR)\ProductVersion.res" \
	"$(INTDIR)\ProductVersionDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WK_VersionInfo.obj"

"$(OUTDIR)\ProductVersion.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "ProductVersion - Win32 Release" || "$(CFG)" ==\
 "ProductVersion - Win32 Debug"
SOURCE=.\ProductVersion.cpp

!IF  "$(CFG)" == "ProductVersion - Win32 Release"

DEP_CPP_PRODU=\
	".\ProductVersion.h"\
	".\ProductVersionDlg.h"\
	".\WK_VersionInfo.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductVersion.obj" : $(SOURCE) $(DEP_CPP_PRODU) "$(INTDIR)"\
 "$(INTDIR)\ProductVersion.pch"


!ELSEIF  "$(CFG)" == "ProductVersion - Win32 Debug"

DEP_CPP_PRODU=\
	".\ProductVersion.h"\
	".\ProductVersionDlg.h"\
	".\WK_VersionInfo.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductVersion.obj"	"$(INTDIR)\ProductVersion.sbr" : $(SOURCE)\
 $(DEP_CPP_PRODU) "$(INTDIR)" "$(INTDIR)\ProductVersion.pch"


!ENDIF 

SOURCE=.\ProductVersion.rc
DEP_RSC_PRODUC=\
	".\res\ProductVersion.ico"\
	".\res\ProductVersion.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\ProductVersion.res" : $(SOURCE) $(DEP_RSC_PRODUC) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ProductVersionDlg.cpp

!IF  "$(CFG)" == "ProductVersion - Win32 Release"

DEP_CPP_PRODUCT=\
	".\ProductVersion.h"\
	".\ProductVersionDlg.h"\
	".\WK_VersionInfo.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductVersionDlg.obj" : $(SOURCE) $(DEP_CPP_PRODUCT) "$(INTDIR)"\
 "$(INTDIR)\ProductVersion.pch"


!ELSEIF  "$(CFG)" == "ProductVersion - Win32 Debug"

DEP_CPP_PRODUCT=\
	".\ProductVersion.h"\
	".\ProductVersionDlg.h"\
	".\WK_VersionInfo.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\ProductVersionDlg.obj"	"$(INTDIR)\ProductVersionDlg.sbr" : $(SOURCE)\
 $(DEP_CPP_PRODUCT) "$(INTDIR)" "$(INTDIR)\ProductVersion.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ProductVersion - Win32 Release"

CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\ProductVersion.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ProductVersion.pch" : $(SOURCE)\
 $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ProductVersion - Win32 Debug"

CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ProductVersion.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\ProductVersion.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\WK_VersionInfo.cpp

!IF  "$(CFG)" == "ProductVersion - Win32 Release"

DEP_CPP_WK_VE=\
	".\WK_VersionInfo.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\WK_VersionInfo.obj" : $(SOURCE) $(DEP_CPP_WK_VE) "$(INTDIR)"\
 "$(INTDIR)\ProductVersion.pch"


!ELSEIF  "$(CFG)" == "ProductVersion - Win32 Debug"

DEP_CPP_WK_VE=\
	".\WK_VersionInfo.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_file.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\lib\wkclasses\wk_wincpp.h"\
	

"$(INTDIR)\WK_VersionInfo.obj"	"$(INTDIR)\WK_VersionInfo.sbr" : $(SOURCE)\
 $(DEP_CPP_WK_VE) "$(INTDIR)" "$(INTDIR)\ProductVersion.pch"


!ENDIF 


!ENDIF 

