// SDIConfigurationDialogNCTDiva.cpp: implementation of the CSDIConfigurationDialogNCTDiva class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIConfigurationDialogNCTDiva.h"
#include "SDIConfigurationRS232NCTDiva.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSDIConfigurationDialogNCTDiva::CSDIConfigurationDialogNCTDiva(SDIControlType eType,
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
CSDIConfigurationDialogNCTDiva::~CSDIConfigurationDialogNCTDiva()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogNCTDiva::InitListControlSampleData()
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
void CSDIConfigurationDialogNCTDiva::EnableControls()
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
CSDIConfigurationRS232* CSDIConfigurationDialogNCTDiva::CreateRS232()
{
	CSDIConfigurationRS232* pRS232 = NULL;
	pRS232 = new CSDIConfigurationRS232NCTDiva(this, m_iCom);
	return pRS232;
}
/////////////////////////////////////////////////////////////////////////////
int CSDIConfigurationDialogNCTDiva::InsertSampleData(CString sOneSample)
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
void CSDIConfigurationDialogNCTDiva::UpdateSampleDataString()
{
	m_sSampleRead = _T("");
	m_sSampleRead = m_lcSample.GetItemText(m_iSelectedItemSample, 1);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogNCTDiva::SetDefaultValues()
{
	m_pCSDIProtocolArray->DeleteAll();

	CSDIProtocol* pProtocol = NULL;
	CSDIProtocolData* pData = NULL;
	CString sString;

	// Ausleihe/Rückgabe
	pProtocol = new CSDIProtocol(1);
	sString = _T("Ausleihe/Rückgabe");
	pProtocol->SetName(sString);
	sString = _T("EUR");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(33);
	//
	pData = new CSDIProtocolData(PARAM_UNKNOWN);
	pData->SetPosition(1);
	pData->SetLength(19);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_UNKNOWN);
	pData->SetPosition(21);
	pData->SetLength(9);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_CURRENCY);
	pData->SetPosition(33);
	pData->SetLength(3);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_VALUE);
	pData->SetPosition(37);
	pData->SetLength(4);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Dienstleistung
	pProtocol = new CSDIProtocol(1);
	sString = _T("Dienstleistung/Ware");
	pProtocol->SetName(sString);
	sString = _T("EUR");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(31);
	//
	pData = new CSDIProtocolData(PARAM_UNKNOWN);
	pData->SetPosition(1);
	pData->SetLength(19);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_UNKNOWN);
	pData->SetPosition(21);
	pData->SetLength(5);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_CURRENCY);
	pData->SetPosition(31);
	pData->SetLength(3);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_VALUE);
	pData->SetPosition(37);
	pData->SetLength(4);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);

	// Gesamtsumme
	pProtocol = new CSDIProtocol(1);
	sString = _T("Gesamtsumme");
	pProtocol->SetName(sString);
	sString = _T("EUR");
	pProtocol->SetIdentifyString(sString);
	pProtocol->SetPositionIdentifyString(18);
	//
	pData = new CSDIProtocolData(PARAM_UNKNOWN);
	pData->SetPosition(1);
	pData->SetLength(7);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_VALUE);
	pData->SetPosition(9);
	pData->SetLength(8);
	pProtocol->GetDataArray().Add(pData);
	pData = new CSDIProtocolData(PARAM_CURRENCY);
	pData->SetPosition(18);
	pData->SetLength(3);
	pProtocol->GetDataArray().Add(pData);
	//
	m_pCSDIProtocolArray->Add(pProtocol);
}
