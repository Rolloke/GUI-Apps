# Microsoft Developer Studio Generated NMAKE File, Based on CoCoUnit.dsp
!IF "$(CFG)" == ""
CFG=CoCoUnit - Win32 Release
!MESSAGE No configuration specified. Defaulting to CoCoUnit - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "CoCoUnit - Win32 Release" && "$(CFG)" !=\
 "CoCoUnit - Win32 Debug" && "$(CFG)" != "CoCoUnit - Win32 SecEng"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CoCoUnit.mak" CFG="CoCoUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CoCoUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CoCoUnit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CoCoUnit - Win32 SecEng" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\CoCoUnit.exe" "$(OUTDIR)\CoCoUnit.bsc"

!ELSE 

ALL : "$(OUTDIR)\CoCoUnit.exe" "$(OUTDIR)\CoCoUnit.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Ccoco.obj"
	-@erase "$(INTDIR)\Ccoco.sbr"
	-@erase "$(INTDIR)\CipcInputCoCoUnit.obj"
	-@erase "$(INTDIR)\CipcInputCoCoUnit.sbr"
	-@erase "$(INTDIR)\CipcOutputCoCoUnit.obj"
	-@erase "$(INTDIR)\CipcOutputCoCoUnit.sbr"
	-@erase "$(INTDIR)\CoCoUnit.pch"
	-@erase "$(INTDIR)\CoCoUnit.res"
	-@erase "$(INTDIR)\CoCoUnitApp.obj"
	-@erase "$(INTDIR)\CoCoUnitApp.sbr"
	-@erase "$(INTDIR)\CoCoUnitDlg.obj"
	-@erase "$(INTDIR)\CoCoUnitDlg.sbr"
	-@erase "$(INTDIR)\cpydata.obj"
	-@erase "$(INTDIR)\cpydata.sbr"
	-@erase "$(INTDIR)\Dlgcolor.obj"
	-@erase "$(INTDIR)\Dlgcolor.sbr"
	-@erase "$(INTDIR)\Settings.obj"
	-@erase "$(INTDIR)\Settings.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\CoCoUnit.bsc"
	-@erase "$(OUTDIR)\CoCoUnit.exe"
	-@erase "$(OUTDIR)\CoCoUnit.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\CoCoUnit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CoCoUnit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Ccoco.sbr" \
	"$(INTDIR)\CipcInputCoCoUnit.sbr" \
	"$(INTDIR)\CipcOutputCoCoUnit.sbr" \
	"$(INTDIR)\CoCoUnitApp.sbr" \
	"$(INTDIR)\CoCoUnitDlg.sbr" \
	"$(INTDIR)\cpydata.sbr" \
	"$(INTDIR)\Dlgcolor.sbr" \
	"$(INTDIR)\Settings.sbr" \
	"$(INTDIR)\stdafx.sbr"

"$(OUTDIR)\CoCoUnit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=CIPC.lib OEMGUI.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\CoCoUnit.pdb" /map:"$(INTDIR)\CoCoUnit.map" /machine:I386\
 /out:"$(OUTDIR)\CoCoUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Ccoco.obj" \
	"$(INTDIR)\CipcInputCoCoUnit.obj" \
	"$(INTDIR)\CipcOutputCoCoUnit.obj" \
	"$(INTDIR)\CoCoUnit.res" \
	"$(INTDIR)\CoCoUnitApp.obj" \
	"$(INTDIR)\CoCoUnitDlg.obj" \
	"$(INTDIR)\cpydata.obj" \
	"$(INTDIR)\Dlgcolor.obj" \
	"$(INTDIR)\Settings.obj" \
	"$(INTDIR)\stdafx.obj" \
	"..\..\MegraV2\MegraV2dll\release\megrav2.lib"

"$(OUTDIR)\CoCoUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\CoCoUnit.exe" "$(OUTDIR)\CoCoUnit.bsc"

!ELSE 

ALL : "$(OUTDIR)\CoCoUnit.exe" "$(OUTDIR)\CoCoUnit.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Ccoco.obj"
	-@erase "$(INTDIR)\Ccoco.sbr"
	-@erase "$(INTDIR)\CipcInputCoCoUnit.obj"
	-@erase "$(INTDIR)\CipcInputCoCoUnit.sbr"
	-@erase "$(INTDIR)\CipcOutputCoCoUnit.obj"
	-@erase "$(INTDIR)\CipcOutputCoCoUnit.sbr"
	-@erase "$(INTDIR)\CoCoUnit.res"
	-@erase "$(INTDIR)\CoCoUnitApp.obj"
	-@erase "$(INTDIR)\CoCoUnitApp.sbr"
	-@erase "$(INTDIR)\CoCoUnitDlg.obj"
	-@erase "$(INTDIR)\CoCoUnitDlg.sbr"
	-@erase "$(INTDIR)\cpydata.obj"
	-@erase "$(INTDIR)\cpydata.sbr"
	-@erase "$(INTDIR)\Dlgcolor.obj"
	-@erase "$(INTDIR)\Dlgcolor.sbr"
	-@erase "$(INTDIR)\Settings.obj"
	-@erase "$(INTDIR)\Settings.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\CoCoUnit.bsc"
	-@erase "$(OUTDIR)\CoCoUnit.exe"
	-@erase "$(OUTDIR)\CoCoUnit.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\CoCoUnit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CoCoUnit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Ccoco.sbr" \
	"$(INTDIR)\CipcInputCoCoUnit.sbr" \
	"$(INTDIR)\CipcOutputCoCoUnit.sbr" \
	"$(INTDIR)\CoCoUnitApp.sbr" \
	"$(INTDIR)\CoCoUnitDlg.sbr" \
	"$(INTDIR)\cpydata.sbr" \
	"$(INTDIR)\Dlgcolor.sbr" \
	"$(INTDIR)\Settings.sbr" \
	"$(INTDIR)\stdafx.sbr"

"$(OUTDIR)\CoCoUnit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\CoCoUnit.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\CoCoUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Ccoco.obj" \
	"$(INTDIR)\CipcInputCoCoUnit.obj" \
	"$(INTDIR)\CipcOutputCoCoUnit.obj" \
	"$(INTDIR)\CoCoUnit.res" \
	"$(INTDIR)\CoCoUnitApp.obj" \
	"$(INTDIR)\CoCoUnitDlg.obj" \
	"$(INTDIR)\cpydata.obj" \
	"$(INTDIR)\Dlgcolor.obj" \
	"$(INTDIR)\Settings.obj" \
	"$(INTDIR)\stdafx.obj" \
	"..\..\MegraV2\MegraV2dll\Debug\MegraV2D.lib"

"$(OUTDIR)\CoCoUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

OUTDIR=.\SecEng
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\SecEng
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\CoCoUnit.exe"

!ELSE 

ALL : "$(OUTDIR)\CoCoUnit.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Ccoco.obj"
	-@erase "$(INTDIR)\CipcInputCoCoUnit.obj"
	-@erase "$(INTDIR)\CipcOutputCoCoUnit.obj"
	-@erase "$(INTDIR)\CoCoUnitApp.obj"
	-@erase "$(INTDIR)\CoCoUnitDlg.obj"
	-@erase "$(INTDIR)\CoCoUnitEng.res"
	-@erase "$(INTDIR)\cpydata.obj"
	-@erase "$(INTDIR)\Dlgcolor.obj"
	-@erase "$(INTDIR)\Settings.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\CoCoUnit.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\CoCoUnitEng.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CoCoUnit.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\CoCoUnit.pdb" /machine:I386 /out:"$(OUTDIR)\CoCoUnit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Ccoco.obj" \
	"$(INTDIR)\CipcInputCoCoUnit.obj" \
	"$(INTDIR)\CipcOutputCoCoUnit.obj" \
	"$(INTDIR)\CoCoUnitApp.obj" \
	"$(INTDIR)\CoCoUnitDlg.obj" \
	"$(INTDIR)\CoCoUnitEng.res" \
	"$(INTDIR)\cpydata.obj" \
	"$(INTDIR)\Dlgcolor.obj" \
	"$(INTDIR)\Settings.obj" \
	"$(INTDIR)\stdafx.obj" \
	"..\..\MegraV2\MegraV2dll\release\megrav2.lib"

"$(OUTDIR)\CoCoUnit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "CoCoUnit - Win32 Release" || "$(CFG)" ==\
 "CoCoUnit - Win32 Debug" || "$(CFG)" == "CoCoUnit - Win32 SecEng"
SOURCE=.\Ccoco.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_CCOCO=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\dlgcolor.h"\
	".\message.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Ccoco.obj"	"$(INTDIR)\Ccoco.sbr" : $(SOURCE) $(DEP_CPP_CCOCO)\
 "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_CCOCO=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\dlgcolor.h"\
	".\message.h"\
	".\stdafx.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Ccoco.obj"	"$(INTDIR)\Ccoco.sbr" : $(SOURCE) $(DEP_CPP_CCOCO)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_CCOCO=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\dlgcolor.h"\
	".\message.h"\
	".\stdafx.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Ccoco.obj" : $(SOURCE) $(DEP_CPP_CCOCO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\CipcInputCoCoUnit.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_CIPCI=\
	".\CCoCo.h"\
	".\CIPCInputCoCoUnit.h"\
	".\cpydata.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\updatedefines.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CipcInputCoCoUnit.obj"	"$(INTDIR)\CipcInputCoCoUnit.sbr" : $(SOURCE)\
 $(DEP_CPP_CIPCI) "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_CIPCI=\
	".\CCoCo.h"\
	".\CIPCInputCoCoUnit.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\updatedefines.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CipcInputCoCoUnit.obj"	"$(INTDIR)\CipcInputCoCoUnit.sbr" : $(SOURCE)\
 $(DEP_CPP_CIPCI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_CIPCI=\
	".\CCoCo.h"\
	".\CIPCInputCoCoUnit.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\updatedefines.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CipcInputCoCoUnit.obj" : $(SOURCE) $(DEP_CPP_CIPCI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\CipcOutputCoCoUnit.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_CIPCO=\
	".\CCoCo.h"\
	".\CipcOutputCoCoUnit.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	"v:\project\cipc\cipcstringdefs.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CipcOutputCoCoUnit.obj"	"$(INTDIR)\CipcOutputCoCoUnit.sbr" : \
$(SOURCE) $(DEP_CPP_CIPCO) "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_CIPCO=\
	".\CCoCo.h"\
	".\CipcOutputCoCoUnit.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	"v:\project\cipc\cipcstringdefs.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CipcOutputCoCoUnit.obj"	"$(INTDIR)\CipcOutputCoCoUnit.sbr" : \
$(SOURCE) $(DEP_CPP_CIPCO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_CIPCO=\
	".\CCoCo.h"\
	".\CipcOutputCoCoUnit.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	"v:\project\cipc\cipcstringdefs.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CipcOutputCoCoUnit.obj" : $(SOURCE) $(DEP_CPP_CIPCO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\CoCoUnitApp.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_COCOU=\
	".\CCoCo.h"\
	".\cocounitapp.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CoCoUnitApp.obj"	"$(INTDIR)\CoCoUnitApp.sbr" : $(SOURCE)\
 $(DEP_CPP_COCOU) "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_COCOU=\
	".\CCoCo.h"\
	".\cocounitapp.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CoCoUnitApp.obj"	"$(INTDIR)\CoCoUnitApp.sbr" : $(SOURCE)\
 $(DEP_CPP_COCOU) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_COCOU=\
	".\CCoCo.h"\
	".\cocounitapp.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"util.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_dongle.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CoCoUnitApp.obj" : $(SOURCE) $(DEP_CPP_COCOU) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\CoCoUnitDlg.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_COCOUN=\
	".\CCoCo.h"\
	".\CIPCInputCoCoUnit.h"\
	".\CipcOutputCoCoUnit.h"\
	".\cocounitapp.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\message.h"\
	".\Settings.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\permission.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\user.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CoCoUnitDlg.obj"	"$(INTDIR)\CoCoUnitDlg.sbr" : $(SOURCE)\
 $(DEP_CPP_COCOUN) "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_COCOUN=\
	".\CCoCo.h"\
	".\CIPCInputCoCoUnit.h"\
	".\CipcOutputCoCoUnit.h"\
	".\cocounitapp.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\message.h"\
	".\Settings.h"\
	".\stdafx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\permission.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\user.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\idipapi.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CoCoUnitDlg.obj"	"$(INTDIR)\CoCoUnitDlg.sbr" : $(SOURCE)\
 $(DEP_CPP_COCOUN) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_COCOUN=\
	".\CCoCo.h"\
	".\CIPCInputCoCoUnit.h"\
	".\CipcOutputCoCoUnit.h"\
	".\cocounitapp.h"\
	".\CoCoUnitDlg.h"\
	".\cpydata.h"\
	".\message.h"\
	".\Settings.h"\
	".\stdafx.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\permission.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\user.h"\
	"v:\project\cipc\wk_profile.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	"v:\project\lib\oemgui\oemguiapi.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"cipcextramemory.h"\
	{$(INCLUDE)}"cipcoutput.h"\
	{$(INCLUDE)}"picturedef.h"\
	{$(INCLUDE)}"picturerecord.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"videojob.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CoCoUnitDlg.obj" : $(SOURCE) $(DEP_CPP_COCOUN) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\cpydata.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_CPYDA=\
	".\CCoCo.h"\
	".\cpydata.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cpydata.obj"	"$(INTDIR)\cpydata.sbr" : $(SOURCE) $(DEP_CPP_CPYDA)\
 "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_CPYDA=\
	".\cpydata.h"\
	".\stdafx.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cpydata.obj"	"$(INTDIR)\cpydata.sbr" : $(SOURCE) $(DEP_CPP_CPYDA)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_CPYDA=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cpydata.obj" : $(SOURCE) $(DEP_CPP_CPYDA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Dlgcolor.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_DLGCO=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\dlgcolor.h"\
	".\message.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Dlgcolor.obj"	"$(INTDIR)\Dlgcolor.sbr" : $(SOURCE) $(DEP_CPP_DLGCO)\
 "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_DLGCO=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\dlgcolor.h"\
	".\message.h"\
	".\stdafx.h"\
	"v:\project\include\wk_msg.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Dlgcolor.obj"	"$(INTDIR)\Dlgcolor.sbr" : $(SOURCE) $(DEP_CPP_DLGCO)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_DLGCO=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\dlgcolor.h"\
	".\message.h"\
	".\stdafx.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Dlgcolor.obj" : $(SOURCE) $(DEP_CPP_DLGCO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Settings.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_SETTI=\
	".\CCoCo.h"\
	".\cocounitapp.h"\
	".\cpydata.h"\
	".\Settings.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Settings.obj"	"$(INTDIR)\Settings.sbr" : $(SOURCE) $(DEP_CPP_SETTI)\
 "$(INTDIR)" "$(INTDIR)\CoCoUnit.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_SETTI=\
	".\CCoCo.h"\
	".\cocounitapp.h"\
	".\cpydata.h"\
	".\Settings.h"\
	".\stdafx.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Settings.obj"	"$(INTDIR)\Settings.sbr" : $(SOURCE) $(DEP_CPP_SETTI)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_SETTI=\
	".\CCoCo.h"\
	".\cocounitapp.h"\
	".\cpydata.h"\
	".\Settings.h"\
	".\stdafx.h"\
	"v:\project\include\cmegra.h"\
	"v:\project\include\cmymutex.h"\
	"v:\project\include\win16_32.h"\
	"v:\project\include\wk_msg.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Settings.obj" : $(SOURCE) $(DEP_CPP_SETTI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_CPP_STDAF=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\CoCoUnit.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CoCoUnit.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_CPP_STDAF=\
	".\CCoCo.h"\
	".\cpydata.h"\
	".\stdafx.h"\
	{$(INCLUDE)}"cipc.h"\
	{$(INCLUDE)}"secid.h"\
	{$(INCLUDE)}"timedmessage.h"\
	{$(INCLUDE)}"unhandledexception.h"\
	{$(INCLUDE)}"wk.h"\
	{$(INCLUDE)}"wk_applicationid.h"\
	{$(INCLUDE)}"wk_file.h"\
	{$(INCLUDE)}"wk_names.h"\
	{$(INCLUDE)}"wk_runtimeerror.h"\
	{$(INCLUDE)}"wk_template.h"\
	{$(INCLUDE)}"wk_trace.h"\
	
CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\CoCoUnit.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\CoCoUnit.rc

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

DEP_RSC_COCOUNI=\
	".\res\CoCoUnit.ico"\
	".\res\CoCoUnit.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\CoCoUnit.res" : $(SOURCE) $(DEP_RSC_COCOUNI) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

DEP_RSC_COCOUNI=\
	".\res\CoCoUnit.ico"\
	".\res\CoCoUnit.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\CoCoUnit.res" : $(SOURCE) $(DEP_RSC_COCOUNI) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

!ENDIF 

SOURCE=.\CoCoUnitEng.rc

!IF  "$(CFG)" == "CoCoUnit - Win32 Release"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 Debug"

!ELSEIF  "$(CFG)" == "CoCoUnit - Win32 SecEng"

DEP_RSC_COCOUNIT=\
	".\res\CoCoUnit.ico"\
	".\res\CoCoUnit.rc2"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\CoCoUnitEng.res" : $(SOURCE) $(DEP_RSC_COCOUNIT) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\CoCoUnitEng.res" /d "NDEBUG" /d "_AFXDLL"\
 $(SOURCE)


!ENDIF 


!ENDIF 

