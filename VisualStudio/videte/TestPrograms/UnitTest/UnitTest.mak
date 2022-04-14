# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=UnitTest - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to UnitTest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "UnitTest - Win32 Release" && "$(CFG)" !=\
 "UnitTest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnitTest.mak" CFG="UnitTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnitTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "UnitTest - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "UnitTest - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "UnitTest - Win32 Release"

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

ALL : "$(OUTDIR)\UnitTest.exe"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\OutputTester.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TestTool.obj"
	-@erase "$(INTDIR)\TickCount.obj"
	-@erase "$(INTDIR)\UnitTest.obj"
	-@erase "$(INTDIR)\UnitTest.pch"
	-@erase "$(INTDIR)\UnitTest.res"
	-@erase "$(INTDIR)\UnitTestDoc.obj"
	-@erase "$(INTDIR)\UnitTestView.obj"
	-@erase "$(OUTDIR)\UnitTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/UnitTest.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x407 /fo"$(INTDIR)/UnitTest.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UnitTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/UnitTest.pdb" /machine:I386 /out:"$(OUTDIR)/UnitTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\OutputTester.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TestTool.obj" \
	"$(INTDIR)\TickCount.obj" \
	"$(INTDIR)\UnitTest.obj" \
	"$(INTDIR)\UnitTest.res" \
	"$(INTDIR)\UnitTestDoc.obj" \
	"$(INTDIR)\UnitTestView.obj" \
	"..\..\Cipc\Debug\CipcDebug.lib"

"$(OUTDIR)\UnitTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"

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

ALL : "$(OUTDIR)\UnitTest.exe" "$(OUTDIR)\UnitTest.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\OutputTester.obj"
	-@erase "$(INTDIR)\OutputTester.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TestTool.obj"
	-@erase "$(INTDIR)\TestTool.sbr"
	-@erase "$(INTDIR)\TickCount.obj"
	-@erase "$(INTDIR)\TickCount.sbr"
	-@erase "$(INTDIR)\UnitTest.obj"
	-@erase "$(INTDIR)\UnitTest.pch"
	-@erase "$(INTDIR)\UnitTest.res"
	-@erase "$(INTDIR)\UnitTest.sbr"
	-@erase "$(INTDIR)\UnitTestDoc.obj"
	-@erase "$(INTDIR)\UnitTestDoc.sbr"
	-@erase "$(INTDIR)\UnitTestView.obj"
	-@erase "$(INTDIR)\UnitTestView.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\UnitTest.bsc"
	-@erase "$(OUTDIR)\UnitTest.exe"
	-@erase "$(OUTDIR)\UnitTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/UnitTest.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x407 /fo"$(INTDIR)/UnitTest.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UnitTest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\OutputTester.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TestTool.sbr" \
	"$(INTDIR)\TickCount.sbr" \
	"$(INTDIR)\UnitTest.sbr" \
	"$(INTDIR)\UnitTestDoc.sbr" \
	"$(INTDIR)\UnitTestView.sbr"

"$(OUTDIR)\UnitTest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/UnitTest.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/UnitTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\OutputTester.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TestTool.obj" \
	"$(INTDIR)\TickCount.obj" \
	"$(INTDIR)\UnitTest.obj" \
	"$(INTDIR)\UnitTest.res" \
	"$(INTDIR)\UnitTestDoc.obj" \
	"$(INTDIR)\UnitTestView.obj" \
	"..\..\Cipc\Debug\CipcDebug.lib"

"$(OUTDIR)\UnitTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "UnitTest - Win32 Release"
# Name "UnitTest - Win32 Debug"

!IF  "$(CFG)" == "UnitTest - Win32 Release"

!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\UnitTest.cpp
DEP_CPP_UNITT=\
	"..\..\Cipc\Cipc.h"\
	"..\..\Cipc\CipcFetchResult.h"\
	"..\..\Cipc\CipcServerControl.h"\
	"..\..\Cipc\PictureDef.h"\
	"..\..\Cipc\SecID.h"\
	"..\..\Cipc\TimedMessage.h"\
	"..\..\Cipc\WK_Trace.h"\
	".\ChildFrm.h"\
	".\MainFrm.h"\
	".\OutputTester.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	".\TickCount.h"\
	".\UnitTest.h"\
	".\UnitTestDoc.h"\
	".\UnitTestView.h"\
	{$(INCLUDE)}"\CipcOutput.h"\
	{$(INCLUDE)}"\CipcServerControlClientSide.h"\
	{$(INCLUDE)}"\Host.h"\
	{$(INCLUDE)}"\User.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Template.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"


"$(INTDIR)\UnitTest.obj" : $(SOURCE) $(DEP_CPP_UNITT) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"


"$(INTDIR)\UnitTest.obj" : $(SOURCE) $(DEP_CPP_UNITT) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\UnitTest.sbr" : $(SOURCE) $(DEP_CPP_UNITT) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/UnitTest.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UnitTest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/UnitTest.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UnitTest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\StdAfx.h"\
	".\UnitTest.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\ChildFrm.h"\
	".\StdAfx.h"\
	".\UnitTest.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\UnitTestDoc.cpp
DEP_CPP_UNITTE=\
	".\StdAfx.h"\
	".\UnitTest.h"\
	".\UnitTestDoc.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"


"$(INTDIR)\UnitTestDoc.obj" : $(SOURCE) $(DEP_CPP_UNITTE) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"


"$(INTDIR)\UnitTestDoc.obj" : $(SOURCE) $(DEP_CPP_UNITTE) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\UnitTestDoc.sbr" : $(SOURCE) $(DEP_CPP_UNITTE) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\UnitTestView.cpp
DEP_CPP_UNITTES=\
	".\StdAfx.h"\
	".\UnitTest.h"\
	".\UnitTestDoc.h"\
	".\UnitTestView.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"


"$(INTDIR)\UnitTestView.obj" : $(SOURCE) $(DEP_CPP_UNITTES) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"


"$(INTDIR)\UnitTestView.obj" : $(SOURCE) $(DEP_CPP_UNITTES) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\UnitTestView.sbr" : $(SOURCE) $(DEP_CPP_UNITTES) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\UnitTest.rc
DEP_RSC_UNITTEST=\
	".\res\Toolbar.bmp"\
	".\res\UnitTest.ico"\
	".\res\UnitTest.rc2"\
	".\res\UnitTestDoc.ico"\
	

"$(INTDIR)\UnitTest.res" : $(SOURCE) $(DEP_RSC_UNITTEST) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TestTool.cpp
DEP_CPP_TESTT=\
	"..\..\Cipc\Cipc.h"\
	"..\..\Cipc\CipcFetchResult.h"\
	"..\..\Cipc\CipcServerControl.h"\
	"..\..\Cipc\PictureDef.h"\
	"..\..\Cipc\SecID.h"\
	"..\..\Cipc\TimedMessage.h"\
	"..\..\Cipc\WK_Trace.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	{$(INCLUDE)}"\CipcServerControlClientSide.h"\
	{$(INCLUDE)}"\Host.h"\
	{$(INCLUDE)}"\User.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Names.h"\
	{$(INCLUDE)}"\WK_Template.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"


"$(INTDIR)\TestTool.obj" : $(SOURCE) $(DEP_CPP_TESTT) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"


"$(INTDIR)\TestTool.obj" : $(SOURCE) $(DEP_CPP_TESTT) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\TestTool.sbr" : $(SOURCE) $(DEP_CPP_TESTT) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OutputTester.cpp

!IF  "$(CFG)" == "UnitTest - Win32 Release"

DEP_CPP_OUTPU=\
	"..\..\Cipc\Cipc.h"\
	"..\..\Cipc\PictureDef.h"\
	"..\..\Cipc\SecID.h"\
	"..\..\Cipc\TimedMessage.h"\
	"..\..\Cipc\WK_Trace.h"\
	".\OutputTester.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	".\TickCount.h"\
	{$(INCLUDE)}"\CipcExtraMemory.h"\
	{$(INCLUDE)}"\CipcOutput.h"\
	{$(INCLUDE)}"\PictureRecord.h"\
	{$(INCLUDE)}"\Util.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Names.h"\
	{$(INCLUDE)}"\WK_Template.h"\
	

"$(INTDIR)\OutputTester.obj" : $(SOURCE) $(DEP_CPP_OUTPU) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"

DEP_CPP_OUTPU=\
	"..\..\Cipc\Cipc.h"\
	"..\..\Cipc\PictureDef.h"\
	"..\..\Cipc\SecID.h"\
	"..\..\Cipc\TimedMessage.h"\
	"..\..\Cipc\WK_Trace.h"\
	".\OutputTester.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	".\TickCount.h"\
	{$(INCLUDE)}"\CipcExtraMemory.h"\
	{$(INCLUDE)}"\CipcOutput.h"\
	{$(INCLUDE)}"\PictureRecord.h"\
	{$(INCLUDE)}"\Util.h"\
	{$(INCLUDE)}"\Wk.h"\
	{$(INCLUDE)}"\WK_Names.h"\
	

"$(INTDIR)\OutputTester.obj" : $(SOURCE) $(DEP_CPP_OUTPU) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\OutputTester.sbr" : $(SOURCE) $(DEP_CPP_OUTPU) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Project\Cipc\Debug\CipcDebug.lib

!IF  "$(CFG)" == "UnitTest - Win32 Release"

!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TickCount.cpp
DEP_CPP_TICKC=\
	".\StdAfx.h"\
	".\TickCount.h"\
	

!IF  "$(CFG)" == "UnitTest - Win32 Release"


"$(INTDIR)\TickCount.obj" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"


"$(INTDIR)\TickCount.obj" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"

"$(INTDIR)\TickCount.sbr" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\UnitTest.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
