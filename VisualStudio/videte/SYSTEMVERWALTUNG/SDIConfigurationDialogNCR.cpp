// SDIConfigurationDialogNCR.cpp: implementation of the CSDIConfigurationDialogNCR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationDialogNCR.h"
#include "SDIConfigurationNcrRS232.h"
#include "SDIAlarmType.h"
#include "SVPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CSDIConfigurationDialogNCR::CSDIConfigurationDialogNCR(SDIControlType eType,
														int iCom,
														CSDIProtocolArray* pProtocolArray,
														CSDIAlarmTypeArray* pAlarmTypeArray,
														CComParameters* pComParameters,
														CSVView* pView)
	: CSDIConfigurationDialog(eType,
								iCom,
								pProtocolArray,
								pAlarmTypeArray,
								pComParameters,
								pView)
{

}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationDialogNCR::~CSDIConfigurationDialogNCR()
{

}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogNCR::InitListControlSampleData()
{
	// ListCtrl initialisieren
	// Erstmal alle Eintraege loeschen
	m_lcSample.DeleteAllItems();

	// Anzahl der Spalten und die Spaltenbreiten festlegen
	const int iColumns = 2;
	const int w[] = {10, 90};		// Fuer jede Spalte ein Eintrag
	const int iwTotal = 100;	// Muss die Summe der einzelnen Spaltenbreiten sein!

	// Spaltenueberschriften ins Array laden
	CString s[iColumns];
	s[0].LoadString(IDS_SDI_SAMPLE_TOTAL_LEN);
	s[1].LoadString(IDS_SDI_SAMPLE_DATA);

	LV_COLUMN	lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	int		icx = 0;
	int		iTotalcx = 0;
	CRect	rect(0,0,0,0);
	m_lcSample.GetClientRect(&rect);
	for (int i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		m_lcSample.DeleteColumn(i);  // assumes return value is OK.
		// Die letzte Spalte bekommt den Rest 
		if (i == iColumns-1) {
			lvcolumn.cx = rect.Width() - iTotalcx;
		}
		else {
			icx = (rect.Width() * w[i])/iwTotal;
			lvcolumn.cx = icx;
		}
		iTotalcx += icx;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		m_lcSample.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}
	LONG dw = ListView_GetExtendedListViewStyle(m_lcSample.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_lcSample.m_hWnd,dw);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogNCR::EnableControls()
{
	EnableControlsBase();

	BOOL bEnableProtocol = FALSE;
	BOOL bEnableData = FALSE;
	BOOL bExtraEnable = FALSE;
	if (-1 != m_lbProtocol.GetCurSel() ) {
		bEnableProtocol = TRUE;
		if (0 != m_lcData.GetItemCount() ) {
			bEnableData = TRUE;
		}
	}

	m_btnAlarmPortrait.ShowWindow(SW_HIDE);
	m_btnAlarmPortrait.EnableWindow(FALSE);

	m_btnAlarmMoney.ShowWindow(SW_HIDE);
	m_btnAlarmMoney.EnableWindow(FALSE);

	bExtraEnable = !m_bSampleReadActive;
	m_btnDefault.ShowWindow(SW_SHOW);
	m_btnDefault.EnableWindow(bExtraEnable);
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationRS232* CSDIConfigurationDialogNCR::CreateRS232()
{
	CSDIConfigurationRS232* pRS232 = NULL;
	pRS232 = new CSDIConfigurationNcrRS232(this, m_iCom);
	return pRS232;
}
/////////////////////////////////////////////////////////////////////////////
int CSDIConfigurationDialogNCR::InsertSampleData(CString sOneSample)
{
	LV_ITEM lvis;
	CString s;
	int iIndex = -1;

	// Wenn iIndex == -1, dann am Ende einfuegen, daher ListControl-Count holen
	if (-1 == iIndex) {
		iIndex = m_lcSample.GetItemCount();
	}

	// Erste Spalte setzen und Item einfuegen
	s.Format(_T("%i"), sOneSample.GetLength());
	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)NULL;
	lvis.iItem = iIndex;
	lvis.iSubItem = 0;
	iIndex = m_lcSample.InsertItem(&lvis);
	s.ReleaseBuffer();

	// Zweite Spalte setzen und Item einfuegen
	s = sOneSample;
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 1;
	m_lcSample.SetItem(&lvis);
	s.ReleaseBuffer();

	m_lcSample.EnsureVisible(iIndex, FALSE);
	return iIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogNCR::UpdateSampleDataString()
{
	m_sSampleRead = _T("");
	m_sSampleRead = m_lcSample.GetItemText(m_iSelectedItemSample, 1);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogNCR::SetDefaultValues()
{
	m_pCSDIProtocolArray->DeleteAll();

	// 4 Protokoll-Varianten anlegen
	// (3 verschiedene Startkommando-Varianten und Diagnose)
	CSDIProtocol* pProtocol = NULL;
	CSDIProtocolData* pData = NULL;
	CString sString;
	
	pProtocol = new CSDIProtocol(1);
	sString.LoadString(IDS_VARIANT);
	sString += _T(" 1 (50xx)");
	pProtocol->SetName(sString);
	pProtocol->SetReadReverse(TRUE);
	pProtocol->SetTotalLength(37);
	sString = _T("\\031");
	pProtocol->SetIdentifyString( ChangeSpecialCharactersTokenToAscii(sString) );
	pProtocol->SetPositionIdentifyString(25);
	//
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(1);
	pData->SetLength(4);
	pData->SetFormat(SDI_DATA_FORMAT_HHMM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(6);
	pData->SetLength(6);
	pData->SetFormat(SDI_DATA_FORMAT_YYMMDD);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TRANSAKTION);
	pData->SetPosition(13);
	pData->SetLength(3);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_BANKCODE);
	pData->SetPosition(17);
	pData->SetLength(8);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(26);
	pData->SetLength(10);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	pProtocol = new CSDIProtocol(2);
	sString.LoadString(IDS_VARIANT);
	sString += _T(" 2 (56xx, 50xx, PersonaSxx)");
	pProtocol->SetName(sString);
	pProtocol->SetReadReverse(TRUE);
	pProtocol->SetTotalLength(36);
	sString = _T("\\031\\031");
	pProtocol->SetIdentifyString( ChangeSpecialCharactersTokenToAscii(sString) );
	pProtocol->SetPositionIdentifyString(16);
	//
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(1);
	pData->SetLength(4);
	pData->SetFormat(SDI_DATA_FORMAT_HHMM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(6);
	pData->SetLength(6);
	pData->SetFormat(SDI_DATA_FORMAT_YYMMDD);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TRANSAKTION);
	pData->SetPosition(13);
	pData->SetLength(3);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_BANKCODE);
	pData->SetPosition(18);
	pData->SetLength(8);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(27);
	pData->SetLength(10);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	pProtocol = new CSDIProtocol(3);
	sString.LoadString(IDS_VARIANT);
	sString += _T(" 3 (56xx, 50xx, PersonaSxx)");
	pProtocol->SetName(sString);
	pProtocol->SetReadReverse(TRUE);
	pProtocol->SetTotalLength(37);
	sString = _T("\\031\\031");
	pProtocol->SetIdentifyString( ChangeSpecialCharactersTokenToAscii(sString) );
	pProtocol->SetPositionIdentifyString(17);
	//
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(1);
	pData->SetLength(4);
	pData->SetFormat(SDI_DATA_FORMAT_HHMM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(6);
	pData->SetLength(6);
	pData->SetFormat(SDI_DATA_FORMAT_YYMMDD);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TRANSAKTION);
	pData->SetPosition(13);
	pData->SetLength(4);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_BANKCODE);
	pData->SetPosition(19);
	pData->SetLength(8);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(28);
	pData->SetLength(10);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	pProtocol = new CSDIProtocol(4);
	sString.LoadString(IDS_DIAGNOSE_STARTCOMMAND);
	pProtocol->SetName(sString);
	pProtocol->SetReadReverse(TRUE);
	pProtocol->SetTotalLength(25);
	//
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(1);
	pData->SetLength(4);
	pData->SetFormat(SDI_DATA_FORMAT_HHMM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(6);
	pData->SetLength(6);
	pData->SetFormat(SDI_DATA_FORMAT_YYMMDD);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TRANSAKTION);
	pData->SetPosition(13);
	pData->SetLength(4);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(6);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogNCR::SetProtocolDefaultValues()
{
	if (0 <= m_iProtocol)
	{
		CSDIProtocol* pProtocol = m_pCSDIProtocolArray->GetAt(m_iProtocol);
		if (pProtocol) {
			pProtocol->SetReadReverse(TRUE);
		}

		// 3 Datenfelder vorgeben, Konto, BLZ, Betrag
		CSDIProtocolData* pData = NULL;

		pData = new CSDIProtocolData(PARAM_ACCOUNT);
		pData->SetPosition(1);
		pData->SetLength(10);
		pProtocol->GetDataArray().Add(pData);
		pData = new CSDIProtocolData(PARAM_BANKCODE);
		pData->SetPosition(1);
		pData->SetLength(8);
		pProtocol->GetDataArray().Add(pData);
		pData = new CSDIProtocolData(PARAM_VALUE);
		pData->SetPosition(1);
		pData->SetLength(0);
		pProtocol->GetDataArray().Add(pData);
	}
}
