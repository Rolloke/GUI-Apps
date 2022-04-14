# Microsoft Developer Studio Project File - Name="capi4_32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=capi4_32 - Win32 DebugUnicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "capi4_32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "capi4_32.mak" CFG="capi4_32 - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "capi4_32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "capi4_32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "capi4_32 - Win32 DebugUnicode" (based on "Win32 (x86) Static Library")
!MESSAGE "capi4_32 - Win32 ReleaseUnicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/Capi/capi4", BFEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# ADD BASE CPP /nologo /G3 /Zp1 /MT /W3 /GX /Od /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_CAPI20_" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /Od /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_CAPI20_" /D "_MBCS" /D "_AFXDLL" /Fr /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# ADD BASE CPP /nologo /G3 /Zp1 /MT /W3 /GX /Z7 /Od /Gf /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_CAPI20_" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_CAPI20_" /D "_MBCS" /D "_AFXDLL" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Debug\Capi4_32_Debug.lib"

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 DebugUnicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "capi4_32___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "capi4_32___Win32_DebugUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_CAPI20_" /D "_AFXDLL" /D "_MBCS" /Fr /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp1 /MDd /W4 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_CAPI20_" /D "_MBCS" /D "_UNICODE" /D "_AFXDLL" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Debug\Capi4_32_Debug.lib"
# ADD LIB32 /nologo /out:".\DebugUnicode\Capi4_32_DebugU.lib"

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "capi4_32___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "capi4_32___Win32_ReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MD /W4 /GX /Od /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_CAPI20_" /D "_AFXDLL" /D "_MBCS" /Fr /YX /FD /c
# ADD CPP /nologo /Zp1 /MD /W4 /GX /Od /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_CAPI20_" /D "_MBCS" /D "_UNICODE" /D "_AFXDLL" /Fr /YX /FD /c
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ReleaseUnicode\capi4_32U.lib"

!ENDIF 

# Begin Target

# Name "capi4_32 - Win32 Release"
# Name "capi4_32 - Win32 Debug"
# Name "capi4_32 - Win32 DebugUnicode"
# Name "capi4_32 - Win32 ReleaseUnicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\20CONN.CPP
# End Source File
# Begin Source File

SOURCE=.\20MAIN.CPP
# End Source File
# Begin Source File

SOURCE=.\20MSG.CPP
# End Source File
# Begin Source File

SOURCE=.\CAPI4.CPP
# End Source File
# Begin Source File

SOURCE=.\COMMON.CPP
# End Source File
# Begin Source File

SOURCE=.\DATAQ.CPP
# End Source File
# Begin Source File

SOURCE=.\DecodeCapiMessages.cpp
# End Source File
# Begin Source File

SOURCE=.\limitedstring.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\20capi.h
# End Source File
# Begin Source File

SOURCE=.\20conn.h
# End Source File
# Begin Source File

SOURCE=.\20def.h
# End Source File
# Begin Source File

SOURCE=.\20info.h
# End Source File
# Begin Source File

SOURCE=.\20main.h
# End Source File
# Begin Source File

SOURCE=.\20msg.h
# End Source File
# Begin Source File

SOURCE=.\dataq.h
# End Source File
# Begin Source File

SOURCE=.\DecodeCapiMessages.h
# End Source File
# Begin Source File

SOURCE=.\LimitedString.h
# End Source File
# Begin Source File

SOURCE=.\LocalTrace.h
# End Source File
# Begin Source File

SOURCE=.\null.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Release\Capi4_32.lib

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Debug\Capi4_32_Debug.lib

!IF  "$(CFG)" == "capi4_32 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 DebugUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "capi4_32 - Win32 ReleaseUnicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DebugUnicode\Capi4_32_DebugU.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ReleaseUnicode\capi4_32U.lib
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\History.txt
# End Source File
# End Target
# End Project
