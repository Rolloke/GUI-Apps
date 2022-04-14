// stdafx.h : Includedatei für Standardsystem-Includedateien,
// oder häufig verwendete, projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#endif


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// einige CString-Konstruktoren sind explizit

// Deaktiviert das Ausblenden von einigen häufigen und oft ignorierten Warnungen
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC-Automatisierungsklassen

#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC-Socket-Erweiterungen
#include <math.h>

#include <afxinet.h>		// InternetSession

#include <rtccore.h>		// Real time Transport Protocoll

#include <Upnp.h>			// universal plug and play


// videte header
#include <wk.h>
#include <WK_Msg.h>
#include <WK_names.h>

#include <wkclasses\WK_WinThread.h>
#include <wkclasses\wk_util.h>
#include <wkclasses\unhandledexception.h>
#include <wkclasses\WK_RunTimeError.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\statistic.h>

#include <cipc.h>
#include <PictureRecord.h>
#include <CIPCStringDefs.h>
#include <CameraControl.h>
#include <wk_timer.h>

// #include <Skins/Skins.h> // Skins before OemGui!

#include <oemgui\oemguiapi.h>
// #include <oemgui\oemLoginDialog.h>

#include <devicedetect\devicedetect.h>
#include <vimage\CJpeg.h>


// Direct Show header
#pragma warning( push )
#pragma warning( disable : 4312 )
#include <strmif.h>
#include <uuids.h>
#include <dshow.h>

#include <atlbase.h>
#include "Streams.h"
#include "mfcutil.h"
#include "dshowutil.h"
#include "dmoreg.h"
#include "namedguid.h"
#include "keyprovider.h"
#include "Objbase.h"

#include <KS.h>
#include <KSMEDIA.h>

#include "qedit.h"
#include <combase.h>

void RemoveLastCharacter(CString&s, TCHAR c);

struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, const char*sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	const char* m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)
#define HRESULT_EXCEPTION(hr) 	if (FAILED(hr)) throw HRESULT_ERROR_AT_POS(hr);

#define WM_HRESULT_ERROR		(WM_USER + 1)
#define WM_INET_STATUS			(WM_USER + 2)
#define WM_THREAD_STARTED		(WM_USER + 3)
#define WM_SET_CAMERA_ACTIVE	(WM_USER + 4)
#define WM_POST_HTTP_RESULT		(WM_USER + 5)

#pragma warning( pop )

#define MAX_CAMERAS 32

#define CSD_CAM_AXIS              _T("AXIS")		// Axis video server
#define CSD_CAM_PANASONIC         _T("Panasonic")	// panasonic network cameras
// TODO! RKE: define further types of supported devices here

// TODO! RKE: remove test keys
#ifdef _DEBUG
//#define _TEST_TASHA
#define _TEST_SV_DLG
#endif

#define CAM_CTRL_PAN	 _T("Pan")	
#define CAM_CTRL_TILT	 _T("Tilt")
#define CAM_CTRL_ZOOM	 _T("Zoom")
#define CAM_CTRL_FOCUS	 _T("Focus")
#define CAM_CTRL_ROLL	 _T("Roll")	

// Wertebereich der Helligkeit
#define MIN_BRIGHTNESS				0
#define MAX_BRIGHTNESS				255
// Wertebereich des Kontrastes
#define MIN_CONTRAST				0
#define MAX_CONTRAST				511
// Wertebereich des Farbsättigung
#define MIN_SATURATION				0
#define MAX_SATURATION				511

#define MAX_IP_CAMERAS				32

// set to camera defs
#define CCT_DIRECT_SHOW 23
