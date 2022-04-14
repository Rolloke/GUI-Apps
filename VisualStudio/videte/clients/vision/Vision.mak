# Microsoft Developer Studio Generated NMAKE File, Based on Vision.dsp
!IF "$(CFG)" == ""
CFG=Vision - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Vision - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Vision - Win32 Release" && "$(CFG)" != "Vision - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Vision.mak" CFG="Vision - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Vision - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Vision - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Vision - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Vision.exe"


CLEAN :
	-@erase "$(INTDIR)\CIPCDataBaseVision.obj"
	-@erase "$(INTDIR)\CIPCInputVision.obj"
	-@erase "$(INTDIR)\CIPCInputVisionCommData.obj"
	-@erase "$(INTDIR)\CIPCOutputVision.obj"
	-@erase "$(INTDIR)\CIPCOutputVisionDecoder.obj"
	-@erase "$(INTDIR)\CIPCServerControlVision.obj"
	-@erase "$(INTDIR)\CocoWindow.obj"
	-@erase "$(INTDIR)\ConnectionDialog.obj"
	-@erase "$(INTDIR)\csbdialog.obj"
	-@erase "$(INTDIR)\Custsite.obj"
	-@erase "$(INTDIR)\DisplayWindow.obj"
	-@erase "$(INTDIR)\HtmlWindow.obj"
	-@erase "$(INTDIR)\Idispimp.obj"
	-@erase "$(INTDIR)\loadsavesequencerdialog.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MicoWindow.obj"
	-@erase "$(INTDIR)\ObjectView.obj"
	-@erase "$(INTDIR)\picturelengthstatistic.obj"
	-@erase "$(INTDIR)\ptwindow.obj"
	-@erase "$(INTDIR)\Sequencer.obj"
	-@erase "$(INTDIR)\Server.obj"
	-@erase "$(INTDIR)\Servers.obj"
	-@erase "$(INTDIR)\SmallToolBar.obj"
	-@erase "$(INTDIR)\SmallWindow.obj"
	-@erase "$(INTDIR)\SNConfigDialog.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\videosndialog.obj"
	-@erase "$(INTDIR)\Vision.obj"
	-@erase "$(INTDIR)\Vision.pch"
	-@erase "$(INTDIR)\Vision.res"
	-@erase "$(INTDIR)\VisionCoolBar.obj"
	-@erase "$(INTDIR)\VisionDoc.obj"
	-@erase "$(INTDIR)\VisionView.obj"
	-@erase "$(INTDIR)\webbrowser2.obj"
	-@erase "$(OUTDIR)\Vision.exe"
	-@erase "$(OUTDIR)\Vision.map"
	-@erase "$(OUTDIR)\Vision.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Vision.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\Vision.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Vision.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=cipc.lib oemgui.lib IMAGN32.lib client40.lib ptdecoder.lib h263dec.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Vision.pdb" /map:"$(INTDIR)\Vision.map" /debug /machine:I386 /out:"$(OUTDIR)\Vision.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CIPCDataBaseVision.obj" \
	"$(INTDIR)\CIPCInputVision.obj" \
	"$(INTDIR)\CIPCInputVisionCommData.obj" \
	"$(INTDIR)\CIPCOutputVision.obj" \
	"$(INTDIR)\CIPCOutputVisionDecoder.obj" \
	"$(INTDIR)\CIPCServerControlVision.obj" \
	"$(INTDIR)\CocoWindow.obj" \
	"$(INTDIR)\ConnectionDialog.obj" \
	"$(INTDIR)\csbdialog.obj" \
	"$(INTDIR)\Custsite.obj" \
	"$(INTDIR)\DisplayWindow.obj" \
	"$(INTDIR)\HtmlWindow.obj" \
	"$(INTDIR)\Idispimp.obj" \
	"$(INTDIR)\loadsavesequencerdialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MicoWindow.obj" \
	"$(INTDIR)\ObjectView.obj" \
	"$(INTDIR)\picturelengthstatistic.obj" \
	"$(INTDIR)\ptwindow.obj" \
	"$(INTDIR)\Sequencer.obj" \
	"$(INTDIR)\Server.obj" \
	"$(INTDIR)\Servers.obj" \
	"$(INTDIR)\SmallToolBar.obj" \
	"$(INTDIR)\SmallWindow.obj" \
	"$(INTDIR)\SNConfigDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\videosndialog.obj" \
	"$(INTDIR)\Vision.obj" \
	"$(INTDIR)\VisionCoolBar.obj" \
	"$(INTDIR)\VisionDoc.obj" \
	"$(INTDIR)\VisionView.obj" \
	"$(INTDIR)\webbrowser2.obj" \
	"$(INTDIR)\Vision.res"

"$(OUTDIR)\Vision.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Vision.exe" "$(OUTDIR)\Vision.bsc"


CLEAN :
	-@erase "$(INTDIR)\CIPCDataBaseVision.obj"
	-@erase "$(INTDIR)\CIPCDataBaseVision.sbr"
	-@erase "$(INTDIR)\CIPCInputVision.obj"
	-@erase "$(INTDIR)\CIPCInputVision.sbr"
	-@erase "$(INTDIR)\CIPCInputVisionCommData.obj"
	-@erase "$(INTDIR)\CIPCInputVisionCommData.sbr"
	-@erase "$(INTDIR)\CIPCOutputVision.obj"
	-@erase "$(INTDIR)\CIPCOutputVision.sbr"
	-@erase "$(INTDIR)\CIPCOutputVisionDecoder.obj"
	-@erase "$(INTDIR)\CIPCOutputVisionDecoder.sbr"
	-@erase "$(INTDIR)\CIPCServerControlVision.obj"
	-@erase "$(INTDIR)\CIPCServerControlVision.sbr"
	-@erase "$(INTDIR)\CocoWindow.obj"
	-@erase "$(INTDIR)\CocoWindow.sbr"
	-@erase "$(INTDIR)\ConnectionDialog.obj"
	-@erase "$(INTDIR)\ConnectionDialog.sbr"
	-@erase "$(INTDIR)\csbdialog.obj"
	-@erase "$(INTDIR)\csbdialog.sbr"
	-@erase "$(INTDIR)\Custsite.obj"
	-@erase "$(INTDIR)\Custsite.sbr"
	-@erase "$(INTDIR)\DisplayWindow.obj"
	-@erase "$(INTDIR)\DisplayWindow.sbr"
	-@erase "$(INTDIR)\HtmlWindow.obj"
	-@erase "$(INTDIR)\HtmlWindow.sbr"
	-@erase "$(INTDIR)\Idispimp.obj"
	-@erase "$(INTDIR)\Idispimp.sbr"
	-@erase "$(INTDIR)\loadsavesequencerdialog.obj"
	-@erase "$(INTDIR)\loadsavesequencerdialog.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MicoWindow.obj"
	-@erase "$(INTDIR)\MicoWindow.sbr"
	-@erase "$(INTDIR)\ObjectView.obj"
	-@erase "$(INTDIR)\ObjectView.sbr"
	-@erase "$(INTDIR)\picturelengthstatistic.obj"
	-@erase "$(INTDIR)\picturelengthstatistic.sbr"
	-@erase "$(INTDIR)\ptwindow.obj"
	-@erase "$(INTDIR)\ptwindow.sbr"
	-@erase "$(INTDIR)\Sequencer.obj"
	-@erase "$(INTDIR)\Sequencer.sbr"
	-@erase "$(INTDIR)\Server.obj"
	-@erase "$(INTDIR)\Server.sbr"
	-@erase "$(INTDIR)\Servers.obj"
	-@erase "$(INTDIR)\Servers.sbr"
	-@erase "$(INTDIR)\SmallToolBar.obj"
	-@erase "$(INTDIR)\SmallToolBar.sbr"
	-@erase "$(INTDIR)\SmallWindow.obj"
	-@erase "$(INTDIR)\SmallWindow.sbr"
	-@erase "$(INTDIR)\SNConfigDialog.obj"
	-@erase "$(INTDIR)\SNConfigDialog.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\videosndialog.obj"
	-@erase "$(INTDIR)\videosndialog.sbr"
	-@erase "$(INTDIR)\Vision.obj"
	-@erase "$(INTDIR)\Vision.pch"
	-@erase "$(INTDIR)\Vision.res"
	-@erase "$(INTDIR)\Vision.sbr"
	-@erase "$(INTDIR)\VisionCoolBar.obj"
	-@erase "$(INTDIR)\VisionCoolBar.sbr"
	-@erase "$(INTDIR)\VisionDoc.obj"
	-@erase "$(INTDIR)\VisionDoc.sbr"
	-@erase "$(INTDIR)\VisionView.obj"
	-@erase "$(INTDIR)\VisionView.sbr"
	-@erase "$(INTDIR)\webbrowser2.obj"
	-@erase "$(INTDIR)\webbrowser2.sbr"
	-@erase "$(OUTDIR)\Vision.bsc"
	-@erase "$(OUTDIR)\Vision.exe"
	-@erase "$(OUTDIR)\Vision.ilk"
	-@erase "$(OUTDIR)\Vision.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Vision.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\Vision.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Vision.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CIPCDataBaseVision.sbr" \
	"$(INTDIR)\CIPCInputVision.sbr" \
	"$(INTDIR)\CIPCInputVisionCommData.sbr" \
	"$(INTDIR)\CIPCOutputVision.sbr" \
	"$(INTDIR)\CIPCOutputVisionDecoder.sbr" \
	"$(INTDIR)\CIPCServerControlVision.sbr" \
	"$(INTDIR)\CocoWindow.sbr" \
	"$(INTDIR)\ConnectionDialog.sbr" \
	"$(INTDIR)\csbdialog.sbr" \
	"$(INTDIR)\Custsite.sbr" \
	"$(INTDIR)\DisplayWindow.sbr" \
	"$(INTDIR)\HtmlWindow.sbr" \
	"$(INTDIR)\Idispimp.sbr" \
	"$(INTDIR)\loadsavesequencerdialog.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\MicoWindow.sbr" \
	"$(INTDIR)\ObjectView.sbr" \
	"$(INTDIR)\picturelengthstatistic.sbr" \
	"$(INTDIR)\ptwindow.sbr" \
	"$(INTDIR)\Sequencer.sbr" \
	"$(INTDIR)\Server.sbr" \
	"$(INTDIR)\Servers.sbr" \
	"$(INTDIR)\SmallToolBar.sbr" \
	"$(INTDIR)\SmallWindow.sbr" \
	"$(INTDIR)\SNConfigDialog.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\videosndialog.sbr" \
	"$(INTDIR)\Vision.sbr" \
	"$(INTDIR)\VisionCoolBar.sbr" \
	"$(INTDIR)\VisionDoc.sbr" \
	"$(INTDIR)\VisionView.sbr" \
	"$(INTDIR)\webbrowser2.sbr"

"$(OUTDIR)\Vision.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=oemguid.lib IMAGN32.lib cipcdebug.lib client40d.lib ptdecoderd.lib h263decd.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Vision.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Vision.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CIPCDataBaseVision.obj" \
	"$(INTDIR)\CIPCInputVision.obj" \
	"$(INTDIR)\CIPCInputVisionCommData.obj" \
	"$(INTDIR)\CIPCOutputVision.obj" \
	"$(INTDIR)\CIPCOutputVisionDecoder.obj" \
	"$(INTDIR)\CIPCServerControlVision.obj" \
	"$(INTDIR)\CocoWindow.obj" \
	"$(INTDIR)\ConnectionDialog.obj" \
	"$(INTDIR)\csbdialog.obj" \
	"$(INTDIR)\Custsite.obj" \
	"$(INTDIR)\DisplayWindow.obj" \
	"$(INTDIR)\HtmlWindow.obj" \
	"$(INTDIR)\Idispimp.obj" \
	"$(INTDIR)\loadsavesequencerdialog.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MicoWindow.obj" \
	"$(INTDIR)\ObjectView.obj" \
	"$(INTDIR)\picturelengthstatistic.obj" \
	"$(INTDIR)\ptwindow.obj" \
	"$(INTDIR)\Sequencer.obj" \
	"$(INTDIR)\Server.obj" \
	"$(INTDIR)\Servers.obj" \
	"$(INTDIR)\SmallToolBar.obj" \
	"$(INTDIR)\SmallWindow.obj" \
	"$(INTDIR)\SNConfigDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\videosndialog.obj" \
	"$(INTDIR)\Vision.obj" \
	"$(INTDIR)\VisionCoolBar.obj" \
	"$(INTDIR)\VisionDoc.obj" \
	"$(INTDIR)\VisionView.obj" \
	"$(INTDIR)\webbrowser2.obj" \
	"$(INTDIR)\Vision.res"

"$(OUTDIR)\Vision.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Vision.dep")
!INCLUDE "Vision.dep"
!ELSE 
!MESSAGE Warning: cannot find "Vision.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Vision - Win32 Release" || "$(CFG)" == "Vision - Win32 Debug"
SOURCE=.\CIPCDataBaseVision.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\CIPCDataBaseVision.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\CIPCDataBaseVision.obj"	"$(INTDIR)\CIPCDataBaseVision.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\CIPCInputVision.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\CIPCInputVision.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\CIPCInputVision.obj"	"$(INTDIR)\CIPCInputVision.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\CIPCInputVisionCommData.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\CIPCInputVisionCommData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\CIPCInputVisionCommData.obj"	"$(INTDIR)\CIPCInputVisionCommData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\CIPCOutputVision.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\CIPCOutputVision.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\CIPCOutputVision.obj"	"$(INTDIR)\CIPCOutputVision.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\CIPCOutputVisionDecoder.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\CIPCOutputVisionDecoder.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\CIPCOutputVisionDecoder.obj"	"$(INTDIR)\CIPCOutputVisionDecoder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\CIPCServerControlVision.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\CIPCServerControlVision.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\CIPCServerControlVision.obj"	"$(INTDIR)\CIPCServerControlVision.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\CocoWindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\CocoWindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\CocoWindow.obj"	"$(INTDIR)\CocoWindow.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\ConnectionDialog.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\ConnectionDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\ConnectionDialog.obj"	"$(INTDIR)\ConnectionDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\csbdialog.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\csbdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\csbdialog.obj"	"$(INTDIR)\csbdialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\Custsite.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\Custsite.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\Custsite.obj"	"$(INTDIR)\Custsite.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\DisplayWindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\DisplayWindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\DisplayWindow.obj"	"$(INTDIR)\DisplayWindow.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\HtmlWindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\HtmlWindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\HtmlWindow.obj"	"$(INTDIR)\HtmlWindow.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\Idispimp.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\Idispimp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\Idispimp.obj"	"$(INTDIR)\Idispimp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\loadsavesequencerdialog.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\loadsavesequencerdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\loadsavesequencerdialog.obj"	"$(INTDIR)\loadsavesequencerdialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\MicoWindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\MicoWindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\MicoWindow.obj"	"$(INTDIR)\MicoWindow.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\ObjectView.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\ObjectView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\ObjectView.obj"	"$(INTDIR)\ObjectView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\picturelengthstatistic.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\picturelengthstatistic.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\picturelengthstatistic.obj"	"$(INTDIR)\picturelengthstatistic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\ptwindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\ptwindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\ptwindow.obj"	"$(INTDIR)\ptwindow.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\Sequencer.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\Sequencer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\Sequencer.obj"	"$(INTDIR)\Sequencer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\Server.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\Server.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\Server.obj"	"$(INTDIR)\Server.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\Servers.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\Servers.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\Servers.obj"	"$(INTDIR)\Servers.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\SmallToolBar.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\SmallToolBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\SmallToolBar.obj"	"$(INTDIR)\SmallToolBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\SmallWindow.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\SmallWindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\SmallWindow.obj"	"$(INTDIR)\SmallWindow.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\SNConfigDialog.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\SNConfigDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\SNConfigDialog.obj"	"$(INTDIR)\SNConfigDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"

CPP_SWITCHES=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\Vision.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Vision.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"

CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Vision.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Vision.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\videosndialog.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\videosndialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\videosndialog.obj"	"$(INTDIR)\videosndialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\Vision.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\Vision.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\Vision.obj"	"$(INTDIR)\Vision.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\Vision.rc

"$(INTDIR)\Vision.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\VisionCoolBar.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\VisionCoolBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\VisionCoolBar.obj"	"$(INTDIR)\VisionCoolBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\VisionDoc.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\VisionDoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\VisionDoc.obj"	"$(INTDIR)\VisionDoc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\VisionView.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\VisionView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\VisionView.obj"	"$(INTDIR)\VisionView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 

SOURCE=.\webbrowser2.cpp

!IF  "$(CFG)" == "Vision - Win32 Release"


"$(INTDIR)\webbrowser2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ELSEIF  "$(CFG)" == "Vision - Win32 Debug"


"$(INTDIR)\webbrowser2.obj"	"$(INTDIR)\webbrowser2.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Vision.pch"


!ENDIF 


!ENDIF 

