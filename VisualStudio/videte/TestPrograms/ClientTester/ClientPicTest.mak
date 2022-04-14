# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=ClientPicTest - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ClientPicTest - Win32\
 Debug.
!ENDIF 

!IF "$(CFG)" != "ClientPicTest - Win32 Release" && "$(CFG)" !=\
 "ClientPicTest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ClientPicTest.mak" CFG="ClientPicTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ClientPicTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ClientPicTest - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "ClientPicTest - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "ClientPicTest - Win32 Release"

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

ALL : "$(OUTDIR)\ClientPicTest.exe" "$(OUTDIR)\ClientPicTest.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\ClientPicTest.obj"
	-@erase "$(INTDIR)\ClientPicTest.pch"
	-@erase "$(INTDIR)\ClientPicTest.res"
	-@erase "$(INTDIR)\ClientPicTest.sbr"
	-@erase "$(INTDIR)\ClientPicTestDoc.obj"
	-@erase "$(INTDIR)\ClientPicTestDoc.sbr"
	-@erase "$(INTDIR)\ClientPicTestView.obj"
	-@erase "$(INTDIR)\ClientPicTestView.sbr"
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
	-@erase "$(OUTDIR)\ClientPicTest.bsc"
	-@erase "$(OUTDIR)\ClientPicTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/ClientPicTest.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x407 /fo"$(INTDIR)/ClientPicTest.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ClientPicTest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\ClientPicTest.sbr" \
	"$(INTDIR)\ClientPicTestDoc.sbr" \
	"$(INTDIR)\ClientPicTestView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\OutputTester.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TestTool.sbr" \
	"$(INTDIR)\TickCount.sbr"

"$(OUTDIR)\ClientPicTest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ClientPicTest.pdb" /machine:I386\
 /out:"$(OUTDIR)/ClientPicTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\ClientPicTest.obj" \
	"$(INTDIR)\ClientPicTest.res" \
	"$(INTDIR)\ClientPicTestDoc.obj" \
	"$(INTDIR)\ClientPicTestView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\OutputTester.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TestTool.obj" \
	"$(INTDIR)\TickCount.obj" \
	"..\..\Cipc\Debug\CipcDebug.lib"

"$(OUTDIR)\ClientPicTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ClientPicTest - Win32 Debug"

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

ALL : "$(OUTDIR)\ClientPicTest.exe" "$(OUTDIR)\ClientPicTest.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\ClientPicTest.obj"
	-@erase "$(INTDIR)\ClientPicTest.pch"
	-@erase "$(INTDIR)\ClientPicTest.res"
	-@erase "$(INTDIR)\ClientPicTest.sbr"
	-@erase "$(INTDIR)\ClientPicTestDoc.obj"
	-@erase "$(INTDIR)\ClientPicTestDoc.sbr"
	-@erase "$(INTDIR)\ClientPicTestView.obj"
	-@erase "$(INTDIR)\ClientPicTestView.sbr"
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
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\ClientPicTest.bsc"
	-@erase "$(OUTDIR)\ClientPicTest.exe"
	-@erase "$(OUTDIR)\ClientPicTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/ClientPicTest.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x407 /fo"$(INTDIR)/ClientPicTest.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ClientPicTest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\ClientPicTest.sbr" \
	"$(INTDIR)\ClientPicTestDoc.sbr" \
	"$(INTDIR)\ClientPicTestView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\OutputTester.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TestTool.sbr" \
	"$(INTDIR)\TickCount.sbr"

"$(OUTDIR)\ClientPicTest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ClientPicTest.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/ClientPicTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\ClientPicTest.obj" \
	"$(INTDIR)\ClientPicTest.res" \
	"$(INTDIR)\ClientPicTestDoc.obj" \
	"$(INTDIR)\ClientPicTestView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\OutputTester.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TestTool.obj" \
	"$(INTDIR)\TickCount.obj" \
	"..\..\Cipc\Debug\CipcDebug.lib"

"$(OUTDIR)\ClientPicTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "ClientPicTest - Win32 Release"
# Name "ClientPicTest - Win32 Debug"

!IF  "$(CFG)" == "ClientPicTest - Win32 Release"

!ELSEIF  "$(CFG)" == "ClientPicTest - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ClientPicTest.cpp

!IF  "$(CFG)" == "ClientPicTest - Win32 Release"

DEP_CPP_CLIEN=\
	".\ChildFrm.h"\
	".\CipcServerControlClientSide.h"\
	".\ClientPicTest.h"\
	".\ClientPicTestDoc.h"\
	".\ClientPicTestView.h"\
	".\Host.h"\
	".\MainFrm.h"\
	".\OutputTester.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	".\TickCount.h"\
	".\User.h"\
	".\Wk.h"\
	
NODEP_CPP_CLIEN=\
	".\CipcOutput.h"\
	".\CipcOutputClient.h"\
	".\PictureDef.h"\
	".\WK_Trace.h"\
	

"$(INTDIR)\ClientPicTest.obj" : $(SOURCE) $(DEP_CPP_CLIEN) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\ClientPicTest.sbr" : $(SOURCE) $(DEP_CPP_CLIEN) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


!ELSEIF  "$(CFG)" == "ClientPicTest - Win32 Debug"

DEP_CPP_CLIEN=\
	".\ChildFrm.h"\
	".\CipcOutput.h"\
	".\CipcOutputClient.h"\
	".\CipcServerControlClientSide.h"\
	".\ClientPicTest.h"\
	".\ClientPicTestDoc.h"\
	".\ClientPicTestView.h"\
	".\Host.h"\
	".\MainFrm.h"\
	".\OutputTester.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	".\TickCount.h"\
	".\User.h"\
	".\Wk.h"\
	
NODEP_CPP_CLIEN=\
	".\PictureDef.h"\
	".\WK_Trace.h"\
	

"$(INTDIR)\ClientPicTest.obj" : $(SOURCE) $(DEP_CPP_CLIEN) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\ClientPicTest.sbr" : $(SOURCE) $(DEP_CPP_CLIEN) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClientPicTest - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/ClientPicTest.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ClientPicTest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ClientPicTest - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/ClientPicTest.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ClientPicTest.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\ClientPicTest.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\ChildFrm.h"\
	".\ClientPicTest.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClientPicTestDoc.cpp
DEP_CPP_CLIENT=\
	".\ClientPicTest.h"\
	".\ClientPicTestDoc.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ClientPicTestDoc.obj" : $(SOURCE) $(DEP_CPP_CLIENT) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\ClientPicTestDoc.sbr" : $(SOURCE) $(DEP_CPP_CLIENT) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClientPicTestView.cpp
DEP_CPP_CLIENTP=\
	".\ClientPicTest.h"\
	".\ClientPicTestDoc.h"\
	".\ClientPicTestView.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ClientPicTestView.obj" : $(SOURCE) $(DEP_CPP_CLIENTP) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\ClientPicTestView.sbr" : $(SOURCE) $(DEP_CPP_CLIENTP) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClientPicTest.rc
DEP_RSC_CLIENTPI=\
	".\res\ClientPicTest.ico"\
	".\res\ClientPicTest.rc2"\
	".\res\ClientPicTestDoc.ico"\
	".\res\Toolbar.bmp"\
	

"$(INTDIR)\ClientPicTest.res" : $(SOURCE) $(DEP_RSC_CLIENTPI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TestTool.cpp
DEP_CPP_TESTT=\
	".\CipcServerControlClientSide.h"\
	".\Host.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	".\User.h"\
	".\Wk.h"\
	".\WK_Names.h"\
	
NODEP_CPP_TESTT=\
	".\PictureDef.h"\
	

"$(INTDIR)\TestTool.obj" : $(SOURCE) $(DEP_CPP_TESTT) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\TestTool.sbr" : $(SOURCE) $(DEP_CPP_TESTT) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\OutputTester.cpp
DEP_CPP_OUTPU=\
	".\CipcExtraMemory.h"\
	".\CipcOutput.h"\
	".\CipcOutputClient.h"\
	".\ClientPicTest.h"\
	".\MainFrm.h"\
	".\OutputTester.h"\
	".\PictureRecord.h"\
	".\StdAfx.h"\
	".\TestTool.h"\
	".\TickCount.h"\
	".\Util.h"\
	".\WK_Names.h"\
	
NODEP_CPP_OUTPU=\
	".\PictureDef.h"\
	

"$(INTDIR)\OutputTester.obj" : $(SOURCE) $(DEP_CPP_OUTPU) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\OutputTester.sbr" : $(SOURCE) $(DEP_CPP_OUTPU) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TickCount.cpp
DEP_CPP_TICKC=\
	".\StdAfx.h"\
	".\TickCount.h"\
	

"$(INTDIR)\TickCount.obj" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"

"$(INTDIR)\TickCount.sbr" : $(SOURCE) $(DEP_CPP_TICKC) "$(INTDIR)"\
 "$(INTDIR)\ClientPicTest.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Project\Cipc\Debug\CipcDebug.lib

!IF  "$(CFG)" == "ClientPicTest - Win32 Release"

!ELSEIF  "$(CFG)" == "ClientPicTest - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
