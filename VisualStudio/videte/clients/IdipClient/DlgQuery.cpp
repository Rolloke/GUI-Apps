// DlgQuery.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgQuery.h"

#include "MainFrm.h"
#include "IdipClientDoc.h"
#include "ViewArchive.h"
#include "WndPlay.h"
#include ".\dlgquery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQuery dialog


CDlgQuery::CDlgQuery(CViewArchive* pParent /*=NULL*/)
	: CSkinDialog(CDlgQuery::IDD, pParent)
{
	m_pViewArchive = pParent;

	m_bExactDate = theApp.GetQueryParameter().UseExactDate();
	m_bExactTime = theApp.GetQueryParameter().UseExactTime();
	m_bTimeFromTo = theApp.GetQueryParameter().UseTimeFromTo();

	m_bShowPictures = theApp.GetQueryParameter().ShowPictures();
	m_bCopyPictures = theApp.GetQueryParameter().CopyPictures();
	m_bAll = theApp.GetQueryParameter().QueryAll();

	m_iServersChecked = 0;

	//{{AFX_DATA_INIT(CDlgQuery)
	m_iScroll = 0;
	m_bSearchArea = FALSE;
	//}}AFX_DATA_INIT

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		// MoveOnClientAreaClick(false);
	}
	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
CDlgQuery::~CDlgQuery()
{
	m_FieldNames.SafeDeleteAll();
	m_FieldValues.SafeDeleteAll();
}


void CDlgQuery::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQuery)
	DDX_Control(pDX, IDC_CHECK_ALL, m_ctrlAll);
	DDX_Control(pDX, IDC_SEARCHAREA, m_ctrlSearchArea);
	DDX_Control(pDX, IDC_CHECK_EXACT_TIME, m_ctrlCheckTime);
	DDX_Control(pDX, IDC_CHECK_DATE, m_ctrlCheckDate);
	DDX_Control(pDX, IDC_CHECK_TIME_FROM, m_ctrlCheckTimeFrom);
	DDX_Control(pDX, IDC_PCDATEEND_TEXT, m_ctrlPCDateEndText);
	DDX_Control(pDX, IDC_TIMEEND_TEXT, m_ctrlTimeEndText);
	DDX_Control(pDX, IDC_TIME_END, m_ctrlTimeEnd);
	DDX_Control(pDX, IDC_PCDATE_END, m_ctrlPCDateEnd);
	DDX_Control(pDX, IDC_CHECK_COPY, m_ctrlCopyPictures);
	DDX_Control(pDX, IDC_GROUP_TIME, m_grpPCTime);
	DDX_Control(pDX, IDC_GRP_FIELDS, m_grpFields);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_SCROLLBAR_FIELDS, m_ScrollBar);
	DDX_Control(pDX, IDC_TIME_EXACT, m_ctrlExactTime);
	DDX_Control(pDX, IDC_PCDATE, m_ctrlPCDate);
	DDX_Control(pDX, IDC_SERVER_ARCHIVES, m_ServerArchives);
	DDX_Check(pDX, IDC_CHECK_ALL, m_bAll);
	DDX_Check(pDX, IDC_CHECK_OPEN, m_bShowPictures);
	DDX_Check(pDX, IDC_CHECK_COPY, m_bCopyPictures);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bExactDate);
	DDX_Check(pDX, IDC_CHECK_EXACT_TIME, m_bExactTime);
	DDX_Check(pDX, IDC_CHECK_TIME_FROM, m_bTimeFromTo);
	DDX_Scroll(pDX, IDC_SCROLLBAR_FIELDS, m_iScroll);
	DDX_Check(pDX, IDC_SEARCHAREA, m_bSearchArea);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQuery, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgQuery)
	ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckAll)
	ON_NOTIFY(NM_CLICK, IDC_SERVER_ARCHIVES, OnClickServerArchives)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_BN_CLICKED(IDC_CHECK_EXACT_TIME, OnCheckExactTime)
	ON_BN_CLICKED(IDC_CHECK_TIME_FROM, OnCheckTimeFrom)
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQuery message handlers

void CDlgQuery::PostNcDestroy() 
{
	m_pViewArchive->m_pDlgQuery = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgQuery::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinButtons();
	// TODO GF Workaround Resource Editor Bug
	m_ctrlExactTime.ModifyStyle(NULL, DTS_TIMEFORMAT);
	m_ctrlTimeEnd.ModifyStyle(NULL, DTS_TIMEFORMAT);

	CMenu *pMenu = GetSystemMenu(FALSE);
	if (pMenu)
	{
		pMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
	}

	m_States.Create(IDB_STATES,14,0,RGB(0,255,255));
	m_ServerArchives.SetImageList(&m_States,TVSIL_STATE);

	//ist eine Sequenz geöffnet ?
	CWndSmall* pSW = NULL;
	CViewIdipClient *pVIC = theApp.GetMainFrame()->GetViewIdipClient();
	pVIC->LockSmallWindows(_T(__FUNCTION__));
	int iSize = pVIC->GetSmallWindows()->GetSize();
	BOOL bShowHideRect = FALSE;
	if(iSize)
	{
		CWndPlay* pDW = NULL;
		//changes for VisualStudio 2005
		int i = 0;
		for(i=0; i < iSize; i++)
		{
			pSW = pVIC->GetSmallWindows()->GetAtFast(i);
			if(pSW && pSW->IsWndPlay())
			{
				pDW = (CWndPlay*)pSW;
				if(pDW && !pDW->GetRectToQuery().IsRectEmpty())
				{
					bShowHideRect = TRUE;
					break;
				}
			}
		}
	}
	pVIC->UnlockSmallWindows();
	ShowHideRectangle(bShowHideRect);						

	InitFields();

	m_ctrlPCDate.SetFormat(_T("ddd, dd. MMM yyy"));
	m_ctrlPCDateEnd.SetFormat(_T("ddd, dd. MMM yyy"));

	if (m_bExactDate)
	{
		CSystemTime t = theApp.GetQueryParameter().GetDateExact();
		m_ctrlPCDate.SetTime(&t);
	}
	if (m_bExactTime)
	{
		CSystemTime t = theApp.GetQueryParameter().GetTimeExact();
		m_ctrlExactTime.SetTime(&t);
	}
	if (m_bTimeFromTo)
	{
		CSystemTime f,t;
		f = theApp.GetQueryParameter().GetPCDateEnd();
		t = theApp.GetQueryParameter().GetTimeEnd();
		m_ctrlPCDateEnd.SetTime(&f);
		m_ctrlTimeEnd.SetTime(&t);
	}

	ShowHide();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::InitFields()
{
	CIdipClientDoc* pDoc = m_pViewArchive->GetDocument();
	CServers*pServers = (CServers*)&pDoc->GetServers();
	int i,j;
	BOOL bConnectedPlay = FALSE;
	pServers->Lock();
	for (i=0;i<pServers->GetSize();i++)
	{
		CServer* pServer = pServers->GetAtFast(i);
		if (pServer && pServer->IsConnectedPlay())
		{
			for (j=0;j<pServer->GetFields().GetSize();j++)
			{
				CIPCField* pField = pServer->GetFields().GetAtFast(j);
				if (NULL == m_FieldNames.GetCIPCField(pField->GetName()))
				{
					m_FieldNames.SafeAdd(new CIPCField(*pField));
				}
			}
			bConnectedPlay = TRUE;
		}
	}
	pServers->Unlock();

	if (bConnectedPlay)
	{
		for (j=0;j<theApp.GetQueryParameter().GetFields().GetSize();j++)
		{
			CIPCField* pField = theApp.GetQueryParameter().GetFields().GetAtFast(j);
			if (NULL == m_FieldValues.GetCIPCField(pField->GetName()))
			{
				m_FieldValues.SafeAdd(new CIPCField(*pField));
			}
		}
	}

	CRect rect;
	int c = m_FieldNames.GetSize();
	CString sName;
	CDlgQueryField* pQueryFieldDialog;

	for (i=0;i<c;i++)
	{
		CIPCField* pFieldName = m_FieldNames.GetAtFast(i);
		
		sName = theApp.GetMappedString(pFieldName->GetName());
		if (sName.IsEmpty())
		{

#ifdef _DTS
			if	(   (pFieldName->GetName() == CIPCField::m_sfStNm)
				 || (pFieldName->GetName() == CIPCField::m_sfCaNm)
				)
			{
				sName = pFieldName->GetValue();
			}
#else
			if	(   (pFieldName->GetName() == CIPCField::m_sfStNm)
				 || (pFieldName->GetName() == CIPCField::m_sfCaNm)
				 || (pFieldName->GetName() == CIPCField::m_sfInNm)
				)
			{
				sName = pFieldName->GetValue();
			}
#endif
		}

		if (!sName.IsEmpty())
		{
			CIPCField* pFieldValue = m_FieldValues.GetCIPCField(pFieldName->GetName());
			if (pFieldValue == NULL)
			{
				pFieldValue = new CIPCField(*pFieldName);
				pFieldValue->SetValue(_T(""));
				m_FieldValues.Add(pFieldValue);
			}
			if (pFieldName->IsDate())
			{
				pQueryFieldDialog = new CDlgQueryFieldDate(sName,pFieldValue,this);
			}
			else if (pFieldName->IsTime())
			{
				pQueryFieldDialog = new CDlgQueryFieldTime(sName,pFieldValue,this);
			}
			else if (pFieldName->IsNumeric())
			{
				pQueryFieldDialog = new CDlgQueryFieldNum(sName,pFieldValue,this);
			}
			else
			{
				pQueryFieldDialog = new CDlgQueryFieldChar(sName,pFieldValue,this);
			}
			m_QueryFieldDialogs.Add(pQueryFieldDialog);
		}
	}

	m_ScrollBar.SetScrollRange(0,m_QueryFieldDialogs.GetSize());
	SetPageSize(m_QueryFieldDialogs.GetSize());

	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::Resize()
{
	CDlgQueryField* pQueryFieldDialog;
	int i,c,height,off,left,width;
	int n;
	CRect scrollrect,rect;

	c = m_QueryFieldDialogs.GetSize();

	m_ScrollBar.GetWindowRect(scrollrect);
	m_grpPCTime.GetWindowRect(rect);
	ScreenToClient(rect);
	ScreenToClient(scrollrect);

	off = scrollrect.top;
	left = rect.left;
	width = scrollrect.left - rect.left;

	int ypos = 0;
	int iOneHidden = 0;

	if (c>0)
	{
		pQueryFieldDialog = m_QueryFieldDialogs.GetAtFast(0);
		pQueryFieldDialog->GetClientRect(rect);

		n = scrollrect.Height() / rect.Height();
		SetPageSize(n);

		for (i=0;i<c;i++)
		{
			pQueryFieldDialog = m_QueryFieldDialogs.GetAtFast(i);
			pQueryFieldDialog->GetClientRect(rect);
			height = rect.Height();
			
			if (   (i<m_iScroll)
				|| ((ypos+height)>scrollrect.bottom)
				)
			{
				pQueryFieldDialog->ShowWindow(SW_HIDE);
				iOneHidden++;
			}
			else
			{
				rect.top = ypos+off;
				rect.left = left;
				rect.right = left + width;
				rect.bottom = rect.top + height;
				if (rect.bottom<scrollrect.bottom)
				{
					pQueryFieldDialog->MoveWindow(rect);
					pQueryFieldDialog->ShowWindow(SW_SHOW);
				}
				ypos += height;
			}
		}
	}

	m_ScrollBar.ShowWindow((iOneHidden>0) ? SW_SHOW : SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::Disconnect(DWORD dwServerID)
{
	WK_TRACE(_T("query disconnect %d\n"),dwServerID);
	HTREEITEM hServer = m_ServerArchives.GetRootItem();
	HTREEITEM hToDelete = NULL;

	CIdipClientDoc* pDoc = m_pViewArchive->GetDocument();
	CServers*pServers = (CServers*)&pDoc->GetServers();
	pServers->Lock();
	for (int i=0;i<pServers->GetSize();i++)
	{
		CServer* pServer = pServers->GetAtFast(i);
		if (pServer && pServer->GetID() == (WORD)dwServerID)
		{
			if (pServer->IsLocal())
			{
				GetDlgItem(IDC_CHECK_COPY)->EnableWindow(FALSE);
			}
			break;
		}
	}
	pServers->Unlock();

	while (hServer)
	{
		hToDelete = NULL;
		if (m_ServerArchives.GetItemData(hServer)==dwServerID)
		{
			hToDelete = hServer;
		}
		hServer = m_ServerArchives.GetNextSiblingItem(hServer);

		if (hToDelete)
		{
			m_ServerArchives.DeleteItem(hToDelete);
		}
	}

	hServer = m_ServerArchives.GetRootItem();
	if (hServer==NULL)
	{
		DestroyWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::InitServerArchives()
{
	if (NULL!=m_ServerArchives.GetRootItem())
	{
		Parameters2QueryParameter();
	}
	m_ServerArchives.DeleteAllItems();

	CIdipClientDoc* pDoc = m_pViewArchive->GetDocument();
	CServers*pServers = (CServers*)&pDoc->GetServers();
	int i,j;
	TVINSERTSTRUCT is;
	CString sName;
	BOOL bConnectedLocal = pDoc->GetLocalDatabase()!=NULL;

	ZeroMemory(&is,sizeof(is));
	CAutoCritSec acs(&pServers->m_cs);
	for (i=0;i<pServers->GetSize();i++)
	{
		CServer* pServer = pServers->GetAtFast(i);
		if (pServer)
		{
			if (pServer->IsConnectedPlay())
			{
				is.hParent = TVI_ROOT;
				is.hInsertAfter = TVI_LAST;
				is.item.mask = TVIF_TEXT | TVIF_PARAM;

				CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
				if (pDatabase==NULL)
				{
					continue;
				}
				CIPCArchivRecords& records = pDatabase->GetRecords();
				
				
				HTREEITEM hServer;

				sName = pServer->GetName();
				is.hParent = TVI_ROOT;
				is.item.pszText = sName.GetBuffer(0);
				is.item.lParam = pServer->GetID();
				hServer = m_ServerArchives.InsertItem(&is);
				sName.ReleaseBuffer();

				SetCheck(m_ServerArchives,hServer,FALSE);

				is.hParent = hServer;
				int iChecked = 0;

				records.Lock(_T(__FUNCTION__));
				for (j=0;j<records.GetSize();j++)
				{
					CIPCArchivRecord* pArchiv;
					HTREEITEM hArchiv;

					pArchiv = records.GetAtFast(j);
					if (   pArchiv->GetID()!=255
						&& !pArchiv->IsAlarmList()
						)
					{
						sName = pArchiv->GetName();
						is.item.pszText = sName.GetBuffer(0);
						is.item.lParam = pArchiv->GetID();
						hArchiv = m_ServerArchives.InsertItem(&is);
						sName.ReleaseBuffer();
						BOOL bCheck = pServer->GetQueryArchives().IsArchive(pArchiv->GetID());
						SetCheck(m_ServerArchives,hArchiv,bCheck);
						if (bCheck)
						{
							iChecked++;
						}
					}
				}
				if (iChecked>0)
				{
					m_iServersChecked++;
					TRACE(_T("TKR ----------- m_iServersChecked++  InitServerArchives()\n"));
					SetCheck(m_ServerArchives,hServer,TRUE);
				}
				else
				{
					SetCheck(m_ServerArchives,hServer,FALSE);
				}
				records.Unlock();
			}
			else
			{
				WK_TRACE_WARNING(_T("try to insert disconnected server into query mask\n"));
			}
		}
	}
	acs.Unlock();
	m_ctrlCopyPictures.EnableWindow(bConnectedLocal);
}
/////////////////////////////////////////////////////////////////////////////
//checks logic of date and time 
void CDlgQuery::CheckDateTime() 
{
	if(m_bTimeFromTo) //search between 2 dates
	{
		CSystemTime stPCDate, stPCDateEnd, stExactTime, stTimeEnd;
		m_ctrlPCDate.GetTime(&stPCDate);
		m_ctrlPCDateEnd.GetTime(&stPCDateEnd);
		//set all time variables to 0
		stPCDate.wHour = stPCDate.wMinute = stPCDate.wSecond = stPCDate.wMilliseconds = 0;
		stPCDateEnd.wHour = stPCDateEnd.wMinute = stPCDateEnd.wSecond = stPCDateEnd.wMilliseconds = 0;

		m_ctrlExactTime.GetTime(&stExactTime);
		m_ctrlTimeEnd.GetTime(&stTimeEnd);

		if(CTime(stPCDate) > CTime(stPCDateEnd)) 
		{
			//StartDate > Enddate: switch PCDate and PCDateEnd
			CSystemTime stDummy;
			stDummy = stPCDateEnd;
			stPCDateEnd = stPCDate;
			stPCDate = stDummy;
			m_ctrlPCDate.SetTime(&stPCDate);
			m_ctrlPCDateEnd.SetTime(&stPCDateEnd);

			if(m_bExactTime)
			{
				//if time is selected, also switch StartTime and EndTime
				m_ctrlExactTime.SetTime(&stTimeEnd);
				m_ctrlTimeEnd.SetTime(&stExactTime);
			}
		}
		else if(CTime(stExactTime) > CTime(stTimeEnd))
		{	
			TRACE(_T("Test\n"));

		}
		if(CTime(stPCDate) == CTime(stPCDateEnd) && CTime(stExactTime) > CTime(stTimeEnd))
		{
			//StartDate == EndDate AND StartTime > EndTime: switch Times
			m_ctrlExactTime.SetTime(&stTimeEnd);
			m_ctrlTimeEnd.SetTime(&stExactTime);
		}
	}
	UpdateData();
}

/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnOK() 
{
	CheckDateTime();
	Parameters2QueryParameter();
	m_pViewArchive->ShowDlgSearchResult();
	StartSearch();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnCancel() 
{
	Parameters2QueryParameter();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnCheckAll() 
{
	UpdateData();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::StartSearch()
{
	HTREEITEM hServer = m_ServerArchives.GetRootItem();
	while (hServer)
	{
		WORD wServerID = (WORD)m_ServerArchives.GetItemData(hServer);
		CServer* pServer = m_pViewArchive->GetDocument()->GetServer(wServerID);
		if (pServer && pServer->IsConnectedPlay())
		{
			if (GetCheck(m_ServerArchives,hServer) || m_bAll)
			{
				pServer->StartSearch();
			}
		}
		hServer = m_ServerArchives.GetNextSiblingItem(hServer);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::Parameters2QueryParameter()
{
	UpdateData();

	// now fields
	theApp.GetQueryParameter().DeleteFields();
	for (int i=0;i<m_FieldValues.GetSize();i++)
	{
		CIPCField* pField = m_FieldValues.GetAtFast(i);
		if (pField && !pField->GetValue().IsEmpty())
		{
			theApp.GetQueryParameter().SetAddField(*pField);
		}
	}

	HTREEITEM hServer = m_ServerArchives.GetRootItem();

	while (hServer)
	{
		WORD wServerID = (WORD)m_ServerArchives.GetItemData(hServer);
		CServer* pServer = m_pViewArchive->GetDocument()->GetServer(wServerID);
		if (pServer && pServer->IsConnectedPlay())
		{
			// delete old archive items
			pServer->GetQueryArchives().DeleteArchives();
			if (GetCheck(m_ServerArchives,hServer) || m_bAll)
			{
				HTREEITEM hArchiv = m_ServerArchives.GetChildItem(hServer);
				while (hArchiv)
				{
					if (GetCheck(m_ServerArchives,hArchiv) || m_bAll)
					{
						WORD wArchivNr = (WORD)m_ServerArchives.GetItemData(hArchiv);
						pServer->GetQueryArchives().AddArchive(wArchivNr);
					}
					hArchiv = m_ServerArchives.GetNextSiblingItem(hArchiv);
				}

			}
		}

		hServer = m_ServerArchives.GetNextSiblingItem(hServer);
	}

	theApp.GetQueryParameter().SetAll(m_bAll);
	theApp.GetQueryParameter().SetCopyPictures(m_bCopyPictures);
	theApp.GetQueryParameter().SetShowPictures(m_bShowPictures);

	theApp.GetQueryParameter().SetExactDate(m_bExactDate);
	theApp.GetQueryParameter().SetExactTime(m_bExactTime);
	theApp.GetQueryParameter().SetTimeFromTo(m_bTimeFromTo);

	if (m_bExactDate)
	{
		CSystemTime t;
		m_ctrlPCDate.GetTime(&t);
		theApp.GetQueryParameter().SetExactDate(t);
	}
	if (m_bExactTime)
	{
		CSystemTime t;
		m_ctrlExactTime.GetTime(&t);
		theApp.GetQueryParameter().SetExactTime(t);
	}

	if (m_bTimeFromTo)
	{
		CSystemTime t,f;
		m_ctrlPCDateEnd.GetTime(&t);
		theApp.GetQueryParameter().SetPCDateEnd(t);
		m_ctrlTimeEnd.GetTime(&f);
		theApp.GetQueryParameter().SetTimeEnd(f);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnClickServerArchives(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hClicked,hParent;
	UINT nFlags;

	m_ServerArchives.ScreenToClient(&pt);
	hClicked = m_ServerArchives.HitTest(pt,&nFlags);

//	TRACE(_T("<%s>\n"),m_ServerArchives.GetItemText(hServer));

	//nur wenn kein Rechteck vorhanden ist, dann die Archive enablen, ansonsten soll beim 
	//anklicken nichts passieren
	BOOL bRectIsNull = theApp.GetQueryParameter().GetRectToQuery().IsRectNull();

	if (bRectIsNull && hClicked && (nFlags & TVHT_ONITEMSTATEICON))
	{
		BOOL bCheck = GetCheck(m_ServerArchives,hClicked);
		SetCheck(m_ServerArchives,hClicked,!bCheck);
		bCheck = GetCheck(m_ServerArchives,hClicked);
		hParent = m_ServerArchives.GetParentItem(hClicked);
		if (NULL==hParent)
		{
			if (!bCheck)
			{
				m_iServersChecked--;
			}
			else
			{
				m_iServersChecked++;
			}
			HTREEITEM hArchiv = m_ServerArchives.GetChildItem(hClicked);
			while (hArchiv)
			{
				SetCheck(m_ServerArchives,hArchiv,bCheck);
				hArchiv = m_ServerArchives.GetNextSiblingItem(hArchiv);
			}
		}
		else
		{
			if (bCheck)
			{
				SetCheck(m_ServerArchives,hParent,TRUE);
				m_iServersChecked++;
			}
		}
	}

	ShowHide();
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::ShowHide()
{
	m_ServerArchives.ShowWindow(m_bAll ? SW_HIDE : SW_SHOW);
	m_ctrlPCDate.ShowWindow(m_bExactDate ? SW_SHOW : SW_HIDE);
	m_ctrlExactTime.ShowWindow(m_bExactTime ? SW_SHOW : SW_HIDE);

	m_ctrlCheckTimeFrom.ShowWindow(m_bExactDate||m_bExactTime ? SW_SHOW : SW_HIDE);
	m_ctrlPCDateEnd.ShowWindow(m_bExactDate&&m_bTimeFromTo ? SW_SHOW : SW_HIDE);
	m_ctrlTimeEnd.ShowWindow(m_bExactTime&&m_bTimeFromTo ? SW_SHOW : SW_HIDE);
	m_ctrlPCDateEndText.ShowWindow(m_bExactDate&&m_bTimeFromTo ? SW_SHOW : SW_HIDE);
	m_ctrlTimeEndText.ShowWindow(m_bExactTime&&m_bTimeFromTo ? SW_SHOW : SW_HIDE);


	BOOL bRectIsNull = theApp.GetQueryParameter().GetRectToQuery().IsRectNull();
//	TRACE(_T("TKR ----------------------- RectIsNull: %i  All: %i  ServerChecked: %i\n"), !bRectIsNull,m_bAll,m_iServersChecked);
	m_ctrlOK.EnableWindow(m_bAll || (m_iServersChecked>0) || !bRectIsNull);

	m_ctrlCopyPictures.ShowWindow(theApp.IsReadOnlyModus() ? SW_HIDE : SW_SHOW);
	m_ctrlSearchArea.ShowWindow(SW_SHOW);

}

/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::ShowHideRectangle(BOOL bShow)
{
	//immer disablen, Benutzer darf Wert nicht ändern, ist nur zur Benutzerinfo
	m_ctrlSearchArea.EnableWindow(FALSE);

	if(bShow)
	{	
		m_ctrlSearchArea.SetCheck(1);
		m_ctrlAll.EnableWindow(0);
		ShowHideServerArchives(TRUE);
	}
	else
	{
		m_ctrlSearchArea.SetCheck(0);
		m_ctrlAll.EnableWindow(1);
		ShowHideServerArchives(FALSE);
	}

	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::ShowHideServerArchives(BOOL bShow)
{
	if(bShow) //hier rein, wenn Rechteck auf Fenster aufgezogen ist
	{
		CWndSmall* pSW = NULL;
		CViewIdipClient *pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
		ASSERT(pViewIdipClient != NULL);

		pViewIdipClient->LockSmallWindows(_T(__FUNCTION__));
		int iSize = pViewIdipClient->GetSmallWindows()->GetSize();
		
		if(iSize == 1)
		{
			pSW = pViewIdipClient->GetSmallWindows()->GetAtFast(0);
		}
		else if(iSize > 1)
		{
			CWndPlay* pDW = NULL;
			//changes for VisualStudio 2005
			int i = 0;
			for(i=0; i < iSize; i++)
			{
				pSW = pViewIdipClient->GetSmallWindows()->GetAtFast(i);
				if(pSW && pSW->IsWndPlay())
				{
					pDW = (CWndPlay*)pSW;
					if(pDW && !pDW->GetRectToQuery().IsRectEmpty())
					{
						if(pDW->IsCmdActive())
						{
							break;
						}
					}
				}
			}
		}

		if(pSW && pSW->IsWndPlay())
		{
			CWndPlay* pDW = (CWndPlay*)pSW;
			if(pDW && pDW->GetServer())
			{
				WORD wArchivNr = pDW->GetArchivNr();
				CString sArchivName = pSW->GetServer()->GetArchiveName(wArchivNr);
				CString sServerName = pSW->GetServerName();
				CString sName = pSW->GetServer()->GetName();

				m_ServerArchives.DeleteAllItems();

				TVINSERTSTRUCT insert;
				insert.hParent = TVI_ROOT;
				insert.hInsertAfter = TVI_LAST;
				insert.item.mask = TVIF_TEXT | TVIF_PARAM;
			
				HTREEITEM hServer;
				insert.item.pszText = sServerName.GetBuffer(0);
				insert.item.lParam = pSW->GetServer()->GetID();
				hServer = m_ServerArchives.InsertItem(&insert);
				sServerName.ReleaseBuffer();
				SetCheck(m_ServerArchives,hServer,TRUE,TRUE);
				insert.hParent = hServer;
	
				HTREEITEM hArchiv;
				insert.item.pszText = sArchivName.GetBuffer(0);
				insert.item.lParam = wArchivNr;
				hArchiv = m_ServerArchives.InsertItem(&insert);
				sArchivName.ReleaseBuffer();
				SetCheck(m_ServerArchives,hArchiv,TRUE,TRUE);

				m_iServersChecked++;
				TRACE(_T("TKR ----------- m_iServersChecked++  ShowHideServerArchives()\n"));

				m_ServerArchives.Expand(hServer, TVE_EXPAND);
			}
		}
		pViewIdipClient->UnlockSmallWindows();
	}
	else
	{	
		m_iServersChecked = 0;
		InitServerArchives();
	}

}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnCheckDate() 
{
	UpdateData();
	if(!m_bExactDate && m_bExactTime)
	{
		m_bExactTime = FALSE;
		m_ctrlCheckTime.SetCheck(0);
	}
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnCheckExactTime() 
{
	UpdateData();
	if(m_bExactTime && !m_bExactDate)
	{
		m_bExactDate = TRUE;
		m_ctrlCheckDate.SetCheck(1);
	}
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnCheckTimeFrom() 
{
	UpdateData();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::SetPageSize(int iSize)
{
	SCROLLINFO ScrollInfo;

	m_ScrollBar.GetScrollInfo(&ScrollInfo);
	ScrollInfo.nPage = iSize;
	m_ScrollBar.SetScrollInfo(&ScrollInfo, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
int CDlgQuery::GetPageSize()
{
	SCROLLINFO ScrollInfo = {0};

	m_ScrollBar.GetScrollInfo(&ScrollInfo);

	return ScrollInfo.nPage;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int oldScroll = m_iScroll;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		m_iScroll++;
		if (m_iScroll>=(m_QueryFieldDialogs.GetSize() - GetPageSize())) 
		{
			m_iScroll = m_QueryFieldDialogs.GetSize() - GetPageSize();
		}
		break;
	case SB_LINEUP:
		m_iScroll--;
		if (m_iScroll < 0) 
		{
			m_iScroll = 0;
		}
		break;
	case SB_PAGEDOWN:
		m_iScroll += GetPageSize();
		if (m_iScroll >= (m_QueryFieldDialogs.GetSize() - GetPageSize())) 
		{
			m_iScroll = m_QueryFieldDialogs.GetSize() - GetPageSize();
		}
		break;
	case SB_PAGEUP:
		m_iScroll -= GetPageSize();
		if (m_iScroll < 0) 
		{
			m_iScroll = 0;
		}
		break;
	case SB_TOP:
		m_iScroll = 0;
		break;
	case SB_BOTTOM:
		m_iScroll = m_QueryFieldDialogs.GetSize() - GetPageSize();
		break;
	case SB_THUMBTRACK:
		m_iScroll = nPos;
		if (m_iScroll>=(m_QueryFieldDialogs.GetSize() - GetPageSize())) 
		{
			m_iScroll = m_QueryFieldDialogs.GetSize()- GetPageSize();
		}
		break;

	}
	if (oldScroll != m_iScroll)
	{
		UpdateData(FALSE);
		Resize();
	}
	
	CSkinDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQuery::OnKillFocus(CWnd* pNewWnd)
{
	CSkinDialog::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
}
