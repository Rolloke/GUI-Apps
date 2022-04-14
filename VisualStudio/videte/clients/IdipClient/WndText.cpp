// WndText.cpp: implementation of the CWndText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "mainfrm.h"

#include "WndText.h"
#include ".\wndtext.h"

IMPLEMENT_DYNAMIC(CWndText, CWndSmall)

//////////////////////////////////////////////////////////////////////
CWndText::CWndText(CServer* pServer, const CSecID id, const CString& sMessage)
	: CWndSmall(pServer)
{
	m_ID = id;
	m_sMessage = sMessage;
	m_stTimeStamp.GetLocalTime();
	m_dwMonitor = USE_MONITOR_2;
	m_nType = WST_TEXT;
}
/////////////////////////////////////////////////////////////////////////////
CWndText::~CWndText()
{

}
/////////////////////////////////////////////////////////////////////////////
UINT CWndText::GetToolBarID()
{
	return IDR_TEXT;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndText, CWndSmall)
	//{{AFX_MSG_MAP(CWndText)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_SMALL_CONTEXT, OnUpdateSmallContext)
	ON_COMMAND(ID_SMALL_CONTEXT, CWndSmall::OnSmallContext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClear)
	ON_WM_DESTROY()
	ON_WM_PARENTNOTIFY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CWndText::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);
}
/////////////////////////////////////////////////////////////////////////////
CString CWndText::GetTitleText(CDC* pDC)
{
	if (IsDebuggerWindow())
	{
		return _T("Debug output");
	}
	else
	{
		CString sTitle,sTime;

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

		sFormat.LoadString(IDS_NOTIFICATION_FROM);
		sTitle.Format(sFormat,GetServerName(),sTime);

		return sTitle;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndText::Create(const RECT& rect, UINT nID, CViewIdipClient* pParentWnd)
{
	BOOL bRet;
	m_pViewIdipClient = pParentWnd;
	bRet = CWnd::Create(NULL,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						nID); 

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CSecID CWndText::GetID()
{
	return m_ID;
}
/////////////////////////////////////////////////////////////////////////////
int CWndText::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndSmall::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	GetClientRect(rect);

	if (IsDebuggerWindow())
	{
		CEdit* pEdit = new CEdit;
		if (!pEdit->Create(WS_VISIBLE|WS_CHILD|ES_READONLY|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE, rect, this, AFX_IDW_PANE_FIRST))
		{
			TRACE(_T("Failed to create richedit ctrl\n"));
			WK_DELETE(pEdit);
			return -1;
		}
		SetDebuggerWindowHandle(pEdit->m_hWnd);
		CheckMessageFont();
		pEdit->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	}
	else
	{
		if (!m_RichEditCtrl.Create(WS_VISIBLE|WS_CHILD|
							   ES_READONLY|ES_MULTILINE,
							   //|ES_AUTOHSCROLL|ES_AUTOVSCROLL,
							   rect,
							   this,
							   AFX_IDW_PANE_FIRST))
		{
			TRACE(_T("Failed to create richedit ctrl\n"));
			return -1;
		}
		m_RichEditCtrl.SetBackgroundColor(FALSE,RGB(192,192,192));
		m_RichEditCtrl.ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
		CheckMessageFont();
		m_RichEditCtrl.SetWindowText(m_sMessage);
	}

	m_wndToolBar.ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::OnSize(UINT nType, int cx, int cy) 
{
	CWndSmall::OnSize(nType, cx, cy);
	
	CRect rect;
	GetFrameRect(rect);
	GetDlgItem(AFX_IDW_PANE_FIRST)->MoveWindow(rect);
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::SetNotificationText(const CString& sMessage)
{
	if (IsDebuggerWindow())
	{
		return;
	}
	m_sMessage = sMessage;
	if (m_hWnd)
	{
		CheckMessageFont();
		GetDlgItem(AFX_IDW_PANE_FIRST)->SetWindowText(m_sMessage);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::CheckMessageFont()
{
	int nFind = m_sMessage.Find(NM_CODEPAGE);
	UINT uCodePage = (UINT)-1;
	if (nFind != -1)
	{
		CString sCodePage = m_sMessage.Mid(nFind + _tcslen(NM_CODEPAGE));
		uCodePage = _ttoi(sCodePage);
		m_sMessage = m_sMessage.Left(nFind);
	}
	
	m_Font.DeleteObject();

	if (uCodePage != (UINT)-1)
	{
		CStringArray ar;
		if (GetCodePageInfo(uCodePage, ar) && ar.GetAt(OEM_CP_GDICHARSET) != _T("00"))
		{
			m_Font.CreatePointFont(120, ar.GetAt(OEM_CP_PROPORTIONALFONT));
			GetDlgItem(AFX_IDW_PANE_FIRST)->SetFont(&m_Font);
		}
	}
	if (m_Font.GetSafeHandle() == NULL && CSkinDialog::UseGlobalFonts())
	{
		GetDlgItem(AFX_IDW_PANE_FIRST)->SetFont(CSkinDialog::GetDialogItemGlobalFont());
		return;
	}
	if (m_Font.GetSafeHandle() == NULL)
	{
		m_Font.CreatePointFont(120,_T("Times New Roman"));
		GetDlgItem(AFX_IDW_PANE_FIRST)->SetFont(&m_Font);
	}
}
/////////////////////////////////////////////////////////////////////////////
int  CWndText::GetNoOfLines()
{
	if (m_RichEditCtrl.m_hWnd)
	{
		return m_RichEditCtrl.GetLineCount();
	}
	else
	{
		CWnd *pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
		ASSERT_KINDOF(CEdit, pWnd);
		return ((CEdit*)pWnd)->GetLineCount();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString  CWndText::GetLine(int n)
{
	CString s;
	if (m_RichEditCtrl.m_hWnd)
	{
		int nLen = m_RichEditCtrl.LineLength(m_RichEditCtrl.LineIndex(n));
		if (nLen)
		{
			nLen = m_RichEditCtrl.GetLine(n, s.GetBufferSetLength(nLen+4), nLen);
			s.ReleaseBuffer(nLen);
		}
	}
	else
	{
		CWnd *pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
		ASSERT_KINDOF(CEdit, pWnd);
		int nLen = ((CEdit*)pWnd)->LineLength(((CEdit*)pWnd)->LineIndex(n));
		if (nLen)
		{
			nLen = ((CEdit*)pWnd)->GetLine(n, s.GetBufferSetLength(nLen+1), nLen);
			s.ReleaseBuffer(nLen);
		}
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::OnDestroy()
{
	if (IsDebuggerWindow())
	{
		CWnd *pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
		{
			delete (CEdit*)pWnd;
		}
		SetDebuggerWindowHandle(NULL);
	}
	CWndSmall::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndText::IsDebuggerWindow()
{
	return (m_pServer == NULL && m_ID == SECID_NO_ID && m_sMessage.IsEmpty()) ? TRUE: FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::PopupMenu(CPoint pt)
{
	BOOL bMonitorMenu = theApp.GetMainFrame()->GetNoOfExternalFrames() > 0;
	BOOL bDebuggerMenu = IsDebuggerWindow();
	CSkinMenu menu;
	menu.LoadMenu(IDR_MENU_LIVE);

	CSkinMenu*pMenuTemp = (CSkinMenu*) menu.GetSubMenu(SUB_MENU_MONITOR);

	if (!bMonitorMenu)
	{
		while (pMenuTemp->GetMenuItemCount())
		{
			pMenuTemp->DeleteMenu(0, MF_BYPOSITION);
		}
	}

	if (bDebuggerMenu)
	{
		CString sClear;
		sClear.LoadString(ID_EDIT_CLEAR);
		int nFind = sClear.Find(_T("\n"));
		if (nFind != -1)
		{
			sClear = sClear.Mid(nFind +1);
		}
		if (pMenuTemp->GetMenuItemCount())
		{
			pMenuTemp->AppendMenu(MF_SEPARATOR, 0, _T(""));
		}
		pMenuTemp->AppendMenu(MF_BYPOSITION, ID_EDIT_CLEAR_ALL, sClear);
	}

	if (m_wndToolBar.GetSize().cy <= 0)
	{
		UINT nID = ID_CLOSE_WND_THIS;
		CString str;
		menu.FindMenuString(nID, str);
		if (pMenuTemp->GetMenuItemCount())
		{
			pMenuTemp->AppendMenu(MF_SEPARATOR, 0, _T(""));
		}
		pMenuTemp->AppendMenu(MF_BYPOSITION, ID_CLOSE_WND_THIS, str);
	}

	UINT nID = ID_FILE_PRINT;
	CString str;
	CMenu *pPrintMenu = menu.FindMenuString(nID, str);
	if (pPrintMenu)
	{
		if (pMenuTemp->GetMenuItemCount())
		{
			pMenuTemp->AppendMenu(MF_SEPARATOR, 0, _T(""));
		}
		int i, n = pPrintMenu->GetMenuItemCount();
		for (i=0; i<n; i++)
		{
			pPrintMenu->GetMenuString(i, str, MF_BYPOSITION);
			pMenuTemp->AppendMenu(MF_BYPOSITION, pPrintMenu->GetMenuItemID(i), str);
		}
	}

	pMenuTemp->ConvertMenu(TRUE);
  	pMenuTemp->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, AfxGetMainWnd());		
	pMenuTemp->ConvertMenu(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsDebuggerWindow());
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::OnEditClear()
{
	if (IsDebuggerWindow())
	{
		CWnd *pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
		if (pWnd)
		{
			pWnd->SetWindowText(_T(""));
		}
	}
}
///////////////////////////////////////////////////////////////////
void CWndText::OnUpdateSmallContext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}
///////////////////////////////////////////////////////////////////
void CWndText::OnParentNotify(UINT message, LPARAM lParam)
{
	CWndSmall::OnParentNotify(message, lParam);
	if (message == WM_LBUTTONDOWN && m_wndToolBar.GetSize().cy <= 0)
	{
		CPoint pt(lParam);
		GetDlgItem(AFX_IDW_PANE_FIRST)->ClientToScreen(&pt);
		PopupMenu(pt);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndText::CanPrint()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndText::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->m_pPD->m_pd.Flags |= PD_RETURNDC;
	BOOL b = m_pViewIdipClient->DoPreparePrinting(pInfo);

	if (b)
	{
		// now the printer DC should be initialized;
		CDC* pDC = CDC::FromHandle(pInfo->m_pPD->GetPrinterDC());
		CFont font;
		font.CreatePointFont(8*10, GetFixFontFaceName(), pDC);
		CFont* pOldFont = (CFont*)pDC->SelectObject(&font);
		TEXTMETRIC metrics;
		pDC->GetTextMetrics(&metrics);
		m_nItemHeight = metrics.tmHeight+5;
		CRect rcPage(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		CRect rcM = theApp.GetPrintMargin(pDC);
		rcPage.DeflateRect(rcM.left, rcM.top, rcM.right, rcM.bottom);
		
		CString s;
		pDC->SelectObject(pOldFont);
		int nLines = GetNoOfLines();
		for (; nLines>0; nLines--)
		{
			s = GetLine(nLines-1);
			s.TrimRight();
			if (!s.IsEmpty())
			{
				break;
			}
		}

		m_nItemsPerPage = (rcPage.Height() / m_nItemHeight);
		m_nItemsPerPage -= 4;
		
		pInfo->SetMinPage(1);

		int iNrPages = nLines/m_nItemsPerPage + ((nLines%m_nItemsPerPage)>0 ? 1 : 0);
		pInfo->SetMaxPage(iNrPages);
	}

	return b;
}
/////////////////////////////////////////////////////////////////////////////
void CWndText::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	int nPrintIndex = (pInfo->m_nCurPage-1)*m_nItemsPerPage;

	CString sMsg		= _T("");

	DOCINFO di;
	ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = _T("logevents");

	// Geeigneten nicht proportionalen Font auswählen
	CFont font;
	font.CreatePointFont(8*10, GetFixFontFaceName(), pDC);
	CFont* pOldFont = (CFont*)pDC->SelectObject(&font);

	// Einen Stift für die Trennlinien auswählen
	CPen pen(PS_SOLID, 4, RGB(0,0,0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	CRect rcPage(pInfo->m_rectDraw);
	CRect rcM = theApp.GetPrintMargin(pDC);
	rcPage.DeflateRect(rcM.left, rcM.top, rcM.right, rcM.bottom);
	
	int nYPos = 0;

	// Datum holen....
	sMsg = GetTitleText(pDC);

	// ...und als Überschrift ausgeben.
	CRect rcTitle = rcPage;
	pDC->DrawText(sMsg, rcTitle, DT_CALCRECT|DT_LEFT|DT_TOP|DT_WORDBREAK);
	pDC->DrawText(sMsg, rcTitle, DT_LEFT|DT_TOP|DT_WORDBREAK);

	sMsg.Format(IDS_ALARM_LIST_PRINT_PAGE,pInfo->m_nCurPage,pInfo->GetMaxPage());
	sMsg = theApp.GetStationName() + _T(": ") + sMsg;
	pDC->DrawText(sMsg, rcPage, DT_RIGHT|DT_TOP|DT_SINGLELINE);
	
	// Waagerechte Trennlinie zeichnen
	pDC->MoveTo(rcPage.left, rcPage.top+rcTitle.Height());
	pDC->LineTo(rcPage.right, rcPage.top+rcTitle.Height());
	int nLines = GetNoOfLines();

	nYPos = rcTitle.Height();
	for (int nI=0; nI<m_nItemsPerPage; nI++)
	{
		if (nPrintIndex < nLines) 
		{
			sMsg = GetLine(nPrintIndex);
			nYPos += m_nItemHeight; 

			CRect rcText(rcPage.left, rcPage.top+nYPos, rcPage.right, rcPage.bottom);
			pDC->DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);
			nPrintIndex++;
		}
	}

	if (nPrintIndex < nLines)
	{
		pInfo->m_bContinuePrinting = TRUE;
	}

	
	pDC->SelectObject(pOldFont);
	pOldFont->DeleteObject();

	pDC->SelectObject(pOldPen);
	pOldPen->DeleteObject();
}
