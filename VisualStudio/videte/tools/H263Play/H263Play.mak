# Microsoft Developer Studio Generated NMAKE File, Based on H263Play.dsp
!IF "$(CFG)" == ""
CFG=H263Play - Win32 Debug
!MESSAGE No configuration specified. Defaulting to H263Play - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "H263Play - Win32 Release" && "$(CFG)" !=\
 "H263Play - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "H263Play.mak" CFG="H263Play - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "H263Play - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "H263Play - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "H263Play - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\H263Play.exe"

!ELSE 

ALL : "$(OUTDIR)\H263Play.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\H263Child.obj"
	-@erase "$(INTDIR)\H263Play.obj"
	-@erase "$(INTDIR)\H263Play.pch"
	-@erase "$(INTDIR)\H263Play.res"
	-@erase "$(INTDIR)\H263PlayDoc.obj"
	-@erase "$(INTDIR)\H263PlayView.obj"
	-@erase "$(INTDIR)\JpegChild.obj"
	-@erase "$(INTDIR)\JpegDoc.obj"
	-@erase "$(INTDIR)\JpegView.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\H263Play.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\H263Play.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\H263Play.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\H263Play.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=h263dec.lib cdjpeg.lib IMAGN32.lib /nologo /subsystem:windows\
 /profile /machine:I386 /out:"$(OUTDIR)\H263Play.exe" 
LINK32_OBJS= \
	"$(INTDIR)\H263Child.obj" \
	"$(INTDIR)\H263Play.obj" \
	"$(INTDIR)\H263Play.res" \
	"$(INTDIR)\H263PlayDoc.obj" \
	"$(INTDIR)\H263PlayView.obj" \
	"$(INTDIR)\JpegChild.obj" \
	"$(INTDIR)\JpegDoc.obj" \
	"$(INTDIR)\JpegView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\H263Play.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\H263Play.exe" "$(OUTDIR)\H263Play.bsc"

!ELSE 

ALL : "$(OUTDIR)\H263Play.exe" "$(OUTDIR)\H263Play.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\H263Child.obj"
	-@erase "$(INTDIR)\H263Child.sbr"
	-@erase "$(INTDIR)\H263Play.obj"
	-@erase "$(INTDIR)\H263Play.pch"
	-@erase "$(INTDIR)\H263Play.res"
	-@erase "$(INTDIR)\H263Play.sbr"
	-@erase "$(INTDIR)\H263PlayDoc.obj"
	-@erase "$(INTDIR)\H263PlayDoc.sbr"
	-@erase "$(INTDIR)\H263PlayView.obj"
	-@erase "$(INTDIR)\H263PlayView.sbr"
	-@erase "$(INTDIR)\JpegChild.obj"
	-@erase "$(INTDIR)\JpegChild.sbr"
	-@erase "$(INTDIR)\JpegDoc.obj"
	-@erase "$(INTDIR)\JpegDoc.sbr"
	-@erase "$(INTDIR)\JpegView.obj"
	-@erase "$(INTDIR)\JpegView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\H263Play.bsc"
	-@erase "$(OUTDIR)\H263Play.exe"
	-@erase "$(OUTDIR)\H263Play.ilk"
	-@erase "$(OUTDIR)\H263Play.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\H263Play.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\H263Play.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\H263Play.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\H263Child.sbr" \
	"$(INTDIR)\H263Play.sbr" \
	"$(INTDIR)\H263PlayDoc.sbr" \
	"$(INTDIR)\H263PlayView.sbr" \
	"$(INTDIR)\JpegChild.sbr" \
	"$(INTDIR)\JpegDoc.sbr" \
	"$(INTDIR)\JpegView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\H263Play.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=h263decd.lib cdjpegd.lib IMAGN32.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\H263Play.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\H263Play.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\H263Child.obj" \
	"$(INTDIR)\H263Play.obj" \
	"$(INTDIR)\H263Play.res" \
	"$(INTDIR)\H263PlayDoc.obj" \
	"$(INTDIR)\H263PlayView.obj" \
	"$(INTDIR)\JpegChild.obj" \
	"$(INTDIR)\JpegDoc.obj" \
	"$(INTDIR)\JpegView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\H263Play.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "H263Play - Win32 Release" || "$(CFG)" ==\
 "H263Play - Win32 Debug"
SOURCE=.\H263Child.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_H263C=\
	".\H263Child.h"\
	".\H263Play.h"\
	

"$(INTDIR)\H263Child.obj" : $(SOURCE) $(DEP_CPP_H263C) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_H263C=\
	".\H263Child.h"\
	".\H263Play.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"h263dec\h263.h"\
	{$(INCLUDE)}"imagn.h"\
	

"$(INTDIR)\H263Child.obj"	"$(INTDIR)\H263Child.sbr" : $(SOURCE)\
 $(DEP_CPP_H263C) "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\H263Play.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_H263P=\
	".\H263Child.h"\
	".\H263Play.h"\
	".\H263PlayDoc.h"\
	".\H263PlayView.h"\
	".\JpegChild.h"\
	".\JpegDoc.h"\
	".\JpegView.h"\
	".\MainFrm.h"\
	

"$(INTDIR)\H263Play.obj" : $(SOURCE) $(DEP_CPP_H263P) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_H263P=\
	".\ChildFrm.h"\
	".\H263Play.h"\
	".\H263PlayDoc.h"\
	".\H263PlayView.h"\
	".\JpegDoc.h"\
	".\JpegView.h"\
	".\MainFrm.h"\
	

"$(INTDIR)\H263Play.obj"	"$(INTDIR)\H263Play.sbr" : $(SOURCE) $(DEP_CPP_H263P)\
 "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\H263Play.rc
DEP_RSC_H263PL=\
	".\res\H263Play.ico"\
	".\res\H263Play.rc2"\
	".\res\H263PlayDoc.ico"\
	".\res\h263type.bmp"\
	".\res\idr_jpeg.ico"\
	

"$(INTDIR)\H263Play.res" : $(SOURCE) $(DEP_RSC_H263PL) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\H263PlayDoc.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_H263PLA=\
	".\H263Play.h"\
	".\H263PlayDoc.h"\
	

"$(INTDIR)\H263PlayDoc.obj" : $(SOURCE) $(DEP_CPP_H263PLA) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_H263PLA=\
	".\H263Play.h"\
	".\H263PlayDoc.h"\
	

"$(INTDIR)\H263PlayDoc.obj"	"$(INTDIR)\H263PlayDoc.sbr" : $(SOURCE)\
 $(DEP_CPP_H263PLA) "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\H263PlayView.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_H263PLAY=\
	".\H263Play.h"\
	".\H263PlayDoc.h"\
	".\H263PlayView.h"\
	"v:\project\lib\cdjpeg\jpegencoder.h"\
	

"$(INTDIR)\H263PlayView.obj" : $(SOURCE) $(DEP_CPP_H263PLAY) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_H263PLAY=\
	".\H263Play.h"\
	".\H263PlayDoc.h"\
	".\H263PlayView.h"\
	"v:\project\lib\cdjpeg\jpegencoder.h"\
	

"$(INTDIR)\H263PlayView.obj"	"$(INTDIR)\H263PlayView.sbr" : $(SOURCE)\
 $(DEP_CPP_H263PLAY) "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\JpegChild.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_JPEGC=\
	".\H263Play.h"\
	".\JpegChild.h"\
	

"$(INTDIR)\JpegChild.obj" : $(SOURCE) $(DEP_CPP_JPEGC) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_JPEGC=\
	".\H263Play.h"\
	".\JpegChild.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"h263dec\h263.h"\
	{$(INCLUDE)}"imagn.h"\
	

"$(INTDIR)\JpegChild.obj"	"$(INTDIR)\JpegChild.sbr" : $(SOURCE)\
 $(DEP_CPP_JPEGC) "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\JpegDoc.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_JPEGD=\
	".\H263Play.h"\
	".\JpegDoc.h"\
	"v:\project\lib\cdjpeg\jpegdecoder.h"\
	"v:\project\lib\cdjpeg\jpegencoder.h"\
	

"$(INTDIR)\JpegDoc.obj" : $(SOURCE) $(DEP_CPP_JPEGD) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_JPEGD=\
	".\H263Play.h"\
	".\JpegDoc.h"\
	"v:\project\lib\cdjpeg\jpegdecoder.h"\
	"v:\project\lib\cdjpeg\jpegencoder.h"\
	

"$(INTDIR)\JpegDoc.obj"	"$(INTDIR)\JpegDoc.sbr" : $(SOURCE) $(DEP_CPP_JPEGD)\
 "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\JpegView.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_JPEGV=\
	".\H263Play.h"\
	".\JpegDoc.h"\
	".\JpegView.h"\
	

"$(INTDIR)\JpegView.obj" : $(SOURCE) $(DEP_CPP_JPEGV) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_JPEGV=\
	".\H263Play.h"\
	".\JpegDoc.h"\
	".\JpegView.h"\
	

"$(INTDIR)\JpegView.obj"	"$(INTDIR)\JpegView.sbr" : $(SOURCE) $(DEP_CPP_JPEGV)\
 "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\H263Play.h"\
	".\MainFrm.h"\
	

!IF  "$(CFG)" == "H263Play - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\H263Play.pch"


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"


"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)" "$(INTDIR)\H263Play.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "H263Play - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	{$(INCLUDE)}"h263dec\h263.h"\
	{$(INCLUDE)}"imagn.h"\
	
CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\H263Play.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\H263Play.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "H263Play - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	{$(INCLUDE)}"h263dec\h263.h"\
	{$(INCLUDE)}"imagn.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\H263Play.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\H263Play.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

