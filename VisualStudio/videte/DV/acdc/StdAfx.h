// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F1C743C0_7061_4C24_B3E0_664CAB8DBF3F__INCLUDED_)
#define AFX_STDAFX_H__F1C743C0_7061_4C24_B3E0_664CAB8DBF3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning (disable:4505)

// Here we import the implementation of NeroWaitCDTexts().

#define NERO_WAITCD_TEXTS

#pragma pack(push, 8)
#include "NeroUserDialog.h"
#pragma pack(pop)

#undef NERO_WAITCD_TEXTS

#pragma pack(push, 8)
#include "NeroAPIGlue.h"
#pragma pack(pop)

#include <stdio.h>
#include <crtdbg.h>

// vector.h produces multi C4100 and C4018 warnings at level 4
#pragma warning( push, 3 )
#include <vector>
#pragma warning( pop ) 

#include <conio.h>
#include <sys\stat.h>
#include <io.h>
#include "WK_Msg.h"


#include <afxmt.h>

#include <wk.h>
#include <WK_Names.h>

#include <wkclasses/wk_thread.h>
#include <wkclasses/wk_wincpp.h>
#include <wkclasses/wk_string.h>
#include "wkclasses/wk_trace.h"
#include <wkclasses/WK_File.h>
#include "wkclasses/WK_RuntimeError.h"
#include <wkclasses/wk_profile.h>
#include "wkclasses/UnhandledException.h"
#include "wkclasses/wk_util.h"

#include <IPCDataCarrier.h>
#include <cipc.h>
#include <cipcservercontrol.h>
#include <oemgui\oemguiapi.h>

#include "AcdcEnums.h"

#include <devicedetect\devicedetect.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F1C743C0_7061_4C24_B3E0_664CAB8DBF3F__INCLUDED_)
