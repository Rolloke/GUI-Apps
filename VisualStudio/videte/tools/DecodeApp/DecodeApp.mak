# Microsoft Developer Studio Generated NMAKE File, Based on DecodeApp.dsp
!IF "$(CFG)" == ""
CFG=DecodeApp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to DecodeApp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DecodeApp - Win32 Release" && "$(CFG)" !=\
 "DecodeApp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DecodeApp.mak" CFG="DecodeApp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DecodeApp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DecodeApp - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "DecodeApp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\DecodeApp.exe"

!ELSE 

ALL : "$(OUTDIR)\DecodeApp.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\About.obj"
	-@erase "$(INTDIR)\cdump.obj"
	-@erase "$(INTDIR)\DecodeApp.obj"
	-@erase "$(INTDIR)\DecodeApp.pch"
	-@erase "$(INTDIR)\DecodeApp.res"
	-@erase "$(INTDIR)\DecodeAppDlg.obj"
	-@erase "$(INTDIR)\DecodeWindow.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\DecodeApp.exe"
	-@erase "$(OUTDIR)\DecodeApp.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /Zi /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DecodeApp.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\DecodeApp.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DecodeApp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /profile /map:"$(INTDIR)\DecodeApp.map"\
 /debug /machine:I386 /out:"$(OUTDIR)\DecodeApp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\About.obj" \
	"$(INTDIR)\cdump.obj" \
	"$(INTDIR)\DecodeApp.obj" \
	"$(INTDIR)\DecodeApp.res" \
	"$(INTDIR)\DecodeAppDlg.obj" \
	"$(INTDIR)\DecodeWindow.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\PTDecoder\Release\PTDecoder.lib"

"$(OUTDIR)\DecodeApp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DecodeApp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\DecodeApp.exe"

!ELSE 

ALL : "$(OUTDIR)\DecodeApp.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\About.obj"
	-@erase "$(INTDIR)\cdump.obj"
	-@erase "$(INTDIR)\DecodeApp.obj"
	-@erase "$(INTDIR)\DecodeApp.pch"
	-@erase "$(INTDIR)\DecodeApp.res"
	-@erase "$(INTDIR)\DecodeAppDlg.obj"
	-@erase "$(INTDIR)\DecodeWindow.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\DecodeApp.exe"
	-@erase "$(OUTDIR)\DecodeApp.ilk"
	-@erase "$(OUTDIR)\DecodeApp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DecodeApp.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\DecodeApp.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DecodeApp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\DecodeApp.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\DecodeApp.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\About.obj" \
	"$(INTDIR)\cdump.obj" \
	"$(INTDIR)\DecodeApp.obj" \
	"$(INTDIR)\DecodeApp.res" \
	"$(INTDIR)\DecodeAppDlg.obj" \
	"$(INTDIR)\DecodeWindow.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\PTDecoder\Debug\PTDecoderD.lib"

"$(OUTDIR)\DecodeApp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "DecodeApp - Win32 Release" || "$(CFG)" ==\
 "DecodeApp - Win32 Debug"
SOURCE=.\About.cpp
DEP_CPP_ABOUT=\
	".\About.h"\
	

"$(INTDIR)\About.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


SOURCE=.\cdump.cpp
DEP_CPP_CDUMP=\
	".\cdump.h"\
	

"$(INTDIR)\cdump.obj" : $(SOURCE) $(DEP_CPP_CDUMP) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


SOURCE=.\DecodeApp.cpp

!IF  "$(CFG)" == "DecodeApp - Win32 Release"

DEP_CPP_DECOD=\
	".\cdump.h"\
	".\DecodeApp.h"\
	".\DecodeAppDlg.h"\
	

"$(INTDIR)\DecodeApp.obj" : $(SOURCE) $(DEP_CPP_DECOD) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


!ELSEIF  "$(CFG)" == "DecodeApp - Win32 Debug"

DEP_CPP_DECOD=\
	".\cdump.h"\
	".\DecodeApp.h"\
	".\DecodeAppDlg.h"\
	

"$(INTDIR)\DecodeApp.obj" : $(SOURCE) $(DEP_CPP_DECOD) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


!ENDIF 

SOURCE=.\DecodeAppDlg.cpp

!IF  "$(CFG)" == "DecodeApp - Win32 Release"

DEP_CPP_DECODE=\
	"..\ptdecoder\ptdecoderclass.h"\
	".\About.h"\
	".\cdump.h"\
	".\DecodeApp.h"\
	".\DecodeAppDlg.h"\
	".\DecodeWindow.h"\
	

"$(INTDIR)\DecodeAppDlg.obj" : $(SOURCE) $(DEP_CPP_DECODE) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


!ELSEIF  "$(CFG)" == "DecodeApp - Win32 Debug"

DEP_CPP_DECODE=\
	"..\ptdecoder\ptdecoderclass.h"\
	".\About.h"\
	".\cdump.h"\
	".\DecodeApp.h"\
	".\DecodeAppDlg.h"\
	".\DecodeWindow.h"\
	

"$(INTDIR)\DecodeAppDlg.obj" : $(SOURCE) $(DEP_CPP_DECODE) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


!ENDIF 

SOURCE=.\DecodeWindow.cpp

!IF  "$(CFG)" == "DecodeApp - Win32 Release"

DEP_CPP_DECODEW=\
	"..\ptdecoder\ptdecoderclass.h"\
	".\DecodeApp.h"\
	".\DecodeWindow.h"\
	

"$(INTDIR)\DecodeWindow.obj" : $(SOURCE) $(DEP_CPP_DECODEW) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


!ELSEIF  "$(CFG)" == "DecodeApp - Win32 Debug"

DEP_CPP_DECODEW=\
	"..\ptdecoder\ptdecoderclass.h"\
	".\DecodeApp.h"\
	".\DecodeWindow.h"\
	

"$(INTDIR)\DecodeWindow.obj" : $(SOURCE) $(DEP_CPP_DECODEW) "$(INTDIR)"\
 "$(INTDIR)\DecodeApp.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DecodeApp - Win32 Release"

CPP_SWITCHES=/nologo /G5 /Zp1 /MD /W3 /GX /Zi /O2 /Ob2 /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DecodeApp.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DecodeApp.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "DecodeApp - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG"\
 /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DecodeApp.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DecodeApp.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\DecodeApp.rc
DEP_RSC_DECODEA=\
	".\res\DecodeApp.ico"\
	".\res\DecodeApp.rc2"\
	

"$(INTDIR)\DecodeApp.res" : $(SOURCE) $(DEP_RSC_DECODEA) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

