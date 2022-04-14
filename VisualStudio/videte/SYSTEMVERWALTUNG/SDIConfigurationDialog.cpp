// SDIExtraConfigurationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIPage.h"
#include "SDIDialog.h"
#include "SpecialCharactersDialog.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationRS232.h"
#include "SVPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IBM_MAX_LENGTH	((DWORD)228)

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationDialog dialog
CSDIConfigurationDialog::CSDIConfigurationDialog(SDIControlType eType,
												 int iCom,
												 CSDIProtocolArray* pCSDIProtocolArray,
												 CSDIAlarmTypeArray* pAlarmTypeArray,
												 CComParameters* pComParameters,
												 CSVView* pView)
	: CWK_Dialog(CSDIConfigurationDialog::IDD)
{
	//{{AFX_DATA_INIT(CSDIConfigurationDialog)
	m_iProtocol = -1;
	m_sName = _T("");
	m_iProtocolFormat = SDI_PROTOCOL_FORMAT_POSITION; // YET Only implemented
	m_iReadOrder = 0;
	m_bDifferenceTotalLength = FALSE;
	m_dwDifferenceTotalLength = 0;
	m_bDifferenceStringAtPosition = FALSE;
	m_dwDifferencePosition = 1;
	m_sDifferenceString = _T("");
	m_sSample = _T("");
	m_iDataType = -1;
	m_dwPosition = 1;
	m_dwDataLength = 0;
	m_bDataLengthVariable = FALSE;
	m_sSeperator = _T("");
	m_sTag = _T("");
	m_iDataFormat = 0;
	m_sSampleRead = _T("");
	m_bSampleReadActive = FALSE;
	//}}AFX_DATA_INIT
	m_pView = pView;
	m_pPage = pView->GetSVPage();
	m_pCSDIProtocolArray = pCSDIProtocolArray;
	m_pCSDIProtocol = NULL;
	m_pCSDIProtocolData = NULL;
	m_eType = eType;
	m_iCom = iCom;
	m_iSelectedItemData = -1;
	m_iSelectedItemSample = -1;
/*
	m_hUp = LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_UP));
	m_hDown = LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_DOWN));
*/
	m_pRS232 = NULL;
	m_bSampleReadActive = FALSE;
	m_bSDIUnitStart = FALSE;
	m_pAlarmTypeArray = pAlarmTypeArray;
	m_pComParameters = pComParameters;
	m_pDatabaseFields = m_pView->GetDocument()->GetDatabaseFields();
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationDialog::~CSDIConfigurationDialog()
{
	PrepareSDIUnitForStart();
/*
	DeleteObject(m_hUp);
	DeleteObject(m_hDown);
*/
	WK_DELETE(m_pRS232);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDIConfigurationDialog)
	DDX_Control(pDX, IDC_ANIMATE_READ_SAMPLE, m_animReadSample);
	DDX_Control(pDX, IDC_LIST_SAMPLE, m_lcSample);
	DDX_Control(pDX, IDC_BUTTON_MONEY, m_btnAlarmMoney);
	DDX_Control(pDX, IDC_BUTTON_PORTRAIT, m_btnAlarmPortrait);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_btnDefault);
	DDX_Control(pDX, IDC_CHECK_READ_SAMPLE, m_checkReadSample);
	DDX_Control(pDX, IDC_BUTTON_SPECIAL_CHARACTERS_IDENTIFY_STRING, m_btnSpecialCharactersIdentifyString);
	DDX_Control(pDX, IDC_EDIT_SAMPLE_READ, m_editSampleRead);
	DDX_Control(pDX, IDC_BUTTON_SAMPLE_TAKEOVER, m_btnSampleTakeover);
	DDX_Control(pDX, IDC_BUTTON_PROTOCOL_DOWN, m_btnProtocolDown);
	DDX_Control(pDX, IDC_BUTTON_DATA_DOWN, m_btnDataDown);
	DDX_Control(pDX, IDC_LIST_PROTOCOL, m_lbProtocol);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_RADIO_POS_LEN, m_btnIdentifyByPosLen);
	DDX_Control(pDX, IDC_RADIO_SEPERATOR, m_btnIdentifyBySeperator);
	DDX_Control(pDX, IDC_RADIO_IDENTIFY_TAG, m_btnIdentifyByTag);
	DDX_Control(pDX, IDC_RADIO_ORDER_LEFT, m_btnReadOrderLeft);
	DDX_Control(pDX, IDC_RADIO_ORDER_RIGHT, m_btnReadOrderRight);
	DDX_Control(pDX, IDC_CHECK_DIFFERENCE_TOTAL_LENGTH, m_btnTotalLength);
	DDX_Control(pDX, IDC_EDIT_DIFFERENCE_TOTAL_LENGTH, m_editDifferenceTotalLength);
	DDX_Control(pDX, IDC_CHECK_DIFFERENCE_STRING_AT_POSITION, m_btnStringAtPosition);
	DDX_Control(pDX, IDC_EDIT_DIFFERENCE_POSITION_OF_STRING, m_editDifferencePositionOfString);
	DDX_Control(pDX, IDC_EDIT_DIFFERENCE_STRING_AT_POSITION, m_editDifferenceStringAtPosition);
	DDX_Control(pDX, IDC_BUTTON_PROTOCOL_NEW, m_btnProtocolNew);
	DDX_Control(pDX, IDC_BUTTON_PROTOCOL_DELETE, m_btnProtocolDelete);
	DDX_Control(pDX, IDC_BUTTON_PROTOCOL_UP, m_btnProtocolUp);
	DDX_Control(pDX, IDC_EDIT_SAMPLE_STRING, m_editSample);
	DDX_Control(pDX, IDC_COMBO_DATA_TYPE, m_cbDataType);
	DDX_Control(pDX, IDC_EDIT_POSITION, m_editPosition);
	DDX_Control(pDX, IDC_EDIT_DATA_LENGTH, m_editDataLength);
	DDX_Control(pDX, IDC_CHECK_DATA_LENGTH_VARIABLE, m_btnDataLengthVariable);
	DDX_Control(pDX, IDC_EDIT_SEPERATOR, m_editSeperator);
	DDX_Control(pDX, IDC_BUTTON_SPECIAL_CHARACTERS_SEPERATOR, m_btnSpecialCharactersSeperator);
	DDX_Control(pDX, IDC_EDIT_TAG, m_editTag);
	DDX_Control(pDX, IDC_COMBO_DATA_FORMAT, m_cbDataFormat);
	DDX_Control(pDX, IDC_BUTTON_DATA_NEW, m_btnDataNew);
	DDX_Control(pDX, IDC_BUTTON_DATA_DELETE, m_btnDataDelete);
	DDX_Control(pDX, IDC_BUTTON_DATA_UP, m_btnDataUp);
	DDX_Control(pDX, IDC_LIST_DATA, m_lcData);
	DDX_Check(pDX, IDC_CHECK_READ_SAMPLE, m_bSampleReadActive);
	DDX_Text(pDX, IDC_EDIT_SAMPLE_READ, m_sSampleRead);
	DDX_LBIndex(pDX, IDC_LIST_PROTOCOL, m_iProtocol);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDX_Radio(pDX, IDC_RADIO_POS_LEN, m_iProtocolFormat);
	DDX_Radio(pDX, IDC_RADIO_ORDER_LEFT, m_iReadOrder);
	DDX_Check(pDX, IDC_CHECK_DIFFERENCE_TOTAL_LENGTH, m_bDifferenceTotalLength);
	DDX_Text(pDX, IDC_EDIT_DIFFERENCE_TOTAL_LENGTH, m_dwDifferenceTotalLength);
	DDX_Check(pDX, IDC_CHECK_DIFFERENCE_STRING_AT_POSITION, m_bDifferenceStringAtPosition);
	DDX_Text(pDX, IDC_EDIT_DIFFERENCE_POSITION_OF_STRING, m_dwDifferencePosition);
	DDV_MinMaxDWord(pDX, m_dwDifferencePosition, 1, 4294967295);
	DDX_Text(pDX, IDC_EDIT_DIFFERENCE_STRING_AT_POSITION, m_sDifferenceString);
	DDX_Text(pDX, IDC_EDIT_SAMPLE_STRING, m_sSample);
	DDX_CBIndex(pDX, IDC_COMBO_DATA_TYPE, m_iDataType);
	DDX_Text(pDX, IDC_EDIT_POSITION, m_dwPosition);
	DDV_MinMaxDWord(pDX, m_dwPosition, 1, 4294967295);
	DDX_Text(pDX, IDC_EDIT_DATA_LENGTH, m_dwDataLength);
	DDV_MinMaxDWord(pDX, m_dwDataLength, 0, 4294967295);
	DDX_Check(pDX, IDC_CHECK_DATA_LENGTH_VARIABLE, m_bDataLengthVariable);
	DDX_Text(pDX, IDC_EDIT_SEPERATOR, m_sSeperator);
	DDX_Text(pDX, IDC_EDIT_TAG, m_sTag);
	DDX_CBIndex(pDX, IDC_COMBO_DATA_FORMAT, m_iDataFormat);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIConfigurationDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CSDIConfigurationDialog)
	ON_LBN_SELCHANGE(IDC_LIST_PROTOCOL, OnSelchangeListProtocol)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	ON_BN_CLICKED(IDC_RADIO_POS_LEN, OnRadioProtocolFormat)
	ON_BN_CLICKED(IDC_RADIO_ORDER_LEFT, OnRadioReadOrder)
	ON_BN_CLICKED(IDC_CHECK_DIFFERENCE_TOTAL_LENGTH, OnCheckDifferenceTotalLength)
	ON_EN_CHANGE(IDC_EDIT_DIFFERENCE_TOTAL_LENGTH, OnChangeEditDifferenceTotalLength)
	ON_BN_CLICKED(IDC_CHECK_DIFFERENCE_STRING_AT_POSITION, OnCheckDifferenceStringAtPosition)
	ON_EN_CHANGE(IDC_EDIT_DIFFERENCE_POSITION_OF_STRING, OnChangeEditDifferencePositionOfString)
	ON_EN_CHANGE(IDC_EDIT_DIFFERENCE_STRING_AT_POSITION, OnChangeEditDifferenceStringAtPosition)
	ON_BN_CLICKED(IDC_BUTTON_PROTOCOL_NEW, OnButtonProtocolNew)
	ON_BN_CLICKED(IDC_BUTTON_PROTOCOL_DELETE, OnButtonProtocolDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedListData)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA_TYPE, OnSelchangeComboDataType)
	ON_EN_CHANGE(IDC_EDIT_POSITION, OnChangeEditPosition)
	ON_EN_CHANGE(IDC_EDIT_DATA_LENGTH, OnChangeEditDataLength)
	ON_BN_CLICKED(IDC_CHECK_DATA_LENGTH_VARIABLE, OnCheckDataLengthVariable)
	ON_EN_CHANGE(IDC_EDIT_SEPERATOR, OnChangeEditSeperator)
	ON_EN_CHANGE(IDC_EDIT_TAG, OnChangeEditTag)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA_FORMAT, OnSelchangeComboDataFormat)
	ON_BN_CLICKED(IDC_BUTTON_DATA_NEW, OnButtonDataNew)
	ON_BN_CLICKED(IDC_BUTTON_DATA_DELETE, OnButtonDataDelete)
	ON_BN_CLICKED(IDC_BUTTON_DATA_UP, OnButtonDataUp)
	ON_BN_CLICKED(IDC_BUTTON_DATA_DOWN, OnButtonDataDown)
	ON_BN_CLICKED(IDC_BUTTON_PROTOCOL_UP, OnButtonProtocolUp)
	ON_BN_CLICKED(IDC_BUTTON_PROTOCOL_DOWN, OnButtonProtocolDown)
	ON_BN_CLICKED(IDC_BUTTON_SAMPLE_TAKEOVER, OnButtonSampleTakeover)
	ON_BN_CLICKED(IDC_BUTTON_SPECIAL_CHARACTERS_IDENTIFY_STRING, OnButtonSpecialCharactersIdentifyString)
	ON_BN_CLICKED(IDC_BUTTON_SPECIAL_CHARACTERS_SEPERATOR, OnButtonSpecialCharactersSeperator)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_BN_CLICKED(IDC_CHECK_READ_SAMPLE, OnCheckReadSample)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SAMPLE, OnItemchangedListSample)
	ON_BN_CLICKED(IDC_BUTTON_PORTRAIT, OnButtonPortrait)
	ON_BN_CLICKED(IDC_BUTTON_MONEY, OnButtonMoney)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_DATA, OnClickListData)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DATA, OnDblclkListData)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DATA, OnRclickListData)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_DATA, OnRdblclkListData)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SAMPLE, OnClickListSample)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SAMPLE, OnDblclkListSample)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SAMPLE, OnRclickListSample)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_SAMPLE, OnRdblclkListSample)
	ON_BN_CLICKED(IDC_RADIO_SEPERATOR, OnRadioProtocolFormat)
	ON_BN_CLICKED(IDC_RADIO_IDENTIFY_TAG, OnRadioProtocolFormat)
	ON_BN_CLICKED(IDC_RADIO_ORDER_RIGHT, OnRadioReadOrder)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SDI_SAMPLE_READ_IN, OnReadInData)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::SetDefaultValues()
{
	// May be overwritten by derived classes
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::SetProtocolDefaultValues()
{
	// May be overwritten by derived classes
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::SetPortraitAlarm()
{
	// May be overwritten by derived classes
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::SetMoneyAlarm()
{
	// May be overwritten by derived classes
}
/////////////////////////////////////////////////////////////////////////////
const CString CSDIConfigurationDialog::GetUIStringOfDataTypeID(const CSecID idType)
{
	CString sText, sReturn;
	CIPCField* pField = NULL;
	for (int i=0 ; i<m_cbDataType.GetCount() ; i++)
	{
		m_cbDataType.GetLBText(i, sText);
		pField = (CIPCField*)m_cbDataType.GetItemDataPtr(i);
		DWORD dwID = GetParamTypeFromDatabaseField(pField);
		if (dwID == idType.GetID())
		{
			sReturn = sText;
			break;
		}
	}
/*	switch ( idParam.GetID() ) {
		case PARAM_ACCOUNT:
			sResource.LoadString(IDS_KTO_NR);
			break;
		case PARAM_BANKCODE:
			sResource.LoadString(IDS_BANKLEITZAHL);
			break;
		case PARAM_VALUE:
			sResource.LoadString(IDS_VALUE);
			break;
		case PARAM_CURRENCY:
			sResource.LoadString(IDS_CURRENCY);
			break;
		case PARAM_WORKSTATION:
			sResource.LoadString(IDS_GA_NR);
			break;
		case PARAM_TRANSAKTION:
			sResource.LoadString(IDS_TA_NR);
			break;
		case PARAM_DATE:
			sResource.LoadString(IDS_DTP_DATE);
			break;
		case PARAM_TIME:
			sResource.LoadString(IDS_DTP_TIME);
			break;
		case PARAM_UNKNOWN:
		default:
			sResource.LoadString(IDS_TO_BE_IGNORED);
			break;
	}
*/
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
const CString CSDIConfigurationDialog::GetUIStringOfDataFormat(eSDIDataFormat eFormat)
{
	CString sResource;
	switch (eFormat) {
		case SDI_DATA_FORMAT_DDMMYY:
			sResource.LoadString(IDS_DATE_DDMMYY);
			break;
		case SDI_DATA_FORMAT_DD_MM_YY:
			sResource.LoadString(IDS_DATE_DD_MM_YY);
			break;
		case SDI_DATA_FORMAT_DDMMYYYY:
			sResource.LoadString(IDS_DATE_DDMMYYYY);
			break;
		case SDI_DATA_FORMAT_DD_MM_YYYY:
			sResource.LoadString(IDS_DATE_DD_MM_YYYY);
			break;
		case SDI_DATA_FORMAT_MMDDYY:
			sResource.LoadString(IDS_DATE_MMDDYY);
			break;
		case SDI_DATA_FORMAT_MM_DD_YY:
			sResource.LoadString(IDS_DATE_MM_DD_YY);
			break;
		case SDI_DATA_FORMAT_MMDDYYYY:
			sResource.LoadString(IDS_DATE_MMDDYYYY);
			break;
		case SDI_DATA_FORMAT_MM_DD_YYYY:
			sResource.LoadString(IDS_DATE_MM_DD_YYYY);
			break;
		case SDI_DATA_FORMAT_YYMMDD:
			sResource.LoadString(IDS_DATE_YYMMDD);
			break;
		case SDI_DATA_FORMAT_YY_MM_DD:
			sResource.LoadString(IDS_DATE_YY_MM_DD);
			break;
		case SDI_DATA_FORMAT_YYYYMMDD:
			sResource.LoadString(IDS_DATE_YYYYMMDD);
			break;
		case SDI_DATA_FORMAT_YYYY_MM_DD:
			sResource.LoadString(IDS_DATE_YYYY_MM_DD);
			break;
		case SDI_DATA_FORMAT_DD_MMM_YY:
			sResource.LoadString(IDS_DATE_DD_MMM_YY);
			break;
		case SDI_DATA_FORMAT_DD_MMM_YYYY:
			sResource.LoadString(IDS_DATE_DD_MMM_YYYY);
			break;
		case SDI_DATA_FORMAT_HHMM:
			sResource.LoadString(IDS_TIME_HHMM);
			break;
		case SDI_DATA_FORMAT_HH_MM:
			sResource.LoadString(IDS_TIME_HH_MM);
			break;
		case SDI_DATA_FORMAT_HHMMSS:
			sResource.LoadString(IDS_TIME_HHMMSS);
			break;
		case SDI_DATA_FORMAT_HH_MM_SS:
			sResource.LoadString(IDS_TIME_HH_MM_SS);
			break;
		case SDI_DATA_FORMAT_IGNORE:
			sResource = _T("");
			break;
		default:
			sResource = _T("ERROR NOT FOUND");
			break;
	}
	return sResource;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::InitDlgControls()
{
	// BitMap Buttons initialisieren
	InitBitmapButtons();
	// ListCtrl Data initialisieren
	InitListControlData();
	// ListCtrl Sample Data initialisieren
	InitListControlSampleData();
	// Combobox DataType fuellen
	FillComboboxDataType();
	// Combobox Seperator fuellen
//	FillComboboxSeperator();
	// Combobox DataFormat fuellen
//	FillComboboxDataFormat();

	// Protokolle einlesen
	// erst mal alle Eintraege loeschen
	m_lbProtocol.ResetContent();
	int iCount = m_pCSDIProtocolArray->GetSize();
	for (int i=0 ; i<iCount ; i++) {
		m_pCSDIProtocol = m_pCSDIProtocolArray->GetAt(i);
		m_lbProtocol.AddString( m_pCSDIProtocol->GetName() );
	}

	if (0<iCount) {
		m_iProtocol = m_lbProtocol.SetCurSel(0);
	}
	else {
		m_iProtocol = m_lbProtocol.SetCurSel(-1);
	}
	LoadFromSDIProtocol();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::InitBitmapButtons()
{
	HICON hUp   = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_UP),     IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);
	HICON hDown = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DOWN),   IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);
	m_btnProtocolUp.SetIcon(hUp);
	m_btnProtocolDown.SetIcon(hDown);
	m_btnDataUp.SetIcon(hUp);
	m_btnDataDown.SetIcon(hDown);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::InitListControlData()
{
	const int		iColumns = 6;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[iColumns];
	int i = 0;
	int w[] = {29,16,16,25,20,24};
	int iwTotal = 130;	// muss die Summe der einzel Weiten sein!

	// ListCtrl initialisieren
	// Erstmal alle Eintraege loeschen
	m_lcData.DeleteAllItems();
	m_lcData.GetClientRect(&rect);
	s[0].LoadString(IDS_SDI_DATA_TYPE);
	s[1].LoadString(IDS_SDI_DATA_POSITION);
	s[2].LoadString(IDS_SDI_DATA_LENGTH);
	s[3].LoadString(IDS_SDI_DATA_SEPERATOR);
	s[4].LoadString(IDS_SDI_DATA_TAG);
	s[5].LoadString(IDS_SDI_DATA_FORMAT);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	int icx = 0;
	int iTotalcx = 0;
	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		m_lcData.DeleteColumn(i);  // assumes return value is OK.
		icx = (rect.Width() * w[i])/iwTotal;
		lvcolumn.cx = icx;
		// Die letzte Spalte bekommt den Rest 
		if (i == iColumns-1) {
			lvcolumn.cx = rect.Width() - iTotalcx;
		}
		iTotalcx += icx;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		m_lcData.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}
	LONG dw = ListView_GetExtendedListViewStyle(m_lcData.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_lcData.m_hWnd,dw);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::FillComboboxDataType()
{
	// erst mal alle Eintraege loeschen
	m_cbDataType.ResetContent();
	int iIndex = -1;
	CString sText;
	if (SDI_PROTOCOL_FORMAT_SEPERATOR == m_iProtocolFormat)
	{
		// Datentyp unbekannt, wird ignoriert
		sText.LoadString(IDS_TO_BE_IGNORED);
		iIndex = m_cbDataType.AddString(sText);
		m_cbDataType.SetItemDataPtr(iIndex, NULL);
	}
	CIPCField* pField = NULL;
	for (int i=0 ; i<m_pDatabaseFields->GetSize() ; i++)
	{
		pField = m_pDatabaseFields->GetAt(i);
		sText = pField->GetValue();
		if (sText != szDontUse)
		{
			iIndex = m_cbDataType.AddString(sText);
			m_cbDataType.SetItemDataPtr(iIndex, pField);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
DWORD CSDIConfigurationDialog::GetParamTypeFromDatabaseField(CIPCField* pField)
{
	DWORD dwReturn = PARAM_UNKNOWN;
	if (pField)
	{
		CString sName = pField->GetName();
		if      (sName == CIPCField::m_sfDate) dwReturn = PARAM_DATE;
		else if (sName == CIPCField::m_sfTime) dwReturn = PARAM_TIME;
		else if (sName == CIPCField::m_sfGaNr) dwReturn = PARAM_WORKSTATION;
		else if (sName == CIPCField::m_sfTaNr) dwReturn = PARAM_TRANSAKTION;
		else if (sName == CIPCField::m_sfKtNr) dwReturn = PARAM_ACCOUNT;
		else if (sName == CIPCField::m_sfBcNr) dwReturn = PARAM_BANKCODE;
		else if (sName == CIPCField::m_sfAmnt) dwReturn = PARAM_VALUE;
		else if (sName == CIPCField::m_sfCurr) dwReturn = PARAM_CURRENCY;
		else
		{
	//		int i = _tscanf(_T("DBD_%04d"), sName, &dwID);
			int iID = _ttoi(sName.Mid(4));
			if (iID > 0)
			{
				dwReturn = MAKELONG(iID, SECID_PARAMS_NEW);
			}
			else
			{
				WK_TRACE_ERROR(_T("SDIConfiguration Unknown Data Type %s\n"), sName);
			}
		}
	}
	return dwReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::FillComboboxDataFormat()
{
	// erst mal alle Eintraege loeschen
	m_cbDataFormat.ResetContent();
	m_iDataFormat = -1;
	int iIndex = -1;

	CIPCField* pField = NULL;
	if (m_iDataType != -1)
	{
		pField = (CIPCField*)m_cbDataType.GetItemDataPtr(m_iDataType);
	}
	if ((pField && pField->IsDate()))
	{
		// SDI_DATA_FORMAT_DDMMYY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_DDMMYY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_DDMMYY);
		// SDI_DATA_FORMAT_DD_MM_YY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_DD_MM_YY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_DD_MM_YY);
		// SDI_DATA_FORMAT_DDMMYYYY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_DDMMYYYY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_DDMMYYYY);
		// SDI_DATA_FORMAT_DD_MM_YYYY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_DD_MM_YYYY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_DD_MM_YYYY);
		iIndex = m_cbDataFormat.GetCount();
		// SDI_DATA_FORMAT_MMDDYY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_MMDDYY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_MMDDYY);
		// SDI_DATA_FORMAT_MM_DD_YY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_MM_DD_YY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_MM_DD_YY);
		// SDI_DATA_FORMAT_MMDDYYYY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_MMDDYYYY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_MMDDYYYY);
		// SDI_DATA_FORMAT_MM_DD_YYYY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_MM_DD_YYYY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_MM_DD_YYYY);
		iIndex = m_cbDataFormat.GetCount();
		// SDI_DATA_FORMAT_YYMMDD
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_YYMMDD));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_YYMMDD);
		// SDI_DATA_FORMAT_YY_MM_DD
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_YY_MM_DD));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_YY_MM_DD);
		// SDI_DATA_FORMAT_YYYYMMDD
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_YYYYMMDD));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_YYYYMMDD);
		// SDI_DATA_FORMAT_YYYY_MM_DD
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_YYYY_MM_DD));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_YYYY_MM_DD);
		// SDI_DATA_FORMAT_DD_MMM_YY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_DD_MMM_YY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_DD_MMM_YY);
		// SDI_DATA_FORMAT_DD_MMM_YYYY
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_DD_MMM_YYYY));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_DD_MMM_YYYY);
		if ( 0 < iIndex ) {
			m_iDataFormat = m_cbDataFormat.SetCurSel( min(0, iIndex) );
		}
	}
	else if (pField && pField->IsTime())
	{
		// SDI_DATA_FORMAT_HHMM
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_HHMM));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_HHMM);
		// SDI_DATA_FORMAT_HH_MM
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_HH_MM));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_HH_MM);
		// SDI_DATA_FORMAT_HHMMSS
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_HHMMSS));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_HHMMSS);
		// SDI_DATA_FORMAT_HH_MM_SS
		iIndex = m_cbDataFormat.AddString(GetUIStringOfDataFormat(SDI_DATA_FORMAT_HH_MM_SS));
		m_cbDataFormat.SetItemData(iIndex, SDI_DATA_FORMAT_HH_MM_SS);
		iIndex = m_cbDataFormat.GetCount();
		if ( 0 < iIndex ) {
			m_iDataFormat = m_cbDataFormat.SetCurSel( min(2, iIndex) );
		}
	}
	else {
		// Hier gibt's keine Eintraege, Box wird sowieso disabled
		m_iDataFormat = -1;
		if (m_pCSDIProtocolData) {
			m_pCSDIProtocolData->SetFormat(SDI_DATA_FORMAT_IGNORE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::EnableControlsBase()
{
	BOOL bEnableProtocol = FALSE;
	BOOL bEnableData = FALSE;
	BOOL bExtraEnable = FALSE;
	if (-1 != m_lbProtocol.GetCurSel() ) {
		bEnableProtocol = TRUE;
		if (0 != m_lcData.GetItemCount() ) {
			bEnableData = TRUE;
		}
	}
	// Protokoll-Elemente
	m_btnProtocolNew.EnableWindow(CanNewProtocol());
	m_btnProtocolDelete.EnableWindow(bEnableProtocol);
	m_lbProtocol.EnableWindow(bEnableProtocol);
// Nicht disablen, sonst sieht es bescheiden aus
//	bExtraEnable = bEnableProtocol && ( 0 < m_lbProtocol.GetCurSel() );
//	m_btnProtocolUp.EnableWindow(bExtraEnable);
//	bExtraEnable = bEnableProtocol && ( m_lbProtocol.GetCurSel() < m_lbProtocol.GetCount()-1 );
//	m_btnProtocolDown.EnableWindow(bExtraEnable);
	m_editName.EnableWindow(bEnableProtocol);
	m_btnIdentifyByPosLen.EnableWindow(bEnableProtocol);
	m_btnIdentifyBySeperator.EnableWindow(FALSE); // Not yet implemented
	m_btnIdentifyByTag.EnableWindow(FALSE); // Not yet implemented
	m_btnReadOrderLeft.EnableWindow(bEnableProtocol);
	m_btnReadOrderRight.EnableWindow(bEnableProtocol);
	m_btnTotalLength.EnableWindow(bEnableProtocol);
	bExtraEnable = bEnableProtocol && m_bDifferenceTotalLength;
	m_editDifferenceTotalLength.EnableWindow(bExtraEnable);
	m_btnStringAtPosition.EnableWindow(bEnableProtocol);
	bExtraEnable = bEnableProtocol && m_bDifferenceStringAtPosition;
	m_editDifferenceStringAtPosition.EnableWindow(bExtraEnable);
	m_editDifferencePositionOfString.EnableWindow(bExtraEnable);
	m_btnSpecialCharactersIdentifyString.EnableWindow(bExtraEnable);

	// Data-Elemente
	m_btnDataDelete.EnableWindow(bEnableData);
	m_btnDataNew.EnableWindow(bEnableProtocol); // haengt von Protokoll ab
	m_lcData.EnableWindow(bEnableData);
// Nicht disablen, sonst sieht es bescheiden aus
//	bExtraEnable = bEnableData && (0 < m_iSelectedItemData);
//	m_btnDataUp.EnableWindow(bExtraEnable);
//	bExtraEnable = bEnableData && ( m_iSelectedItemData!=-1
//									&& m_iSelectedItemData != m_pCSDIProtocol->GetDataArray().GetUpperBound() );
//	m_btnDataDown.EnableWindow(bExtraEnable);
	m_editSample.EnableWindow(bEnableData);
	m_cbDataType.EnableWindow(bEnableData);
	bExtraEnable = bEnableData && (SDI_PROTOCOL_FORMAT_POSITION==m_iProtocolFormat);
	m_editPosition.EnableWindow(bExtraEnable);

	CIPCField* pField = NULL;
	if (m_iDataType != -1)
	{
		pField = (CIPCField*)m_cbDataType.GetItemDataPtr(m_iDataType);
	}
	BOOL bIsDateOrTime = (pField &&(pField->IsDate() || pField->IsTime()));

	bExtraEnable = bEnableData && (SDI_PROTOCOL_FORMAT_POSITION==m_iProtocolFormat) && !bIsDateOrTime;
	
	m_editDataLength.EnableWindow(bExtraEnable && !m_bDataLengthVariable);
	m_btnDataLengthVariable.EnableWindow(bExtraEnable);
	bExtraEnable = bEnableData && ( ( (SDI_PROTOCOL_FORMAT_POSITION==m_iProtocolFormat)
									 && m_bDataLengthVariable
								   || (SDI_PROTOCOL_FORMAT_SEPERATOR==m_iProtocolFormat)
								   || (SDI_PROTOCOL_FORMAT_TAG==m_iProtocolFormat) ) );
	m_editSeperator.EnableWindow(bExtraEnable);
	m_btnSpecialCharactersSeperator.EnableWindow(bExtraEnable);
	m_editTag.EnableWindow(bEnableData && (SDI_PROTOCOL_FORMAT_TAG==m_iProtocolFormat));
	bExtraEnable = bEnableData && bIsDateOrTime;
	m_cbDataFormat.EnableWindow(bExtraEnable);

	// SampleDataRead
	bExtraEnable = bEnableProtocol && !m_bSampleReadActive && m_pPage->IsLocal();
	m_lcSample.EnableWindow(bExtraEnable);
	m_editSampleRead.EnableWindow(bExtraEnable);
	m_checkReadSample.EnableWindow(bEnableProtocol && m_pPage->IsLocal());
	int iStartChar, iEndChar;
	m_editSampleRead.GetSel(iStartChar, iEndChar);
	bExtraEnable = bEnableData && !m_bSampleReadActive && m_pPage->IsLocal() && (iEndChar-iStartChar>0);
	m_btnSampleTakeover.EnableWindow(bExtraEnable);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigurationDialog::IsProtocolDataValid(BOOL bOnNew/*=FALSE*/) 
{
	BOOL bValid = TRUE;
	if (bOnNew) {
		bValid = IsProtocolDataValidForMultiple();
	}
	else if ( 1 < m_pCSDIProtocolArray->GetSize() ) {
		bValid = IsProtocolDataValidForMultiple();
	}
	else {
		bValid = IsProtocolDataValidForSingle();
	}
	return bValid;	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigurationDialog::IsProtocolDataValidForMultiple() 
{
	BOOL bValid = TRUE;
	CString sMsg;
	if (m_pCSDIProtocol) {
		// Weder Gesamtlaenge noch StringAtPosition
		if ( !m_bDifferenceTotalLength && !m_bDifferenceStringAtPosition) {
			bValid = FALSE;
			AfxMessageBox(IDP_SDI_NO_DIFFERENCES, MB_ICONSTOP);
			m_btnTotalLength.SetFocus();
		}
		// Wenn Gesamtlaenge, aber keine gueltige Laenge
		else if ( m_bDifferenceTotalLength
				  && ( 0==m_dwDifferenceTotalLength
					   || IBM_MAX_LENGTH < m_dwDifferenceTotalLength) ) {
			bValid = FALSE;
			sMsg.Format(IDP_SDI_WRONG_TOTAL_LENGTH, IBM_MAX_LENGTH);
			AfxMessageBox(sMsg, MB_ICONSTOP);
			m_editDifferenceTotalLength.SetFocus();
		}
		else if ( m_bDifferenceStringAtPosition && m_sDifferenceString.IsEmpty() ) {
			bValid = FALSE;
			AfxMessageBox(IDP_SDI_NO_DIFFERENCE_STRING, MB_ICONSTOP);
			m_editDifferenceStringAtPosition.SetFocus();
		}
		else if (m_bDifferenceStringAtPosition && 0 == m_dwDifferencePosition) {
			bValid = FALSE;
			AfxMessageBox(IDP_SDI_WRONG_POSITION, MB_ICONSTOP);
			m_editDifferencePositionOfString.SetFocus();
		}
		else {
			CSDIProtocol* pProtocol = NULL;
			for (int i=0 ; i<m_pCSDIProtocolArray->GetSize() ; i++) {
				pProtocol = m_pCSDIProtocolArray->GetAt(i);
				if (m_pCSDIProtocol != pProtocol) {
					if ( m_bDifferenceTotalLength
						 && !m_bDifferenceStringAtPosition
						 && m_dwDifferenceTotalLength == pProtocol->GetTotalLength() )
					{
						sMsg.Format(IDP_SDI_SAME_TOTAL_LENGTH,
									m_pCSDIProtocol->GetName(),
									pProtocol->GetName() );
						if (IDCANCEL == AfxMessageBox(sMsg, MB_ICONSTOP|MB_OKCANCEL) )
						{
							bValid = FALSE;
							m_editDifferenceTotalLength.SetFocus();
							break;
						}
					}
					else if ( !m_bDifferenceTotalLength
						      && m_bDifferenceStringAtPosition
							  && m_dwDifferencePosition == pProtocol->GetPositionIdentifyString()
							  && m_sDifferenceString == pProtocol->GetIdentifyString() )
					{
						sMsg.Format(IDP_SDI_SAME_IDENTIFY_STRING,
									m_pCSDIProtocol->GetName(),
									pProtocol->GetName() );
						if (IDCANCEL == AfxMessageBox(sMsg, MB_ICONSTOP|MB_OKCANCEL) )
						{
							bValid = FALSE;
							m_editDifferenceStringAtPosition.SetFocus();
							break;
						}
					}
					else if ( m_bDifferenceTotalLength
						      && m_bDifferenceStringAtPosition
							  && m_dwDifferenceTotalLength == pProtocol->GetTotalLength()
							  && m_dwDifferencePosition == pProtocol->GetPositionIdentifyString()
							  && m_sDifferenceString == pProtocol->GetIdentifyString() )
					{
						sMsg.Format(IDP_SDI_SAME_TOTAL_LENGTH_AND_IDENTIFY_STRING,
									m_pCSDIProtocol->GetName(),
									pProtocol->GetName() );
						if (IDCANCEL == AfxMessageBox(sMsg, MB_ICONSTOP|MB_OKCANCEL) )
						{
							bValid = FALSE;
							m_editDifferenceTotalLength.SetFocus();
							break;
						}
					}
				}
			}
		}

		if (bValid) {
			bValid = IsProtocolDataValidForSingle();
		}
	}
	return bValid;	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigurationDialog::IsProtocolDataValidForSingle() 
{
	BOOL bValid = TRUE;
	if (m_pCSDIProtocol) {
		// Alle Dateneintraege gegeneinander abchecken
		const CSDIProtocolDataArray& rProtocolDataArray = m_pCSDIProtocol->GetDataArray();
		CSDIProtocolData* pData1 = NULL;
		CSDIProtocolData* pData2 = NULL;
		DWORD dwPos1 = 0;
		DWORD dwPos2 = 0;
		DWORD dwLen1 = 0;
		DWORD dwLen2 = 0;
		// Gesamtlaenge des IBM Protokolls, maximal IBM_MAX_LENGTH
		// OOPS todo gf, was ist bei anderen
		const DWORD dwTotalLength = m_pCSDIProtocol->GetTotalLength();
		const DWORD dwMaxLength = (0==dwTotalLength) ? IBM_MAX_LENGTH : dwTotalLength;
		// Nimm erst einen Dateneintrag...
		for (int i=0 ; bValid && i<rProtocolDataArray.GetSize() ; i++)
		{
			pData1 = rProtocolDataArray.GetAt(i);
			dwPos1 = pData1->GetPosition();
			dwLen1 = pData1->GetLength();
			// Nur beim Protokollformat Position und Laenge?
			if ( SDI_PROTOCOL_FORMAT_POSITION == m_pCSDIProtocol->GetProtocolFormat() )
			{
				// Ueberschreitet dieser Eintrag die Gesamtlaenge
				// Achtung Position beginnt bei 1, daher +1
				if (dwMaxLength + 1 < dwPos1 + dwLen1) {
					bValid = FALSE;
					CString sMsg;
					sMsg.Format(IDP_SDI_DATA_TOO_LONG, dwPos1);
					AfxMessageBox(sMsg, MB_ICONSTOP);
					SelectData(i, TRUE);
				}
				else {
					m_pCSDIProtocolData = pData1;
					bValid = IsDataValid(i);
					// Messagebox, selection and focus already set, 
				}
			}
			// Nur weitermachen, wenn noch valid
			if (bValid) {
				// ...und vergleiche ihn mit allen folgenden
				for (int j=i+1 ; bValid && j<rProtocolDataArray.GetSize() ; j++)
				{
					pData2 = rProtocolDataArray.GetAt(j);
					dwPos2 = pData2->GetPosition();
					dwLen2 = pData2->GetLength();
					// Sind Datentypen doppelt vergeben?
					if ( pData1->GetID() == pData2->GetID() )
					{
						bValid = FALSE;
						CString sMsg;
						sMsg.Format(IDP_SDI_DATA_TYPE_DOUBLE,
									GetUIStringOfDataTypeID( pData1->GetID() ));
						AfxMessageBox(sMsg, MB_ICONSTOP);
						SelectData(j, TRUE);
					}
					// Nur beim Protokollformat Position und Laenge?
					else if ( SDI_PROTOCOL_FORMAT_POSITION == m_pCSDIProtocol->GetProtocolFormat() ) {
						// Ueberschneiden sich Eintraege 
						if (dwPos1 < dwPos2) {
							// Variable Laenge = -1 kann nicht abgecheckt werden
							if ( -1 != dwLen1
								&& dwPos1 + dwLen1 > dwPos2 )
							{
								bValid = FALSE;
								CString sMsg;
								sMsg.Format(IDP_SDI_DATA_OVERLAPPING, dwPos1, dwPos2 );
								AfxMessageBox(sMsg, MB_ICONSTOP);
								SelectData(j, TRUE);
							}
						}
						else {
							if ( -1 != dwLen2
								 && dwPos2 + dwLen2 > dwPos1 )
							{
								bValid = FALSE;
								CString sMsg;
								sMsg.Format(IDP_SDI_DATA_OVERLAPPING, dwPos2, dwPos1 );
								AfxMessageBox(sMsg, MB_ICONSTOP);
								SelectData(j, TRUE);
							}
						}
					}
				}
			}
		}
	}
	return bValid;	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigurationDialog::IsDataValid(int iActualData)
{
	if (m_pCSDIProtocolData)
	{
		CIPCField* pField = NULL;
		if (m_iDataType != -1)
		{
			pField = (CIPCField*)m_cbDataType.GetItemDataPtr(m_iDataType);
		}
		else
		{
			AfxMessageBox(IDP_SDI_NO_DATA_TYPE, MB_ICONSTOP);
			SelectData(iActualData, TRUE);
			m_cbDataType.SetFocus();
			return FALSE;
		}

		if (SDI_PROTOCOL_FORMAT_POSITION==m_iProtocolFormat)
		{
			if (0 == m_dwPosition)
			{
				AfxMessageBox(IDP_SDI_WRONG_POSITION, MB_ICONSTOP);
				SelectData(iActualData, TRUE);
				m_editPosition.SetFocus();
				return FALSE;
			}
			else if (m_bDataLengthVariable)
			{
				// dann keine Laengenueberpruefung
				if (m_sSeperator.IsEmpty())
				{
					AfxMessageBox(IDP_SDI_NO_SEPERATOR, MB_ICONSTOP);
					SelectData(iActualData, TRUE);
					m_editSeperator.SetFocus();
					return FALSE;
				}
			}
			else if ( !IsDataLengthValid(iActualData) )
			{
				return FALSE;
				// Messagebox and focus already set
			}
		}
		else if (SDI_PROTOCOL_FORMAT_SEPERATOR==m_iProtocolFormat)
		{
			if ( m_sSeperator.IsEmpty() )
			{
				AfxMessageBox(IDP_SDI_NO_SEPERATOR, MB_ICONSTOP);
				SelectData(iActualData, TRUE);
				m_editSeperator.SetFocus();
				return FALSE;
			}
		}
		else if (SDI_PROTOCOL_FORMAT_TAG==m_iProtocolFormat)
		{
			if ( m_sSeperator.IsEmpty() )
			{
				AfxMessageBox(IDP_SDI_NO_SEPERATOR, MB_ICONSTOP);
				SelectData(iActualData, TRUE);
				m_editSeperator.SetFocus();
				return FALSE;
			}
			else if ( m_sTag.IsEmpty() )
			{
				AfxMessageBox(IDP_SDI_NO_TAG, MB_ICONSTOP);
				SelectData(iActualData, TRUE);
				m_editTag.SetFocus();
				return FALSE;
			}
		}
		if (pField && (pField->IsDate() || pField->IsTime()))
		{
			if (-1==m_iDataFormat)
			{
				AfxMessageBox(IDP_SDI_NO_DATA_FORMAT, MB_ICONSTOP);
				SelectData(iActualData, TRUE);
				m_cbDataFormat.SetFocus();
				return FALSE;
			}
		}
	}
	return TRUE;	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigurationDialog::IsDataLengthValid(int iActualData) 
{
	BOOL bValid = TRUE;
	BOOL bNull = FALSE;
	BOOL bToLong = FALSE;
	if (m_pCSDIProtocolData) {
		DWORD dwMaxLen = 0;
		DWORD dwLen = m_pCSDIProtocolData->GetLength();
		if (dwLen == 0) {
			bNull = TRUE;
		}
		else {
			switch (m_pCSDIProtocolData->GetID().GetID()) {
				case PARAM_ACCOUNT:
					dwMaxLen = 34;
					if (dwLen > dwMaxLen) {
						bToLong = TRUE;
					}
					break;
				case PARAM_BANKCODE:
					dwMaxLen = 8;
					if (dwLen > dwMaxLen) {
						bToLong = TRUE;
					}
					break;
				case PARAM_VALUE:
					dwMaxLen = 11;
					if (dwLen > dwMaxLen) {
						bToLong = TRUE;
					}
					break;
				case PARAM_CURRENCY:
					dwMaxLen = 3;
					if (dwLen > dwMaxLen) {
						bToLong = TRUE;
					}
					break;
				case PARAM_WORKSTATION:
					dwMaxLen = 6;
					if (dwLen > dwMaxLen) {
						bToLong = TRUE;
					}
					break;
				case PARAM_TRANSAKTION:
					dwMaxLen = 4;
					if (dwLen > dwMaxLen) {
						bToLong = TRUE;
					}
					break;
			}
		}
		if (bNull) {
			bValid = FALSE;
			AfxMessageBox(IDP_SDI_LENGTH_NULL, MB_ICONSTOP);
		}
		else if (bToLong) {
			bValid = FALSE;
			CString sMsg;
			sMsg.Format(IDP_SDI_LENGTH_TO_LONG, dwMaxLen);
			AfxMessageBox(sMsg, MB_ICONSTOP);
		}
		if (bValid == FALSE) {
			bValid = FALSE;
			SelectData(iActualData, TRUE);
			m_editDataLength.SetFocus();
			m_editDataLength.SetSel(0, -1);
		}
	}
	return (bValid);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::GetDialogData() 
{
	UpdateData();	
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::SetDialogData() 
{
	// ListCtrl m_lcData Strings setzen
	CString s;
	// DataType
	CIPCField* pField = NULL;
	if (m_iDataType != -1)
	{
		pField = (CIPCField*)m_cbDataType.GetItemDataPtr(m_iDataType);
	}
	CSecID id(GetParamTypeFromDatabaseField(pField));
	m_lcData.SetItemText(m_iSelectedItemData, 0, GetUIStringOfDataTypeID(id));
	// Position
	s.Format(_T("%lu"), m_dwPosition);
	m_lcData.SetItemText(m_iSelectedItemData, 1, s);
	// Laenge
	if (m_bDataLengthVariable) {
		s.LoadString(IDS_VARIABLE);
	}
	else {
		s.Format(_T("%lu"), m_dwDataLength);
	}
	m_lcData.SetItemText(m_iSelectedItemData, 2, s);
	// Trennzeichen
	m_lcData.SetItemText(m_iSelectedItemData, 3, m_sSeperator);
	// Tag
	m_lcData.SetItemText(m_iSelectedItemData, 4, m_sTag);
	// DataFormat
	eSDIDataFormat eFormat = (eSDIDataFormat)m_cbDataFormat.GetItemData(m_iDataFormat);
	if (SDI_DATA_FORMAT_IGNORE == eFormat) {
		s = _T("");
	}
	else {
		s = GetUIStringOfDataFormat(eFormat);
	}
	m_lcData.SetItemText(m_iSelectedItemData, 5, s);

	UpdateSampleStringPlaceholder();
	UpdateData(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::LoadFromSDIProtocol() 
{
	if (0 <= m_iProtocol) {
		m_pCSDIProtocol = m_pCSDIProtocolArray->GetAt(m_iProtocol);
		if (m_pCSDIProtocol) {
			m_sName = m_pCSDIProtocol->GetName();
			m_iProtocolFormat = m_pCSDIProtocol->GetProtocolFormat();
			m_iReadOrder = m_pCSDIProtocol->GetReadReverse();
			m_dwDifferenceTotalLength = m_pCSDIProtocol->GetTotalLength();
			m_bDifferenceTotalLength = (0 < m_dwDifferenceTotalLength);
			m_dwDifferencePosition = m_pCSDIProtocol->GetPositionIdentifyString();
			m_sDifferenceString = ChangeSpecialCharactersAsciiToToken(
													m_pCSDIProtocol->GetIdentifyString() );
			m_bDifferenceStringAtPosition = ( !m_sDifferenceString.IsEmpty() );
		}
	}
	else {
		m_pCSDIProtocol = NULL;
	}
	FillData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::LoadFromSDIProtocolData() 
{
	if (m_pCSDIProtocolData)
	{
		DWORD dwData = 0xffffffff;
		m_iDataType = -1;
		CIPCField* pField = NULL;
		for (int i=0 ; i<m_cbDataType.GetCount() ; i++)
		{
			pField = (CIPCField*)m_cbDataType.GetItemDataPtr(i);
			dwData = GetParamTypeFromDatabaseField(pField);
			if (dwData == m_pCSDIProtocolData->GetID().GetID() )
			{
				m_iDataType = i;
				break;
			}
		}
		m_dwPosition = m_pCSDIProtocolData->GetPosition();
		m_dwDataLength = m_pCSDIProtocolData->GetLength();
		m_bDataLengthVariable = FALSE;
		if (-1==m_dwDataLength) {
			m_bDataLengthVariable = TRUE;
			m_dwDataLength = 0;
		}
		m_sSeperator = ChangeSpecialCharactersAsciiToToken( m_pCSDIProtocolData->GetSeperator() );
		m_sTag = m_pCSDIProtocolData->GetTag();
		m_iDataFormat = -1;
		FillComboboxDataFormat();
		for (i=0 ; i<m_cbDataFormat.GetCount() ; i++) {
			dwData = m_cbDataFormat.GetItemData(i);
			if ( (eSDIDataFormat)dwData == m_pCSDIProtocolData->GetFormat() ) {
				m_iDataFormat = i;
				break;
			}
		}
	}
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::ClearData() 
{
	m_lcData.DeleteAllItems();
	m_iSelectedItemData = -1;
	m_pCSDIProtocolData = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::FillData() 
{
	ClearData();
	int iDataToSelect = -1;
	if (m_pCSDIProtocol) {
		CSDIProtocolData* pProtocolData = NULL;
		// Schleife ueber alle Data-Elemente
		for (int i=0 ; i<m_pCSDIProtocol->GetDataArray().GetSize() ; i++) 
		{
			iDataToSelect = 0;
			pProtocolData = m_pCSDIProtocol->GetDataArray().GetAt(i);
			InsertData(-1, pProtocolData);
		}
	}
	SelectData(iDataToSelect);
}
/////////////////////////////////////////////////////////////////////////////
int CSDIConfigurationDialog::InsertData(int iIndex, const CSDIProtocolData* pProtocolData)
{
	LV_ITEM lvis;
	CString s,sl;

	// Wenn iIndex == -1, dann am Ende einfuegen, daher ListControl-Count holen
	if (-1 == iIndex) {
		iIndex = m_lcData.GetItemCount();
	}

	// DatenTyp setzen und Item einfuegen
	s = GetUIStringOfDataTypeID(pProtocolData->GetID());
	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)pProtocolData;
	lvis.iItem = iIndex;
	lvis.iSubItem = 0;
	iIndex = m_lcData.InsertItem(&lvis);
	s.ReleaseBuffer();

	// Position setzen
	s.Format(_T("%lu"), pProtocolData->GetPosition());
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 1;
	m_lcData.SetItem(&lvis);
	s.ReleaseBuffer();

	// Laenge setzen
	if ( 0 <= pProtocolData->GetPosition() ) {
		s.Format(_T("%lu"), pProtocolData->GetLength() );
	}
	else {
		s.LoadString(IDS_VARIABLE);
	}
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 2;
	m_lcData.SetItem(&lvis);
	s.ReleaseBuffer();

	// Trennzeichen setzen
	s = ChangeSpecialCharactersAsciiToToken( pProtocolData->GetSeperator() );
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 3;
	m_lcData.SetItem(&lvis);
	s.ReleaseBuffer();

	// Tag setzen
	s = pProtocolData->GetTag();
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 4;
	m_lcData.SetItem(&lvis);
	s.ReleaseBuffer();

	// Format setzen
	s = GetUIStringOfDataFormat(pProtocolData->GetFormat());
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 5;
	m_lcData.SetItem(&lvis);
	s.ReleaseBuffer();

	return iIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::SelectData(int i, BOOL bSetSel/* = TRUE*/)
{
	// DO NOT check i==m_iSelectedItemData, if a new item is inserted
	// the first index might need a refresh

	if (i>=0)
	{
		m_iSelectedItemData = i;
		if (bSetSel)
		{
			m_lcData.SetItemState(m_iSelectedItemData,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		}

		m_pCSDIProtocolData = (CSDIProtocolData*)(m_lcData.GetItemData(m_iSelectedItemData));
	}
	else
	{
		m_iSelectedItemData = -1;
		m_pCSDIProtocolData = NULL;
	}
	LoadFromSDIProtocolData();
}
/////////////////////////////////////////////////////////////////////////////
CString CSDIConfigurationDialog::ChangeSpecialCharactersTokenToAscii(CString& sToken) 
{
	CString sAscii;
	int iAscii = 0;
	for (int i=0 ; i<sToken.GetLength() ; i++) {
		if (_T('\\') == sToken.GetAt(i)
			&& 3 < sToken.GetLength()-i
			&& isdigit( sToken.GetAt(i+1) )
			&& isdigit( sToken.GetAt(i+2) )
			&& isdigit( sToken.GetAt(i+3) )
			)
		{
			iAscii = _ttoi( sToken.Mid(i+1, 3) );
			// Niemals ASCII_NUL uebernehmen
			if (0 < iAscii && iAscii < 256) { 
				sAscii += (char)iAscii;
			}
			else {
				// ASCII_NUL oder >255 wird ignoriert
				WK_TRACE_ERROR(_T("ASCII_NUL or >255 in Token, ignored\n"));
				AfxMessageBox(IDP_WRONG_ASCII_IN_TOKEN);
				sToken = sToken.Left(i) + sToken.Mid(i+4);
			}
			i += 3;
		}
		else {
			sAscii += sToken.GetAt(i);
		}
	}
	return sAscii;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDIConfigurationDialog::ChangeSpecialCharactersAsciiToToken(const CString& sAscii) 
{
	CString sToken;
	CString sAppend;
	for (int i=0 ; i<sAscii.GetLength() ; i++) {
		if (0 < sAscii.GetAt(i) 
			&& sAscii.GetAt(i) < 32 )
		{
			sAppend.Format( _T("\\%03i"), (int)sAscii.GetAt(i) );
			sToken += sAppend;
		}
		else {
			sToken += sAscii.GetAt(i);
		}
	}
	return sToken;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::ClearSampleData() 
{
	m_lcSample.DeleteAllItems();
	m_iSelectedItemSample = -1;
	m_sSampleRead = _T("");
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::SelectSampleData(int i, BOOL bSetSel/* = TRUE*/)
{
	// DO NOT check i==m_iSelectedItemSample, if a new item is inserted
	// the first index might need a refresh

	if (i>=0)
	{
		m_iSelectedItemSample = i;
		if (bSetSel)
		{
			m_lcData.SetItemState(m_iSelectedItemSample,
								  LVIS_SELECTED|LVIS_FOCUSED,
								  LVIS_SELECTED|LVIS_FOCUSED);
		}
		UpdateSampleDataString();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::UpdateSampleStringPlaceholder() 
{
	if (m_pCSDIProtocol)
	{
		m_sSample = m_pCSDIProtocol->GetSample();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::MoveProtocol(BOOL bUp)
{
	// move object in Array und ListBox
	int iOriginalIndex = m_lbProtocol.GetCurSel();
	int iNewIndex = iOriginalIndex;
	if (bUp) {
		iNewIndex = iOriginalIndex - 1;
	}
	else {
		iNewIndex = iOriginalIndex + 1;
	}
	m_pCSDIProtocolArray->RemoveAt(iOriginalIndex);
	m_lbProtocol.DeleteString(iOriginalIndex);
	m_pCSDIProtocolArray->InsertAt(iNewIndex, m_pCSDIProtocol);
	m_lbProtocol.InsertString( iNewIndex, m_pCSDIProtocol->GetName() );
	m_lbProtocol.SetCurSel(iNewIndex);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::MoveProtocolData(BOOL bUp)
{
	// move object in Array und ListCtrl
	int iOriginalIndex = m_iSelectedItemData;
	int iNewIndex = iOriginalIndex;
	if (bUp) {
		iNewIndex = iOriginalIndex - 1;
	}
	else {
		iNewIndex = iOriginalIndex + 1;
	}
	CSDIProtocolData* pOriginalData = m_pCSDIProtocol->GetDataArray().GetAt(m_iSelectedItemData);
	m_pCSDIProtocol->GetDataArray().RemoveAt(m_iSelectedItemData);
	if (m_lcData.DeleteItem(m_iSelectedItemData))	// remove from listCtrl
	{
		m_pCSDIProtocol->GetDataArray().InsertAt(iNewIndex, pOriginalData);
		InsertData(iNewIndex, pOriginalData);
		SelectData(iNewIndex, TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigurationDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	CString sTitel, sFormat;
	GetWindowText(sFormat);
	sTitel.Format(sFormat, CSDIControl::UIStringOfEnum(m_eType), m_iCom);
	SetWindowText(sTitel);

	CenterWindow(GetParent());

	InitDlgControls();

	m_editSampleRead.GetWindowRect(&m_rcEditSampleRead);
	m_animReadSample.Open(IDR_AVI_READ_SAMPLE_STOPPED);

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
/*
	CString sClass;
	GetClassName(m_animReadSample.m_hWnd,sClass.GetBuffer(255),255);
	sClass.ReleaseBuffer();
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonProtocolNew() 
{
	if ( IsProtocolDataValid(TRUE) ) {
		CSDIProtocol* pProtocol = new CSDIProtocol( m_pCSDIProtocolArray->GetSize()+1 );
		CString sName;
		sName.LoadString(IDS_NEW_PROTOCOLFORMAT);
		pProtocol->SetName(sName);
		int iIndex = m_lbProtocol.AddString( pProtocol->GetName() );
		if (LB_ERR != iIndex) {
			m_pCSDIProtocolArray->Add(pProtocol);
			m_iProtocol = m_lbProtocol.SetCurSel(iIndex);
			SetProtocolDefaultValues();
			LoadFromSDIProtocol();
		}
		else {
			WK_DELETE(pProtocol);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonProtocolDelete() 
{
	m_lbProtocol.DeleteString(m_iProtocol);
	m_pCSDIProtocolArray->RemoveAt(m_iProtocol);
	WK_DELETE(m_pCSDIProtocol);
	m_iProtocol = m_lbProtocol.SetCurSel( min(m_iProtocol+1,
											  m_pCSDIProtocolArray->GetSize()-1 ) );
	LoadFromSDIProtocol();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnSelchangeListProtocol() 
{
	if ( IsProtocolDataValid() ) {
		GetDialogData();
		LoadFromSDIProtocol();
	}
	else {
		m_lbProtocol.SetCurSel(m_iProtocol);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonProtocolUp() 
{
	if ( 0 < m_lbProtocol.GetCurSel() ) {
		// move object in Array und ListBox
		MoveProtocol(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonProtocolDown() 
{
	if ( m_lbProtocol.GetCurSel() < m_lbProtocol.GetCount() - 1 ) {
		// move object in Array und ListBox
		MoveProtocol(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	m_pCSDIProtocol->SetName(m_sName);
	m_lbProtocol.DeleteString(m_iProtocol);
	m_lbProtocol.InsertString(m_iProtocol, m_sName);
	m_lbProtocol.SetCurSel(m_iProtocol);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnRadioProtocolFormat() 
{
	GetDialogData();
	m_pCSDIProtocol->SetProtocolFormat((eSDIProtocolFormat)m_iProtocolFormat);
	FillComboboxDataType();
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnRadioReadOrder() 
{
	GetDialogData();
	m_pCSDIProtocol->SetReadReverse(m_iReadOrder);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnCheckDifferenceTotalLength() 
{
	GetDialogData();
	if (!m_bDifferenceTotalLength) {
		m_dwDifferenceTotalLength = 0;
	}
	m_pCSDIProtocol->SetTotalLength(m_dwDifferenceTotalLength);
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditDifferenceTotalLength() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	m_pCSDIProtocol->SetTotalLength(m_dwDifferenceTotalLength);
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnCheckDifferenceStringAtPosition() 
{
	GetDialogData();
	if (!m_bDifferenceStringAtPosition) {
		m_dwDifferencePosition = 1;
		m_sDifferenceString.Empty();
	}
	m_pCSDIProtocol->SetPositionIdentifyString(m_dwDifferencePosition);
	m_pCSDIProtocol->SetIdentifyString(m_sDifferenceString);
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditDifferenceStringAtPosition() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	// Sonderzeichen muessen umgewandelt werden
	m_pCSDIProtocol->SetIdentifyString( ChangeSpecialCharactersTokenToAscii(m_sDifferenceString) );
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditDifferencePositionOfString() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	m_pCSDIProtocol->SetPositionIdentifyString(m_dwDifferencePosition);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonSpecialCharactersIdentifyString() 
{
	CSpecialCharactersDialog dlg;
	if ( IDOK == dlg.DoModal() ) {
		m_editDifferenceStringAtPosition.ReplaceSel( dlg.GetSpecialCharacter(), TRUE );
		// OnChangeEditDifferenceStringAtPosition wird automatisch aufgerufen
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonDataNew() 
{
	if ( IsDataValid(m_iSelectedItemData) ) {
		CSDIProtocolData* pData = new CSDIProtocolData(PARAM_ACCOUNT);
		int iIndex = InsertData(-1, pData);
		if (-1 != iIndex) {
			m_pCSDIProtocol->GetDataArray().Add(pData);
			SelectData(iIndex);
			m_lcData.SetFocus();
		}
		else {
			WK_DELETE(pData);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonDataDelete() 
{
	if (m_iSelectedItemData!=-1)
	{
		// delete object first
		m_pCSDIProtocol->GetDataArray().RemoveAt(m_iSelectedItemData);
		WK_DELETE(m_pCSDIProtocolData);
		// calc new selection
		int	newCount = m_lcData.GetItemCount()-1;	// pre-subtract before DeleteItem
		int i=m_iSelectedItemData;	// new selection

		// range check
		if (i > newCount-1 ) {	// outside ?
			i=newCount-1;
		} else if (newCount==0) {	// last one ?
			i = -1;
		}

		// CAVEAT DeleteItem seems to cause SelectActivation automatically
		if (m_lcData.DeleteItem(m_iSelectedItemData))	// remove from listCtrl
		{
			SelectData(i);
			m_lcData.SetFocus();
		}
	}
}/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != m_iSelectedItemData)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectData(pNMListView->iItem, FALSE);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnClickListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem == -1)
	{
		SelectData(m_iSelectedItemData, TRUE);
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnDblclkListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem == -1)
	{
		SelectData(m_iSelectedItemData, TRUE);
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnRclickListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem == -1)
	{
		SelectData(m_iSelectedItemData, TRUE);
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnRdblclkListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem == -1)
	{
		SelectData(m_iSelectedItemData, TRUE);
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonDataUp() 
{
	if (0 < m_iSelectedItemData)
	{
		// move object in Array und ListCtrl
		MoveProtocolData(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonDataDown() 
{
	if (m_iSelectedItemData!=-1
		&& m_iSelectedItemData != m_pCSDIProtocol->GetDataArray().GetUpperBound() )
	{
		// move object in Array und ListCtrl
		MoveProtocolData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnSelchangeComboDataType() 
{
	GetDialogData();
	CIPCField* pField = NULL;
	if (m_iDataType != -1)
	{
		pField = (CIPCField*)m_cbDataType.GetItemDataPtr(m_iDataType);
	}
	CSecID id(GetParamTypeFromDatabaseField(pField));
	m_pCSDIProtocolData->SetID(id);
	FillComboboxDataFormat();
	BOOL bIsDateOrTime = (pField && (pField->IsDate() || pField->IsTime()));
	if (bIsDateOrTime)
	{
		eSDIDataFormat eFormat = (eSDIDataFormat)m_cbDataFormat.GetItemData(m_iDataFormat);
		m_pCSDIProtocolData->SetFormat(eFormat);

		m_dwDataLength = GetUIStringOfDataFormat(eFormat).GetLength();
		m_pCSDIProtocolData->SetLength(m_dwDataLength);
		m_bDataLengthVariable = FALSE;

		m_sSeperator = _T("");
		m_pCSDIProtocolData->SetSeperator(m_sSeperator);
	}

	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditPosition() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	m_pCSDIProtocolData->SetPosition(m_dwPosition);
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditDataLength() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	m_pCSDIProtocolData->SetLength(m_dwDataLength);
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnCheckDataLengthVariable() 
{
	GetDialogData();
	if (m_bDataLengthVariable) {
		m_dwDataLength = 0;
		m_pCSDIProtocolData->SetLength((DWORD)(-1));
	}
	else {
		m_pCSDIProtocolData->SetLength(m_dwDataLength);
		m_sSeperator = _T("");
		m_pCSDIProtocolData->SetSeperator(m_sSeperator);
	}
	SetDialogData();
	if (m_bDataLengthVariable) {
		m_editSeperator.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditSeperator() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	// Sonderzeichen muessen umgewandelt werden
	m_pCSDIProtocolData->SetSeperator( ChangeSpecialCharactersTokenToAscii(m_sSeperator) );
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonSpecialCharactersSeperator()
{
	CSpecialCharactersDialog dlg;
	if ( IDOK == dlg.DoModal() ) {
		m_editSeperator.ReplaceSel( dlg.GetSpecialCharacter(), TRUE );
		// OnChangeEditSeperator wird automatisch aufgerufen
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnChangeEditTag() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	GetDialogData();
	m_pCSDIProtocolData->SetTag(m_sTag);
	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnSelchangeComboDataFormat() 
{
	GetDialogData();
	eSDIDataFormat eFormat = (eSDIDataFormat)m_cbDataFormat.GetItemData(m_iDataFormat);
	m_pCSDIProtocolData->SetFormat(eFormat);

	m_dwDataLength = GetUIStringOfDataFormat(eFormat).GetLength();
	m_pCSDIProtocolData->SetLength(m_dwDataLength);
	m_bDataLengthVariable = FALSE;

	m_sSeperator = _T("");
	m_pCSDIProtocolData->SetSeperator(m_sSeperator);

	SetDialogData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnItemchangedListSample(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItemSample)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectSampleData(pNMListView->iItem,FALSE);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnClickListSample(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItemData) {
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_lcSample.SetItemState(m_iSelectedItemData,
							  LVIS_SELECTED|LVIS_FOCUSED,
							  LVIS_SELECTED|LVIS_FOCUSED);
	}
*/
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnDblclkListSample(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItemData) {
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_lcSample.SetItemState(m_iSelectedItemData,
							  LVIS_SELECTED|LVIS_FOCUSED,
							  LVIS_SELECTED|LVIS_FOCUSED);
	}
*/
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnRclickListSample(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItemData) {
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_lcSample.SetItemState(m_iSelectedItemData,
							  LVIS_SELECTED|LVIS_FOCUSED,
							  LVIS_SELECTED|LVIS_FOCUSED);
	}
*/
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnRdblclkListSample(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItemData) {
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_lcSample.SetItemState(m_iSelectedItemData,
							  LVIS_SELECTED|LVIS_FOCUSED,
							  LVIS_SELECTED|LVIS_FOCUSED);
	}
*/
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonPortrait() 
{
	SetPortraitAlarm();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonMoney() 
{
	SetMoneyAlarm();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnCheckReadSample() 
{
	if (m_pRS232 == NULL) {
		m_pRS232 = CreateRS232();
		if (m_pRS232) {
			PrepareSDIUnitForStop();
			int iCounter = 0;
			while (iCounter<3 && m_pRS232->IsOpen() == FALSE) {
				iCounter++;
				m_pRS232->Open(*m_pComParameters);
				Sleep(100);
			}
			if ( m_pRS232->IsOpen() ) {
				// Alles ok, wir koennen weitermachen
				ClearSampleData();
				m_bSampleReadActive = TRUE;
				m_pRS232->StartReadInData();
			}
			else {
				// Hat nicht geklappt, koennen keine Daten kommen
				WK_DELETE(m_pRS232);
				AfxMessageBox(IDP_PORT_NOT_OPENED, MB_OK);
				m_bSDIUnitStart = FALSE;
			}
			UpdateData(FALSE);
			EnableControls();
			if (m_bSampleReadActive) {
//				m_animReadSample.ShowWindow(SW_SHOW);
				m_animReadSample.Open(IDR_AVI_READ_SAMPLE);
			}
		}
		else {
		}
	}
	else {
		m_pRS232->StopReadInData();
		CWK_StopWatch watch;
		m_pRS232->Close();
		CString sWatchTrace;
		sWatchTrace.Format(_T("Com%i closed"), m_pRS232->GetCOMNumber());
		watch.StartWatch();
		while (m_pRS232->IsShuttingDown())
		{
			Sleep(0);
			if (watch.GetElapsedMS() > 5000)
			{
				sWatchTrace.Format(_T("ERROR: Com%i close FAILED"), m_pRS232->GetCOMNumber());
				break;
			}
		}
		watch.StopWatch(sWatchTrace);
		WK_DELETE(m_pRS232);
		m_animReadSample.Open(IDR_AVI_READ_SAMPLE_STOPPED);
//		m_animReadSample.ShowWindow(SW_HIDE);
		m_bSampleReadActive = FALSE;
		PrepareSDIUnitForStart();
		UpdateData(FALSE);
		EnableControls();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::PrepareSDIUnitForStop()
{
	CString sAppName = GetAppnameFromId(WAI_SDIUNIT);
	HWND hWnd = ::FindWindow(sAppName,NULL);
	if (hWnd) {
		m_bSDIUnitStart = TRUE;
		::PostMessage(hWnd,WM_COMMAND,ID_APP_EXIT,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::PrepareSDIUnitForStart()
{
	if (m_bSDIUnitStart) {
		CString sAppName = GetAppnameFromId(WAI_SDIUNIT);
		HWND hWnd = ::FindWindow(sAppName,NULL);
		::PostMessage(hWnd,WM_COMMAND,ID_APP_EXIT,0L);
		m_bSDIUnitStart = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSDIConfigurationDialog::OnReadInData(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
	if (m_pRS232) 
	{
		CString sData = m_pRS232->GetAndRemoveData();
		if (m_pCSDIProtocol->GetReadReverse())
		{
			sData.MakeReverse();
		}

		InsertSampleData(sData);
	}
	return (LRESULT)0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonSampleTakeover() 
{
	TRACE(_T("CSDIConfigurationDialog::OnButtonSampleTakeover\n"));
	if (m_iSelectedItemData!=-1)
	{
		CString s;
		s = m_lcData.GetItemText(m_iSelectedItemData,0);

		int iStart=0, iEnd=0;
		m_editSampleRead.GetSel(iStart, iEnd);
		if ((iEnd - iStart) > 0)
		{
			CString sMsg;
			sMsg.Format(IDP_SAMPLE_TAKEOVER, iStart + 1, iEnd - iStart, s);
			if (IDYES == AfxMessageBox(sMsg,MB_YESNO))
			{
				m_dwPosition = iStart + 1;
				m_dwDataLength = iEnd - iStart;
				m_pCSDIProtocolData->SetPosition(m_dwPosition);
				m_pCSDIProtocolData->SetLength(m_dwDataLength);
				SetDialogData();
			}
		}
		else
		{
			m_btnSampleTakeover.EnableWindow(FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnOK() 
{
	if ( IsProtocolDataValid() ) {
		CWK_Dialog::OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnButtonDefault() 
{
	// TODO: Add your control notification handler code here
	SetDefaultValues();
	InitDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	m_editSampleRead.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.left = rect.left - 50;
	rect.top = rect.top - 50;
	rect.right = rect.right + 25;
	rect.bottom = rect.bottom + 25;
	if (rect.PtInRect(point) )
	{
		EnableControls();
	}
	
	CWK_Dialog::OnMouseMove(nFlags, point);
}
