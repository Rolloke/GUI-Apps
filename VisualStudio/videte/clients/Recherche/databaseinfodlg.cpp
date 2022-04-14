// DataBaseInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Recherche.h"
#include "Server.h"
#include "CIPCDatabaseRecherche.h"
#include "DataBaseInfoDlg.h"
#include "MainFrm.h"
#include "images.h"


CString int64ToString(__int64 i, BOOL bPoints=FALSE)
{
	BOOL bMinus = FALSE;
	__int64 t = i;
	CString r;
	int c;

	if (t<0)
	{
		t = -t;
		bMinus = TRUE;
	}
	if (t==0)
	{
		r = _T("0");
	}
	else
	{
		while(t>0)
		{
			c = (int)(t%10);
			t = t/10;
			r = (char)(c+_T('0')) + r;
		}
	}

	if (bMinus)
	{
		r = _T('-') + r;
	}

	if (bPoints)
	{
		CString s3;
		CString s;

		while (r.GetLength()>3)
		{
			s3 = r.Mid(r.GetLength()-3);
			s = _T('.') + s3 + s;
			r = r.Left(r.GetLength()-3);
		}
		r = r+s;
	}

	return r;
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Defines für die Positionen der Columns innerhalb des Listviews

#define	COL_POS_ARCHIV_NAME		(0)
#define	COL_POS_ARCHIV_NR		(1)
#define	COL_POS_ARCHIV_FUELL	(2)
#define	COL_POS_ARCHIV_MB		(3)
#define	COL_POS_ARCHIV_BENUTZT	(4)
#define	COL_POS_ARCHIV_FREI		(5)
#define	COL_POS_ARCHIV_AKTUELL	(6)

// Defines für die Breite der Columns des Listviews

#define	COL_WIDTH_ARCHIV_NAME		(150)
#define	COL_WIDTH_ARCHIV_NR			(40)
#define	COL_WIDTH_ARCHIV_FUELL		(50)
#define	COL_WIDTH_ARCHIV_MB			(80)
#define	COL_WIDTH_ARCHIV_BENUTZT	(80)
#define	COL_WIDTH_ARCHIV_FREI		(80)
#define	COL_WIDTH_ARCHIV_AKTUELL	(100)

/////////////////////////////////////////////////////////////////////////////
// CDataBaseInfoDlg dialog


CDataBaseInfoDlg::CDataBaseInfoDlg(CWnd* pParent, CServer* pServer)
	: CWK_Dialog(CDataBaseInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataBaseInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInfosAvailable = TRUE;
	m_pServer = pServer;
	Create(IDD, pParent);
	ShowWindow(SW_SHOW);
}

CDataBaseInfoDlg::~CDataBaseInfoDlg()
{
}

void CDataBaseInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataBaseInfoDlg)
	DDX_Control(pDX, IDC_ARCHIV_INFO_LISTE, m_ArchivList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataBaseInfoDlg, CWK_Dialog)
	//{{AFX_MSG_MAP(CDataBaseInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataBaseInfoDlg message handlers

void CDataBaseInfoDlg::OnCancel()
{
	OnOK();
}

void CDataBaseInfoDlg::OnOK()
{
	DestroyWindow();
}

void CDataBaseInfoDlg::PostNcDestroy()
{
	m_pServer->m_pDataBaseInfoDlg = NULL;
	delete this;
}

BOOL CDataBaseInfoDlg::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();
	
	InitializeListView();
	LONG dw = ListView_GetExtendedListViewStyle(m_ArchivList.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_ArchivList.m_hWnd,dw);

	CString sTitle;

	GetWindowText(sTitle);
	sTitle += _T(": ") + m_pServer->GetName();
	SetWindowText(sTitle);

	FillArchivList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataBaseInfoDlg::FillArchivList()
{
	m_ArchivList.DeleteAllItems();

	if (m_pServer && m_pServer->IsConnected())
	{
		CIPCDatabaseRecherche* pDatabase = m_pServer->GetDatabase();

		pDatabase->Lock();

		const CIPCArchivRecordArray& ar = pDatabase->GetRecords();
		CIPCArchivRecord *pArchiv = NULL;
		
		CString sText;
		int i,c;
		int index = -1;
		__int64 iSumByte = 0;
		DWORD dwSumMBUsed = 0;
		DWORD dwSumMB = 0;

		c = ar.GetSize();
		for (i=0; i<c; i++)
		{
			pArchiv = ar[i];
			if (!pArchiv->GetName().IsEmpty())
			{
				BYTE byTyp = pArchiv->GetFlags();
				int nImage = 1;
				if (byTyp == CAR_IS_RING)
				{
					nImage = _IMAGE_RING;
				}
				else if (byTyp == CAR_IS_SAFE_RING)
				{
					nImage = _IMAGE_VORALARM;
				}
				else if (byTyp == CAR_IS_ALARM)
				{
					nImage = _IMAGE_ALARM;
				}
				else if (byTyp == CAR_IS_BACKUP)
				{
					nImage = _IMAGE_BACKUP;
				}
				else if (byTyp & CAR_IS_SEARCH) 
				{
					// type is CAR_IS_RING|CAR_IS_SEARCH
					// only test for search
					nImage = _IMAGE_SEARCH;
				}
				else
				{
					WK_TRACE_ERROR(_T("unbekannter Archivtyp %s\n"),pArchiv->GetName());
				}

				// Archiv Name ausgeben
				index = m_ArchivList.InsertItem(i, pArchiv->GetName(), nImage);
				if (index != -1)
				{
					// Archiv Nummer ausgeben
					sText.Format(_T("%d"), (int)pArchiv->GetSubID());
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_NR, sText);

					// MB ausgeben
					DWORD dwGesamt = pArchiv->SizeInMB();
					sText.Format(_T("%d"), dwGesamt);
					dwSumMB += dwGesamt;
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_MB, sText);

					// Benutzten Platz ausgeben
					DWORD dwVoll = (DWORD)(pArchiv->BenutzterPlatz()/1024/1024);
					sText.Format(_T("%d"), dwVoll);
					dwSumMBUsed += dwVoll;
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_BENUTZT, sText);

					// Freien Platz ausgeben
					int  nLeer = dwGesamt - dwVoll;
					sText.Format(_T("%d"), nLeer);
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_FREI, sText);
					
					// benutzter Platz in Bytes ausgeben
					sText = int64ToString(pArchiv->BenutzterPlatz(),TRUE);
					iSumByte += pArchiv->BenutzterPlatz();
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_AKTUELL, sText);

					// Füllstand ausgeben
					double nProzent=0;
					if (dwGesamt>0)
					{
						nProzent = ((double)dwVoll*100.0)/(double)dwGesamt;
					}
					sText.Format(_T("%2.1f %%"), nProzent);
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_FUELL, sText);
				}
			}
		} // for

		pDatabase->Unlock();

		sText.LoadString(IDS_SUM);
		index = m_ArchivList.InsertItem(i, sText, _IMAGE_SUM);
		sText.Format(_T("%d"),dwSumMBUsed);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_BENUTZT, sText);
		sText.Format(_T("%d"),dwSumMB);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_MB, sText);
		sText.Format(_T("%d"),dwSumMB-dwSumMBUsed);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_FREI, sText);

		if (dwSumMB>0)
		{
			sText.Format(_T("%2.1f %%"), ((((double)dwSumMBUsed*100.0)/(double)dwSumMB)));
		}
		else
		{
			sText = _T("0 %");
		}
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_FUELL, sText);
		sText = int64ToString(iSumByte,TRUE);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_AKTUELL, sText);
		m_bInfosAvailable = TRUE;
	}
}

void CDataBaseInfoDlg::InitializeListView()
{
	CString sNr;
	CString sFuellgrad;
	CString sArchivName;
	CString sTyp;
	CString sMB;
	CString sBenutzt;
	CString sFrei;
	CString sAktuellBytes;

	sNr.LoadString(IDS_DBPROP_NR);
	sFuellgrad.LoadString(IDS_DBPROP_FUELL);
	sArchivName.LoadString(IDS_DBPROP_NAME);
	sTyp.LoadString(IDS_DBPROP_TYP);
	sMB.LoadString(IDS_DBPROP_MB);
	sBenutzt.LoadString(IDS_DBPROP_BENUTZT);
	sFrei.LoadString(IDS_DBPROP_FREI);
	sAktuellBytes.LoadString(IDS_DBPROP_AKTUELL);
	
	LV_COLUMN Colum;
	Colum.mask	= LVCF_FMT;
	Colum.fmt	= LVCFMT_RIGHT;

	m_Images.Create(IDB_IMAGES,_IMAGE_WIDTH,0,RGB(0,255,255));
	m_ArchivList.SetImageList(&m_Images,LVSIL_SMALL);

	m_ArchivList.InsertColumn	( COL_POS_ARCHIV_NAME, sArchivName,	LVCFMT_LEFT, COL_WIDTH_ARCHIV_NAME);
	m_ArchivList.InsertColumn	( COL_POS_ARCHIV_NR, sNr,			LVCFMT_LEFT, COL_WIDTH_ARCHIV_NR);

	m_ArchivList.InsertColumn	( COL_POS_ARCHIV_FUELL, sFuellgrad,	LVCFMT_LEFT, COL_WIDTH_ARCHIV_FUELL);
	m_ArchivList.SetColumn	( COL_POS_ARCHIV_FUELL, &Colum );

	m_ArchivList.InsertColumn	( COL_POS_ARCHIV_MB, sMB,			LVCFMT_LEFT, COL_WIDTH_ARCHIV_MB);
	m_ArchivList.SetColumn	( COL_POS_ARCHIV_MB, &Colum );

	m_ArchivList.InsertColumn	( COL_POS_ARCHIV_BENUTZT, sBenutzt,	LVCFMT_LEFT, COL_WIDTH_ARCHIV_BENUTZT);
	m_ArchivList.SetColumn	( COL_POS_ARCHIV_BENUTZT, &Colum );		
	m_ArchivList.InsertColumn	( COL_POS_ARCHIV_FREI, sFrei,		LVCFMT_LEFT, COL_WIDTH_ARCHIV_FREI);
	m_ArchivList.SetColumn	( COL_POS_ARCHIV_FREI, &Colum );

	m_ArchivList.InsertColumn	( COL_POS_ARCHIV_AKTUELL,sAktuellBytes,LVCFMT_LEFT, COL_WIDTH_ARCHIV_AKTUELL);
	m_ArchivList.SetColumn	( COL_POS_ARCHIV_AKTUELL, &Colum );
}

BOOL CDataBaseInfoDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==ID_ARCHIV_INFO)
	{
		FillArchivList();
		return TRUE;
	}
	return CWK_Dialog::OnCommand(wParam, lParam);
}
