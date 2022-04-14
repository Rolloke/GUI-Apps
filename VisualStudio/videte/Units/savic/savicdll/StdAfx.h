/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Stdafx.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/Stdafx.h $
// CHECKIN:		$Date: 24.07.02 14:56 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 24.07.02 13:18 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <sys\stat.h>
#include <mmsystem.h>
//#include <SystemTime.h>
#include <io.h> 
#include <math.h>
#include <afxmt.h>
#include <afxcmn.h>

#include <WK_Names.h>
#include "WK_Profile.h"
#include "WK_RuntimeError.h"
#include "WK_Util.h"
#include "WK_File.h"
#include <WKClasses\WK_PerfMon.h>
#include "CIPCStringDefs.h"

#include "SavicDefs.h"
#include "SavicReg.h"
#include "SavicTrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



