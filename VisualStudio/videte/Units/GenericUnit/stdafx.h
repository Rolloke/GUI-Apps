/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: stdafx.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/stdafx.h $
// CHECKIN:		$Date: 20.01.06 12:40 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 20.01.06 12:33 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxmt.h>         // MFC core and standard components
#include <afxtempl.h>
#include <sys\stat.h>
#include <mmsystem.h>
#include <SystemTime.h>
#include <io.h> 
#include <afxcmn.h>


// cipc
#include <WK_Names.h>
#include <WK_Dongle.h>
#include <CipcStringDefs.h>

// wkclasses
#include <wkclasses\WK_Util.h>
#include <wkclasses\WK_RuntimeError.h>
#include <wkclasses\WK_Profile.h>
#include <wkclasses\WK_Trace.h>
#include <wkclasses\WK_File.h>

#include <JpegLib\CJpeg.h>

#include "GenericDefs.h"
#include "CCodec.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



