// WndHTML.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"

#include "MainFrm.h"

#include "WndHtml.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewCamera.h"
#include "ViewAlarmList.h"
#include "ViewAudio.h"
#include "ViewRelais.h"
#include "DlgHTMLmap.h"
#include "DlgHTMLlink.h"

#include <mshtml.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
#define HTML_IMG_DETECTOR			0
#define HTML_IMG_DETECTOR_ALARM0	1
#define HTML_IMG_DETECTOR_ALARM1    2
#define HTML_IMG_DETECTOR_DISABLED	3
#define HTML_IMG_CAM				4
#define HTML_IMG_CAM_PTZ			5
#define HTML_IMG_CAM_RGB			6
#define HTML_IMG_CAM_REF			7
#define HTML_IMG_ACTIVE				8
#define HTML_IMG_RELAY_CLOSED		9
#define HTML_IMG_RELAY_OPEN			10
#define HTML_IMG_HOST				11
#define HTML_IMG_HOST_CONNECTED		12
#define HTML_IMG_DISABLED			13
#define HTML_IMG_SELECTED			14
#define HTML_IMG_NOT_CONNECTED		15
#define HTML_IMG_MARKED				16
#define HTML_IMG_MICROPHONE			17
#define HTML_IMG_LOUDSPEAKER		18
#define HTML_IMG_SUB_LINK			19
#define HTML_IMG_SUB_LINK_ALARM0	20
#define HTML_IMG_SUB_LINK_ALARM1	21

#define HTML_IMG_SIZE_X				27
#define HTML_IMG_SIZE_Y				27

#define HTML_SHOW_STATES			_T("HTMLShowStates")
#define HTML_LINK					_T("href=")
#define HTML_COORDS					_T("coords=")
#define HTML_ALT					_T("alt=")
#define HTML_SRC					_T("src=")
#define HTML_USEMAP					_T("useMap=")
#define HTML_LINK_HOST				_T("host://")
#define HTML_LINK_CAMERA			_T("cam://")
#define HTML_LINK_RELAY				_T("relay://")
#define HTML_LINK_DETECTOR			_T("detector://")
#define HTML_LINK_MEDIA				_T("media://")
#define HTML_LINK_FILE				_T("file://")

#define HTML_ELEMENT_ID				L"%08x:%08x:%08x"	// ID, Flags, Server/Host
#define HTML_META_TAG               L"META"
#define HTML_META_NAME              _T("NAME=")
#define HTML_META_CONTENT           _T("CONTENT=")
#define META_NAME_ZOOM              _T("ZOOM")
#define META_NAME_MAPTYPE           _T("MAPTYPE")
#define META_NAME_GENERATOR         _T("GENERATOR")

#define UPDATE_HTML_WND_TIMER		0x0815
#define UPDATE_HTML_WND_TIMER_EX	0x0816
#define TOOLTIP_EDIT_TIMER			0x0817

#define HTML_CHANGE_OLD_TYPE        0x00000001
#define HTML_CHANGE_MISSING_ID      0x00000002
#define HTML_CHANGE_LINK_NAME       0x00000004
#define HTML_CHANGE_IN_PROGRESS     0x00000008
#define HTML_INSERT_KNOWN_HOSTS     0x00000010
#define HTML_DESTROY_AND_DELETE		0x00000020

#define MK_IESERVERWND              0x10000000

#define HTML_TEXT_TOP    HTML_BEGIN_TAG HTML_LINEFEED \
                         HTML_HEAD_TAG HTML_LINEFEED \
						 HTML_CODEPAGE_METATAG HTML_LINEFEED \
                         HTML_GENERATOR_METATAG HTML_LINEFEED \
                         HTML_MAPTYPE_METATAG HTML_LINEFEED \
                         HTML_ZOOM_METATAG HTML_LINEFEED \
                         HTML_TITLE_TAG HTML_LINEFEED \
                         HTML_HEADEND_TAG HTML_LINEFEED \
                         HTML_BODY_TAG HTML_LINEFEED\
                         _T("  <map name=\"FPMap0\">") HTML_LINEFEED

#define HTML_TEXT_LINK   _T("   <area href=\"%s://%s\" shape=\"rect\" coords=\"%d, %d, %d, %d\" title=\"%s\" id=\"%s\">") HTML_LINEFEED

#define HTML_TEXT_BOTTOM _T("  </map>") HTML_LINEFEED \
                         _T("   <img border=\"0\" src=\"%s\" usemap=\"#FPMap0\" width=\"%d\" height=\"%d\">") HTML_LINEFEED \
                         HTML_BODYEND_TAG HTML_LINEFEED \
                         HTML_END_TAG HTML_LINEFEED

/////////////////////////////////////////////////////////////////////////////
#define RELEASE_OBJECT(pObject)	\
{								\
	if (pObject)				\
	{							\
		pObject->Release();		\
		pObject = NULL;			\
	}							\
}							
//		int nR = pObject->Release();												\
//		if (nR == 0)																\
//		{																			\
//			TRACE(_T("%s(%d):%s::Release(%d)\n"), __FILE__, __LINE__, _T(#pObject), nR);\
//		}																			\
/////////////////////////////////////////////////////////////////////////////
struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, LPCTSTR sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int     m_nLine;
	HRESULT m_hr;
	LPCTSTR	m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, _T(__FILE__))
#define HR_EXCEPTION(HR) if (FAILED(hr=HR))	throw HRESULT_ERROR_AT_POS(hr);



/////////////////////////////////////////////////////////////////////////////
// Class CHTML_Links
CHTML_Links::CHTML_Links()
{
}
/////////////////////////////////////////////////////////////////////////////
CHTML_Links::~CHTML_Links()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
CHTML_Link *CHTML_Links::HitLink(CPoint point)
{
	POSITION pos;
	for (pos = GetHeadPosition(); pos != NULL; )
	{
		CHTML_Link*pLink = GetNext(pos);
		if (pLink->rcShape.PtInRect(point))
		{
			return pLink;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CHTML_Link *CHTML_Links::Contains(CSecID id, DWORD dwServer)
{
	POSITION pos;
	for (pos = GetHeadPosition(); pos != NULL; )
	{
		CHTML_Link*pLink = GetNext(pos);
		if (   (pLink->secID    == id      )
			 && (HIWORD(pLink->dwServer) == HIWORD(dwServer)))
		{
			return pLink;
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// Class CHtmlPageHint
IMPLEMENT_DYNAMIC(CHtmlPageHint, CObject)

CHtmlPageHint::CHtmlPageHint(CObject*pRec, WORD wHost, WORD wServer, WORD wFlags)
{
	m_pRecord = pRec;
	m_wHost   = wHost;
	m_wServer = wServer;
	m_wFlags  = wFlags;
}

CSecID CHtmlPageHint::GetHostSecID()
{
	return CSecID(SECID_GROUP_HOST, m_wHost);
}

//////////////////////////////////////////////////////////////////////////
// Class CHtmlPageHint
IMPLEMENT_DYNAMIC(CHtmlElement, CObject)

CHtmlElement::CHtmlElement(IHTMLElement*pHE) : m_ptPosition(-1, -1)
{
	m_pHE = pHE;
}
CHtmlElement::CHtmlElement(CPoint pt)
{
	m_pHE = NULL;
	m_ptPosition = pt;
}

/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////

CImageList CWndHTML::gm_Images;

IMPLEMENT_DYNAMIC(CWndHTML, CWndSmall)
// CWndHTML
void CWndHTML::Init()
{
	m_nType = WST_HTML;
	m_pViewIdipClient = NULL;
	m_bForward   = false;
	m_bBack      = false;
	m_bMap       = false;
	m_bEditPage  = false;

	m_stTimeStamp.GetLocalTime();
	m_dwMonitor = USE_MONITOR_2;

	m_dwShow             = 0;
	m_hwndIEServer       = NULL;
	m_pHTMLTextContainer = NULL;
	m_pMAPImage          = NULL;

	m_pHESelected			= NULL;
	
	m_nZoomValue         = 100;
	m_nNewZoomValue      = 100;
	m_nHTMLtype          = 0;
	m_pSelected          = NULL;
	m_pToolTip           = NULL;
	m_uToolTipTimer      = 0;
	m_bDragging          = FALSE;
	m_lIEServerWndProc   = 0;
	m_ptDiff.x           = 0;
	m_ptDiff.y           = 0;
	m_dwChangeOptions    = 0;
	m_nCurrentBlinkingElement = 0;
	m_nBlinkDivider = 0;
}
/////////////////////////////////////////////////////////////////////////////
CWndHTML::CWndHTML(const CSecID id, CString &sURL)
	: CWndSmall(NULL)
{
	m_bMap = true;
	m_ID = id;
	m_sURL = sURL; 
	Init();
}
//////////////////////////////////////////////////////////////////////////
CWndHTML::CWndHTML(const CSecID id, CString &sURL, CString &sTitle, CString &sPicture, int nZoom, int nType)
{
	Init();
	m_ID = id;
	m_sURL           = sURL; 
	m_sHTMLTitle     = sTitle;

	m_sPictureSource = sPicture;
	m_nZoomValue     = nZoom;
	m_nNewZoomValue  = nZoom;
	m_nHTMLtype      = nType;
}
/////////////////////////////////////////////////////////////////////////////
CWndHTML::CWndHTML(CServer* pServer, const CString& sURL, const CSecID id)
	: CWndSmall(pServer)
{
	Init();
	m_ID = id;
	m_bMap = false;
	CString sTemp;

	sTemp = sURL;
	sTemp.MakeLower();
	if (0!=sTemp.Find(_T("http:")))
	{
		if (!DoesFileExist(sTemp))
		{
			CSecID idHost(SECID_LOCAL_HOST);
			if (pServer)
			{
				idHost = m_pServer->GetHostID();
			}
			else
			{
				m_wServerID = SECID_NO_SUBID;
				m_wHostID   = idHost.GetSubID();
			}
			sTemp.Format(_T("%s\\%lx\\%s"), CNotificationMessage::GetWWWRoot(), idHost.GetID(), sURL);
		}
	}
	m_sURL = sTemp;

	if (0==sURL.CompareNoCase(MAIN_CAMERAMAP_FILE_NAME))
	{
		m_bMap = true;
	}
}
/////////////////////////////////////////////////////////////////////////////
CWndHTML::CWndHTML(CServer* pServer, const CSecID id, const CString& sName)
	: CWndSmall(pServer)
{
	m_bMap = false;
	Init();
	m_ID = id;
	m_sName = sName;
	
	CString sURL;
	CSecID idHost(SECID_LOCAL_HOST);
	if (pServer)
	{
		idHost = m_pServer->GetHostID();
	}
	else
	{
		m_wServerID = SECID_NO_SUBID;
		m_wHostID = idHost.GetSubID();
	}

	sURL.Format(_T("%s\\%lx\\%lx.htm"), CNotificationMessage::GetWWWRoot(), idHost.GetID(),m_ID.GetID());
	m_sURL = sURL;
}
/////////////////////////////////////////////////////////////////////////////
UINT CWndHTML::GetToolBarID()
{
	return IDR_HTML;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::SetServer(CServer *pServer)
{
	m_pServer = pServer;

	if (m_pServer)
	{
		m_wServerID = pServer->GetID();
		m_wHostID   = pServer->GetHostID().GetSubID();
	}
	else
	{
		m_wServerID = SECID_NO_SUBID;
		m_wHostID   = SECSUBID_LOCAL_HOST;
	}
}
/////////////////////////////////////////////////////////////////////////////
CWndHTML::~CWndHTML()
{
	ReleaseHTMLelements();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::IsURL(const CString& sURL)
{
	if (0==m_sURL.CompareNoCase(sURL))
	{
		return TRUE;
	}
	CString s1 = sURL;
	CString s2 = m_sURL;
	s1.MakeLower();
	s2.MakeLower();
	s1.MakeReverse();
	s2.MakeReverse();
	if (s2.Find(s1)==0)
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndHTML, CWndSmall)
	//{{AFX_MSG_MAP(CWndHTML)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_HTML_HOME, OnHtmlHome)
	ON_COMMAND(ID_HTML_NEXT, OnHtmlNext)
	ON_COMMAND(ID_HTML_PREV, OnHtmlPrev)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_HTML_NEXT, OnUpdateHtmlNext)
	ON_UPDATE_COMMAND_UI(ID_HTML_PREV, OnUpdateHtmlPrev)
	ON_UPDATE_COMMAND_UI(ID_HTML_HOME, OnUpdateHtmlHome)
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_UPDATE_COMMAND_UI(ID_VIEW_MEDIAS, OnUpdateViewMedias)
	ON_COMMAND(ID_VIEW_MEDIAS, OnViewMedias)
	ON_COMMAND(ID_EDIT_CLEAR_MAP, OnEditClearMap)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_MAP, OnUpdateEditClearMap)
	ON_COMMAND(ID_CONNECT_HOST, OnConnectHost)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_HOST, OnUpdateConnectHost)
	ON_MESSAGE(WM_USER, OnUser)
	ON_COMMAND(ID_VIEW_HOSTS, OnViewHosts)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HOSTS, OnUpdateViewHosts)
	ON_COMMAND(ID_VIEW_CAMERAS, OnViewCameras)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAMERAS, OnUpdateViewCameras)
	ON_COMMAND(ID_VIEW_DETECTORS, OnViewDetectors)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETECTORS, OnUpdateViewDetectors)
	ON_COMMAND(ID_VIEW_RELAIS, OnViewRelays)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELAIS, OnUpdateViewRelays)
	ON_COMMAND(ID_ALARM_SOUND, OnAlarmSound)
	ON_UPDATE_COMMAND_UI(ID_ALARM_SOUND, OnUpdateAlarmSound)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, OnUpdateViewRefresh)
	ON_COMMAND(ID_HTML_SAVE, OnHtmlSave)
	ON_UPDATE_COMMAND_UI(ID_HTML_SAVE, OnUpdateHtmlSave)
	ON_COMMAND(ID_SMALL_CONTEXT, OnSmallContext)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CONTEXT, OnUpdateSmallContext)
	ON_COMMAND(ID_EDIT_HTML_LINK_PROPERTIES, OnEditHtmlLinkProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HTML_LINK_PROPERTIES, OnUpdateEditHtmlLinkProperties)
	ON_COMMAND(ID_HTML_NEW_PAGE, OnHtmlNewPage)
	ON_UPDATE_COMMAND_UI(ID_HTML_NEW_PAGE, OnUpdateHtmlNewPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CWndHTML, CWnd)
	ON_EVENT(CWndHTML, AFX_IDW_PANE_FIRST, 102 /* StatusTextChange */, OnStatusTextChange, VTS_BSTR)
	ON_EVENT(CWndHTML, AFX_IDW_PANE_FIRST, 105 /* CommandStateChange */, OnCommandStateChange, VTS_I4 VTS_BOOL)
	ON_EVENT(CWndHTML, AFX_IDW_PANE_FIRST, 113 /* TitleChange */, OnTitleChange, VTS_BSTR)
	ON_EVENT(CWndHTML, AFX_IDW_PANE_FIRST, 112 /* PropertyChange */, OnPropertyChange, VTS_BSTR)
	ON_EVENT(CWndHTML, AFX_IDW_PANE_FIRST, 251 /* NewWindow2 */, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CWndHTML, AFX_IDW_PANE_FIRST, 253 /* OnQuit */, OnQuit, VTS_NONE)
	ON_EVENT(CWndHTML, AFX_IDW_PANE_FIRST, 250 /* BeforeNavigate2 */, BeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CSecID CWndHTML::GetID()
{
	return m_ID;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndHTML::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime;

	if (IsMapWindow())
	{
		sTitle.LoadString(IDS_MAP);
		sTitle += _T(" ") + m_sHTMLTitle;
		if (m_bEditPage)
		{
			sTime.LoadString(IDS_HTML_EDIT_MODE);
			sTitle += sTime;
		}
		return sTitle;
	}
	CString sFormat;
	CSize size;
	CRect rect;

	GetClientRect(rect);

	sTitle = GetServerName();
	sTime.Format(_T("%02d.%02d. , %02d:%02d:%02d"),
				 m_stTimeStamp.GetDay(),
				 m_stTimeStamp.GetMonth(),
				 m_stTimeStamp.GetHour(),
				 m_stTimeStamp.GetMinute(),
				 m_stTimeStamp.GetSecond());

	if (!m_sName.IsEmpty())
	{
		sTitle += _T(": ") + m_sName;

		size = pDC->GetOutputTextExtent(sTitle);
		sTitle = m_sName + sTime;
	}
	else
	{
		sFormat.LoadString(IDS_NOTIFICATION_FROM);
		sTitle.Format(sFormat,GetServerName(),sTime);
	}

	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
// CWndHTML message handlers
/////////////////////////////////////////////////////////////////////////////
// CWndHTML event reflectors
/////////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::Create(const RECT& rect, UINT nID, CViewIdipClient* pParentWnd)
{
	BOOL bRet;
	m_pViewIdipClient = pParentWnd;
	DWORD dwFlags = WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS;
	if (nID == (UINT)-1)
	{
		dwFlags |= WS_VISIBLE;
	}
	bRet = CWnd::Create(NULL,NULL, 
						dwFlags,
						rect, pParentWnd,
						nID); 

	if (bRet && !m_sURL.IsEmpty())// && m_pServer)
	{
		NavigateTo(m_sURL);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CWndHTML::HTMLwndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	long lWndProc = GetWindowLong(hwnd, GWL_USERDATA);
/*
	if (uMsg == WM_PARENTNOTIFY)
	{
		switch (LOWORD(wParam))
		{
			case WM_RBUTTONDOWN:
				::SendMessage(::GetParent(hwnd), LOWORD(wParam), wParam, lParam);
				break;
		}
	}
*/
	return ::CallWindowProc((WNDPROC)lWndProc, hwnd, uMsg, wParam, lParam); 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CWndHTML::FindIEServer(HWND hwnd, LPARAM lParam)
{
	_TCHAR szText[256];
	GetClassName(hwnd, szText, 255);
	if (_tcscmp(szText, _T("Internet Explorer_Server")) == 0)
	{
		*((HWND*)lParam) = hwnd;
		return 0;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CWndHTML::IEServerwndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndHTML *pThis = (CWndHTML*) GetWindowLong(hwnd, GWL_ID);
	BOOL bEditPage = FALSE;
	BOOL bUpdate = FALSE;
	switch (uMsg)
	{
		case WM_MOUSEMOVE:
		{
			pThis->OnMouseMove(wParam|MK_IESERVERWND, lParam);
			bEditPage = pThis->m_bEditPage;
		}break;
		case WM_PAINT: 
		case WM_SIZE:
		case WM_MOVE:
			bUpdate = TRUE;
			break;
		case WM_NCHITTEST:
		{
			UINT nCode = pThis->OnNcHitTest(lParam);
			if (pThis->m_bEditPage)
			{
				return nCode;
			}
		}break;
		case WM_SETCURSOR:
		{
			BOOL bReturn = pThis->OnSetCursor(CWnd::FromHandle((HWND)wParam), LOWORD(lParam), HIWORD(lParam));
			if (pThis->m_bEditPage)
			{
				return bReturn;
			}
		}break;
		case WM_LBUTTONDOWN:
		{
			bEditPage = pThis->OnLButtonDown(wParam|MK_IESERVERWND, lParam);
		}break;
		case WM_LBUTTONDBLCLK:
		{
			pThis->OnLButtonDblClk(wParam|MK_IESERVERWND, lParam);
		}break;
		case WM_LBUTTONUP:
		{
			bEditPage = pThis->OnLButtonUp(wParam|MK_IESERVERWND, lParam);
		}break;
		case WM_RBUTTONDOWN:
		{			
			pThis->OnRButtonDown(wParam|MK_IESERVERWND, lParam);
			bEditPage = TRUE;
		}	break;
	}
//	TRACE(_T("Msg:%x\n"), uMsg);
	if (bEditPage) return 0;

	LRESULT lResult = ::CallWindowProc((WNDPROC)pThis->m_lIEServerWndProc, hwnd, uMsg, wParam, lParam); 
	if (bUpdate)
	{
		pThis->PostMessage(WM_TIMER, UPDATE_HTML_WND_TIMER_EX, 0);
	}

	return lResult;
}
/////////////////////////////////////////////////////////////////////////////
int CWndHTML::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (gm_Images.GetSafeHandle() == NULL)
	{
		gm_Images.Create(IDB_HTML_IMG,HTML_IMG_SIZE_X, 1, RGB(0,255,255));
	}
	
	if (CWndSmall::OnCreate(lpCreateStruct) == -1)
		return -1;

/*	CString sBuffer;
	GetModuleFileName(theApp.m_hInstance, sBuffer.GetBuffer(MAX_PATH), MAX_PATH);
	sBuffer = sBuffer.Left(sBuffer.ReverseFind('\\'));
*/
	CRect rect;
	GetClientRect(rect);
	m_WebBrowser.Create(_T("IdipClientBrowser"),WS_VISIBLE|WS_CHILD,
						rect,this,AFX_IDW_PANE_FIRST);


/*
	// in dieser Subclass-Funktion wird zur Zeit nichts abgefangen RKE
	if (theApp.IsWinXP())
	{
		SetWindowLong(m_WebBrowser.m_hWnd, GWL_USERDATA, SetWindowLong(m_WebBrowser.m_hWnd, GWL_WNDPROC, (long)CWndHTML::HTMLwndProc));
	}
*/	
	CWK_Profile wkp;
	m_dwShow    = wkp.GetInt(GetSection(), HTML_SHOW_STATES, HTML_ELEMENT_SHOW_ALL);
	m_dwMonitor = wkp.GetInt(GetSection(), USE_MONITOR, 0);

	m_uUpdateTimer = SetTimer(UPDATE_HTML_WND_TIMER, 500, NULL);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnDestroy() 
{
	if (m_nHTMLtype == HTML_TYPE_HOST_MAP || m_nHTMLtype == HTML_TYPE_CAMERA_MAP)
	{
		CWK_Profile wkp;
		wkp.WriteInt(GetSection(), HTML_SHOW_STATES, m_dwShow);
		wkp.WriteInt(GetSection(), USE_MONITOR, m_dwMonitor);
	}
	if (m_nHTMLtype == HTML_TYPE_HOST_MAP)
	{
		InsertKnownHostsToCameraView(FALSE);
	}
	CWndSmall::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
CString CWndHTML::GetSection()
{
	CString sSection;
	DWORD  dwServer = MAKELONG(m_wServerID, SECID_GROUP_HOST);
	if (!m_sURL.IsEmpty())
	{
		int nFind = m_sURL.Find(_T("dvrtwww"));
		if (nFind != -1)
		{
			sSection = IDIP_CLIENT_SETTINGS;
			sSection = sSection + _T("\\") + m_sURL.Mid(nFind+8);
		}
		else
		{
			int nFind = m_sURL.ReverseFind('\\');
			if (nFind != -1)
			{
				sSection.Format(_T("%s\\%08lx\\%s"), IDIP_CLIENT_SETTINGS, dwServer, m_sURL.Mid(nFind+1));
			}
		}
	}
	if (sSection.IsEmpty())
	{
		sSection.Format(_T("%s\\%08lx\\"), IDIP_CLIENT_SETTINGS, dwServer);
	}
	return sSection;
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::Refresh()
{
	m_WebBrowser.Refresh();
	m_stTimeStamp.GetLocalTime();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::NavigateTo(const CString& sURL)
{
	CString sMyURL = sURL;

	if (0==sMyURL.Find(HTML_LINK_FILE))
	{
		CString sFile = sURL.Mid(_tcslen(HTML_LINK_FILE));
		int nFind = sFile.ReverseFind(_T('/'));
		if (nFind != -1)
		{
			sFile = sFile.Left(nFind);
		}
		if (sFile[1] != _T(':')) // relativer pfad
		{
			CString sPath, sFileName;
			WK_SplitPath(m_sURL, sPath, sFileName);
			sFile = sPath + sFile;
		}
		if (DoesFileExist(sFile))
		{
			sMyURL = sFile;
		}
		else
		{
			sMyURL = theApp.GetDefaultURL();
		}
	}
	else if (sMyURL.GetLength()>3)
	{
		if (sMyURL[1] == _T(':') && sMyURL[2] == _T('\\'))
		{
			if (!DoesFileExist(sMyURL))
			{
				sMyURL = theApp.GetDefaultURL();
			}
		}
	}
	m_WebBrowser.Navigate(sMyURL,NULL,NULL,NULL,NULL);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnSize(UINT nType, int cx, int cy) 
{
	CWndSmall::OnSize(nType, cx, cy);

	CRect rect;
	GetFrameRect(rect);

	m_WebBrowser.MoveWindow(rect);

	CalculateClientRect();
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnHtmlNext() 
{
	m_WebBrowser.GoForward();
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateHtmlNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bForward && !m_bEditPage);
	
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnHtmlPrev() 
{
	m_WebBrowser.GoBack();
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateHtmlPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bBack && !m_bEditPage);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnHtmlHome() 
{
	if (!m_sHome.IsEmpty())
	{
		NavigateTo(m_sHome);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateHtmlHome(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_sHome.IsEmpty() && !m_bEditPage);
}
/////////////////////////////////////////////////////////////////////////////
// CWndHTML Events
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnCommandStateChange(long lCommand, BOOL bEnable)
{
	if (lCommand==0x1)
	{
		m_bForward = bEnable ? true : false;
	}
	if (lCommand==0x2)
	{
		m_bBack = bEnable ? true : false;
	}
	// user will override to handle this notification
	UNUSED_ALWAYS(lCommand);
	UNUSED_ALWAYS(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnTitleChange(LPCTSTR lpszText)
{
	// user will override to handle this notification
	CString sTitle = lpszText;
//	TRACE(_T("OnTitleChange(\"%s\")\n"), sTitle);
	sTitle.MakeLower();
	CString sFile;
	int nFindExtHTM = sTitle.Find(_T(".htm"));
	if (nFindExtHTM == -1)
	{
		sTitle = m_WebBrowser.GetLocationURL();
		sFile  = ClearStringFromProcentASCII(sTitle);
		CheckPathURL(sFile);
//		TRACE(_T("LocationURL:\"%s\"\n"), sTitle);
		if (IsMapWindow() && DoesFileExist(sFile))
		{
			m_sHTMLTitle = lpszText;
			m_wndToolBar.SetWindowText(GetTitleText(NULL));
		}
	}
	else if (m_sHome.IsEmpty())
	{
		m_sHome = ClearStringFromProcentASCII(m_WebBrowser.GetLocationURL());
		CheckPathURL(m_sHome);
	}

	if (!sTitle.IsEmpty())
	{
		sTitle.MakeLower();
		int nFind = sTitle.Find(_T(".htm"));
		if (nFind != -1)
		{
			sTitle = sTitle.Left(nFind);
		}
		nFind = sTitle.ReverseFind(_T('/'));
		if (nFind != -1)
		{
			sTitle = sTitle.Mid(nFind+1);
		}
		if (sTitle.Find(_T("camera")) != -1)
		{
			m_nHTMLtype = HTML_TYPE_CAMERA_MAP;
//			m_sHome.Empty();
		}
		else if (sTitle == _T("map"))
		{
			m_nHTMLtype = HTML_TYPE_HOST_MAP;
			m_wHostID   = SECSUBID_LOCAL_HOST;
//			m_sHome.Empty();
		}
		else if (sTitle == _T("input"))
		{
			m_nHTMLtype = HTML_TYPE_INPUT_TYPES;
		}
		else if (sTitle == _T("output"))
		{
			m_nHTMLtype = HTML_TYPE_CAMERA_CARDS;
		}
		else if (sTitle.Left(3) == _T("100"))
		{
			m_nHTMLtype = sTitle.GetLength() <=4 ? HTML_TYPE_INPUTS : HTML_TYPE_INPUT;
		}
		else if (sTitle.Left(3) == _T("200"))
		{
			m_nHTMLtype = sTitle.GetLength() <=4 ? HTML_TYPE_CAMERAS : HTML_TYPE_CAMERA;
		}
		else
		{
			m_nHTMLtype = HTML_TYPE_UNDEFINED;
		}
	}

	if (nFindExtHTM == -1)
	{
//		TRACE(_T("UpdateHTMLElements\n"));
		UpdateHTMLElements();
	}
	m_bMap = (m_nHTMLtype == HTML_TYPE_CAMERA_MAP || m_nHTMLtype == HTML_TYPE_HOST_MAP) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnPropertyChange(LPCTSTR lpszProperty)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszProperty);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
	// default to continuing
	*bCancel = TRUE;

	// user will override to handle this notification
	UNUSED_ALWAYS(ppDisp);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnDocumentComplete(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	//REVIEW: this needs a reflector
	UNUSED_ALWAYS(lpszURL);
//	TRACE(_T("OnDocumentComplete(LPCTSTR lpszURL=%s)\n"),lpszURL);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnQuit()
{
	// user will override to handle this notification
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnNavigateComplete2(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
//	TRACE(_T("OnNavigateComplete2(LPCTSTR lpszURL=%s)\n"),lpszURL);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::BeforeNavigate2(LPDISPATCH /* pDisp */, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	ASSERT(V_VT(URL) == VT_BSTR);
	ASSERT(V_VT(TargetFrameName) == VT_BSTR);
	ASSERT(V_VT(PostData) == (VT_VARIANT | VT_BYREF));
	ASSERT(V_VT(Headers) == VT_BSTR);
	ASSERT(Cancel != NULL);

	USES_CONVERSION;

	VARIANT* vtPostedData = V_VARIANTREF(PostData);
	CByteArray array;
	if (V_VT(vtPostedData) & VT_ARRAY)
	{
		// must be a vector of bytes
		ASSERT(vtPostedData->parray->cDims == 1 && vtPostedData->parray->cbElements == 1);

		vtPostedData->vt |= VT_UI1;
		COleSafeArray safe(vtPostedData);

		DWORD dwSize = safe.GetOneDimSize();
		LPVOID pVoid;
		safe.AccessData(&pVoid);

		array.SetSize(dwSize);
		LPBYTE lpByte = array.GetData();

		memcpy(lpByte, pVoid, dwSize);
		safe.UnaccessData();
	}
	// make real parameters out of the notification

	CString strTargetFrameName(V_BSTR(TargetFrameName));
	CString strURL = V_BSTR(URL);
	CString strHeaders = V_BSTR(Headers);
	DWORD nFlags = V_I4(Flags);

	// notify the user's class
	OnBeforeNavigate2(strURL, nFlags, strTargetFrameName,
		array, strHeaders, Cancel);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
									LPCTSTR lpszTargetFrameName, CByteArray& baPostData,
									LPCTSTR lpszHeaders, BOOL* bCancel)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(lpszTargetFrameName);
	UNUSED_ALWAYS(baPostData);
	UNUSED_ALWAYS(lpszHeaders);

/*
	// default to continuing
	CString sURL(lpszURL);

	sURL.MakeLower();
	if (0==sURL.Find(HTML_LINK_HOST))
	{
		*bCancel = TRUE;
		OnSelectHost(lpszURL);
	}
	else if (0==sURL.Find(HTML_LINK_CAMERA))
	{
		*bCancel = TRUE;
		OnSelectCamera(lpszURL);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnStatusTextChange(LPCTSTR pszText)
{
	if (m_bEditPage)
	{
		return;
	}
	UNUSED_ALWAYS(pszText);

	if (pszText && pszText[0])
	{
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, 0, (LPARAM)pszText);
//		TRACE(_T("OnStatusTextChange(LPCTSTR pszText=%s)\n"),pszText);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnSelectHost(IHTMLElement*pHE)
{
	CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
	if (pDoc)
	{
		DWORD dwFlags, dwServer;
		CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
		if (id != SECID_NO_ID)
		{
			CSecID idHost(SECID_GROUP_HOST, HOSTID_OF(dwServer));
			BOOL bConnect = pDoc->Connect(idHost);
			if (bConnect == TRUE)
			{
				// ATTENTION delete's myself !!!
				PostMessage(WM_COMMAND,ID_CLOSE_WND_THIS);
			}
			else if (bConnect == FALSE)
			{
				CString str;
				str.Format(IDP_NO_HOST, GetHTMLelementName(pHE));
				AfxMessageBox(str, MB_OK|MB_ICONERROR);
			}
			else
			{
				CString str;
				str.Format(_T("%08x:"), id.GetID());// nach der ID suchen
				DWORD dwUpdate = HTML_ELEMENT_SELECT|SEARCH_BY_ID|SEARCH_IN_HOSTS;
				pDoc->UpdateMapElement(SECID_NO_ID, str, dwUpdate);
			}
		}
		else
		{
			CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
			if (pDoc)
			{
				CString sHost = GetHTMLelementName(pHE);
				CHostArray& ha = theApp.GetHosts();
				CHost* pHost=NULL;
				for (int i=0;i<ha.GetSize();i++)
				{
					if (0==sHost.CompareNoCase(ha.GetAtFast(i)->GetName()))
					{
						pHost = ha.GetAtFast(i);
						break;
					}
				}

				if (pHost)
				{
					pDoc->Connect(pHost->GetID());
					// ATTENTION delete's myself !!!
					PostMessage(WM_COMMAND,ID_CLOSE_WND_THIS);
				}
				else
				{
					CString s;
					s.Format(IDP_NO_HOST,sHost);
					COemGuiApi::MessageBox(s,20);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnSelectCamera(IHTMLElement*pHE)
{
	CViewCamera*pView = theApp.GetMainFrame()->GetViewCamera();
	if (pView)
	{
		DWORD dwFlags, dwServer;
		CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
		WORD wServerID, wHost = HOSTID_OF(dwServer);

		if (wHost != SECID_NO_SUBID)
		{
			wServerID = pView->GetDocument()->GetServerIDbyHostID(wHost, TRUE);
		}
		else
		{
			wServerID = m_wServerID;
		}

		if (wServerID != SECID_NO_SUBID)
		{
			pView->CViewControllItems::Select(wServerID, id);
		}
		else
		{
			pView->SetFirstCamID(id);
		}
	}
/*		
		CString sID;
		CSecID ServerID(wHost, m_wServerID);
		sID.Format(_T("%08x"), id.GetID());
		DWORD dwUpdate = HTML_ELEMENT_UPDATE_PAGE|HTML_ELEMENT_SELECT|SEARCH_IN_CAMERAS|SEARCH_BY_ID;
		pDoc->UpdateMapElement(ServerID, sID, dwUpdate);
*/
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnSelectDetector(IHTMLElement*pHE)
{
	CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
	if (pDoc)
	{
		DWORD   dwFlags, dwServer;
		CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);

		CString sID;
		CSecID  ServerID(HOSTID_OF(dwServer), m_wServerID);
		sID.Format(_T("%08x"), id.GetID());
		DWORD dwUpdate = HTML_ELEMENT_UPDATE_PAGE|HTML_ELEMENT_SELECT|SEARCH_IN_INPUT|SEARCH_BY_ID;
		pDoc->UpdateMapElement(ServerID, sID, dwUpdate);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnSelectRelay(IHTMLElement*pHE)
{
	CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
	if (pDoc)
	{
		DWORD   dwFlags, dwServer;
		CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);
		CViewCamera*pView = theApp.GetMainFrame()->GetViewCamera();
		if (pView)
		{
			WORD wServerID = 0, wHost = HOSTID_OF(dwServer);

			if (wHost != SECID_NO_SUBID)
			{	// Connect if disconnected
				wServerID = pDoc->GetServerIDbyHostID(wHost, TRUE);
			}

			if (wServerID == SECID_NO_SUBID)	// not connected
			{
				pView->SetFirstCamID(SECID_NO_ID-1);// no cameras to begin
			}
		}
		CString sID;
		CSecID  ServerID(HOSTID_OF(dwServer), m_wServerID);
		sID.Format(_T("%08x"), id.GetID());
		DWORD dwUpdate = HTML_ELEMENT_UPDATE_PAGE|HTML_ELEMENT_SELECT|SEARCH_IN_RELAYS|SEARCH_BY_ID;
		pDoc->UpdateMapElement(ServerID, sID, dwUpdate);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnSelectMedia(IHTMLElement*pHE)
{
	CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
	CViewAudio* pView = theApp.GetMainFrame()->GetViewAudio();
	if (pView)
	{
		DWORD dwFlags, dwServer;
		CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
		WORD wServerID, wHost = HOSTID_OF(dwServer);

		if (wHost != SECID_NO_SUBID)
		{
			wServerID = pDoc->GetServerIDbyHostID(wHost, TRUE);
		}
		else
		{
			wServerID = m_wServerID;
		}

		if (wServerID != SECID_NO_SUBID)
		{
			pView->CViewControllItems::Select(wServerID, id);
		}
		else
		{
			pView->SetSelectMediaID(id);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::ExtractNameFromURL(const CString &sURL, CString &sName, WORD &wHost)
{
	CString sURLpure = ClearStringFromProcentASCII(sURL);
	int p;
	p = sURLpure.Find(_T("://"));
	if (p!=-1)
	{
		sName = sURLpure.Mid(p+3);
		if (sName.GetAt(sName.GetLength()-1)=='/')
		{
			sName = sName.Left(sName.GetLength()-1);
		}
		p = sName.Find(_T("@"));
		if (p != -1)
		{
			DWORD dwHost;
			_stscanf(sName.Mid(p+1), _T("%04x"), &dwHost);
			wHost = (WORD)dwHost;
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::CheckPathURL(CString &sURL)
{
	if (sURL.Find(HTML_LINK_FILE) == 0)
	{
		sURL = sURL.Mid(_tcslen(HTML_LINK_FILE));
		if (sURL[0] == _T('/'))
		{
			sURL = sURL.Mid(1);
		}
		sURL.Replace(_T('/'), _T('\\'));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_pViewIdipClient->SetWndSmallCmdActive(this);
	m_ptLastHit = point;
	CWndSmall::OnRButtonDown(nFlags, point);
	CMainFrame*	pMF = theApp.GetMainFrame();
	CMenu		menu,  *pMonMenu = NULL;
	CSkinMenu*	pSubMenu=NULL;
	menu.LoadMenu(IDR_MENU_LIVE);
	if (pMF->GetNoOfExternalFrames()>0)
	{
		pMonMenu = menu.GetSubMenu(SUB_MENU_MONITOR);
	}
	CMenu   *pFPSMenu;
	CString  sL;
	pFPSMenu = menu.GetSubMenu(SUB_MENU_HTML);
	ASSERT(pFPSMenu != NULL);
	if (   theApp.m_pPermission == NULL								// Berechtigung
		 || !theApp.m_pPermission->IsSuperVisor()					// Kein Supervisor, keine Map
		 || (m_nHTMLtype != HTML_TYPE_HOST_MAP && m_nHTMLtype != HTML_TYPE_CAMERA_MAP))
	{
		int nCount = pFPSMenu->GetMenuItemCount();					// menü ist hinten
		pFPSMenu->DeleteMenu(nCount-1, MF_BYPOSITION);				// Editmeü löschen
	}

	if ((m_nHTMLtype == HTML_TYPE_HOST_MAP || m_nHTMLtype == HTML_TYPE_CAMERA_MAP)
		 && m_wHostID == SECSUBID_LOCAL_HOST)						// ist es ´ne Map
	{
		if (m_bEditPage)											// Edit mode
		{
			pFPSMenu = pFPSMenu->GetSubMenu(2);						// Nur Bearbeiten
		}
		if (m_wndToolBar.GetSize().cy > 0)
		{
			pFPSMenu->DeleteMenu(0, MF_BYPOSITION);					// Wechseln zu löschen
		}
		else
		{
			UINT nID = ID_CLOSE_WND_THIS;
			CString str;
			if (((CSkinMenu*)&menu)->FindMenuString(nID, str))
			{
				nID = ID_CLOSE_WND_THIS;
				pFPSMenu->InsertMenu(0, MF_BYPOSITION, nID, str); 
			}
		}
		if (pFPSMenu->GetMenuItemCount() == 1)						// wenn nur noch eins da ist
		{
			pFPSMenu = pFPSMenu->GetSubMenu(0);						// Menü direkt aufrufen
		}
	}
	else															// bei den Info Karten
	{
		pFPSMenu = pFPSMenu->GetSubMenu(0);							// Nur Wechseln zu
		if (m_wndToolBar.GetSize().cy <= 0)
		{
			UINT nID = ID_CLOSE_WND_THIS;
			CString str;
			if (((CSkinMenu*)&menu)->FindMenuString(nID, str))
			{
				nID = ID_CLOSE_WND_THIS;
				pFPSMenu->InsertMenu(0, MF_BYPOSITION, nID, str); 
			}
		}
	}

	if (pMonMenu && pFPSMenu)										// Monitor items hinzufügen
	{
		CString sL;
		pFPSMenu->AppendMenu(MF_SEPARATOR, 0, _T(""));
		int i, nCount = pMonMenu->GetMenuItemCount();
		for (i=0; i<nCount; i++)
		{
			pMonMenu->GetMenuString(i, sL, MF_BYPOSITION);
			pFPSMenu->AppendMenu(MF_STRING, pMonMenu->GetMenuItemID(i), sL);
		}
		pSubMenu = (CSkinMenu*)pFPSMenu;
/*
		{	// Menu wird hier etwas anders zusammengebaut RKE
			int i, nCount = pFPSMenu->GetMenuItemCount();
			for (i=0; i<nCount; i++)
			{
				CMenu *pSub = pFPSMenu->GetSubMenu(i);
				if (pSub == NULL) break;
				pFPSMenu->GetMenuString(i, sL, MF_BYPOSITION);
				pSubMenu->AppendMenu(MF_POPUP,(UINT)pSub->m_hMenu,sL);
			}
		}
*/
	}
	else
	{
		pSubMenu = (CSkinMenu*)pFPSMenu;
	}

	if (m_bEditPage)
	{
		if (m_bDragging)
		{
			gm_Images.DragLeave(this);
			gm_Images.EndDrag();
			m_bDragging = FALSE;
			ReleaseCapture();
			m_ptDiff = CPoint(0, 0);
			return;
		}
		else
		{
			HitEditLink(point, &m_pSelected);
		}
	}
	else
	{
		IHTMLElement *pHE = FindElementFromPoint(point, SEARCH_IN_ALL);
		CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
		if (pHE && pDoc)
		{
			DWORD dwFlags, dwServer, dwUpdate = HTML_ELEMENT_MENU|SEARCH_BY_ID;
			CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
			CSecID ServerID(HOSTID_OF(dwServer), m_wServerID);
			CString str;
			if (m_pHESelected != pHE)
			{
				InvalidateElementRgn(m_pHESelected);
				InvalidateElementRgn(pHE);
				m_pHESelected = pHE;
			}

			str.Format(_T("%08x"), id.GetID());
			switch (HE_TYPE(dwFlags))
			{
				case HTML_ELEMENT_IS_RELAY:
				{
					dwUpdate |= SEARCH_IN_RELAYS;
					if (pDoc->UpdateMapElement(ServerID, str, dwUpdate) != SECID_NO_ID)
					{
						pSubMenu = NULL;
					}
				}break;
				case HTML_ELEMENT_IS_CAMERA:
				{
					dwUpdate |= SEARCH_IN_CAMERAS;
					if (pDoc->UpdateMapElement(ServerID, str, dwUpdate) != SECID_NO_ID)
					{
						pSubMenu = NULL;
					}
				}break;
				case HTML_ELEMENT_IS_DETECTOR:
				{
					dwUpdate |= SEARCH_IN_INPUT;
					if (pDoc->UpdateMapElement(ServerID, str, dwUpdate) != SECID_NO_ID)
					{
						pSubMenu = NULL;
					}
				}break;
				case HTML_ELEMENT_IS_AUDIO_IN:
				case HTML_ELEMENT_IS_AUDIO_OUT:
				{
					dwUpdate |= SEARCH_IN_MEDIA;
					if (pDoc->UpdateMapElement(ServerID, str, dwUpdate) != SECID_NO_ID)
					{
						pSubMenu = NULL;
					}
				}break;
				case HTML_ELEMENT_IS_HOST:
				{
					dwUpdate |= SEARCH_IN_HOSTS;
					if (pDoc->UpdateMapElement(SECID_NO_ID, str, dwUpdate) == SECID_NO_ID)
					{
						pSubMenu = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_CONNECT_HOST);
					}
					else
					{
						pSubMenu = NULL;
					}
				}break;
			}
		}
	}
	if (pSubMenu)
	{
		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->ConvertMenu(TRUE);
		pSubMenu->TrackPopupMenu(0, pt.x, pt.y, pMF);
		pSubMenu->ConvertMenu(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnViewHosts() 
{
	if (m_dwShow & HTML_ELEMENT_SHOW_HOSTS)
	{
		m_dwShow &= ~HTML_ELEMENT_SHOW_HOSTS;
	}
	else
	{
		m_dwShow |= HTML_ELEMENT_SHOW_HOSTS;
	}
	::InvalidateRect(m_hwndIEServer, NULL, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateViewHosts(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Hosts.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_HOSTS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnViewCameras() 
{
	if (m_dwShow & HTML_ELEMENT_SHOW_CAMERAS)
	{
		m_dwShow &= ~HTML_ELEMENT_SHOW_CAMERAS;
	}
	else
	{
		m_dwShow |= HTML_ELEMENT_SHOW_CAMERAS;
	}
	::InvalidateRect(m_hwndIEServer, NULL, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateViewCameras(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Cameras.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_CAMERAS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnViewDetectors() 
{
	if (m_dwShow & HTML_ELEMENT_SHOW_DETECTORS)
	{
		m_dwShow &= ~HTML_ELEMENT_SHOW_DETECTORS;
	}
	else
	{
		m_dwShow |= HTML_ELEMENT_SHOW_DETECTORS;
	}
	::InvalidateRect(m_hwndIEServer, NULL, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateViewDetectors(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Detectors.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_DETECTORS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnViewRelays() 
{
	if (m_dwShow & HTML_ELEMENT_SHOW_RELAIS)
	{
		m_dwShow &= ~HTML_ELEMENT_SHOW_RELAIS;
	}
	else
	{
		m_dwShow |= HTML_ELEMENT_SHOW_RELAIS;
	}
	::InvalidateRect(m_hwndIEServer, NULL, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateViewRelays(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Relays.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_RELAIS) ? TRUE : FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnViewMedias() 
{
	if (m_dwShow & HTML_ELEMENT_SHOW_MEDIAS)
	{
		m_dwShow &= ~HTML_ELEMENT_SHOW_MEDIAS;
	}
	else
	{
		m_dwShow |= HTML_ELEMENT_SHOW_MEDIAS;
	}
	::InvalidateRect(m_hwndIEServer, NULL, FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateViewMedias(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Medias.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_MEDIAS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnAlarmSound() 
{
	if (m_dwShow & HTML_ELEMENT_SOUND)
	{
		m_dwShow &= ~HTML_ELEMENT_SOUND;
	}
	else
	{
		m_dwShow |= HTML_ELEMENT_SOUND;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateAlarmSound(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Detectors.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SOUND) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnViewRefresh() 
{
	if (!m_bEditPage)
	{
		Refresh();
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateViewRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bEditPage);	
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnTimer(UINT nIDEvent)
{
	BOOL bTimerEvent = TRUE;
	if (nIDEvent == m_uToolTipTimer)
	{
		KillTimer(m_uToolTipTimer);
		m_uToolTipTimer = 0;
		
		MSG msg = {m_hWnd, WM_MOUSEMOVE, 0, 0, 0, {0,0}};
		m_nFlags |= WF_TRACKINGTOOLTIPS;
		FilterToolTipMessage(&msg);
		m_nFlags &= ~WF_TRACKINGTOOLTIPS;
		return;
	}
	else if (nIDEvent == UPDATE_HTML_WND_TIMER_EX)
	{
		nIDEvent    = m_uUpdateTimer;
		m_dwShow   |= HTML_ELEMENT_UPDATE;
		bTimerEvent = FALSE;
	}
	else if (nIDEvent == m_uUpdateTimer && (m_ptDiff.x != 0 || m_ptDiff.y !=0))
	{
		CPoint ptScroll;
		m_pHTMLTextContainer->get_scrollLeft(&ptScroll.x);
		m_pHTMLTextContainer->get_scrollTop(&ptScroll.y);
		ptScroll += m_ptDiff;
		m_pHTMLTextContainer->put_scrollLeft(ptScroll.x);
		m_pHTMLTextContainer->put_scrollTop(ptScroll.y);
		m_bDragging |= 2;
	}

	
	if (   nIDEvent == m_uUpdateTimer 
		 && m_hwndIEServer != NULL
		 && (m_dwShow & HTML_ELEMENT_UPDATE) != 0)
	{
		int nBlinkingElements = 0;
		IHTMLElement *pCurrentBlinking = NULL;
		CRgn rgn;
		CDC *pDC = CWnd::FromHandle(m_hwndIEServer)->GetDC();
		if (pDC == NULL)
		{
			return;
		}
		pDC->SaveDC();
		rgn.CreateRectRgn(m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
		pDC->SelectClipRgn(&rgn);
		m_dwShow &= ~HTML_ELEMENT_UPDATE;

		if (m_bEditPage)
		{
			POSITION pos;
			CPoint pt;
			
			for (pos = m_EditLinks.GetHeadPosition(); pos != NULL; )
			{
				CHTML_Link*pLink = m_EditLinks.GetNext(pos);
				pt = GetHTMLelementPosition(NULL, 0, &pLink->rcShape);
				if (pLink->dwFlags & HTML_ELEMENT_NAME_CHANGED)
				{
					gm_Images.Draw(pDC, HTML_IMG_MARKED, pt, ILD_TRANSPARENT);
				}
				if (m_pSelected == pLink)
				{
					gm_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
				}
				gm_Images.Draw(pDC, pLink->nItem, pt, ILD_TRANSPARENT);
			}
		}
		else
		{
			IHTMLElement *pHE;
			CPoint pt;
			
			if (bTimerEvent)
			{
				if (m_dwShow & HTML_ELEMENT_BLINK_STATE)
				{
					m_dwShow &= ~HTML_ELEMENT_BLINK_STATE;
					if (m_nBlinkDivider++ == 2)
					{
						m_nCurrentBlinkingElement++;
						m_nBlinkDivider = 0;
					}
				}
				else
				{
					m_dwShow |= HTML_ELEMENT_BLINK_STATE;
				}
			}
			if (m_dwShow & HTML_ELEMENT_SHOW_HOSTS)
			{
				POSITION pos = m_Hosts.GetHeadPosition();
				while (pos)
				{
					pHE = (IHTMLElement*) m_Hosts.GetNext(pos);

					int nImage = HTML_IMG_HOST;
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					CRect  rc;

					pt  = GetHTMLelementPosition(pHE, 0, &rc);
					if (dwFlags & HTML_ELEMENT_IS_LINK)
					{
						if (   dwFlags & HTML_ELEMENT_BLINK)
						{
							nBlinkingElements++;
							if (bTimerEvent && m_nCurrentBlinkingElement == nBlinkingElements)
							{
								pCurrentBlinking = pHE;
							}
							if (m_dwShow & HTML_ELEMENT_BLINK_STATE)
							{
								if (m_dwShow & HTML_ELEMENT_SOUND && bTimerEvent) Beep(440, 100);
								nImage = HTML_IMG_SUB_LINK_ALARM0;
							}
							else
							{
								nImage = HTML_IMG_SUB_LINK_ALARM1;
							}


							m_dwShow |= HTML_ELEMENT_UPDATE;
						}
						else
						{
							nImage = HTML_IMG_SUB_LINK;
						}
					}
					else if (dwFlags & HTML_ELEMENT_ACTIVE) nImage = HTML_IMG_HOST_CONNECTED;
					gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
					if (!(dwFlags & HTML_ELEMENT_OK))
					{
						gm_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
					}
				}
			}
			if (m_dwShow & HTML_ELEMENT_SHOW_CAMERAS)
			{
				POSITION pos = m_Cameras.GetHeadPosition();
				while (pos)
				{
					int nImage = HTML_IMG_CAM;
					pHE = (IHTMLElement*) m_Cameras.GetNext(pos);
					pt  = GetHTMLelementPosition(pHE, 0);
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (dwFlags & HTML_ELEMENT_NAME_CHANGED)
					{	// red background
						gm_Images.Draw(pDC, HTML_IMG_MARKED, pt, ILD_TRANSPARENT);
					}
					else if (m_pHESelected == pHE)
					{	// green background
						gm_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}
				
					if (!(dwFlags & HTML_ELEMENT_CAM_FIXED)) nImage = HTML_IMG_CAM_PTZ;
					// draw base symbol
					gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
/*
					// RKE: too much info is not good for the user
					if (dwFlags & HTML_ELEMENT_CAM_COLOR)
					{	// draw rgb symbol inside
						CPoint ptRGB(pt);
						if (nImage == HTML_IMG_CAM_PTZ)
						{
							ptRGB.Offset(1, -2);
						}
						gm_Images.Draw(pDC, HTML_IMG_CAM_RGB, ptRGB, ILD_TRANSPARENT);
					}

					if (dwFlags & HTML_ELEMENT_CAM_REF)
					{	// draw reference cam symbol above base
						gm_Images.Draw(pDC, HTML_IMG_CAM_REF, pt, ILD_TRANSPARENT);
					}
*/
					if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{	// draw unplugged symbol under base
						gm_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}

					if (!(dwFlags & HTML_ELEMENT_OK))
					{	// draw cross over base
						gm_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
					}
					else if (dwFlags & HTML_ELEMENT_ENABLED)
					{	// draw rect around all
						gm_Images.Draw(pDC, HTML_IMG_ACTIVE, pt, ILD_TRANSPARENT);
					}

				}
			}
			if (m_dwShow & HTML_ELEMENT_SHOW_RELAIS)
			{
				POSITION pos = m_Relays.GetHeadPosition();
				while (pos)
				{
					int nImage = HTML_IMG_RELAY_CLOSED;
					pHE = (IHTMLElement*) m_Relays.GetNext(pos);
					pt  = GetHTMLelementPosition(pHE, 0);
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (m_pHESelected == pHE)
					{
						gm_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}

					if (dwFlags & HTML_ELEMENT_NAME_CHANGED)
					{
						gm_Images.Draw(pDC, HTML_IMG_RELAY_OPEN, pt, ILD_TRANSPARENT);
					}
					else if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{
						gm_Images.Draw(pDC, HTML_IMG_RELAY_OPEN   , pt, ILD_TRANSPARENT);
						gm_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}
					else
					{
						if (dwFlags & HTML_ELEMENT_OPEN) nImage = HTML_IMG_RELAY_OPEN;
						gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
						if (!(dwFlags & HTML_ELEMENT_OK))
						{
							gm_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
						}
					}
				}
			}
			if (m_dwShow & HTML_ELEMENT_SHOW_DETECTORS)
			{
				POSITION pos = m_Detectors.GetHeadPosition();
				int nImage;
				while (pos)
				{
					CRect rc;
					pHE = (IHTMLElement*) m_Detectors.GetNext(pos);
					pt  = GetHTMLelementPosition(pHE, 0, &rc);
					nImage = HTML_IMG_DETECTOR;
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (m_pHESelected == pHE)
					{
						gm_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}
					if (dwFlags & HTML_ELEMENT_NAME_CHANGED)
					{
						gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
					}
					else if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{
						gm_Images.Draw(pDC, HTML_IMG_DETECTOR     , pt, ILD_TRANSPARENT);
						gm_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}
					else
					{
						if (dwFlags & HTML_ELEMENT_BLINK)
						{
							nBlinkingElements++;
							if (bTimerEvent && m_nCurrentBlinkingElement == nBlinkingElements)
							{
								pCurrentBlinking = pHE;
							}
							if (m_dwShow & HTML_ELEMENT_BLINK_STATE)
							{
								if (m_dwShow & HTML_ELEMENT_SOUND && bTimerEvent) Beep(440, 100);
								nImage = HTML_IMG_DETECTOR_ALARM0;
							}
							else
							{
								nImage = HTML_IMG_DETECTOR_ALARM1;
							}
							m_dwShow |= HTML_ELEMENT_UPDATE;
						}
						else if (!(dwFlags & HTML_ELEMENT_OK))
						{
							nImage = HTML_IMG_DETECTOR_DISABLED;
						}
						gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
					}
				}
			}
			if (m_dwShow & HTML_ELEMENT_SHOW_MEDIAS)
			{
				POSITION pos = m_Medias.GetHeadPosition();
				while (pos)
				{
					int nImage = HTML_IMG_MARKED;
					pHE = (IHTMLElement*) m_Medias.GetNext(pos);
					pt  = GetHTMLelementPosition(pHE, 0);
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (HE_TYPE(dwFlags) == HTML_ELEMENT_IS_AUDIO_OUT) nImage = HTML_IMG_LOUDSPEAKER;
					if (HE_TYPE(dwFlags) == HTML_ELEMENT_IS_AUDIO_IN)  nImage = HTML_IMG_MICROPHONE;
					if (m_pHESelected == pHE)
					{
						gm_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}
					if (dwFlags & HTML_ELEMENT_NAME_CHANGED)
					{
						gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
					}
					else if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{
						gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
						gm_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}
					else
					{
						gm_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
						if (!(dwFlags & HTML_ELEMENT_ENABLED))
						{
							gm_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
						}
						else if (dwFlags & HTML_ELEMENT_ACTIVE)
						{
							gm_Images.Draw(pDC, HTML_IMG_ACTIVE, pt, ILD_TRANSPARENT);
						}
					}
				}
			}
		}
		pDC->RestoreDC(-1);
		CWnd::FromHandle(m_hwndIEServer)->ReleaseDC(pDC);

		if (bTimerEvent && m_nCurrentBlinkingElement >= nBlinkingElements)
		{
			m_nCurrentBlinkingElement = 0;
		}
		if (pCurrentBlinking)
		{
			CPoint pt;
			CRect  rc;
			pt = GetHTMLelementPosition(pCurrentBlinking, 0, &rc);
			if (!IsHTMLElememtVisible(pt, rc))
			{
				IHTMLTextContainer * pSave = m_pHTMLTextContainer;
				m_pHTMLTextContainer = NULL;
				pt = GetHTMLelementPosition(pCurrentBlinking, 0, &rc);
				m_pHTMLTextContainer = pSave;
				ScrollToPosition(pt);
			}
		}
	}
	else
	{
		CWndSmall::OnTimer(nIDEvent);
	}
}
//////////////////////////////////////////////////////////////////////////
LRESULT CWndHTML::OnUser(WPARAM wParam, LPARAM lParam)
{
	if (wParam == HTML_CHANGE_LINK_NAME)
	{
		int nResult = AfxMessageBox(IDS_HTML_LINK_NAMES_CHANGED, MB_ICONINFORMATION|MB_YESNOCANCEL);
		if (nResult	== IDYES)
		{
			OnEditHtmlPage();
			OnEditHtmlPage();
		}
		else if (m_pViewIdipClient && nResult == IDNO)
		{
			OnEditHtmlPage();
			m_pViewIdipClient->SetWndSmallCmdActive(this);
			m_pViewIdipClient->OnView1();
		}
//		else // cancel: nothing to do
//		{
//		}
	}
	else if (wParam == HTML_CHANGE_OLD_TYPE)
	{
		OnEditHtmlPage();
		OnEditHtmlPage();
	}
	else if (wParam == HTML_INSERT_KNOWN_HOSTS)
	{
		InsertKnownHostsToCameraView(lParam);
	}
	else if (wParam == HTML_DESTROY_AND_DELETE)
	{
		delete this;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CPoint CWndHTML::GetHTMLelementPosition(IHTMLElement *pHE, UINT uAlign, CRect*prcBound)
{
	CPoint pt, ptOffset, ptScroll;
	CRect  rc;
	if (pHE)
	{
		pHE->get_offsetTop(&pt.y);
		pHE->get_offsetLeft(&pt.x);
	}
	else if (prcBound)
	{
		pt.x = prcBound->left;
		pt.y = prcBound->top;
	}

	if (m_pMAPImage)
	{
		m_pMAPImage->get_offsetTop(&ptOffset.y);
		m_pMAPImage->get_offsetLeft(&ptOffset.x);
		pt += ptOffset;
	}
	
	if (m_pHTMLTextContainer)
	{
		m_pHTMLTextContainer->get_scrollTop(&ptScroll.y);
		m_pHTMLTextContainer->get_scrollLeft(&ptScroll.x);
		pt -= ptScroll;
	}

	//	::MapWindowPoints(m_hwndIEServer, m_hWnd, &pt, 1);

	if (prcBound && pHE)
	{
		long nHeight, nWidth;
		prcBound->left   = pt.x;
		prcBound->top    = pt.y;
		pHE->get_offsetHeight(&nHeight);
		pHE->get_offsetWidth(&nWidth);
		prcBound->right  = prcBound->left + nWidth;
		prcBound->bottom = prcBound->top  + nHeight;
	}
	return pt;
}
/////////////////////////////////////////////////////////////////////////////
CSecID CWndHTML::GetHTMLelementID(IHTMLElement *pHE, DWORD& dwFlags, DWORD &dwServer)
{
	CSecID secID;
	BSTR id;
	pHE->get_id(&id);
	if (id)
	{
		DWORD dwElement = (DWORD)pHE, dwID, dwValue2, dwValue3;
		int nValues = swscanf(id, HTML_ELEMENT_ID, (DWORD)&dwID, &dwValue2, &dwValue3);
		if (nValues == 3)
		{
			secID    = CSecID(dwID);
			dwFlags  = dwValue2;
			dwServer = dwValue3;
		}
		if (m_HTMLelementFlags.Lookup(dwElement, dwValue2))
		{
			dwFlags  = dwValue2;
		}
	}
	return secID;
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::SetHTMLelementID(IHTMLElement *pHE, CSecID secID, DWORD dwFlags, DWORD dwServer, BOOL bForce/*=FALSE*/)
{
	DWORD dwElement = (DWORD)pHE, dwOldFlags = dwFlags;
	if (bForce || !m_HTMLelementFlags.Lookup(dwElement, dwOldFlags))
	{
		wchar_t id[32];
		swprintf(id, HTML_ELEMENT_ID, secID.GetID(), dwFlags, dwServer);
		pHE->put_id(id);
	}
	m_HTMLelementFlags.SetAt(dwElement, dwFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::ReleaseHTMLelements()
{
	m_HTMLelementFlags.RemoveAll();
	m_LinkDetectors.RemoveAll();

	while (m_Hosts.GetCount())
	{
		((IHTMLElement*)m_Hosts.RemoveHead())->Release();
	}
	while (m_Cameras.GetCount())
	{
		((IHTMLElement*)m_Cameras.RemoveHead())->Release();
	}
	while (m_Detectors.GetCount())
	{
		((IHTMLElement*)m_Detectors.RemoveHead())->Release();
	}
	while (m_Relays.GetCount())
	{
		((IHTMLElement*)m_Relays.RemoveHead())->Release();
	}
	while (m_Medias.GetCount())
	{
		((IHTMLElement*)m_Medias.RemoveHead())->Release();
	}
	RELEASE_OBJECT(m_pHTMLTextContainer);
	RELEASE_OBJECT(m_pMAPImage);

	m_pHESelected = NULL;	// if the Elements are deleted:
	m_pSelected   = NULL;	// set selections to NULL
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	DWORD         dwFlags,
		          dwServer = MAKELONG(m_wServerID, m_wHostID);
	CRect         rc;
	CPoint        pt(-1, -1);
	IHTMLElement *pScrollToBeVisible = NULL;
	
	ASSERT_KINDOF(CHtmlPageHint, pHint);
	CHtmlPageHint *pHPhint = (CHtmlPageHint*)pHint;
	
	if (    m_bEditPage 
		 && !m_bDragging)
	{
		CHTML_Link *pOld = m_pSelected;
		CSecID id;
		if (pHPhint->GetRecord() && pHPhint->GetRecord()->IsKindOf(RUNTIME_CLASS(CIPCInputRecord)))
		{
			CIPCInputRecord *pRecI = (CIPCInputRecord*)pHPhint->GetRecord();
			id = pRecI->GetID();
		}
		else if (pHPhint->GetRecord() && pHPhint->GetRecord()->IsKindOf(RUNTIME_CLASS(CIPCOutputRecord)))
		{
			CIPCOutputRecord *pRecO = (CIPCOutputRecord*)pHPhint->GetRecord();
			id = pRecO->GetID();
		}
		else if (pHPhint->GetRecord() && pHPhint->GetRecord()->IsKindOf(RUNTIME_CLASS(CIPCMediaRecord)))
		{
			CIPCMediaRecord *pRecM = (CIPCMediaRecord*)pHPhint->GetRecord();
			id = pRecM->GetID();
		}

		if (pHPhint->GetFlags() & VDH_FLAGS_SELECTED)
		{
			m_pSelected = NULL;
		}
		
		CHTML_Link *pNew = m_EditLinks.Contains(id, dwServer);
		if (pNew)
		{
			m_pSelected = pNew;
			CPoint ptCenter = m_pSelected->rcShape.CenterPoint();
			CPoint ptScroll, ptTest;
			m_pHTMLTextContainer->get_scrollLeft(&ptScroll.x);
			m_pHTMLTextContainer->get_scrollTop(&ptScroll.y);
			ptTest = ptCenter - ptScroll;
			if (!m_rcClient.PtInRect(ptTest))
			{
				pt = ptCenter;
			}
		}
		if (pOld != m_pSelected)
		{
			InvalidateLinkRgn(pOld);
			InvalidateLinkRgn(m_pSelected);
		}
	}
	else
	{
		IHTMLElement *pHEold   = m_pHESelected;
		if (pHPhint->GetFlags() & VDH_FLAGS_SELECTED)
		{
			m_pHESelected = NULL;
		}
		
		if (pHPhint->GetRecord() == NULL) // Host
		{
			POSITION pos = m_Hosts.GetHeadPosition();
			IHTMLElement *pHE;
			CSecID id, idSearch = pHPhint->GetHostSecID();
			while (pos)
			{
				pHE = (IHTMLElement*) m_Hosts.GetNext(pos);
				id = GetHTMLelementID(pHE, dwFlags, dwServer);
				if (id == idSearch)
				{
					if (pHPhint->GetFlags() == TRUE) dwFlags |=  HTML_ELEMENT_ACTIVE;
					else                             dwFlags &= ~HTML_ELEMENT_ACTIVE;
					SetHTMLelementID(pHE, id, dwFlags, dwServer);

					CPoint pt = GetHTMLelementPosition(pHE, 0, &rc);
					if (IsHTMLElememtVisible(pt, rc))
					{
						InvalidateRectRgn(&rc);
//						TRACE(_T("Invalidate Host\n"));
					}
					else
					{
						pScrollToBeVisible = pHE;
					}
				}
			}
		}
		else if (pHPhint->GetRecord()->IsKindOf(RUNTIME_CLASS(CIPCInputRecord)))
		{
			CIPCInputRecord *pRecI = (CIPCInputRecord*)pHPhint->GetRecord();
			POSITION pos = m_Detectors.GetHeadPosition();
			IHTMLElement *pHE;
			CSecID id, idInput = pRecI->GetID();
			DWORD dwLink = 0;
			if (!m_LinkDetectors.Lookup(idInput.GetID(), dwLink) && dwLink)
			{
				dwLink = 0;
			}
			if (dwLink)
			{
				pHE = (IHTMLElement*) dwLink;
				id = GetHTMLelementID(pHE, dwFlags, dwServer);
				dwFlags = HTML_ELEMENT_IS_LINK|HTML_ELEMENT_OK;
				if (pRecI->GetIsActive())
				{
					dwFlags |= HTML_ELEMENT_ACTIVE|HTML_ELEMENT_BLINK;
				}
				else
				{
					//TODO! RKE: determine the state of the other detectors
					CIdipClientDoc*pDoc = m_pViewIdipClient->GetDocument();
					CString str;
					DWORD dwID, dwL, dwUpdate;
					POSITION pos = m_LinkDetectors.GetStartPosition();
					while (pos)
					{
						m_LinkDetectors.GetNextAssoc(pos, dwID, dwL);
						if (dwL == dwLink && idInput.GetID() != dwID)
						{
							str.Format(_T("%08x:"), dwID);
							dwUpdate = SEARCH_BY_ID|SEARCH_IN_INPUT|HTML_ELEMENT_GET_STATE_FLAG;
							pDoc->UpdateMapElement(CSecID(pHPhint->GetHostID(), pHPhint->GetServerID()), str, dwUpdate);
							if (dwUpdate & HTML_ELEMENT_ACTIVE)
							{
								dwFlags |= HTML_ELEMENT_ACTIVE|HTML_ELEMENT_BLINK;
								break;
							}
						}
					}

				}
				SetHTMLelementID(pHE, id, dwFlags, dwServer);
				if (pHPhint->GetFlags() & VDH_FLAGS_UPDATE)
				{
					CPoint pt = GetHTMLelementPosition(pHE, 0, &rc);
					if (IsHTMLElememtVisible(pt, rc))
					{
						InvalidateRectRgn(&rc);
//							TRACE(_T("Invalidate Input\n"));
					}
					else
					{
						pScrollToBeVisible = pHE;
					}
				}
			}
			else
			{
				while (pos)
				{
					pHE = (IHTMLElement*) m_Detectors.GetNext(pos);
					id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (   id == idInput && pHPhint->GetHostID() == HOSTID_OF(dwServer)
						|| dwLink != NULL)
					{
						dwFlags = HTML_ELEMENT_IS_DETECTOR;
						if (pHPhint->GetFlags() & VDH_FLAGS_DISCONNECT)
						{
							dwFlags |= HTML_ELEMENT_NOT_CONNECTED;
						}
						if (pRecI->GetIsActive())
						{
							dwFlags |= HTML_ELEMENT_ACTIVE|HTML_ELEMENT_BLINK;
						}
						if (pRecI->GetIsEnabled())
						{
							dwFlags |= HTML_ELEMENT_ENABLED;
						}
						if (pRecI->GetIsOkay())
						{
							dwFlags |= HTML_ELEMENT_OK;
						}
						SetHTMLelementID(pHE, id, dwFlags, dwServer);
						if (pHPhint->GetFlags() & VDH_FLAGS_UPDATE)
						{
							CPoint pt = GetHTMLelementPosition(pHE, 0, &rc);
							if (IsHTMLElememtVisible(pt, rc))
							{
								InvalidateRectRgn(&rc);
	//							TRACE(_T("Invalidate Input\n"));
							}
							else
							{
								pScrollToBeVisible = pHE;
							}
						}
						if (pHPhint->GetFlags() & VDH_FLAGS_SELECTED)
						{
							m_pHESelected = pHE;
						}
						break;
					}
				}
			}
		}
		else if (pHPhint->GetRecord()->IsKindOf(RUNTIME_CLASS(CIPCOutputRecord)))
		{
			CIPCOutputRecord *pRecO = (CIPCOutputRecord*)pHPhint->GetRecord();
			if (pRecO->IsCamera())
			{
				POSITION pos = m_Cameras.GetHeadPosition();
				IHTMLElement *pHE;
				CSecID id, idOutput = pRecO->GetID();
				while (pos)
				{
					pHE = (IHTMLElement*) m_Cameras.GetNext(pos);
					id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (id == idOutput && pHPhint->GetHostID() == HOSTID_OF(dwServer))
					{
						dwFlags = HTML_ELEMENT_IS_CAMERA;
						if (pHPhint->GetFlags() & VDH_FLAGS_DISCONNECT)
						{
							dwFlags |= HTML_ELEMENT_NOT_CONNECTED;
						}
						if (pRecO->CameraHasColor())
						{
							dwFlags |= HTML_ELEMENT_CAM_COLOR;
						}
						if (pRecO->IsReference())
						{
							dwFlags |= HTML_ELEMENT_CAM_REF;
						}
						if (pRecO->CameraIsFixed())
						{
							dwFlags |= HTML_ELEMENT_CAM_FIXED;
						}
						if (pRecO->GetFlags() & OREC_CAMERA_VISIBLE)
						{
							dwFlags |= HTML_ELEMENT_ENABLED;
						}
						if (pRecO->IsOkay())
						{
							dwFlags |= HTML_ELEMENT_OK;
						}
						SetHTMLelementID(pHE, id, dwFlags, dwServer);
						if (pHPhint->GetFlags() & VDH_FLAGS_UPDATE)
						{
							CPoint pt = GetHTMLelementPosition(pHE, 0, &rc);
							if (IsHTMLElememtVisible(pt, rc))
							{
								InvalidateRectRgn(&rc);
//								TRACE(_T("Invalidate Camera\n"));
							}
							else
							{
								pScrollToBeVisible = pHE;
							}
						}
						if (pHPhint->GetFlags() & VDH_FLAGS_SELECTED)
						{
							m_pHESelected = pHE;
						}
						break;
					}
				}
			}
			else if (pRecO->IsRelay())
			{
				POSITION pos = m_Relays.GetHeadPosition();
				IHTMLElement *pHE;
				CSecID id, idOutput = pRecO->GetID();
				while (pos)
				{
					pHE = (IHTMLElement*) m_Relays.GetNext(pos);
					id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (id == idOutput && pHPhint->GetHostID() == HOSTID_OF(dwServer))
					{
						dwFlags = HTML_ELEMENT_IS_RELAY;
						if (pHPhint->GetFlags() & VDH_FLAGS_DISCONNECT)
						{
							dwFlags |= HTML_ELEMENT_NOT_CONNECTED;
						}
						if (pRecO->RelayIsOpen())
						{
							dwFlags |= HTML_ELEMENT_OPEN;
						}
						if (pRecO->IsEnabled())
						{
							dwFlags |= HTML_ELEMENT_ENABLED;
						}
						if (pRecO->IsOkay())
						{
							dwFlags |= HTML_ELEMENT_OK;
						}
						SetHTMLelementID(pHE, id, dwFlags, dwServer);
						if (pHPhint->GetFlags() & VDH_FLAGS_UPDATE)
						{
							CPoint pt = GetHTMLelementPosition(pHE, 0, &rc);
							if (IsHTMLElememtVisible(pt, rc))
							{
								InvalidateRectRgn(&rc);
//								TRACE(_T("Invalidate Relay\n"));
							}
							else
							{
								pScrollToBeVisible = pHE;
							}
						}
						if (pHPhint->GetFlags() & VDH_FLAGS_SELECTED)
						{
							m_pHESelected = pHE;
						}
						break;
					}
				}
			}
		}
		else if (pHPhint->GetRecord()->IsKindOf(RUNTIME_CLASS(CIPCMediaRecord)))
		{
			CIPCMediaRecord*pRecM = (CIPCMediaRecord*)pHPhint->GetRecord();
			POSITION pos = m_Medias.GetHeadPosition();
			IHTMLElement *pHE;
			CSecID id, idMedia = pRecM->GetID();
			while (pos)
			{
				pHE = (IHTMLElement*) m_Medias.GetNext(pos);
				id = GetHTMLelementID(pHE, dwFlags, dwServer);
				if (id == idMedia && pHPhint->GetHostID() == HOSTID_OF(dwServer))
				{
					dwFlags = HTML_ELEMENT_IS_MEDIA;
					if (pRecM->IsAudio())
					{
						if (pRecM->IsOutput())
						{
							dwFlags = HTML_ELEMENT_IS_AUDIO_OUT;
							if (pRecM->IsRendering()) dwFlags |= HTML_ELEMENT_ACTIVE;
						}
						if (pRecM->IsInput())
						{
							dwFlags = HTML_ELEMENT_IS_AUDIO_IN;
							if (pRecM->IsCapturing()) dwFlags |= HTML_ELEMENT_ACTIVE;
						}
					}
					if (pRecM->IsEnabled()) dwFlags |= HTML_ELEMENT_ENABLED;

					if (pHPhint->GetFlags() & VDH_FLAGS_DISCONNECT)
					{
						dwFlags |= HTML_ELEMENT_NOT_CONNECTED;
					}

					SetHTMLelementID(pHE, id, dwFlags, dwServer);

					if (pHPhint->GetFlags() & VDH_FLAGS_UPDATE)
					{
						CPoint pt = GetHTMLelementPosition(pHE, 0, &rc);
						if (IsHTMLElememtVisible(pt, rc))
						{
							InvalidateRectRgn(&rc);
//								TRACE(_T("Invalidate Media\n"));
						}
						else
						{
							pScrollToBeVisible = pHE;
						}
					}
					if (pHPhint->GetFlags() & VDH_FLAGS_SELECTED)
					{
						m_pHESelected = pHE;
					}
				}
			}
		}
		else if (pHPhint->GetRecord()->IsKindOf(RUNTIME_CLASS(CHtmlElement)))
		{
			pScrollToBeVisible = ((CHtmlElement*)pHPhint->GetRecord())->GetHTMLElement();
			pt = ((CHtmlElement*)pHPhint->GetRecord())->GetPosition();
		}

		if (pHEold != m_pHESelected)
		{
			InvalidateElementRgn(pHEold);
			InvalidateElementRgn(m_pHESelected);
		}
	}

	if (m_pHTMLTextContainer)
	{
		if (pScrollToBeVisible)
		{
			IHTMLTextContainer * pSave = m_pHTMLTextContainer;
			m_pHTMLTextContainer = NULL;
			pt = GetHTMLelementPosition(pScrollToBeVisible, 0);
			m_pHTMLTextContainer = pSave;
		}
		ScrollToPosition(pt);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::ScrollToPosition(CPoint &pt)
{
	if (pt.x != -1)
	{
		CPoint ptScroll, ptCurrent,
					ptClientCenter = m_rcClient.CenterPoint();
		CSize  szScroll,
					szClient(m_rcClient.Width(), m_rcClient.Height());
		m_pHTMLTextContainer->get_scrollWidth(&szScroll.cx);
		m_pHTMLTextContainer->get_scrollHeight(&szScroll.cy);

		ptScroll.x = pt.x - ptClientCenter.x;
		ptScroll.y = pt.y - ptClientCenter.y;
		if (ptScroll.x + szClient.cx > szScroll.cx)
		{
			ptScroll.x = szScroll.cx-szClient.cx;
		}
		if (ptScroll.x < 0)
		{
			ptScroll.x = 0;
		}
		if (ptScroll.y + szClient.cy > szScroll.cy)
		{
			ptScroll.y = szScroll.cy-szClient.cy;
		}
		if (ptScroll.y < 0)
		{
			ptScroll.y = 0;
		}
		m_pHTMLTextContainer->put_scrollLeft(ptScroll.x);
		m_pHTMLTextContainer->put_scrollTop(ptScroll.y);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndHTML::UpdateHTMLElements()
{
	ReleaseHTMLelements();

	if (m_wServerID == CViewControllItems::GetLocalServerID())
	{
		// TODO! RKE: delete only local!!
		CViewControllItems::DeleteLocalHTMLmappings();
	}
#ifdef _DEBUG
	else
	{
		// Delete non local mappings
	}
#endif

	HRESULT hr;
	IDispatch *pDisp = NULL;
	IHTMLDocument2 *pDoc2 = NULL;
	try
	{
		pDisp = m_WebBrowser.GetDocument();
		if (!pDisp) HR_EXCEPTION(E_POINTER);
		CIdipClientDoc*pDoc = m_pViewIdipClient->GetDocument();
		if (!pDoc) HR_EXCEPTION(E_POINTER);

		HR_EXCEPTION(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2));
		IHTMLElementCollection *pColl = NULL;
		VARIANT var;
		IDispatch *pD;
		IHTMLElement *pHE;
		BSTR Text;
		long i, len;
		DWORD dwFlags, dwServer, dwUpdate;
		CSecID ServerID;
		VariantInit(&var);

		hr = pDoc2->get_all(&pColl);	// Zoom finden
		if (SUCCEEDED(hr))
		{
			var.vt = VT_I4;
			hr = pColl->get_length(&len);
			if (FAILED(hr)) len = 0;
			for (i=0; i<len; i++)
			{
				var.intVal = i;
				hr = pColl->item(var, var, &pD);
				if (SUCCEEDED(hr) &&  pD)
				{
					hr = pD->QueryInterface(IID_IHTMLElement, (void**)&pHE);
					if (SUCCEEDED(hr) &&  pHE)
					{
						int nFind = -1;
						pHE->get_tagName(&Text);
						if (Text && wcscmp(Text, HTML_META_TAG) == 0)
						{
							pHE->get_outerHTML(&Text);
							CString str(Text);
							str.MakeUpper();
							nFind = str.Find(HTML_META_NAME);
							if (nFind != -1)
							{
								int nContent = str.Find(HTML_META_CONTENT);
								if (str.Find(META_NAME_ZOOM, nFind) != -1)
								{
									if (nContent != -1)
									{
										m_nZoomValue = _ttoi(str.Mid(nContent+_tcslen(HTML_META_CONTENT)));
									}
									else
									{
										m_nZoomValue = 100;
									}
									m_nNewZoomValue = m_nZoomValue;
									pHE = NULL;
//										len = 0; // nicht weiter suchen
								}
								else if (str.Find(META_NAME_MAPTYPE, nFind) != -1)
								{
									if (nContent != -1)
									{
										m_nHTMLtype = _ttoi(str.Mid(nContent+_tcslen(HTML_META_CONTENT)));
									}
								}
								else if (str.Find(META_NAME_GENERATOR, nFind) != -1)
								{
									if (nContent != -1)
									{
										nContent += _tcslen(HTML_META_CONTENT);
										if (str.Find(_T("\""), nContent) != -1) nContent++;
										nFind = str.Find(_T("\""), nContent);
										if (nFind != -1)
										{
											m_sGenerator = str.Mid(nContent, nFind-nContent);
										}
									}
								}
							}
						}
						if (pHE) pHE->Release();
					}
					pD->Release();
				}
			}
			pColl->Release();
		}

		hr = pDoc2->get_body(&pHE);			// Textcontainer für Offset
		if (SUCCEEDED(hr) && pHE)
		{
			hr = pHE->QueryInterface(IID_IHTMLTextContainer, (void**)&m_pHTMLTextContainer);
			pHE->Release();
		}

		hr = pDoc2->get_images(&pColl);		// Map Image für Offset
		if (SUCCEEDED(hr) && pColl)
		{
			var.vt = VT_I4;
			hr = pColl->get_length(&len);
			if (FAILED(hr)) len = 0;
			for (i=0; i<len; i++)
			{
				var.intVal = i;
				hr = pColl->item(var, var, &pD);
				if (SUCCEEDED(hr) &&  pD)
				{
					pD->QueryInterface(IID_IHTMLElement, (void**)&pHE);
					if (SUCCEEDED(hr) &&  pHE)
					{
						int nFind = -1;
						hr = pHE->get_outerHTML(&Text);
						if (SUCCEEDED(hr) && Text)
						{
							CString str(Text);
							nFind = str.Find(HTML_USEMAP);
							if (nFind != -1)
							{
								GetHTMLelementPosition(pHE, 0, &m_rcMapImage);
								m_pMAPImage = pHE;
								pHE = NULL;
								nFind = str.Find(HTML_SRC);
								if (nFind != -1)
								{
									nFind += _tcslen(HTML_SRC)+1;
									int n = str.Find(_T("\""), nFind);
									if (n != -1)
									{
										m_sPictureSource = str.Mid(nFind, n-nFind);
									}
								}
							}
						}
						if (pHE) pHE->Release();
					}
					pD->Release();
				}
			}
			pColl->Release();
		}

		hr = pDoc2->get_links(&pColl);		// Links der Map
		if (SUCCEEDED(hr))
		{
			var.vt = VT_I4;
			hr = pColl->get_length(&len);
			if (FAILED(hr)) len = 0;
			for (i=0; i<len; i++)
			{
				ServerID.SetSubID(m_wServerID);
				var.intVal = i;
				hr = pColl->item(var, var, &pD);
				if (SUCCEEDED(hr) &&  pD)
				{
					dwServer = MAKELONG(m_wServerID, m_wHostID);
					hr = pD->QueryInterface(IID_IHTMLElement, (void**)&pHE);
					if (SUCCEEDED(hr) && pHE)
					{
						int nFind = -1;
						CString str;
						hr = pHE->get_outerHTML(&Text);
						if (SUCCEEDED(hr) && Text)
						{
							str = CString(Text);
							nFind = str.Find(HTML_LINK);
							if (nFind != -1)
							{
								str = str.Mid(nFind+6);
								nFind = str.Find(HTML_LINK_HOST);
								if (nFind != -1)
								{
									CSecID idTemp, id = GetHTMLelementID(pHE, dwFlags, dwServer);
									if (id.GetID() != SECID_NO_ID)	// neue Karte von IdipClient erstellt ?
									{
										str.Format(_T("%08x:"), id.GetID());// nach der ID suchen
										dwUpdate = SEARCH_BY_ID|SEARCH_IN_HOSTS;
										if (   !(m_dwChangeOptions & HTML_CHANGE_IN_PROGRESS) // keine Rekursion
												&& theApp.m_pPermission && theApp.m_pPermission->IsSuperVisor()) // Rechte
										{
											str += GetHTMLelementName(pHE);
											dwUpdate |= HTML_ELEMENT_CHECK_NAME; // Namen überprüfen
										}
										idTemp = pDoc->UpdateMapElement(SECID_NO_ID, str, dwUpdate);
									}
									else										// alte Karten
									{
										dwUpdate = 0;
										str = GetHTMLelementName(pHE);// anhand des Namens idntifizieren
										idTemp = pDoc->UpdateMapElement(SECID_NO_ID, str, dwUpdate);
									}
									dwFlags = HTML_ELEMENT_IS_HOST|HTML_ELEMENT_OK;
									if (idTemp != SECID_NO_ID)			// gültige ID
									{
										dwFlags |= HTML_ELEMENT_ACTIVE;
									}
									else
									{
									}

									if (dwUpdate & HTML_ELEMENT_DELETED)
									{
										dwFlags |= HTML_ELEMENT_DELETED;
										SetHTMLelementID(pHE, id, dwFlags, dwServer, TRUE);
										m_dwChangeOptions |= HTML_CHANGE_LINK_NAME;
									}
									else if (dwUpdate & HTML_ELEMENT_NAME_CHANGED)
									{
										BSTR bstr;
										dwFlags |= HTML_ELEMENT_NAME_CHANGED;
										SetHTMLelementID(pHE, id, dwFlags, dwServer, TRUE);
										bstr = str.AllocSysString();
										VERIFY(SUCCEEDED(pHE->put_title(bstr)));
										m_dwChangeOptions |= HTML_CHANGE_LINK_NAME;
									}
									SetHTMLelementID(pHE, id, dwFlags, dwServer, TRUE);
									m_Hosts.AddTail(pHE);
									pHE = NULL;
								}
								else
								{
									CPtrList *pList = NULL;
									DWORD dwSearch = 0;
									nFind = str.Find(HTML_LINK_CAMERA);
									if (nFind != -1)
									{
										pList = &m_Cameras;
										dwSearch |= SEARCH_IN_CAMERAS;
									}
									else
									{
										nFind = str.Find(HTML_LINK_RELAY);
										if (nFind != -1)
										{
											pList = &m_Relays;
											dwSearch |= SEARCH_IN_RELAYS;
										}
										else
										{
											nFind = str.Find(HTML_LINK_DETECTOR);
											if (nFind != -1)
											{
												pList = &m_Detectors;
												dwSearch |= SEARCH_IN_INPUT;
											}
											else 
											{
												nFind = str.Find(HTML_LINK_MEDIA);
												if (nFind != -1)
												{
													pList = &m_Medias;
													dwSearch |= SEARCH_IN_MEDIA;
												}
												else
												{
													nFind = str.Find(HTML_LINK_FILE);
													if (nFind != -1)
													{
														CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
														dwFlags = HTML_ELEMENT_IS_LINK|HTML_ELEMENT_OK;
														SetHTMLelementID(pHE, id, dwFlags, dwServer, TRUE);
														m_Hosts.AddTail(pHE);

														CString sFile = str.Left(str.GetLength()-2);
														CheckPathURL(sFile);
														CWndHTML *pWnd = new CWndHTML(m_pServer, sFile, m_ID);
														if (DoesFileExist(pWnd->m_sURL))
														{
															CRect rc(0,0,0,0);
															pWnd->m_sHome = pWnd->m_sURL;
															pWnd->m_sURL.Empty();

															pWnd->Create(rc, m_ID.GetID(), m_pViewIdipClient);
															CWnd*pOwner = GetOwner();
															DWORD dwLink = (DWORD)pHE;
 															if (    pOwner == NULL 
																|| !pOwner->IsKindOf(RUNTIME_CLASS(CWndHTML)))
															{
																m_LinkDetectors.SetAt((DWORD)pWnd, dwLink);
																pOwner = this;
															}
															else if (pOwner->IsKindOf(RUNTIME_CLASS(CWndHTML)))
															{
																((CWndHTML*)pOwner)->m_LinkDetectors.SetAt((DWORD)pWnd, dwLink);
															}
															pWnd->SetOwner(pOwner);
															pWnd->PostMessage(WM_COMMAND, ID_HTML_HOME);
														}
														else
														{
															WK_DELETE(pWnd);
														}
														pHE = NULL;
													}
												}
											}
										}
									}
									if (pList)
									{
										CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
										if (id.GetID() != SECID_NO_ID)// neue Karten (mit IdipClient erstellt)
										{
											if (HOSTID_OF(dwServer)  != SECSUBID_LOCAL_HOST) 
											{									// Verweist der Link auf eine andere Station
												if (m_wServerID == CViewControllItems::GetLocalServerID()) 
												{								// ist es eine lokale Karte
																				// dann muss diese ID zusätzlich mitgeführt werden
													ServerID.SetGroupID(HOSTID_OF(dwServer));
													CViewControllItems::AddLocalHTMLmapping(MAKELONG(0, HOSTID_OF(dwServer)), id);
												}
												else							// auf einer externen Karte
												{
													CHost* pHost=NULL;
#ifdef _DEBUG
													//TODO! RKE: activate at 5.1
													BSTR bstr;
													if (SUCCEEDED(pHE->get_title(&bstr)))
													{
														CString sHost(bstr);
														int i = sHost.Find(_T("@"));
														if (i != -1)
														{
															sHost = sHost.Mid(i+1);
															CHostArray& ha = theApp.GetHosts();
															for (i=0;i<ha.GetSize();i++)
															{
																if (0==sHost.CompareNoCase(ha.GetAtFast(i)->GetName()))
																{
																	pHost = ha.GetAtFast(i);
																	break;
																}
															}
														}															
													}
#endif
													if (pHost)
													{
														// Hostnamen gefunden: Identifizierung anhand der HostID möglich
														ServerID.SetGroupID(pHost->GetID().GetSubID());
														dwServer = MAKELONG(m_wServerID, pHost->GetID().GetSubID());
														SetHTMLelementID(pHE, id, dwFlags, dwServer, TRUE);
														CViewControllItems::AddLocalHTMLmapping(dwServer, id);
													}
													else
													{
														id = SECID_NO_ID;		// Element nicht einfügen, da die HostIDs nicht identisch sind
													}
												}
											}
											else								// Verweist der Link auf die lokale Station (HostID)
											{
												if (m_wServerID != CViewControllItems::GetLocalServerID())	
												{								// ist es eine externe Karte
																				// muss die HostID mitgeführt werden
													dwServer = MAKELONG(LOWORD(dwServer), m_wHostID);
													SetHTMLelementID(pHE, id, dwFlags, dwServer, TRUE);
												}								// da die Elemente immer zur ServerID gehören
												ServerID.SetGroupID(SECID_NO_GROUPID);
											}
											str.Format(_T("%08x:"), id.GetID());
											dwSearch |= SEARCH_BY_ID;
										}
										else									// alte Karten sollen auch eingefügt werden
										{
											dwFlags = 0;
											str = GetHTMLelementName(pHE);
											dwUpdate = dwSearch;
											id  = pDoc->UpdateMapElement(ServerID, str, dwUpdate);
											if (id != SECID_NO_ID)
											{
												m_dwChangeOptions |= HTML_CHANGE_OLD_TYPE;
											}
											else
											{
												m_dwChangeOptions |= HTML_CHANGE_MISSING_ID;
											}
										}
										
										if (id.GetID() != SECID_NO_ID)
										{
											pList->AddTail(pHE);
											if (dwFlags == 0)
											{
												SetHTMLelementID(pHE, id, 0, dwServer, TRUE);
											}
											dwUpdate = HTML_ELEMENT_UPDATE_STATE|dwSearch;
											if (   m_wHostID == SECSUBID_LOCAL_HOST// nur lokale Page
													&& !(m_dwChangeOptions & HTML_CHANGE_IN_PROGRESS) // keine Rekursion
													&& theApp.m_pPermission && theApp.m_pPermission->IsSuperVisor()) // Rechte
											{
												str += GetHTMLelementName(pHE);	// Namen überprüfen
												dwUpdate |= HTML_ELEMENT_CHECK_NAME;
											}
											CSecID idTemp = pDoc->UpdateMapElement(ServerID, str, dwUpdate);;
											if (idTemp.GetID() == SECID_NO_ID)
											{
												DWORD dwTemp = dwFlags&HTML_ELEMENT_TYPE_FLAGS;
												switch(dwTemp)
												{
													case HTML_ELEMENT_IS_CAMERA:
														dwTemp |= HTML_ELEMENT_CAM_FIXED;
														break;
												}
												SetHTMLelementID(pHE, id, dwTemp|HTML_ELEMENT_NOT_CONNECTED, dwServer, TRUE);
											}
											else
											{
												if (dwUpdate & HTML_ELEMENT_NAME_CHANGED)
												{
													BSTR bstr;
													dwFlags |= HTML_ELEMENT_NAME_CHANGED;
													SetHTMLelementID(pHE, id, dwFlags, dwServer, TRUE);
													bstr = str.AllocSysString();
													VERIFY(SUCCEEDED(pHE->put_title(bstr)));
													m_dwChangeOptions |= HTML_CHANGE_LINK_NAME;
												}
											}
											pHE = NULL;
										}
										else
										{
											id = GetHTMLelementID(pHE, dwFlags, dwServer);
											SetHTMLelementID(pHE, id, HTML_ELEMENT_NOT_CONNECTED, dwServer, TRUE);
										}
									}
								}
							}
						}
						if (pHE) pHE->Release();
					}
					pD->Release();
				}
			}
			pColl->Release();
		}

		DWORD dwShow = 0;
		if (m_Hosts.GetCount()    ) dwShow |= HTML_ELEMENT_SHOW_HOSTS;
		if (m_Cameras.GetCount()  ) dwShow |= HTML_ELEMENT_SHOW_CAMERAS;
		if (m_Relays.GetCount()   ) dwShow |= HTML_ELEMENT_SHOW_RELAIS;
		if (m_Medias.GetCount()   ) dwShow |= HTML_ELEMENT_SHOW_MEDIAS;
		if (m_Detectors.GetCount()) dwShow |= HTML_ELEMENT_SHOW_DETECTORS;

		CWnd*pOwner = GetOwner();
		if (   pOwner
			&& pOwner->IsKindOf(RUNTIME_CLASS(CWndHTML)))
		{
			DWORD dwLink = 0;
			CWndHTML *pWnd = (CWndHTML*)pOwner;
			if (   pWnd->m_LinkDetectors.Lookup((DWORD)this, dwLink)
				&& dwLink)
			{
				POSITION pos = m_Detectors.GetHeadPosition();
				DWORD dwFlags, dwServer;
				while (pos)
				{
					CSecID id = GetHTMLelementID((IHTMLElement*) m_Detectors.GetNext(pos), dwFlags, dwServer);
					pWnd->m_LinkDetectors.SetAt(id.GetID(), dwLink);
				}
			}
			pWnd->m_LinkDetectors.RemoveKey((DWORD)this);
			PostMessage(WM_USER, HTML_DESTROY_AND_DELETE);
		}

		if (m_dwShow != dwShow)		// wenn es sich aendert, alle anzeigen
		{
			m_dwShow = dwShow;
		}
	}
	catch (ErrorStruct *ps)
	{
		if (FAILED(ps->m_hr))
		{
			if (theApp.m_ComDSDll.GetModuleHandle())
			{
				void (WINAPIV*pGetErrorStrings)(HRESULT, CString&, CString&) = (void (WINAPIV*)(HRESULT, CString&, CString&)) GetProcAddress(theApp.m_ComDSDll.GetModuleHandle(), GET_ERROR_STRING);
				if (pGetErrorStrings)
				{
					CString str, sError, sDescription;
					pGetErrorStrings(ps->m_hr, sError, sDescription);
					str.Format(_T("%s(%d): DirectShow Error %s(%x)\n%s\n"), ps->m_sFile, ps->m_nLine, sError, ps->m_hr, sDescription);
					WK_TRACE(str);
				}
			}
		}
	}

	RELEASE_OBJECT(pDoc2);
	RELEASE_OBJECT(pDisp);

	if (m_hwndIEServer == NULL)
	{
		if (!theApp.IsNT40())
		{
			EnumChildWindows(m_hWnd, FindIEServer, (LPARAM)&m_hwndIEServer);			
			if (m_hwndIEServer)
			{
				long lRes = GetWindowLong(m_hwndIEServer, GWL_ID);
				if (lRes == 0)
				{
					SetWindowLong(m_hwndIEServer, GWL_ID, (long)this);
					m_lIEServerWndProc = SetWindowLong(m_hwndIEServer, GWL_WNDPROC, (long)CWndHTML::IEServerwndProc);
				}
				CalculateClientRect();
			}
		}
	}

	if (!(m_dwChangeOptions & HTML_CHANGE_IN_PROGRESS))
	{
		if (m_dwChangeOptions & HTML_CHANGE_OLD_TYPE && !(m_dwChangeOptions & HTML_CHANGE_MISSING_ID))
		{
			m_dwChangeOptions |= HTML_CHANGE_IN_PROGRESS;
			PostMessage(WM_USER, HTML_CHANGE_OLD_TYPE);
		}
		else if (m_dwChangeOptions & HTML_CHANGE_LINK_NAME)
		{
			m_dwChangeOptions |= HTML_CHANGE_IN_PROGRESS;
			PostMessage(WM_USER, HTML_CHANGE_LINK_NAME);
		}
	}
	::InvalidateRect(m_hwndIEServer, NULL, TRUE);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnFileSaveHtmlText(CSize &szImage) 
{
	CString sHTMLName = m_sURL;
	if (m_WebBrowser.m_hWnd)
	{
		CString sURL;
		sURL = ClearStringFromProcentASCII(m_WebBrowser.GetLocationURL());
		CheckPathURL(sURL);
		if (sHTMLName.CompareNoCase(sURL) != 0)
		{
			sHTMLName = sURL;
		}
	}
	if (sHTMLName.IsEmpty())
	{
		if (m_nHTMLtype == HTML_TYPE_HOST_MAP)
		{
			sHTMLName = theApp.GetMapURL();
		}
		else if (m_nHTMLtype == HTML_TYPE_CAMERA_MAP)
		{
			sHTMLName.Format(_T("%s\\%08lx\\%s"), CNotificationMessage::GetWWWRoot(), SECID_LOCAL_HOST, MAIN_CAMERAMAP_FILE_NAME);
		}
	}

	if (!sHTMLName.IsEmpty())
	{
		CWK_String sText;
		CString sID,
		        sFormat(HTML_ELEMENT_ID),
		        sPath = sHTMLName;
		int     nFind  = sPath.ReverseFind('\\');


		CRect   rc(0,0,0,0);
		CPoint  pt, ptOffset;// = GetHTMLelementPosition(NULL, 0, &rc);

		if (nFind != -1)
		{
			sPath = sPath.Left(nFind+1);
		}
		if (szImage.cx == 0)
		{
			CDlgHTMLmap dlg;
			dlg.m_sPictureFileName = m_sPictureSource;
			dlg.m_sPath = sPath;
			dlg.CheckPictureFile();
			szImage = dlg.m_szPicture;
			if (szImage.cx == 0)
			{
				CString s;
				AfxFormatString1(s, AFX_IDP_ARCH_BADINDEX, m_sPictureSource);
				AfxMessageBox(s);
				return;
			}
		}

		CSize szIcons(HTML_IMG_SIZE_X, HTML_IMG_SIZE_Y);
		if (m_nNewZoomValue != 100)
		{
			szImage.cx = MulDiv(szImage.cx, m_nNewZoomValue, 100);
			szImage.cy = MulDiv(szImage.cy, m_nNewZoomValue, 100);
		}
		
		ptOffset.x  = MulDiv(szIcons.cx, 1, 2);
		ptOffset.y  = MulDiv(szIcons.cy, 1, 2);
	
		POSITION pos;
		CFile file (sHTMLName, CFile::modeCreate|CFile::modeWrite);

		sText.Format(HTML_TEXT_TOP, GetHTMLCodePage(),theApp.m_pszExeName, m_nHTMLtype, m_nNewZoomValue, m_sHTMLTitle);
		sText.Write(file);

		for (pos = m_EditLinks.GetHeadPosition(); pos != NULL; )
		{
			CHTML_Link*pLink = m_EditLinks.GetNext(pos);
			pLink->dwFlags &= ~HTML_ELEMENT_NAME_CHANGED;
			sID.Format(sFormat, pLink->secID.GetID(), pLink->dwFlags, pLink->dwServer);
			if (m_nNewZoomValue != m_nZoomValue)
			{
				pt = pLink->rcShape.TopLeft() + ptOffset;
				pt.x = MulDiv(pt.x, m_nNewZoomValue, m_nZoomValue);
				pt.y = MulDiv(pt.y, m_nNewZoomValue, m_nZoomValue);
				pt -= ptOffset;
				pLink->rcShape = CRect(pt, szIcons);
			}
			sText.Format(HTML_TEXT_LINK, pLink->sType, pLink->sName,
				pLink->rcShape.left, pLink->rcShape.top, pLink->rcShape.right, pLink->rcShape.bottom,
				pLink->sInfo, sID);
			sText.Write(file);
		}
		sText.Format(HTML_TEXT_BOTTOM, m_sPictureSource, szImage.cx, szImage.cy);
		sText.Write(file);
		m_nZoomValue = m_nNewZoomValue;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::OnEditHtmlPage() 
{
	m_pSelected = NULL;
	m_pHESelected = NULL;
	if (m_bEditPage)												// edit mode
	{
		CSize szImage(0,0);
		OnFileSaveHtmlText(szImage);								// Datei speichern
		m_EditLinks.DeleteAll();									// alle Links löschen, da jetzt wieder HTML
		m_bEditPage = false;										// auf False setzen
		if (m_uUpdateTimer)
		{
			KillTimer(m_uUpdateTimer);
		}
		m_uUpdateTimer = SetTimer(UPDATE_HTML_WND_TIMER, 500, NULL);
		OnViewRefresh();											// Updaten (m_bEditPage muss hier false sein)
		RedrawWindow();
		m_dwChangeOptions = 0;
		InsertKnownHostsToCameraView(FALSE);
	}
	else															// normal mode
	{
		CPtrList *pList[] = {&m_Hosts, &m_Cameras, &m_Detectors, &m_Relays, &m_Medias};
		IHTMLElement *pHE;
		BSTR Text;
		CHTML_Link *pLink;
		int i;
		
		for (i=0; i<5; i++)											// alle Elemente
		{
			while (pList[i]->GetCount())
			{
				pHE = (IHTMLElement*)pList[i]->RemoveHead();
				pHE->get_outerHTML(&Text);
				CString str(Text);
				pLink = new CHTML_Link();

				pLink->sName = GetHTMLelementName(pHE, &pLink->nItem, &pLink->sType);
				pLink->secID = GetHTMLelementID(pHE, pLink->dwFlags, pLink->dwServer);
				if (pLink->dwFlags & HTML_ELEMENT_NAME_CHANGED)
				{
					BSTR bstr;
					if (SUCCEEDED(pHE->get_title(&bstr)))
					{
						pLink->sName = bstr;
						pLink->sInfo = pLink->sName;
					}
				}
				else if (pLink->dwFlags & HTML_ELEMENT_DELETED)
				{
					delete pLink;
					continue;
				}
				else
				{
					if (SUCCEEDED(pHE->get_title(&Text)))
					{
						pLink->sInfo = Text;
					}
				}
				m_EditLinks.AddTail(pLink);							// in den edit container einfügen
																	// Attribute lesen
				pHE->get_offsetLeft(&pLink->rcShape.left);
				pHE->get_offsetTop(&pLink->rcShape.top);
				pHE->get_offsetWidth(&pLink->rcShape.right);
				pHE->get_offsetHeight(&pLink->rcShape.bottom);
				pLink->rcShape.right  = pLink->rcShape.left + pLink->rcShape.right;
				pLink->rcShape.bottom =  pLink->rcShape.top + pLink->rcShape.bottom;
				
				pLink->dwFlags = pLink->dwFlags&(HTML_ELEMENT_TYPE_FLAGS|HTML_ELEMENT_NAME_CHANGED);
				if (pLink->nItem == -1)
				{
					if (HE_TYPE(pLink->dwFlags) == HTML_ELEMENT_IS_AUDIO_IN)
					{
						pLink->nItem = HTML_IMG_MICROPHONE;
					}
					else if (HE_TYPE(pLink->dwFlags) == HTML_ELEMENT_IS_AUDIO_OUT)
					{
						pLink->nItem = HTML_IMG_LOUDSPEAKER;
					}
				}
				pHE->Release();
			}
		}

		if (m_uUpdateTimer)
		{
			KillTimer(m_uUpdateTimer);
		}
		m_uUpdateTimer = SetTimer(UPDATE_HTML_WND_TIMER, 100, NULL);
		::InvalidateRect(m_hwndIEServer, NULL, FALSE);
		m_bEditPage = true;
		RedrawWindow();
		PostMessage(WM_USER, HTML_INSERT_KNOWN_HOSTS, 1);
	}
	
	CViewCamera* pViewCamera = theApp.GetMainFrame()->GetViewCamera();
	if (pViewCamera)
	{
		if (m_bEditPage)
		{
			pViewCamera->ModifyStyle(TVS_DISABLEDRAGDROP, 0);
		}
		else
		{
			pViewCamera->ModifyStyle(0, TVS_DISABLEDRAGDROP);
		}
	}

	return m_bEditPage ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateEditHtmlPage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnEditHtmlLinkProperties() 
{
	if (m_pSelected)
	{
		CDlgHTMLlink dlg;
		CHTML_Link *pSel = m_pSelected;
		dlg.m_sHTMLlinkInfo = pSel->sInfo;
		if (dlg.DoModal() == IDOK)
		{
			pSel->sInfo = dlg.m_sHTMLlinkInfo;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateEditHtmlLinkProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pSelected != NULL);
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bEditPage)
	{
		if (m_rcClient.PtInRect(point))
		{
			LPCTSTR pstr = NULL;
			HitEditLink(point, &m_pSelected);
			if (m_pSelected)
			{
				pstr = m_pSelected->sName;
				CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
				if (pDoc)
				{
					CString sID;
					CSecID ServerID(HOSTID_OF(m_pSelected->dwServer), m_wServerID);
					sID.Format(_T("%08x"), m_pSelected->secID.GetID());
					DWORD dwUpdate = HTML_ELEMENT_SELECT|SEARCH_IN_ALL|SEARCH_BY_ID;
					pDoc->UpdateMapElement(ServerID, sID, dwUpdate);
				}
				
			}
			AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, 0, (LPARAM)pstr);
			::SetCapture(m_hwndIEServer);
			return m_bEditPage;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		CWndSmall::OnLButtonDown(nFlags, point);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::OnMouseMove(UINT nFlags, CPoint pt)
{
	if (m_bEditPage)
	{
		if (nFlags & MK_LBUTTON)
		{
			if (m_pSelected)
			{
				if (m_rcClient.PtInRect(pt))
				{
					CRect rc(0,0,0,0);
					IHTMLTextContainer *pOld = m_pHTMLTextContainer;
					m_pHTMLTextContainer     = NULL;
					CPoint ptOffset          = GetHTMLelementPosition(NULL, 0, &rc);
					CPoint point             = pt + ptOffset;
					m_pHTMLTextContainer     = pOld;
					if (m_bDragging)
					{
						gm_Images.DragMove(point);
						if (m_rcMapImage.PtInRect(point))
						{
							SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
						}
						else
						{
							SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
						}
					}
					else
					{
						CPoint pt(21, 3);
						if (m_wndToolBar.GetSize().cy <= 0)
						{
							pt.y += TOOLBAR_HEIGHT;
						}
						if (gm_Images.BeginDrag(m_pSelected->nItem, pt))
						{
							gm_Images.DragEnter(this, point);
							m_bDragging = TRUE;
						}
					}
					if (m_ptDiff.x != 0 || m_ptDiff.y !=0)
					{
						m_ptDiff = CPoint(0, 0);
						gm_Images.DragShowNolock(TRUE);
						::InvalidateRect(m_hwndIEServer, NULL, FALSE);
					}
					return TRUE;
				}
				else
				{
					HCURSOR hCursor = CheckDragScroll(&pt);
					if (hCursor)
					{
						SetCursor(hCursor);
						gm_Images.DragShowNolock(FALSE);
					}
					else
					{
						gm_Images.DragShowNolock(TRUE);
					}
				}
			}
		}
	}
	CWndSmall::OnMouseMove(nFlags, pt);
	return m_bEditPage;
}
//////////////////////////////////////////////////////////////////////////
UINT CWndHTML::OnNcHitTest(CPoint point)
{
	if (m_bEditPage)
	{
		CRect       rc(0,0,0,0);
		GetHTMLelementPosition(m_pMAPImage, 0, &rc);
		rc.InflateRect(GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));
		m_WebBrowser.ScreenToClient(&point);
		if (rc.PtInRect(point))
		{
			return HTCLIENT;
		}
		else
		{
			return (UINT)HTERROR;
		}
	}
	else
	{
/*		UINT uHitTest = CWndSmall::OnNcHitTest(point);
		TRACE(_T("HitTest %u"), uHitTest);
		return uHitTest;
*/		return CWndSmall::OnNcHitTest(point);
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bEditPage)
	{
		if (nHitTest == HTCLIENT)
		{
			CPoint point;
			GetCursorPos(&point);
			m_WebBrowser.ScreenToClient(&point);
			CHTML_Link *pOld = m_pToolTip;
			HitEditLink(point, &m_pToolTip, false);
			if (m_pToolTip)
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));

				if (pOld != m_pToolTip)
				{
					m_uToolTipTimer = SetTimer(TOOLTIP_EDIT_TIMER, 500, NULL);
				}
			}
			else
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

				if (m_uToolTipTimer)
				{
					KillTimer(m_uToolTipTimer);
					m_uToolTipTimer = 0;
				}
				MSG msg = {m_hWnd, WM_MOUSEMOVE, 0, 0, 0, {0,0}};
				m_nFlags |= WF_TRACKINGTOOLTIPS;
				FilterToolTipMessage(&msg);
				m_nFlags &= ~WF_TRACKINGTOOLTIPS;
			}

		}
	}
	else
	{
		CWndSmall::OnSetCursor(pWnd, nHitTest, message);
	}
	return m_bEditPage;
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL bReturn = FALSE;
	if (m_bEditPage)
	{
		if (m_bDragging)
		{
			gm_Images.DragLeave(this);
			gm_Images.EndDrag();
			ASSERT(m_pSelected != NULL);
			if (m_bDragging & 2)
			{
				if (m_pSelected)
				{
					m_pSelected->rcShape = SetLinkShapeRect(point);
				}
				::InvalidateRect(m_hwndIEServer, NULL, FALSE);
			}
			else if (m_pSelected)
			{
				InvalidateLinkRgn(m_pSelected);
				m_pSelected->rcShape = SetLinkShapeRect(point);
				InvalidateLinkRgn(m_pSelected);
			}
			m_bDragging = FALSE;
		}
		ReleaseCapture();
		m_ptDiff = CPoint(0, 0);
		bReturn = TRUE;
	}
	else
	{
//		CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
		CViewCamera* pViewCamera = theApp.GetMainFrame()->GetViewCamera();
		if (pViewCamera)
		{
			IHTMLElement *pHE = FindElementFromPoint(point, SEARCH_IN_ALL);
			if (pHE)
			{
				DWORD   dwFlags, dwServer;
				CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);
				bReturn = TRUE;
				switch (HE_TYPE(dwFlags))
				{
				   case HTML_ELEMENT_IS_CAMERA:
					{
						OnSelectCamera(pHE);
					}break;
					case HTML_ELEMENT_IS_DETECTOR:
					{
						OnSelectDetector(pHE);
					}break;
					case HTML_ELEMENT_IS_RELAY:
					{
						OnSelectRelay(pHE);
					}break;
					case HTML_ELEMENT_IS_AUDIO_IN:
					case HTML_ELEMENT_IS_AUDIO_OUT:
					{
						OnSelectMedia(pHE);
					}break;
					case HTML_ELEMENT_IS_HOST:
					{
						OnSelectHost(pHE);
					}break;
					case HTML_ELEMENT_IS_LINK:
					{
						BSTR Text;
						ASSERT(pHE != NULL);
						pHE->toString(&Text);
						NavigateTo(ClearStringFromProcentASCII(CString(Text)));
					}break;
					default:
					{
						bReturn = FALSE;
						AfxMessageBox(IDS_INVALID_HTML_ELEMENT, MB_OK|MB_ICONINFORMATION);
					}break;
				}
			}
			else
			{

				HRESULT hr;
				IDispatch *pDisp = m_WebBrowser.GetDocument();
				if (pDisp)
				{
					IHTMLDocument2 *pDoc2 = NULL;
					hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2);
					if (SUCCEEDED(hr) &&  pDoc2)
					{
						IHTMLElement *pHE;
						pDoc2->elementFromPoint(point.x, point.y, &pHE);
						if (pHE)
						{
							DWORD   dwFlags, dwServer;
							CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);
							if (id != SECID_NO_ID &&  dwFlags & HTML_ELEMENT_NOT_CONNECTED)
							{
								bReturn = TRUE;
							}
							pHE->Release();
						}
						pDoc2->Release();
					}
					pDisp->Release();
				}
			}
			if (m_pHESelected != pHE)
			{
				InvalidateElementRgn(pHE);
				InvalidateElementRgn(m_pHESelected);
				m_pHESelected = pHE;
			}
		}
	}
	CWndSmall::OnLButtonUp(nFlags, point);
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::ExtractNameFromOuterHTML(const CString& sHTML, CString&sName)
{
	int nFind = sHTML.Find(_T("://"));
	if (nFind != -1)
	{
		nFind += 3;
		int nEnd = sHTML.Find(_T("\">"), nFind);
		sName = ClearStringFromProcentASCII(sHTML.Mid(nFind, nEnd-nFind));
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
CString CWndHTML::GetHTMLelementName(IHTMLElement*pHE, int *pnType, CString *psType)
{
	BSTR Text;
	ASSERT(pHE != NULL);
	pHE->toString(&Text);
	CString sName = ClearStringFromProcentASCII(CString(Text));
	int nFind = sName.Find(_T("://"));

	if (nFind != -1)
	{
		nFind += 3;
		if (pnType)
		{
			CString sType = sName.Left(nFind);
			if (sType.CompareNoCase(HTML_LINK_CAMERA) == 0)
			{
				*pnType = HTML_IMG_CAM;
			}
			else if (sType.CompareNoCase(HTML_LINK_DETECTOR) == 0)
			{
				*pnType = HTML_IMG_DETECTOR;
			}
			else if (sType.CompareNoCase(HTML_LINK_RELAY) == 0)
			{
				*pnType = HTML_IMG_RELAY_OPEN;
			}
			else if (sType.CompareNoCase(HTML_LINK_HOST) == 0)
			{
				*pnType = HTML_IMG_HOST;
			}
			else if (sType.CompareNoCase(HTML_LINK_FILE) == 0)
			{
				*pnType = HTML_IMG_SUB_LINK;
			}
			else if (sType.CompareNoCase(HTML_LINK_MEDIA) == 0)
			{
				*pnType = -1;
			}
			if (psType)
			{
				*psType = sType.Left(nFind-3);
			}
		}
		int nLen = sName.GetLength();
		if (pnType && *pnType == HTML_IMG_SUB_LINK && sName[nFind] == _T('/'))
		{
			nFind++;
		}
		sName = sName.Mid(nFind, nLen - nFind);
		nLen = sName.GetLength();
		if (sName.GetAt(nLen-1) == '/')
		{
			sName = sName.Left(nLen - 1);
		}
	}
	return sName;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::InsertElement(CSecID id, CString sName, WORD wServer, WORD wHost, CObject*pObj)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_WebBrowser.ScreenToClient(&pt);
	if (   m_bEditPage 
		 && m_rcClient.PtInRect(pt) 
		 && m_EditLinks.Contains(id, MAKELONG(wServer, wHost)) == NULL)
	{
		CHTML_LinkPtr pLink = new CHTML_Link();						// erstellen
		m_EditLinks.AddTail(pLink);										// hinzufügen

		pLink->rcShape = SetLinkShapeRect(pt);

		if (HTML_TYPE_HOST_MAP == m_nHTMLtype)
		{
			pLink->secID = id;
			pLink->sName = sName;
			pLink->sInfo = sName;
			pLink->nItem = HTML_IMG_HOST;
			pLink->sType = HTML_LINK_HOST;
			pLink->dwServer = MAKELONG(wServer, wHost);
			pLink->dwFlags  = HTML_ELEMENT_IS_HOST;
		}
		else
		{			
			pLink->secID  = id;
			if (wServer != m_wServerID)
			{
				pLink->sName.Format(_T("%s@%04x"), sName, wHost);
				pLink->sInfo  = sName;
			}
			else
			{
				pLink->sName  = sName;
				pLink->sInfo  = sName;
			}
			pLink->dwServer = MAKELONG(wServer, wHost);


			CServer *pServer = m_pViewIdipClient->GetDocument()->GetServer(wServer);
			if (pServer)
			{
				if (wHost != SECSUBID_LOCAL_HOST && HTML_TYPE_CAMERA_MAP == m_nHTMLtype)
				{
					pLink->sInfo += _T("@") + pServer->GetName();
				}
				if (id.IsInputID())
				{
					pLink->nItem   = HTML_IMG_DETECTOR;
					pLink->sType   = HTML_LINK_DETECTOR;
					pLink->dwFlags = HTML_ELEMENT_IS_DETECTOR;
				}
				else if (id.IsOutputID())
				{
					const CIPCOutputRecord &outputRecord = pServer->GetOutput()->GetOutputRecordFromSecID(id);
					if (outputRecord.IsCamera())
					{
						pLink->nItem   = HTML_IMG_CAM;
						pLink->sType   = HTML_LINK_CAMERA;
						pLink->dwFlags = HTML_ELEMENT_IS_CAMERA;
					}
					else if (outputRecord.IsRelay())
					{
						pLink->nItem   = HTML_IMG_RELAY_OPEN;
						pLink->sType   = HTML_LINK_RELAY;
						pLink->dwFlags = HTML_ELEMENT_IS_RELAY;
					}
				}
				else if (id.IsMediaID())
				{
					if (pObj && pObj->IsKindOf(RUNTIME_CLASS(CIPCMediaRecord)))
					{
						pLink->sType   = HTML_LINK_MEDIA;
						CIPCMediaRecord *pMR = (CIPCMediaRecord*)pObj;
						if (pMR->IsAudio())
						{
							if     (pMR->IsInput() )
							{
								pLink->nItem   = HTML_IMG_MICROPHONE;
								pLink->dwFlags = HTML_ELEMENT_IS_AUDIO_IN;
							}
							else if(pMR->IsOutput())
							{
								pLink->nItem   = HTML_IMG_LOUDSPEAKER;
								pLink->dwFlags = HTML_ELEMENT_IS_AUDIO_OUT;
							}
						}
					}
				}
			}
		}
		int nFind = pLink->sType.Find(_T(":"));
		if (nFind != -1)
		{
			pLink->sType = pLink->sType.Left(nFind);
		}
		if (m_bDragging & 2)
		{
			::InvalidateRgn(m_hwndIEServer, NULL, FALSE);
		}
		else
		{
			InvalidateLinkRgn(pLink);
		}
	}
	m_ptDiff = CPoint(0, 0);
	m_bDragging = FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnEditClear()
{
	// delete selected item/link
	POSITION pos = m_EditLinks.Find(m_pSelected);
	if (pos)
	{
		if (m_pSelected->sType == _T("file"))
		{
			CString sFileName, sPath;
			if (m_pSelected->sName[1] != _T(':'))
			{
				WK_SplitPath(m_sURL, sPath, sFileName);
				sFileName = sPath + m_pSelected->sName;
			}
			else
			{
				sFileName = m_pSelected->sName;
			}
			if (DoesFileExist(sFileName))
			{
				CWndHTML wnd(SECID_NO_ID, sFileName);
				CRect rc(0,0,0,0);
				wnd.Create(rc, (UINT)-1, m_pViewIdipClient);
				wnd.m_sHTMLTitle = m_pSelected->sInfo;
				wnd.m_bMap = true;
				wnd.OnEditClearMap();
				if (!wnd.m_sURL.IsEmpty())
				{
					return;
				}
			}
		}

		m_EditLinks.RemoveAt(pos);
		InvalidateLinkRgn(m_pSelected);
		WK_DELETE(m_pSelected);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pSelected != NULL);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnEditHtmlMapProperties()
{
	ASSERT(m_bEditPage);
	CDlgHTMLmap dlg;
	dlg.m_sTitle = m_sHTMLTitle;
	dlg.m_sPictureFileName = m_sPictureSource;
	CString sPath, sFilename;
	WK_SplitPath(m_sURL, sPath, sFilename);
	dlg.m_sPath = sPath;
	if (dlg.m_sPath.IsEmpty() && m_nHTMLtype == HTML_TYPE_HOST_MAP)
	{
		dlg.m_sPath.Format(_T("%s\\"),CNotificationMessage::GetWWWRoot());
	}

	dlg.m_nZoomValue = m_nNewZoomValue;

	if (dlg.DoModal() == IDOK)
	{
		dlg.CheckCopyPictureFile();
		m_sHTMLTitle     = dlg.m_sTitle;
		m_sPictureSource = dlg.m_sPictureFileName;
		m_nNewZoomValue  = dlg.m_nZoomValue;
		OnEditHtmlPage();
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateEditHtmlMapProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnEditClearMap() 
{
	if (IsMapWindow())
	{
		CString sMsg;
		sMsg.Format(IDP_DELETE_HTML_MAP, m_sHTMLTitle);
		if (AfxMessageBox(sMsg, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) == IDYES)
		{
			int iHtmlType = GetHTMLwndType();
			m_bEditPage = false;
			CString sURL;
			sURL = ClearStringFromProcentASCII(m_WebBrowser.GetLocationURL());
			CheckPathURL(sURL);
			if (DoesFileExist(sURL))
			{
				TRY
				{
					CString sPath, sFile, sPictureSource;
					WK_SplitPath(sURL, sPath, sFile);
					POSITION pos;
					IHTMLElement *pHE;
					for (pos = m_Hosts.GetHeadPosition(); pos != NULL; )
					{
						pHE = (IHTMLElement*)m_Hosts.GetNext(pos);
						sFile = GetHTMLelementName(pHE);
						sFile = HTML_LINK_FILE + sFile;
						CheckPathURL(sFile);
						if (sFile[1] != _T(':'))
						{
							sFile = sPath + sFile;
						}
						if (DoesFileExist(sFile))
						{
							CWndHTML wnd(SECID_NO_ID, sFile);
							CRect rc(0,0,0,0);
							BSTR Text;
							wnd.Create(rc, (UINT)-1, m_pViewIdipClient);
							if (SUCCEEDED(pHE->get_title(&Text)))
							{
								wnd.m_sHTMLTitle = Text;
							}
							wnd.m_bMap = true;
							wnd.OnEditClearMap();
							if (!wnd.m_sURL.IsEmpty())
							{
								return;
							}
						}
					}
					sPictureSource = sPath + m_sPictureSource;
					if (DoesFileExist(sPictureSource))
					{
						SetFileAttributes(sPictureSource, GetFileAttributes(sPictureSource)& ~FILE_ATTRIBUTE_READONLY);
						CFile::Remove(sPictureSource);
					}
					SetFileAttributes(sURL, GetFileAttributes(sURL)& ~FILE_ATTRIBUTE_READONLY);
					CFile::Remove(sURL);
					m_sURL.Empty();
					if (m_ID != SECID_NO_ID)
					{
						OnSmallClose();
					}
				}
				CATCH( CFileException, e )
				{
					WK_TRACE_ERROR(_T("File %s cannot be removed\n"), m_sURL);
				}
				END_CATCH
			}
			else if (iHtmlType == HTML_TYPE_HOST_MAP)
			{
				if (theApp.DeleteMap())
				{
					OnSmallClose();
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("CWndHTML::OnEditClearMap() UNKNOWN type %i\n"), iHtmlType);
			}
		}
		theApp.CheckHostMap();
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateEditClearMap(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	if (HTML_TYPE_CAMERA_MAP == m_nHTMLtype)
	{
		// only allow for local server
		if (SECSUBID_LOCAL_HOST == m_wHostID)
		{
			bEnable = !m_bEditPage;
		}
	}
	else if (HTML_TYPE_HOST_MAP == m_nHTMLtype)
	{ // it must be a host map
		bEnable = !m_bEditPage;
	}
	pCmdUI->Enable(bEnable);
}
//////////////////////////////////////////////////////////////////////////
IHTMLElement* CWndHTML::FindElementByName(CString &sFind, DWORD dwSearch)
{
	POSITION pos;
	IHTMLElement *pHE;
	if (dwSearch & SEARCH_IN_CAMERAS)
	{
		for (pos = m_Cameras.GetHeadPosition(); pos != NULL; )
		{
			pHE = (IHTMLElement*)m_Cameras.GetNext(pos);
			if (GetHTMLelementName(pHE) == sFind)
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_INPUT)
	{
		for (pos = m_Detectors.GetHeadPosition(); pos != NULL; )
		{
			pHE = (IHTMLElement*)m_Detectors.GetNext(pos);
			if (GetHTMLelementName(pHE) == sFind)
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_RELAYS)
	{
		for (pos = m_Relays.GetHeadPosition(); pos != NULL; )
		{
			pHE = (IHTMLElement*)m_Relays.GetNext(pos);
			if (GetHTMLelementName(pHE) == sFind)
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_MEDIA)
	{
		for (pos = m_Medias.GetHeadPosition(); pos != NULL; )
		{
			pHE = (IHTMLElement*)m_Medias.GetNext(pos);
			if (GetHTMLelementName(pHE) == sFind)
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_HOSTS)
	{
		for (pos = m_Hosts.GetHeadPosition(); pos != NULL; )
		{
			pHE = (IHTMLElement*)m_Hosts.GetNext(pos);
			if (GetHTMLelementName(pHE) == sFind)
			{
				return pHE;
			}
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CWndHTML
 Function   : FindElementFromPoint
 Description: Finds an already known HtmlElement (Link) from point

 Parameters:   
  pt      : (E): Client coordinates   (CPoint)
  dwSearch: (E): Search option  (DWORD)
					SEARCH_IN_INPUT, SEARCH_IN_CAMERAS, SEARCH_IN_RELAYS, SEARCH_IN_MEDIA, SEARCH_IN_HOSTS, SEARCH_IN_ALL

 Result type:  (IHTMLElement*)
 created: January, 09 2004
 <TITLE FindElementFromPoint>
*********************************************************************************************/
IHTMLElement* CWndHTML::FindElementFromPoint(CPoint pt, DWORD dwSearch)
{
	POSITION pos;
	CRect rc;
	if (dwSearch & SEARCH_IN_CAMERAS)
	{
		for (pos = m_Cameras.GetHeadPosition(); pos != NULL; )
		{
			IHTMLElement *pHE = (IHTMLElement*)m_Cameras.GetNext(pos);
			GetHTMLelementPosition(pHE, 0, &rc);
			if (rc.PtInRect(pt))
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_INPUT)
	{
		for (pos = m_Detectors.GetHeadPosition(); pos != NULL; )
		{
			IHTMLElement *pHE = (IHTMLElement*)m_Detectors.GetNext(pos);
			GetHTMLelementPosition(pHE, 0, &rc);
			if (rc.PtInRect(pt))
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_RELAYS)
	{
		for (pos = m_Relays.GetHeadPosition(); pos != NULL; )
		{
			IHTMLElement *pHE = (IHTMLElement*)m_Relays.GetNext(pos);
			GetHTMLelementPosition(pHE, 0, &rc);
			if (rc.PtInRect(pt))
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_MEDIA)
	{
		for (pos = m_Medias.GetHeadPosition(); pos != NULL; )
		{
			IHTMLElement *pHE = (IHTMLElement*)m_Medias.GetNext(pos);
			GetHTMLelementPosition(pHE, 0, &rc);
			if (rc.PtInRect(pt))
			{
				return pHE;
			}
		}
	}
	if (dwSearch & SEARCH_IN_HOSTS)
	{
		for (pos = m_Hosts.GetHeadPosition(); pos != NULL; )
		{
			IHTMLElement *pHE = (IHTMLElement*)m_Hosts.GetNext(pos);
			GetHTMLelementPosition(pHE, 0, &rc);
			if (rc.PtInRect(pt))
			{
				return pHE;
			}
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::InvalidateRectRgn(CRect*prc)
{
	if (prc)
	{
		CRgn  rgn;
//		TRACE(_T("InvalidateRect(%d, %d, %d, %d)\n"), prc->left, prc->top, prc->right, prc->bottom);
		rgn.CreateRectRgn(prc->left, prc->top, prc->right, prc->bottom);
		::InvalidateRgn(m_hwndIEServer, rgn, FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::InvalidateElementRgn(IHTMLElement*pHE)
{
	if (pHE)
	{
		CRect rc;
		CRgn  rgn;
		GetHTMLelementPosition(pHE, 0, &rc);
		InvalidateRectRgn(&rc);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::InvalidateLinkRgn(CHTML_Link*pLink)
{
	if (pLink)
	{
		CRect rc(0,0,0,0);
		CPoint ptOffset = GetHTMLelementPosition(NULL, 0, &rc);
		rc = pLink->rcShape;
		rc += ptOffset;

		InvalidateRectRgn(&rc);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnHtmlSave() 
{
	if (m_bEditPage)
	{
		OnEditHtmlPage();
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateHtmlSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnSmallContext() 
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnRButtonDown(0, pt);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateSmallContext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);//!m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnRequestSave()
{
	if (m_bEditPage)
	{
		CString str;
		AfxFormatString1(str, AFX_IDP_ASK_TO_SAVE, m_sHTMLTitle);
		if (AfxMessageBox(str, MB_YESNO) == IDYES)
		{
			CSize sz(0,0);
			OnEditHtmlPage(); // Saves it, because we are in Edit Mode
		}
	}
}
//////////////////////////////////////////////////////////////////////////
CRect CWndHTML::SetLinkShapeRect(CPoint ptClient)
{
	CRect rc(0,0,0,0);
	CPoint ptOffset = GetHTMLelementPosition(NULL, 0, &rc);
	CSize szIcons(HTML_IMG_SIZE_X, HTML_IMG_SIZE_Y);
	ptClient -= ptOffset;
	ptClient -= CPoint(szIcons.cx/2, szIcons.cy/2);
	BOOL bChanged = FALSE;
	if (ptClient.x < 0)
	{
		ptClient.x = 0;
		bChanged   = 1;
	}
	if (ptClient.y < 0)
	{
		ptClient.y = 0;
		bChanged  |= 2;
	}
	rc = CRect(ptClient, szIcons);

	if (bChanged != 3)
	{
		if (m_pMAPImage)
		{
			m_pMAPImage->get_offsetTop(&ptOffset.y);
			m_pMAPImage->get_offsetLeft(&ptOffset.x);
		}

		CPoint ptDiff(0, 0);
		if (rc.right  + ptOffset.x > m_rcMapImage.right ) ptDiff.x = m_rcMapImage.right  - rc.right  - ptOffset.x;
		if (rc.bottom + ptOffset.y > m_rcMapImage.bottom) ptDiff.y = m_rcMapImage.bottom - rc.bottom - ptOffset.y;
		if (ptDiff.x || ptDiff.y)
		{
			rc.OffsetRect(ptDiff);
		}
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////
HCURSOR CWndHTML::CheckDragScroll(CPoint* pptClient)
{
	DWORD  dwCursor = 0;
	HCURSOR hCursor = NULL;
	CPoint ptDiff = CPoint(0, 0);
	CPoint ptDiffTL = *pptClient - m_rcClient.TopLeft() ;
	CPoint ptDiffBR = *pptClient - m_rcClient.BottomRight();
	if (ptDiffTL.x < 0)
	{
		ptDiff.x = ptDiffTL.x;
		dwCursor  = 0x01;
	}
	if (ptDiffTL.y < 0)
	{
		ptDiff.y = ptDiffTL.y;
		dwCursor  |= 0x02;
	}
	if (ptDiffBR.x > 0)
	{
		ptDiff.x = ptDiffBR.x;
		dwCursor  |= 0x04;
	}
	if (ptDiffBR.y > 0)
	{
		ptDiff.y = ptDiffBR.y;
		dwCursor  |= 0x08;
	}

	switch (dwCursor)
	{
		case  1: hCursor = theApp.m_hPanLeft;		break;
		case  4: hCursor = theApp.m_hPanRight;		break;
		case  2: hCursor = theApp.m_hTiltUp;		break;
		case  8: hCursor = theApp.m_hTiltDown;		break;
		case  3: hCursor = theApp.m_hLeftTop;		break;
		case  6: hCursor = theApp.m_hRightTop;		break;
		case 12: hCursor = theApp.m_hRightBottom;	break;
		case  9: hCursor = theApp.m_hLeftBottom;	break;
	}

	if (m_ptDiff.x == 0 && m_ptDiff.y == 0)
	{
//		::InvalidateRect(m_hwndIEServer, NULL, FALSE);
	}

	m_ptDiff = ptDiff;
	return hCursor;
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::DraggedInto(CPoint& pt)
{
	if (m_wndToolBar.GetSize().cy > 0)
	{
		pt.y -= m_wndToolBar.GetSize().cy;
	}
	BOOL bInside = m_rcClient.PtInRect(pt);
	if (bInside)
	{
		m_ptDiff = CPoint(0,0);
		if (m_pSelected)
		{
			InvalidateLinkRgn(m_pSelected);
			m_pSelected = NULL;
		}
	}
	return bInside;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::HitEditLink(CPoint point, CHTML_Link**ppLink, bool bUpdate)
{
	CRect       rc(0,0,0,0);
	CPoint      ptOffset = GetHTMLelementPosition(NULL, 0, &rc);
	CHTML_Link *pOld     = *ppLink;
	point -= ptOffset;
	*ppLink = m_EditLinks.HitLink(point);

	if (bUpdate && *ppLink != pOld)
	{
		InvalidateLinkRgn(pOld);
		InvalidateLinkRgn(*ppLink);
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::IsHTMLElememtVisible(const CPoint& point, const CRect& rect)
{
	BOOL bInside = m_rcClient.PtInRect(point);
	if (bInside)
	{
		CPoint ptBottomRight = point;
		ptBottomRight.x += rect.Width();
		ptBottomRight.y += rect.Height();
		bInside = m_rcClient.PtInRect(ptBottomRight);
	}
	return bInside;
}
//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
int CWndHTML::OnToolHitTest( CPoint point, TOOLINFOW* pTI ) const
#else
int CWndHTML::OnToolHitTest( CPoint point, TOOLINFOA* pTI ) const
#endif
{
	if (m_pToolTip)
	{
		pTI->lpszText = (LPTSTR)LPCTSTR(m_pToolTip->sInfo);
		pTI->hinst    = AfxGetInstanceHandle();
		pTI->uId      = (UINT)m_pToolTip->secID.GetID();
		pTI->hwnd     = m_hWnd;
		pTI->uFlags  |= TTF_ALWAYSTIP;//|TTF_ABSOLUTE|TTF_TRACK ;
		return 1;
	}
	else
	{
		CancelToolTips();
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnConnectHost() 
{
	if (m_pHESelected)
	{
		OnSelectHost(m_pHESelected);
	}
	
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateConnectHost(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pHESelected	!= NULL);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (nFlags & MK_IESERVERWND)
	{
		if (!m_bEditPage)
		{
			// Check, if double click was really in client area
			if (m_rcClient.PtInRect(point))
			{
				IHTMLElement* pHE = FindElementFromPoint(point, SEARCH_IN_ALL);
				if (pHE)
				{
					// found element should be the selected one (at first click)
					if (pHE == m_pHESelected)
					{
						DWORD   dwFlags, dwServer;
						CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);
						if (HE_TYPE(dwFlags) == HTML_ELEMENT_IS_RELAY)
						{
							AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_RELAIS_TOGGLE);
						}
						if (   HE_TYPE(dwFlags) == HTML_ELEMENT_IS_DETECTOR
							 && dwFlags & HTML_ELEMENT_BLINK)
						{
							AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_INPUT_CONFIRM);
						}
					}
					else
					{
						// Why not? Debug info
						WK_TRACE_WARNING(_T("CWndHTML::OnLButtonDblClk() pHE(%s) !=  m_pHESelected(%s)\n"),
							GetHTMLelementName(pHE), m_pHESelected ? GetHTMLelementName(m_pHESelected):_T("none"));
					}
				}
			}
			else
			{
//				TRACE(_T("CWndHTML::OnLButtonDblClk() point not in client rect\n"));
			}
		}
	}
	else
	{
		CWndSmall::OnLButtonDblClk(nFlags, point);
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::CalculateClientRect()
{
	if (m_hwndIEServer)
	{
		::GetClientRect(m_hwndIEServer, &m_rcClient);
		// calculate offset of map from HTML page
		CRect rc(0,0,0,0);
		IHTMLTextContainer *pOld = m_pHTMLTextContainer;
		m_pHTMLTextContainer     = NULL;
		CPoint ptOffset          = GetHTMLelementPosition(NULL, 0, &rc);
		m_pHTMLTextContainer     = pOld;
		// calculate client size, may be scroll bars?
		// smaller width results in a horizontal scroll bar
		if ((m_rcMapImage.Width() + ptOffset.x) > m_rcClient.Width())
		{
			m_rcClient.bottom -= GetSystemMetrics(SM_CYHSCROLL);
		}
		// smaller height results in a vertical scroll bar
		// vertical scrollbar is always visible, only disabled
	//				if ((m_rcMapImage.Height() + ptOffset.y) > m_rcClient.Height())
		{
			m_rcClient.right  -= GetSystemMetrics(SM_CXVSCROLL);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
CString CWndHTML::GetHTMLCodePage()
{
	CString str;
#ifdef _UNICODE
	CStringArray ar;
	if (GetCodePageInfo(CWK_String::GetCodePage(), ar))
	{
/*
		for (int i=OEM_CP_FLAGS; i<=OEM_CP_GDICHARSET; i++)
		{
			WK_TRACE(_T("%s|"), ar.GetAt(i));
		}
*/
		str = ar.GetAt(OEM_CP_WEBCHARSET);
		return str;
	}
#endif
	str.Format(_T("windows-%d"), CWK_String::GetCodePage());
	return str;
}
//////////////////////////////////////////////////////////////////////////
CString CWndHTML::GetHTMLCodePageMetaTag()
{
	CString sMetaTag;
	sMetaTag.Format(HTML_CODEPAGE_METATAG, GetHTMLCodePage());
	return sMetaTag;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnHtmlNewPage()
{
	if (m_bEditPage)
	{
		CDlgHTMLmap dlg;
		CString sPath, sFileName, sTemp;
		int i;
		WK_SplitPath(m_sURL, sPath, sFileName);
		i = sFileName.Find(_T("."));
		if (i != -1)
		{
			sFileName = sFileName.Left(i);
		}
		for (i=1; i<100; i++)
		{
			sTemp.Format(_T("%s%s_%d.htm"), sPath, sFileName, i);
			if (!DoesFileExist(sTemp))
			{
				WK_SplitPath(sTemp, sPath, sFileName);
				break;
			}
		}
		dlg.m_sPath = sPath;
		dlg.m_sTitle = m_sHTMLTitle;
		if (dlg.DoModal() == IDOK)
		{
			dlg.CheckCopyPictureFile();
			dlg.m_sPath += sFileName;
			CWndHTML wnd(theApp.GetNextHtmlWindowID(), dlg.m_sPath, dlg.m_sTitle, dlg.m_sPictureFileName, dlg.m_nZoomValue, HTML_TYPE_CAMERA_MAP);
			wnd.m_nHTMLtype = m_nHTMLtype;
			wnd.OnFileSaveHtmlText(dlg.m_szPicture);

			CHTML_LinkPtr pLink = new CHTML_Link();						// erstellen
			m_EditLinks.AddTail(pLink);									// hinzufügen

			pLink->rcShape = SetLinkShapeRect(m_ptLastHit);
			pLink->secID = SECID_NO_ID;
			pLink->sName = sFileName;
//			pLink->sName = dlg.m_sPath;
			pLink->sInfo = dlg.m_sTitle;
			pLink->nItem = HTML_IMG_SUB_LINK;
			pLink->sType = HTML_LINK_FILE;
			pLink->sType = pLink->sType.Left(pLink->sType.GetLength()-3);
			pLink->dwServer = 0;
			pLink->dwFlags  = HTML_ELEMENT_IS_LINK;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnUpdateHtmlNewPage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::InsertKnownHostsToCameraView(BOOL bInsert)
{
	if (CIdipClientApp::GetIntValue(_T("InsertKnownHost"), TRUE) && GetHTMLwndType() == HTML_TYPE_HOST_MAP)
	{
		CViewCamera*pView = theApp.GetMainFrame()->GetViewCamera();
		if (pView)
		{
			if (bInsert)
			{
				if (pView->GetVisibilityState() == OTVS_Never)
				{
					pView->SetVisibility(OTVS_InitiallyNo);
				}
				if (!pView->IsWindowVisible())
				{
					pView->ToggleVisibility();
				}
			}
			BOOL bChanged = pView->InsertKnownHosts(bInsert);
			if (bChanged && !bInsert)
			{
				pView->CheckVisibility();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::CanPrint()
{
#ifdef _DEBUG
	return !m_bMap;
#else
	return FALSE;
#endif
}
//////////////////////////////////////////////////////////////////////////
BOOL CWndHTML::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->m_pPD->m_pd.Flags |= PD_RETURNDC;
	BOOL b = m_pViewIdipClient->DoPreparePrinting(pInfo);

	if (b)
	{
	}
	return b;
}
//////////////////////////////////////////////////////////////////////////
void CWndHTML::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO! RKE: so einfach ist es nicht
//	::SendMessage(m_hwndIEServer, WM_PRINTCLIENT, (WPARAM)pDC->m_hDC, PRF_CLIENT);
//	m_WebBrowser.PrintClient(pDC, PRF_CLIENT);
}
