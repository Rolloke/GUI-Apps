// SearchResultDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "SearchResultDialog.h"


#include "RechercheDoc.h"
#include "RechercheView.h"
#include "ObjectView.h"
#include "Server.h"
#include "CIPCDatabaseRecherche.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchResultDialog dialog


CSearchResultDialog::CSearchResultDialog(CObjectView* pParent)
	: CWK_Dialog(CSearchResultDialog::IDD, pParent)
{
	m_pObjectView = pParent;
	m_iFound = 0;
	m_iCopied = 0;
	//{{AFX_DATA_INIT(CSearchResultDialog)
	m_sQuery = _T("");
	m_sServers = _T("");
	m_sElapsed = _T("00:00:00");
	//}}AFX_DATA_INIT
	m_dwStart = 0;
	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
}

CSearchResultDialog::~CSearchResultDialog()
{
	m_QueryResultsToShow.SafeDeleteAll();
	m_QueryResultsInserted.SafeDeleteAll();
}


void CSearchResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchResultDialog)
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


BEGIN_MESSAGE_MAP(CSearchResultDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CSearchResultDialog)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESULTS, OnDblclkListResults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::OnCancel() 
{
	m_pObjectView->GetDocument()->StopSearch();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::OnOK() 
{
	m_pObjectView->GetDocument()->StopSearchCopy();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSearchResultDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);

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
CString CSearchResultDialog::GetSearchName()
{
	return m_sQuery;
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::InitFields()
{
	BOOL bDisconnect = FALSE;

	CRechercheDoc* pDoc = m_pObjectView->GetDocument();
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

	for (i=0;i<pDoc->GetServers().GetSize();i++)
	{
		CServer* pServer = pDoc->GetServers().GetAt(i);
		if (   pServer 
			&& pServer->IsConnected()
			&& pServer->GetQueryArchives().GetArchives().GetSize())
		{
			if (!m_sServers.IsEmpty())
			{
				m_sServers += _T(", ");
			}
			m_sServers += pServer->GetName();

			for (j=0;j<pServer->GetFields().GetSize();j++)
			{
				CIPCField* pField = pServer->GetFields().GetAt(j);
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
		CIPCField* pField = m_Fields.GetAt(i);
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
void CSearchResultDialog::PostNcDestroy() 
{
	m_pObjectView->m_pSearchResultDialog = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::Disconnect(DWORD dwServerID)
{
	TRACE(_T("search result disconnect %d\n"),dwServerID);
	if (!m_pObjectView->GetDocument()->GetServers().IsSearching())
	{
		DestroyWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSearchResultDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
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
	
	return CWK_Dialog::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::AddResult(DWORD dwUserID, WORD wServer,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr,
		           int iNumFields, const CIPCField fields[])
{
	m_QueryResultsToShow.SafeAddTail(new CQueryResult(dwUserID,wServer,wArchivNr,wSequenceNr,
		dwRecordNr,iNumFields,fields));
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::AddResult(WORD wServer, const CIPCPictureRecord &pictData)
{
	m_QueryResultsToShow.SafeAddTail(new CQueryResult(wServer,pictData));
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::InsertQueryResult(CQueryResult* pQueryResult)
{
	CServer* pServer = m_pObjectView->GetDocument()->GetServer(pQueryResult->GetServer());
	int i=-1;
	int c;
	CString s;

	c = m_Results.GetItemCount();

	if (c>1000)
	{
		m_Results.DeleteAllItems();
	}

	if (pServer && pServer->IsConnected())
	{
		s = pServer->GetName();
		i = m_Results.InsertItem(c,s);
		CQueryResult* pQueryResultCopy = new CQueryResult(*pQueryResult);
		m_Results.SetItemData(i,(DWORD)pQueryResultCopy);
		m_QueryResultsInserted.Add(pQueryResultCopy);
		if (pQueryResult->GetArchiveNr()!=0)
		{
			s = pServer->GetArchivName(pQueryResult->GetArchiveNr());
			m_Results.SetItemText(i,1,s);
		}

		int j;
		for (j=0;j<m_Fields.GetSize();j++)
		{
			CIPCField* pField;
			CIPCField* pFieldResult;
			pField = m_Fields.GetAt(j);
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
			 // void CIPCDatabaseRecherche::OnIndicateNewQueryResult(..)
			theApp.GetQueryParameter().ShowPictures())
		{
			CRechercheView* pRV = m_pObjectView->GetDocument()->GetRechercheView();

			if (pRV)
			{
				CDisplayWindow* pDW= pRV->GetDisplayWindow(
									  pServer->GetID(),
									  pQueryResult->GetArchiveNr(),
									  pQueryResult->GetSequenceNr());
				if (pDW==NULL)
				{
					OpenQueryResult(pQueryResult);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::OnTimer(UINT nIDEvent) 
{
	DWORD d = GetTickCount()-m_dwStart;
	DWORD s,m,h;

	d /= 1000;
	s = d % 60;
	d /= 60;
	m = d % 60;
	h = d / 60;

	m_sElapsed.Format(_T("%02d:%02d:%02d"),h,m,s);
	UpdateData(FALSE);
	
	CWK_Dialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::OnDblclkListResults(NMHDR* pNMHDR, LRESULT* pResult) 
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
void CSearchResultDialog::OpenQueryResult(CQueryResult* pQueryResult)
{
	CServer* pServer = m_pObjectView->GetDocument()->GetServer(pQueryResult->GetServer());

	if (pServer && pServer->IsConnected())
	{
		CIPCDatabaseRecherche* pDatabase = pServer->GetDatabase();
		
		if (pDatabase->GetVersion()>2)
		{
			CRechercheView* pRV = m_pObjectView->GetDocument()->GetRechercheView();

			if (pRV)
			{
				CDisplayWindow* pDW = pRV->GetDisplayWindow(pServer->GetID(),
														    pQueryResult->GetArchiveNr(),
														    pQueryResult->GetSequenceNr());
				if (pDW)
				{
					pDW->Navigate(pQueryResult->GetRecordNr());
				}
				else
				{
					pServer->OpenSequence(pQueryResult->GetArchiveNr(),
										  pQueryResult->GetSequenceNr(),
										  pQueryResult->GetRecordNr());
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::OnSearchResult()
{
	CString sFound;
	sFound.Format(_T("%d"),++m_iFound);
	m_staticFound.SetWindowText(sFound);
	while (m_QueryResultsToShow.GetCount())
	{
		CQueryResult* pResult = m_QueryResultsToShow.SafeGetAndRemoveHead();
		if (pResult)
		{
			InsertQueryResult(pResult);
			WK_DELETE(pResult);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::OnSearchResultCopied()
{
	CString s;
	s.Format(_T("%d"),++m_iCopied);
	m_staticCopied.SetWindowText(s);
	TestForDisconnect();
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::TestForDisconnect()
{
	if (!m_pObjectView->GetDocument()->GetServers().IsSearching())
	{
		if (m_iCopied>=m_iFound)
		{
			if (m_btnDisconnect.GetCheck())
			{
				WK_TRACE(_T("disconnect all by seach finished\n"));
				// this disconnect will destroy myself
				m_pObjectView->GetDocument()->DisconnectRemote();

			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSearchResultDialog::OnSearchFinished()
{
	if (!m_pObjectView->GetDocument()->GetServers().IsSearching())
	{
		m_AnimateSearch.Stop();
		KillTimer(1);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		UpdateData();
		TestForDisconnect();
	}
}
