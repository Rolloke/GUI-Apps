/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: Stdafx.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/Stdafx.h $
// CHECKIN:		$Date: 24.07.02 14:57 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 24.07.02 13:48 $
// BY AUTHOR:	$Author: Martin.lueck $
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

#include "cipc.h"
#include "WK.h"
#include "WK_Trace.h"
#include "WK_Names.h"
#include "WK_File.h"
#include "WK_Util.h"
#include "WK_RuntimeError.h"
#include "WKClasses\WK_PerfMon.h"
#include "WK_Profile.h"
#include "WK_Dongle.h"
#include "WK_Timer.h"
#include "WK_Msg.h"

#include <afxcmn.h>

// User/Permission
#include "User.h"
#include "Permission.h"
#include "oemgui\oemlogindialog.h"

// SaVicLib
#include "..\SavicDll\SavicLib.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



