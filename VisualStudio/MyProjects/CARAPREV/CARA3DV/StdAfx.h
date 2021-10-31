// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__30FA9E7A_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_STDAFX_H__30FA9E7A_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		  // Selten verwendete Teile der Windows-Header nicht einbinden

#include <afxwin.h>           // MFC-Kern- und -Standardkomponenten
#include <afxext.h>           // MFC-Erweiterungen
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>		  // MFC-Unterstützung für gängige Windows-Steuerelemente

#include <GL/gl.h>            // Open-Gl Headerdateien
#include <GL/glu.h>
#include <cmath>
#include "CVector.h"
#include "CustErr.h"
#include "EtsAppWndMsg.h"

#include "..\Label.h"         // Labelklasse
#include "..\RectLabel.h"     // RechteckLabelklasse
#include "..\PictureLabel.h"  // BildLabelklasse
#include "..\TextLabel.h"     // TextLabelklasse
#include "..\LabelContainer.h"// LabelContainerklasse

#ifdef _DEBUG
   void glcLastError();
   void AssertAndReportInt(int);
   #define ASSERT_AND_REPORT_INT(B) AssertAndReportInt(B)
   #define ASSERT_GL_ERROR          glcLastError()
#else
   #define ASSERT_AND_REPORT_INT(B)  ((void)(0))
   #define ASSERT_GL_ERROR           ((void)(0))
#endif

#define ETSDEBUG_REPORT 1

//#define ETS_OLE_SERVER 1

// global mathematical constant defines
//#define M_PI         3.1415926535898
#define M_180_D_PI  57.2957795131
#define M_PI_D_180   0.0174532925199
#define M_INV_256    0.00390625
#define M_INV_128    0.0078125

#define ABS(X) (((X)>0) ? (X) : -(X))

#define SPACE_STRING " "
 
#define MIN_3D_COLORS               2
#define MAX_3D_COLORS               4096

#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_STDAFX_H__30FA9E7A_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
