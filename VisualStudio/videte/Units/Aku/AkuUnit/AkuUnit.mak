# Microsoft Developer Studio Generated NMAKE File, Based on AkuUnit.dsp
!IF "$(CFG)" == ""
CFG=AkuUnit - Win32 Release
!MESSAGE No configuration specified. Defaulting to AkuUnit - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "AkuUnit - Win32 Release" && "$(CFG)" !=\
 "AkuUnit - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AkuUnit.mak" CFG="AkuUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AkuUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AkuUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AkuUnit.exe"

!ELSE 

ALL : "$(OUTDIR)\AkuUnit.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AkuUnit.obj"
	-@erase "$(INTDIR)\AkuUnit.pch"
	-@erase "$(INTDIR)\AkuUnit.res"
	-@erase "$(INTDIR)\AkuUnitDlg.obj"
	-@erase "$(INTDIR)\CAku.obj"
	-@erase "$(INTDIR)\CAlarm.obj"
	-@erase "$(INTDIR)\CIo.obj"
	-@erase "$(INTDIR)\CipcInputAkuUnit.obj"
	-@erase "$(INTDIR)\CipcOutputAkuUnit.obj"
	-@erase "$(INTDIR)\crelay.obj"
	-@erase "$(INTDIR)\DAAku.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\AkuUnit.exe"
	-@erase "$(OUTDIR)\AkuUnit.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AkuUnit.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\AkuUnit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AkuUnit.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=CIPC.lib OEMGUI.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\AkuUnit.pdb" /map:"$(INTDIR)\AkuUnit.map" /machine:I386\
 /out:"$(OUTDIR)\AkuUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AkuUnit.obj" \
	"$(INTDIR)\AkuUnit.res" \
	"$(INTDIR)\AkuUnitDlg.obj" \
	"$(INTDIR)\CAku.obj" \
	"$(INTDIR)\CAlarm.obj" \
	"$(INTDIR)\CIo.obj" \
	"$(INTDIR)\CipcInputAkuUnit.obj" \
	"$(INTDIR)\CipcOutputAkuUnit.obj" \
	"$(INTDIR)\crelay.obj" \
	"$(INTDIR)\DAAku.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AkuUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AkuUnit.exe" "$(OUTDIR)\AkuUnit.bsc"

!ELSE 

ALL : "$(OUTDIR)\AkuUnit.exe" "$(OUTDIR)\AkuUnit.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AkuUnit.obj"
	-@erase "$(INTDIR)\AkuUnit.pch"
	-@erase "$(INTDIR)\AkuUnit.res"
	-@erase "$(INTDIR)\AkuUnit.sbr"
	-@erase "$(INTDIR)\AkuUnitDlg.obj"
	-@erase "$(INTDIR)\AkuUnitDlg.sbr"
	-@erase "$(INTDIR)\CAku.obj"
	-@erase "$(INTDIR)\CAku.sbr"
	-@erase "$(INTDIR)\CAlarm.obj"
	-@erase "$(INTDIR)\CAlarm.sbr"
	-@erase "$(INTDIR)\CIo.obj"
	-@erase "$(INTDIR)\CIo.sbr"
	-@erase "$(INTDIR)\CipcInputAkuUnit.obj"
	-@erase "$(INTDIR)\CipcInputAkuUnit.sbr"
	-@erase "$(INTDIR)\CipcOutputAkuUnit.obj"
	-@erase "$(INTDIR)\CipcOutputAkuUnit.sbr"
	-@erase "$(INTDIR)\crelay.obj"
	-@erase "$(INTDIR)\crelay.sbr"
	-@erase "$(INTDIR)\DAAku.obj"
	-@erase "$(INTDIR)\DAAku.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\AkuUnit.bsc"
	-@erase "$(OUTDIR)\AkuUnit.exe"
	-@erase "$(OUTDIR)\AkuUnit.ilk"
	-@erase "$(OUTDIR)\AkuUnit.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AkuUnit.pch"\
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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\AkuUnit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AkuUnit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AkuUnit.sbr" \
	"$(INTDIR)\AkuUnitDlg.sbr" \
	"$(INTDIR)\CAku.sbr" \
	"$(INTDIR)\CAlarm.sbr" \
	"$(INTDIR)\CIo.sbr" \
	"$(INTDIR)\CipcInputAkuUnit.sbr" \
	"$(INTDIR)\CipcOutputAkuUnit.sbr" \
	"$(INTDIR)\crelay.sbr" \
	"$(INTDIR)\DAAku.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\AkuUnit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=CIPCDEBUG.lib OEMGUID.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\AkuUnit.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\AkuUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AkuUnit.obj" \
	"$(INTDIR)\AkuUnit.res" \
	"$(INTDIR)\AkuUnitDlg.obj" \
	"$(INTDIR)\CAku.obj" \
	"$(INTDIR)\CAlarm.obj" \
	"$(INTDIR)\CIo.obj" \
	"$(INTDIR)\CipcInputAkuUnit.obj" \
	"$(INTDIR)\CipcOutputAkuUnit.obj" \
	"$(INTDIR)\crelay.obj" \
	"$(INTDIR)\DAAku.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AkuUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "AkuUnit - Win32 Release" || "$(CFG)" ==\
 "AkuUnit - Win32 Debug"
SOURCE=.\AkuUnit.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_AKUUN=\
	".\AkuUnit.h"\
	".\AkuUnitDlg.h"\
	".\DAAku.h"\
	".\Ini2Reg.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\util.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\AkuUnit.obj" : $(SOURCE) $(DEP_CPP_AKUUN) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_AKUUN=\
	".\AkuUnit.h"\
	".\AkuUnitDlg.h"\
	".\DAAku.h"\
	".\Ini2Reg.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\util.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_dongle.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_runtimeerror.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_names.h"\
	

"$(INTDIR)\AkuUnit.obj"	"$(INTDIR)\AkuUnit.sbr" : $(SOURCE) $(DEP_CPP_AKUUN)\
 "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\AkuUnitDlg.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_AKUUNI=\
	".\AkuUnit.h"\
	".\AkuUnitDlg.h"\
	".\CAku.h"\
	".\Cio.h"\
	".\CIPCInputAkuUnit.h"\
	".\CipcOutputAkuUnit.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	"v:\project\include\wk_names.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\AkuUnitDlg.obj" : $(SOURCE) $(DEP_CPP_AKUUNI) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_AKUUNI=\
	".\AkuUnit.h"\
	".\AkuUnitDlg.h"\
	".\CAku.h"\
	".\Cio.h"\
	".\CIPCInputAkuUnit.h"\
	".\CipcOutputAkuUnit.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	"v:\project\include\wk_names.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	

"$(INTDIR)\AkuUnitDlg.obj"	"$(INTDIR)\AkuUnitDlg.sbr" : $(SOURCE)\
 $(DEP_CPP_AKUUNI) "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\CAku.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_CAKU_=\
	".\AkuReg.h"\
	".\CAku.h"\
	".\CAlarm.h"\
	".\Cio.h"\
	".\CRelay.h"\
	".\Ini2Reg.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CAku.obj" : $(SOURCE) $(DEP_CPP_CAKU_) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_CAKU_=\
	".\AkuReg.h"\
	".\CAku.h"\
	".\CAlarm.h"\
	".\Cio.h"\
	".\CRelay.h"\
	".\Ini2Reg.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CAku.obj"	"$(INTDIR)\CAku.sbr" : $(SOURCE) $(DEP_CPP_CAKU_)\
 "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\CAlarm.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_CALAR=\
	".\AkuReg.h"\
	".\CAku.h"\
	".\CAlarm.h"\
	".\Cio.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CAlarm.obj" : $(SOURCE) $(DEP_CPP_CALAR) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_CALAR=\
	".\AkuReg.h"\
	".\CAku.h"\
	".\CAlarm.h"\
	".\Cio.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CAlarm.obj"	"$(INTDIR)\CAlarm.sbr" : $(SOURCE) $(DEP_CPP_CALAR)\
 "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\CIo.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_CIO_C=\
	".\AkuReg.h"\
	".\Cio.h"\
	".\DAAku.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\CIo.obj" : $(SOURCE) $(DEP_CPP_CIO_C) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_CIO_C=\
	".\AkuReg.h"\
	".\Cio.h"\
	".\DAAku.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\CIo.obj"	"$(INTDIR)\CIo.sbr" : $(SOURCE) $(DEP_CPP_CIO_C)\
 "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\CipcInputAkuUnit.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_CIPCI=\
	".\CAku.h"\
	".\Cio.h"\
	".\CIPCInputAkuUnit.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CipcInputAkuUnit.obj" : $(SOURCE) $(DEP_CPP_CIPCI) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_CIPCI=\
	".\CAku.h"\
	".\Cio.h"\
	".\CIPCInputAkuUnit.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CipcInputAkuUnit.obj"	"$(INTDIR)\CipcInputAkuUnit.sbr" : $(SOURCE)\
 $(DEP_CPP_CIPCI) "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\CipcOutputAkuUnit.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_CIPCO=\
	".\AkuUnitDlg.h"\
	".\CAku.h"\
	".\Cio.h"\
	".\CipcOutputAkuUnit.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcextramemory.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcstringdefs.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CipcOutputAkuUnit.obj" : $(SOURCE) $(DEP_CPP_CIPCO) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_CIPCO=\
	".\AkuUnitDlg.h"\
	".\CAku.h"\
	".\Cio.h"\
	".\CipcOutputAkuUnit.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcextramemory.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcstringdefs.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\CipcOutputAkuUnit.obj"	"$(INTDIR)\CipcOutputAkuUnit.sbr" : $(SOURCE)\
 $(DEP_CPP_CIPCO) "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\crelay.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_CRELA=\
	".\AkuReg.h"\
	".\CAku.h"\
	".\Cio.h"\
	".\CRelay.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\crelay.obj" : $(SOURCE) $(DEP_CPP_CRELA) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_CRELA=\
	".\AkuReg.h"\
	".\CAku.h"\
	".\Cio.h"\
	".\CRelay.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	"v:\project\include\wk_msg.h"\
	

"$(INTDIR)\crelay.obj"	"$(INTDIR)\crelay.sbr" : $(SOURCE) $(DEP_CPP_CRELA)\
 "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\DAAku.cpp

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

DEP_CPP_DAAKU=\
	"..\akusys\akuioctl.h"\
	".\AkuDef.h"\
	".\DAAku.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\DAAku.obj" : $(SOURCE) $(DEP_CPP_DAAKU) "$(INTDIR)"\
 "$(INTDIR)\AkuUnit.pch"


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

DEP_CPP_DAAKU=\
	"..\..\..\..\winntddk\inc\devioctl.h"\
	"..\akusys\akuioctl.h"\
	".\AkuDef.h"\
	".\DAAku.h"\
	"v:\project\cipc\unhandledexception.h"\
	"v:\project\cipc\wk_applicationid.h"\
	"v:\project\cipc\wk_trace.h"\
	

"$(INTDIR)\DAAku.obj"	"$(INTDIR)\DAAku.sbr" : $(SOURCE) $(DEP_CPP_DAAKU)\
 "$(INTDIR)" "$(INTDIR)\AkuUnit.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "AkuUnit - Win32 Release"

CPP_SWITCHES=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AkuUnit.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AkuUnit.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AkuUnit - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG"\
 /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\AkuUnit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\AkuUnit.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\AkuUnit.rc
DEP_RSC_AKUUNIT=\
	".\res\AkuUnit.ico"\
	".\res\AkuUnit.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\AkuUnit.res" : $(SOURCE) $(DEP_RSC_AKUUNIT) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\AkuUnitEng.rc

!ENDIF 

