// SDIConfigurationDialogIBM.cpp: implementation of the CSDIConfigurationDialogIBM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationDialogIBM.h"
#include "SDIConfigurationIBMRS232.h"
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
CSDIConfigurationDialogIBM::CSDIConfigurationDialogIBM(SDIControlType eType,
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
CSDIConfigurationDialogIBM::~CSDIConfigurationDialogIBM()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogIBM::InitListControlSampleData()
{
	// ListCtrl initialisieren
	// Erstmal alle Eintraege loeschen
	m_lcSample.DeleteAllItems();

	// Anzahl der Spalten und die Spaltenbreiten festlegen
	const int iColumns = 4;
	const int w[] = {42,15,11,72};	// Fuer jede Spalte ein Eintrag
	const int iwTotal = 140;	// Muss die Summe der einzelnen Spaltenbreiten sein!

	// Spaltenueberschriften ins Array schreiben
	CString s[iColumns];		// Array fuer die Spaltenueberschriften
	s[0].LoadString(IDS_SDI_SAMPLE_HEADER);
	s[1].LoadString(IDS_SDI_SAMPLE_ALARM);
	s[2].LoadString(IDS_SDI_SAMPLE_TOTAL_LEN);
	s[3].LoadString(IDS_SDI_SAMPLE_DATA);

	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	int icx = 0;
	int iTotalcx = 0;
	CRect			rect;
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
void CSDIConfigurationDialogIBM::EnableControls()
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

	bExtraEnable = bEnableData && !m_bSampleReadActive && m_pPage->IsLocal();
	m_btnAlarmPortrait.EnableWindow(bExtraEnable);
	m_btnAlarmMoney.EnableWindow(bExtraEnable);

	m_btnDefault.ShowWindow(SW_HIDE);
	m_btnDefault.EnableWindow(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationRS232* CSDIConfigurationDialogIBM::CreateRS232()
{
	CSDIConfigurationRS232* pRS232 = NULL;
	pRS232 = new CSDIConfigurationIbmRS232(this, m_iCom);
	return pRS232;
}
/////////////////////////////////////////////////////////////////////////////
int CSDIConfigurationDialogIBM::InsertSampleData(CString sOneSample)
{
	LV_ITEM lvis;
	CString s,sl;
	int iIndex = -1;
	int iIndexString;

	// Wenn iIndex == -1, dann am Ende einfuegen, daher ListControl-Count holen
	if (-1 == iIndex) {
		iIndex = m_lcSample.GetItemCount();
	}

	// Erste Spalte setzen und Item einfuegen
	s = sOneSample.Left(IBM_MSG_HEADER_LEN);
	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)NULL;
	lvis.iItem = iIndex;
	lvis.iSubItem = 0;
	iIndex = m_lcSample.InsertItem(&lvis);
	s.ReleaseBuffer();

	// Zweite Spalte setzen und Item einfuegen
	iIndexString = IBM_MSG_HEADER_LEN-3;
	if (sOneSample.GetLength() >= iIndexString) {
		s = sOneSample.Mid(iIndexString, 2);
		lvis.mask = LVIF_TEXT;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();
		lvis.iItem = iIndex;
		lvis.iSubItem = 1;
		m_lcSample.SetItem(&lvis);
		s.ReleaseBuffer();
	}

	// Header abschneiden
	iIndexString = IBM_MSG_HEADER_LEN;
	if (sOneSample.GetLength() >= iIndexString) {
		sOneSample = sOneSample.Mid(iIndexString);
	}

	// Dritte Spalte setzen und Item einfuegen
	s.Format(_T("%i"), sOneSample.GetLength());
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 2;
	m_lcSample.SetItem(&lvis);
	s.ReleaseBuffer();

	// Vierte Spalte setzen und Item einfuegen
	s = sOneSample;
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 3;
	m_lcSample.SetItem(&lvis);
	s.ReleaseBuffer();

	m_lcSample.EnsureVisible(iIndex, FALSE);
	return iIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogIBM::UpdateSampleDataString()
{
	m_sSampleRead = _T("");
	m_sSampleRead = m_lcSample.GetItemText(m_iSelectedItemSample, 3);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogIBM::SetProtocolDefaultValues()
{
	if (0 <= m_iProtocol)
	{
		CSDIProtocol* pProtocol = m_pCSDIProtocolArray->GetAt(m_iProtocol);

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
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogIBM::SetPortraitAlarm()
{
	if (m_iSelectedItemSample!=-1)
	{
		int iAlarm = _ttoi(m_lcSample.GetItemText(m_iSelectedItemSample, 1));
		CString s;
		s.Format(IDP_PHOTOSTEP_AS_PORTRAIT,iAlarm);
		if (IDYES==AfxMessageBox(s,MB_YESNO))
		{
			CSDIAlarmType* pNewAlarmType = new CSDIAlarmType(SDI_ALARM_PORTRAIT, iAlarm-1);
			m_pAlarmTypeArray->AddOrReplaceAllOnlyOnce(pNewAlarmType);
		}
	}
	else
	{
		AfxMessageBox(IDP_NO_PHOTOSTEP);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationDialogIBM::SetMoneyAlarm()
{
	if (m_iSelectedItemSample!=-1)
	{
		int iAlarm = _ttoi(m_lcSample.GetItemText(m_iSelectedItemSample, 1));
		CString s;
		s.Format(IDP_PHOTOSTEP_AS_MONEY,iAlarm);
		if (IDYES==AfxMessageBox(s,MB_YESNO))
		{
			CSDIAlarmType* pNewAlarmType = new CSDIAlarmType(SDI_ALARM_MONEY, iAlarm-1);
			m_pAlarmTypeArray->AddOrReplaceAllOnlyOnce(pNewAlarmType);
		}
	}
	else
	{
		AfxMessageBox(IDP_NO_PHOTOSTEP);
	}
}
