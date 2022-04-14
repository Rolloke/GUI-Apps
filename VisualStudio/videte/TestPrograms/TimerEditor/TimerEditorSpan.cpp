// TimerEditorSpan.cpp : implementation file
//

#include "stdafx.h"
#include "timereditor.h"
#include "TimerEditorSpan.h"

#include "TimerEditorDlg.h"

#include "TimerSetDialog.h"
#include "WKClasses/WK_String.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorSpan dialog



CTimerEditorSpan::CTimerEditorSpan(CTimerEditorDlg * pParent)
	: CDialog(CTimerEditorSpan::IDD, pParent)
{
	m_pParent = NULL;
	//{{AFX_DATA_INIT(CTimerEditorSpan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pSelectedTimer = NULL;

	Create(IDD,m_pParent);	// CAVEAT order is important, calls OnInitDialog
}


void CTimerEditorSpan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerEditorSpan)
	DDX_Control(pDX, IDC_SPAN_REMOVE, m_buttonSpanRemove);
	DDX_Control(pDX, IDC_SPAN_EDIT, m_buttonSpanEdit);
	DDX_Control(pDX, IDC_SPAN_ADD, m_buttonSpanAdd);
	DDX_Control(pDX, IDC_TIMER_SPAN_LIST, m_listSpans);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimerEditorSpan, CDialog)
	//{{AFX_MSG_MAP(CTimerEditorSpan)
	ON_BN_CLICKED(IDC_SPAN_EDIT, OnSpanEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_SPAN_LIST, OnItemchangedTimerSpanList)
	ON_BN_CLICKED(IDC_SPAN_REMOVE, OnTimeSpanRemove)
	ON_BN_CLICKED(IDC_SPAN_ADD, OnTimeSpanAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorSpan message handlers
void CTimerEditorSpan::PostNcDestroy() 
{
	delete this;
}
void CTimerEditorSpan::OnOK()
{
	DestroyWindow();
}
void CTimerEditorSpan::OnCancel()
{
	DestroyWindow();
}


BOOL CTimerEditorSpan::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ixSelectedSpan = -1;
	
	CRect rect;
	m_listSpans.GetWindowRect(rect);
	ScreenToClient(rect);

	DWORD dw = ListView_GetExtendedListViewStyle(m_listSpans);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listSpans,dw);


	// NOT YET via LoadString
	m_listSpans.InsertColumn(0,"Wochentag / Datum",LVCFMT_LEFT,rect.Width()/4);
	m_listSpans.InsertColumn(1,"Stunden",LVCFMT_LEFT,(3*rect.Width()/4)-4);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CTimerEditorSpan::FillFromTimer(CSecTimer *pTimer)
{
	if (pTimer==NULL) {
		WK_TRACE_ERROR("NULL timer\n");
		return;
	}

	m_pSelectedTimer = pTimer;

	m_listSpans.DeleteAllItems();

	// fill fom timer
	const CSecTimeSpanArray & spans = pTimer->GetTotalSpans();

	int i=0;
	// loop over weekdays first
	for (int d=0;d<8;d++) {
		CString sSpans;
		
		// calc day in string format from index of loop
		CString sDay,sFullDay;

		switch (d) {
			// NOT YET localized
			case 0: sDay = "MO"; sFullDay="Mo"; break;
			case 1: sDay = "DI"; sFullDay="Di"; break;
			case 2: sDay = "MI"; sFullDay="Mi"; break;
			case 3: sDay = "DO"; sFullDay="Do"; break;
			case 4: sDay = "FR"; sFullDay="Fr"; break;
			case 5: sDay = "SA"; sFullDay="Sa"; break;
			case 6: sDay = "SO"; sFullDay="So"; break;
			case 7: sDay = "DT"; sFullDay="De"; break;
			default:
				sDay = "??"; sFullDay="??"; 

		}


		for (i=0;i<spans.GetSize();i++) {
			CSecTimeSpan *pSpan= spans[i];

			CString sStart = pSpan->GetStartString();

			if (sStart.Find(sDay) != -1) {	// a span for that day ?
				if (sSpans.GetLength()) {
					sSpans +=" , ";
				}

				// skip MO:
				sSpans += ((const char*)pSpan->GetStartString())+3;
				sSpans += "-";
				sSpans += ((const char *)pSpan->GetEndString())+3;
			}
		}

		if (sSpans.GetLength()) {
			int ix = m_listSpans.GetItemCount();
			ix = m_listSpans.InsertItem(ix, sFullDay);
			m_listSpans.SetItemText(ix, 1,sSpans);
			// CAVEAT tereis no itemData associated, it's just a plain string interface
		}
	}	// end of loop over week days

	// than add specific dates
	CWK_Strings alreadyHandledDates;

	for (i=0;i<spans.GetSize();i++) {
		CSecTimeSpan *pSpan= spans[i];
		CString sStart = pSpan->GetStartString();

		if (isdigit(sStart[0])) {
			// a specific date looks like this '20.11.1998,01:00-09:00'
			CString sSpans;
			CString sDate;

			int ix=0;
			// slice day.month.year
			ix = sStart.Find('.');
			sDate += sStart.Left(ix);
			sDate += '.';
			sStart = sStart.Mid(ix+1);

			ix = sStart.Find('.');
			sDate += sStart.Left(ix);
			sDate += '.';
			sStart = sStart.Mid(ix+1);

			// NOT YET convert **** to *
			ix = sStart.Find(',');
			sDate += sStart.Left(ix);

			if (alreadyHandledDates.Contains(sDate)==FALSE) {
				// first time encountered
				if (sStart.IsEmpty()) {
					// no span given
					sSpans = "0:00-0:00";
				} else {
					// read the given span
					sSpans= sStart.Mid(ix+1);
					sSpans += '-';

					// also needs end times
					CString sEnd = pSpan->GetEndString();
					ix = sEnd.Find(',');
					if ( ix != -1) {
						sSpans += sEnd.Mid(ix+1);
					} else {
						WK_TRACE_ERROR("Missing , in end time '%s'\n",sEnd);
						sSpans = "0:00-0:00";
					}
				}

				// now loop over all spans, there might be more spans for this day
				// it's okay to search the rest, since the first date is used
				for (int j=i+1;j<spans.GetSize();j++) {
					pSpan= spans[j];
					sStart = pSpan->GetStartString();
					if (sStart.Find(sDate)==0) {
						// another span for the current date
						ix = sStart.Find(',');
						// first time encountered
						if (sStart.IsEmpty()) {
							// no span given
							// OOPS should not happen
						} else {
							// read the given span
							sSpans += " , ";
							sSpans += sStart.Mid(ix+1);
							sSpans += '-';

							// also needs end times
							CString sEnd = pSpan->GetEndString();
							ix = sEnd.Find(',');
							if ( ix != -1) {
								sSpans += sEnd.Mid(ix+1);
							} else {
								WK_TRACE_ERROR("Missing , in end time '%s'\n",sEnd);
								sSpans = "0:00-0:00";
							}
						}

					} else {
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
			} else {
				// date already handled
			}
	
			
		} else {
			// non-digit at the start, no date format, has to be a weekday
		}


	}



	// NOT YET

	// select the first span
	if (m_listSpans.GetItemCount()) {
		DWORD dwState = m_listSpans.GetItemState(0, LVIS_SELECTED);
		dwState |= LVIS_SELECTED;
		m_listSpans.SetItemState(0,dwState, LVIS_SELECTED);
	}


	UpdateActionButtons();
}

void CTimerEditorSpan::OnSpanEdit() 
{
	if (m_ixSelectedSpan != -1 && m_pSelectedTimer) {
		CString sTmp;
		sTmp = m_listSpans.GetItemText(m_ixSelectedSpan,1);
		CTimerSetDialog dlg;
		dlg.FillHoursFromString(sTmp);
		dlg.m_sTimerName = m_pSelectedTimer->GetName();
		
		CString sDay = sTmp = m_listSpans.GetItemText(m_ixSelectedSpan,0);
		dlg.SetDayType(sDay);

		int iResult = dlg.DoModal();
		if (iResult==IDOK) {
			// get the modified data from the dialog
			m_listSpans.SetItemText(m_ixSelectedSpan,1,dlg.GetHoursAsString());
			m_listSpans.SetItemText(m_ixSelectedSpan,0,dlg.GetDayAsString());
			// modify/update the current Timer via 
			if (m_pSelectedTimer) {
				for (int i=0;i<m_listSpans.GetItemCount();i++) {
					CString sDay = m_listSpans.GetItemText(m_ixSelectedSpan,0);
					CString sSpans = m_listSpans.GetItemText(m_ixSelectedSpan,1);

					TRACE("TRANSFER '%s:%s'\n",sDay,sSpans);
					CWK_Strings spans;
					spans.SplitArgs(sSpans,',',TRUE);

					/*
					CSecTimeSpan *pSpan = new CSecTimeSpan();
					m_pSelectedTimer->AddTimeSpan(*pSpan);
					*/
				}
			} else {
				WK_TRACE_ERROR("No selected timer !?\n");
			}
		}
	} else {
		WK_TRACE_ERROR("InternalError: span %d, timer %08x\n",m_ixSelectedSpan,(DWORD)m_pSelectedTimer);
	}
}

void CTimerEditorSpan::OnItemchangedTimerSpanList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	BOOL bEnableEdit=FALSE;
	BOOL bEnableRemove=FALSE;

	// get the selected item
	if (pNMListView->uNewState & LVIS_SELECTED) {
		m_ixSelectedSpan = pNMListView->iItem;
		TRACE("Span Item Selected %d\n",m_ixSelectedSpan);
	}

	*pResult = 0;
}

void CTimerEditorSpan::OnTimeSpanRemove() 
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
	}

	UpdateActionButtons();
}

void CTimerEditorSpan::OnTimeSpanAdd() 
{
	CString sDay ("Mo");	// OOPS
	CString sSpan("8:00-9:00");
	int ix = m_listSpans.InsertItem(m_listSpans.GetItemCount(), sDay);
	m_listSpans.SetItemText(ix, 1,sSpan);

	m_ixSelectedSpan = ix;

	DWORD dwState = m_listSpans.GetItemState(m_ixSelectedSpan, LVIS_SELECTED);
	dwState |= LVIS_SELECTED;
	m_listSpans.SetItemState(m_ixSelectedSpan,dwState, LVIS_SELECTED);
	m_listSpans.SetFocus();
}

void CTimerEditorSpan::UpdateActionButtons()
{
	int iCount = m_listSpans.GetItemCount();
	
	if (iCount) {
		m_buttonSpanRemove.EnableWindow(TRUE);
		m_buttonSpanEdit.EnableWindow(TRUE);
	} else {
		// no items no remove/edit
		m_buttonSpanRemove.EnableWindow(FALSE);
		m_buttonSpanEdit.EnableWindow(FALSE);
	}
}