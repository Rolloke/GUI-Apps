# Microsoft Developer Studio Generated NMAKE File, Based on PrintVersion.dsp
!IF "$(CFG)" == ""
CFG=PrintVersion - Win32 Release
!MESSAGE No configuration specified. Defaulting to PrintVersion - Win32\
 Release.
!ENDIF 

!IF "$(CFG)" != "PrintVersion - Win32 Release" && "$(CFG)" !=\
 "PrintVersion - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PrintVersion.mak" CFG="PrintVersion - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PrintVersion - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "PrintVersion - Win32 Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PrintVersion - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PrintVersion.exe"

!ELSE 

ALL : "$(OUTDIR)\PrintVersion.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\PrintVersion.obj"
	-@erase "$(OUTDIR)\PrintVersion.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\PrintVersion.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PrintVersion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=version.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\PrintVersion.pdb" /machine:I386\
 /out:"$(OUTDIR)\PrintVersion.exe" 
LINK32_OBJS= \
	"$(INTDIR)\PrintVersion.obj"

"$(OUTDIR)\PrintVersion.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PrintVersion - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PrintVersion.exe"

!ELSE 

ALL : "$(OUTDIR)\PrintVersion.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\PrintVersion.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\PrintVersion.exe"
	-@erase "$(OUTDIR)\PrintVersion.ilk"
	-@erase "$(OUTDIR)\PrintVersion.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\PrintVersion.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PrintVersion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=version.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\PrintVersion.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\PrintVersion.exe" 
LINK32_OBJS= \
	"$(INTDIR)\PrintVersion.obj"

"$(OUTDIR)\PrintVersion.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "PrintVersion - Win32 Release" || "$(CFG)" ==\
 "PrintVersion - Win32 Debug"
SOURCE=.\PrintVersion.cpp

"$(INTDIR)\PrintVersion.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

