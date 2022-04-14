/* GlobalReplace: WK_GetProfileString --> GetProfile().GetString */
/* GlobalReplace: WK_GetProfileInt --> GetProfile().GetInt */
// ISDNPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ISDNPage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "CipcStringDefs.h"
#include ".\isdnpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSection1[] = _T("ISDNUnit");
static TCHAR BASED_CODE szSection2[] = _T("ISDNUnit2");

static TCHAR BASED_CODE szProcessorUsage[] = _T("ProcessorUsage"); // int 1(High)..3(Low)
static TCHAR BASED_CODE szEnableKnockKnockBox[] = _T("EnableKnockKnockBox"); // int 1(On)
static TCHAR BASED_CODE szOwnNumber[]    = _T("OwnNumber"); // string
static TCHAR BASED_CODE szNrBChannels[]  = _T("NrBChannels"); // int 1,2
static TCHAR BASED_CODE szSupportMSN[]   = _T("SupportMSN"); // int 0,1
static TCHAR BASED_CODE szRejectWithoutNumber[]   = _T("RejectWithoutNumber"); // int 0,1
static TCHAR BASED_CODE szUseHostList[]   = _T("UseHostList"); // int 0,1

/////////////////////////////////////////////////////////////////////////////
// CISDNPage dialog


CISDNPage::CISDNPage(CSVView* pParent, int nIsdnNr) : CSVPage(CISDNPage::IDD)
{
	CString sTemp;
	m_pParent			= pParent;
	m_nIsdnNr			= nIsdnNr;	// Nummer der ISDN-Unit (1 oder 2)
	m_bLoadingHostList	= FALSE;

	if (nIsdnNr == ISDN1)
	{
		m_sSection = szSection1;
		m_sAppName = WK_APP_NAME_ISDN;
		sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_ISDN2,_T(""));
		m_bISDNOther = sTemp.CompareNoCase(_T("ISDNUnit.exe"))==0; 
	}
	else
	{
		m_sSection = szSection2;
		m_sAppName = WK_APP_NAME_ISDN2;
		sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_ISDN,_T(""));
		m_bISDNOther = sTemp.CompareNoCase(_T("ISDNUnit.exe"))==0; 
	}

	//{{AFX_DATA_INIT(CISDNPage)
	m_bISDN = FALSE;
	m_sRufNummer = _T("A");
	m_bKnock = TRUE;
	m_bSupportMSN = FALSE;
	m_iSystem = 0;
	m_iBChannels = 0;
	m_iAccept = 0;
	m_bUseHostList = FALSE;
	m_bRejectWithoutNumber = FALSE;
	m_sPrefix = _T("");
	m_iMinimumPrefix = 3;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}


void CISDNPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISDNPage)
	DDX_Control(pDX, IDC_TXT_PREFIX_MINIMUM, m_staticMinimumPrefix);
	DDX_Control(pDX, IDC_TXT_PREFIX, m_staticPrefix);
	DDX_Control(pDX, IDC_EDIT_PREFIX, m_editPrefix);
	DDX_Control(pDX, IDC_EDIT_PREFIX_MINIMUM, m_editMinimumPrefix);
	DDX_Control(pDX, IDC_REJECT_WITHOUT_NUMBER, m_btnRejectWithoutnumber);
	DDX_Control(pDX, IDC_USE_HOSTLIST, m_btnUseHostList);
	DDX_Control(pDX, IDC_HOST_LIST, m_EditHostList);
	DDX_Control(pDX, IDC_CHECK_ISDN, m_btnISDN);
	DDX_Control(pDX, IDC_RADIO_B1, m_btnB1);
	DDX_Control(pDX, IDC_RADIO_B2, m_btnB2);
	DDX_Control(pDX, IDC_RADIO_NR, m_btnNr);
	DDX_Control(pDX, IDC_RADIO_ALL, m_btnAll);
	DDX_Control(pDX, IDC_RADIO_NOTHING, m_btnNothing);
	DDX_Control(pDX, IDC_USAGE_LOW, m_btnLow);
	DDX_Control(pDX, IDC_USAGE_MEDIUM, m_btnMedium);
	DDX_Control(pDX, IDC_USAGE_HIGH, m_btnHigh);
	DDX_Control(pDX, IDC_ENABLE_KNOCK_BOX, m_btnKnock);
	DDX_Control(pDX, IDC_RUFNUMMER, m_editRufnummer);
	DDX_Check(pDX, IDC_CHECK_ISDN, m_bISDN);
	DDX_Text(pDX, IDC_RUFNUMMER, m_sRufNummer);
	DDX_Check(pDX, IDC_ENABLE_KNOCK_BOX, m_bKnock);
	DDX_Check(pDX, IDC_CHECK_MSN, m_bSupportMSN);
	DDX_Radio(pDX, IDC_USAGE_HIGH, m_iSystem);
	DDX_Radio(pDX, IDC_RADIO_B1, m_iBChannels);
	DDX_Radio(pDX, IDC_RADIO_NR, m_iAccept);
	DDX_Check(pDX, IDC_USE_HOSTLIST, m_bUseHostList);
	DDX_Check(pDX, IDC_REJECT_WITHOUT_NUMBER, m_bRejectWithoutNumber);
	DDX_Text(pDX, IDC_EDIT_PREFIX, m_sPrefix);
	DDX_Text(pDX, IDC_EDIT_PREFIX_MINIMUM, m_iMinimumPrefix);
	DDX_Control(pDX, IDC_COMBO_ISDN_COMP, m_cCompression);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CISDNPage, CSVPage)
	//{{AFX_MSG_MAP(CISDNPage)
	ON_BN_CLICKED(IDC_CHECK_ISDN, OnCheckIsdn)
	ON_BN_CLICKED(IDC_RADIO_NR, OnRadioNr)
	ON_BN_CLICKED(IDC_ENABLE_KNOCK_BOX, OnChange)
	ON_BN_CLICKED(IDC_USE_HOSTLIST, OnUseHostlist)
	ON_BN_CLICKED(IDC_REJECT_WITHOUT_NUMBER, OnRejectWithoutNumber)
	ON_EN_CHANGE(IDC_HOST_LIST, OnChangeHostList)
	ON_EN_CHANGE(IDC_RUFNUMMER, OnChangeRufnummer)
	ON_BN_CLICKED(IDC_COPY_ISDN_HOSTS, OnCopyIsdnHosts)
	ON_BN_CLICKED(IDC_USAGE_HIGH, OnChange)
	ON_BN_CLICKED(IDC_USAGE_LOW, OnChange)
	ON_BN_CLICKED(IDC_USAGE_MEDIUM, OnChange)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnChange)
	ON_BN_CLICKED(IDC_RADIO_NOTHING, OnChange)
	ON_BN_CLICKED(IDC_RADIO_B1, OnChange)
	ON_BN_CLICKED(IDC_RADIO_B2, OnChange)
	ON_EN_CHANGE(IDC_EDIT_PREFIX_MINIMUM, OnChange)
	ON_EN_CHANGE(IDC_EDIT_PREFIX, OnChange)
	ON_CBN_SELCHANGE(IDC_COMBO_ISDN_COMP, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CISDNPage message handlers
// Overrides
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::SaveChanges()
{
	WK_TRACE_USER(_T("ISDN-Einstellungen wurden geändert\n"));
	UpdateData();
	GetProfile().WriteString(theApp.GetModuleSection(),m_sAppName,m_bISDN ? _T("ISDNUnit.exe") : _T(""));

	GetProfile().WriteInt(m_sSection,szEnableKnockKnockBox,m_bKnock);
	GetProfile().WriteInt(m_sSection,szProcessorUsage,m_iSystem+1);
	GetProfile().WriteInt(m_sSection,szNrBChannels,m_iBChannels+1);

	GetProfile().WriteInt(m_sSection, szRejectWithoutNumber,m_bRejectWithoutNumber);
	GetProfile().WriteInt(m_sSection, szUseHostList,m_bUseHostList);
	GetProfile().WriteInt(m_sSection, CSD_COMPRESSION, m_cCompression.GetItemData(m_cCompression.GetCurSel()));

	GetProfile().WriteString(szSection1,_T("Prefix"),m_sPrefix);
	GetProfile().WriteString(szSection2,_T("Prefix"),m_sPrefix);

	GetProfile().WriteInt(szSection1,_T("MinimumPrefix"),m_iMinimumPrefix);
	GetProfile().WriteInt(szSection2,_T("MinimumPrefix"),m_iMinimumPrefix);


	CString sTmp;

	switch (m_iAccept)
	{
	case 0:
		sTmp = m_sRufNummer;
		break;
	case 1:
		sTmp = _T("A");
		break;
	case 2:
		sTmp = _T("R");
		break;
	default:
		sTmp = _T("A");
	}

	CWK_Profile::Encode(sTmp);
#ifdef _UNICODE
	GetProfile().WriteString(m_sSection,szOwnNumber,sTmp,TRUE);
#else
	GetProfile().WriteString(m_sSection,szOwnNumber,sTmp);
#endif

	SaveHostListToRegistry();
}

/////////////////////////////////////////////////////////////////////////////
void CISDNPage::CancelChanges()
{
	CString sTemp;
	sTemp = GetProfile().GetString(theApp.GetModuleSection(), m_sAppName,_T(""));
	m_bISDN = sTemp.CompareNoCase(_T("ISDNUnit.exe"))==0; 

	LoadHostListFromRegistry();

	m_sPrefix = GetProfile().GetString(szSection1,_T("Prefix"),_T(""));
	m_iMinimumPrefix = GetProfile().GetInt(szSection1,_T("MinimumPrefix"),m_iMinimumPrefix);
	m_bRejectWithoutNumber = GetProfile().GetInt(m_sSection,szRejectWithoutNumber, FALSE);
	m_bUseHostList = GetProfile().GetInt(m_sSection,szUseHostList, FALSE);

	m_bKnock = GetProfile().GetInt(m_sSection,szEnableKnockKnockBox,TRUE);
	m_bSupportMSN = GetProfile().GetInt(m_sSection,szSupportMSN,FALSE);
	m_iSystem = GetProfile().GetInt(m_sSection,szProcessorUsage,2)-1;
	int i = GetProfile().GetInt(m_sSection,szNrBChannels,1);

	if (GetDongle().IsDemo())
	{
		m_iBChannels = 0;
	}
	else
	{
		m_iBChannels = i-1;
	}


	CString sTmp;
#ifdef _UNICODE
	sTmp = GetProfile().GetString(m_sSection,szOwnNumber, _T(""), TRUE);
#else
	sTmp = GetProfile().GetString(m_sSection,szOwnNumber, _T(""));
#endif	


	if (!sTmp.IsEmpty())
	{
#ifdef _UNICODE
		CWK_Profile::Decode(sTmp, TRUE);
#else
		CWK_Profile::Decode(sTmp);
#endif	
		sTmp.TrimRight();
		
		if ((sTmp==_T("A")) || (sTmp==_T("a")))
		{
			m_sRufNummer = _T("");
			m_iAccept = 1;
		}
		else if ((sTmp==_T("R")) || (sTmp==_T("r")))
		{
			m_sRufNummer = _T("");
			m_iAccept = 2;
		}
		else
		{
			m_sRufNummer = sTmp;
			m_iAccept = 0;
		}
	}
	else
	{
		m_sRufNummer = _T("");
		m_iAccept = 1;
	}

	m_cCompression.SetCurSel(0);
	DWORD nCompression = (DWORD)GetProfile().GetInt(m_sSection, CSD_COMPRESSION, 0);
	int nCount = m_cCompression.GetCount();
	for (i=0; i<nCount; i++)
	{
		if (m_cCompression.GetItemData(i) == nCompression)
		{
			m_cCompression.SetCurSel(i);
			break;
		}
	}

	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnCheckIsdn() 
{
	SetModified();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNPage::OnInitDialog() 
{
	CString sTemp;
	int nItem;
	CSVPage::OnInitDialog();
	
	// Einen nicht proportionalen Font wählen
	m_fixedFont.CreatePointFont(10*10, GetFixFontFaceName());
	m_EditHostList.SetFont(&m_fixedFont, TRUE);
	m_EditHostList.SetEventMask(ENM_CHANGE);

	sTemp.LoadString(IDS_NOTHING);
	nItem = m_cCompression.AddString(sTemp);
	m_cCompression.SetItemData(nItem, 0);

	GetDlgItemText(IDC_USAGE_LOW, sTemp);
	sTemp.Replace(_T("&"), _T(""));
	nItem = m_cCompression.AddString(sTemp);
	m_cCompression.SetItemData(nItem, 1);

	GetDlgItemText(IDC_USAGE_MEDIUM, sTemp);
	sTemp.Replace(_T("&"), _T(""));
	nItem = m_cCompression.AddString(sTemp);
	m_cCompression.SetItemData(nItem, 6);

	GetDlgItemText(IDC_USAGE_HIGH, sTemp);
	sTemp.Replace(_T("&"), _T(""));
	nItem = m_cCompression.AddString(sTemp);
	m_cCompression.SetItemData(nItem, 9);

	sTemp = GetProfile().GetString(theApp.GetModuleSection(),m_sAppName,_T(""));
	m_bISDN = sTemp.CompareNoCase(_T("ISDNUnit.exe"))==0; 

	if (m_nIsdnNr == ISDN1)
	{
		if (m_bISDNOther)
			m_btnISDN.EnableWindow(FALSE);
	}
	else
	{
		if (!m_bISDNOther)
			m_btnISDN.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	
	CancelChanges();
	GetDlgItemText(IDC_CHECK_MSN, sTemp);
	SetDlgItemText(IDC_CHECK_TXT_MSN, sTemp);

	m_btnNothing.ShowWindow(SW_HIDE);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::ShowHide()
{
	UpdateData();

	if (GetDongle().IsDemo())
	{
		m_btnB1.EnableWindow(FALSE);
		m_btnB2.EnableWindow(FALSE);
	}
	else
	{
		m_btnB1.EnableWindow(m_bISDN);
		m_btnB2.EnableWindow(m_bISDN);
	}

	m_btnNr.EnableWindow(m_bISDN);
	m_btnAll.EnableWindow(m_bISDN);
	m_btnNothing.EnableWindow(m_bISDN);
	m_btnLow.EnableWindow(m_bISDN);
	m_btnMedium.EnableWindow(m_bISDN);
	m_btnHigh.EnableWindow(m_bISDN);
	m_btnKnock.EnableWindow(m_bISDN);
	m_editRufnummer.EnableWindow(m_bISDN && (m_iAccept==0));
	m_btnRejectWithoutnumber.EnableWindow(m_bISDN);
	m_btnUseHostList.EnableWindow(m_bISDN);
	m_EditHostList.EnableWindow(m_bUseHostList && m_bISDN);
	m_cCompression.EnableWindow(m_bISDN);

	if (m_nIsdnNr == 1)
	{
		m_editPrefix.EnableWindow(m_bISDN);
		m_editMinimumPrefix.EnableWindow(m_bISDN);
	}
	else
	{
		m_editPrefix.ShowWindow(SW_HIDE);
		m_editMinimumPrefix.ShowWindow(SW_HIDE);
		m_staticPrefix.ShowWindow(SW_HIDE);
		m_staticMinimumPrefix.ShowWindow(SW_HIDE);
	}

	if (m_bUseHostList && m_bISDN)
		m_EditHostList.SetBackgroundColor(FALSE, RGB(255, 255, 255));
	else
		m_EditHostList.SetBackgroundColor(FALSE, RGB(192, 192, 192));

}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnChangeRufnummer() 
{
	UpdateData();
	CString sTmp;

	int i,c;
	BOOL bValid = TRUE;

	c = m_sRufNummer.GetLength();
	for (i=0;i<c;i++)
	{
		if (isdigit(m_sRufNummer[i]) || 
			(m_sRufNummer[i]==_T(' ')) || 
			(m_sRufNummer[i]==_T('-')) || 
			(m_sRufNummer[i]==_T('/')))
		{
			sTmp += m_sRufNummer[i];
		}
		else
		{
			bValid = FALSE;
			break;
		}
	}

	if (!bValid)
	{
		m_sRufNummer = sTmp;
		UpdateData(FALSE);
		m_editRufnummer.SetSel(i,i);
	}
	OnChange();
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CISDNPage::SaveHostListToRegistry()
{
	TCHAR szBuffer[1024];
	CString sLineNr;
	CString sKey;

	if (!m_bUseHostList)
		return TRUE;

	sKey.Format(_T("%s\\HostList"), m_sSection);

	GetProfile().DeleteSection(sKey);

	for (int nI = 0; nI < m_EditHostList.GetLineCount(); nI++)
	{
		int nCnt = m_EditHostList.GetLine(nI, szBuffer, 1024);
		if (nCnt >= 2)
			szBuffer[nCnt-2] = 0;
		sLineNr.Format(_T("%05d"), nI);
		GetProfile().WriteString(sKey, sLineNr, szBuffer);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CISDNPage::IsHostInHostList(const CString &sHost)
{
	TCHAR	szBuffer[1024];
	BOOL	bFind = FALSE;

	for (int nI = 0; nI < m_EditHostList.GetLineCount() && !bFind; nI++)
	{
		int nCnt = m_EditHostList.GetLine(nI, szBuffer, 1024);
		if (nCnt >= 2)
			szBuffer[nCnt-2] = 0;

		if (sHost == CString(szBuffer))
			bFind = TRUE;
	}
	return bFind;
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CISDNPage::LoadHostListFromRegistry()
{
	CString sLineNr;
	CString sHost;
	int nI = 0;
	CString sKey;

	sKey.Format(_T("%s\\HostList"), m_sSection);
   	m_EditHostList.SetSel(0, -1);

	int b = TRUE;
	while (b)
	{
		sLineNr.Format(_T("%05d"), nI++);
		sHost = GetProfile().GetString(sKey, sLineNr, _T(""));

		sLineNr.Format(_T("%05d"), nI);
		if (GetProfile().GetString(sKey, sLineNr, _T("<<EndeDerListe>>")) != _T("<<EndeDerListe>>"))
		{
			m_EditHostList.ReplaceSel(sHost + _T("\r\n"));
		}
		else
		{
			m_EditHostList.ReplaceSel(sHost); // Die letzte Zeile hat kein CrLf
			b = FALSE;
		}
			
	
	}

	m_bLoadingHostList = TRUE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnRadioNr() 
{
	OnChange();
	m_editRufnummer.SetFocus();
	m_editRufnummer.SetSel(0, -1);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnChange() 
{
	if (m_bLoadingHostList)
	{
		SetModified(TRUE,FALSE);
		ShowHide();
		m_pParent->AddInitApp(WK_APP_NAME_ISDN);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnChangeHostList() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	if (m_bLoadingHostList)
	{
		SetModified(TRUE);
		ShowHide();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnUseHostlist() 
{
	SetModified(TRUE);

	ShowHide();
}

/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnRejectWithoutNumber() 
{
	SetModified(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnCopyIsdnHosts() 
{
	CHostArray	Hosts;

	Hosts.Load(GetProfile());
   	m_EditHostList.SetSel(0, 0);

	// Alle ISDN-Hosts durchsuchen
	for (int nI = 0; nI < Hosts.GetSize(); nI++)
	{
		if (Hosts.GetAt(nI)->GetTyp() == _T("ISDN"))
		{
			CString sNumber	= Hosts.GetAt(nI)->GetNumber();
			// Ist der Host schon in der Liste?
			if (Hosts.GetAt(nI)->IsISDN())
			{
				if (!IsHostInHostList(sNumber))
					m_EditHostList.ReplaceSel(sNumber + _T("\r\n"));
			}
		}
	}
}
