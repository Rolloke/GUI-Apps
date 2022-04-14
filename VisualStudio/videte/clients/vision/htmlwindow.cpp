// HtmlWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"

#include "MainFrm.h"

#include "HtmlWindow.h"
#include "VisionDoc.h"
#include "VisionView.h"
#include "ObjectView.h"
#include "HTMLmapProperties.h"
#include "HTMLlinkProperties.h"

#include <mshtml.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
#define HTML_IMG_DETECTOR				0
#define HTML_IMG_DETECTOR_ALARM0		1
#define HTML_IMG_DETECTOR_ALARM1    2
#define HTML_IMG_DETECTOR_DISABLED	3
#define HTML_IMG_CAM						4
#define HTML_IMG_CAM_MD					5
#define HTML_IMG_CAM_RGB				6
#define HTML_IMG_CAM_PTZ				7
#define HTML_IMG_ACTIVE					8
#define HTML_IMG_RELAY_CLOSED			9
#define HTML_IMG_RELAY_OPEN			10
#define HTML_IMG_HOST					11
#define HTML_IMG_HOST_CONNECTED		12
#define HTML_IMG_DISABLED		      13
#define HTML_IMG_SELECTED		      14
#define HTML_IMG_NOT_CONNECTED      15
#define HTML_IMG_MARKED		         16
#define HTML_IMG_MICROPHONE         17
#define HTML_IMG_LOUDSPEAKER        18

#define HTML_IMG_SIZE_X			      27
#define HTML_IMG_SIZE_Y			      27

#define HTML_SHOW_STATES				_T("HTMLShowStates")
#define HTML_LINK							_T("href=")
#define HTML_COORDS						_T("coords=")
#define HTML_ALT							_T("alt=")
#define HTML_SRC							_T("src=")
#define HTML_USEMAP						_T("useMap=")
#define HTML_LINK_HOST					_T("host://")
#define HTML_LINK_CAMERA				_T("cam://")
#define HTML_LINK_RELAY					_T("relay://")
#define HTML_LINK_DETECTOR				_T("detector://")
#define HTML_LINK_MEDIA 				_T("media://")

#define HTML_ELEMENT_ID					L"%08x:%08x:%08x"	// ID, Flags, Server/Host
#define HTML_META_TAG               L"META"
#define HTML_META_NAME              _T("NAME=")
#define HTML_META_CONTENT           _T("CONTENT=")
#define META_NAME_ZOOM              _T("ZOOM")
#define META_NAME_MAPTYPE           _T("MAPTYPE")
#define META_NAME_GENERATOR         _T("GENERATOR")

#define UPDATE_HTML_WND_TIMER			0x0815
#define UPDATE_HTML_WND_TIMER_EX		0x0816
#define TOOLTIP_EDIT_TIMER		      0x0817

#define HTML_CHANGE_OLD_TYPE        0x00000001
#define HTML_CHANGE_MISSING_ID      0x00000002
#define HTML_CHANGE_LINK_NAME       0x00000004
#define HTML_CHANGE_IN_PROGRESS     0x00000008

#define MK_IESERVERWND              0x10000000

#define HTML_TEXT_TOP    _T("<html>\r\n")  \
                         _T(" <head>\r\n") \
                         _T("  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-%s\">\r\n") \
                         _T("  <meta name=\"GENERATOR\" content=\"Videte Vision\">\r\n") \
                         _T("  <meta name=\"MAPTYPE\" content=\"%d\">\r\n") \
                         _T("  <meta name=\"ZOOM\" content=\"%d\">\r\n") \
                         _T("  <title>%s</title>\r\n") \
                         _T(" </head>\r\n") \
                         _T(" <body>\r\n") \
                         _T("  <map name=\"FPMap0\">\r\n")
#define HTML_TEXT_LINK   _T("   <area href=\"%s://%s\" shape=\"rect\" coords=\"%d, %d, %d, %d\" title=\"%s\" id=\"%s\">\r\n")
#define HTML_TEXT_BOTTOM _T("  </map>\r\n") \
                         _T("   <img border=\"0\" src=\"%s\" usemap=\"#FPMap0\" width=\"%d\" height=\"%d\">\r\n") \
                         _T(" </body>\r\n") \
                         _T("</html>\r\n")

/////////////////////////////////////////////////////////////////////////////
#define RELEASE_OBJECT(pObject)																\
{																										\
	if (pObject) {																					\
		int nR = pObject->Release();															\
		if (nR == 0) TRACE(_T("%s(%d):%s::Release(%d)\n"), __FILE__, __LINE__, #pObject, nR);	\
		pObject = NULL;																			\
	}																									\
}																
/////////////////////////////////////////////////////////////////////////////
static TCHAR szFile[] = _T("file://");

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
IMPLEMENT_DYNCREATE(CHtmlPageHint, CObject)

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
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow
void CHtmlWindow::Init()
{
	m_pVisionView = NULL;
	m_bForward = FALSE;
	m_bBack = FALSE;
	m_stTimeStamp.GetLocalTime();
	m_dwMonitor = USE_MONITOR_2;

#ifdef _HTML_ELEMENTS
	m_dwShow             = 0;
	m_hwndIEServer       = NULL;
	m_pHTMLTextContainer = NULL;
	m_pMAPImage          = NULL;

	m_pHESelected			= NULL;
	
	m_nZoomValue         = 100;
	m_nNewZoomValue      = 100;
	m_nHTMLtype          = 0;
	m_bEditPage          = FALSE;
	m_pSelected          = NULL;
	m_pToolTip           = NULL;
	m_uToolTipTimer      = 0;
	m_bDragging          = FALSE;
	m_lIEServerWndProc   = 0;
	m_ptDiff.x           = 0;
	m_ptDiff.y           = 0;
	m_dwChangeOptions    = 0;
#endif
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow::CHtmlWindow(const CSecID id, CString &sURL)
	: CSmallWindow(NULL)
{
	m_bMap = TRUE;
	m_ID = id;
	m_sURL = sURL; 
	Init();
}
//////////////////////////////////////////////////////////////////////////
CHtmlWindow::CHtmlWindow(const CSecID id, CString &sURL, CString &sTitle, CString &sPicture, int nZoom, int nType)
{
	Init();
	m_ID = id;
	m_sURL           = sURL; 
	m_sHTMLTitle     = sTitle;
#ifdef _HTML_ELEMENTS
	m_sPictureSource = sPicture;
	m_nZoomValue     = nZoom;
	m_nNewZoomValue  = nZoom;
	m_nHTMLtype      = nType;
#endif
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow::CHtmlWindow(CServer* pServer, const CString& sURL, const CSecID id)
	: CSmallWindow(pServer)
{
	Init();
	m_ID = id;
	m_bMap = FALSE;
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

	if (0==sURL.CompareNoCase(_T("camera.htm")))
	{
		m_bMap = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow::CHtmlWindow(CServer* pServer, const CSecID id, const CString& sName)
	: CSmallWindow(pServer)
{
	m_bMap = FALSE;
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
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::SetServer(CServer *pServer)
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
CHtmlWindow::~CHtmlWindow()
{
#ifdef _HTML_ELEMENTS
	ReleaseHTMLelements();
#endif
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::IsURL(const CString& sURL)
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
BEGIN_MESSAGE_MAP(CHtmlWindow, CSmallWindow)
	//{{AFX_MSG_MAP(CHtmlWindow)
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
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
#ifdef _HTML_ELEMENTS
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
	ON_COMMAND(ID_VIEW_RELAIS, OnViewRelais)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELAIS, OnUpdateViewRelais)
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
#endif
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHtmlWindow, CWnd)
	ON_EVENT(CHtmlWindow, AFX_IDW_PANE_FIRST, 102 /* StatusTextChange */, OnStatusTextChange, VTS_BSTR)
	ON_EVENT(CHtmlWindow, AFX_IDW_PANE_FIRST, 105 /* CommandStateChange */, OnCommandStateChange, VTS_I4 VTS_BOOL)
	ON_EVENT(CHtmlWindow, AFX_IDW_PANE_FIRST, 113 /* TitleChange */, OnTitleChange, VTS_BSTR)
	ON_EVENT(CHtmlWindow, AFX_IDW_PANE_FIRST, 112 /* PropertyChange */, OnPropertyChange, VTS_BSTR)
	ON_EVENT(CHtmlWindow, AFX_IDW_PANE_FIRST, 251 /* NewWindow2 */, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CHtmlWindow, AFX_IDW_PANE_FIRST, 253 /* OnQuit */, OnQuit, VTS_NONE)
	ON_EVENT(CHtmlWindow, AFX_IDW_PANE_FIRST, 250 /* BeforeNavigate2 */, BeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::IsHtmlWindow()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSecID CHtmlWindow::GetID()
{
	return m_ID;
}
/////////////////////////////////////////////////////////////////////////////
CString CHtmlWindow::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime;

	if (IsMapWindow())
	{
		sTitle.LoadString(IDS_MAP);
		sTitle += _T(" ") + m_sHTMLTitle;
#ifdef _HTML_ELEMENTS
		if (m_bEditPage)
		{
			sTime.LoadString(IDS_HTML_EDIT_MODE);
			sTitle += sTime;
		}
#endif
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
		if (size.cx > (rect.Width()-m_ToolBarSize.cx))
		{
			sTitle = m_sName + sTime;
		}
	}
	else
	{
		sFormat.LoadString(IDS_NOTIFICATION_FROM);
		sTitle.Format(sFormat,GetServerName(),sTime);
	}

	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow message handlers
/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow event reflectors
/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::Create(const RECT& rect, UINT nID, CVisionView* pParentWnd)
{
	BOOL bRet;
	m_pVisionView = pParentWnd;
	bRet = CWnd::Create(NULL,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						nID); 

	if (bRet)// && m_pServer)
	{
		NavigateTo(m_sURL);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _HTML_ELEMENTS
LRESULT CALLBACK CHtmlWindow::HTMLwndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
BOOL CALLBACK CHtmlWindow::FindIEServer(HWND hwnd, LPARAM lParam)
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
LRESULT CALLBACK CHtmlWindow::IEServerwndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CHtmlWindow *pThis = (CHtmlWindow*) GetWindowLong(hwnd, GWL_ID);
	BOOL bEditPage = FALSE;
	switch (uMsg)
	{
		case WM_MOUSEMOVE:
		{
			pThis->OnMouseMove(wParam|MK_IESERVERWND, lParam);
			bEditPage = pThis->m_bEditPage;
		}break;
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
	if (uMsg == WM_PAINT)
	{
		pThis->PostMessage(WM_TIMER, UPDATE_HTML_WND_TIMER_EX, 0);
	}

	return lResult;
}
#endif
/////////////////////////////////////////////////////////////////////////////
int CHtmlWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
#ifdef _HTML_ELEMENTS
	m_Images.Create(IDB_HTML_IMG,HTML_IMG_SIZE_X, 1, RGB(0,255,255));
#endif
	
	if (CSmallWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

/*	CString sBuffer;
	GetModuleFileName(theApp.m_hInstance, sBuffer.GetBuffer(MAX_PATH), MAX_PATH);
	sBuffer = sBuffer.Left(sBuffer.ReverseFind('\\'));
*/
	CRect rect;
	GetClientRect(rect);
	m_WebBrowser.Create(_T("VisionBrowser"),WS_VISIBLE|WS_CHILD,
						rect,this,AFX_IDW_PANE_FIRST);

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_HTML))
	{
		TRACE(_T("Failed to create display toolbar\n"));
		return -1;      // fail to create
	}

	DWORD dwStyle = m_wndToolBar.GetBarStyle();
	dwStyle &= ~(CBRS_BORDER_RIGHT|CBRS_BORDER_TOP|CBRS_BORDER_LEFT|CBRS_BORDER_BOTTOM);
	dwStyle |= CBRS_TOOLTIPS | CBRS_SIZE_FIXED;
	m_wndToolBar.SetBarStyle(dwStyle);
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);
	CSize sizeButton(14,14);
	CSize sizeImage(12,12);
	m_wndToolBar.SetBorders(1,1,1,1);
	m_wndToolBar.SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(sizeImage.cx, sizeImage.cy));
	m_wndToolBar.SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(sizeButton.cx, sizeButton.cy));
	m_ToolBarSize.cx = m_wndToolBar.GetToolBarCtrl().GetButtonCount()*(sizeButton.cx+4)+4;
	m_ToolBarSize.cy = sizeButton.cy+6;

#ifdef _HTML_ELEMENTS
/*
	// in dieser Subclass-Funktion wird zur Zeit nichts abgefangen RKE
	if (theApp.IsWinXP())
	{
		SetWindowLong(m_WebBrowser.m_hWnd, GWL_USERDATA, SetWindowLong(m_WebBrowser.m_hWnd, GWL_WNDPROC, (long)CHtmlWindow::HTMLwndProc));
	}
*/	
	CWK_Profile wkp;
	m_dwShow    = wkp.GetInt(GetSection(), HTML_SHOW_STATES, HTML_ELEMENT_SHOW_ALL);
	m_dwMonitor = wkp.GetInt(GetSection(), USE_MONITOR, 0);

	SetTimer(UPDATE_HTML_WND_TIMER, 500, NULL);
#endif

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnDestroy() 
{
#ifdef _HTML_ELEMENTS
	if (m_nHTMLtype == HTML_TYPE_HOST_MAP || m_nHTMLtype == HTML_TYPE_CAMERA_MAP)
	{
		CWK_Profile wkp;
		wkp.WriteInt(GetSection(), HTML_SHOW_STATES, m_dwShow);
		wkp.WriteInt(GetSection(), USE_MONITOR, m_dwMonitor);
	}
#endif
	CSmallWindow::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
CString CHtmlWindow::GetSection()
{
	CString sSection;
	DWORD  dwServer = MAKELONG(m_wServerID, SECID_GROUP_HOST);
	if (!m_sURL.IsEmpty())
	{
		int nFind = m_sURL.Find(_T("dvrtwww"));
		if (nFind != -1)
		{
			sSection = VISION_SETTINGS;
			sSection = sSection + _T("\\") + m_sURL.Mid(nFind+8);
		}
		else
		{
			int nFind = m_sURL.ReverseFind('\\');
			if (nFind != -1)
			{
				sSection.Format(_T("%s\\%08lx\\%s"), VISION_SETTINGS, dwServer, m_sURL.Mid(nFind+1));
			}
		}
	}
	if (sSection.IsEmpty())
	{
		sSection.Format(_T("%s\\%08lx\\"), VISION_SETTINGS, dwServer);
	}
	return sSection;
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::Refresh()
{
	m_WebBrowser.Refresh();
	m_stTimeStamp.GetLocalTime();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::NavigateTo(const CString& sURL)
{
	CString sMyURL = sURL;

	if (0==sMyURL.Find(szFile))
	{
		CString sFile = sURL.Mid(_tcslen(szFile));
		if (!DoesFileExist(sFile))
		{
			sMyURL = theApp.GetDefaultURL();
		}
	}
	if (sMyURL.GetLength()>3)
	{
		if ((sMyURL.GetAt(1) == ':') && (sMyURL.GetAt(2) == '\\'))
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
void CHtmlWindow::OnSize(UINT nType, int cx, int cy) 
{
	CSmallWindow::OnSize(nType, cx, cy);

	CRect rect;
	GetFrameRect(rect);

	m_WebBrowser.MoveWindow(rect);

#ifdef _HTML_ELEMENTS
	CalculateClientRect();
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnHtmlNext() 
{
	m_WebBrowser.GoForward();
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateHtmlNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bForward);
	
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnHtmlPrev() 
{
	m_WebBrowser.GoBack();
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateHtmlPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bBack);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnHtmlHome() 
{
	if (!m_sHome.IsEmpty())
	{
		NavigateTo(m_sHome);
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateHtmlHome(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_sHome.IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow Events
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnCommandStateChange(long lCommand, BOOL bEnable)
{
	if (lCommand==0x1)
	{
		m_bForward = bEnable;
	}
	if (lCommand==0x2)
	{
		m_bBack = bEnable;
	}
	// user will override to handle this notification
	UNUSED_ALWAYS(lCommand);
	UNUSED_ALWAYS(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnTitleChange(LPCTSTR lpszText)
{
	// user will override to handle this notification
	CString sTitle = lpszText;
	TRACE(_T("OnTitleChange(\"%s\")\n"), sTitle);
	sTitle.MakeLower();
	int nFindExtHTM = sTitle.Find(_T(".htm"));
	if (nFindExtHTM == -1)
	{
		if (IsMapWindow())
		{
			m_sHTMLTitle = lpszText;
			CRect rect;
			GetTitleRect(rect);
			InvalidateRect(&rect, FALSE);
		}
		sTitle = m_WebBrowser.GetLocationURL();
		TRACE(_T("LocationURL:\"%s\"\n"), sTitle);
	}
	else if (m_sHome.IsEmpty())
	{
		m_sHome = m_WebBrowser.GetLocationURL();
		if (m_sHome.Find(szFile) == 0)
		{
			m_sHome = m_sHome.Mid(_tcslen(szFile));
		}
	}

	if (!sTitle.IsEmpty())
	{
#ifdef _HTML_ELEMENTS
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
		if (sTitle == _T("camera"))
		{
			m_nHTMLtype = HTML_TYPE_CAMERA_MAP;
			m_sHome.Empty();
		}
		else if (sTitle == _T("map"))
		{
			m_nHTMLtype = HTML_TYPE_HOST_MAP;
			m_wHostID   = SECSUBID_LOCAL_HOST;
			m_sHome.Empty();
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
#endif
	}

#ifdef _HTML_ELEMENTS

	if (nFindExtHTM == -1)
	{
		TRACE(_T("UpdateHTMLElements\n"));
		UpdateHTMLElements();
	}
	m_bMap = (m_nHTMLtype == HTML_TYPE_CAMERA_MAP || m_nHTMLtype == HTML_TYPE_HOST_MAP) ? TRUE : FALSE;
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnPropertyChange(LPCTSTR lpszProperty)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszProperty);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
	// default to continuing
	*bCancel = TRUE;

	// user will override to handle this notification
	UNUSED_ALWAYS(ppDisp);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnDocumentComplete(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	//REVIEW: this needs a reflector
	UNUSED_ALWAYS(lpszURL);
	TRACE(_T("OnDocumentComplete(LPCTSTR lpszURL=%s)\n"),lpszURL);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnQuit()
{
	// user will override to handle this notification
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnNavigateComplete2(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	TRACE(_T("OnNavigateComplete2(LPCTSTR lpszURL=%s)\n"),lpszURL);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::BeforeNavigate2(LPDISPATCH /* pDisp */, VARIANT* URL,
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
void CHtmlWindow::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
									LPCTSTR lpszTargetFrameName, CByteArray& baPostData,
									LPCTSTR lpszHeaders, BOOL* bCancel)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(lpszTargetFrameName);
	UNUSED_ALWAYS(baPostData);
	UNUSED_ALWAYS(lpszHeaders);

	// default to continuing
	CString sURL(lpszURL);

	sURL.MakeLower();
#ifdef _HTML_ELEMENTS

#else
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
#endif
	{
		*bCancel = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnStatusTextChange(LPCTSTR pszText)
{
#ifdef _HTML_ELEMENTS
	if (m_bEditPage)
	{
		return;
	}
#endif
	UNUSED_ALWAYS(pszText);

	if (pszText && pszText[0])
	{
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, 0, (LPARAM)pszText);
//		TRACE(_T("OnStatusTextChange(LPCTSTR pszText=%s)\n"),pszText);
	}
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _HTML_ELEMENTS
void CHtmlWindow::OnSelectHost(IHTMLElement*pHE)
{
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
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
				PostMessage(WM_COMMAND,ID_SMALL_CLOSE);
			}
			else if (bConnect == FALSE)
			{
				CString str;
				str.Format(IDP_NO_HOST, GetHTMLelementName(pHE));
				AfxMessageBox(str, MB_OK|MB_ICONERROR);
			}
			else
			{
				CObjectView* pObjectView = pDoc->GetObjectView();
				if (pObjectView)
				{
					CString str;
					str.Format(_T("%08x:"), id.GetID());// nach der ID suchen
					DWORD dwUpdate = HTML_ELEMENT_FIND|HTML_ELEMENT_SELECT|SEARCH_BY_ID|SEARCH_IN_HOSTS;
					pObjectView->UpdateMapElement(SECID_NO_ID, str, dwUpdate);
				}
			}
		}
		else
		{
			CVisionDoc* pDoc = m_pVisionView->GetDocument();
			if (pDoc)
			{
				CString sHost = GetHTMLelementName(pHE);
				CHostArray& ha = theApp.GetHosts();
				CHost* pHost=NULL;
				for (int i=0;i<ha.GetSize();i++)
				{
					if (0==sHost.CompareNoCase(ha.GetAt(i)->GetName()))
					{
						pHost = ha.GetAt(i);
						break;
					}
				}

				if (pHost)
				{
					pDoc->Connect(pHost->GetID());
					// ATTENTION delete's myself !!!
					PostMessage(WM_COMMAND,ID_SMALL_CLOSE);
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
void CHtmlWindow::OnSelectCamera(IHTMLElement*pHE)
{
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
	CObjectView* pObjectView = pDoc->GetObjectView();
	if (WK_IS_WINDOW(pObjectView))
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
			pObjectView->Select(wServerID, id);
		}
		else
		{
			pObjectView->SetFirstCamID(id);
		}
/*		
		CString sID;
		CSecID ServerID(wHost, m_wServerID);
		sID.Format(_T("%08x"), id.GetID());
		DWORD dwUpdate = HTML_ELEMENT_UPDATE_PAGE|HTML_ELEMENT_SELECT|SEARCH_IN_CAMERAS|SEARCH_BY_ID;
		pObjectView->UpdateMapElement(ServerID, sID, dwUpdate);
*/
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSelectDetector(IHTMLElement*pHE)
{
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
	CObjectView* pObjectView = pDoc->GetObjectView();

	if (WK_IS_WINDOW(pObjectView))
	{
		DWORD   dwFlags, dwServer;
		CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);

		CString sID;
		CSecID  ServerID(HOSTID_OF(dwServer), m_wServerID);
		sID.Format(_T("%08x"), id.GetID());
		DWORD dwUpdate = HTML_ELEMENT_UPDATE_PAGE|HTML_ELEMENT_SELECT|SEARCH_IN_INPUT|SEARCH_BY_ID;
		pObjectView->UpdateMapElement(ServerID, sID, dwUpdate);
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSelectRelay(IHTMLElement*pHE)
{
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
	CObjectView* pObjectView = pDoc->GetObjectView();

	if (WK_IS_WINDOW(pObjectView))
	{
		DWORD   dwFlags, dwServer;
		CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);
		
		CString sID;
		CSecID  ServerID(HOSTID_OF(dwServer), m_wServerID);
		sID.Format(_T("%08x"), id.GetID());
		DWORD dwUpdate = HTML_ELEMENT_UPDATE_PAGE|HTML_ELEMENT_SELECT|SEARCH_IN_RELAYS|SEARCH_BY_ID;
		pObjectView->UpdateMapElement(ServerID, sID, dwUpdate);
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSelectMedia(IHTMLElement*pHE)
{
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
	CObjectView* pObjectView = pDoc->GetObjectView();
	if (WK_IS_WINDOW(pObjectView))
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
			pObjectView->Select(wServerID, id);
		}
		else
		{
			pObjectView->SetSelectMediaID(id);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSelectHost(const CString& sHostURL)
{
	CString sHost;
	WORD wHost;
	if (ExtractNameFromURL(sHostURL, sHost, wHost))
	{
		CVisionDoc* pDoc = m_pVisionView->GetDocument();
		if (pDoc)
		{
			CHostArray& ha = theApp.GetHosts();
			CHost* pHost=NULL;
			for (int i=0;i<ha.GetSize();i++)
			{
				if (0==sHost.CompareNoCase(ha.GetAt(i)->GetName()))
				{
					pHost = ha.GetAt(i);
					break;
				}
			}

			if (pHost)
			{
				pDoc->Connect(pHost->GetID());
				// ATTENTION delete's myself !!!
				PostMessage(WM_COMMAND,ID_SMALL_CLOSE);
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
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSelectCamera(const CString& sCamURL)
{
	CString sCam;
	WORD wHost = SECID_NO_SUBID;
	if (ExtractNameFromURL(sCamURL, sCam, wHost))
	{
		CVisionDoc* pDoc = m_pVisionView->GetDocument();
		CObjectView* pObjectView = pDoc->GetObjectView();
		
		if (WK_IS_WINDOW(pObjectView))
		{
			pObjectView->Select(m_wServerID, sCam);
		}
	}
}
#endif
/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::ExtractNameFromURL(const CString &sURL, CString &sName, WORD &wHost)
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
void CHtmlWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_pVisionView->SetCmdActiveSmallWindow(this);
	CSmallWindow::OnRButtonDown(nFlags, point);
	CMainFrame *pMF = (CMainFrame*)GetParentFrame();
	CMenu menu, *pSubMenu=NULL, *pMonMenu = NULL;
	menu.LoadMenu(IDR_CONTEXT);
	if (pMF->GetNoOfExternalFrames()>0)
	{
		pMonMenu = menu.GetSubMenu(SUB_MENU_MONITOR);
	}
#ifdef _HTML_ELEMENTS
	CMenu   *pFPSMenu;
	CString  sL;
	pFPSMenu = menu.GetSubMenu(SUB_MENU_HTML);
	ASSERT(pFPSMenu != NULL);
	if (   theApp.m_pPermission == NULL									// Berechtigung
		 || !theApp.m_pPermission->IsSuperVisor()						// Kein Supervisor, keine Map
		 || (m_nHTMLtype != HTML_TYPE_HOST_MAP && m_nHTMLtype != HTML_TYPE_CAMERA_MAP))
	{
		int nCount = pFPSMenu->GetMenuItemCount();					// menü ist hinten
		pFPSMenu->DeleteMenu(--nCount, MF_BYPOSITION);				// Editmeü löschen
	}

	if ((m_nHTMLtype == HTML_TYPE_HOST_MAP || m_nHTMLtype == HTML_TYPE_CAMERA_MAP)
		 && m_wHostID == SECSUBID_LOCAL_HOST)							// ist es ´ne Map
	{
		if (m_bEditPage)														// Edit mode
		{
			pFPSMenu = pFPSMenu->GetSubMenu(2);							// Nur Bearbeiten
		}
		else																		// HTML mode
		{
			pFPSMenu->DeleteMenu(0, MF_BYPOSITION);					// Wechseln zu löschen
			if (pFPSMenu->GetMenuItemCount() == 1)						// wenn nur noch eins da ist
			{
				pFPSMenu = pFPSMenu->GetSubMenu(0);						// Menü direkt aufrufen
			}
		}
	}
	else																			// bei den Info Karten
	{
		pFPSMenu = pFPSMenu->GetSubMenu(0);								// Nur Wechseln zu
	}

	if (pMonMenu && pFPSMenu)												// Monitor items hinzufügen
	{
		CString sL;
		pFPSMenu->AppendMenu(MF_SEPARATOR, 0, _T(""));
		int i, nCount = pMonMenu->GetMenuItemCount();
		for (i=0; i<nCount; i++)
		{
			pMonMenu->GetMenuString(i, sL, MF_BYPOSITION);
			pFPSMenu->AppendMenu(MF_STRING, pMonMenu->GetMenuItemID(i), sL);
		}
		pSubMenu = pFPSMenu;
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
		pSubMenu = pFPSMenu;
	}

	if (m_bEditPage)
	{
		HitEditLink(point, &m_pSelected);
	}
	else
	{
		CVisionDoc* pDoc = m_pVisionView->GetDocument();
		CObjectView* pObjectView = pDoc->GetObjectView();
		if (WK_IS_WINDOW(pObjectView))
		{
			IHTMLElement *pHE = FindElementFromPoint(point, SEARCH_IN_ALL);
			if (pHE)
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
						if (pObjectView->UpdateMapElement(ServerID, str, dwUpdate))
						{
							pSubMenu = NULL;
						}
					}break;
					case HTML_ELEMENT_IS_CAMERA:
					{
						dwUpdate |= SEARCH_IN_CAMERAS;
						if (pObjectView->UpdateMapElement(ServerID, str, dwUpdate))
						{
							pSubMenu = NULL;
						}
					}break;
					case HTML_ELEMENT_IS_DETECTOR:
					{
						dwUpdate |= SEARCH_IN_INPUT;
						if (pObjectView->UpdateMapElement(ServerID, str, dwUpdate))
						{
							pSubMenu = NULL;
						}
					}break;
					case HTML_ELEMENT_IS_AUDIO_IN:
					case HTML_ELEMENT_IS_AUDIO_OUT:
					{
						dwUpdate |= SEARCH_IN_MEDIA;
						if (pObjectView->UpdateMapElement(ServerID, str, dwUpdate))
						{
							pSubMenu = NULL;
						}
					}break;
					case HTML_ELEMENT_IS_HOST:
					{
						if (pObjectView->UpdateMapElement(SECID_NO_ID, str, dwUpdate) == SECID_NO_ID)
						{
							pSubMenu = menu.GetSubMenu(SUB_MENU_CONNECT_HOST);
						}
						else
						{
							pSubMenu = NULL;
						}
					}break;
				}
			}
		}
	}
#endif
	if (pSubMenu)
	{
		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(0, pt.x, pt.y, pMF);
	}
}

#ifdef _HTML_ELEMENTS
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnViewHosts() 
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
void CHtmlWindow::OnUpdateViewHosts(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Hosts.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_HOSTS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnViewCameras() 
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
void CHtmlWindow::OnUpdateViewCameras(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Cameras.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_CAMERAS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnViewDetectors() 
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
void CHtmlWindow::OnUpdateViewDetectors(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Detectors.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_DETECTORS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnViewRelais() 
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
void CHtmlWindow::OnUpdateViewRelais(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Relais.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_RELAIS) ? TRUE : FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnViewMedias() 
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
void CHtmlWindow::OnUpdateViewMedias(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Medias.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SHOW_MEDIAS) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnAlarmSound() 
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
void CHtmlWindow::OnUpdateAlarmSound(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_Detectors.GetCount() > 0;
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck((m_dwShow & HTML_ELEMENT_SOUND) ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnViewRefresh() 
{
	if (!m_bEditPage)
	{
		Refresh();
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateViewRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bEditPage);	
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnTimer(UINT nIDEvent)
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
		nIDEvent    = UPDATE_HTML_WND_TIMER;
		m_dwShow   |= HTML_ELEMENT_UPDATE;
		bTimerEvent = FALSE;
	}
	else if (nIDEvent == UPDATE_HTML_WND_TIMER && (m_ptDiff.x != 0 || m_ptDiff.y !=0))
	{
		CPoint ptScroll;
		m_pHTMLTextContainer->get_scrollLeft(&ptScroll.x);
		m_pHTMLTextContainer->get_scrollTop(&ptScroll.y);
		ptScroll += m_ptDiff;
		m_pHTMLTextContainer->put_scrollLeft(ptScroll.x);
		m_pHTMLTextContainer->put_scrollTop(ptScroll.y);
		m_bDragging |= 2;
	}

	
	if (   nIDEvent == UPDATE_HTML_WND_TIMER 
		 && m_hwndIEServer != NULL
		 && (m_dwShow & HTML_ELEMENT_UPDATE) != 0)
	{
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
					m_Images.Draw(pDC, HTML_IMG_MARKED, pt, ILD_TRANSPARENT);
				}
				if (m_pSelected == pLink)
				{
					m_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
				}
				m_Images.Draw(pDC, pLink->nItem, pt, ILD_TRANSPARENT);
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
					int nImage = HTML_IMG_HOST;
					pHE = (IHTMLElement*) m_Hosts.GetNext(pos);
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (dwFlags & HTML_ELEMENT_ACTIVE) nImage = HTML_IMG_HOST_CONNECTED;
					pt  = GetHTMLelementPosition(pHE, 0);
					m_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
					if (!(dwFlags & HTML_ELEMENT_OK))
					{
						m_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
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
					if (m_pHESelected == pHE)
					{
						m_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}
				
					if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{
						m_Images.Draw(pDC, HTML_IMG_CAM          , pt, ILD_TRANSPARENT);
						m_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}
					else
					{
						if (dwFlags & HTML_ELEMENT_CAM_COLOR)  nImage = HTML_IMG_CAM_RGB;
						m_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
						if (!(dwFlags & HTML_ELEMENT_OK))
						{
							m_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
						}
						else if (dwFlags & HTML_ELEMENT_ENABLED)
						{
							m_Images.Draw(pDC, HTML_IMG_ACTIVE, pt, ILD_TRANSPARENT);
						}
						if (!(dwFlags & HTML_ELEMENT_CAM_FIXED))
						{
							m_Images.Draw(pDC, HTML_IMG_CAM_PTZ, pt, ILD_TRANSPARENT);
						}
					}
				}
			}
			if (m_dwShow & HTML_ELEMENT_SHOW_RELAIS)
			{
				POSITION pos = m_Relais.GetHeadPosition();
				while (pos)
				{
					int nImage = HTML_IMG_RELAY_CLOSED;
					pHE = (IHTMLElement*) m_Relais.GetNext(pos);
					pt  = GetHTMLelementPosition(pHE, 0);
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (m_pHESelected == pHE)
					{
						m_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}
					if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{
						m_Images.Draw(pDC, HTML_IMG_RELAY_OPEN   , pt, ILD_TRANSPARENT);
						m_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}
					else
					{
						if (dwFlags & HTML_ELEMENT_OPEN) nImage = HTML_IMG_RELAY_OPEN;
						m_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
						if (!(dwFlags & HTML_ELEMENT_OK))
						{
							m_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
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
					pHE = (IHTMLElement*) m_Detectors.GetNext(pos);
					pt  = GetHTMLelementPosition(pHE, 0);
					nImage = HTML_IMG_DETECTOR;
					DWORD dwFlags, dwServer;
					CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
					if (m_pHESelected == pHE)
					{
						m_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}
					if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{
						m_Images.Draw(pDC, HTML_IMG_DETECTOR     , pt, ILD_TRANSPARENT);
						m_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}
					else
					{
						if (dwFlags & HTML_ELEMENT_BLINK)
						{
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
						m_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
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
						m_Images.Draw(pDC, HTML_IMG_SELECTED, pt, ILD_TRANSPARENT);
					}
					if (dwFlags & HTML_ELEMENT_NOT_CONNECTED)
					{
						m_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
						m_Images.Draw(pDC, HTML_IMG_NOT_CONNECTED, pt, ILD_TRANSPARENT);
					}
					else
					{
						m_Images.Draw(pDC, nImage, pt, ILD_TRANSPARENT);
						if (!(dwFlags & HTML_ELEMENT_ENABLED))
						{
							m_Images.Draw(pDC, HTML_IMG_DISABLED, pt, ILD_TRANSPARENT);
						}
						else if (dwFlags & HTML_ELEMENT_ACTIVE)
						{
							m_Images.Draw(pDC, HTML_IMG_ACTIVE, pt, ILD_TRANSPARENT);
						}
					}
				}
			}
		}
		pDC->RestoreDC(-1);
		CWnd::FromHandle(m_hwndIEServer)->ReleaseDC(pDC);
	}
	else
	{
		CSmallWindow::OnTimer(nIDEvent);
	}
}
//////////////////////////////////////////////////////////////////////////
LRESULT CHtmlWindow::OnUser(WPARAM wParam, LPARAM lParam)
{
	if (wParam == HTML_CHANGE_LINK_NAME)
	{
		OnEditHtmlPage();
		if (AfxMessageBox(IDS_HTML_LINK_NAMES_CHANGED, MB_ICONINFORMATION|MB_YESNO) == IDYES)
		{
			OnEditHtmlPage();
		}
		else if (m_pVisionView)
		{
			m_pVisionView->SetCmdActiveSmallWindow(this);
			m_pVisionView->OnView1();
		}
	}
	else if (wParam == HTML_CHANGE_OLD_TYPE)
	{
		OnEditHtmlPage();
		OnEditHtmlPage();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CPoint CHtmlWindow::GetHTMLelementPosition(IHTMLElement *pHE, UINT uAlign, CRect*prcBound)
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
CSecID CHtmlWindow::GetHTMLelementID(IHTMLElement *pHE, DWORD& dwFlags, DWORD &dwServer)
{
	CSecID secID;
	BSTR id;
	pHE->get_id(&id);
	if (id)
	{
		DWORD dwID, dwValue2, dwValue3;
		int nValues = swscanf(id, HTML_ELEMENT_ID, (DWORD)&dwID, &dwValue2, &dwValue3);
		if (nValues == 3)
		{
			secID    = CSecID(dwID);
			dwFlags  = dwValue2;
			dwServer = dwValue3;
		}
	}
	return secID;
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::SetHTMLelementID(IHTMLElement *pHE, CSecID secID, DWORD dwFlags, DWORD dwServer)
{
	wchar_t id[32];
	swprintf(id, HTML_ELEMENT_ID, secID.GetID(), dwFlags, dwServer);
	pHE->put_id(id);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::ReleaseHTMLelements()
{
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
	while (m_Relais.GetCount())
	{
		((IHTMLElement*)m_Relais.RemoveHead())->Release();
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
void CHtmlWindow::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
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
//					if (m_rcClient.PtInRect(pt))
					{
						InvalidateRectRgn(&rc);
						TRACE(_T("Invalidate Host\n"));
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
			while (pos)
			{
				pHE = (IHTMLElement*) m_Detectors.GetNext(pos);
				id = GetHTMLelementID(pHE, dwFlags, dwServer);
				if (id == idInput && pHPhint->GetHostID() == HOSTID_OF(dwServer))
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
//						if (m_rcClient.PtInRect(pt))
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
							if (m_rcClient.PtInRect(pt))
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
					}
				}
			}
			else if (pRecO->IsRelay())
			{
				POSITION pos = m_Relais.GetHeadPosition();
				IHTMLElement *pHE;
				CSecID id, idOutput = pRecO->GetID();
				while (pos)
				{
					pHE = (IHTMLElement*) m_Relais.GetNext(pos);
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
//							if (m_rcClient.PtInRect(pt))
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
//							if (m_rcClient.PtInRect(pt))
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
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::UpdateHTMLElements()
{
	ReleaseHTMLelements();
	HRESULT hr;
	IDispatch *pDisp = m_WebBrowser.GetDocument();
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
	CObjectView* pObjectView = pDoc->GetObjectView();
	if (pDisp && WK_IS_WINDOW(pObjectView))
	{
		if (m_wServerID == pObjectView->GetLocalServerID())
		{
			pObjectView->DeleteLocalHTMLmappings();
		}
		IHTMLDocument2 *pDoc2 = NULL;
		hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2);
		if (SUCCEEDED(hr) &&  pDoc2)
		{
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
										if (id.GetID() != SECID_NO_ID)	// neue Karte von Vision erstellt ?
										{
											str.Format(_T("%08x:"), id.GetID());// nach der ID suchen
											dwUpdate = HTML_ELEMENT_FIND|SEARCH_BY_ID|SEARCH_IN_HOSTS;
											if (   !(m_dwChangeOptions & HTML_CHANGE_IN_PROGRESS) // keine Rekursion
												 && theApp.m_pPermission && theApp.m_pPermission->IsSuperVisor()) // Rechte
											{
												str += GetHTMLelementName(pHE);
												dwUpdate |= HTML_ELEMENT_CHECK_NAME; // Namen überprüfen
											}
											idTemp = pObjectView->UpdateMapElement(SECID_NO_ID, str, dwUpdate);
										}
										else										// alte Karten
										{
											dwUpdate = HTML_ELEMENT_FIND;
											str = GetHTMLelementName(pHE);// anhand des Namens idntifizieren
											idTemp = pObjectView->UpdateMapElement(SECID_NO_ID, str, dwUpdate);
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
											SetHTMLelementID(pHE, id, dwFlags, dwServer);
											m_dwChangeOptions |= HTML_CHANGE_LINK_NAME;
										}
										else if (dwUpdate & HTML_ELEMENT_NAME_CHANGED)
										{
											BSTR bstr;
											dwFlags |= HTML_ELEMENT_NAME_CHANGED;
											SetHTMLelementID(pHE, id, dwFlags, dwServer);
											bstr = str.AllocSysString();
											VERIFY(SUCCEEDED(pHE->put_title(bstr)));
											m_dwChangeOptions |= HTML_CHANGE_LINK_NAME;
										}
										SetHTMLelementID(pHE, id, dwFlags, dwServer);
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
												pList = &m_Relais;
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
												}
											}
										}
										if (pList)
										{
											CSecID id = GetHTMLelementID(pHE, dwFlags, dwServer);
											if (id.GetID() != SECID_NO_ID)// neue Karten (mit Vision erstellt)
											{
												if (HOSTID_OF(dwServer)  != SECSUBID_LOCAL_HOST) 
												{									// Verweist der Link auf eine andere Station
													if (m_wServerID == pObjectView->GetLocalServerID()) 
													{								// ist es eine lokale Karte
																					// dann muss diese ID zusätzlich mitgeführt werden
														ServerID.SetGroupID(HOSTID_OF(dwServer));
														pObjectView->AddLocalHTMLmapping(MAKELONG(0, HOSTID_OF(dwServer)), id);
													}
													else							// auf einer externen Karte
													{
														id = SECID_NO_ID;		// Element nicht einfügen, da die HostIDs nicht identisch sind
													}
												}
												else								// Verweist der Link auf die lokale Station (HostID)
												{
													if (m_wServerID != pObjectView->GetLocalServerID())	
													{								// ist es eine externe Karte
																					// muss die HostID mitgeführt werden
														dwServer = MAKELONG(LOWORD(dwServer), m_wHostID);
														SetHTMLelementID(pHE, id, dwFlags, dwServer);
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
												dwUpdate = dwSearch|HTML_ELEMENT_FIND;
												id  = pObjectView->UpdateMapElement(ServerID, str, dwUpdate);
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
													SetHTMLelementID(pHE, id, 0, dwServer);
												}
												dwUpdate = HTML_ELEMENT_UPDATE_STATE|dwSearch;
												if (   m_wHostID == SECSUBID_LOCAL_HOST// nur lokale Page
													 && !(m_dwChangeOptions & HTML_CHANGE_IN_PROGRESS) // keine Rekursion
													 && theApp.m_pPermission && theApp.m_pPermission->IsSuperVisor()) // Rechte
												{
													str += GetHTMLelementName(pHE);	// Namen überprüfen
													dwUpdate |= HTML_ELEMENT_CHECK_NAME;
												}
												CSecID idTemp = pObjectView->UpdateMapElement(ServerID, str, dwUpdate);
												if (idTemp.GetID() == SECID_NO_ID)
												{
													SetHTMLelementID(pHE, id, dwFlags&HTML_ELEMENT_TYPE_FLAGS|HTML_ELEMENT_NOT_CONNECTED, dwServer);
												}
												else
												{
													if (dwUpdate & HTML_ELEMENT_NAME_CHANGED)
													{
														BSTR bstr;
														dwFlags |= HTML_ELEMENT_NAME_CHANGED;
														SetHTMLelementID(pHE, id, dwFlags, dwServer);
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
												SetHTMLelementID(pHE, id, HTML_ELEMENT_NOT_CONNECTED, dwServer);
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

			pDoc2->Release();
		}
		pDisp->Release();

		DWORD dwShow = 0;
		if (m_Hosts.GetCount()    ) dwShow |= HTML_ELEMENT_SHOW_HOSTS;
		if (m_Cameras.GetCount()  ) dwShow |= HTML_ELEMENT_SHOW_CAMERAS;
		if (m_Detectors.GetCount()) dwShow |= HTML_ELEMENT_SHOW_DETECTORS;
		if (m_Relais.GetCount()   ) dwShow |= HTML_ELEMENT_SHOW_RELAIS;
		if (m_Medias.GetCount()   ) dwShow |= HTML_ELEMENT_SHOW_MEDIAS;

		if (m_dwShow != dwShow)		// wenn es sich aendert, alle anzeigen
		{
			m_dwShow = dwShow;
		}
	}
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
					m_lIEServerWndProc = SetWindowLong(m_hwndIEServer, GWL_WNDPROC, (long)CHtmlWindow::IEServerwndProc);
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
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnFileSaveHtmlText(CSize &szImage) 
{
	CString sHTMLName = m_sURL;
	if (sHTMLName.IsEmpty() && m_nHTMLtype == HTML_TYPE_HOST_MAP)
	{
		sHTMLName = theApp.GetMapURL();
	}
	if (!sHTMLName.IsEmpty())
	{
		CString sText, sID,
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
			CHTMLmapProperties dlg;
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

		sText.Format(HTML_TEXT_TOP, _T("1252"), m_nHTMLtype, m_nNewZoomValue, m_sHTMLTitle);
		file.Write(LPCTSTR(sText), sText.GetLength());
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
			file.Write(LPCTSTR(sText), sText.GetLength());
		}
		sText.Format(HTML_TEXT_BOTTOM, m_sPictureSource, szImage.cx, szImage.cy);
		file.Write(LPCTSTR(sText), sText.GetLength());
		m_nZoomValue = m_nNewZoomValue;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::OnEditHtmlPage() 
{
	m_pSelected = NULL;
	m_pHESelected = NULL;
	if (m_bEditPage)															// edit mode
	{
		CSize szImage(0,0);
		OnFileSaveHtmlText(szImage);										// Datei speichern
		m_EditLinks.DeleteAll();											// alle Links löschen, da jetzt wieder HTML
		m_bEditPage = FALSE;													// auf False setzen
		KillTimer(UPDATE_HTML_WND_TIMER);
		SetTimer(UPDATE_HTML_WND_TIMER, 500, NULL);
		OnViewRefresh();														// Updaten (m_bEditPage muss hier false sein)
		RedrawWindow();
		m_dwChangeOptions = 0;
	}
	else																			// normal mode
	{
		CPtrList *pList = NULL;
		IHTMLElement *pHE;
		BSTR Text;
		CHTML_Link *pLink;
		int i;
		
		for (i=0; i<5; i++)													// alle Elemente
		{
			if      (i == 0) pList = &m_Hosts;
			else if (i == 1) pList = &m_Cameras;
			else if (i == 2) pList = &m_Detectors;
			else if (i == 3) pList = &m_Relais;
			else if (i == 4) pList = &m_Medias;

			while (pList->GetCount())
			{
				pHE = (IHTMLElement*)pList->RemoveHead();
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
				m_EditLinks.AddTail(pLink);								// in den edit container einfügen
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

		KillTimer(UPDATE_HTML_WND_TIMER);
		SetTimer(UPDATE_HTML_WND_TIMER, 100, NULL);
		::InvalidateRect(m_hwndIEServer, NULL, FALSE);
		m_bEditPage = TRUE;
		RedrawWindow();
	}
	
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
	CObjectView* pObjectView = pDoc->GetObjectView();
	if (WK_IS_WINDOW(pObjectView))
	{
		if (m_bEditPage)
		{
			pObjectView->ModifyStyle(TVS_DISABLEDRAGDROP, 0);
		}
		else
		{
			pObjectView->ModifyStyle(0, TVS_DISABLEDRAGDROP);
		}
	}

	return m_bEditPage;
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateEditHtmlPage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnEditHtmlLinkProperties() 
{
	if (m_pSelected)
	{
		CHTMLlinkProperties dlg;
		CHTML_Link *pSel = m_pSelected;
		dlg.m_sHTMLlinkInfo = pSel->sInfo;
		if (dlg.DoModal() == IDOK)
		{
			pSel->sInfo = dlg.m_sHTMLlinkInfo;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateEditHtmlLinkProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pSelected != NULL);
}
//////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::OnLButtonDown(UINT nFlags, CPoint point)
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
				CVisionDoc* pDoc = m_pVisionView->GetDocument();
				CObjectView* pObjectView = pDoc->GetObjectView();
				if (WK_IS_WINDOW(pObjectView))
				{
					CString sID;
					CSecID ServerID(HOSTID_OF(m_pSelected->dwServer), m_wServerID);
					sID.Format(_T("%08x"), m_pSelected->secID.GetID());
					DWORD dwUpdate = HTML_ELEMENT_SELECT|SEARCH_IN_ALL|SEARCH_BY_ID;
					pObjectView->UpdateMapElement(ServerID, sID, dwUpdate);
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
		CSmallWindow::OnLButtonDown(nFlags, point);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::OnMouseMove(UINT nFlags, CPoint pt)
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
						m_Images.DragMove(point);
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
						CPoint pt(21, 3); // 13, 9
						if (m_Images.BeginDrag(m_pSelected->nItem, pt))
						{
							m_Images.DragEnter(this, point);
							m_bDragging = TRUE;
						}
					}
					if (m_ptDiff.x != 0 || m_ptDiff.y !=0)
					{
						m_ptDiff = CPoint(0, 0);
						::InvalidateRect(m_hwndIEServer, NULL, FALSE);
					}
					return TRUE;
				}
				else
				{
					CheckDragScroll(&pt);
				}
			}
		}
	}
	CSmallWindow::OnMouseMove(nFlags, pt);
	return m_bEditPage;
}
//////////////////////////////////////////////////////////////////////////
UINT CHtmlWindow::OnNcHitTest(CPoint point)
{
	if (m_bEditPage)
	{
		CRect       rc(0,0,0,0);
		GetHTMLelementPosition(m_pMAPImage, 0, &rc);
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
/*		UINT uHitTest = CSmallWindow::OnNcHitTest(point);
		TRACE(_T("HitTest %u"), uHitTest);
		return uHitTest;
*/		return CSmallWindow::OnNcHitTest(point);
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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
		CSmallWindow::OnSetCursor(pWnd, nHitTest, message);
	}
	return m_bEditPage;
}
//////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL bReturn = FALSE;
	if (m_bEditPage)
	{
		if (m_bDragging)
		{
			m_Images.DragLeave(this);
			m_Images.EndDrag();
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
		CVisionDoc* pDoc = m_pVisionView->GetDocument();
		CObjectView* pObjectView = pDoc->GetObjectView();
		if (WK_IS_WINDOW(pObjectView))
		{
			IHTMLElement *pHE = FindElementFromPoint(point, SEARCH_IN_ALL);
			if (pHE)
			{
				DWORD   dwFlags, dwServer;
				CSecID  id = GetHTMLelementID(pHE, dwFlags, dwServer);
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
					default:
					{
						AfxMessageBox(IDS_INVALID_HTML_ELEMENT, MB_OK|MB_ICONINFORMATION);
					}break;
				}
				bReturn = TRUE;
			}
			else
			{
				// TODO! RKE: Test Disabled Element
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
	CSmallWindow::OnLButtonUp(nFlags, point);
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::ExtractNameFromOuterHTML(const CString& sHTML, CString&sName)
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
CString CHtmlWindow::GetHTMLelementName(IHTMLElement*pHE, int *pnType, CString *psType)
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
		sName = ClearStringFromProcentASCII(sName.Mid(nFind, nLen - nFind));
		nLen = sName.GetLength();
		if (sName.GetAt(nLen-1) == '/')
		{
			sName = sName.Left(nLen - 1);
		}
	}
	return sName;
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::InsertElement(CSecID id, CString sName, WORD wServer, WORD wHost, CObject*pObj)
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


			CServer *pServer = m_pVisionView->GetDocument()->GetServer(wServer);
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
			m_bDragging = FALSE;
		}
		else
		{
			InvalidateLinkRgn(pLink);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnEditClear()
{
	// delete selected item/link
	POSITION pos = m_EditLinks.Find(m_pSelected);
	if (pos)
	{
		m_EditLinks.RemoveAt(pos);
		InvalidateLinkRgn(m_pSelected);
		WK_DELETE(m_pSelected);
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pSelected != NULL);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnEditHtmlMapProperties()
{
	ASSERT(m_bEditPage);
	CHTMLmapProperties dlg;
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
void CHtmlWindow::OnUpdateEditHtmlMapProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnEditClearMap() 
{
	if (IsMapWindow())
	{
		CString sMsg;
		sMsg.Format(IDP_DELETE_HTML_MAP, m_sHTMLTitle);
		if (AfxMessageBox(sMsg, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) == IDYES)
		{
			int iHtmlType = GetHTMLwndType();
			if (iHtmlType == HTML_TYPE_HOST_MAP)
			{
				if (theApp.DeleteMap())
				{
					OnSmallClose();
				}
			}
			else if (iHtmlType == HTML_TYPE_CAMERA_MAP)
			{
				if (DoesFileExist(m_sURL))
				{
					TRY
					{
						CFile::Remove(m_sURL);
						OnSmallClose();
					}
					CATCH( CFileException, e )
					{
						WK_TRACE_ERROR(_T("File %s cannot be removed\n"), m_sURL);
					}
					END_CATCH
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("CHtmlWindow::OnEditClearMap() UNKNOWN type %i\n"), iHtmlType);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateEditClearMap(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	if (HTML_TYPE_CAMERA_MAP == m_nHTMLtype)
	{
		// only allow for local server
		if (SECSUBID_LOCAL_HOST == m_wHostID)
		{
			bEnable = TRUE;
		}
	}
	else if (HTML_TYPE_HOST_MAP == m_nHTMLtype)
	{ // it must be a host map
		bEnable = TRUE;
	}
	pCmdUI->Enable(bEnable);
}
//////////////////////////////////////////////////////////////////////////
IHTMLElement* CHtmlWindow::FindElementByName(CString &sFind, DWORD dwSearch)
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
		for (pos = m_Relais.GetHeadPosition(); pos != NULL; )
		{
			pHE = (IHTMLElement*)m_Relais.GetNext(pos);
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
 Class      : CHtmlWindow
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
IHTMLElement* CHtmlWindow::FindElementFromPoint(CPoint pt, DWORD dwSearch)
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
		for (pos = m_Relais.GetHeadPosition(); pos != NULL; )
		{
			IHTMLElement *pHE = (IHTMLElement*)m_Relais.GetNext(pos);
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
void CHtmlWindow::InvalidateRectRgn(CRect*prc)
{
	if (prc)
	{
		CRgn  rgn;
		rgn.CreateRectRgn(prc->left, prc->top, prc->right, prc->bottom);
		::InvalidateRgn(m_hwndIEServer, rgn, FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::InvalidateElementRgn(IHTMLElement*pHE)
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
void CHtmlWindow::InvalidateLinkRgn(CHTML_Link*pLink)
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
void CHtmlWindow::OnHtmlSave() 
{
	if (m_bEditPage)
	{
		OnEditHtmlPage();
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateHtmlSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSmallContext() 
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnRButtonDown(0, pt);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateSmallContext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);//!m_bEditPage);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnRequestSave()
{
	if (m_bEditPage)
	{
		CString str;
		AfxFormatString1(str, AFX_IDP_ASK_TO_SAVE, m_sHTMLTitle);
		if (AfxMessageBox(str, MB_YESNO) == IDYES)
		{
			CSize sz(0,0);
			OnFileSaveHtmlText(sz);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
CRect CHtmlWindow::SetLinkShapeRect(CPoint ptClient)
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
void CHtmlWindow::CheckDragScroll(CPoint* pptClient)
{
	CPoint ptDiff = CPoint(0, 0);
	CPoint ptDiffTL = *pptClient - m_rcClient.TopLeft() ;
	CPoint ptDiffBR = *pptClient - m_rcClient.BottomRight();
	if (ptDiffTL.x < 0)
	{
		ptDiff.x = ptDiffTL.x;
	}
	if (ptDiffTL.y < 0)
	{
		ptDiff.y = ptDiffTL.y;
	}
	if (ptDiffBR.x > 0)
	{
		ptDiff.x = ptDiffBR.x;
	}
	if (ptDiffBR.y > 0)
	{
		ptDiff.y = ptDiffBR.y;
	}

	if (m_ptDiff.x == 0 && m_ptDiff.y == 0)
	{
		::InvalidateRect(m_hwndIEServer, NULL, FALSE);
	}

	m_ptDiff = ptDiff;
}
//////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::DraggedInto(CPoint pt)
{
	BOOL bInside = m_rcClient.PtInRect(pt);
	if (bInside)
	{
		m_ptDiff = CPoint(0,0);
	}
	return bInside;
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::HitEditLink(CPoint point, CHTML_Link**ppLink, bool bUpdate)
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
BOOL CHtmlWindow::IsHTMLElememtVisible(const CPoint& point, const CRect& rect)
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
int CHtmlWindow::OnToolHitTest( CPoint point, TOOLINFOW* pTI ) const
#else
int CHtmlWindow::OnToolHitTest( CPoint point, TOOLINFOA* pTI ) const
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
void CHtmlWindow::OnConnectHost() 
{
	if (m_pHESelected)
	{
		OnSelectHost(m_pHESelected);
	}
	
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateConnectHost(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pHESelected	!= NULL);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnLButtonDblClk(UINT nFlags, CPoint point) 
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
							if (dwFlags & HTML_ELEMENT_OPEN)
							{
								AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_RELAIS_CLOSE);
							}
							else
							{
								AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_RELAIS_OPEN);
							}
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
						WK_TRACE_WARNING(_T("CHtmlWindow::OnLButtonDblClk() pHE(%s) !=  m_pHESelected(%s)\n"),
												GetHTMLelementName(pHE), GetHTMLelementName(m_pHESelected));
					}
				}
			}
			else
			{
//				TRACE(_T("CHtmlWindow::OnLButtonDblClk() point not in client rect\n"));
			}
		}
	}
	else
	{
		CSmallWindow::OnLButtonDblClk(nFlags, point);
	}
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::CalculateClientRect()
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
#endif
//////////////////////////////////////////////////////////////////////////
