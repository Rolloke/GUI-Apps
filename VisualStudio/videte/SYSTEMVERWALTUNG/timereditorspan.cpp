/* GlobalReplace: CTimerEditorDlg --> CTimerEditorPage */
// TimerEditorSpan.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "TimerEditorSpan.h"

#include "TimerEditorPage.h"

#include "TimerSetDialog.h"
#include "WKClasses/WK_String.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorSpan dialog
CTimerEditorSpan::CTimerEditorSpan(CTimerEditorPage * pParent)
	: CWK_Dialog(CTimerEditorSpan::IDD, pParent)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CTimerEditorSpan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pSelectedTimer = NULL;

	Create(IDD,m_pParent);	// CAVEAT order is important, calls OnInitDialog
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerEditorSpan)
	DDX_Control(pDX, IDC_SPAN_REMOVE, m_buttonSpanRemove);
	DDX_Control(pDX, IDC_SPAN_EDIT, m_buttonSpanEdit);
	DDX_Control(pDX, IDC_SPAN_ADD, m_buttonSpanAdd);
	DDX_Control(pDX, IDC_TIMER_SPAN_LIST, m_listSpans);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTimerEditorSpan, CWK_Dialog)
	//{{AFX_MSG_MAP(CTimerEditorSpan)
	ON_BN_CLICKED(IDC_SPAN_EDIT, OnSpanEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_SPAN_LIST, OnItemchangedTimerSpanList)
	ON_BN_CLICKED(IDC_SPAN_REMOVE, OnTimeSpanRemove)
	ON_BN_CLICKED(IDC_SPAN_ADD, OnTimeSpanAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_TIMER_SPAN_LIST, OnDblclkTimerSpanList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CTimerEditorSpan message handlers
void CTimerEditorSpan::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::OnOK()
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::OnCancel()
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTimerEditorSpan::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	m_ixSelectedSpan = -1;

	DWORD dw = ListView_GetExtendedListViewStyle(m_listSpans);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listSpans,dw);

	CRect rect;
	m_listSpans.GetWindowRect(rect);
	ScreenToClient(rect);

	// NOT YET via LoadString
	CString sText1, sText2;
	sText1.LoadString(IDS_WEEKDAY);
	sText2.LoadString(IDS_DATE);
	m_listSpans.InsertColumn(0, sText1 + " / " + sText2, LVCFMT_LEFT, rect.Width()/4);

	sText1.LoadString(IDS_HOURS);
	m_listSpans.InsertColumn(1, sText1, LVCFMT_LEFT, (3*rect.Width()/4)-4);

//	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::FillFromTimer(CSecTimer *pTimer)
{
	if (pTimer==NULL)
	{
		WK_TRACE_ERROR(_T("NULL timer\n"));
		return;
	}

	m_pSelectedTimer = pTimer;

	m_listSpans.DeleteAllItems();

	// fill fom timer
//	pTimer->CalcTotalSpans(*m_pParent->m_pTimers);	// OOPS realy required ?
	const CSecTimeSpanArray & spans = pTimer->GetTotalSpans();

	// check for weekdays first
	// loop over possible "days" first
	for (int d=0 ; d<10 ; d++)
	{
		// CAVEAT: SWITCH block generates order of days in list view
		// Set the seeked iIndexOfDay for loop index, will be compared with iDayIndex of timer span
		// sFullday will be displayed in GUI only
		int iIndexOfDay = -1;
		CString sFullDay;
		switch (d)
		{
			case 0:	iIndexOfDay = 2;	sFullDay.LoadString(IDS_MONDAY);	break;
			case 1:	iIndexOfDay = 3;	sFullDay.LoadString(IDS_TUESDAY);	break;
			case 2:	iIndexOfDay = 4;	sFullDay.LoadString(IDS_WEDNESDAY);	break;
			case 3:	iIndexOfDay = 5;	sFullDay.LoadString(IDS_THURSDAY);	break;
			case 4:	iIndexOfDay = 6;	sFullDay.LoadString(IDS_FRIDAY);	break;
			case 5:	iIndexOfDay = 7;	sFullDay.LoadString(IDS_SATURDAY);	break;
			case 6:	iIndexOfDay = 1;	sFullDay.LoadString(IDS_SUNDAY);	break;
			case 7:	iIndexOfDay = 0;	sFullDay.LoadString(IDS_DAILY);		break;
			case 8:	iIndexOfDay = 11;	sFullDay.LoadString(IDS_MO_FR);		break;
			case 9:	iIndexOfDay = 12;	sFullDay.LoadString(IDS_SA_SU);		break;
			default:iIndexOfDay = -1;	sFullDay=_T("??");						break;
		}
		CString sSpans;

		// loop over spans
		for (int i=0 ; i<spans.GetSize() ; i++)
		{
			CSecTimeSpan *pSpan= spans[i];

			CString sStart = pSpan->GetStartString();
			// weekdays and combinations have no digits as first char
			if (isdigit(sStart[0]) == FALSE)
			{
				CString sEnd = pSpan->GetEndString();
				int iSeperator = sStart.Find (_T(':'));
				CString sSpanDay=sStart.Left(iSeperator);
				
				int iDayIndex = -1;

				if (sSpanDay.IsEmpty() == FALSE)
				{
					iDayIndex = CSecTimeSpan::DayNrFromString(sSpanDay);
					// a span for that day ?
					if (   (iDayIndex != -1)
						&& (iDayIndex == iIndexOfDay)
						)	// a span for that day ?
					{
						if (sSpans.GetLength())
						{
							sSpans += _T(" , ");
						}
						
						// skip day like MO: (by adjusting pointer! - scary...)
						sSpans += (LPCTSTR)(pSpan->GetStartString()) + iSeperator + 1;
						sSpans += _T("-");
						sSpans += (LPCTSTR)(pSpan->GetEndString()) + iSeperator + 1;
					}
				}
			}
		}
		// Insert time span always, even if no time,is checked at saving
		if (sSpans.GetLength())
		{
			int ix = m_listSpans.GetItemCount();
			ix = m_listSpans.InsertItem(ix, sFullDay);
			m_listSpans.SetItemText(ix, 1,sSpans);
			// CAVEAT tereis no itemData associated, it's just a plain string interface
		} // end of loof over spans
	} // end ef loop over possible "days"

	// than add specific dates
	CWK_Strings alreadyHandledDates;

	for (int i=0 ; i<spans.GetSize() ; i++)
	{
		CSecTimeSpan *pSpan= spans[i];
		CString sStart = pSpan->GetStartString();

// ML 17.8.99		if (isdigit(sStart[0])) {
		if (!sStart.IsEmpty() && isdigit(sStart[0]))
		{
			// a specific date looks like this '20.11.1998,01:00-09:00'
			CString sSpans;
			CString sDate;

			int ix=0;
			// slice day.month.year
			ix = sStart.Find(_T('.'));
			sDate += sStart.Left(ix);
			sDate += _T('.');
			sStart = sStart.Mid(ix+1);

			ix = sStart.Find(_T('.'));
			sDate += sStart.Left(ix);
			sDate += _T('.');
			sStart = sStart.Mid(ix+1);

			// NOT YET convert **** to *
			ix = sStart.Find(_T(','));
			sDate += sStart.Left(ix);

			if (alreadyHandledDates.Contains(sDate)==FALSE)
			{
				// first time encountered
				if (sStart.IsEmpty())
				{
					// no span given
					sSpans = _T("0:00-0:00");
				}
				else 
				{
					// read the given span
					sSpans= sStart.Mid(ix+1);
					sSpans += _T('-');

					// also needs end times
					CString sEnd = pSpan->GetEndString();
					ix = sEnd.Find(_T(','));
					if ( ix != -1)
					{
						sSpans += sEnd.Mid(ix+1);
					}
					else
					{
						WK_TRACE_ERROR(_T("Missing , in end time '%s'\n"),sEnd);
						sSpans = _T("0:00-0:00");
					}
				}

				// now loop over all spans, there might be more spans for this day
				// it's okay to search the rest, since the first date is used
				for (int j=i+1;j<spans.GetSize();j++)
				{
					pSpan= spans[j];
					sStart = pSpan->GetStartString();
					if (sStart.Find(sDate)==0)
					{
						// another span for the current date
						ix = sStart.Find(_T(','));
						// first time encountered
						if (sStart.IsEmpty())
						{
							// no span given
							// OOPS should not happen
						}
						else
						{
							// read the given span
							sSpans += _T(" , ");
							sSpans += sStart.Mid(ix+1);
							sSpans += _T('-');

							// also needs end times
							CString sEnd = pSpan->GetEndString();
							ix = sEnd.Find(_T(','));
							if ( ix != -1)
							{
								sSpans += sEnd.Mid(ix+1);
							}
							else
							{
								WK_TRACE_ERROR(_T("Missing , in end time '%s'\n"),sEnd);
								sSpans = _T("0:00-0:00");
							}
						}

					}
					else
					{
						// some other date ? OOPS
					}
				} // end of loop over remaining spans

				// remember
				alreadyHandledDates.Add(new CWK_String(sDate));

				// also needs end times
				ix = m_listSpans.GetItemCount();
				ix = m_listSpans.InsertItem(ix, sDate);
				m_listSpans.SetItemText(ix, 1,sSpans);
				// CAVEAT tereis no itemData associated, it's just a plain string interface
			} else
			{
				// date already handled
			}
		}
		else
		{
			// non-digit at the start, no date format, has to be a weekday
		}
	}

	// select the first span
	if (m_listSpans.GetItemCount())
	{
		DWORD dwState = m_listSpans.GetItemState(0, LVIS_SELECTED);
		dwState |= LVIS_SELECTED;
		m_listSpans.SetItemState(0,dwState, LVIS_SELECTED);
	}

	UpdateActionButtons();
}
/////////////////////////////////////////////////////////////////////////////
CString LocalizedWeekDayToInternal(CString sDay)
{
	CString sResult(sDay);
	CString sFullDay;

	sFullDay.LoadString(IDS_MONDAY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("MO");
	}
	sFullDay.LoadString(IDS_TUESDAY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("DI");
	}
	sFullDay.LoadString(IDS_WEDNESDAY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("MI");
	}
	sFullDay.LoadString(IDS_THURSDAY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("DO");
	}
	sFullDay.LoadString(IDS_FRIDAY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("FR");
	}
	sFullDay.LoadString(IDS_SATURDAY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("SA");
	}
	sFullDay.LoadString(IDS_SUNDAY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("SO");
	}

	sFullDay.LoadString(IDS_DAILY);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("**");
	}

	sFullDay.LoadString(IDS_MO_FR);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("MO_FR");
	}

	sFullDay.LoadString(IDS_SA_SU);
	if (sFullDay.CompareNoCase(sDay)==0) {
		sResult = _T("SA_SU");
	}

	return sResult;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::UpdateSelectedTimer()
{
	if (m_pSelectedTimer==NULL) {
		return;	// <<< EXIT >>>
	}

	// since we can not easily delete a specific span, rebuild them all
	m_pSelectedTimer->RemoveAllTimeSpans();
	for (int i=0;i<m_listSpans.GetItemCount();i++) {	// loop over all entries
		// each entry can have a comma separated list
		CString sDay = m_listSpans.GetItemText(i,0);	// left column the day/date
		// convert localized strings to internal representation
		CString sSpans = m_listSpans.GetItemText(i,1);

		// split the list of spans
		CWK_Strings spans;
		spans.SplitArgs(sSpans,_T(','),TRUE);
		spans.TrimLeft();
		spans.TrimRight();

		CString sStart,sEnd;
		for (int s=0;s<spans.GetSize();s++) {
			// split a single span in its start and end time
			CWK_Strings spanParts;
			spanParts.SplitArgs(*spans[s],_T('-'),TRUE);
//ML 17.8.99	if (isdigit(sDay[0])==FALSE) {
			if (!sDay.IsEmpty() && isdigit(sDay[0])==FALSE) {
				sDay = LocalizedWeekDayToInternal(sDay);
				// week day
				sStart = sDay + _T(':') + *spanParts[0];
				sEnd =  sDay + _T(':') + *spanParts[1];
			} else {
				sStart = sDay + _T(',') + *spanParts[0];
				sEnd =  sDay + _T(',') + *spanParts[1];
			}
			CTime ct = CTime::GetCurrentTime();
			CSecTimeSpan span (ct,sStart,sEnd);
			if (span.IsValid()) {
				m_pSelectedTimer->AddTimeSpan(span);
			} else {
				WK_TRACE_ERROR(_T("Invalid timespan %s-%s\n"),sStart,sEnd);
			}
		}
	}	// end of loop over spans in list
	m_pSelectedTimer->CalcTotalSpans(*m_pParent->m_pTimers);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::OnSpanEdit() 
{
	if (m_ixSelectedSpan != -1 && m_pSelectedTimer) {
		CString sTimes;
		sTimes = m_listSpans.GetItemText(m_ixSelectedSpan,1);
		CTimerSetDialog dlg;
		dlg.FillHoursFromString(sTimes);
		dlg.SetTimerName(m_pSelectedTimer->GetName());
		
		CString sDay = m_listSpans.GetItemText(m_ixSelectedSpan,0);
		dlg.SetDayType(sDay);

		// OOPS bring to top ?
		int iResult = dlg.DoModal();
		if (iResult==IDOK) {
			// get the modified data from the dialog
			m_listSpans.SetItemText(m_ixSelectedSpan,1,dlg.GetHoursAsString());
			CString sDay = dlg.GetDayAsString();
			m_listSpans.SetItemText(m_ixSelectedSpan,0,dlg.GetDayAsString());
			// modify/update the current Timer via 
			UpdateSelectedTimer();
			m_pParent->SetModified(TRUE,TRUE);	// needs a server reset too
		}
		else if (sTimes.IsEmpty())
		{
			RemoveTimeSpan(FALSE);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("InternalError: span %d, timer %08x\n"),
						m_ixSelectedSpan,(DWORD)m_pSelectedTimer);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::OnItemchangedTimerSpanList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// get the selected item
	if (pNMListView->uNewState & LVIS_SELECTED) {
		m_ixSelectedSpan = pNMListView->iItem;
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::OnTimeSpanRemove() 
{
	RemoveTimeSpan(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::OnTimeSpanAdd() 
{
	// NOT YET do not add duplicateentries
	// so scan the existing entries first

	// CAVEAT redundannt data for GUI and internal data
//	CString sStart(_T("Mo:08:00"));	//	the internal representation
//	CString sEnd(_T("Mo:09:00"));
//	CString sSpan(_T("8:00-9:00"));
	CString sStart;	//	the internal representation
	CString sEnd;
	CString sSpan;

	CString sDay;
	sDay.LoadString(IDS_MONDAY);	// the name for the GUI (localized)
	int ix = m_listSpans.InsertItem(m_listSpans.GetItemCount(), sDay);
	m_listSpans.SetItemText(ix, 1,sSpan);

	m_ixSelectedSpan = ix;

	DWORD dwState = m_listSpans.GetItemState(m_ixSelectedSpan, LVIS_SELECTED);
	dwState |= LVIS_SELECTED;
	m_listSpans.SetItemState(m_ixSelectedSpan,dwState, LVIS_SELECTED);
	m_listSpans.SetFocus();
	UpdateActionButtons();

	if (m_pSelectedTimer) {
		CTime ct = CTime::GetCurrentTime();
		CSecTimeSpan newSpan(ct,sStart,sEnd);
		m_pSelectedTimer->AddTimeSpan(newSpan);
	}
	UpdateSelectedTimer();
	OnSpanEdit();
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::UpdateActionButtons()
{
	BOOL bEnable = (m_pSelectedTimer != NULL);
	m_buttonSpanAdd.EnableWindow(bEnable);

	bEnable &= m_listSpans.GetItemCount()>0;
	
	m_buttonSpanRemove.EnableWindow(bEnable);
	m_buttonSpanEdit.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTimerEditorSpan::StretchElements()
{
	// return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::OnDblclkTimerSpanList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// get the selected item
	if (pNMListView->iItem == m_ixSelectedSpan)
	{
		if (   (m_pSelectedTimer != NULL)
			&& (m_ixSelectedSpan != -1)
			)
		{
			OnSpanEdit();
		}
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorSpan::RemoveTimeSpan(BOOL bModified)
{
	// all data is in the ...
	//m_listSpans.Setc
	m_listSpans.DeleteItem(m_ixSelectedSpan);
	m_listSpans.SetFocus();

	int iCount = m_listSpans.GetItemCount();	// so what is left
	if (iCount) {
		if (m_ixSelectedSpan>=iCount) {
			m_ixSelectedSpan =  m_listSpans.GetItemCount()-1;
		}
		DWORD dwState = m_listSpans.GetItemState(m_ixSelectedSpan, LVIS_SELECTED);
		dwState |= LVIS_SELECTED;
		m_listSpans.SetItemState(m_ixSelectedSpan,dwState, LVIS_SELECTED);
	} else {
		// handle empty list and buttons (edit/remove disabled)
		m_ixSelectedSpan = -1;
	}

	UpdateActionButtons();
	UpdateSelectedTimer();
	if (bModified)
	{
		m_pParent->SetModified(TRUE,TRUE);	// needs a server reset too
	}
}
