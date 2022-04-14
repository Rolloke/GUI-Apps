/******************************************************************************
|* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
|* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
|* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
|* PARTICULAR PURPOSE.
|* 
|* Copyright 2005 videte IT ® AG. All Rights Reserved.
|*-----------------------------------------------------------------------------
|* videteSDK / Sample
|*
|* PROGRAM: stdafx.h
|*
|* PURPOSE: Include file for standard system include files,
|*          or project specific include files that are used frequently, but
|*          are changed infrequently.
******************************************************************************/

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls

//////////////////////////////////////////////////////////////////////////
//include all necessary headers
#include <winsock2.h>
#include <cipcoutputclient.h>
#include <vimage/vimage.h>
#include <wkclasses\wk_trace.h>




//////////////////////////////////////////////////////////////////////////
//TODO folgende Header sollen noch "rausfliegen", da sie nicht unbedingt benötigt werden
#include <wk_dongle.h>
#include <wk_names.h> //wird noch benötigt für das Faken von WK_ALONE(WK_APP_NAME_SOCKETS);
#include <IPCFetch.h> //fetch soll in die NFC ausgelagert werden (Uwe weiß bescheid)

//////////////////////////////////////////////////////////////////////////

#include <nfc/IPCServerControlInterface.h>

/////////////////////////////////////////////////////////////////////////////
// Messages
/////////////////////////////////////////////////////////////////////////////
#define WM_REQUEST_PICTURES		(WM_USER + 0x0001)
#define WM_DECODE_VIDEO			(WM_USER + 0x0002)

/////////////////////////////////////////////////////////////////////////////
// delete macros, that check for NULL and reset to NULL
/////////////////////////////////////////////////////////////////////////////
#ifndef DELETE_PTR
#define DELETE_PTR(ptr)		{if(ptr){delete ptr; ptr=NULL;}}
#endif 

#ifndef DELETE_PTR_ARRAY
#define DELETE_PTR_ARRAY(ptr)	{if(ptr){delete [] ptr; ptr=NULL;}}
#endif 

#endif // _AFX_NO_AFXCMN_SUPPORT

