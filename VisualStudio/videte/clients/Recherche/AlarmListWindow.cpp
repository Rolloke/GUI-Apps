// AlarmListWindow.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "rechercheview.h"
#include "AlarmListWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmListWindow
CAlarmListWindow::CAlarmListWindow(CServer* pServer, CIPCSequenceRecord* pSequence)
 : CSmallWindow(pServer),m_SequenceRecord(*pSequence)
{
	m_pAlarmListDialog = new CAlarmListDialog(this);
	m_nTypeLen = 0;
	m_nDateLen = 0;
	m_nTimeLen = 0;
	m_bQuery = FALSE;
	m_pQueryResult = NULL;

	if (m_pServer->IsConnected())
	{
		CIPCDatabaseRecherche* pCIPCDatabaseRecherche = m_pServer->GetDatabase();
		CIPCArchivRecord* pAR = pCIPCDatabaseRecherche->GetArchiv(m_SequenceRecord.GetArchiveNr());
		if (pAR)
		{
			m_sName = pAR->GetName();
		}
	}
	m_nPrintIndex = 0;
	m_nItemsPerPage = 0;
	m_nItemHeight = 0;
}
/////////////////////////////////////////////////////////////////////////////
CAlarmListWindow::~CAlarmListWindow()
{
	WK_DELETE(m_pAlarmListDialog);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAlarmListWindow, CSmallWindow)
	//{{AFX_MSG_MAP(CAlarmListWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_NAVIGATE_FORWARD, OnNavigateForward)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FORWARD, OnUpdateNavigateForward)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ALARM_LIST, OnAlarmList)
	ON_COMMAND(ID_SEARCH_RESULT, OnQueryResult)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CAlarmListWindow message handlers
BOOL CAlarmListWindow::Create(const RECT& rect, CRechercheView* pParentWnd)
{
	m_pRechercheView = pParentWnd;

	return CWnd::Create(SMALL_WINDOW_CLASS,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						(int)MAKELONG(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr()));
}
/////////////////////////////////////////////////////////////////////////////
CSecID CAlarmListWindow::GetID()
{
	return CSecID(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr());
}
/////////////////////////////////////////////////////////////////////////////
CString	CAlarmListWindow::GetName()
{
	return m_sName + _T(" ") + m_SequenceRecord.GetTime().GetLocalizedDateTime(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
UINT CAlarmListWindow::GetToolBarID()
{
	return IDR_ALARM_LIST;
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnSetWindowSize()
{
	CRect rect;
	GetClientRect(rect);
	rect.top = m_ToolBarSize.cy;
	m_pAlarmListDialog->MoveWindow(rect);
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::PopupMenu(CPoint pt)
{
	// we don't have a right mouse click menu
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);
}
/////////////////////////////////////////////////////////////////////////////
CString CAlarmListWindow::GetTitleText(CDC* pDC)
{
	return GetName();
}
/////////////////////////////////////////////////////////////////
int CAlarmListWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSmallWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pAlarmListDialog->Create(CAlarmListDialog::IDD,this);
	m_pAlarmListDialog->ShowWindow(SW_SHOW);
	
	return 0;
}
/////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnSize(UINT nType, int cx, int cy) 
{
	CSmallWindow::OnSize(nType, cx, cy);
	
	OnSetWindowSize();
	
}
/////////////////////////////////////////////////////////////////
BOOL CAlarmListWindow::IsAlarmListWindow()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////
void CAlarmListWindow::ConfirmRecords(CIPCFields fields,
									  CIPCFields records)
{
	// in CIPC Thread
	if (fields.GetSize() >= 3)
	{
		if (fields.GetAt(0)->GetName() == _T("DVR_TYP"))
			m_nTypeLen = fields.GetAt(0)->GetMaxLength();
		if (fields.GetAt(1)->GetName() == _T("DVR_DATE"))
			m_nDateLen = fields.GetAt(1)->GetMaxLength();
		if (fields.GetAt(2)->GetName() == _T("DVR_TIME"))
			m_nTimeLen = fields.GetAt(2)->GetMaxLength();
	}
	else
	{
		WK_TRACE_ERROR(_T("CAlarmListWindow::ConfirmRecords fields len %i\n"),fields.GetSize());
	}
	m_Records = records;
	PostMessage(WM_COMMAND,ID_ALARM_LIST);
}
/////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnAlarmList()
{
	// in Main Thread
	m_pAlarmListDialog->DeleteAlarms();
	if (   (m_nTypeLen > 0) 
		&& (m_nDateLen > 0) 
		&& (m_nTimeLen > 0))
	{
		for (int nI = 0; nI < m_Records.GetSize(); nI++)
		{
			CIPCField* pField = m_Records.GetAt(nI);
			if (pField)
			{
				CString sRecord		= pField->GetValue();
				CString sType		= sRecord.Left(m_nTypeLen);
				CString sDate		= sRecord.Mid(m_nTypeLen, m_nDateLen);
				CString sTime		= sRecord.Mid(m_nTypeLen+m_nDateLen, m_nTimeLen);
				m_pAlarmListDialog->AddAlarm(sType,sTime,sDate);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnNavigateForward() 
{
	if (!m_bQuery)
	{
		CString sTyp,sTime,sDate;
		if (m_pAlarmListDialog->GetAlarm(m_pAlarmListDialog->GetCurSelAlarm(),sTyp,sTime,sDate))
		{
			if (m_pServer->IsConnected())
			{
				CIPCDatabaseRecherche* pCIPCDatabaseRecherche = m_pServer->GetDatabase();
				CIPCFields fields;
				WORD w[1];

				// find out the correct Ring archive for this alarm list.
				CIPCArchivRecord* pAR = pCIPCDatabaseRecherche->GetArchiv(m_SequenceRecord.GetArchiveNr());

				if (pAR)
				{
					w[0] = pAR->GetSafeRingID();
				}
				else
				{
					w[0] = (WORD)((WORD)(m_SequenceRecord.GetArchiveNr() & 0xFF) | (WORD)0x2000);
				}


				fields.SafeAdd(new CIPCField(_T("DVR_DATE"),sDate,_T('}')));
				fields.SafeAdd(new CIPCField(_T("DVR_TIME"),sTime,_T('}')));

				m_bQuery = TRUE;
				pCIPCDatabaseRecherche->DoRequestNewQuery(GetID().GetID(),1,w,fields,1);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnUpdateNavigateForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   (m_pAlarmListDialog->GetNrOfAlarms()>0)
				   && (m_pAlarmListDialog->GetCurSelAlarm()!=-1)
				   );
	
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::ConfirmQuery()
{
	// in CIPC Thread
	// nothing found
	m_bQuery = FALSE;
	WK_DELETE(m_pQueryResult);
	PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::ConfirmQuery(DWORD dwUserID,
									WORD wArchivNr,
									WORD wSequenceNr, 
									DWORD dwRecordNr,
									int iNumFields,
									const CIPCField fields[])
{
	// in CIPC Thread
	m_bQuery = FALSE;
	WK_DELETE(m_pQueryResult);
	m_pQueryResult = new CQueryResult(dwUserID,m_pServer->GetID(),
		wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);

	PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListWindow::IsInQuery()
{
	return m_bQuery;
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnQueryResult()
{
	if (m_pQueryResult)
	{
		// in Main Thread
		// now show the corresponding CDisplayWindow
		CDisplayWindow* pDW= m_pRechercheView->GetDisplayWindow(m_pServer->GetID(),
																m_pQueryResult->GetArchiveNr(),
																m_pQueryResult->GetSequenceNr());
		if (pDW==NULL)
		{
			m_pServer->OpenSequence(m_pQueryResult->GetArchiveNr(),
									m_pQueryResult->GetSequenceNr(),
									m_pQueryResult->GetRecordNr());

		}
		else
		{
			pDW->Navigate(m_pQueryResult->GetRecordNr(),SECID_NO_ID);
		}
		WK_DELETE(m_pQueryResult)
	}
	else
	{
		// nothing found at query.
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::UpdateAlarmList()
{
	// In Main Thread
	OnAlarmList();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListWindow::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->m_pPD->m_pd.Flags |= PD_RETURNDC;
	BOOL b = m_pRechercheView->DoPreparePrinting(pInfo);

	if (b)
	{
		// now the printer DC should be initialized;
		CDC* pDC = CDC::FromHandle(pInfo->m_pPD->GetPrinterDC());
		CFont font;
		font.CreatePointFont(8*10, _T("Courier New"), pDC);
		CFont* pOldFont = (CFont*)pDC->SelectObject(&font);
		TEXTMETRIC metrics;
		pDC->GetTextMetrics(&metrics);
		m_nItemHeight = metrics.tmHeight+5;
		pDC->SelectObject(pOldFont);
		CRect rcPage(0,0,pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		rcPage.DeflateRect(200,200,200, 200);
		
		m_nItemsPerPage = (rcPage.Height() / m_nItemHeight);
		m_nItemsPerPage -= 4;
		m_nItemsPerPage *=2;
		
		pInfo->SetMinPage(1);
		int iNrPages = m_pAlarmListDialog->GetNrOfAlarms()/m_nItemsPerPage
			+ ((m_pAlarmListDialog->GetNrOfAlarms()%m_nItemsPerPage)>0 ? 1 : 0);
		pInfo->SetMaxPage(iNrPages);
	}

	return b;
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListWindow::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	m_nPrintIndex = (pInfo->m_nCurPage-1)*m_nItemsPerPage;

	CString sError		= _T("");
	CString sTimestamp	= _T("");
	CString sConfirm	= _T("");
	CString sRteNr		= _T("");
	CString sMsg		= _T("");
	CString	sTmp		= _T("");

	int		nSpalten	= 2;

	DOCINFO di;
	ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = _T("logevents");

	// Geeigneten nicht proportionalen Font auswählen
	CFont font;
	font.CreatePointFont(8*10, _T("Courier New"), pDC);
	CFont* pOldFont = (CFont*)pDC->SelectObject(&font);

	// Einen Stift für die Trennlinien auswählen
	CPen pen(PS_SOLID, 4, RGB(0,0,0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	CRect rcPage (0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
	rcPage.DeflateRect(200,200,200, 200);
	
	int nYPos = 0;
	int nXPos = 0;

	// Datum holen....
	CSystemTime time;
	CString sTyp;
	m_pAlarmListDialog->GetAlarm(0,sTyp,time);
	sMsg.Format(IDS_ALARM_LIST_PRINT_TITLE,
				m_pServer->GetName(),
				time.GetLocalizedDate(FALSE),m_sName);

	// ...und als Überschrift ausgeben.
	CRect rcTitle = rcPage;
	pDC->DrawText(sMsg, rcTitle, DT_CALCRECT|DT_LEFT|DT_TOP|DT_WORDBREAK);
	pDC->DrawText(sMsg, rcTitle, DT_LEFT|DT_TOP|DT_WORDBREAK);

	sMsg.Format(IDS_ALARM_LIST_PRINT_PAGE,pInfo->m_nCurPage,pInfo->GetMaxPage());
	pDC->DrawText(sMsg, rcPage, DT_RIGHT|DT_TOP|DT_SINGLELINE);
	
	// Waagerechte Trennlinie zeichnen
	pDC->MoveTo(rcPage.left, rcPage.top+rcTitle.Height());
	pDC->LineTo(rcPage.right + rcPage.Width(), rcPage.top+rcTitle.Height());

	int nMaxLinesPerPage = m_nItemsPerPage/2;
	TRACE(_T("nMaxLinesPerPage=%d\n"),nMaxLinesPerPage);
	nXPos = 0;
	for (int nJ = 0; nJ < nSpalten; nJ++)
	{
		nYPos = rcTitle.Height();
		for (int nI = 0; nI < nMaxLinesPerPage; nI++)
		{
			if (m_nPrintIndex < m_pAlarmListDialog->GetNrOfAlarms()) 
			{
				CString sTyp;
				CSystemTime stTime;
				if (m_pAlarmListDialog->GetAlarm(m_nPrintIndex,sTyp,stTime))
				{
					// Eine Zeile weiter					
					nYPos += m_nItemHeight; 

					CString sIndex;
					sIndex.Format(_T("%d"),m_nPrintIndex+1);

					// Text ausgeben.
					sMsg.Format(_T("   %5s.) %s , %s , %s"), 
						sIndex, 
						sTyp,
						stTime.GetLocalizedDate(),
						stTime.GetLocalizedTime());
					CRect rcText(rcPage.left+nXPos, rcPage.top+nYPos, rcPage.right, rcPage.top);
					pDC->DrawText(sMsg, rcText, DT_CALCRECT | DT_LEFT|DT_WORDBREAK);
					pDC->DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);
				}
				m_nPrintIndex++;
			}
		}
		nXPos += rcPage.Width() / nSpalten;	

		// Vertikale Trennlinie zeichnen
		pDC->MoveTo(rcPage.left + rcPage.Width()/2, rcPage.top+rcTitle.Height());
		pDC->LineTo(rcPage.left + rcPage.Width()/2, rcPage.bottom-rcTitle.Height());
	}	
	if (m_nPrintIndex < m_pAlarmListDialog->GetNrOfAlarms())
	{
		pInfo->m_bContinuePrinting = TRUE;
	}

	
	pDC->SelectObject(pOldFont);
	pOldFont->DeleteObject();

	pDC->SelectObject(pOldPen);
	pOldPen->DeleteObject();
}
