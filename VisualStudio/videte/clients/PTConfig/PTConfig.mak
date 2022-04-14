# Microsoft Developer Studio Generated NMAKE File, Based on PTConfig.dsp
!IF "$(CFG)" == ""
CFG=PTConfig - Win32 Debug
!MESSAGE No configuration specified. Defaulting to PTConfig - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PTConfig - Win32 Release" && "$(CFG)" !=\
 "PTConfig - Win32 Debug" && "$(CFG)" != "PTConfig - Win32 GarnyRelease"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PTConfig.mak" CFG="PTConfig - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PTConfig - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PTConfig - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "PTConfig - Win32 GarnyRelease" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "PTConfig - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PTConfig.exe"

!ELSE 

ALL : "$(OUTDIR)\PTConfig.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ConnectDialog.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\PostFetch.obj"
	-@erase "$(INTDIR)\PTConfig.obj"
	-@erase "$(INTDIR)\PTConfig.pch"
	-@erase "$(INTDIR)\PTConfig.res"
	-@erase "$(INTDIR)\PTConfigDoc.obj"
	-@erase "$(INTDIR)\PTConfigView.obj"
	-@erase "$(INTDIR)\PTInputClient.obj"
	-@erase "$(INTDIR)\PTOutputClient.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\PTConfig.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\PTConfig.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
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
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\PTConfig.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PTConfig.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\PTConfig.pdb" /machine:I386 /out:"$(OUTDIR)\PTConfig.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\ConnectDialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\PostFetch.obj" \
	"$(INTDIR)\PTConfig.obj" \
	"$(INTDIR)\PTConfig.res" \
	"$(INTDIR)\PTConfigDoc.obj" \
	"$(INTDIR)\PTConfigView.obj" \
	"$(INTDIR)\PTInputClient.obj" \
	"$(INTDIR)\PTOutputClient.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"V:\Project\Cipc\Release\Cipc.lib"

"$(OUTDIR)\PTConfig.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PTConfig.exe" "$(OUTDIR)\PTConfig.bsc"

!ELSE 

ALL : "$(OUTDIR)\PTConfig.exe" "$(OUTDIR)\PTConfig.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\ConnectDialog.obj"
	-@erase "$(INTDIR)\ConnectDialog.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\PostFetch.obj"
	-@erase "$(INTDIR)\PostFetch.sbr"
	-@erase "$(INTDIR)\PTConfig.obj"
	-@erase "$(INTDIR)\PTConfig.pch"
	-@erase "$(INTDIR)\PTConfig.res"
	-@erase "$(INTDIR)\PTConfig.sbr"
	-@erase "$(INTDIR)\PTConfigDoc.obj"
	-@erase "$(INTDIR)\PTConfigDoc.sbr"
	-@erase "$(INTDIR)\PTConfigView.obj"
	-@erase "$(INTDIR)\PTConfigView.sbr"
	-@erase "$(INTDIR)\PTInputClient.obj"
	-@erase "$(INTDIR)\PTInputClient.sbr"
	-@erase "$(INTDIR)\PTOutputClient.obj"
	-@erase "$(INTDIR)\PTOutputClient.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\PTConfig.bsc"
	-@erase "$(OUTDIR)\PTConfig.exe"
	-@erase "$(OUTDIR)\PTConfig.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\PTConfig.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\PTConfig.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PTConfig.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\ConnectDialog.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\PostFetch.sbr" \
	"$(INTDIR)\PTConfig.sbr" \
	"$(INTDIR)\PTConfigDoc.sbr" \
	"$(INTDIR)\PTConfigView.sbr" \
	"$(INTDIR)\PTInputClient.sbr" \
	"$(INTDIR)\PTOutputClient.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\PTConfig.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\PTConfig.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\PTConfig.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\ConnectDialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\PostFetch.obj" \
	"$(INTDIR)\PTConfig.obj" \
	"$(INTDIR)\PTConfig.res" \
	"$(INTDIR)\PTConfigDoc.obj" \
	"$(INTDIR)\PTConfigView.obj" \
	"$(INTDIR)\PTInputClient.obj" \
	"$(INTDIR)\PTOutputClient.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"V:\Project\Cipc\Debug\CipcDebug.lib"

"$(OUTDIR)\PTConfig.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

OUTDIR=.\GarnyRelease
INTDIR=.\GarnyRelease
# Begin Custom Macros
OutDir=.\GarnyRelease
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PTConfig.exe"

!ELSE 

ALL : "$(OUTDIR)\PTConfig.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ConnectDialog.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\PostFetch.obj"
	-@erase "$(INTDIR)\PTConfig.obj"
	-@erase "$(INTDIR)\PTConfig.pch"
	-@erase "$(INTDIR)\PTConfig.res"
	-@erase "$(INTDIR)\PTConfigDoc.obj"
	-@erase "$(INTDIR)\PTConfigView.obj"
	-@erase "$(INTDIR)\PTInputClient.obj"
	-@erase "$(INTDIR)\PTOutputClient.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\PTConfig.exe"
	-@erase "$(OUTDIR)\PTConfig.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_GARNY" /Fp"$(INTDIR)\PTConfig.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\GarnyRelease/
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
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\PTConfig.res" /d "NDEBUG" /d "_AFXDLL" /d\
 "_GARNY" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PTConfig.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\PTConfig.pdb" /map:"$(INTDIR)\PTConfig.map" /machine:I386\
 /out:"$(OUTDIR)\PTConfig.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\ConnectDialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\PostFetch.obj" \
	"$(INTDIR)\PTConfig.obj" \
	"$(INTDIR)\PTConfig.res" \
	"$(INTDIR)\PTConfigDoc.obj" \
	"$(INTDIR)\PTConfigView.obj" \
	"$(INTDIR)\PTInputClient.obj" \
	"$(INTDIR)\PTOutputClient.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"V:\Project\Cipc\Release\Cipc.lib"

"$(OUTDIR)\PTConfig.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "PTConfig - Win32 Release" || "$(CFG)" ==\
 "PTConfig - Win32 Debug" || "$(CFG)" == "PTConfig - Win32 GarnyRelease"
SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\ChildFrm.h"\
	".\PTConfig.h"\
	

!IF  "$(CFG)" == "PTConfig - Win32 Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"


"$(INTDIR)\ChildFrm.obj"	"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) $(DEP_CPP_CHILD)\
 "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\ConnectDialog.cpp
DEP_CPP_CONNE=\
	".\ConnectDialog.h"\
	".\PTConfig.h"\
	

!IF  "$(CFG)" == "PTConfig - Win32 Release"


"$(INTDIR)\ConnectDialog.obj" : $(SOURCE) $(DEP_CPP_CONNE) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"


"$(INTDIR)\ConnectDialog.obj"	"$(INTDIR)\ConnectDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_CONNE) "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"


"$(INTDIR)\ConnectDialog.obj" : $(SOURCE) $(DEP_CPP_CONNE) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "PTConfig - Win32 Release"

DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\PostFetch.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\PostFetch.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\PostFetch.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\PostFetch.cpp

!IF  "$(CFG)" == "PTConfig - Win32 Release"

DEP_CPP_POSTF=\
	".\PostFetch.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PostFetch.obj" : $(SOURCE) $(DEP_CPP_POSTF) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

DEP_CPP_POSTF=\
	".\PostFetch.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PostFetch.obj"	"$(INTDIR)\PostFetch.sbr" : $(SOURCE)\
 $(DEP_CPP_POSTF) "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

DEP_CPP_POSTF=\
	".\PostFetch.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PostFetch.obj" : $(SOURCE) $(DEP_CPP_POSTF) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\PTConfig.cpp

!IF  "$(CFG)" == "PTConfig - Win32 Release"

DEP_CPP_PTCON=\
	".\ChildFrm.h"\
	".\MainFrm.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTConfigView.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfig.obj" : $(SOURCE) $(DEP_CPP_PTCON) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

DEP_CPP_PTCON=\
	".\ChildFrm.h"\
	".\MainFrm.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTConfigView.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfig.obj"	"$(INTDIR)\PTConfig.sbr" : $(SOURCE) $(DEP_CPP_PTCON)\
 "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

DEP_CPP_PTCON=\
	".\ChildFrm.h"\
	".\MainFrm.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTConfigView.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfig.obj" : $(SOURCE) $(DEP_CPP_PTCON) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\PTConfig.rc
DEP_RSC_PTCONF=\
	".\AviFiles\filecopy.avi"\
	".\AviFiles\search.avi"\
	".\res\actions.ico"\
	".\res\alarms.ico"\
	".\res\basics.ico"\
	".\res\cameras.ico"\
	".\res\PTConfig.ico"\
	".\res\PTConfig.rc2"\
	".\res\PTConfigDoc.ico"\
	".\res\Toolbar.bmp"\
	".\res\toolbar1.bmp"\
	{$(INCLUDE)}"WK_Version.h"\
	

"$(INTDIR)\PTConfig.res" : $(SOURCE) $(DEP_RSC_PTCONF) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\PTConfigDoc.cpp

!IF  "$(CFG)" == "PTConfig - Win32 Release"

DEP_CPP_PTCONFI=\
	".\ConnectDialog.h"\
	".\PostFetch.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTInputClient.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfigDoc.obj" : $(SOURCE) $(DEP_CPP_PTCONFI) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

DEP_CPP_PTCONFI=\
	".\ConnectDialog.h"\
	".\PostFetch.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTInputClient.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfigDoc.obj"	"$(INTDIR)\PTConfigDoc.sbr" : $(SOURCE)\
 $(DEP_CPP_PTCONFI) "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

DEP_CPP_PTCONFI=\
	".\ConnectDialog.h"\
	".\PostFetch.h"\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTInputClient.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcfetchresult.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\cipcservercontrol.h"\
	"v:\project\cipc\cipcservercontrolclientside.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfigDoc.obj" : $(SOURCE) $(DEP_CPP_PTCONFI) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\PTConfigView.cpp

!IF  "$(CFG)" == "PTConfig - Win32 Release"

DEP_CPP_PTCONFIG=\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTConfigView.h"\
	".\PTInputClient.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfigView.obj" : $(SOURCE) $(DEP_CPP_PTCONFIG) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

DEP_CPP_PTCONFIG=\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTConfigView.h"\
	".\PTInputClient.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfigView.obj"	"$(INTDIR)\PTConfigView.sbr" : $(SOURCE)\
 $(DEP_CPP_PTCONFIG) "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

DEP_CPP_PTCONFIG=\
	".\PTConfig.h"\
	".\PTConfigDoc.h"\
	".\PTConfigView.h"\
	".\PTInputClient.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTConfigView.obj" : $(SOURCE) $(DEP_CPP_PTCONFIG) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\PTInputClient.cpp

!IF  "$(CFG)" == "PTConfig - Win32 Release"

DEP_CPP_PTINP=\
	".\PTConfigDoc.h"\
	".\PTInputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTInputClient.obj" : $(SOURCE) $(DEP_CPP_PTINP) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

DEP_CPP_PTINP=\
	".\PTConfigDoc.h"\
	".\PTInputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTInputClient.obj"	"$(INTDIR)\PTInputClient.sbr" : $(SOURCE)\
 $(DEP_CPP_PTINP) "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

DEP_CPP_PTINP=\
	".\PTConfigDoc.h"\
	".\PTInputClient.h"\
	"v:\project\cipc\cameracontrol.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcinput.h"\
	"v:\project\cipc\cipcinputclient.h"\
	"v:\project\cipc\cipcinputrecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\updatedefines.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTInputClient.obj" : $(SOURCE) $(DEP_CPP_PTINP) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\PTOutputClient.cpp

!IF  "$(CFG)" == "PTConfig - Win32 Release"

DEP_CPP_PTOUT=\
	".\PTConfigDoc.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTOutputClient.obj" : $(SOURCE) $(DEP_CPP_PTOUT) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

DEP_CPP_PTOUT=\
	".\PTConfigDoc.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTOutputClient.obj"	"$(INTDIR)\PTOutputClient.sbr" : $(SOURCE)\
 $(DEP_CPP_PTOUT) "$(INTDIR)" "$(INTDIR)\PTConfig.pch"


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

DEP_CPP_PTOUT=\
	".\PTConfigDoc.h"\
	".\PTOutputClient.h"\
	"v:\project\cipc\cipc.h"\
	"v:\project\cipc\cipcoutput.h"\
	"v:\project\cipc\cipcoutputclient.h"\
	"v:\project\cipc\cipcoutputrecord.h"\
	"v:\project\cipc\picturedef.h"\
	"v:\project\cipc\picturerecord.h"\
	"v:\project\cipc\secid.h"\
	"v:\project\cipc\timedmessage.h"\
	"v:\project\cipc\wk_template.h"\
	"v:\project\cipc\wk_trace.h"\
	"v:\project\include\wk.h"\
	

"$(INTDIR)\PTOutputClient.obj" : $(SOURCE) $(DEP_CPP_PTOUT) "$(INTDIR)"\
 "$(INTDIR)\PTConfig.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "PTConfig - Win32 Release"

CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\PTConfig.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\PTConfig.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "PTConfig - Win32 Debug"

CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\PTConfig.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\PTConfig.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "PTConfig - Win32 GarnyRelease"

CPP_SWITCHES=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_GARNY" /Fp"$(INTDIR)\PTConfig.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\PTConfig.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

