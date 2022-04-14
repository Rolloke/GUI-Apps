# Microsoft Developer Studio Generated NMAKE File, Based on LogZipLib.dsp
!IF "$(CFG)" == ""
CFG=LogZipLib - Win32 Release
!MESSAGE No configuration specified. Defaulting to LogZipLib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "LogZipLib - Win32 Release" && "$(CFG)" !=\
 "LogZipLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LogZipLib.mak" CFG="LogZipLib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LogZipLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LogZipLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\LogZipLib.lib" "$(OUTDIR)\LogZipLib.bsc"

!ELSE 

ALL : "$(OUTDIR)\LogZipLib.lib" "$(OUTDIR)\LogZipLib.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\adler32.sbr"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\compress.sbr"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crc32.sbr"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\deflate.sbr"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\gzio.sbr"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infblock.sbr"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\infcodes.sbr"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inffast.sbr"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inflate.sbr"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\inftrees.sbr"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\infutil.sbr"
	-@erase "$(INTDIR)\LogZip.obj"
	-@erase "$(INTDIR)\LogZip.sbr"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\trees.sbr"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\uncompr.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(INTDIR)\zutil.sbr"
	-@erase "$(OUTDIR)\LogZipLib.bsc"
	-@erase "$(OUTDIR)\LogZipLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\LogZipLib.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/

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

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogZipLib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adler32.sbr" \
	"$(INTDIR)\compress.sbr" \
	"$(INTDIR)\crc32.sbr" \
	"$(INTDIR)\deflate.sbr" \
	"$(INTDIR)\gzio.sbr" \
	"$(INTDIR)\infblock.sbr" \
	"$(INTDIR)\infcodes.sbr" \
	"$(INTDIR)\inffast.sbr" \
	"$(INTDIR)\inflate.sbr" \
	"$(INTDIR)\inftrees.sbr" \
	"$(INTDIR)\infutil.sbr" \
	"$(INTDIR)\LogZip.sbr" \
	"$(INTDIR)\trees.sbr" \
	"$(INTDIR)\uncompr.sbr" \
	"$(INTDIR)\zutil.sbr"

"$(OUTDIR)\LogZipLib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LogZipLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\LogZip.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\LogZipLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\LogZipLibDebug.lib" "$(OUTDIR)\LogZipLib.bsc"

!ELSE 

ALL : "$(OUTDIR)\LogZipLibDebug.lib" "$(OUTDIR)\LogZipLib.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\adler32.sbr"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\compress.sbr"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crc32.sbr"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\deflate.sbr"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\gzio.sbr"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infblock.sbr"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\infcodes.sbr"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inffast.sbr"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inflate.sbr"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\inftrees.sbr"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\infutil.sbr"
	-@erase "$(INTDIR)\LogZip.obj"
	-@erase "$(INTDIR)\LogZip.sbr"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\trees.sbr"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\uncompr.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(INTDIR)\zutil.sbr"
	-@erase "$(OUTDIR)\LogZipLib.bsc"
	-@erase "$(OUTDIR)\LogZipLibDebug.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\LogZipLib.pch" /YX\
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

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogZipLib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adler32.sbr" \
	"$(INTDIR)\compress.sbr" \
	"$(INTDIR)\crc32.sbr" \
	"$(INTDIR)\deflate.sbr" \
	"$(INTDIR)\gzio.sbr" \
	"$(INTDIR)\infblock.sbr" \
	"$(INTDIR)\infcodes.sbr" \
	"$(INTDIR)\inffast.sbr" \
	"$(INTDIR)\inflate.sbr" \
	"$(INTDIR)\inftrees.sbr" \
	"$(INTDIR)\infutil.sbr" \
	"$(INTDIR)\LogZip.sbr" \
	"$(INTDIR)\trees.sbr" \
	"$(INTDIR)\uncompr.sbr" \
	"$(INTDIR)\zutil.sbr"

"$(OUTDIR)\LogZipLib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LogZipLibDebug.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\LogZip.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\LogZipLibDebug.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "LogZipLib - Win32 Release" || "$(CFG)" ==\
 "LogZipLib - Win32 Debug"
SOURCE=.\adler32.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_ADLER=\
	".\zconf.h"\
	".\zlib.h"\
	

"$(INTDIR)\adler32.obj"	"$(INTDIR)\adler32.sbr" : $(SOURCE) $(DEP_CPP_ADLER)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_ADLER=\
	".\zconf.h"\
	".\zlib.h"\
	

"$(INTDIR)\adler32.obj"	"$(INTDIR)\adler32.sbr" : $(SOURCE) $(DEP_CPP_ADLER)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\compress.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_COMPR=\
	".\zconf.h"\
	".\zlib.h"\
	

"$(INTDIR)\compress.obj"	"$(INTDIR)\compress.sbr" : $(SOURCE) $(DEP_CPP_COMPR)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_COMPR=\
	".\zconf.h"\
	".\zlib.h"\
	

"$(INTDIR)\compress.obj"	"$(INTDIR)\compress.sbr" : $(SOURCE) $(DEP_CPP_COMPR)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\crc32.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_CRC32=\
	".\zconf.h"\
	".\zlib.h"\
	

"$(INTDIR)\crc32.obj"	"$(INTDIR)\crc32.sbr" : $(SOURCE) $(DEP_CPP_CRC32)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_CRC32=\
	".\zconf.h"\
	".\zlib.h"\
	

"$(INTDIR)\crc32.obj"	"$(INTDIR)\crc32.sbr" : $(SOURCE) $(DEP_CPP_CRC32)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\deflate.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_DEFLA=\
	".\deflate.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\deflate.obj"	"$(INTDIR)\deflate.sbr" : $(SOURCE) $(DEP_CPP_DEFLA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_DEFLA=\
	".\deflate.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\deflate.obj"	"$(INTDIR)\deflate.sbr" : $(SOURCE) $(DEP_CPP_DEFLA)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\gzio.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_GZIO_=\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\gzio.obj"	"$(INTDIR)\gzio.sbr" : $(SOURCE) $(DEP_CPP_GZIO_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_GZIO_=\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\gzio.obj"	"$(INTDIR)\gzio.sbr" : $(SOURCE) $(DEP_CPP_GZIO_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\infblock.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_INFBL=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\infblock.obj"	"$(INTDIR)\infblock.sbr" : $(SOURCE) $(DEP_CPP_INFBL)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_INFBL=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\infblock.obj"	"$(INTDIR)\infblock.sbr" : $(SOURCE) $(DEP_CPP_INFBL)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\infcodes.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_INFCO=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inffast.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\infcodes.obj"	"$(INTDIR)\infcodes.sbr" : $(SOURCE) $(DEP_CPP_INFCO)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_INFCO=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inffast.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\infcodes.obj"	"$(INTDIR)\infcodes.sbr" : $(SOURCE) $(DEP_CPP_INFCO)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\inffast.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_INFFA=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inffast.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\inffast.obj"	"$(INTDIR)\inffast.sbr" : $(SOURCE) $(DEP_CPP_INFFA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_INFFA=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inffast.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\inffast.obj"	"$(INTDIR)\inffast.sbr" : $(SOURCE) $(DEP_CPP_INFFA)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\inflate.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_INFLA=\
	".\infblock.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\inflate.obj"	"$(INTDIR)\inflate.sbr" : $(SOURCE) $(DEP_CPP_INFLA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_INFLA=\
	".\infblock.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\inflate.obj"	"$(INTDIR)\inflate.sbr" : $(SOURCE) $(DEP_CPP_INFLA)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\inftrees.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_INFTR=\
	".\inftrees.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\inftrees.obj"	"$(INTDIR)\inftrees.sbr" : $(SOURCE) $(DEP_CPP_INFTR)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_INFTR=\
	".\inftrees.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\inftrees.obj"	"$(INTDIR)\inftrees.sbr" : $(SOURCE) $(DEP_CPP_INFTR)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\infutil.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_INFUT=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\infutil.obj"	"$(INTDIR)\infutil.sbr" : $(SOURCE) $(DEP_CPP_INFUT)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_INFUT=\
	".\infblock.h"\
	".\infcodes.h"\
	".\inftrees.h"\
	".\infutil.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\infutil.obj"	"$(INTDIR)\infutil.sbr" : $(SOURCE) $(DEP_CPP_INFUT)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\LogZip.cpp

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_LOGZI=\
	".\zconf.h"\
	".\zlib.h"\
	{$(INCLUDE)}"logzip.h"\
	

"$(INTDIR)\LogZip.obj"	"$(INTDIR)\LogZip.sbr" : $(SOURCE) $(DEP_CPP_LOGZI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_LOGZI=\
	".\zconf.h"\
	".\zlib.h"\
	{$(INCLUDE)}"logzip.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\LogZip.obj"	"$(INTDIR)\LogZip.sbr" : $(SOURCE) $(DEP_CPP_LOGZI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\trees.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_TREES=\
	".\deflate.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\trees.obj"	"$(INTDIR)\trees.sbr" : $(SOURCE) $(DEP_CPP_TREES)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_TREES=\
	".\deflate.h"\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\trees.obj"	"$(INTDIR)\trees.sbr" : $(SOURCE) $(DEP_CPP_TREES)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\uncompr.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_UNCOM=\
	".\zconf.h"\
	".\zlib.h"\
	

"$(INTDIR)\uncompr.obj"	"$(INTDIR)\uncompr.sbr" : $(SOURCE) $(DEP_CPP_UNCOM)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_UNCOM=\
	".\zconf.h"\
	".\zlib.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\uncompr.obj"	"$(INTDIR)\uncompr.sbr" : $(SOURCE) $(DEP_CPP_UNCOM)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\zutil.c

!IF  "$(CFG)" == "LogZipLib - Win32 Release"

DEP_CPP_ZUTIL=\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	

"$(INTDIR)\zutil.obj"	"$(INTDIR)\zutil.sbr" : $(SOURCE) $(DEP_CPP_ZUTIL)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "LogZipLib - Win32 Debug"

DEP_CPP_ZUTIL=\
	".\zconf.h"\
	".\zlib.h"\
	".\zutil.h"\
	{$(INCLUDE)}"unix.h"\
	

"$(INTDIR)\zutil.obj"	"$(INTDIR)\zutil.sbr" : $(SOURCE) $(DEP_CPP_ZUTIL)\
 "$(INTDIR)"


!ENDIF 


!ENDIF 

