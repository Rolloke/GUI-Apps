/* GlobalReplace: WK_GetProfileString --> GetProfile().GetString */
/* GlobalReplace: WK_GetProfileInt --> GetProfile().GetInt */
// ISDNPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ISDNPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szModules[] = _T("SecurityLauncher\\Modules");

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


CISDNPage::CISDNPage(CSVView* pParent) : CSVPage(CISDNPage::IDD)
{
	CString sTemp;
	m_pParent			= pParent;

	//{{AFX_DATA_INIT(CISDNPage)
	m_bISDN = FALSE;
	m_sRufNummer = _T("A");
	m_iBChannels = 0;
	m_bUseMSN = FALSE;
	m_sPrefix = _T("");
	m_iMinimumPrefix = 3;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}


void CISDNPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISDNPage)
	DDX_Control(pDX, IDC_EDIT_PREFIX_MINIMUM, m_editMinimumPrefix);
	DDX_Control(pDX, IDC_EDIT_PREFIX, m_editPrefix);
	DDX_Control(pDX, IDC_CHECK_ISDN, m_btnISDN);
	DDX_Control(pDX, IDC_RADIO_B1, m_btnB1);
	DDX_Control(pDX, IDC_RADIO_B2, m_btnB2);
	DDX_Control(pDX, IDC_RUFNUMMER, m_editRufnummer);
	DDX_Check(pDX, IDC_CHECK_ISDN, m_bISDN);
	DDX_Text(pDX, IDC_RUFNUMMER, m_sRufNummer);
	DDX_Radio(pDX, IDC_RADIO_B1, m_iBChannels);
	DDX_Check(pDX, IDC_CHECK_MSN, m_bUseMSN);
	DDX_Text(pDX, IDC_EDIT_PREFIX, m_sPrefix);
	DDX_Text(pDX, IDC_EDIT_PREFIX_MINIMUM, m_iMinimumPrefix);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CISDNPage, CSVPage)
	//{{AFX_MSG_MAP(CISDNPage)
	ON_BN_CLICKED(IDC_CHECK_ISDN, OnCheckIsdn)
	ON_BN_CLICKED(IDC_RADIO_B1, OnChange)
	ON_EN_CHANGE(IDC_RUFNUMMER, OnChangeRufnummer)
	ON_BN_CLICKED(IDC_CHECK_MSN, OnCheckMsn)
	ON_BN_CLICKED(IDC_RADIO_B2, OnChange)
	ON_EN_CHANGE(IDC_EDIT_PREFIX, OnChange)
	ON_EN_CHANGE(IDC_EDIT_PREFIX_MINIMUM, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CISDNPage message handlers
// Overrides
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNPage::IsDataValid()
{
	if (m_bUseMSN
		&& m_sRufNummer.IsEmpty()
		)
	{
		m_bUseMSN = FALSE;
		UpdateData(FALSE);
		ShowHide();
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::SaveChanges()
{
	UpdateData();

	GetProfile().WriteString(szModules,WK_APP_NAME_ISDN,m_bISDN ? _T("ISDNUnit.exe") : _T(""));
	GetProfile().WriteString(szModules,WK_APP_NAME_ISDN2,m_bISDN ? _T("ISDNUnit.exe") : _T(""));
	
	GetProfile().WriteString(szSection1,_T("Prefix"),m_sPrefix);
	GetProfile().WriteString(szSection2,_T("Prefix"),m_sPrefix);

	GetProfile().WriteInt(szSection1,_T("MinimumPrefix"),m_iMinimumPrefix);
	GetProfile().WriteInt(szSection2,_T("MinimumPrefix"),m_iMinimumPrefix);

	GetProfile().WriteInt(szSection1,szNrBChannels,m_iBChannels+1);
	GetProfile().WriteInt(szSection2,szNrBChannels,1);

	CString sTmp;

	if (m_bUseMSN)
	{
		sTmp = m_sRufNummer;
	}
	else
	{
		sTmp = _T("A");
	}

	CWK_Profile::Encode(sTmp);

	GetProfile().WriteString(szSection1,szOwnNumber,sTmp);
	GetProfile().WriteString(szSection2,szOwnNumber,sTmp);
}

/////////////////////////////////////////////////////////////////////////////
void CISDNPage::CancelChanges()
{
	CString sTemp;
	sTemp = GetProfile().GetString(szModules,WK_APP_NAME_ISDN,_T(""));
	m_bISDN = sTemp.CompareNoCase(_T("ISDNUnit.exe"))==0; 
	m_sPrefix = GetProfile().GetString(szSection1,_T("Prefix"),_T(""));
	m_iMinimumPrefix = GetProfile().GetInt(szSection1,_T("MinimumPrefix"),m_iMinimumPrefix);

	int i = GetProfile().GetInt(szSection1,szNrBChannels,1);
	if (GetDongle().IsDemo())
	{
		m_iBChannels = 0;
	}
	else
	{
		m_iBChannels = i-1;
	}

	CString sTmp;
	sTmp = GetProfile().GetString(szSection1,szOwnNumber,_T(""));
	if (!sTmp.IsEmpty())
	{
		CWK_Profile::Decode(sTmp);
		sTmp.TrimRight();
		
		if ((sTmp==_T("A")) || (sTmp==_T("a")))
		{
			m_bUseMSN = FALSE;
			m_sRufNummer = _T("");
		}
		else
		{
			m_bUseMSN = TRUE;
			m_sRufNummer = sTmp;
		}
	}
	else
	{
		m_sRufNummer = _T("A");
		m_bUseMSN = FALSE;
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
	CSVPage::OnInitDialog();
	
	CancelChanges();

	
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
    m_editRufnummer.EnableWindow(m_bISDN && m_bUseMSN);
	m_editPrefix.EnableWindow(m_bISDN);
	m_editMinimumPrefix.EnableWindow(m_bISDN);
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
/////////////////////////////////////////////////////////////////////////////
void CISDNPage::OnChange() 
{
	SetModified(TRUE,FALSE);
	ShowHide();
	m_pParent->AddInitApp(WK_APP_NAME_ISDN);
	m_pParent->AddInitApp(WK_APP_NAME_ISDN2);
}

void CISDNPage::OnCheckMsn() 
{
	UpdateData();
	OnChange();
	if (m_bUseMSN)
	{
		m_editRufnummer.EnableWindow();
		m_editRufnummer.SetFocus();
		m_editRufnummer.SetSel(0, -1);
	}
	else
	{
		m_editRufnummer.EnableWindow(FALSE);
	}
}

