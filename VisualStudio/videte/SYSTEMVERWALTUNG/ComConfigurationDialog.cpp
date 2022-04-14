// ComConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ComConfigurationDialog.h"
#include ".\comconfigurationdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComConfigurationDialog dialog
CComConfigurationDialog::CComConfigurationDialog(const CString& sCom,
												 CComParameters* pComParameters,
												 CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CComConfigurationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComConfigurationDialog)
	m_iIndexBaudRate = -1;
	m_iIndexDataBits = -1;
	m_iIndexParity = -1;
	m_iIndexStopBits = -1;
	//}}AFX_DATA_INIT
	m_pComParams = pComParameters;
	m_sCom = sCom;
	m_bModified = FALSE;
	m_bShowDefaultBtn = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::FillComboboxBaudRate()
{
	// erst mal alle Eintraege loeschen
	m_cbBaudRate.ResetContent();
	int iIndex = -1;
	CString sBaudRate;
	sBaudRate.Format(_T("%i"), CBR_110);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_110);
	sBaudRate.Format(_T("%i"), CBR_300);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_300);
	sBaudRate.Format(_T("%i"), CBR_600);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_600);
	sBaudRate.Format(_T("%i"), CBR_1200);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_1200);
	sBaudRate.Format(_T("%i"), CBR_2400);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_2400);
	sBaudRate.Format(_T("%i"), CBR_4800);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_4800);
	sBaudRate.Format(_T("%i"), CBR_9600);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_9600);
	sBaudRate.Format(_T("%i"), CBR_14400);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_14400);
	sBaudRate.Format(_T("%i"), CBR_19200);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_19200);
	sBaudRate.Format(_T("%i"), CBR_38400);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_38400);
	sBaudRate.Format(_T("%i"), CBR_56000);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_56000);
	sBaudRate.Format(_T("%i"), CBR_57600);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_57600);
	sBaudRate.Format(_T("%i"), CBR_115200);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_115200);
	sBaudRate.Format(_T("%i"), CBR_128000);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_128000);
	sBaudRate.Format(_T("%i"), CBR_256000);
	iIndex = m_cbBaudRate.AddString(sBaudRate);
	m_cbBaudRate.SetItemData(iIndex, CBR_256000);
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::FillComboboxParity()
{
	// erst mal alle Eintraege loeschen
	m_cbParity.ResetContent();
	int iIndex = -1;
	CString sParity;
	sParity.LoadString(IDS_NOPARITY);
	iIndex = m_cbParity.AddString(sParity);
	m_cbParity.SetItemData(iIndex, NOPARITY);
	sParity.LoadString(IDS_ODDPARITY);
	iIndex = m_cbParity.AddString(sParity);
	m_cbParity.SetItemData(iIndex, ODDPARITY);
	sParity.LoadString(IDS_EVENPARITY);
	iIndex = m_cbParity.AddString(sParity);
	m_cbParity.SetItemData(iIndex, EVENPARITY);
	sParity.LoadString(IDS_MARKPARITY);
	iIndex = m_cbParity.AddString(sParity);
	m_cbParity.SetItemData(iIndex, MARKPARITY);
	sParity.LoadString(IDS_SPACEPARITY);
	iIndex = m_cbParity.AddString(sParity);
	m_cbParity.SetItemData(iIndex, SPACEPARITY);
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::FillComboboxDataBits()
{
	// erst mal alle Eintraege loeschen
	m_cbDataBits.ResetContent();
	int iIndex = -1;
	iIndex = m_cbDataBits.AddString(_T("4"));
	m_cbDataBits.SetItemData(iIndex, 4);
	iIndex = m_cbDataBits.AddString(_T("5"));
	m_cbDataBits.SetItemData(iIndex, 5);
	iIndex = m_cbDataBits.AddString(_T("6"));
	m_cbDataBits.SetItemData(iIndex, 6);
	iIndex = m_cbDataBits.AddString(_T("7"));
	m_cbDataBits.SetItemData(iIndex, 7);
	iIndex = m_cbDataBits.AddString(_T("8"));
	m_cbDataBits.SetItemData(iIndex, 8);
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::FillComboboxStopBits()
{
	// erst mal alle Eintraege loeschen
	m_cbStopBits.ResetContent();
	int iIndex = -1;
	iIndex = m_cbStopBits.AddString(_T("1"));
	m_cbStopBits.SetItemData(iIndex, ONESTOPBIT);
	iIndex = m_cbStopBits.AddString(_T("1.5"));
	m_cbStopBits.SetItemData(iIndex, ONE5STOPBITS);
	iIndex = m_cbStopBits.AddString(_T("2"));
	m_cbStopBits.SetItemData(iIndex, TWOSTOPBITS);
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::InitData()
{
	if (m_pComParams) {
		m_iIndexBaudRate = -1;
		for (int i=0 ; i<m_cbBaudRate.GetCount() ; i++) {
			if (m_cbBaudRate.GetItemData(i) == m_pComParams->GetBaudRate() ) {
				m_iIndexBaudRate = i;
				break;
			}
		}
		m_iIndexParity = -1;
		for (i=0 ; i<m_cbParity.GetCount() ; i++) {
			if (m_cbParity.GetItemData(i) == m_pComParams->GetParity() ) {
				m_iIndexParity = i;
				break;
			}
		}
		m_iIndexDataBits = -1;
		for (i=0 ; i<m_cbDataBits.GetCount() ; i++) {
			if (m_cbDataBits.GetItemData(i) == m_pComParams->GetDataBits() ) {
				m_iIndexDataBits = i;
				break;
			}
		}
		m_iIndexStopBits = -1;
		for (i=0 ; i<m_cbStopBits.GetCount() ; i++) {
			if (m_cbStopBits.GetItemData(i) == m_pComParams->GetStopBits() ) {
				m_iIndexStopBits = i;
				break;
			}
		}
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComConfigurationDialog)
	DDX_Control(pDX, IDC_COMBO_STOPBITS, m_cbStopBits);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_cbParity);
	DDX_Control(pDX, IDC_COMBO_DATABITS, m_cbDataBits);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_cbBaudRate);
	DDX_CBIndex(pDX, IDC_COMBO_BAUDRATE, m_iIndexBaudRate);
	DDX_CBIndex(pDX, IDC_COMBO_DATABITS, m_iIndexDataBits);
	DDX_CBIndex(pDX, IDC_COMBO_PARITY, m_iIndexParity);
	DDX_CBIndex(pDX, IDC_COMBO_STOPBITS, m_iIndexStopBits);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CComConfigurationDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CComConfigurationDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, OnSelchangeComboBaudRate)
	ON_CBN_SELCHANGE(IDC_COMBO_PARITY, OnSelchangeComboParity)
	ON_CBN_SELCHANGE(IDC_COMBO_DATABITS, OnSelchangeComboDataBits)
	ON_CBN_SELCHANGE(IDC_COMBO_STOPBITS, OnSelchangeComboStopBits)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_COM_DEFAULT, OnBnClickedBtnComDefault)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CComConfigurationDialog message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CComConfigurationDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}
	
	// TODO: Add extra initialization here
	CString sTitle;
	sTitle.LoadString(IDS_TITLE_COM_CONFIGURATION);
	sTitle += _T(" ");
	sTitle += m_sCom;
	SetWindowText(sTitle);

	FillComboboxBaudRate();
	FillComboboxParity();
	FillComboboxDataBits();
	FillComboboxStopBits();

	InitData();

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	if (m_bShowDefaultBtn)
	{
		GetDlgItem(IDC_BTN_COM_DEFAULT)->ShowWindow(SW_SHOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::OnSelchangeComboBaudRate() 
{
	UpdateData();
	m_bModified = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::OnSelchangeComboParity() 
{
	UpdateData();
	m_bModified = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::OnSelchangeComboDataBits() 
{
	UpdateData();
	m_bModified = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::OnSelchangeComboStopBits() 
{
	UpdateData();
	m_bModified = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CComConfigurationDialog::OnOK() 
{
	BOOL bCanClose = TRUE;
	if(m_bModified) {
		// Warnung ausgeben
		CString sMsg;
		sMsg.LoadString(IDS_COM_PARAMS_CHANGED);
		int iReturn = AfxMessageBox(sMsg, MB_YESNO|MB_ICONQUESTION);
		switch (iReturn) {
		case IDYES:
			if (m_pComParams) {
				m_pComParams->SetBaudRate(m_cbBaudRate.GetItemData(m_iIndexBaudRate));
				m_pComParams->SetParity((BYTE)m_cbParity.GetItemData(m_iIndexParity));
				m_pComParams->SetDataBits((BYTE)m_cbDataBits.GetItemData(m_iIndexDataBits));
				m_pComParams->SetStopBits((BYTE)m_cbStopBits.GetItemData(m_iIndexStopBits));
			}
			break;
		case IDNO:
		default:
			bCanClose = FALSE;
			break;
		}
	}
	
	if (bCanClose) {
		CWK_Dialog::OnOK();
	}
}

void CComConfigurationDialog::OnBnClickedBtnComDefault()
{
	EndDialog(IDC_BTN_COM_DEFAULT);
}
