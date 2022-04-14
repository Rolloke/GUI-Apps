#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
#                                                                           *
#****************************************************************************

!IF  "$(CFG)" == "Release"
!MESSAGE "Using Release"
CVXDFLAGS	= -Zdp -Gs -c -DIS_32 -Zl -O2 -Zp1
AFLAGS		= -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6
IFLAGS		= 
MFLAGS		= -DEBUG:NONE -PDB:NONE
VERSION		= "iST"

!ELSEIF  "$(CFG)" == "Debug"
!MESSAGE "Using Debug"
CVXDFLAGS	= -Gs -c -DIS_32 -O2 -Zl -Zp1 -Zi -DDEBLEVEL=1 -DDEBUG
AFLAGS		= -coff -DBLD_COFF -DIS_32 -W2 -Zi -c -Cx -DMASM6 -DDEBLEVEL=1 -DDEBUG
#IFLAGS		= /DEBUG /DEBUGTYPE:CV
#MFLAGS		= 
IFLAGS		= /DEBUG /DEBUGTYPE:CV
MFLAGS		= 

VERSION		= "iST"

!ELSE
!ERROR An invalid configuration is specified.
!ENDIF 

all: direxist bin\mico.vxd

direxist:
        if not exist bin\nul    md bin

bin\micovxd.obj: micovxd.c
        cl $(CVXDFLAGS) -Fo$@ %s

bin\micoasm.obj: micovxd.asm
        ml $(AFLAGS) -Fo$@ %s

bin\mico.vxd: bin\micovxd.obj bin\micoasm.obj \
                  micovxd.def mico.res
        echo >NUL @<<micovxd.crf
-MACHINE:i386 $(MFLAGS)
-DEF:micovxd.def -OUT:bin\mico.vxd -MAP:bin\mico.map
-VXD vxdwraps.clb bin\micovxd.obj bin\micoasm.obj
<<
        link $(IFLAGS) @micovxd.crf

        rc16 -r -d $(VERSION) mico.rc
        adrc2vxd bin\mico.vxd mico.res

		copy bin\mico.vxd c:\win95\system\mico.vxd        
		copy bin\mico.vxd c:\windows\system\mico.vxd        

        cd bin
        mapsym mico
        cd ..

clean:
        del bin\*.obj
        del bin\*.vxd
        del bin\*.exe
        del bin\*.exp
        del bin\*.map
        del bin\*.sym
        del bin\*.lib
        rd  bin
        del *.pdb

