// DlgDatabaseInfo.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgDatabaseInfo.h"

#include "MainFrm.h"

#include "Server.h"
#include "CIPCDatabaseIdipClient.h"
#include "images.h"

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
// Defines für die Positionen der Columns innerhalb des Listviews
#define	COL_POS_ARCHIV_NAME		(0)
#define	COL_POS_ARCHIV_NR		(1)
#define	COL_POS_ARCHIV_PERCENT	(2)
#define	COL_POS_ARCHIV_SIZE_GB	(3)
#define	COL_POS_ARCHIV_USED_GB	(4)
#define	COL_POS_ARCHIV_SIZE_MB	(5)
#define	COL_POS_ARCHIV_USED_MB	(6)

/////////////////////////////////////////////////////////////////////////////
// Defines für die Breite der Columns des Listviews
#define	COL_WIDTH_ARCHIV_NAME		(150)
#define	COL_WIDTH_ARCHIV_NR			(40)
#define	COL_WIDTH_ARCHIV_FUELL		(50)
#define	COL_WIDTH_ARCHIV_SIZE_MB	(80)
#define	COL_WIDTH_ARCHIV_USED_MB	(80)
#define	COL_WIDTH_ARCHIV_SIZE_GB	(80)
#define	COL_WIDTH_ARCHIV_USED_GB	(100)

/////////////////////////////////////////////////////////////////////////////
// CDlgDatabaseInfo dialog
CDlgDatabaseInfo::CDlgDatabaseInfo(CWnd* pParent, CServer* pServer)
	: CSkinDialog(CDlgDatabaseInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDatabaseInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInfosAvailable = TRUE;
	m_pServer = pServer;

	m_nInitToolTips = FALSE;
	MoveOnClientAreaClick(true);

	Create(IDD, pParent);
	ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
CDlgDatabaseInfo::~CDlgDatabaseInfo()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDlgDatabaseInfo::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDatabaseInfo)
	DDX_Control(pDX, IDC_ARCHIV_INFO_LISTE, m_ArchivList);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgDatabaseInfo, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgDatabaseInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CDlgDatabaseInfo::OnCancel()
{
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgDatabaseInfo::OnOK()
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgDatabaseInfo::PostNcDestroy()
{
	m_pServer->m_pDlgDatabaseInfo = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgDatabaseInfo::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	
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
CString MB2GB(DWORD dwMB)
{
	CString s;
	if (dwMB<1024)
	{
		s.Format(_T("%1.3f"), (double)dwMB/(double)1024);
	}
	else
	{
		s.Format(_T("%.1f"), (double)dwMB/(double)1024);
	}
	return s;

}
/////////////////////////////////////////////////////////////////////////////
void CDlgDatabaseInfo::FillArchivList()
{
	m_ArchivList.DeleteAllItems();

	if (m_pServer && m_pServer->IsConnectedPlay())
	{
		CIPCDatabaseIdipClient* pDatabase = m_pServer->GetDatabase();

		CIPCArchivRecordArray& ar = pDatabase->GetRecords();
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
					nImage = _IMAGE_ARCH_RING;
				}
				else if (byTyp == CAR_IS_SAFE_RING)
				{
					nImage = _IMAGE_ARCH_PRE_ALARM;
				}
				else if (byTyp == CAR_IS_ALARM)
				{
					nImage = _IMAGE_ARCH_ALARM;
				}
				else if (byTyp == CAR_IS_BACKUP)
				{
					nImage = _IMAGE_ARCH_BACKUP;
				}
				else if (byTyp == CAR_IS_ALARM_LIST)
				{
					nImage = _IMAGE_ARCH_ALARMLIST;
				}
				else if (byTyp & CAR_IS_SEARCH) 
				{
					// type is CAR_IS_RING|CAR_IS_SEARCH
					// only test for search
					nImage = _IMAGE_ARCH_SEARCH;
				}
				else
				{
					WK_TRACE_ERROR(_T("unbekannter Archivtyp %s\n"),pArchiv->GetName());
				}

				// Archiv Name ausgeben
				index = m_ArchivList.InsertItem(i, pArchiv->GetName(), nImage);
				if (index != -1)
				{
					BOOL bIsBackupArchive = pArchiv->GetFlags() == CAR_IS_BACKUP;

					// Archiv Nummer ausgeben
					sText.Format(_T("%d"), (int)pArchiv->GetSubID());
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_NR, sText);

					// Archiv Größe in MB ausgeben
					DWORD dwGesamt = pArchiv->SizeInMB();
					sText.Format(_T("%d"), dwGesamt);

					if(bIsBackupArchive && dwGesamt == 0)
					{
						//Backup Archive können kleiner als 1MB sein
						//Backup Archive sind immer so groß wie der benutzte Platz
						__int64 i64BackupArchivSizeInBytes = pArchiv->BenutzterPlatz();
						DWORD dBPkleiner1MBInKB = (DWORD)(i64BackupArchivSizeInBytes/1024);
						sText = MB2GB(dBPkleiner1MBInKB);
					}
					dwSumMB += dwGesamt;
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_SIZE_MB, sText);

					// Benutzten Platz ausgeben
					__int64 i64BP = pArchiv->BenutzterPlatz();
					__int64 i64BPkleiner1MBInBytes = 0; 
					DWORD dwVoll = (DWORD)(i64BP/1024/1024);

					sText.Format(_T("%d"), dwVoll);
					if(dwVoll == 0 && i64BP > 0)
					{
						//benutzter Platz < 1MB; wird für Füllstandsangabe benötigt
						i64BPkleiner1MBInBytes = i64BP;
						DWORD dBPkleiner1MBInKB = (DWORD)(i64BPkleiner1MBInBytes/1024);
						sText = MB2GB(dBPkleiner1MBInKB);
					}

					dwSumMBUsed += dwVoll;
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_USED_MB, sText);

					// Größe in GB ausgeben
					sText = MB2GB(dwGesamt);
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_SIZE_GB, sText);
					
					// benutzter Platz in GB ausgeben
					//sText = int64ToString(pArchiv->BenutzterPlatz(),TRUE);
					iSumByte += pArchiv->BenutzterPlatz();
					sText = MB2GB(dwVoll);
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_USED_GB, sText);

					// Füllstand ausgeben
					double nProzent=0;
					if (dwGesamt>0)
					{
						nProzent = ((double)dwVoll*100.0)/(double)dwGesamt;
						if(i64BPkleiner1MBInBytes > 0)
						{					
							__int64 i64GesamtInBytes = dwGesamt * 1024 * 1024;
							nProzent = ((double)i64BPkleiner1MBInBytes*100.0)/(double)i64GesamtInBytes;
						}
					}

					sText.Format(_T("%2.1f %%"), nProzent);
					if (bIsBackupArchive)
					{
						//Füllstand ist zwar 100%, soll aber leer bleiben
						sText.Empty();
					}
					m_ArchivList.SetItemText(index, COL_POS_ARCHIV_PERCENT, sText);
				}
			}
		} // for


		sText.LoadString(IDS_SUM);
		index = m_ArchivList.InsertItem(i, sText, _IMAGE_ARCH_SUM);
		
		sText.Format(_T("%d"),dwSumMBUsed);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_USED_MB, sText);
		
		sText.Format(_T("%d"),dwSumMB);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_SIZE_MB, sText);
		

		if (dwSumMB>0)
		{
			sText.Format(_T("%2.1f %%"), ((((double)dwSumMBUsed*100.0)/(double)dwSumMB)));
		}
		else
		{
			sText = _T("0 %");
		}
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_PERCENT, sText);

		sText = MB2GB(dwSumMB);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_SIZE_GB, sText);

		sText = MB2GB(dwSumMBUsed);
		m_ArchivList.SetItemText(index, COL_POS_ARCHIV_USED_GB, sText);
		m_bInfosAvailable = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgDatabaseInfo::InitializeListView()
{
	CString sNr;
	CString sFuellgrad;
	CString sArchivName;
	CString sTyp;
	CString sSizeInMB;
	CString sUsedInMB;
	CString sSizeInGB;
	CString sUsedInGB;

	sNr.LoadString(IDS_DBPROP_NR);
	sFuellgrad.LoadString(IDS_DBPROP_FUELL);
	sArchivName.LoadString(IDS_DBPROP_NAME);
	sTyp.LoadString(IDS_DBPROP_TYP);
	sSizeInMB.LoadString(IDS_DBPROP_MB);
	sUsedInMB.LoadString(IDS_DBPROP_BENUTZT);
	sSizeInGB.LoadString(IDS_DBPROP_SIZE_GB);
	sUsedInGB.LoadString(IDS_DBPROP_USED_GB);
	
	LV_COLUMN Colum;
	Colum.mask	= LVCF_FMT;
	Colum.fmt	= LVCFMT_RIGHT;

	m_Images.Create(IDB_IMAGES,_IMAGE_WIDTH,0,RGB(0,255,255));
	m_ArchivList.SetImageList(&m_Images,LVSIL_SMALL);

	m_ArchivList.InsertColumn(COL_POS_ARCHIV_NAME, sArchivName,LVCFMT_LEFT, COL_WIDTH_ARCHIV_NAME);
	m_ArchivList.InsertColumn(COL_POS_ARCHIV_NR, sNr,LVCFMT_LEFT, COL_WIDTH_ARCHIV_NR);

	m_ArchivList.InsertColumn( COL_POS_ARCHIV_PERCENT, sFuellgrad,LVCFMT_LEFT, COL_WIDTH_ARCHIV_FUELL);
	m_ArchivList.SetColumn( COL_POS_ARCHIV_PERCENT, &Colum );

	m_ArchivList.InsertColumn( COL_POS_ARCHIV_SIZE_GB, sSizeInGB,LVCFMT_LEFT, COL_WIDTH_ARCHIV_SIZE_GB);
	m_ArchivList.SetColumn( COL_POS_ARCHIV_SIZE_GB, &Colum );
	m_ArchivList.InsertColumn( COL_POS_ARCHIV_USED_GB,sUsedInGB,LVCFMT_LEFT, COL_WIDTH_ARCHIV_USED_GB);
	m_ArchivList.SetColumn( COL_POS_ARCHIV_USED_GB, &Colum );

	m_ArchivList.InsertColumn( COL_POS_ARCHIV_SIZE_MB, sSizeInMB,LVCFMT_LEFT, COL_WIDTH_ARCHIV_SIZE_MB);
	m_ArchivList.SetColumn( COL_POS_ARCHIV_SIZE_MB, &Colum );
	m_ArchivList.InsertColumn( COL_POS_ARCHIV_USED_MB, sUsedInMB,LVCFMT_LEFT, COL_WIDTH_ARCHIV_USED_MB);
	m_ArchivList.SetColumn( COL_POS_ARCHIV_USED_MB, &Colum );		

}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgDatabaseInfo::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==ID_ARCHIV_INFO)
	{
		FillArchivList();
		return TRUE;
	}
	return CSkinDialog::OnCommand(wParam, lParam);
}
