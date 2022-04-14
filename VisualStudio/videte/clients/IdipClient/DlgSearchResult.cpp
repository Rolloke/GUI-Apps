// DlgSearchResult.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"

#include "DlgSearchResult.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewArchive.h"

#include "Server.h"
#include "CIPCDatabaseIdipClient.h"
#include "WndPlay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSearchResult dialog
CDlgSearchResult::CDlgSearchResult(CViewArchive* pParent)
	: CSkinDialog(CDlgSearchResult::IDD, pParent)
{
	m_pViewArchive = pParent;
	m_iFound = 0;
	m_iCopied = 0;
	//{{AFX_DATA_INIT(CDlgSearchResult)
	m_sQuery = _T("");
	m_sServers = _T("");
	m_sElapsed = _T("00:00:00");
	//}}AFX_DATA_INIT
	m_dwStart = 0;

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		MoveOnClientAreaClick(true);
	}
	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
}

CDlgSearchResult::~CDlgSearchResult()
{
	m_QueryResultsToShow.SafeDeleteAll();
	m_QueryResultsInserted.SafeDeleteAll();
}


void CDlgSearchResult::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSearchResult)
	DDX_Control(pDX, IDC_COPIED, m_staticCopied);
	DDX_Control(pDX, IDC_CHECK_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_LIST_RESULTS, m_Results);
	DDX_Control(pDX, IDC_TXT_FOUND, m_staticFound);
	DDX_Control(pDX, IDC_ANIMATE_SEARCH, m_AnimateSearch);
	DDX_Text(pDX, IDC_EDIT_QUERY, m_sQuery);
	DDX_Text(pDX, IDC_EDIT_SEARCH_IN, m_sServers);
	DDX_Text(pDX, IDC_ELAPSED, m_sElapsed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSearchResult, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgSearchResult)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESULTS, OnDblclkListResults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OnCancel() 
{
	m_pViewArchive->GetDocument()->StopSearch();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OnOK() 
{
	m_pViewArchive->GetDocument()->StopSearchCopy();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgSearchResult::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	CMenu *pMenu = GetSystemMenu(FALSE);
	if(pMenu)
	{
		pMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
	}

	InitFields();

	m_AnimateSearch.Open(IDR_SEARCH);
	m_AnimateSearch.Play(0,((UINT)-1),((UINT)-1));
	m_AnimateSearch.RedrawWindow();

	m_dwStart = GetTickCount();
	SetTimer(1,1000,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
CString CDlgSearchResult::GetSearchName()
{
	return m_sQuery;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::InitFields()
{
	BOOL bDisconnect = FALSE;

	CIdipClientDoc* pDoc = m_pViewArchive->GetDocument();
	int i,j;
	CString sAnd;

	m_sQuery.Empty();
	sAnd.LoadString(IDS_AND);
	sAnd = _T(" ") + sAnd + _T(" ");
	m_sServers.Empty();

#ifdef _DEBUG //TKR
	//TODO TKR hier den Fenstertext ändern mit Language-abh. Variablen
	if(theApp.GetQueryParameter().GetRectToQuery().IsRectNull())
	{
//		SetWindowText(IDC_SEARCH_RESULT_RECTANGLE);
	}
	else
	{
//		SetWindowText(IDC_SEARCH_RESULT);
	}
#endif

	if(theApp.GetQueryParameter().UseTimeFromTo())
	{
		BOOL bExactDate = theApp.GetQueryParameter().UseExactDate();
		BOOL bExactTime = theApp.GetQueryParameter().UseExactTime();

		if (bExactDate && bExactTime)
		{
			CString s,t;
			s.LoadString(IDS_REC_DATE);
			t.LoadString(IDS_REC_TIME);
			CString sDateTime;
			sDateTime = s + _T("/") + t.Right(t.GetLength()-3);

			m_sQuery += _T("\r\n") + sDateTime + _T(" >= ") + theApp.GetQueryParameter().GetDateExact().GetDate();
			m_sQuery += _T(" - ") + theApp.GetQueryParameter().GetTimeExact().GetTime();
			m_sQuery += sAnd + sDateTime + _T(" <= ");
			m_sQuery += theApp.GetQueryParameter().GetPCDateEnd().GetDate() + _T(" - ");
			m_sQuery += theApp.GetQueryParameter().GetTimeEnd().GetTime();
		}

		else if (bExactDate)
		{
			CString s;
			s.LoadString(IDS_REC_DATE);

			m_sQuery += s + _T(" >= ") + theApp.GetQueryParameter().GetDateExact().GetDate();
			m_sQuery += sAnd + s + _T(" <= ") + theApp.GetQueryParameter().GetPCDateEnd().GetDate();
		}
	}
	else
	{
		if (theApp.GetQueryParameter().UseExactDate())
		{
			CString s;
			s.LoadString(IDS_REC_DATE);
			if (!m_sQuery.IsEmpty())
			{
				m_sQuery += sAnd;
			}
			m_sQuery += s + _T(" = ") + theApp.GetQueryParameter().GetDateExact().GetDate();
		}
		if (theApp.GetQueryParameter().UseExactTime())
		{
			CString s;
			s.LoadString(IDS_REC_TIME);
			if (!m_sQuery.IsEmpty())
			{
				m_sQuery += sAnd;
			}
			m_sQuery += s + _T(" = ") + theApp.GetQueryParameter().GetTimeExact().GetTime();
		}
	}

	CServers*pServers = (CServers*)&pDoc->GetServers();
	CAutoCritSec acs(&pServers->m_cs);
	for (i=0;i<pServers->GetSize();i++)
	{
		CServer* pServer = pServers->GetAtFast(i);
		if (   pServer 
			&& pServer->IsConnectedPlay()
			&& pServer->GetQueryArchives().GetArchives().GetSize())
		{
			if (!m_sServers.IsEmpty())
			{
				m_sServers += _T(", ");
			}
			m_sServers += pServer->GetName();

			for (j=0;j<pServer->GetFields().GetSize();j++)
			{
				CIPCField* pField = pServer->GetFields().GetAtFast(j);
				if (NULL == m_Fields.GetCIPCField(pField->GetName()))
				{
					CString s = theApp.GetMappedString(pField->GetName());
					if (!s.IsEmpty())
					{
						CIPCField* pNewField = new CIPCField(*pField);
						pNewField->SetValue(s);
						m_Fields.SafeAdd(pNewField);

						CIPCField* pQueryField = theApp.GetQueryParameter().GetFields().GetCIPCField(pField->GetName());
						if (pQueryField)
						{
							if (!m_sQuery.IsEmpty())
							{
								m_sQuery += sAnd;
							}
							m_sQuery += s + _T(" = ") + pQueryField->ToString();
						}
					}
				}
			}
		}
		if (!pServer->IsLocal() && theApp.GetQueryParameter().CopyPictures())
		{
			bDisconnect = TRUE;
		}
	}
	acs.Unlock();
//	GetDlgItem(IDC_CHECK_DISCONNECT)->ShowWindow(bDisconnect ? SW_SHOW : SW_HIDE);
	m_btnDisconnect.ShowWindow(bDisconnect ? SW_SHOW : SW_HIDE);


	CRect rect;
	int c = m_Fields.GetSize();
	m_Results.GetClientRect(rect);

	m_Results.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	int iw,d;
	CString s;

	iw = rect.Width()/(c+2);
	s.LoadString(IDS_STATION2);
	m_Results.InsertColumn(0,s,LVCFMT_LEFT,iw);
	s.LoadString(IDS_ARCHIVE);
	m_Results.InsertColumn(1,s,LVCFMT_LEFT,iw);

	for (i=0;i<c;i++)
	{
		CIPCField* pField = m_Fields.GetAtFast(i);
		d = m_Results.GetHeaderCtrl()->GetItemCount();
		m_Results.InsertColumn(d+1,pField->GetValue(),LVCFMT_LEFT,iw);
	}

	CString sSearch;
	CSystemTime st;
	st.GetLocalTime();

//TODO TKR 
	//hier den Text ändern in Bereichsuche vom... bei RectangleSearch
	sSearch.Format(IDS_SEARCH_FROM,st.GetDateTime(),m_sQuery);
	m_sQuery = sSearch;

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::PostNcDestroy() 
{
	m_pViewArchive->m_pDlgSearchResult = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::Disconnect(DWORD dwServerID)
{
	TRACE(_T("search result disconnect %d\n"),dwServerID);
	if (!m_pViewArchive->GetDocument()->GetServers().IsSearching())
	{
		DestroyWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgSearchResult::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
	case ID_SEARCH_RESULT:
		OnSearchResult();
		return TRUE;
		break;
	case ID_SEARCH_CANCELED:
		return TRUE;
		break;
	case ID_SEARCH_RESULT_COPY:
		OnSearchResultCopied();
		return TRUE;
		break;
	case ID_SEARCH_FINISHED:
		OnSearchFinished();
		return TRUE;
		break;
	default:
		break;
	}
	
	return CSkinDialog::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::AddResult(DWORD dwUserID, WORD wServer,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr,
		           int iNumFields, const CIPCField fields[])
{
	m_QueryResultsToShow.SafeAddTail(new CQueryResult(dwUserID,wServer,wArchivNr,wSequenceNr,
		dwRecordNr,iNumFields,fields));
	PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::AddResult(WORD wServer, const CIPCPictureRecord &pictData)
{
	m_QueryResultsToShow.SafeAddTail(new CQueryResult(wServer,pictData));
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::InsertQueryResult(CQueryResult* pQueryResult)
{
	CServer* pServer = m_pViewArchive->GetDocument()->GetServer(pQueryResult->GetServer());
	int i=-1;
	int c;
	CString s;

	c = m_Results.GetItemCount();

	if (c>1000)
	{
		m_Results.DeleteAllItems();
	}

	if (pServer && pServer->IsConnectedPlay())
	{
		s = pServer->GetName();
		i = m_Results.InsertItem(c,s);
		CQueryResult* pQueryResultCopy = new CQueryResult(*pQueryResult);
		m_Results.SetItemData(i,(DWORD)pQueryResultCopy);
		m_QueryResultsInserted.Add(pQueryResultCopy);
		if (pQueryResult->GetArchiveNr()!=0)
		{
			s = pServer->GetArchiveName(pQueryResult->GetArchiveNr());
			m_Results.SetItemText(i,1,s);
		}

		int j;
		for (j=0;j<m_Fields.GetSize();j++)
		{
			CIPCField* pField;
			CIPCField* pFieldResult;
			pField = m_Fields.GetAtFast(j);
			pFieldResult = pQueryResult->GetFields().GetCIPCField(pField->GetName());
			{
				if (pFieldResult)
				{
					m_Results.SetItemText(i,2+j,pFieldResult->ToString());
				}
			}
		}

		// should we open it ?
		if (// !theApp.GetQueryParameter().CopyPictures() && 
			 // RKE: Es sind zwar nicht dieselben, aber die gleichen
			 // Die richtige Stelle ist wahrscheinlich:
			 // void CIPCDatabaseIdipClient::OnIndicateNewQueryResult(..)
			theApp.GetQueryParameter().ShowPictures())
		{
			CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();

			if (pRV)
			{
				CWndPlay* pDW= pRV->GetWndPlay(
									  pServer->GetID(),
									  pQueryResult->GetArchiveNr(),
									  pQueryResult->GetSequenceNr());


				//open WndPlay if it shows not the correct sequence
				
				if (pDW==NULL)
				{
					OpenQueryResult(pQueryResult);
				}
				else
				{
					//if sequence is already open jump to the new founded picture
					if(c==0)	//first item found
					{
						OpenQueryResult(pQueryResult);
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OnTimer(UINT nIDEvent) 
{
	int d = GetTimeSpanSigned(m_dwStart);
	DWORD s,m,h;

	d /= 1000;
	s = d % 60;
	d /= 60;
	m = d % 60;
	h = d / 60;

	m_sElapsed.Format(_T("%02d:%02d:%02d"),h,m,s);
	UpdateData(FALSE);
	
	CSkinDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OnDblclkListResults(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!theApp.GetQueryParameter().ShowPictures())
	{
		int i,c = m_Results.GetItemCount();
		CQueryResult* pQueryResult = NULL;

		for (i=0;i<c;i++)
		{
			if (LVIS_SELECTED == m_Results.GetItemState(i,LVIS_SELECTED))
			{
				pQueryResult = (CQueryResult*)m_Results.GetItemData(i);
				break;
			}
		}

		if (pQueryResult)
		{
			OpenQueryResult(pQueryResult);
		}
	}

	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OpenQueryResult(CQueryResult* pQueryResult)
{
	CServer* pServer = m_pViewArchive->GetDocument()->GetServer(pQueryResult->GetServer());

	if (   pServer 
		&& pServer->IsConnectedPlay())
	{
		CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();

		if (pRV)
		{
			CWndPlay* pDW = pRV->GetWndPlay(pServer->GetID(),
											pQueryResult->GetArchiveNr(),
											pQueryResult->GetSequenceNr());
			if (pDW)
			{
				// Fenster zu Suchergebnis gefunden 
				// das Navigate fragt das Bild an
				// im Confirm wird das Bild angezeigt
				// und kopiert falls der Haken an
				pDW->Navigate(pQueryResult->GetRecordNr());
			}
			else
			{
				// gibt es zu diesem Archiv schon ein passendes Fenster
				// z.B. wenn die Suche über mehrere Sequenzen Resultate liefert
				CWndPlay* pDWSameArchive = pRV->GetWndPlay(pServer->GetID(),
															pQueryResult->GetArchiveNr(),
															0);
				if (pDWSameArchive)
				{
					// TODO UF TKR
					//pDWSameArchive->Navigate(pQueryResult->GetRecordNr());
				}
				else
				{
					// nix offen, dann machen wir halt ein neues Fenster auf.
					pServer->OpenSequence(pQueryResult->GetArchiveNr(),
											pQueryResult->GetSequenceNr(),
											pQueryResult->GetRecordNr());
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OnSearchResult()
{
	if (m_QueryResultsToShow.GetCount())
	{
		CQueryResult* pResult = m_QueryResultsToShow.SafeGetAndRemoveHead();
		if (pResult)
		{
			InsertQueryResult(pResult);
			WK_DELETE(pResult);
		}
	}
	CString sFound;
	sFound.Format(_T("%d"),++m_iFound);
	m_staticFound.SetWindowText(sFound);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OnSearchResultCopied()
{
	CString s;
	s.Format(_T("%d"),++m_iCopied);
	m_staticCopied.SetWindowText(s);
	TestForDisconnect();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::TestForDisconnect()
{
	if (!m_pViewArchive->GetDocument()->GetServers().IsSearching())
	{
		if (m_iCopied>=m_iFound)
		{
			if (m_btnDisconnect.GetCheck())
			{
				WK_TRACE(_T("disconnect all by seach finished\n"));
				// this disconnect will destroy myself
				m_pViewArchive->GetDocument()->DisconnectRemote();

			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSearchResult::OnSearchFinished()
{
	if (!m_pViewArchive->GetDocument()->GetServers().IsSearching())
	{
		m_AnimateSearch.Stop();
		KillTimer(1);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		UpdateData();
		TestForDisconnect();
	}
}
