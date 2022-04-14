// StandardEntriesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sditest.h"
#include "SDITestDoc.h"
#include "SDITestView.h"
#include "StandardEntriesDlg.h"

//#include "DocAvmCustomerSafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStandardEntriesDlg dialog
CStandardEntriesDlg::CStandardEntriesDlg(CSDITestDoc* pDoc, CSDITestView* pView)
	: CWK_Dialog(CStandardEntriesDlg::IDD, pView)
{
	//{{AFX_DATA_INIT(CStandardEntriesDlg)
	m_bRepeat = FALSE;
	m_uRepeatTime = 1000;
	m_iRepeatHow = 0;
	m_bLimited = FALSE;
	m_uLimited = 2;
	m_bBitError = FALSE;
	m_bBLZ = FALSE;
	m_bKonto = FALSE;
	m_sBLZ = _T("");
	m_sKonto = _T("");
	m_bDate = FALSE;
	m_bTime = FALSE;
	m_sDate = _T("");
	m_sTime = _T("");
	m_bBetrag = FALSE;
	m_sBetrag = _T("");
	m_bTAN = FALSE;
	m_sTAN = _T("");
	m_bCurrency = FALSE;
	m_sCurrency = _T("");
	m_bWSID = FALSE;
	m_sWSID = _T("");
	m_bSendSingleBytes = FALSE;
	m_uSendDelay = 6;
	//}}AFX_DATA_INIT
	if (NULL == pDoc) {
		WK_TRACE_ERROR(_T("%s pDoc = NULL\n"), __FILE__);
		return;
	}
	m_pDocument = pDoc;
	if (NULL == pView) {
		WK_TRACE_ERROR(_T("%s pView = NULL\n"), __FILE__);
		return;
	}
	m_pView = pView;
	Create(IDD, m_pView);
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::ShowHide()
{
	// Wenn Daten gesendet werden, sollen die Einstellungen nicht veraendert werden
	BOOL bAllowed = ((GetDocument()->m_bStarted) == FALSE);

	// Den Wiederholungsbutton anzeigen
	BOOL bEnable = bAllowed;
	m_btnRepeat.EnableWindow(bEnable);

	// Die weiteren Wiederholungsoptionen anzeigen
	bEnable = bAllowed && m_bRepeat;
	m_editRepeatTime.EnableWindow(bEnable);
	m_btnChosenUnits.EnableWindow(bEnable);
	m_btnRandom.EnableWindow(bEnable);
	m_btnSuccessive.EnableWindow(bEnable);
	m_btnLimited.EnableWindow(bEnable);

	// Die begrenzte Anzahl anzeigen
	bEnable = bAllowed && m_bRepeat && m_bLimited;
	m_editLimited.EnableWindow(bEnable);

	// Den Bitfehlerbutton anzeigen
	bEnable = bAllowed;
	m_btnBitError.EnableWindow(bEnable);

	// Den Single-Byte-Send-Button anzeigen
	bEnable = bAllowed;
	m_btnSendSingleBytes.EnableWindow(bEnable);

	// Send delay
	bEnable = bAllowed && m_bSendSingleBytes;
	m_editSendDelay.EnableWindow(bEnable);

//	BOOL bIsAvmCustomer = GetDocument()->IsKindOf(RUNTIME_CLASS(CDocAvmCustomerSafe));
	// Die 'explizit vorgeben'-Buttons anzeigen
	BOOL bAvailable = FALSE;
	// Datum
	bAvailable = CSDIControl::HasDate( GetDocument()->GetType() );
	bEnable = bAllowed && bAvailable;
	m_btnDate.EnableWindow(bEnable);
	m_editDate.EnableWindow(bEnable && m_bDate);
	// Zeit
	bAvailable = CSDIControl::HasTime( GetDocument()->GetType() );
	bEnable = bAllowed && bAvailable;
	m_btnTime.EnableWindow(bEnable);
	m_editTime.EnableWindow(bEnable && m_bTime);
	// WSID
	bAvailable = CSDIControl::HasWSID( GetDocument()->GetType() );
/*	if(bIsAvmCustomer) {
		CString sText;
		sText.LoadString(IDS_CUSTOMER_ID);
		m_btnWSID.SetWindowText(sText);
		bAvailable = TRUE;
	}
*/	bEnable = bAllowed && bAvailable;
	m_btnWSID.EnableWindow(bEnable);
	m_editWSID.EnableWindow(bEnable && m_bWSID);
	// TAN
	bAvailable = CSDIControl::HasTAN( GetDocument()->GetType() );
	bEnable = bAllowed && bAvailable;
	m_btnTAN.EnableWindow(bEnable);
/*	if(bIsAvmCustomer) {
		CString sText;
		sText.LoadString(IDS_PROCESS_ID);
		m_btnTAN.SetWindowText(sText);
	}
*/	m_editTAN.EnableWindow(bEnable && m_bTAN);
	// BLZ
	bAvailable = CSDIControl::HasBankCode( GetDocument()->GetType() );
/*	if(bIsAvmCustomer) {
		CString sText;
		sText.LoadString(IDS_FIRST_NAME);
		m_btnBLZ.SetWindowText(sText);
		bAvailable = TRUE;
	}
*/	bEnable = bAllowed && bAvailable;
	m_btnBLZ.EnableWindow(bEnable);
	m_editBLZ.EnableWindow(bEnable && m_bBLZ);
	// Konto
	bAvailable = CSDIControl::HasAccount( GetDocument()->GetType() );
	bEnable = bAllowed && bAvailable;
	m_btnKonto.EnableWindow(bEnable);
/*	if(bIsAvmCustomer) {
		CString sText;
		sText.LoadString(IDS_LAST_NAME);
		m_btnKonto.SetWindowText(sText);
	}
*/	m_editKonto.EnableWindow(bEnable && m_bKonto);
	// Betrag
	bAvailable = CSDIControl::HasValue( GetDocument()->GetType() );
/*	if(bIsAvmCustomer) {
		CString sText;
		sText.LoadString(IDS_BOX_ID);
		m_btnBetrag.SetWindowText(sText);
		bAvailable = TRUE;
	}
*/	bEnable = bAllowed && bAvailable;
	m_btnBetrag.EnableWindow(bEnable);
	m_editBetrag.EnableWindow(bEnable && m_bBetrag);
	// Waehrung
	bAvailable = CSDIControl::HasCurrency( GetDocument()->GetType() );
	bEnable = bAllowed && bAvailable;
	m_btnCurrency.EnableWindow(bEnable);
	m_cbCurrency.EnableWindow(bEnable && m_bCurrency);
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::InitializeData()
{
	m_bRepeat			= GetDocument()->m_bRepeat;
	m_uRepeatTime		= GetDocument()->m_uRepeatTime;
	m_iRepeatHow		= GetDocument()->m_iRepeatHow;
	m_bLimited			= GetDocument()->m_bLimited;
	m_uLimited			= GetDocument()->m_uLimited;
	m_bBitError			= GetDocument()->m_bBitError;
	m_bSendSingleBytes	= GetDocument()->m_bSendSingleBytes;
	m_uSendDelay		= GetDocument()->m_uSendDelay;
	m_sDate				= GetDocument()->m_sDate;
	m_bDate				= !m_sDate.IsEmpty();
	m_sTime				= GetDocument()->m_sTime;
	m_bTime				= !m_sTime.IsEmpty();
	m_sBLZ				= GetDocument()->m_sBLZ;
	m_bBLZ				= !m_sBLZ.IsEmpty();
	m_sKonto			= GetDocument()->m_sKonto;
	m_bKonto			= !m_sKonto.IsEmpty();
	m_sBetrag			= GetDocument()->m_sBetrag;
	m_bBetrag			= !m_sBetrag.IsEmpty(); 
	m_sTAN				= GetDocument()->m_sTAN;
	m_bTAN				= !m_sTAN.IsEmpty();
	m_sWSID				= GetDocument()->m_sWSID;
	m_bWSID				= !m_sWSID.IsEmpty();

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStandardEntriesDlg)
	DDX_Control(pDX, IDC_EDIT_SEND_DELAY, m_editSendDelay);
	DDX_Control(pDX, IDC_CHECK_SEND_SINGLE_BYTE, m_btnSendSingleBytes);
	DDX_Control(pDX, IDC_EDIT_WSID, m_editWSID);
	DDX_Control(pDX, IDC_CHECK_WSID, m_btnWSID);
	DDX_Control(pDX, IDC_COMBO_CURRENCY, m_cbCurrency);
	DDX_Control(pDX, IDC_CHECK_CURRENCY, m_btnCurrency);
	DDX_Control(pDX, IDC_EDIT_TIME, m_editTime);
	DDX_Control(pDX, IDC_EDIT_DATE, m_editDate);
	DDX_Control(pDX, IDC_CHECK_TIME, m_btnTime);
	DDX_Control(pDX, IDC_CHECK_DATE, m_btnDate);
	DDX_Control(pDX, IDC_CHECK_REPEAT, m_btnRepeat);
	DDX_Control(pDX, IDC_EDIT_REPEAT_TIME, m_editRepeatTime);
	DDX_Control(pDX, IDC_RADIO_CHOSEN_UNITS, m_btnChosenUnits);
	DDX_Control(pDX, IDC_RADIO_RANDOM, m_btnRandom);
	DDX_Control(pDX, IDC_RADIO_SUCCESSIVE, m_btnSuccessive);
	DDX_Control(pDX, IDC_CHECK_LIMITED, m_btnLimited);
	DDX_Control(pDX, IDC_EDIT_LIMITED, m_editLimited);
	DDX_Control(pDX, IDC_CHECK_BIT_ERROR, m_btnBitError);
	DDX_Control(pDX, IDC_CHECK_KONTO, m_btnKonto);
	DDX_Control(pDX, IDC_EDIT_KONTO, m_editKonto);
	DDX_Control(pDX, IDC_CHECK_BLZ, m_btnBLZ);
	DDX_Control(pDX, IDC_EDIT_BLZ, m_editBLZ);
	DDX_Control(pDX, IDC_EDIT_TAN, m_editTAN);
	DDX_Control(pDX, IDC_CHECK_TAN, m_btnTAN);
	DDX_Control(pDX, IDC_CHECK_BETRAG, m_btnBetrag);
	DDX_Control(pDX, IDC_EDIT_BETRAG, m_editBetrag);
	DDX_Check(pDX, IDC_CHECK_REPEAT, m_bRepeat);
	DDX_Text(pDX, IDC_EDIT_REPEAT_TIME, m_uRepeatTime);
	DDV_MinMaxUInt(pDX, m_uRepeatTime, 1, 4294967);
	DDX_Radio(pDX, IDC_RADIO_CHOSEN_UNITS, m_iRepeatHow);
	DDX_Check(pDX, IDC_CHECK_LIMITED, m_bLimited);
	DDX_Text(pDX, IDC_EDIT_LIMITED, m_uLimited);
	DDV_MinMaxUInt(pDX, m_uLimited, 1, 4294967294);
	DDX_Check(pDX, IDC_CHECK_BIT_ERROR, m_bBitError);
	DDX_Check(pDX, IDC_CHECK_BLZ, m_bBLZ);
	DDX_Check(pDX, IDC_CHECK_KONTO, m_bKonto);
	DDX_Text(pDX, IDC_EDIT_BLZ, m_sBLZ);
	DDV_MaxChars(pDX, m_sBLZ, 8);
	DDX_Text(pDX, IDC_EDIT_KONTO, m_sKonto);
	DDV_MaxChars(pDX, m_sKonto, 16);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bDate);
	DDX_Check(pDX, IDC_CHECK_TIME, m_bTime);
	DDX_Text(pDX, IDC_EDIT_DATE, m_sDate);
	DDV_MaxChars(pDX, m_sDate, 8);
	DDX_Text(pDX, IDC_EDIT_TIME, m_sTime);
	DDV_MaxChars(pDX, m_sTime, 6);
	DDX_Check(pDX, IDC_CHECK_BETRAG, m_bBetrag);
	DDX_Text(pDX, IDC_EDIT_BETRAG, m_sBetrag);
	DDV_MaxChars(pDX, m_sBetrag, 8);
	DDX_Check(pDX, IDC_CHECK_TAN, m_bTAN);
	DDX_Text(pDX, IDC_EDIT_TAN, m_sTAN);
	DDV_MaxChars(pDX, m_sTAN, 4);
	DDX_Check(pDX, IDC_CHECK_CURRENCY, m_bCurrency);
	DDX_CBString(pDX, IDC_COMBO_CURRENCY, m_sCurrency);
	DDV_MaxChars(pDX, m_sCurrency, 3);
	DDX_Check(pDX, IDC_CHECK_WSID, m_bWSID);
	DDX_Text(pDX, IDC_EDIT_WSID, m_sWSID);
	DDV_MaxChars(pDX, m_sWSID, 6);
	DDX_Check(pDX, IDC_CHECK_SEND_SINGLE_BYTE, m_bSendSingleBytes);
	DDX_Text(pDX, IDC_EDIT_SEND_DELAY, m_uSendDelay);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CStandardEntriesDlg, CWK_Dialog)
	//{{AFX_MSG_MAP(CStandardEntriesDlg)
	ON_BN_CLICKED(IDC_CHECK_BIT_ERROR, OnCheckBitError)
	ON_BN_CLICKED(IDC_CHECK_LIMITED, OnCheckLimited)
	ON_BN_CLICKED(IDC_CHECK_REPEAT, OnCheckRepeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LIMITED, OnKillfocusEditLimited)
	ON_EN_KILLFOCUS(IDC_EDIT_REPEAT_TIME, OnKillfocusEditRepeatTime)
	ON_BN_CLICKED(IDC_RADIO_CHOSEN_UNITS, OnRadioRepeatHow)
	ON_BN_CLICKED(IDC_CHECK_BLZ, OnCheckBlz)
	ON_BN_CLICKED(IDC_CHECK_KONTO, OnCheckKonto)
	ON_EN_KILLFOCUS(IDC_EDIT_BLZ, OnKillfocusEditBlz)
	ON_EN_KILLFOCUS(IDC_EDIT_KONTO, OnKillfocusEditKonto)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_EN_KILLFOCUS(IDC_EDIT_DATE, OnKillfocusEditDate)
	ON_BN_CLICKED(IDC_CHECK_TIME, OnCheckTime)
	ON_EN_KILLFOCUS(IDC_EDIT_TIME, OnKillfocusEditTime)
	ON_EN_CHANGE(IDC_EDIT_REPEAT_TIME, OnChangeEditRepeatTime)
	ON_EN_CHANGE(IDC_EDIT_LIMITED, OnChangeEditLimited)
	ON_EN_CHANGE(IDC_EDIT_BLZ, OnChangeEditBlz)
	ON_EN_CHANGE(IDC_EDIT_KONTO, OnChangeEditKonto)
	ON_EN_CHANGE(IDC_EDIT_TIME, OnChangeEditTime)
	ON_EN_CHANGE(IDC_EDIT_DATE, OnChangeEditDate)
	ON_BN_CLICKED(IDC_CHECK_BETRAG, OnCheckBetrag)
	ON_EN_CHANGE(IDC_EDIT_BETRAG, OnChangeEditBetrag)
	ON_BN_CLICKED(IDC_CHECK_TAN, OnCheckTAN)
	ON_EN_CHANGE(IDC_EDIT_TAN, OnChangeEditTAN)
	ON_BN_CLICKED(IDC_CHECK_CURRENCY, OnCheckCurrency)
	ON_BN_CLICKED(IDC_CHECK_WSID, OnCheckWsid)
	ON_CBN_EDITCHANGE(IDC_COMBO_CURRENCY, OnEditchangeComboCurrency)
	ON_CBN_SELCHANGE(IDC_COMBO_CURRENCY, OnSelchangeComboCurrency)
	ON_EN_CHANGE(IDC_EDIT_WSID, OnChangeEditWsid)
	ON_EN_KILLFOCUS(IDC_EDIT_WSID, OnKillfocusEditWsid)
	ON_CBN_KILLFOCUS(IDC_COMBO_CURRENCY, OnKillfocusComboCurrency)
	ON_EN_KILLFOCUS(IDC_EDIT_TAN, OnKillfocusEditTan)
	ON_BN_CLICKED(IDC_CHECK_SEND_SINGLE_BYTE, OnCheckSendSingleByte)
	ON_BN_CLICKED(IDC_RADIO_RANDOM, OnRadioRepeatHow)
	ON_BN_CLICKED(IDC_RADIO_SUCCESSIVE, OnRadioRepeatHow)
	ON_EN_CHANGE(IDC_EDIT_SEND_DELAY, OnChangeEditSendDelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CStandardEntriesDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CStandardEntriesDlg::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnOK() 
{
	m_pView->PostMessage(WM_COMMAND, ID_SEND, NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCancel() 
{
	m_pView->PostMessage(WM_COMMAND, ID_STOP_SEND, NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckRepeat() 
{
	UpdateData();
	if (m_bRepeat) {
		m_editRepeatTime.SetFocus();
		m_editRepeatTime.SetSel(0,-1);
	}
	else {
		m_uRepeatTime = 2;
		GetDocument()->m_uRepeatTime = m_uRepeatTime;
		m_iRepeatHow = 0;
		GetDocument()->m_iRepeatHow = m_iRepeatHow;
		m_btnRepeat.SetFocus();
		UpdateData(FALSE);
	}
	GetDocument()->m_bRepeat = m_bRepeat;
	m_pView->ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditRepeatTime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	UpdateData();
	GetDocument()->m_uRepeatTime = m_uRepeatTime;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditRepeatTime() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnRadioRepeatHow() 
{
	UpdateData();
	GetDocument()->m_iRepeatHow = m_iRepeatHow;
	m_pView->ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckLimited() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bLimited) {
		m_editLimited.SetFocus();
		m_editLimited.SetSel(0,-1);
	}
	else {
		m_uLimited = 2;
		GetDocument()->m_uLimited = m_uLimited;
		m_btnLimited.SetFocus();
		UpdateData(FALSE);
	}
	GetDocument()->m_bLimited = m_bLimited;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditLimited() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	UpdateData();
	GetDocument()->m_uLimited = m_uLimited;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditLimited() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckBitError() 
{
	UpdateData();
	GetDocument()->m_bBitError = m_bBitError;
	m_pView->ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckSendSingleByte() 
{
	UpdateData();
	GetDocument()->m_bSendSingleBytes = m_bSendSingleBytes;
	m_pView->ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditSendDelay() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	GetDocument()->m_uSendDelay = m_uSendDelay;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckDate() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bDate) {
		m_editDate.SetFocus();
		m_editDate.SetSel(0,-1);
	}
	else {
		m_sDate.Empty();
		GetDocument()->m_sDate.Empty();
		m_btnDate.SetFocus();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditDate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	UpdateData();
	GetDocument()->m_sDate = m_sDate;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditDate()
{
	UpdateData();
	GetDocument()->IsDateOK();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckTime() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bTime) {
		m_editTime.SetFocus();
		m_editTime.SetSel(0,-1);
	}
	else {
		m_sTime.Empty();
		GetDocument()->m_sTime.Empty();
		m_btnTime.SetFocus();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditTime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	UpdateData();
	GetDocument()->m_sTime = m_sTime;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditTime() 
{
	UpdateData();
	GetDocument()->IsTimeOK();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckWsid() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bWSID) {
		m_editWSID.SetFocus();
		m_editWSID.SetSel(0,-1);
	}
	else {
		m_sWSID.Empty();
		GetDocument()->m_sWSID.Empty();
		m_btnWSID.SetFocus();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditWsid() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData();
	GetDocument()->m_sWSID = m_sWSID;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditWsid() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckTAN() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bTAN) {
		m_editTAN.SetFocus();
		m_editTAN.SetSel(0,-1);
	}
	else {
		m_sTAN.Empty();
		GetDocument()->m_sTAN.Empty();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditTAN() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSDITestView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	UpdateData();
	GetDocument()->m_sTAN = m_sTAN;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditTan() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckBlz() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bBLZ) {
		m_editBLZ.SetFocus();
		m_editBLZ.SetSel(0,-1);
	}
	else {
		m_sBLZ.Empty();
		GetDocument()->m_sBLZ.Empty();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditBlz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	UpdateData();
	GetDocument()->m_sBLZ = m_sBLZ;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditBlz() 
{
	UpdateData();
	GetDocument()->IsBlzOK();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckKonto() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bKonto) {
		m_editKonto.SetFocus();
		m_editKonto.SetSel(0,-1);
	}
	else {
		m_sKonto.Empty();
		GetDocument()->m_sKonto.Empty();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditKonto() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	UpdateData();
	GetDocument()->m_sKonto = m_sKonto;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusEditKonto() 
{
	UpdateData();
	GetDocument()->IsKontoOK();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckBetrag() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bBetrag) {
		m_editBetrag.SetFocus();
		m_editBetrag.SetSel(0,-1);
	}
	else {
		m_sBetrag.Empty();
		GetDocument()->m_sBetrag.Empty();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnChangeEditBetrag() 
{
	UpdateData();
	GetDocument()->m_sBetrag = m_sBetrag;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnCheckCurrency() 
{
	UpdateData();
	m_pView->ShowHide();
	if (m_bCurrency) {
		m_cbCurrency.SetFocus();
		m_cbCurrency.SetEditSel(0,-1);
	}
	else {
		m_sCurrency.Empty();
		GetDocument()->m_sCurrency.Empty();
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnEditchangeComboCurrency() 
{
	UpdateData();
	if (m_sCurrency.GetLength() > 3) {
		MessageBeep((UINT)-1);
		m_sCurrency = m_sCurrency.Left(3);
		m_cbCurrency.SetWindowText(m_sCurrency);
		m_cbCurrency.SetEditSel( m_sCurrency.GetLength(), -1 );
	}
	GetDocument()->m_sCurrency = m_sCurrency;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnSelchangeComboCurrency() 
{
	UpdateData();
	GetDocument()->m_sCurrency = m_sCurrency;
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::OnKillfocusComboCurrency() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CStandardEntriesDlg::PostNcDestroy() 
{
	delete this;
}
