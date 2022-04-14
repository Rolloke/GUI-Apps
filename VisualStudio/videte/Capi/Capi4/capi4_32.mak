# Microsoft Developer Studio Generated NMAKE File, Based on capi4_32.dsp
!IF "$(CFG)" == ""
CFG=capi4_32 - Win32 Release
!MESSAGE No configuration specified. Defaulting to capi4_32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "capi4_32 - Win32 Release" && "$(CFG)" !=\
 "capi4_32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "capi4_32.mak" CFG="capi4_32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "capi4_32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "capi4_32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Capi4_32.lib" "$(OUTDIR)\capi4_32.bsc"

!ELSE 

ALL : "$(OUTDIR)\Capi4_32.lib" "$(OUTDIR)\capi4_32.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\20CONN.OBJ"
	-@erase "$(INTDIR)\20CONN.SBR"
	-@erase "$(INTDIR)\20MAIN.OBJ"
	-@erase "$(INTDIR)\20MAIN.SBR"
	-@erase "$(INTDIR)\20MSG.OBJ"
	-@erase "$(INTDIR)\20MSG.SBR"
	-@erase "$(INTDIR)\CAPI4.OBJ"
	-@erase "$(INTDIR)\CAPI4.SBR"
	-@erase "$(INTDIR)\COMMON.OBJ"
	-@erase "$(INTDIR)\COMMON.SBR"
	-@erase "$(INTDIR)\DATAQ.OBJ"
	-@erase "$(INTDIR)\DATAQ.SBR"
	-@erase "$(INTDIR)\DecodeCapiMessages.obj"
	-@erase "$(INTDIR)\DecodeCapiMessages.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\capi4_32.bsc"
	-@erase "$(OUTDIR)\Capi4_32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W4 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_CAPI20_" /D "_AFXDLL" /D "_MBCS" /Fr"$(INTDIR)\\"\
 /Fp"$(INTDIR)\capi4_32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\capi4_32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\20CONN.SBR" \
	"$(INTDIR)\20MAIN.SBR" \
	"$(INTDIR)\20MSG.SBR" \
	"$(INTDIR)\CAPI4.SBR" \
	"$(INTDIR)\COMMON.SBR" \
	"$(INTDIR)\DATAQ.SBR" \
	"$(INTDIR)\DecodeCapiMessages.sbr"

"$(OUTDIR)\capi4_32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\capi4_32.lib" 
LIB32_OBJS= \
	"$(INTDIR)\20CONN.OBJ" \
	"$(INTDIR)\20MAIN.OBJ" \
	"$(INTDIR)\20MSG.OBJ" \
	"$(INTDIR)\CAPI4.OBJ" \
	"$(INTDIR)\COMMON.OBJ" \
	"$(INTDIR)\DATAQ.OBJ" \
	"$(INTDIR)\DecodeCapiMessages.obj"

"$(OUTDIR)\Capi4_32.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Capi4_32_Debug.lib" "$(OUTDIR)\capi4_32.bsc"

!ELSE 

ALL : "$(OUTDIR)\Capi4_32_Debug.lib" "$(OUTDIR)\capi4_32.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\20CONN.OBJ"
	-@erase "$(INTDIR)\20CONN.SBR"
	-@erase "$(INTDIR)\20MAIN.OBJ"
	-@erase "$(INTDIR)\20MAIN.SBR"
	-@erase "$(INTDIR)\20MSG.OBJ"
	-@erase "$(INTDIR)\20MSG.SBR"
	-@erase "$(INTDIR)\CAPI4.OBJ"
	-@erase "$(INTDIR)\CAPI4.SBR"
	-@erase "$(INTDIR)\COMMON.OBJ"
	-@erase "$(INTDIR)\COMMON.SBR"
	-@erase "$(INTDIR)\DATAQ.OBJ"
	-@erase "$(INTDIR)\DATAQ.SBR"
	-@erase "$(INTDIR)\DecodeCapiMessages.obj"
	-@erase "$(INTDIR)\DecodeCapiMessages.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\capi4_32.bsc"
	-@erase "$(OUTDIR)\Capi4_32_Debug.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_CAPI20_" /D "_AFXDLL" /D "_MBCS" /Fr"$(INTDIR)\\" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\capi4_32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\20CONN.SBR" \
	"$(INTDIR)\20MAIN.SBR" \
	"$(INTDIR)\20MSG.SBR" \
	"$(INTDIR)\CAPI4.SBR" \
	"$(INTDIR)\COMMON.SBR" \
	"$(INTDIR)\DATAQ.SBR" \
	"$(INTDIR)\DecodeCapiMessages.sbr"

"$(OUTDIR)\capi4_32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Capi4_32_Debug.lib" 
LIB32_OBJS= \
	"$(INTDIR)\20CONN.OBJ" \
	"$(INTDIR)\20MAIN.OBJ" \
	"$(INTDIR)\20MSG.OBJ" \
	"$(INTDIR)\CAPI4.OBJ" \
	"$(INTDIR)\COMMON.OBJ" \
	"$(INTDIR)\DATAQ.OBJ" \
	"$(INTDIR)\DecodeCapiMessages.obj"

"$(OUTDIR)\Capi4_32_Debug.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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


!IF "$(CFG)" == "capi4_32 - Win32 Release" || "$(CFG)" ==\
 "capi4_32 - Win32 Debug"
SOURCE=.\20CONN.CPP

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

DEP_CPP_20CON=\
	".\20conn.h"\
	".\20def.h"\
	".\DecodeCapiMessages.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\20CONN.OBJ"	"$(INTDIR)\20CONN.SBR" : $(SOURCE) $(DEP_CPP_20CON)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

DEP_CPP_20CON=\
	".\20conn.h"\
	".\20def.h"\
	".\DecodeCapiMessages.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\20CONN.OBJ"	"$(INTDIR)\20CONN.SBR" : $(SOURCE) $(DEP_CPP_20CON)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\20MAIN.CPP

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

DEP_CPP_20MAI=\
	".\20capi.h"\
	".\20conn.h"\
	".\20def.h"\
	".\20info.h"\
	".\20main.h"\
	".\20msg.h"\
	".\dataq.h"\
	".\DecodeCapiMessages.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\abstractcapi.h"\
	"v:\project\include\capi4.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\20MAIN.OBJ"	"$(INTDIR)\20MAIN.SBR" : $(SOURCE) $(DEP_CPP_20MAI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

DEP_CPP_20MAI=\
	".\20capi.h"\
	".\20conn.h"\
	".\20def.h"\
	".\20info.h"\
	".\20main.h"\
	".\20msg.h"\
	".\dataq.h"\
	".\DecodeCapiMessages.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\abstractcapi.h"\
	"v:\project\include\capi4.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\20MAIN.OBJ"	"$(INTDIR)\20MAIN.SBR" : $(SOURCE) $(DEP_CPP_20MAI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\20MSG.CPP

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

DEP_CPP_20MSG=\
	".\20def.h"\
	".\20msg.h"\
	".\null.h"\
	

"$(INTDIR)\20MSG.OBJ"	"$(INTDIR)\20MSG.SBR" : $(SOURCE) $(DEP_CPP_20MSG)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

DEP_CPP_20MSG=\
	".\20def.h"\
	".\20msg.h"\
	".\null.h"\
	

"$(INTDIR)\20MSG.OBJ"	"$(INTDIR)\20MSG.SBR" : $(SOURCE) $(DEP_CPP_20MSG)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\CAPI4.CPP

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

DEP_CPP_CAPI4=\
	".\20capi.h"\
	".\20conn.h"\
	".\20def.h"\
	".\20info.h"\
	".\20main.h"\
	".\dataq.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\capi4.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\CAPI4.OBJ"	"$(INTDIR)\CAPI4.SBR" : $(SOURCE) $(DEP_CPP_CAPI4)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

DEP_CPP_CAPI4=\
	".\20capi.h"\
	".\20conn.h"\
	".\20def.h"\
	".\20info.h"\
	".\20main.h"\
	".\dataq.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\capi4.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\CAPI4.OBJ"	"$(INTDIR)\CAPI4.SBR" : $(SOURCE) $(DEP_CPP_CAPI4)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\COMMON.CPP

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

DEP_CPP_COMMO=\
	".\20capi.h"\
	".\20conn.h"\
	".\20def.h"\
	".\20info.h"\
	".\20main.h"\
	".\dataq.h"\
	".\DecodeCapiMessages.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\abstractcapi.h"\
	"v:\project\include\capi4.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\COMMON.OBJ"	"$(INTDIR)\COMMON.SBR" : $(SOURCE) $(DEP_CPP_COMMO)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

DEP_CPP_COMMO=\
	".\20capi.h"\
	".\20conn.h"\
	".\20def.h"\
	".\20info.h"\
	".\20main.h"\
	".\dataq.h"\
	".\DecodeCapiMessages.h"\
	".\LimitedString.h"\
	".\null.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\abstractcapi.h"\
	"v:\project\include\capi4.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\COMMON.OBJ"	"$(INTDIR)\COMMON.SBR" : $(SOURCE) $(DEP_CPP_COMMO)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\DATAQ.CPP

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

DEP_CPP_DATAQ=\
	".\dataq.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\DATAQ.OBJ"	"$(INTDIR)\DATAQ.SBR" : $(SOURCE) $(DEP_CPP_DATAQ)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

DEP_CPP_DATAQ=\
	".\dataq.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\hdr.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\DATAQ.OBJ"	"$(INTDIR)\DATAQ.SBR" : $(SOURCE) $(DEP_CPP_DATAQ)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\DecodeCapiMessages.cpp
DEP_CPP_DECOD=\
	".\DecodeCapiMessages.h"\
	

"$(INTDIR)\DecodeCapiMessages.obj"	"$(INTDIR)\DecodeCapiMessages.sbr" : \
$(SOURCE) $(DEP_CPP_DECOD) "$(INTDIR)"



!ENDIF 

