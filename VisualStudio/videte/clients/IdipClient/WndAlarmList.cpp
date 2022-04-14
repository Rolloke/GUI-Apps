// WndAlarmList.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"
#include "WndAlarmList.h"

#include "ViewIdipClient.h"
#include "WndPlay.h"
#include "DlgComment.h"
#include ".\wndalarmlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	COLUMN_WIDTH	_T("ColumnWidth")	
#define ACTIVITY		_T("Activity")	
#define DETECTOR1		_T("Detector1")	
#define	DETECTOR2		_T("Detector2")	

IMPLEMENT_DYNAMIC(CWndAlarmList, CWndSmall)
/////////////////////////////////////////////////////////////////////////////
// CWndAlarmList
CWndAlarmList::CWndAlarmList(CServer* pServer, CIPCSequenceRecord* pSequence)
 : CWndSmall(pServer),m_SequenceRecord(*pSequence)
{
	m_pDlgAlarmList = new CDlgAlarmList(this);
	m_bQuery = FALSE;
	m_pQueryResult = NULL;

	if (m_pServer->IsConnectedPlay())
	{
		CIPCDatabaseIdipClient* pDB = m_pServer->GetDatabase();
		if(pDB)
		{
			CIPCArchivRecord* pAR = pDB->GetArchive(m_SequenceRecord.GetArchiveNr());
			if (pAR)
			{
				m_sName = pAR->GetName();
			}
		}
	}

	m_nItemsPerPage = 0;
	m_nItemHeight = 0;

	m_OptimizeColumnWidth.lh.wLow = OPTIMIZE_BY_CONTENT;
	m_OptimizeColumnWidth.lh.wHigh = OPTIMIZE_TO_WND_WIDTH;
	m_nType = WST_ALARM_LIST;
}
/////////////////////////////////////////////////////////////////////////////
CWndAlarmList::~CWndAlarmList()
{
	WK_DELETE(m_pDlgAlarmList);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndAlarmList, CWndSmall)
	//{{AFX_MSG_MAP(CWndAlarmList)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_NAVIGATE_FORWARD, OnNavigateForward)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FORWARD, OnUpdateNavigateForward)
	ON_COMMAND(ID_SEARCH_RESULT, OnQueryResult)
	ON_COMMAND(ID_SMALL_CONTEXT, OnSmallContext)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CONTEXT, OnUpdateSmallContext)
	ON_COMMAND(ID_VIEW_OPTIMIZE_COLUMN_WIDTH, OnViewOptimizeColumnWidth)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIMIZE_COLUMN_WIDTH, OnUpdateViewOptimizeColumnWidth)
	ON_COMMAND(ID_VIEW_OPTIMIZE_BY_CONTENT, OnViewOptimizeByContent)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIMIZE_BY_CONTENT, OnUpdateViewOptimizeByContent)
	ON_COMMAND(ID_VIEW_OPTIMIZE_BY_HEADER, OnViewOptimizeByHeader)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIMIZE_BY_HEADER, OnUpdateViewOptimizeByHeader)
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_OPTIMIZE_TO_WND_WIDTH, OnViewOptimizeToWndWidth)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIMIZE_TO_WND_WIDTH, OnUpdateViewOptimizeToWndWidth)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CWndAlarmList message handlers
BOOL CWndAlarmList::Create(const RECT& rect, CViewIdipClient* pParentWnd)
{
	m_pViewIdipClient = pParentWnd;

	return CWnd::Create(SMALL_WINDOW_CLASS,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						(int)MAKELONG(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr()));
}
/////////////////////////////////////////////////////////////////////////////
CString	CWndAlarmList::GetSection()
{
	CString sSection;
	CSecID idArchive (SECID_GROUP_ARCHIVE, m_SequenceRecord.GetArchiveNr());
	sSection.Format(_T("%s\\%08lx\\%08lx"), IDIP_CLIENT_SETTINGS, m_pServer->GetHostID().GetID(), idArchive);
	return sSection;
}
/////////////////////////////////////////////////////////////////////////////
CSecID CWndAlarmList::GetID()
{
	return CSecID(m_SequenceRecord.GetArchiveNr(),m_SequenceRecord.GetSequenceNr());
}
/////////////////////////////////////////////////////////////////////////////
CString	CWndAlarmList::GetName()
{
	return m_sName + _T(" ") + m_SequenceRecord.GetTime().GetLocalizedDateTime(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
UINT CWndAlarmList::GetToolBarID()
{
	return IDR_ALARM_LIST;
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnSetWindowSize()
{
	CRect rect;
	GetFrameRect(&rect);
	m_pDlgAlarmList->MoveWindow(rect);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndAlarmList::CanPrint()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::PopupMenu(CPoint pt)
{
	CMenu menuLive, menuArchive;
	CMenu* pMenu;
	CMenu* pFPSMenu;
	UINT	nID, nTemp;
	CString sL;

	menuLive.LoadMenu(IDR_MENU_LIVE);
	menuArchive.LoadMenu(IDR_MENU_ARCHIVE);
	nID = ID_CLOSE_WND_THIS;
	pMenu = ((CSkinMenu*)&menuLive)->FindMenuString(nID, sL);

	if (pMenu)
	{
		nID = nTemp = ID_VIEW_OPTIMIZE_COLUMN_WIDTH;
		pFPSMenu = ((CSkinMenu*)&menuArchive)->FindMenuString(nTemp, sL);
		if (pFPSMenu)
		{
			pMenu->AppendMenu(MF_SEPARATOR, 0, _T(""));
			int i, nCount = pFPSMenu->GetMenuItemCount();
			for (i=0; i<nCount; i++)
			{
				pFPSMenu->GetMenuString(i, sL, MF_BYPOSITION);
				pMenu->AppendMenu(MF_STRING, pFPSMenu->GetMenuItemID(i), sL);
			}
		}

		nID = ID_FILE_PRINT_SETUP;
		pFPSMenu = ((CSkinMenu*)&menuLive)->FindMenuString(nID, sL);
		if (pFPSMenu)
		{
			sL.Replace(0, _T('\\'));
			CStringArray ar;
			SplitString(sL, ar, _T('\\'));
			pMenu->AppendMenu(MF_SEPARATOR, 0, _T(""));
			pMenu->AppendMenu(MF_POPUP,(UINT)pFPSMenu->m_hMenu, ar.GetAt(1));
		}

		if (theApp.GetMainFrame()->GetNoOfExternalFrames() > 0)
		{
			pFPSMenu = menuLive.GetSubMenu(SUB_MENU_MONITOR);	// Monitor
			pMenu->AppendMenu(MF_SEPARATOR, 0, _T(""));
			int i, nCount = pFPSMenu->GetMenuItemCount();
			for (i=0; i<nCount; i++)
			{
				pFPSMenu->GetMenuString(i, sL, MF_BYPOSITION);
				pMenu->AppendMenu(MF_STRING, pFPSMenu->GetMenuItemID(i), sL);
			}
		}

		COemGuiApi::DoUpdatePopupMenu(this, pMenu);
		CSkinMenu*pMenuTemp = (CSkinMenu*)pMenu;
		pMenuTemp->ConvertMenu(TRUE);
  		pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, AfxGetMainWnd());
		pMenuTemp->ConvertMenu(FALSE);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);
}
/////////////////////////////////////////////////////////////////////////////
CString CWndAlarmList::GetTitleText(CDC* pDC)
{
	return GetName();
}
/////////////////////////////////////////////////////////////////////////////
WORD CWndAlarmList::GetArchivNr()
{
	return m_SequenceRecord.GetArchiveNr();
}
/////////////////////////////////////////////////////////////////
int CWndAlarmList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndSmall::OnCreate(lpCreateStruct) == -1)
		return -1;

	CWK_Profile wkp;
	CString sSection = GetSection();
	m_dwMonitor = wkp.GetInt(sSection, USE_MONITOR, m_dwMonitor);
	m_OptimizeColumnWidth.dw = wkp.GetInt(sSection, COLUMN_WIDTH, m_OptimizeColumnWidth.dw);
	m_pDlgAlarmList->m_bActivity = wkp.GetInt(sSection, ACTIVITY, m_pDlgAlarmList->m_bActivity);
	m_pDlgAlarmList->m_bDetector1 = wkp.GetInt(sSection, DETECTOR1, m_pDlgAlarmList->m_bDetector1);
	m_pDlgAlarmList->m_bDetector2 = wkp.GetInt(sSection, DETECTOR2, m_pDlgAlarmList->m_bDetector2);
	
	m_pDlgAlarmList->Create(CDlgAlarmList::IDD,this);
	m_pDlgAlarmList->ShowWindow(SW_SHOW);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnDestroy()
{
	CWK_Profile wkp;
	CString sSection = GetSection();
	wkp.WriteInt(sSection, USE_MONITOR, m_dwMonitor);
	wkp.WriteInt(sSection, COLUMN_WIDTH, m_OptimizeColumnWidth.dw);
	wkp.WriteInt(sSection, ACTIVITY, m_pDlgAlarmList->m_bActivity);
	wkp.WriteInt(sSection, DETECTOR1, m_pDlgAlarmList->m_bDetector1);
	wkp.WriteInt(sSection, DETECTOR2, m_pDlgAlarmList->m_bDetector2);

	CWndSmall::OnDestroy();
}
/////////////////////////////////////////////////////////////////
void CWndAlarmList::OnSize(UINT nType, int cx, int cy) 
{
	CWndSmall::OnSize(nType, cx, cy);
	
	OnSetWindowSize();
	
}
/////////////////////////////////////////////////////////////////
void CWndAlarmList::ConfirmRecords(const CIPCFields& fields,
								   const CIPCFields& records)
{
	// in CIPC Thread
	m_Fields = fields;
	m_Records = records;
	PostMessage(WM_COMMAND, ID_VIEW_REFRESH);
}
/////////////////////////////////////////////////////////////////
BOOL CWndAlarmList::GetAlarm(int i, CString& sType, CString& sDate, CString& sTime, CString& sMS, 
							CSecID& idInput, CSecID& idOutput,CSecID& idArchive, CStringArray& sa)
{
	BOOL bRet = FALSE;
	if (i>=0 && i<m_Records.GetSize())
	{
		CString s,sRecord = m_Records.GetAtFast(i)->GetValue();
		int pos,len;

		pos = 0;
		for (int j=0;j<m_Fields.GetSize();j++)
		{
			CIPCField* pField = m_Fields.GetAtFast(j);
			len = pField->GetMaxLength();
			s = sRecord.Mid(pos,len);

			if (   pField->GetName() == _T("DVR_TYP")
				|| pField->GetName() == _T("DBP_TYP"))
			{
				sType = s;
			}
			else if (   pField->GetName() == _T("DVR_DATE")
				|| pField->GetName() == _T("DBP_DATE"))
			{
				sDate = s;

			}
			else if (   pField->GetName() == _T("DVR_TIME")
				|| pField->GetName() == _T("DBP_TIME"))
			{
				sTime = s;
			}
			else if (   pField->GetName() == _T("DVR_MS")
				|| pField->GetName() == _T("DBP_MS"))
			{
				sMS = s;
			}
			else
			{
				if (pField->GetName() == _T("DBP_INPNR"))
				{
					DWORD dwInput = 0;
					_stscanf((LPCTSTR)s,_T("%08lx"),&dwInput);
					s.Empty();
					if (m_pServer->GetInput())
					{
						s = m_pServer->GetInput()->GetInputNameFromID(dwInput);
					}
					idInput = dwInput;
				}
				else if (pField->GetName() == _T("DBP_ARCNR"))
				{
					DWORD dwArchive = 0;
					_stscanf((LPCTSTR)s,_T("%08lx"),&dwArchive);
					s.Empty();
					if (m_pServer->GetDatabase())
					{
						CSecID id(dwArchive);
						s = m_pServer->GetDatabase()->GetArchiveName(id.GetSubID());
					}
					idArchive = dwArchive;
				}
				else if (pField->GetName() == _T("DBP_CANR"))
				{
					DWORD dwOutput = 0;
					_stscanf((LPCTSTR)s,_T("%08lx"),&dwOutput);
					s.Empty();
					if (m_pServer->GetOutput())
					{
						s = m_pServer->GetOutput()->GetOutputNameFromID(dwOutput);
					}
					idOutput = dwOutput;
				}
				sa.Add(s);
			}

			pos += len;
		}
		bRet = TRUE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnNavigateForward() 
{
	if (!m_bQuery)
	{

		int i = m_pDlgAlarmList->GetCurSelAlarm();

		if (i>=0 && i<m_Records.GetSize())
		{
			CString sTyp,sTime,sDate,sMS;
			CSecID idInput,idOutput,idArchive;
			CStringArray sa;

			if (m_pServer->IsConnectedDatabase())
			{
				CIPCDatabaseIdipClient* pCIPCDatabaseIdipClient = m_pServer->GetDatabase();
				CIPCFields fields;
				WORD wArchiveIDs[1];
				wArchiveIDs[0] = 0;

				GetAlarm(i,sTyp,sDate,sTime,sMS,idInput,idOutput,idArchive,sa);

				if (m_pServer->IsDTS())
				{
					// find out the correct Ring archive for this alarm list.
					CIPCArchivRecord* pAR = pCIPCDatabaseIdipClient->GetArchive(m_SequenceRecord.GetArchiveNr());

					if (pAR)
					{
						wArchiveIDs[0] = pAR->GetSafeRingID();
					}
					else
					{
						wArchiveIDs[0] = (WORD)((WORD)(m_SequenceRecord.GetArchiveNr() & 0xFF) | (WORD)0x2000);
					}
					fields.SafeAdd(new CIPCField(_T("DVR_DATE"),sDate,_T('}')));
					fields.SafeAdd(new CIPCField(_T("DVR_TIME"),sTime,_T('}')));
				}
				else
				{
					CSystemTime t;
					t.SetDBTime(sTime);
					t.SetDBDate(sDate);
					DWORD dwMS = 0;
					_stscanf(sMS,_T("d"),&dwMS);
					t.wMilliseconds = (WORD)dwMS;

					t.DecrementTime(0,0,0,0,50);
					sDate = t.GetDBDate();
					sTime = t.GetDBTime();
					sMS.Format(_T("%d"),t.wMilliseconds);

					fields.SafeAdd(new CIPCField(_T("DBP_DATE"),sDate,_T('}')));
					fields.SafeAdd(new CIPCField(_T("DBP_TIME"),sTime,_T('}')));
					fields.SafeAdd(new CIPCField(_T("DBP_MS"),sMS,_T('}')));
					wArchiveIDs[0] = idArchive.GetSubID();
				}

				if (wArchiveIDs[0]!= SECID_NO_SUBID)
				{
					m_bQuery = TRUE;
					pCIPCDatabaseIdipClient->DoRequestNewQuery(GetID().GetID(),1,wArchiveIDs,fields,1);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnUpdateNavigateForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   (m_pDlgAlarmList->GetNrOfAlarms()>0)
				   && (m_pDlgAlarmList->GetCurSelAlarm()!=-1)
				   );
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::ConfirmQuery()
{
	// in CIPC Thread
	// nothing found
	m_bQuery = FALSE;
	WK_DELETE(m_pQueryResult);
	PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::ConfirmQuery(DWORD dwUserID,
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
BOOL CWndAlarmList::IsInQuery()
{
	return m_bQuery;
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnQueryResult()
{
	if (m_pQueryResult)
	{
		// in Main Thread
		// now show the corresponding CWndPlay
		CWndPlay* pDW= m_pViewIdipClient->GetWndPlay(m_pServer->GetID(),
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
BOOL CWndAlarmList::OnPreparePrinting(CPrintInfo* pInfo)
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

		CRect rcPage (0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		CRect rcM = theApp.GetPrintMargin(pDC);
		rcPage.DeflateRect(rcM.left, rcM.top, rcM.right, rcM.bottom);

		m_dwaColumnWidth.RemoveAll();
		int nTotal = m_pDlgAlarmList->OptimizeColumnWidth(pDC, &m_dwaColumnWidth);
		CString sIndex;
		sIndex.Format(_T("%d:#"),10);
		CSize sz = pDC->GetOutputTextExtent(sIndex);
		m_dwaColumnWidth[0] += sz.cx;
		nTotal              += sz.cx;
		pDC->SelectObject(pOldFont);

		m_nPageColumns = rcPage.Width() / nTotal;
		if (m_nPageColumns == 0) m_nPageColumns = 1;
		if (m_nPageColumns > 3) m_nPageColumns = 3;

		int j, nWidth = rcPage.Width();
		if (m_nPageColumns > 1)
		{
			nWidth /= m_nPageColumns;
		}
		if (nWidth > nTotal)
		{
			for (j=0; j<m_dwaColumnWidth.GetCount(); j++)
			{
				m_dwaColumnWidth[j] = MulDiv(m_dwaColumnWidth[j], nWidth, nTotal);
			}
		}
		m_nItemsPerPage = (rcPage.Height() / m_nItemHeight);
		m_nItemsPerPage -= 4;
		m_nItemsPerPage *= m_nPageColumns;
		
		pInfo->SetMinPage(1);
		int iNrPages = m_pDlgAlarmList->GetNrOfAlarms()/m_nItemsPerPage
						+ ((m_pDlgAlarmList->GetNrOfAlarms()%m_nItemsPerPage)>0 ? 1 : 0);
		pInfo->SetMaxPage(iNrPages);
	}

	return b;
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	int nPrintIndex = (pInfo->m_nCurPage-1)*m_nItemsPerPage;

	CString sMsg		= _T("");

	DOCINFO di;
	ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = _T("logevents");

	// Geeigneten nicht proportionalen Font auswählen
	CFont font, fontBold;
	LOGFONT lf;
	font.CreatePointFont(8*10, GetFixFontFaceName(), pDC);
	if (font.GetLogFont(&lf))
	{
		lf.lfWeight = FW_BOLD;
		fontBold.CreateFontIndirect(&lf);
	}
	CFont* pOldFont = (CFont*)pDC->SelectObject(&font);

	// Einen Stift für die Trennlinien auswählen
	CPen pen(PS_SOLID, 4, RGB(0,0,0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	CRect rcPage(pInfo->m_rectDraw);
	CRect rcM = theApp.GetPrintMargin(pDC);
	rcPage.DeflateRect(rcM.left, rcM.top, rcM.right, rcM.bottom);
	
	int nYPos = 0;
	int nXPos = 0;

	// Datum holen....
	CSystemTime time;
	CString sTyp;
	CStringArray saColumns;
	m_pDlgAlarmList->GetAlarm(nPrintIndex, sTyp, time, saColumns);
	sMsg.Format(IDS_ALARM_LIST_PRINT_TITLE, m_pServer->GetName(), time.GetLocalizedDate(FALSE), m_sName);

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

	int nMaxLinesPerPage = m_nItemsPerPage/m_nPageColumns;
	CSize szChar = pDC->GetOutputTextExtent(_T("#"));
	TRACE(_T("nMaxLinesPerPage=%d\n"),nMaxLinesPerPage);
	nXPos = szChar.cx;
	for (int nJ = 0; nJ < m_nPageColumns; nJ++)
	{
		nYPos = rcTitle.Height();
		if (fontBold.m_hObject)
		{
			int i;
			nYPos += m_nItemHeight; 
			pDC->SelectObject(&fontBold);
			saColumns.RemoveAll();
			m_pDlgAlarmList->GetHeaders(saColumns);

			CRect rcText(rcPage.left+nXPos, rcPage.top+nYPos, rcPage.right, rcPage.bottom);
			for (i=0; i<saColumns.GetCount(); i++)
			{
				if (m_dwaColumnWidth[i])
				{
					pDC->DrawText(saColumns.GetAt(i), rcText, DT_LEFT|DT_WORDBREAK);
					rcText.left += m_dwaColumnWidth[i];
				}
			}
			pDC->SelectObject(&font);
		}
		for (int nI = 0; nI < nMaxLinesPerPage; nI++)
		{
			if (nPrintIndex < m_pDlgAlarmList->GetNrOfAlarms()) 
			{
				CString sTyp;
				CSystemTime stTime;
				saColumns.RemoveAll();
				if (m_pDlgAlarmList->GetAlarm(nPrintIndex, sTyp, stTime, saColumns))
				{
					// Eine Zeile weiter					
					nYPos += m_nItemHeight; 

					CString sIndex;
					sIndex.Format(_T("%d: "),nPrintIndex+1);
					CRect rcText(rcPage.left+nXPos, rcPage.top+nYPos, rcPage.right, rcPage.bottom);
					sMsg = sIndex + sTyp;
					pDC->DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);
					rcText.right = rcPage.right;
					rcText.left += m_dwaColumnWidth[0];
					pDC->DrawText(stTime.GetLocalizedDate(), rcText, DT_LEFT|DT_WORDBREAK);
					rcText.left += m_dwaColumnWidth[1];
					pDC->DrawText(stTime.GetLocalizedTime(), rcText, DT_LEFT|DT_WORDBREAK);
					rcText.left += m_dwaColumnWidth[2];
					int n;
					for (n=0; n<saColumns.GetCount(); n++)
					{
						sMsg = saColumns.GetAt(n);
						sMsg.TrimRight();
						if (!sMsg.IsEmpty())
						{
							pDC->DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);
							rcText.left += m_dwaColumnWidth[n+3];
						}
					}
				}
				nPrintIndex++;
			}
		}
		if (nJ < m_nPageColumns - 1)
		{
			nXPos += rcPage.Width() / m_nPageColumns;

			// Vertikale Trennlinie zeichnen
			pDC->MoveTo(rcPage.left + rcPage.Width()/2, rcPage.top+rcTitle.Height());
			pDC->LineTo(rcPage.left + rcPage.Width()/2, rcPage.bottom-rcTitle.Height());
		}
	}

	if (nPrintIndex < m_pDlgAlarmList->GetNrOfAlarms())
	{
		pInfo->m_bContinuePrinting = TRUE;
	}

	
	pDC->SelectObject(pOldFont);
	pOldFont->DeleteObject();

	pDC->SelectObject(pOldPen);
	pOldPen->DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnUpdateSmallContext(CCmdUI* pCmdUI) 
{
	if (theApp.m_bDisableSmall)
	{
		pCmdUI->Enable(!theApp.GetMainFrame()->IsFullScreen());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnViewOptimizeColumnWidth()
{
	m_pDlgAlarmList->OptimizeColumnWidth();
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnUpdateViewOptimizeColumnWidth(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pDlgAlarmList->m_ListAlarm.GetItemCount() > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnViewOptimizeByContent()
{
	m_OptimizeColumnWidth.lh.wLow = OPTIMIZE_BY_CONTENT;
	m_pDlgAlarmList->OptimizeColumnWidth();
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnUpdateViewOptimizeByContent(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pDlgAlarmList->m_ListAlarm.GetItemCount() > 0);
	pCmdUI->SetRadio(m_OptimizeColumnWidth.lh.wLow == OPTIMIZE_BY_CONTENT);
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnViewOptimizeByHeader()
{
	m_OptimizeColumnWidth.lh.wLow = OPTIMIZE_BY_HEADER;
	m_pDlgAlarmList->OptimizeColumnWidth();
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnUpdateViewOptimizeByHeader(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pDlgAlarmList->m_ListAlarm.GetItemCount() > 0);
	pCmdUI->SetRadio(m_OptimizeColumnWidth.lh.wLow == OPTIMIZE_BY_HEADER);
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnViewOptimizeToWndWidth()
{
	if (m_OptimizeColumnWidth.lh.wHigh & OPTIMIZE_TO_WND_WIDTH)
	{
		m_OptimizeColumnWidth.lh.wHigh &= ~OPTIMIZE_TO_WND_WIDTH;
	}
	else
	{
		m_OptimizeColumnWidth.lh.wHigh |= OPTIMIZE_TO_WND_WIDTH;
	}
	m_pDlgAlarmList->OptimizeColumnWidth();
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnUpdateViewOptimizeToWndWidth(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_OptimizeColumnWidth.lh.wHigh & OPTIMIZE_TO_WND_WIDTH);
}
/////////////////////////////////////////////////////////////////////////////
void CWndAlarmList::OnViewRefresh()
{
	// in Main Thread
	ASSERT(theApp.IsInMainThread());
	m_pDlgAlarmList->DeleteAlarms();
	m_pDlgAlarmList->CreateColumnHeader(m_Fields);

	for (int nI = 0; nI < m_Records.GetSize(); nI++)
	{
		CIPCField* pRecord = m_Records.GetAtFast(nI);
		if (pRecord)
		{
			CString sRecord	= pRecord->GetValue();
			CSystemTime st;
			CString s,sType,sDate,sTime,sMS;
			CStringArray sa;
			CSecID idInput,idOutput,idArchive;

			GetAlarm(nI,sType,sDate,sTime,sMS,idInput,idOutput,idArchive,sa);
			st.SetDBDate(sDate);
			st.SetDBTime(sTime);
			if (!sMS.IsEmpty())
			{
				DWORD dwMS = 0;
				_stscanf((LPCTSTR)sMS,_T("%d"),&dwMS);
				st.wMilliseconds = (WORD)dwMS;
			}

			m_pDlgAlarmList->AddAlarm(sType,st,sa);
		}
	}
	m_pDlgAlarmList->OptimizeColumnWidth();
}
