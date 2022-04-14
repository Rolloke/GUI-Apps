# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Tester - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Tester - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Tester - Win32 Release" && "$(CFG)" != "Tester - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tester.mak" CFG="Tester - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Tester - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Tester - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Tester - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Tester - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Tester.exe" "$(OUTDIR)\Tester.hlp"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\CIPCInputTester.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Tester.hlp"
	-@erase "$(INTDIR)\Tester.obj"
	-@erase "$(INTDIR)\Tester.pch"
	-@erase "$(INTDIR)\Tester.res"
	-@erase "$(INTDIR)\TesterDoc.obj"
	-@erase "$(INTDIR)\TesterView.obj"
	-@erase "$(INTDIR)\TickCount.obj"
	-@erase "$(OUTDIR)\Tester.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /Od /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /Od /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tester.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x407 /fo"$(INTDIR)/Tester.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tester.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Tester.pdb" /machine:I386 /out:"$(OUTDIR)/Tester.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\CIPCInputTester.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Tester.obj" \
	"$(INTDIR)\Tester.res" \
	"$(INTDIR)\TesterDoc.obj" \
	"$(INTDIR)\TesterView.obj" \
	"$(INTDIR)\TickCount.obj" \
	"..\..\Cipc\Release\Cipc.lib"

"$(OUTDIR)\Tester.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Tester.exe" "$(OUTDIR)\Tester.bsc" "$(OUTDIR)\Tester.hlp"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\CIPCInputTester.obj"
	-@erase "$(INTDIR)\CIPCInputTester.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Tester.hlp"
	-@erase "$(INTDIR)\Tester.obj"
	-@erase "$(INTDIR)\Tester.pch"
	-@erase "$(INTDIR)\Tester.res"
	-@erase "$(INTDIR)\Tester.sbr"
	-@erase "$(INTDIR)\TesterDoc.obj"
	-@erase "$(INTDIR)\TesterDoc.sbr"
	-@erase "$(INTDIR)\TesterView.obj"
	-@erase "$(INTDIR)\TesterView.sbr"
	-@erase "$(INTDIR)\TickCount.obj"
	-@erase "$(INTDIR)\TickCount.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Tester.bsc"
	-@erase "$(OUTDIR)\Tester.exe"
	-@erase "$(OUTDIR)\Tester.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Tester.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x407 /fo"$(INTDIR)/Tester.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tester.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\CIPCInputTester.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\Tester.sbr" \
	"$(INTDIR)\TesterDoc.sbr" \
	"$(INTDIR)\TesterView.sbr" \
	"$(INTDIR)\TickCount.sbr"

"$(OUTDIR)\Tester.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Tester.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Tester.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\CIPCInputTester.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Tester.obj" \
	"$(INTDIR)\Tester.res" \
	"$(INTDIR)\TesterDoc.obj" \
	"$(INTDIR)\TesterView.obj" \
	"$(INTDIR)\TickCount.obj" \
	"..\..\Cipc\Debug\CipcDebug.lib"

"$(OUTDIR)\Tester.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Tester - Win32 Release"
# Name "Tester - Win32 Debug"

!IF  "$(CFG)" == "Tester - Win32 Release"

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Tester - Win32 Release"

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Tester.cpp

!IF  "$(CFG)" == "Tester - Win32 Release"

DEP_CPP_TESTE=\
	"..\..\Cipc\CipcFetchResult.h"\
	".\ChildFrm.h"\
	".\CIPCInputTester.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	".\Tester.h"\
	".\TesterDoc.h"\
	".\TesterView.h"\
	".\TickCount.h"\
	{$(INCLUDE)}"\CameraControl.h"\
	{$(INCLUDE)}"\Cipc.h"\
	{$(INCLUDE)}"\CipcInput.h"\
	{$(INCLUDE)}"\CipcInputRecord.h"\
	{$(INCLUDE)}"\CipcServerControl.h"\
	{$(INCLUDE)}"\CipcServerControlClientSide.h"\
	{$(INCLUDE)}"\Host.h"\
	{$(INCLUDE)}"\SecID.h"\
	{$(INCLUDE)}"\TimedMessage.h"\
	{$(INCLUDE)}"\UpdateDefines.h"\
	{$(INCLUDE)}"\User.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Template.h"\
	{$(INCLUDE)}"\WK_Trace.h"\
	

"$(INTDIR)\Tester.obj" : $(SOURCE) $(DEP_CPP_TESTE) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

DEP_CPP_TESTE=\
	"..\..\Cipc\CipcFetchResult.h"\
	".\ChildFrm.h"\
	".\CIPCInputTester.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	".\Tester.h"\
	".\TesterDoc.h"\
	".\TesterView.h"\
	".\TickCount.h"\
	{$(INCLUDE)}"\CameraControl.h"\
	{$(INCLUDE)}"\Cipc.h"\
	{$(INCLUDE)}"\CipcInput.h"\
	{$(INCLUDE)}"\CipcInputRecord.h"\
	{$(INCLUDE)}"\CipcServerControl.h"\
	{$(INCLUDE)}"\CipcServerControlClientSide.h"\
	{$(INCLUDE)}"\Host.h"\
	{$(INCLUDE)}"\SecID.h"\
	{$(INCLUDE)}"\TimedMessage.h"\
	{$(INCLUDE)}"\UpdateDefines.h"\
	{$(INCLUDE)}"\User.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Template.h"\
	{$(INCLUDE)}"\WK_Trace.h"\
	

"$(INTDIR)\Tester.obj" : $(SOURCE) $(DEP_CPP_TESTE) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"

"$(INTDIR)\Tester.sbr" : $(SOURCE) $(DEP_CPP_TESTE) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Tester - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MD /W3 /GX /Od /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tester.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tester.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Tester.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tester.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\StdAfx.h"\
	".\Tester.h"\
	

!IF  "$(CFG)" == "Tester - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\ChildFrm.h"\
	".\StdAfx.h"\
	".\Tester.h"\
	

!IF  "$(CFG)" == "Tester - Win32 Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"

"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TesterDoc.cpp
DEP_CPP_TESTER=\
	".\StdAfx.h"\
	".\Tester.h"\
	".\TesterDoc.h"\
	

!IF  "$(CFG)" == "Tester - Win32 Release"


"$(INTDIR)\TesterDoc.obj" : $(SOURCE) $(DEP_CPP_TESTER) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"


"$(INTDIR)\TesterDoc.obj" : $(SOURCE) $(DEP_CPP_TESTER) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"

"$(INTDIR)\TesterDoc.sbr" : $(SOURCE) $(DEP_CPP_TESTER) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TesterView.cpp
DEP_CPP_TESTERV=\
	".\StdAfx.h"\
	".\Tester.h"\
	".\TesterDoc.h"\
	".\TesterView.h"\
	

!IF  "$(CFG)" == "Tester - Win32 Release"


"$(INTDIR)\TesterView.obj" : $(SOURCE) $(DEP_CPP_TESTERV) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"


"$(INTDIR)\TesterView.obj" : $(SOURCE) $(DEP_CPP_TESTERV) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"

"$(INTDIR)\TesterView.sbr" : $(SOURCE) $(DEP_CPP_TESTERV) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Tester.rc
DEP_RSC_TESTER_=\
	".\res\Tester.ico"\
	".\res\Tester.rc2"\
	".\res\TesterDoc.ico"\
	".\res\Toolbar.bmp"\
	

"$(INTDIR)\Tester.res" : $(SOURCE) $(DEP_RSC_TESTER_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hlp\Tester.hpj

!IF  "$(CFG)" == "Tester - Win32 Release"

# Begin Custom Build - Making help file...
OutDir=.\Release
ProjDir=.
TargetName=Tester
InputPath=.\hlp\Tester.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

# Begin Custom Build - Making help file...
OutDir=.\Debug
ProjDir=.
TargetName=Tester
InputPath=.\hlp\Tester.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   "$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Project\Cipc\Debug\CipcDebug.lib

!IF  "$(CFG)" == "Tester - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Project\Cipc\Release\Cipc.lib

!IF  "$(CFG)" == "Tester - Win32 Release"

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CIPCInputTester.cpp

!IF  "$(CFG)" == "Tester - Win32 Release"

DEP_CPP_CIPCI=\
	".\CIPCInputTester.h"\
	".\StdAfx.h"\
	".\Tester.h"\
	".\TickCount.h"\
	{$(INCLUDE)}"\CameraControl.h"\
	{$(INCLUDE)}"\Cipc.h"\
	{$(INCLUDE)}"\CipcInput.h"\
	{$(INCLUDE)}"\CipcInputRecord.h"\
	{$(INCLUDE)}"\SecID.h"\
	{$(INCLUDE)}"\TimedMessage.h"\
	{$(INCLUDE)}"\UpdateDefines.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Template.h"\
	{$(INCLUDE)}"\WK_Trace.h"\
	

"$(INTDIR)\CIPCInputTester.obj" : $(SOURCE) $(DEP_CPP_CIPCI) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

DEP_CPP_CIPCI=\
	".\CIPCInputTester.h"\
	".\StdAfx.h"\
	".\Tester.h"\
	".\TickCount.h"\
	{$(INCLUDE)}"\CameraControl.h"\
	{$(INCLUDE)}"\Cipc.h"\
	{$(INCLUDE)}"\CipcInput.h"\
	{$(INCLUDE)}"\CipcInputRecord.h"\
	{$(INCLUDE)}"\SecID.h"\
	{$(INCLUDE)}"\TimedMessage.h"\
	{$(INCLUDE)}"\UpdateDefines.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Template.h"\
	{$(INCLUDE)}"\WK_Trace.h"\
	

"$(INTDIR)\CIPCInputTester.obj" : $(SOURCE) $(DEP_CPP_CIPCI) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"

"$(INTDIR)\CIPCInputTester.sbr" : $(SOURCE) $(DEP_CPP_CIPCI) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TickCount.cpp
DEP_CPP_TICKC=\
	".\StdAfx.h"\
	".\TickCount.h"\
	

!IF  "$(CFG)" == "Tester - Win32 Release"


"$(INTDIR)\TickCount.obj" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"


"$(INTDIR)\TickCount.obj" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"

"$(INTDIR)\TickCount.sbr" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\Tester.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\History.txt

!IF  "$(CFG)" == "Tester - Win32 Release"

!ELSEIF  "$(CFG)" == "Tester - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
