/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: searchdlg.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/searchdlg.cpp $
// CHECKIN:		$Date: 11.01.99 15:51 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 11.01.99 14:48 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#include "stdafx.h"
#include "Recherche.h"

#include "SearchDlg.h"

#include "MainFrm.h"
#include "RechercheDoc.h"
#include "ObjectView.h"

#include "Server.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog


CSearchDlg::CSearchDlg(CServer* pServer, CObjectView* pParent)
	: CDialog(IDD_SEARCH, pParent)
{
	m_pServer =	pServer;
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CSearchDlg)
	m_bArchiv = FALSE;
	m_sBetragText = "";
	m_sDatumText = "";
	m_sBlzText = "";
	m_sGaNrText = "";
	m_sKtoNrText = "";
	m_sTaNrText = "";
	m_sZeitText = "";
	m_sRecTime = _T("");
	m_sRecTimeFrom = _T("");
	m_sRecTimeTo = _T("");
	m_sDate = _T("");
	//}}AFX_DATA_INIT

	CString sText;

	sText = theApp.GetMappedString(DFN_KTO_NR);
	if (!sText.IsEmpty())
	{
		m_sKtoNrText = "&" + sText + ":";
	}
	
	sText = theApp.GetMappedString(DFN_VALUE);
	if (!sText.IsEmpty())
	{
		m_sBetragText = "&" + sText + ":";
	}
	
	sText = theApp.GetMappedString(DFN_REC_DATE);
	if (!sText.IsEmpty())
	{
		m_sDatumText = "&" + sText + ":";
	}
	
	sText = theApp.GetMappedString(DFN_BLZ);
	if (!sText.IsEmpty())
	{
		m_sBlzText = "&" + sText + ":";
	}
	
	sText = theApp.GetMappedString(DFN_GA_NR);
	if (!sText.IsEmpty())
	{
		m_sGaNrText = "&" + sText + ":";
	}

	sText = theApp.GetMappedString(DFN_TA_NR);
	if (!sText.IsEmpty())
	{
		m_sTaNrText = "&" + sText + ":";
	}

	sText = theApp.GetMappedString(DFN_REC_TIME);
	if (!sText.IsEmpty())
	{
		m_sZeitText = "&" + sText + ":";
	}

	sText = theApp.GetMappedString(DFN_KAMERA_NR);
	if (!sText.IsEmpty())
	{
		m_sKameraNrText = "&" + sText + ":";
	}

	// m_sQuery;

	m_DbGesamt	=	1;
	m_OpenFiles	=	0;
	m_Archiv	=	0;
	memset (m_szFoundFile, 0, sizeof(m_szFoundFile));

	Create(IDD_SEARCH,pParent);
}


void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchDlg)
	DDX_Control(pDX, IDE_SEARCH_TANR, m_EditTANummer);
	DDX_Control(pDX, IDE_SEARCH_GANR, m_EditGANummer);
	DDX_Control(pDX, IDE_SEARCH_BLZ, m_EditBlz);
	DDX_Control(pDX, IDE_SEARCH_BETRAG, m_EditBetrag);
	DDX_Control(pDX, IDE_SEARCH_KTONR, m_EditKtoNr);
	DDX_Check(pDX, IDC_ARCHIV, m_bArchiv);
	DDX_Control(pDX, IDC_ARCHIV, m_btnArchiv);
	DDX_Control(pDX, IDE_SEARCH_ZEIT_VON, m_EditVonZeit);
	DDX_Control(pDX, IDE_SEARCH_ZEIT_BIS, m_EditBisZeit);
	DDX_Control(pDX, IDE_SEARCH_ZEIT, m_EditZeit);
	DDX_Control(pDX, IDE_SEARCH_DATUM, m_EditDatum);
	DDX_Control(pDX, IDC_ARCHIV_LISTE, m_lbArchivList);
	DDX_Text(pDX, IDC_BETRAG_TEXT, m_sBetragText);
	DDX_Text(pDX, IDC_DATUM_TEXT, m_sDatumText);
	DDX_Text(pDX, IDC_BLZ_TEXT, m_sBlzText);
	DDX_Text(pDX, IDC_GANR_TEXT, m_sGaNrText);
	DDX_Text(pDX, IDC_KTONR_TEXT, m_sKtoNrText);
	DDX_Text(pDX, IDC_TANR_TEXT, m_sTaNrText);
	DDX_Text(pDX, IDC_ZEIT_TEXT, m_sZeitText);
	DDX_Text(pDX, IDC_KAMERA_TEXT, m_sKameraNrText);
	DDX_Text(pDX, IDE_SEARCH_ZEIT, m_sRecTime);
	DDX_Text(pDX, IDE_SEARCH_ZEIT_VON, m_sRecTimeFrom);
	DDX_Text(pDX, IDE_SEARCH_ZEIT_BIS, m_sRecTimeTo);
	DDX_Text(pDX, IDE_SEARCH_DATUM, m_sDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchDlg)
	ON_BN_CLICKED(IDC_ARCHIV, OnArchiv)
	ON_BN_CLICKED(IDC_OPEN_FILES, OnOpenFiles)
	ON_BN_CLICKED(IDC_DB_GESAMT, OnDbGesamt)
	ON_EN_CHANGE(IDE_SEARCH_ZEIT, OnChangeSearchZeit)
	ON_EN_CHANGE(IDE_SEARCH_ZEIT_BIS, OnChangeSearchZeitBis)
	ON_EN_CHANGE(IDE_SEARCH_ZEIT_VON, OnChangeSearchZeitVon)
	ON_COMMAND(IDOK, OnOK)
	ON_EN_CHANGE(IDE_SEARCH_DATUM, OnChangeSearchDatum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg message handlers

BOOL CSearchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
#ifdef GARNY
    m_bitlogo.AutoLoad (IDC_LOGO, this);
#endif
    
	CheckRadioButton(IDC_DB_GESAMT, IDC_ARCHIV, IDC_DB_GESAMT);

	InitListBox();

	if (m_sKtoNrText.IsEmpty())
	{
		m_EditKtoNr.ShowWindow(SW_HIDE);
	}
	if (m_sBetragText.IsEmpty())
	{
		m_EditBetrag.ShowWindow(SW_HIDE);
	}
	if (m_sBlzText.IsEmpty())
	{
		m_EditBlz.ShowWindow(SW_HIDE);
	}
	if (m_sGaNrText.IsEmpty())
	{
		m_EditGANummer.ShowWindow(SW_HIDE);
	}
	if (m_sTaNrText.IsEmpty())
	{
		m_EditTANummer.ShowWindow(SW_HIDE);
	}
	
	return TRUE;
}

void CSearchDlg::OnCancel() 
{
	DestroyWindow();
}

void CSearchDlg::PostNcDestroy() 
{
//	m_pParent->m_pSearchDlg = NULL;
	delete this;
}

void CSearchDlg::OnDbGesamt() 
{
	m_DbGesamt	=	1;
	m_OpenFiles	=	0;

	UpdateData();
	m_lbArchivList.ShowWindow(m_bArchiv ? SW_SHOW : SW_HIDE);
	m_btnArchiv.EnableWindow();
}

void CSearchDlg::OnOpenFiles() 
{
	m_DbGesamt	=	0;
	m_OpenFiles	=	1;

	m_lbArchivList.ShowWindow(SW_HIDE);
	m_btnArchiv.EnableWindow(FALSE);
}

void CSearchDlg::OnArchiv() 
{
	m_lbArchivList.ShowWindow(!m_lbArchivList.IsWindowVisible());
	m_Archiv	=	1;
}

void CSearchDlg::OnChangeSearchZeit() 
{
	UpdateData();
	int iSel = SetTimeEdit(m_sRecTime);
	UpdateData(FALSE);
	m_EditZeit.SetSel(iSel,iSel);
	if (m_sRecTime == szEmptyTime || m_EditZeit.LineLength()==0)
	{
		m_EditVonZeit.ShowWindow(SW_SHOW);
		m_EditBisZeit.ShowWindow(SW_SHOW);
	}
	else
	{
		m_EditVonZeit.ShowWindow(SW_HIDE);
		m_EditBisZeit.ShowWindow(SW_HIDE);
	}
}

void CSearchDlg::OnChangeSearchZeitBis() 
{
	UpdateData();
	int iSel = SetTimeEdit(m_sRecTimeTo);
	UpdateData(FALSE);
	m_EditBisZeit.SetSel(iSel,iSel);

	if ((m_sRecTimeFrom == szEmptyTime || m_EditVonZeit.LineLength()==0) &&
		(m_sRecTimeTo == szEmptyTime || m_EditBisZeit.LineLength()==0))
	{
		m_EditZeit.ShowWindow(SW_SHOW);
	}
	else
	{
		m_EditZeit.ShowWindow(SW_HIDE);
	}
}

void CSearchDlg::OnChangeSearchZeitVon() 
{
	UpdateData();
	int iSel = SetTimeEdit(m_sRecTimeFrom);
	UpdateData(FALSE);
	m_EditVonZeit.SetSel(iSel,iSel);

	if ((m_sRecTimeFrom == szEmptyTime || m_EditVonZeit.LineLength()==0) &&
		(m_sRecTimeTo == szEmptyTime || m_EditBisZeit.LineLength()==0))
	{
		m_EditZeit.ShowWindow(SW_SHOW);
	}else{
		m_EditZeit.ShowWindow(SW_HIDE);
	}
}

BOOL CSearchDlg::Validate(CWnd* pWnd, FIELD_TYPE ft, CString& sValue, int iLeastLen/*=0*/)
{
	CString sData;
	CString sRet;

	pWnd->GetWindowText(sData);

	if (sData.IsEmpty())
	{
		sValue.Empty();
		return TRUE;
	}

	switch(ft)
	{
	case FT_DATE:
		sRet = GetCorrectDate(sData,pWnd);
		break;
	case FT_TIME:
		sRet = GetCorrectTime(sData);
		break;
	case FT_NUMBER:
		sRet = GetCorrectNr(sData,iLeastLen);
		break;
	}
	if (sRet.IsEmpty())
	{
		sValue.Empty();
		return FALSE;
	}
	else
	{
		sValue = sRet;
		return TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString	CSearchDlg::BuildFieldQueryC(const CString& sValue, const CString& sField)
{
	CString s;
	if (!sValue.IsEmpty())
	{
		s.Format("(%s = '%s')",sField,sValue);
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString	CSearchDlg::BuildFieldQueryN(const CString& sValue, const CString& sField)
{
	CString s;
	if (!sValue.IsEmpty())
	{
		s.Format("(%s = %s)",sField,sValue);
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
void CSearchDlg::OnOK()
{
	UpdateData();

	m_sQuery.Empty();

	CString sValue;
	sValue = SearchForArchivNr();
	
	AddToQueryString(sValue);

	if (Validate(GetDlgItem(IDE_SEARCH_DATUM), FT_DATE, sValue))
	{
		AddToQueryString(BuildFieldQueryC(sValue,DFN_REC_DATE));
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_ZEIT), FT_TIME, sValue))
	{
		AddToQueryString(BuildFieldQueryC(sValue,DFN_REC_TIME));
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_ZEIT_VON), FT_TIME, sValue))
	{
		if (!sValue.IsEmpty())
		{
			CString s;
			s.Format("(%s >= '%s')",DFN_REC_TIME,sValue);
			AddToQueryString(s);
		}
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_ZEIT_BIS), FT_TIME, sValue))
	{
		if (!sValue.IsEmpty())
		{
			CString s;
			s.Format("(%s <= '%s')",DFN_REC_TIME,sValue);
			AddToQueryString(s);
		}
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_GANR), FT_NUMBER, sValue, 6))
	{
		AddToQueryString(BuildFieldQueryC(sValue,DFN_GA_NR));
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_TANR), FT_NUMBER, sValue, 4))
	{
		AddToQueryString(BuildFieldQueryN(sValue,DFN_TA_NR));
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_BLZ), FT_NUMBER, sValue, 8))
	{
		AddToQueryString(BuildFieldQueryN(sValue,DFN_BLZ));
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_BETRAG), FT_NUMBER, sValue, 4))
	{
		AddToQueryString(BuildFieldQueryN(sValue,DFN_VALUE));
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_KTONR), FT_NUMBER, sValue, 10))
	{
		AddToQueryString(BuildFieldQueryC(sValue,DFN_KTO_NR));
	} else { return; }

	if (Validate(GetDlgItem(IDE_SEARCH_KAMERA), FT_NUMBER, sValue, 2))
	{
		if (!sValue.IsEmpty())
		{
			DWORD i = atoi(sValue);
			if (i>0)
			{
				sValue.Format("%02d",i-1);
				AddToQueryString(BuildFieldQueryC(sValue,DFN_KAMERA_NR));
			} else { return; }
		}
	} else { return; }
	
	if (m_sQuery.IsEmpty()){
		m_sQuery = " ";
		// To prevent extramemory Fault
	}
	if (m_OpenFiles){
		m_sQuery += "@";
	}

	StartSearch();

//	m_pParent->ShowSearchResultDialog(m_pServer);

	DestroyWindow();
}

CString CSearchDlg::SearchForArchivNr()
{
	CString s;
	CString sArchivNrQuery;

	if (m_lbArchivList.IsWindowVisible())
	{
		int i,c;
		
		c = m_lbArchivList.GetCount();

		for (i=0; i < c; i++)
		{
			if (m_lbArchivList.GetCheck(i))
			{
				if (!sArchivNrQuery.IsEmpty())
				{
					sArchivNrQuery += " .OR. ";
				}
				else
				{
					sArchivNrQuery = "(";
				}
				s.Format("(ARC_NR = %d)",m_lbArchivList.GetItemData(i));
				sArchivNrQuery += s;
			}
		}
	}
	
	if (!sArchivNrQuery.IsEmpty())
	{
		sArchivNrQuery += ")";
	}
	
	return sArchivNrQuery;
}
////////////////////////////////////////////////////////////////////////
void CSearchDlg::AddToQueryString(const CString& sField)
{
	if (!sField.IsEmpty()){
		if(!m_sQuery.IsEmpty()){
			m_sQuery += " .AND. ";
		}
		m_sQuery += sField;
	}
}
///////////////////////////////////////////////////////////////////////////
void CSearchDlg::StartSearch()
{
	if (m_pServer && m_pServer->IsConnected())
	{
		CIPCDatabaseRecherche* pDataBase = m_pServer->GetDatabase();
		pDataBase->DoRequestQuery(GetTickCount(), m_sQuery);
		WK_TRACE("searching %s for %s\n",m_pServer->GetName(),m_sQuery);
	}
}
///////////////////////////////////////////////////////////////////////////
CString CSearchDlg::NoSpace(const CString& s)
{
	CString tmp;
	int l = s.GetLength();
	for (int i=0; i<l; i++)
	{
		if (s[i] != ' '){
			tmp += s[i];
		}
	}
	return tmp;
}
///////////////////////////////////////////////////////////////////////////
CString CSearchDlg::OnlyDigits(const CString& s)
{
	CString tmp;
	int l = s.GetLength();
	for (int i=0; i<l; i++)
	{
		if (isdigit(s[i])){
			tmp += s[i];
		}
	}
	return tmp;
}
///////////////////////////////////////////////////////////////////////////////////
CString CSearchDlg::GetCorrectNr(const CString& s, int iLeast)
{
	CString sNullen('0',(iLeast - s.GetLength()));
	return sNullen + s;
}

/////////////////////////////////////////////////////////////////////////////
void CSearchDlg::OnChangeSearchDatum() 
{
	UpdateData();
	int iSel = SetDateEdit(m_sDate);
	UpdateData(FALSE);
	m_EditDatum.SetSel(iSel,iSel);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSearchDlg::InitListBox() 
{
	m_lbArchivList.ResetContent();
	m_lbArchivList.SetCheckStyle(BS_AUTOCHECKBOX);

	InitNormalListBox();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSearchDlg::InitNormalListBox()
{
	int index = 0;
	CString	sName;

	if (m_pServer && m_pServer->IsConnected())
	{
		CIPCDatabaseRecherche* pDataBase = m_pServer->GetDatabase();
	
		CString sTemp;
		
		GetWindowText(sTemp);
		sTemp += " " + m_pServer->GetName();
		SetWindowText(sTemp);

		const CIPCArchivRecordArray& ar = pDataBase->GetRecords();

		for (int i=0; i<ar.GetSize(); i++)
		{
			index = m_lbArchivList.AddString(ar[i]->GetName());
			if (index != LB_ERR)
			{
				m_lbArchivList.SetItemData(index, ar[i]->GetSubID());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSearchDlg::InitBackupListBox()
{
	int index = 0;
	CString	sName;

	// TODO:
	/*	
	if (theApp.IsDataBase(m_pDataBase) && m_pDataBase->GotRecords())
	{
		CString sTemp;
		sTemp.LoadString(IDS_TITLE_SEARCH);
		sTemp += m_pDataBase->m_ServerName;
		sTemp += " / ";
		sTemp += m_pMerker->GetBackupName();
		SetWindowText(sTemp);
		const CBackupArchivRecordSafeArray& ArcArray = m_pDataBase->GetBackupRecords();

		for (int i=0; i<ArcArray.GetSize(); i++)
		{
			// BackupNr und Drive müssen stimmen
			if (m_pMerker->GetBackupNr() == ArcArray[i]->GetBackupNr()){
				if (m_pMerker->GetDrive() == ArcArray[i]->GetDrive()){
					sName.Format("%d  %s", (int)ArcArray[i]->GetSubID(), ArcArray[i]->GetArchivName());
					index = m_lbArchivList.AddString(sName);
					if (index != LB_ERR){
						m_lbArchivList.SetItemData(index, ArcArray[i]->GetSubID());
					}
				}
			}
		}
	}
	*/
}
/////////////////////////////////////////////////////////////////////////////
void CSearchDlg::BuildBackupQueryString()
{
	CString s = m_sQuery;
	m_sQuery = "$";
	m_sQuery += ""; //TODO: m_pMerker->GetBackupName();
	m_sQuery += "$";
	m_sQuery += s;
}
