#if !defined(AFX_STDAFX_H__43378A07_0CD5_45BB_8BA0_8033F854BD3A__INCLUDED_)
#define AFX_STDAFX_H__43378A07_0CD5_45BB_8BA0_8033F854BD3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
//#include <afxdb.h>			// MFC database classes
//#include <afxdao.h>			// MFC DAO database classes


//////////////////////////////////////////////////////////////////////////
//include all necessary headers
#include <winsock2.h>
#include <cipcoutputclient.h>
#include <vimage/vimage.h>
#include <wkclasses\wk_trace.h>



//////////////////////////////////////////////////////////////////////////
//TODO folgende Header sollen noch "rausfliegen", da sie nicht unbedingt benötigt werden
#include <wkclasses/wk_util.h>
#include <wk_names.h> //wird noch benötigt für das Faken von WK_ALONE(WK_APP_NAME_SOCKETS);
#include <wk_dongle.h>
#include <IPCFetch.h> //fetch soll in die NFC ausgelagert werden (Uwe weiß bescheid)


#include <nfc/IPCServerControlInterface.h>
#include <afxwin.h>

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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__43378A07_0CD5_45BB_8BA0_8033F854BD3A__INCLUDED_)
