// HtmlWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Recherche.h"

#include "HtmlWindow.h"
#include "RechercheDoc.h"
#include "RechercheView.h"

#include <mshtml.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RELEASE_OBJECT(pObject)																\
{																										\
	if (pObject) {																					\
		int nR = pObject->Release();															\
		if (nR == 0) TRACE(_T("%s(%d):%s::Release(%d)\n"), __FILE__, __LINE__, #pObject, nR);	\
		pObject = NULL;																			\
	}																									\
}																

#define HTML_LINK							_T("href=")
#define HTML_USEMAP						_T("useMap=")
#define HTML_LINK_HOST					_T("host://")
#define HTML_ELEMENT_ID					L"%08x:%08x:%08x"	// ID, Flags, Server/Host

#define UPDATE_HTML_WND_TIMER_EX		0x0816

static TCHAR szFile[] = _T("file://");
/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow
void CHtmlWindow::Init()
{
	m_pRechercheView = NULL;
	m_bForward = FALSE;
	m_bBack = FALSE;
	m_stTimeStamp.GetLocalTime();

	m_pHTMLTextContainer = NULL;
	m_pMAPImage = NULL;
	m_hwndIEServer       = NULL;
	m_lIEServerWndProc = 0;
	m_hHostIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_HOSTICON));
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow::CHtmlWindow()
	: CSmallWindow(NULL)
{
	m_bMap = TRUE;
	Init();
}
/////////////////////////////////////////////////////////////////////////////
CHtmlWindow::~CHtmlWindow()
{
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
	ON_COMMAND(ID_HTML_NEXT, OnHtmlNext)
	ON_COMMAND(ID_HTML_PREV, OnHtmlPrev)
	ON_UPDATE_COMMAND_UI(ID_HTML_NEXT, OnUpdateHtmlNext)
	ON_UPDATE_COMMAND_UI(ID_HTML_PREV, OnUpdateHtmlPrev)
	ON_COMMAND(ID_SMALL_CLOSE, OnSmallClose)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
//	ON_COMMAND(ID_HTML_HOME, OnHtmlHome)
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
CString CHtmlWindow::GetName()
{
	return m_sHTMLTitle;
}
/////////////////////////////////////////////////////////////////////////////
CString CHtmlWindow::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime;

	sTitle.LoadString(IDS_MAP);
	sTitle += _T(" ") + m_sHTMLTitle;
	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
UINT CHtmlWindow::GetToolBarID()
{
	return IDR_HTML;
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSetWindowSize()
{
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::PopupMenu(CPoint pt)
{
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::GetFrameRect(LPRECT lpRect)
{
	GetClientRect(lpRect);
	lpRect->top = m_ToolBarSize.cy;
}
/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow message handlers
/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow event reflectors

/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlWindow::Create(const RECT& rect, UINT nID, CRechercheView* pParentWnd)
{
	BOOL bRet;
	m_pRechercheView = pParentWnd;
	bRet = CWnd::Create(NULL,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						nID); 

	if (bRet && m_pServer)
	{
		NavigateTo(m_sURL);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
int CHtmlWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSmallWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

/*	CString sT;
	GetModuleFileName(theApp.m_hInstance, sT.GetBuffer(MAX_PATH), MAX_PATH);
	sT.ReleaseBuffer();
	sT = sT.Left(sT.ReverseFind(_T('\\')));
*/
	CRect rect;
	GetClientRect(rect);
	m_WebBrowser.Create(_T("RechercheBrowser"),WS_VISIBLE|WS_CHILD,
						rect,this,AFX_IDW_PANE_FIRST);


	return 0;
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
		CString sFile = sURL.Mid(sizeof(szFile)-1);
		if (!DoesFileExist(sFile))
		{
			sMyURL = theApp.GetDefaultURL();
		}
	}
	if (sMyURL.GetLength()>3)
	{
		if ((sMyURL.GetAt(1) == _T(':')) && (sMyURL.GetAt(2) == _T('\\')))
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
	
	::GetClientRect(m_hwndIEServer, &m_rcClient);
	m_rcClient.right  -= GetSystemMetrics(SM_CXVSCROLL);
	m_rcClient.bottom -= GetSystemMetrics(SM_CYHSCROLL);
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
void CHtmlWindow::OnHtmlPrev() 
{
	m_WebBrowser.GoBack();
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnHtmlHome() 
{
	m_WebBrowser.GoHome();
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
	m_sHTMLTitle = lpszText;

	CRect rect;
	GetTitleRect(rect);
	InvalidateRect(rect,FALSE);
	UpdateHTMLElements();
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
//	TRACE(_T("OnNavigateComplete2(LPCTSTR lpszURL=%s)\n"),lpszURL);
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

	WK_TRACE(_T("navigate to %s\n"),lpszURL);
	
	// default to continuing
	CString sURL(lpszURL);

	sURL.MakeLower();
	if (0==sURL.Find(_T("host://")))
	{
		*bCancel = TRUE;
		OnSelectHost(lpszURL);
	}
	else
	{
		*bCancel = FALSE;
	}

}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnStatusTextChange(LPCTSTR pszText)
{
	// try to set the status bar text via the frame
	UNUSED_ALWAYS(pszText);
//	TRACE(_T("OnStatusTextChange(LPCTSTR pszText=%s)\n"),pszText);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateHtmlNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bForward);
	
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnUpdateHtmlPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bBack);
}
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSelectHost(const CString& sHostURL)
{
	CString sHost;
	int p;
	p = sHostURL.Find(_T("://"));
	if (p!=-1)
	{
		sHost = sHostURL.Mid(p+3);
		if (sHost.GetAt(sHost.GetLength()-1)==_T('/'))
		{
			sHost = sHost.Left(sHost.GetLength()-1);
		}
		sHost = ClearStringFromProcentASCII(sHost);

		IHTMLElement*pHE;
		POSITION pos = m_Hosts.GetHeadPosition();
		CSecID id;
		while (pos)
		{
			pHE = (IHTMLElement*) m_Hosts.GetNext(pos);
			BSTR Text;
			pHE->toString(&Text);
			if (sHostURL == CString(Text))
			{
				id = GetHTMLelementID(pHE);
				break;
			}
		}

		CRechercheDoc* pDoc = m_pRechercheView->GetDocument();
		if (pDoc)
		{
         CHostArray& ha = theApp.GetHosts();
			CHost* pHost=NULL;
			if (id != SECID_NO_ID)
			{
				pHost = ha.GetHost(id);
			}
			if (!pHost)
			{
				for (int i=0;i<ha.GetSize();i++)
				{
					if (0==sHost.CompareNoCase(ha.GetAt(i)->GetName()))
					{
						pHost = ha.GetAt(i);
						break;
					}
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
/////////////////////////////////////////////////////////////////////////////
void CHtmlWindow::OnSmallClose() 
{
	// attention delete's myself
	if (WK_IS_WINDOW(m_pRechercheView))
	{
		m_pRechercheView->DeleteHtmlWindow(this);
	}
}

void CHtmlWindow::ReleaseHTMLelements()
{
	while (m_Hosts.GetCount())
	{
		((IHTMLElement*)m_Hosts.RemoveHead())->Release();
	}

	RELEASE_OBJECT(m_pHTMLTextContainer);
	RELEASE_OBJECT(m_pMAPImage);
}
//////////////////////////////////////////////////////////////////////////
void CHtmlWindow::UpdateHTMLElements()
{
	ReleaseHTMLelements();
	HRESULT hr;
	IDispatch *pDisp = m_WebBrowser.GetDocument();

	if (pDisp)
	{
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
			CSecID ServerID;
         CHostArray& ha = theApp.GetHosts();
			CHost* pHost=NULL;

			VariantInit(&var);

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
									m_pMAPImage = pHE;
									pHE = NULL;
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
										pHost = ha.GetHost(GetHTMLelementID(pHE));
										if (pHost)
										{
											m_Hosts.AddTail(pHE);
											pHE = NULL;
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
			}
		}
	}
}
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

	LRESULT lResult = ::CallWindowProc((WNDPROC)pThis->m_lIEServerWndProc, hwnd, uMsg, wParam, lParam); 
	if (uMsg == WM_PAINT)
	{
		pThis->PostMessage(WM_TIMER, UPDATE_HTML_WND_TIMER_EX, 0);
	}

	return lResult;
}

void CHtmlWindow::OnTimer(UINT nIDEvent)
{
	if (   nIDEvent == UPDATE_HTML_WND_TIMER_EX
		 && m_hwndIEServer != NULL)
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

		IHTMLElement *pHE;
		CPoint pt;
			
		POSITION pos = m_Hosts.GetHeadPosition();
		while (pos)
		{
			pHE = (IHTMLElement*) m_Hosts.GetNext(pos);
			pt  = GetHTMLelementPosition(pHE, 0);
			pDC->DrawIcon(pt, m_hHostIcon);
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
CPoint CHtmlWindow::GetHTMLelementPosition(IHTMLElement *pHE, UINT uAlign, CRect*prcBound)
{
	CPoint pt(0,0), ptOffset, ptScroll;
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

CSecID CHtmlWindow::GetHTMLelementID(IHTMLElement *pHE, DWORD* pdwFlags, DWORD *pdwServer)
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
			
			if (pdwFlags) *pdwFlags  = dwValue2;
			if (pdwServer) *pdwServer = dwValue3;
		}
	}
	return secID;
}
