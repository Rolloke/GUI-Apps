# Microsoft Developer Studio Generated NMAKE File, Based on MiCoDemo.dsp
!IF "$(CFG)" == ""
CFG=MiCoDemo - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MiCoDemo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MiCoDemo - Win32 Release" && "$(CFG)" !=\
 "MiCoDemo - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MiCoDemo.mak" CFG="MiCoDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MiCoDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MiCoDemo - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\MiCoDemo.exe"

!ELSE 

ALL : "$(OUTDIR)\MiCoDemo.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CAlarm.obj"
	-@erase "$(INTDIR)\CBT829.obj"
	-@erase "$(INTDIR)\cbt856.obj"
	-@erase "$(INTDIR)\CI2C.obj"
	-@erase "$(INTDIR)\CIo.obj"
	-@erase "$(INTDIR)\CMico.obj"
	-@erase "$(INTDIR)\crelay.obj"
	-@erase "$(INTDIR)\CZoran.obj"
	-@erase "$(INTDIR)\Dlgcolor.obj"
	-@erase "$(INTDIR)\MicoUnit.res"
	-@erase "$(INTDIR)\MiCoUnitApp.obj"
	-@erase "$(INTDIR)\MiCoUnitDlg.obj"
	-@erase "$(INTDIR)\Settings.obj"
	-@erase "$(INTDIR)\StandAlone.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\zoranio.obj"
	-@erase "$(INTDIR)\Zr050.obj"
	-@erase "$(OUTDIR)\MiCoDemo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\MiCoDemo.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\MicoUnit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MiCoDemo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\MiCoDemo.pdb" /machine:I386 /out:"$(OUTDIR)\MiCoDemo.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CAlarm.obj" \
	"$(INTDIR)\CBT829.obj" \
	"$(INTDIR)\cbt856.obj" \
	"$(INTDIR)\CI2C.obj" \
	"$(INTDIR)\CIo.obj" \
	"$(INTDIR)\CMico.obj" \
	"$(INTDIR)\crelay.obj" \
	"$(INTDIR)\CZoran.obj" \
	"$(INTDIR)\Dlgcolor.obj" \
	"$(INTDIR)\MicoUnit.res" \
	"$(INTDIR)\MiCoUnitApp.obj" \
	"$(INTDIR)\MiCoUnitDlg.obj" \
	"$(INTDIR)\Settings.obj" \
	"$(INTDIR)\StandAlone.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\zoranio.obj" \
	"$(INTDIR)\Zr050.obj"

"$(OUTDIR)\MiCoDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\MiCoDemo.exe"

!ELSE 

ALL : "$(OUTDIR)\MiCoDemo.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CAlarm.obj"
	-@erase "$(INTDIR)\CBT829.obj"
	-@erase "$(INTDIR)\cbt856.obj"
	-@erase "$(INTDIR)\CI2C.obj"
	-@erase "$(INTDIR)\CIo.obj"
	-@erase "$(INTDIR)\CMico.obj"
	-@erase "$(INTDIR)\crelay.obj"
	-@erase "$(INTDIR)\CZoran.obj"
	-@erase "$(INTDIR)\Dlgcolor.obj"
	-@erase "$(INTDIR)\MicoUnit.res"
	-@erase "$(INTDIR)\MiCoUnitApp.obj"
	-@erase "$(INTDIR)\MiCoUnitDlg.obj"
	-@erase "$(INTDIR)\Settings.obj"
	-@erase "$(INTDIR)\StandAlone.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\zoranio.obj"
	-@erase "$(INTDIR)\Zr050.obj"
	-@erase "$(OUTDIR)\MiCoDemo.exe"
	-@erase "$(OUTDIR)\MiCoDemo.map"
	-@erase "$(OUTDIR)\MiCoDemo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /Zp1 /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)\MiCoDemo.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\MicoUnit.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MiCoDemo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\MiCoDemo.pdb" /map:"$(INTDIR)\MiCoDemo.map" /debug\
 /debugtype:cv /machine:I386 /out:"$(OUTDIR)\MiCoDemo.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CAlarm.obj" \
	"$(INTDIR)\CBT829.obj" \
	"$(INTDIR)\cbt856.obj" \
	"$(INTDIR)\CI2C.obj" \
	"$(INTDIR)\CIo.obj" \
	"$(INTDIR)\CMico.obj" \
	"$(INTDIR)\crelay.obj" \
	"$(INTDIR)\CZoran.obj" \
	"$(INTDIR)\Dlgcolor.obj" \
	"$(INTDIR)\MicoUnit.res" \
	"$(INTDIR)\MiCoUnitApp.obj" \
	"$(INTDIR)\MiCoUnitDlg.obj" \
	"$(INTDIR)\Settings.obj" \
	"$(INTDIR)\StandAlone.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\zoranio.obj" \
	"$(INTDIR)\Zr050.obj"

"$(OUTDIR)\MiCoDemo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "MiCoDemo - Win32 Release" || "$(CFG)" ==\
 "MiCoDemo - Win32 Debug"
SOURCE=.\CAlarm.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CALAR=\
	".\CAlarm.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CAlarm.obj" : $(SOURCE) $(DEP_CPP_CALAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CALAR=\
	".\CAlarm.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CAlarm.obj" : $(SOURCE) $(DEP_CPP_CALAR) "$(INTDIR)"


!ENDIF 

SOURCE=.\CBT829.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CBT82=\
	".\CBT829.h"\
	".\CI2C.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CBT829.obj" : $(SOURCE) $(DEP_CPP_CBT82) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CBT82=\
	".\CBT829.h"\
	".\CI2C.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CBT829.obj" : $(SOURCE) $(DEP_CPP_CBT82) "$(INTDIR)"


!ENDIF 

SOURCE=.\cbt856.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CBT85=\
	".\cbt856.h"\
	".\CI2C.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\cbt856.obj" : $(SOURCE) $(DEP_CPP_CBT85) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CBT85=\
	".\cbt856.h"\
	".\CI2C.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\cbt856.obj" : $(SOURCE) $(DEP_CPP_CBT85) "$(INTDIR)"


!ENDIF 

SOURCE=.\CI2C.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CI2C_=\
	".\CI2C.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CI2C.obj" : $(SOURCE) $(DEP_CPP_CI2C_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CI2C_=\
	".\CI2C.h"\
	".\CIO.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CI2C.obj" : $(SOURCE) $(DEP_CPP_CI2C_) "$(INTDIR)"


!ENDIF 

SOURCE=.\CIo.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CIO_C=\
	".\CIO.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CIo.obj" : $(SOURCE) $(DEP_CPP_CIO_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CIO_C=\
	".\CIO.h"\
	".\stdafx.h"\
	

"$(INTDIR)\CIo.obj" : $(SOURCE) $(DEP_CPP_CIO_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\CMico.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CMICO=\
	".\CAlarm.h"\
	".\CBT829.h"\
	".\cbt856.h"\
	".\CI2C.h"\
	".\CIO.h"\
	".\CMico.h"\
	".\CRelay.h"\
	".\CZoran.h"\
	".\Dlgcolor.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	".\ZoranIo.h"\
	".\Zr050.h"\
	".\Zr050_io.h"\
	

"$(INTDIR)\CMico.obj" : $(SOURCE) $(DEP_CPP_CMICO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CMICO=\
	".\CAlarm.h"\
	".\CBT829.h"\
	".\cbt856.h"\
	".\CI2C.h"\
	".\CIO.h"\
	".\CMico.h"\
	".\CRelay.h"\
	".\CZoran.h"\
	".\Dlgcolor.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	".\ZoranIo.h"\
	".\Zr050.h"\
	".\Zr050_io.h"\
	

"$(INTDIR)\CMico.obj" : $(SOURCE) $(DEP_CPP_CMICO) "$(INTDIR)"


!ENDIF 

SOURCE=.\crelay.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CRELA=\
	".\CIO.h"\
	".\CRelay.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\crelay.obj" : $(SOURCE) $(DEP_CPP_CRELA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CRELA=\
	".\CIO.h"\
	".\CRelay.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\crelay.obj" : $(SOURCE) $(DEP_CPP_CRELA) "$(INTDIR)"


!ENDIF 

SOURCE=.\CZoran.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_CZORA=\
	".\CIO.h"\
	".\CZoran.h"\
	".\H55_tbls.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	".\ZoranIo.h"\
	".\Zr050.h"\
	".\Zr050_io.h"\
	

"$(INTDIR)\CZoran.obj" : $(SOURCE) $(DEP_CPP_CZORA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_CZORA=\
	".\CIO.h"\
	".\CZoran.h"\
	".\H55_tbls.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	".\ZoranIo.h"\
	".\Zr050.h"\
	".\Zr050_io.h"\
	

"$(INTDIR)\CZoran.obj" : $(SOURCE) $(DEP_CPP_CZORA) "$(INTDIR)"


!ENDIF 

SOURCE=.\Dlgcolor.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_DLGCO=\
	".\CIO.h"\
	".\CMico.h"\
	".\Dlgcolor.h"\
	".\MicoDefs.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Dlgcolor.obj" : $(SOURCE) $(DEP_CPP_DLGCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_DLGCO=\
	".\CIO.h"\
	".\CMico.h"\
	".\Dlgcolor.h"\
	".\MicoDefs.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Dlgcolor.obj" : $(SOURCE) $(DEP_CPP_DLGCO) "$(INTDIR)"


!ENDIF 

SOURCE=.\MiCoUnitApp.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_MICOU=\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\MiCoUnitApp.h"\
	".\MiCoUnitDlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MiCoUnitApp.obj" : $(SOURCE) $(DEP_CPP_MICOU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_MICOU=\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\MiCoUnitApp.h"\
	".\MiCoUnitDlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MiCoUnitApp.obj" : $(SOURCE) $(DEP_CPP_MICOU) "$(INTDIR)"


!ENDIF 

SOURCE=.\MiCoUnitDlg.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_MICOUN=\
	".\CIO.h"\
	".\CMico.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\MiCoUnitApp.h"\
	".\MiCoUnitDlg.h"\
	".\Settings.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MiCoUnitDlg.obj" : $(SOURCE) $(DEP_CPP_MICOUN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_MICOUN=\
	".\CIO.h"\
	".\CMico.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\MiCoUnitApp.h"\
	".\MiCoUnitDlg.h"\
	".\Settings.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MiCoUnitDlg.obj" : $(SOURCE) $(DEP_CPP_MICOUN) "$(INTDIR)"


!ENDIF 

SOURCE=.\Settings.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_SETTI=\
	".\CIO.h"\
	".\CMico.h"\
	".\MicoDefs.h"\
	".\MiCoUnitApp.h"\
	".\Settings.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_SETTI=\
	".\CIO.h"\
	".\CMico.h"\
	".\MicoDefs.h"\
	".\MiCoUnitApp.h"\
	".\Settings.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"


!ENDIF 

SOURCE=.\StandAlone.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_STAND=\
	".\CIO.h"\
	".\CMico.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\MiCoUnitApp.h"\
	".\MiCoUnitDlg.h"\
	".\Settings.h"\
	".\stdafx.h"\
	

"$(INTDIR)\StandAlone.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_STAND=\
	".\CIO.h"\
	".\CMico.h"\
	".\MicoDefs.h"\
	".\MiCoReg.h"\
	".\MiCoUnitApp.h"\
	".\MiCoUnitDlg.h"\
	".\Settings.h"\
	".\stdafx.h"\
	

"$(INTDIR)\StandAlone.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"


!ENDIF 

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


SOURCE=.\zoranio.cpp

!IF  "$(CFG)" == "MiCoDemo - Win32 Release"

DEP_CPP_ZORAN=\
	".\CIO.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	".\ZoranIo.h"\
	".\Zr050_io.h"\
	

"$(INTDIR)\zoranio.obj" : $(SOURCE) $(DEP_CPP_ZORAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MiCoDemo - Win32 Debug"

DEP_CPP_ZORAN=\
	".\CIO.h"\
	".\MiCoReg.h"\
	".\stdafx.h"\
	".\ZoranIo.h"\
	".\Zr050_io.h"\
	

"$(INTDIR)\zoranio.obj" : $(SOURCE) $(DEP_CPP_ZORAN) "$(INTDIR)"


!ENDIF 

SOURCE=.\Zr050.cpp
DEP_CPP_ZR050=\
	".\Zr050.h"\
	".\Zr050_io.h"\
	

"$(INTDIR)\Zr050.obj" : $(SOURCE) $(DEP_CPP_ZR050) "$(INTDIR)"


SOURCE=.\MicoUnit.rc
DEP_RSC_MICOUNI=\
	".\res\MiCoUnit.ico"\
	".\res\MicoUnit.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\MicoUnit.res" : $(SOURCE) $(DEP_RSC_MICOUNI) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

