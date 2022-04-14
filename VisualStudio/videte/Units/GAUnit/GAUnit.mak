# Microsoft Developer Studio Generated NMAKE File, Based on GAUnit.dsp
!IF "$(CFG)" == ""
CFG=GAUnit - Win32 Release
!MESSAGE No configuration specified. Defaulting to GAUnit - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "GAUnit - Win32 Release" && "$(CFG)" != "GAUnit - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GAUnit.mak" CFG="GAUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GAUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GAUnit - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "GAUnit - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\GAUnit.exe" "$(OUTDIR)\GAUnit.bsc"

!ELSE 

ALL : "$(OUTDIR)\GAUnit.exe" "$(OUTDIR)\GAUnit.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CRS232.obj"
	-@erase "$(INTDIR)\CRS232.sbr"
	-@erase "$(INTDIR)\GAInput.obj"
	-@erase "$(INTDIR)\GAInput.sbr"
	-@erase "$(INTDIR)\GAUnit.obj"
	-@erase "$(INTDIR)\GAUnit.pch"
	-@erase "$(INTDIR)\GAUnit.res"
	-@erase "$(INTDIR)\GAUnit.sbr"
	-@erase "$(INTDIR)\GAWindow.obj"
	-@erase "$(INTDIR)\GAWindow.sbr"
	-@erase "$(INTDIR)\GeldDaten.obj"
	-@erase "$(INTDIR)\GeldDaten.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\GAUnit.bsc"
	-@erase "$(OUTDIR)\GAUnit.exe"
	-@erase "$(OUTDIR)\GAUnit.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /D "USE_DEBUGGER" /D "_GARNY" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\GAUnit.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\GAUnit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GAUnit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CRS232.sbr" \
	"$(INTDIR)\GAInput.sbr" \
	"$(INTDIR)\GAUnit.sbr" \
	"$(INTDIR)\GAWindow.sbr" \
	"$(INTDIR)\GeldDaten.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\GAUnit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=cipc.lib oemgui.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\GAUnit.pdb" /map:"$(INTDIR)\GAUnit.map" /machine:I386\
 /out:"$(OUTDIR)\GAUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CRS232.obj" \
	"$(INTDIR)\GAInput.obj" \
	"$(INTDIR)\GAUnit.obj" \
	"$(INTDIR)\GAUnit.res" \
	"$(INTDIR)\GAWindow.obj" \
	"$(INTDIR)\GeldDaten.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\GAUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GAUnit - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\GAUnit.exe" "$(OUTDIR)\GAUnit.bsc"

!ELSE 

ALL : "$(OUTDIR)\GAUnit.exe" "$(OUTDIR)\GAUnit.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CRS232.obj"
	-@erase "$(INTDIR)\CRS232.sbr"
	-@erase "$(INTDIR)\GAInput.obj"
	-@erase "$(INTDIR)\GAInput.sbr"
	-@erase "$(INTDIR)\GAUnit.obj"
	-@erase "$(INTDIR)\GAUnit.pch"
	-@erase "$(INTDIR)\GAUnit.res"
	-@erase "$(INTDIR)\GAUnit.sbr"
	-@erase "$(INTDIR)\GAWindow.obj"
	-@erase "$(INTDIR)\GAWindow.sbr"
	-@erase "$(INTDIR)\GeldDaten.obj"
	-@erase "$(INTDIR)\GeldDaten.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\GAUnit.bsc"
	-@erase "$(OUTDIR)\GAUnit.exe"
	-@erase "$(OUTDIR)\GAUnit.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "USE_DEBUGGER" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\GAUnit.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\GAUnit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GAUnit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CRS232.sbr" \
	"$(INTDIR)\GAInput.sbr" \
	"$(INTDIR)\GAUnit.sbr" \
	"$(INTDIR)\GAWindow.sbr" \
	"$(INTDIR)\GeldDaten.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\GAUnit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=cipcdebug.lib oemguid.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\GAUnit.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\GAUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CRS232.obj" \
	"$(INTDIR)\GAInput.obj" \
	"$(INTDIR)\GAUnit.obj" \
	"$(INTDIR)\GAUnit.res" \
	"$(INTDIR)\GAWindow.obj" \
	"$(INTDIR)\GeldDaten.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\GAUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "GAUnit - Win32 Release" || "$(CFG)" == "GAUnit - Win32 Debug"
SOURCE=.\CRS232.cpp

!IF  "$(CFG)" == "GAUnit - Win32 Release"

DEP_CPP_CRS23=\
	".\CRS232.h"\
	".\GAUnit.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\CRS232.obj"	"$(INTDIR)\CRS232.sbr" : $(SOURCE) $(DEP_CPP_CRS23)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ELSEIF  "$(CFG)" == "GAUnit - Win32 Debug"

DEP_CPP_CRS23=\
	".\CRS232.h"\
	".\GAUnit.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\CRS232.obj"	"$(INTDIR)\CRS232.sbr" : $(SOURCE) $(DEP_CPP_CRS23)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ENDIF 

SOURCE=.\GAInput.cpp

!IF  "$(CFG)" == "GAUnit - Win32 Release"

DEP_CPP_GAINP=\
	".\GAInput.h"\
	".\GAUnit.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\GAInput.obj"	"$(INTDIR)\GAInput.sbr" : $(SOURCE) $(DEP_CPP_GAINP)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ELSEIF  "$(CFG)" == "GAUnit - Win32 Debug"

DEP_CPP_GAINP=\
	".\GAInput.h"\
	".\GAUnit.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\GAInput.obj"	"$(INTDIR)\GAInput.sbr" : $(SOURCE) $(DEP_CPP_GAINP)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ENDIF 

SOURCE=.\GAUnit.cpp

!IF  "$(CFG)" == "GAUnit - Win32 Release"

DEP_CPP_GAUNI=\
	".\CRS232.h"\
	".\GAInput.h"\
	".\GAUnit.h"\
	".\GAWindow.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\GAUnit.obj"	"$(INTDIR)\GAUnit.sbr" : $(SOURCE) $(DEP_CPP_GAUNI)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ELSEIF  "$(CFG)" == "GAUnit - Win32 Debug"

DEP_CPP_GAUNI=\
	".\CRS232.h"\
	".\GAInput.h"\
	".\GAUnit.h"\
	".\GAWindow.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\GAUnit.obj"	"$(INTDIR)\GAUnit.sbr" : $(SOURCE) $(DEP_CPP_GAUNI)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ENDIF 

SOURCE=.\GAUnit.rc
DEP_RSC_GAUNIT=\
	".\res\GAUnit.ico"\
	".\res\GAUnit.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\GAUnit.res" : $(SOURCE) $(DEP_RSC_GAUNIT) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\GAWindow.cpp

!IF  "$(CFG)" == "GAUnit - Win32 Release"

DEP_CPP_GAWIN=\
	".\GAUnit.h"\
	".\GAWindow.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\GAWindow.obj"	"$(INTDIR)\GAWindow.sbr" : $(SOURCE) $(DEP_CPP_GAWIN)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ELSEIF  "$(CFG)" == "GAUnit - Win32 Debug"

DEP_CPP_GAWIN=\
	".\GAUnit.h"\
	".\GAWindow.h"\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\GAWindow.obj"	"$(INTDIR)\GAWindow.sbr" : $(SOURCE) $(DEP_CPP_GAWIN)\
 "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


!ENDIF 

SOURCE=.\GeldDaten.cpp
DEP_CPP_GELDD=\
	".\GeldDaten.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\GeldDaten.obj"	"$(INTDIR)\GeldDaten.sbr" : $(SOURCE)\
 $(DEP_CPP_GELDD) "$(INTDIR)" "$(INTDIR)\GAUnit.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "GAUnit - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\util.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_debugoptions.h"\
	"v:\project\include\wk_names.h"\
	"v:\project\lib\oemgui\\oemguiapi.h"\
	
CPP_SWITCHES=/nologo /G5 /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "USE_DEBUGGER" /D "_GARNY"\
 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\GAUnit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\GAUnit.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "GAUnit - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\util.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\idipapi.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_debugoptions.h"\
	"v:\project\include\wk_names.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "USE_DEBUGGER" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\GAUnit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\GAUnit.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

