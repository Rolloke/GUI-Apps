// SDIConfigurationDialogRondo.cpp: implementation of the CSDIConfigurationDialogRondo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationDialogRondo.h"
#include "SDIConfigurationRondoRS232.h"
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
CSDIConfigurationDialogRondo::CSDIConfigurationDialogRondo(SDIControlType eType,
														int iCom,
														CSDIProtocolArray* pProtocolArray,
														CSDIAlarmTypeArray* pAlarmTypeArray,
														CComParameters* pComParameters,
														CSVPage* pParent)
	: CSDIConfigurationDialog(eType,
								iCom,
								pProtocolArray,
								pAlarmTypeArray,
								pComParameters,
								pParent)
{
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationDialogRondo::~CSDIConfigurationDialogRondo()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogRondo::InitListControlSampleData()
{
	// ListCtrl initialisieren
	// Erstmal alle Eintraege loeschen
	m_lcSample.DeleteAllItems();

	// Anzahl der Spalten und die Spaltenbreiten festlegen
	const int iColumns = 3;
	const int w[] = {20, 10, 90};		// Fuer jede Spalte ein Eintrag
	const int iwTotal = 120;	// Muss die Summe der einzelnen Spaltenbreiten sein!

	// Spaltenueberschriften ins Array laden
	CString s[iColumns];
	s[0].LoadString(IDS_SDI_SAMPLE_HEADER);
	s[1].LoadString(IDS_SDI_SAMPLE_ALARM);
	s[2].LoadString(IDS_SDI_SAMPLE_DATA);

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
void CSDIConfigurationDialogRondo::EnableControls()
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
CSDIConfigurationRS232* CSDIConfigurationDialogRondo::CreateRS232()
{
	CSDIConfigurationRS232* pRS232 = NULL;
	pRS232 = new CSDIConfigurationRondoRS232(this, m_iCom);
	return pRS232;
}
/////////////////////////////////////////////////////////////////////////////
int CSDIConfigurationDialogRondo::InsertSampleData(CString sOneSample)
{
	LV_ITEM lvis;
	CString s;
	int iIndex = -1;
	int iIndexString;

	// Wenn iIndex == -1, dann am Ende einfuegen, daher ListControl-Count holen
	if (-1 == iIndex) {
		iIndex = m_lcSample.GetItemCount();
	}

	// Erste Spalte setzen und Item einfuegen
	s = sOneSample.Left(RONDO_MSG_HEADER_LEN);
	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)NULL;
	lvis.iItem = iIndex;
	lvis.iSubItem = 0;
	iIndex = m_lcSample.InsertItem(&lvis);
	s.ReleaseBuffer();

	// Zweite Spalte setzen und Item einfuegen
	s = sOneSample.Left(1);
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 1;
	m_lcSample.SetItem(&lvis);
	s.ReleaseBuffer();

	// Header abschneiden
	iIndexString = RONDO_MSG_HEADER_LEN;
	if (sOneSample.GetLength() >= iIndexString) {
		sOneSample = sOneSample.Mid(iIndexString);
	}

	// Dritte Spalte setzen und Item einfuegen
	s = sOneSample;
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 2;
	m_lcSample.SetItem(&lvis);
	s.ReleaseBuffer();

	m_lcSample.EnsureVisible(iIndex, FALSE);
	return iIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogRondo::UpdateSampleDataString()
{
	m_sSampleRead = "";
	m_sSampleRead = m_lcSample.GetItemText(m_iSelectedItemSample, 1);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogRondo::SetDefaultValues()
{
	m_pCSDIProtocolArray->DeleteAll();

	// 1 Protokoll-Varianten anlegen
	CSDIProtocol* pProtocol = NULL;
	CSDIProtocolData* pData = NULL;
	CString sString;
	
	pProtocol = new CSDIProtocol(1);
	sString.LoadString(IDS_VARIANT);
	sString += " 1";
	pProtocol->SetName(sString);
	//
	pData = new CSDIProtocolData(PARAM_WORKSTATION);
	pData->SetPosition(1);
	pData->SetLength(6);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_TRANSAKTION);
	pData->SetPosition(8);
	pData->SetLength(4);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_VALUE);
	pData->SetPosition(23);
	pData->SetLength(8);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_BANKCODE);
	pData->SetPosition(38);
	pData->SetLength(8);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_ACCOUNT);
	pData->SetPosition(46);
	pData->SetLength(10);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogRondo::SetSpecialProtocolDefaultValues(CSDIProtocol* pProtocol)
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogRondo::SetPortraitAlarm()
{
	// Nicht erlaubt!
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogRondo::SetMoneyAlarm()
{
	// Nicht erlaubt!
}
