# Microsoft Developer Studio Generated NMAKE File, Based on CommUnit.dsp
!IF "$(CFG)" == ""
CFG=CommUnit - Win32 Release
!MESSAGE No configuration specified. Defaulting to CommUnit - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "CommUnit - Win32 Release" && "$(CFG)" !=\
 "CommUnit - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CommUnit.mak" CFG="CommUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CommUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CommUnit - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "CommUnit - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\CommUnit.exe"

!ELSE 

ALL : "$(OUTDIR)\CommUnit.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CameraControlRecord.obj"
	-@erase "$(INTDIR)\CommInput.obj"
	-@erase "$(INTDIR)\CommUnit.obj"
	-@erase "$(INTDIR)\CommUnit.pch"
	-@erase "$(INTDIR)\CommUnit.res"
	-@erase "$(INTDIR)\CommWindow.obj"
	-@erase "$(INTDIR)\CRS232.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\CommUnit.exe"
	-@erase "$(OUTDIR)\CommUnit.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CommUnit.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\CommUnit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CommUnit.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=cipc.lib oemgui.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\CommUnit.pdb" /map:"$(INTDIR)\CommUnit.map" /machine:I386\
 /out:"$(OUTDIR)\CommUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CameraControlRecord.obj" \
	"$(INTDIR)\CommInput.obj" \
	"$(INTDIR)\CommUnit.obj" \
	"$(INTDIR)\CommUnit.res" \
	"$(INTDIR)\CommWindow.obj" \
	"$(INTDIR)\CRS232.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\CommUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\CommUnit.exe" "$(OUTDIR)\CommUnit.bsc"

!ELSE 

ALL : "$(OUTDIR)\CommUnit.exe" "$(OUTDIR)\CommUnit.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CameraControlRecord.obj"
	-@erase "$(INTDIR)\CameraControlRecord.sbr"
	-@erase "$(INTDIR)\CommInput.obj"
	-@erase "$(INTDIR)\CommInput.sbr"
	-@erase "$(INTDIR)\CommUnit.obj"
	-@erase "$(INTDIR)\CommUnit.pch"
	-@erase "$(INTDIR)\CommUnit.res"
	-@erase "$(INTDIR)\CommUnit.sbr"
	-@erase "$(INTDIR)\CommWindow.obj"
	-@erase "$(INTDIR)\CommWindow.sbr"
	-@erase "$(INTDIR)\CRS232.obj"
	-@erase "$(INTDIR)\CRS232.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\CommUnit.bsc"
	-@erase "$(OUTDIR)\CommUnit.exe"
	-@erase "$(OUTDIR)\CommUnit.ilk"
	-@erase "$(OUTDIR)\CommUnit.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CommUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\CommUnit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CommUnit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CameraControlRecord.sbr" \
	"$(INTDIR)\CommInput.sbr" \
	"$(INTDIR)\CommUnit.sbr" \
	"$(INTDIR)\CommWindow.sbr" \
	"$(INTDIR)\CRS232.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\CommUnit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=cipcdebug.lib oemguid.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\CommUnit.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\CommUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CameraControlRecord.obj" \
	"$(INTDIR)\CommInput.obj" \
	"$(INTDIR)\CommUnit.obj" \
	"$(INTDIR)\CommUnit.res" \
	"$(INTDIR)\CommWindow.obj" \
	"$(INTDIR)\CRS232.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\CommUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "CommUnit - Win32 Release" || "$(CFG)" ==\
 "CommUnit - Win32 Debug"
SOURCE=.\CameraControlRecord.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

DEP_CPP_CAMER=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommUnit.h"\
	".\CRS232.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CameraControlRecord.obj" : $(SOURCE) $(DEP_CPP_CAMER) "$(INTDIR)"\
 "$(INTDIR)\CommUnit.pch"


!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

DEP_CPP_CAMER=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommUnit.h"\
	".\CRS232.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CameraControlRecord.obj"	"$(INTDIR)\CameraControlRecord.sbr" : \
$(SOURCE) $(DEP_CPP_CAMER) "$(INTDIR)" "$(INTDIR)\CommUnit.pch"


!ENDIF 

SOURCE=.\CommInput.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

DEP_CPP_COMMI=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommInput.h"\
	".\CommUnit.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CommInput.obj" : $(SOURCE) $(DEP_CPP_COMMI) "$(INTDIR)"\
 "$(INTDIR)\CommUnit.pch"


!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

DEP_CPP_COMMI=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommInput.h"\
	".\CommUnit.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CommInput.obj"	"$(INTDIR)\CommInput.sbr" : $(SOURCE)\
 $(DEP_CPP_COMMI) "$(INTDIR)" "$(INTDIR)\CommUnit.pch"


!ENDIF 

SOURCE=.\CommUnit.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

DEP_CPP_COMMU=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommInput.h"\
	".\CommUnit.h"\
	".\CommWindow.h"\
	".\CRS232.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CommUnit.obj" : $(SOURCE) $(DEP_CPP_COMMU) "$(INTDIR)"\
 "$(INTDIR)\CommUnit.pch"


!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

DEP_CPP_COMMU=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommInput.h"\
	".\CommUnit.h"\
	".\CommWindow.h"\
	".\CRS232.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CommUnit.obj"	"$(INTDIR)\CommUnit.sbr" : $(SOURCE) $(DEP_CPP_COMMU)\
 "$(INTDIR)" "$(INTDIR)\CommUnit.pch"


!ENDIF 

SOURCE=.\CommUnit.rc
DEP_RSC_COMMUN=\
	".\res\CommUnit.ico"\
	".\res\CommUnit.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\CommUnit.res" : $(SOURCE) $(DEP_RSC_COMMUN) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\CommUnitEng.rc
SOURCE=.\CommWindow.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

DEP_CPP_COMMW=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommUnit.h"\
	".\CommWindow.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\idipapi.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CommWindow.obj" : $(SOURCE) $(DEP_CPP_COMMW) "$(INTDIR)"\
 "$(INTDIR)\CommUnit.pch"


!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

DEP_CPP_COMMW=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommUnit.h"\
	".\CommWindow.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CommWindow.obj"	"$(INTDIR)\CommWindow.sbr" : $(SOURCE)\
 $(DEP_CPP_COMMW) "$(INTDIR)" "$(INTDIR)\CommUnit.pch"


!ENDIF 

SOURCE=.\CRS232.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

DEP_CPP_CRS23=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommUnit.h"\
	".\CRS232.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CRS232.obj" : $(SOURCE) $(DEP_CPP_CRS23) "$(INTDIR)"\
 "$(INTDIR)\CommUnit.pch"


!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

DEP_CPP_CRS23=\
	".\CameraCommandRecord.h"\
	".\CameraControlRecord.h"\
	".\CommUnit.h"\
	".\CRS232.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\util.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\CRS232.obj"	"$(INTDIR)\CRS232.sbr" : $(SOURCE) $(DEP_CPP_CRS23)\
 "$(INTDIR)" "$(INTDIR)\CommUnit.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "CommUnit - Win32 Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcextramemory.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\util.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_debugoptions.h"\
	"v:\project\include\wk_names.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CommUnit.pch" /Yc /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\CommUnit.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CommUnit - Win32 Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcextramemory.h"\
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
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CommUnit.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\CommUnit.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

