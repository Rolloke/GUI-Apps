// DlgAlarmList.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgAlarmList.h"
#include "WndAlarmList.h"
#include ".\dlgalarmlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmList dialog
CDlgAlarmList::CDlgAlarmList(CWndAlarmList* pParent)
	: CSkinDialog(CDlgAlarmList::IDD, pParent)
{
	m_pWndAlarmList = pParent;
	m_iSelectedItem = -1;
	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		MoveOnClientAreaClick(true);
	}
	//{{AFX_DATA_INIT(CDlgAlarmList)
	m_bDetector1 = TRUE;
	m_bDetector2 = TRUE;
	m_bActivity = TRUE;
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAlarmList)
	DDX_Control(pDX, IDC_LIST_ALARM, m_ListAlarm);
	DDX_Check(pDX, IDC_CHECK_1, m_bDetector1);
	DDX_Check(pDX, IDC_CHECK_2, m_bDetector2);
	DDX_Check(pDX, IDC_CHECK_ACTIVITY, m_bActivity);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgAlarmList, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgAlarmList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ALARM, OnItemchangedListAlarm)
	ON_BN_CLICKED(IDC_CHECK_ACTIVITY, OnCheckActivity)
	ON_BN_CLICKED(IDC_CHECK_1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK_2, OnCheck2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALARM, OnDblclkListAlarm)
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_PARENTNOTIFY()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmList message handlers
void CDlgAlarmList::OnCancel() 
{
	EndDialog(IDCANCEL);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnOK() 
{
	EndDialog(IDOK);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgAlarmList::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgAlarmList::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	m_ListAlarm.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ListAlarm.ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	GetDlgItem(IDC_CHECK_ACTIVITY)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	GetDlgItem(IDC_CHECK_1)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	GetDlgItem(IDC_CHECK_2)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);

	RemoveFromResize(IDC_LIST_ALARM);

	if (!m_pWndAlarmList->GetServer()->IsDTS())
	{
		CWnd* pWnd = GetDlgItem(IDC_CHECK_2);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::CreateColumnHeader(const CIPCFields& fields)
{
	int nColumnCount = m_ListAlarm.GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_ListAlarm.DeleteColumn(0);
	}

	if (fields.GetSize()>0)
	{
		CRect rect;
		m_ListAlarm.GetClientRect(rect);

		CString s;
		// Calculate ListControl unit to match coloumn width
		int c = fields.GetSize();
		if (NULL!=fields.GetCIPCField(_T("DBP_MS")))
		{
			c--;
		}
		int iWidth = rect.Width()/c;

		for (int i=0;i<fields.GetSize();i++)
		{
			CIPCField* pField = fields.GetAtFast(i);
			if (pField)
			{
				if (pField->GetName() != _T("DBP_MS"))
				{
					CreateColumnHeader(pField,iWidth);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::CreateColumnHeader(CIPCField* pField, int iWidth)
{
	CString s;

	if (   pField->GetName() == _T("DVR_TYP")
		|| pField->GetName() == _T("DBP_TYP"))
	{
		s.LoadString(IDS_TYP);
	}
	else if (pField->GetName() == _T("DVR_DATE")
		||pField->GetName() == _T("DBP_DATE"))
	{
		s.LoadString(IDS_DATE);
	}
	else if (pField->GetName() == _T("DVR_TIME")
		|| pField->GetName() == _T("DBP_TIME"))
	{
		s.LoadString(IDS_TIME);
	}
	else if (pField->GetName() == _T("DBP_ARCNR"))
	{
		s.LoadString(IDS_ARCHIVE);
	}
	else if (pField->GetName() == _T("DBP_CANR"))
	{
		s.LoadString(IDS_CAM_NAME);
	}
	else if (pField->GetName() == _T("DBP_INPNR"))
	{
		s.LoadString(IDS_INPUT);
	}
	else
	{
		s = theApp.GetMappedString(pField->GetName());
	}

	m_ListAlarm.InsertColumn(m_ListAlarm.GetHeaderCtrl()->GetItemCount(),s,LVCFMT_LEFT,iWidth);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::DeleteAlarms()
{
	m_ListAlarm.DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::AddAlarm(const CString& sTyp, const CSystemTime& st, const CStringArray& sa)
{
	if (   (sTyp == _T("A") && m_bActivity)
		|| (sTyp == _T("1") && m_bDetector1)
		|| (sTyp == _T("2") && m_bDetector2)
		)
	{
		int i = m_ListAlarm.InsertItem(m_ListAlarm.GetItemCount(),sTyp);
		CString sDate = st.GetDate();
		CString sTime = st.GetTime();
		if (st.wMilliseconds>0)
		{
			CString s;
			s.Format(_T("%03d"),st.wMilliseconds);
			sTime += _T(",") + s;
		}
		m_ListAlarm.SetItemText(i,1,sDate);
		m_ListAlarm.SetItemText(i,2,sTime);

		for (int j=0;j<sa.GetSize();j++)
		{
			m_ListAlarm.SetItemText(i,j+3,sa[j]);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CDlgAlarmList::GetNrOfAlarms()
{
	return m_ListAlarm.GetItemCount();
}
/////////////////////////////////////////////////////////////////////////////
int CDlgAlarmList::GetCurSelAlarm()
{
	return m_iSelectedItem;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnItemchangedListAlarm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_iSelectedItem = pNMListView->iItem;
	TRACE(_T("Cur sel is %d\n"),m_iSelectedItem);
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgAlarmList::GetAlarm(int i, CString& sTyp, CSystemTime& st, CStringArray& saRest)
{
	CString sDate,sTime;
	if (GetAlarm(i, sTyp, sTime, sDate, saRest))
	{
		st.SetDBDate(sDate);
		st.SetDBTime(sTime);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgAlarmList::GetAlarm(int i, CString& sTyp, CString& sTime, CString& sDate, CStringArray& saRest)
{
	if (IsBetween(i, 0, m_ListAlarm.GetItemCount()-1))
	{
		CString s;

		s = m_ListAlarm.GetItemText(i,0);
		sTyp = s;
		s = m_ListAlarm.GetItemText(i,1);
		sDate = s.Right(4) + s.Mid(3,2) + s.Left(2);
		s = m_ListAlarm.GetItemText(i,2);
		sTime = s.Left(2) + s.Mid(3,2) + s.Mid(6,2);
		int j, nColumns = m_ListAlarm.GetHeaderCtrl()->GetItemCount();
		for (j=3; j<nColumns; j++)
		{
			s = m_ListAlarm.GetItemText(i, j);
			saRest.Add(s);
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgAlarmList::GetHeaders(CStringArray& saHeaders)
{
	int j, nColumns =m_ListAlarm.GetHeaderCtrl()->GetItemCount();
	CString s;
	HDITEM hdi;
	for (j=0; j<nColumns; j++)
	{
		hdi.mask = HDI_TEXT;
		hdi.pszText = s.GetBufferSetLength(MAX_PATH);
		hdi.cchTextMax = MAX_PATH;
		m_ListAlarm.GetHeaderCtrl()->GetItem(j, &hdi);
		s.ReleaseBuffer();
		saHeaders.Add(s);
	}
	return (nColumns > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnCheckActivity() 
{
	UpdateData();
	ASSERT(m_pWndAlarmList);
	m_pWndAlarmList->SendMessage(WM_COMMAND, ID_VIEW_REFRESH);
	m_pWndAlarmList->SendMessage(WM_RBUTTONDOWN, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnCheck1() 
{
	UpdateData();
	ASSERT(m_pWndAlarmList);
	m_pWndAlarmList->SendMessage(WM_COMMAND, ID_VIEW_REFRESH);
	m_pWndAlarmList->SendMessage(WM_RBUTTONDOWN, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnCheck2() 
{
	UpdateData();
	ASSERT(m_pWndAlarmList);
	m_pWndAlarmList->SendMessage(WM_COMMAND, ID_VIEW_REFRESH);
	m_pWndAlarmList->SendMessage(WM_RBUTTONDOWN, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnDblclkListAlarm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_pWndAlarmList->PostMessage(WM_COMMAND,ID_NAVIGATE_FORWARD);
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
int CDlgAlarmList::OptimizeColumnWidth(CDC*pDC/*=NULL*/, CDWordArray* pdwaWidth/*=NULL*/)
{
	CRect rc;
	CString sText;
	int i, j, nItems, nColumns, nWidth, nMinWidth, nTotal, nRest, nSpace = 0;
	int *pnWidth = NULL;
	BOOL	bOptimizeByContent  = m_pWndAlarmList->m_OptimizeColumnWidth.lh.wLow == OPTIMIZE_BY_CONTENT ? TRUE : FALSE;
	BOOL	bOptimizeToWndWidth = m_pWndAlarmList->m_OptimizeColumnWidth.lh.wHigh & OPTIMIZE_TO_WND_WIDTH ? TRUE : FALSE;
	TCHAR szColumn[64];
	LVCOLUMN lvc;
	ZERO_INIT(lvc);
	lvc.mask = LVCF_TEXT;
	lvc.pszText = szColumn;
	lvc.cchTextMax = 64;

	m_ListAlarm.GetClientRect(&rc);
	if (pDC && pdwaWidth)
	{
		bOptimizeByContent  = TRUE;
		bOptimizeToWndWidth = FALSE;
		rc.left = 0;
		rc.right = 0;
		CSize sz = pDC->GetOutputTextExtent(_T("##"));
		nSpace = sz.cx;
	}

	nRest = rc.Width();
	nTotal = 0;
	nItems = m_ListAlarm.GetItemCount();
	nColumns = m_ListAlarm.GetHeaderCtrl()->GetItemCount();
	pnWidth = new int[nColumns];

	for (j=0; j<nColumns; j++)
	{
		m_ListAlarm.GetColumn(j, &lvc);
		if (bOptimizeByContent)
		{
			nMinWidth = 0;
			for (i=0; i<nItems; i++)
			{
				sText = m_ListAlarm.GetItemText(i, j);
				sText.TrimRight();
				if (!sText.IsEmpty())
				{
					if (pDC)
					{
						CSize sz = pDC->GetOutputTextExtent(sText);
						nWidth = sz.cx;
					}
					else
					{
						nWidth = m_ListAlarm.GetStringWidth(sText);
					}
					nMinWidth = max(nWidth, nMinWidth);
				}
			}
		}
		else
		{
			nMinWidth = 1;
		}
		if (nMinWidth)
		{
			if (pDC)
			{
				nMinWidth += nSpace;
				CSize sz = pDC->GetOutputTextExtent(szColumn, _tcslen(szColumn));
				nWidth = sz.cx + nSpace;
			}
			else
			{
				nMinWidth += 15;
				nWidth = m_ListAlarm.GetStringWidth(szColumn) + 10;
			}
			nMinWidth = max(nWidth, nMinWidth);
		}
		else
		{
			nMinWidth = 0;
		}
		pnWidth[j] = nMinWidth;
		nTotal += nMinWidth;
		if (j<nColumns-1)
		{
			nRest  -= nMinWidth;
		}
	}

	nRest = rc.Width();
	if (bOptimizeToWndWidth || nTotal < nRest)
	{
		for (j=0; j<nColumns; j++)
		{
			pnWidth[j] = MulDiv(pnWidth[j], nRest, nTotal);
		}
	}
	if (pDC && pdwaWidth)
	{
		for (j=0; j<nColumns; j++)
		{
			pdwaWidth->Add(pnWidth[j]);
		}
	}
	else
	{
		for (j=0; j<nColumns; j++)
		{
			m_ListAlarm.SetColumnWidth(j, pnWidth[j]);
		}
	}
	WK_DELETE(pnWidth);
	return nTotal;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnSize(UINT nType, int cx, int cy)
{
	CSkinDialog::OnSize(nType, cx, cy);
	CWnd *pWnd = GetDlgItem(IDC_CHECK_ACTIVITY);
	if (pWnd)
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.bottom += 5;
		m_ListAlarm.MoveWindow(0, rc.bottom, cx, cy - rc.bottom);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnRButtonDown(UINT nFlags, CPoint point)
{
	CSkinDialog::OnRButtonDown(nFlags, point);
	MapWindowPoints(m_pWndAlarmList, &point, 1);
	m_pWndAlarmList->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnRButtonUp(UINT nFlags, CPoint point)
{
	CSkinDialog::OnRButtonUp(nFlags, point);
	m_pWndAlarmList->SendMessage(WM_COMMAND, ID_SMALL_CONTEXT, (LPARAM)m_hWnd);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgAlarmList::OnParentNotify(UINT message, LPARAM lParam)
{
	CSkinDialog::OnParentNotify(message, lParam);
	if (message == WM_RBUTTONDOWN)
	{
		m_pWndAlarmList->SendMessage(WM_RBUTTONDOWN, MK_RBUTTON, lParam);
		m_pWndAlarmList->PostMessage(WM_COMMAND, ID_SMALL_CONTEXT, (LPARAM)m_hWnd);
	}
}
