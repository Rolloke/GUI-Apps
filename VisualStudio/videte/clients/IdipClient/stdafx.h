// stdafx.h : Includedatei für Standardsystem-Includedateien,
// oder häufig verwendete, projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#endif

// Ändern Sie folgende Definitionen für Plattformen, die älter als die unten angegebenen sind.
// Unter MSDN finden Sie die neuesten Informationen über die entsprechenden Werte für die unterschiedlichen Plattformen.
/*
#ifndef WINVER				// Lassen Sie die Verwendung von Features spezifisch für Windows 95 und Windows NT 4 oder später zu.
#define WINVER 0x0400		// Ändern Sie den entsprechenden Wert, um auf Windows 98 und mindestens Windows 2000 abzuzielen.
#endif

#ifndef _WIN32_WINNT		// Lassen Sie die Verwendung von Features spezifisch für Windows NT 4 oder später zu.
#define _WIN32_WINNT 0x0400		// Ändern Sie den entsprechenden Wert, um auf Windows 98 und mindestens Windows 2000 abzuzielen.
#endif						

#ifndef _WIN32_WINDOWS		// Lassen Sie die Verwendung von Features spezifisch für Windows 98 oder später zu.
#define _WIN32_WINDOWS 0x0410 // Ändern Sie den entsprechenden Wert, um auf mindestens Windows Me abzuzielen.
#endif

#ifndef _WIN32_IE			// Lassen Sie die Verwendung von Features spezifisch für IE 4.0 oder später zu.
#define _WIN32_IE 0x0400	// Ändern Sie den entsprechenden Wert, um auf mindestens IE 5.0 abzuzielen.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// einige CString-Konstruktoren sind explizit
*/
// Deaktiviert das Ausblenden von einigen häufigen und oft ignorierten Warnungen
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC-Automatisierungsklassen

// #include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcview.h>
#include <afxmt.h>
#include <afxpriv.h>
#if _MFC_VER == 0x0600
	#include <..\src\afximpl.h>
#else if _MFC_VER >= 0x0700
	#include <afximpl.h>
	#include <afxocc.h>
	#include <ocdb.h>
#endif

#ifdef _WK_CLASSES_DLL
 #define WK_AFX_EXT_CLASS AFX_EXT_CLASS
#else
 #define WK_AFX_EXT_CLASS
#endif

// Misc
#include <math.h>
#include <dbt.h>

// CIPC
#include <CIPCStringDefs.h>
#include <IPCFetch.h>
#include <CIPCField.h>
#include <CipcServerControl.h>
#include <CIPCDatabaseClient.h>
#include <CIPCInputClient.h>
#include <CIPCOutputClient.h>
#include <IPCDataCarrier.h>
#include <PictureRecord.h>
#include <CipcOutputRecord.h>
#include <CIPCMediaRecord.h>
#include <wk.h>
#include <WK_Names.h>
#include <WK_DebugOptions.h>
#include <wk_dongle.h>
#include <wk_timer.h>
#include <ErrorMessage.h>
#include <SecID.h>
#include <User.h>		// User/Permission
#include <Permission.h>
#include <Host.h>
#include <AutoLogout.h>
#include <NotificationMessage.h>

// include
#include <wk_msg.h>

// Skins
#include <Skins\Skins.h>		// include before oemgui
#include <Skins\SkinDialog.h>

// OEMGUI
#include <oemgui\oemguiapi.h>
#include <oemgui\oemconnectiondialog.h>
#include <oemgui\oemlogindialog.h>
#include <oemgui\OEMCoolBar.h>
#include <oemgui\oemdatetimedialog.h>
#include <oemgui\OemFileDialog.h>
#include <oemgui\oemgui.h>
#include <oemgui\DlgLoginNew.h>

// VImage
#include <vimage/vimage.h>
#include <vimage/CAVCodec.h>
#include <vimage/DibSection.h>

// WK Classes
#include <wkclasses\WK_RunTimeError.h>
#include <wkclasses\MonitorInfo.h>
#include <wkclasses\wk_wincpp.h>
#include <wkclasses\wk_string.h>
#include <wkclasses\wk_thread.h>
#include <wkclasses\wk_dialog.h>
#include <wkclasses\wk_perfmon.h>
#include <wkclasses\explorermenu.h>
#include <wkclasses\joystickdx.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\wk_util.h>
#include <wkclasses\wk_trace.h>
#include <wkclasses\wk_wincpp.h>


#define SECTION_IDIP_CLIENT		_T("IdipClient")
// Defined in wk_profile.h #define IDIP_CLIENT_SETTINGS	SECTION_IDIP_CLIENT _T("\\Settings")
#define IDIP_CLIENT_VISIBILITY	IDIP_CLIENT_SETTINGS _T("\\Visibility")
#define	USE_MONITOR				_T("UseMonitor")

// special WM_SIZE Parameter
#define SIZE_UPDATE 5

#define WPARAM_BASE					0x5000
#define GET_MONITOR_RECT			0x5001
#define REJECT_ANNOUNCEMENT			0x5002
#define NO_MEDIA_RECEIVE_CONFIRM	0x5003
#define ALARM_CONNECTION			0x5004
#define REMOVE_SEQUENCE				0x5005
#define INIT_FIELDS					0x5006
#define WPARAM_PANE_INT				0x5007
#define WPARAM_PANE_TXT				0x5008
#define WPARAM_CLOSE_CONNECT_DLG	0x5009
#define WPARAM_CHECK_PERF_LEVEL		0x500a
#define WPARAM_UNREQUEST_PICTURES	0x500b


/////////////////////////////////////////////////////////////////////////////
// Messages
#define WM_USER_UPDATE_AND_ENABLE_CONTROLS		(WM_USER + 0x0002)
#define WM_USER_HANDLE_CD_AND_DVD_INFO			(WM_USER + 0x0003)
#define WM_USER_BACKUP_DATA_CARRIER_FINISHED	(WM_USER + 0x0004)
#define WM_NOTIFY_WINDOW						(WM_USER + 0x0005)
#define WM_NOTIFY_WINDOWO						(WM_USER + 0x0006)
#define WM_NOTIFY_WINDOWI						(WM_USER + 0x0007)
#define WM_UPDATE_ALL_VIEWS						(WM_USER + 0x0008)
#define WM_CHANGE_VIDEO							(WM_USER + 0x0009)
#define WM_DECODE_VIDEO							(WM_USER + 0x000a)
#define WM_CHANGE_AUDIO							(WM_USER + 0x000b)
#define WM_REDRAW_VIDEO							(WM_USER + 0x000c)
#define WM_UPDATE_DIALOGS						(WM_USER + 0x000d)
#define WM_DRIVE_INFO							(WM_USER + 0x000e)
#define WM_PICTUREDATA							(WM_USER + 0x000f)

// TODO! RKE: Search for _DTS, it is obsolete
#ifdef _DTS
	#undef _HTML_ELEMENTS
#else
	#define _HTML_ELEMENTS
#endif

#ifndef IDC_HAND
  #define IDC_HAND            MAKEINTRESOURCE(32649)
#endif

enum eVisibilityStates
{
	OTVS_Undefined		= -1,
	OTVS_Always			= 1,
	OTVS_IfNotEmpty		= 2,
	OTVS_Never			= 3,
	OTVS_InitiallyYes	= 4,
	OTVS_InitiallyNo	= 5,
};

void DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars, int nID);

int  GetPosition(CDWordArray& array, DWORD dwValue);
void AddSorted(CDWordArray& array, DWORD dwValue);
int  CompareStringRelative(const CString &s1, const CString &s2);

class CStateCmdUI: public CCmdUI
{
public: // re-implementations only
	CStateCmdUI();
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetRadio( BOOL bOn = TRUE );
	BOOL m_bEnabled;
	int  m_nChecked;
};
