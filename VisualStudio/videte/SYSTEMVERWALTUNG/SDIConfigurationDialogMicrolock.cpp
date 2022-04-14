// SDIConfigurationDialogMicrolock.cpp: implementation of the CSDIConfigurationDialogMicrolock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIConfigurationDialogMicrolock.h"
#include "SDIConfigurationMicrolockRS232.h"
#include "SDIAlarmType.h"
#include "SVPage.h"
#include "SDIDefaultMicrolockDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CSDIConfigurationDialogMicrolock::CSDIConfigurationDialogMicrolock(SDIControlType eType,
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
//////////////////////////////////////////////////////////////////////
CSDIConfigurationDialogMicrolock::~CSDIConfigurationDialogMicrolock()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CSDIConfigurationDialogMicrolock::CanNewProtocol()
{
	return (m_lbProtocol.GetCount() < 32);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogMicrolock::InitListControlSampleData()
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
void CSDIConfigurationDialogMicrolock::EnableControls()
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
CSDIConfigurationRS232* CSDIConfigurationDialogMicrolock::CreateRS232()
{
	CSDIConfigurationRS232* pRS232 = NULL;
	pRS232 = new CSDIConfigurationMicrolockRS232(this, m_iCom);
	return pRS232;
}
/////////////////////////////////////////////////////////////////////////////
int CSDIConfigurationDialogMicrolock::InsertSampleData(CString sOneSample)
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
void CSDIConfigurationDialogMicrolock::UpdateSampleDataString()
{
	m_sSampleRead = _T("");
	m_sSampleRead = m_lcSample.GetItemText(m_iSelectedItemSample, 1);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogMicrolock::SetDefaultValues()
{
	CSDIDefaultMicrolockDlg dlg(this);
	if (dlg.DoModal() == IDCANCEL)
	{
		return;
	}

	int iType = dlg.GetType();
	switch (iType)
	{
	case SDI_MICROLOCK_TYPE_SYSTEM1X:
		SetDefaultValuesSystem1x();
		break;
	case SDI_MICROLOCK_TYPE_SYSTEM1:
		SetDefaultValuesSystem1();
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogMicrolock::SetDefaultValuesSystem1x()
{
	m_pCSDIProtocolArray->DeleteAll();

	// CAVEAT: At maximum 32 variants!
	CSDIProtocol* pProtocol = NULL;
	CSDIProtocolData* pData = NULL;
	CString sString;

	// Access granted
	pProtocol = new CSDIProtocol(1);
	sString.LoadString(IDS_ACCESS_GRANTED);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("A");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, ID not in memory
	pProtocol = new CSDIProtocol(2);
	sString.LoadString(IDS_ACCESS_DENIED_ID_UNKNOWN);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("B");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, ID not valid for door
	pProtocol = new CSDIProtocol(3);
	sString.LoadString(IDS_ACCESS_DENIED_ID_INVALID);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("C");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, ID expired
	pProtocol = new CSDIProtocol(4);
	sString.LoadString(IDS_ACCESS_DENIED_ID_EXPIRED);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("D");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, out of time zone
	pProtocol = new CSDIProtocol(5);
	sString.LoadString(IDS_ACCESS_DENIED_OUT_OF_TIMEZONE);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("E");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, anti pass back enforced
	pProtocol = new CSDIProtocol(6);
	sString.LoadString(IDS_ACCESS_DENIED_ANTI_PASS_BACK);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("F");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, PIN time-out
	pProtocol = new CSDIProtocol(7);
	sString.LoadString(IDS_ACCESS_DENIED_PIN_TIMEOUT);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("G");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, wrong PIN
	pProtocol = new CSDIProtocol(8);
	sString.LoadString(IDS_ACCESS_DENIED_WRONG_PIN);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("H");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, 4th (or more) wrong PIN
	pProtocol = new CSDIProtocol(9);
	sString.LoadString(IDS_ACCESS_DENIED_TOO_MANY_WRONG_PIN);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("I");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Access denied, relay latched off
	pProtocol = new CSDIProtocol(10);
	sString.LoadString(IDS_ACCESS_DENIED_RELAY_LATCHED_OFF);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(34);
	sString = _T("K");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(34);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(24);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Card read error
	pProtocol = new CSDIProtocol(11);
	sString.LoadString(IDS_READ_ERROR);
	pProtocol->SetName(sString);
	pProtocol->SetTotalLength(26);
	sString = _T("X01");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(24);
	//
	pData = new CSDIProtocolData(PARAM_DATE);
	pData->SetPosition(1);
	pData->SetLength(8);
	pData->SetFormat(SDI_DATA_FORMAT_DD_MM_YY);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TIME);
	pData->SetPosition(11);
	pData->SetLength(5);
	pData->SetFormat(SDI_DATA_FORMAT_HH_MM);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(18);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);
	// CAVEAT: At maximum 32 variants!
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogMicrolock::SetDefaultValuesSystem1()
{
	AfxMessageBox(IDS_NOTIMPLEMENTED);
}
