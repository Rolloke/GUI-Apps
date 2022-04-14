# Microsoft Visual C++ generated build script - Do not modify

PROJ = MICOTST
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\PROJECT\TOOLS\TSTTOOLS\MICOTST\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = MICOTST.CPP 
RC = rc
CFLAGS_D_DEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /D "_DOS" /FR /Fd"MICOTST.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G2 /W3 /AL /Od /D "NDEBUG" /D "_DOS" /FR 
LFLAGS_D_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /CO /STACK:5120
LFLAGS_R_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /STACK:5120
LIBS_D_DEXE = oldnames llibce
LIBS_R_DEXE = oldnames llibce
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = CALARM.SBR \
		CBT829.SBR \
		CBT856.SBR \
		CI2C.SBR \
		CIO.SBR \
		CRELAY.SBR \
		CWDOG.SBR \
		CZORAN.SBR \
		MICOTST.SBR \
		ZORANIO.SBR \
		ZR050.SBR


CALARM_DEP = d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\calarm.h


CBT829_DEP = d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\cbt829.h \
	d:\project\tools\tsttools\micotst\ci2c.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\micoini.h


CBT856_DEP = d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\cbt856.h \
	d:\project\tools\tsttools\micotst\ci2c.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\micoini.h


CI2C_DEP = d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\ci2c.h


CIO_DEP = d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\micoreg.h


CRELAY_DEP = d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\crelay.h


CWDOG_DEP = d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\cwdog.h \
	d:\project\tools\tsttools\micotst\cio.h


CZORAN_DEP = d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\czoran.h \
	d:\project\tools\tsttools\micotst\zr050.h \
	d:\project\tools\tsttools\micotst\zr050_io.h \
	d:\project\tools\tsttools\micotst\zoranio.h \
	d:\project\tools\tsttools\micotst\h55_tbls.h


MICOTST_DEP = d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\micoreg.h \
	d:\project\tools\tsttools\micotst\cbt829.h \
	d:\project\tools\tsttools\micotst\ci2c.h \
	d:\project\tools\tsttools\micotst\cio.h \
	d:\project\tools\tsttools\micotst\cbt856.h \
	d:\project\tools\tsttools\micotst\crelay.h \
	d:\project\tools\tsttools\micotst\calarm.h \
	d:\project\tools\tsttools\micotst\micodefs.h \
	d:\project\tools\tsttools\micotst\cwdog.h \
	d:\project\tools\tsttools\micotst\czoran.h \
	d:\project\tools\tsttools\micotst\zr050.h \
	d:\project\tools\tsttools\micotst\zr050_io.h \
	d:\project\tools\tsttools\micotst\zoranio.h


ZORANIO_DEP = d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\zoranio.h \
	d:\project\tools\tsttools\micotst\zr050_io.h


ZR050_DEP = d:\project\tools\tsttools\micotst\stdafx.h \
	d:\project\tools\tsttools\micotst\micotst.h \
	d:\project\tools\tsttools\micotst\zr050_io.h \
	d:\project\tools\tsttools\micotst\zr050.h


all:	$(PROJ).EXE $(PROJ).BSC

CALARM.OBJ:	CALARM.CPP $(CALARM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CALARM.CPP

CBT829.OBJ:	CBT829.CPP $(CBT829_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CBT829.CPP

CBT856.OBJ:	CBT856.CPP $(CBT856_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CBT856.CPP

CI2C.OBJ:	CI2C.CPP $(CI2C_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CI2C.CPP

CIO.OBJ:	CIO.CPP $(CIO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CIO.CPP

CRELAY.OBJ:	CRELAY.CPP $(CRELAY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CRELAY.CPP

CWDOG.OBJ:	CWDOG.CPP $(CWDOG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CWDOG.CPP

CZORAN.OBJ:	CZORAN.CPP $(CZORAN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CZORAN.CPP

MICOTST.OBJ:	MICOTST.CPP $(MICOTST_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c MICOTST.CPP

ZORANIO.OBJ:	ZORANIO.CPP $(ZORANIO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ZORANIO.CPP

ZR050.OBJ:	ZR050.CPP $(ZR050_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ZR050.CPP

$(PROJ).EXE::	CALARM.OBJ CBT829.OBJ CBT856.OBJ CI2C.OBJ CIO.OBJ CRELAY.OBJ CWDOG.OBJ \
	CZORAN.OBJ MICOTST.OBJ ZORANIO.OBJ ZR050.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
CALARM.OBJ +
CBT829.OBJ +
CBT856.OBJ +
CI2C.OBJ +
CIO.OBJ +
CRELAY.OBJ +
CWDOG.OBJ +
CZORAN.OBJ +
MICOTST.OBJ +
ZORANIO.OBJ +
ZR050.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
