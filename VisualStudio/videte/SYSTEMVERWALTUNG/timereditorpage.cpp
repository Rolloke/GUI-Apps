/* GlobalReplace: IDC_GRP_DUMMY --> IDC_TIMER_GRP_DUMMY */
// TimerEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "TimerEditorPage.h"
#include "TimerTestDialog.h"
#include "TimerImportDialog.h"

#include "SecTimer.h"
#include "WKClasses/WK_String.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// resturn th CSecTimeSpan index using localized names
int GetIndexByLocalizedWeekDayName(const CString &sDay)
{
	int ix= -1;
	CString sWeekDay;

	sWeekDay.LoadString(IDS_MONDAY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=0; } 
	sWeekDay.LoadString(IDS_TUESDAY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=1; } 
	sWeekDay.LoadString(IDS_WEDNESDAY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=2; } 
	sWeekDay.LoadString(IDS_THURSDAY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=3; }
	sWeekDay.LoadString(IDS_FRIDAY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=4; } 
	sWeekDay.LoadString(IDS_SATURDAY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=5; } 
	sWeekDay.LoadString(IDS_SUNDAY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=6; }
	sWeekDay.LoadString(IDS_DAILY);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=7; }
	sWeekDay.LoadString(IDS_MO_FR);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=8; }
	sWeekDay.LoadString(IDS_SA_SU);
	if (sDay.CompareNoCase(sWeekDay)==0) { ix=9; }

	return ix;
}
/////////////////////////////////////////////////////////////////////////////
static int GetSelectedItemFromList(const CListCtrl &listCtrl)
{
	int ix = -1;

	int iCount = listCtrl.GetItemCount();

	for (int i=0;ix==-1 && i<iCount;i++) {
		DWORD dwState = listCtrl.GetItemState(i,LVIS_SELECTED);

		if (dwState & LVIS_SELECTED) {
			ix = i;
		}
	}

	return ix;
}
/////////////////////////////////////////////////////////////////////////////
// CTimerEditorPage dialog
CTimerEditorPage::CTimerEditorPage(CSVView* pParent ) : CSVPage(CTimerEditorPage::IDD)
{
	m_pParent = pParent;
	m_pTimers = pParent->GetDocument()->GetTimers();
	//{{AFX_DATA_INIT(CTimerEditorPage)
	m_iTypeSelected = -1;
	m_sSelectedName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_pTimerEditorSpan = NULL;
	m_pTimerEditorCombi = NULL;
	m_pSelectedTimer = NULL;
	m_iNewCounter = 1;

	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::FillListBox()
{
	// fill timer list
	m_listTimer.DeleteAllItems();

	for (int i=0;i<m_pTimers->GetSize();i++) {
		CSecTimer *pTimer = m_pTimers->GetAt(i);
		int ix = m_listTimer.GetItemCount();
		ix = m_listTimer.InsertItem(ix, pTimer->GetName());
		m_listTimer.SetItemData(ix,(DWORD)pTimer);	// direct pointer no ID
	}

}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::EnableExceptNew()
{
	BOOL bEnable = (m_listTimer.GetItemCount() > 0);
	m_ctlSelectedName.EnableWindow(bEnable);
	m_listTimer.EnableWindow(bEnable);
	m_btnTimerTest.EnableWindow(bEnable);
	m_btnTimerExport.EnableWindow(bEnable);
	m_ctlLocks.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerEditorPage)
	DDX_Control(pDX, IDC_TIMER_EXPORT, m_btnTimerExport);
	DDX_Control(pDX, IDC_TIMER_DO_TEST, m_btnTimerTest);
	DDX_Control(pDX, IDC_TIMER_SELECTED_NAME, m_ctlSelectedName);
	DDX_Control(pDX, IDC_RADIO_TIMER_TYPE_SPAN, m_ctlSpanType);
	DDX_Control(pDX, IDC_RADIO_TIMER_TYPE_COMBINATION, m_ctlCombinationType);
	DDX_Control(pDX, IDC_TIMER_LOCKS, m_ctlLocks);
	DDX_Control(pDX, IDC_TIMER_GRP_DUMMY, m_groupDummy);
	DDX_Control(pDX, IDC_TIMER_LIST, m_listTimer);
	DDX_Radio(pDX, IDC_RADIO_TIMER_TYPE_SPAN, m_iTypeSelected);
	DDX_Text(pDX, IDC_TIMER_SELECTED_NAME, m_sSelectedName);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTimerEditorPage, CSVPage)
	//{{AFX_MSG_MAP(CTimerEditorPage)
	ON_BN_CLICKED(IDC_RADIO_TIMER_TYPE_COMBINATION, OnRadioTimerTypeCombination)
	ON_BN_CLICKED(IDC_RADIO_TIMER_TYPE_SPAN, OnRadioTimerTypeSpan)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_LIST, OnItemchangedTimerList)
	ON_LBN_SELCHANGE(IDC_TIMER_LOCKS, OnSelchangeTimerLocks)
	ON_EN_UPDATE(IDC_TIMER_SELECTED_NAME, OnUpdateTimerSelectedName)
	ON_EN_CHANGE(IDC_TIMER_SELECTED_NAME, OnChangeTimerSelectedName)
	ON_BN_CLICKED(IDC_TIMER_DO_TEST, OnTimerDoTest)
	ON_BN_CLICKED(IDC_TIMER_EXPORT, OnTimerExport)
	ON_BN_CLICKED(IDC_TIMER_IMPORT, OnTimerImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CTimerEditorPage message handlers
BOOL CTimerEditorPage::OnInitDialog()
{
	// create both dialog to enable the WK_Dialog stretching
	ShowSpanEditor();
	// save m_pTimerEditorSpan to avoid destroy at ShowCombiEditor
	CTimerEditorSpan*	tmpTimerEditorSpan = m_pTimerEditorSpan;
	m_pTimerEditorSpan = NULL;	// avoid delete
	ShowCombiEditor();

	CSVPage::OnInitDialog();	// will also call InitSizes() which is used by StretchElements

	// now kill the dialogs the according one is creted later on
	if (m_pTimerEditorCombi) {
		m_pTimerEditorCombi->DestroyWindow();
		m_pTimerEditorCombi = NULL;
	}
	m_pTimerEditorSpan=tmpTimerEditorSpan;
	if (m_pTimerEditorSpan) {
		m_pTimerEditorSpan->DestroyWindow();
		m_pTimerEditorSpan = NULL;
	}

	CRect rect;
	m_listTimer.GetWindowRect(rect);
	ScreenToClient(rect);

	// set special styles
	DWORD dw = ListView_GetExtendedListViewStyle(m_listTimer);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listTimer,dw);

	// fill the ListCtrl
	CString sName;
	sName.LoadString(IDS_NAME);
	m_listTimer.InsertColumn(0,sName,LVCFMT_LEFT,rect.Width()-4);	// OOPS guessed border/frame width

	CancelChanges();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::FillLockListBox()
{
	// fill the lock list m_ctlLocks
	CInputList *pInputList	= m_pParent->GetDocument()->GetInputs();
	m_ctlLocks.ResetContent();
	for (int i=0;i<pInputList->GetSize();i++) {
		CInputGroup* pGrp = pInputList->GetGroupAt(i);
		if (pGrp) {
			for (int iInp=0 ; iInp<pGrp->GetSize(); iInp++)
			{
				const CInput *pInput=pGrp->GetInput(iInp);
				if (pInput->GetIDActivate()==SECID_ACTIVE_LOCK)
				{
					int ix = m_ctlLocks.GetCount();
					ix = m_ctlLocks.InsertString(ix, pInput->GetName());
					m_ctlLocks.SetItemData (ix, pInput->GetID().GetID());	// id as ItemData
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnRadioTimerTypeCombination() 
{
	if (m_iTypeSelected != 1) {
		int nResponse = AfxMessageBox(IDS_TIMER_MODE_CHANGE,
						MB_YESNO | MB_ICONQUESTION
						);

		if (nResponse == IDYES) {
			// NOT YET remove all spans
			UpdateData();
			SetModified(TRUE,TRUE);	// needs server reset
			ShowCombiEditor();
		} else {
			m_iTypeSelected = 0;
			UpdateData(FALSE);
			// editor remains
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnRadioTimerTypeSpan() 
{
	if (m_iTypeSelected != 0) {
		// CTimerWarning dlg;
		int nResponse = AfxMessageBox(IDS_TIMER_MODE_CHANGE,
						MB_YESNO | MB_ICONQUESTION
						);
		if (nResponse == IDYES) {
			UpdateData();
			SetModified(TRUE,TRUE);	// needs server reset
			ShowSpanEditor();
		} else {
			m_iTypeSelected = 1;
			UpdateData(FALSE);
			// editor remains
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CTimerEditorPage::~CTimerEditorPage()
{
	// CAVEAT can't do that here it's global data
	// m_timers.DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::ShowSpanEditor()
{
	if (m_pTimerEditorCombi) {
		m_pTimerEditorCombi->DestroyWindow();
		m_pTimerEditorCombi = NULL;
	}
	
	// also update the radio buttons
	m_iTypeSelected = 0;
	UpdateData(FALSE);

	if (m_pTimerEditorSpan==NULL) {
		m_pTimerEditorSpan = new  CTimerEditorSpan(this);
		CRect rect;

		m_groupDummy.GetWindowRect(rect);
		ScreenToClient(rect);
		m_pTimerEditorSpan->MoveWindow(rect);
		m_pTimerEditorSpan->ShowWindow(SW_SHOW);
	}

	int ix = GetSelectedItemFromList(m_listTimer);
	if (ix != -1) {
		CSecTimer *pTimer = (CSecTimer *)m_listTimer.GetItemData(ix);
		m_pTimerEditorSpan->FillFromTimer(pTimer);
	}
	else {
		WK_TRACE_ERROR(_T("InternalError: ShowSpanEditor NO Timer selected\n"));
	}

	m_pTimerEditorSpan->UpdateActionButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::ShowCombiEditor()
{
	if (m_pTimerEditorSpan)
	{
		m_pTimerEditorSpan->DestroyWindow();
		m_pTimerEditorSpan = NULL;
	}

	// also update the radio buttons
	m_iTypeSelected = 1;
	UpdateData(FALSE);

	if (m_pTimerEditorCombi==NULL) {
		m_pTimerEditorCombi = new CTimerEditorCombi(this);
		CRect rect;
		m_groupDummy.GetWindowRect(rect);
		ScreenToClient(rect);
		m_pTimerEditorCombi->MoveWindow(rect);
		m_pTimerEditorCombi->ShowWindow(SW_SHOW);
	}

	int ix = GetSelectedItemFromList(m_listTimer);
	if (ix != -1) {
		CSecTimer *pTimer = (CSecTimer *)m_listTimer.GetItemData(ix);
		m_pTimerEditorCombi->FillFromTimer(pTimer,*m_pTimers);
	}
	else {
		WK_TRACE_ERROR(_T("InternalError: ShowCombiEditor NO Timer selected\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
int CTimerEditorPage::GetIndexOfTimer(CSecTimer *pTimer)
{

	LVFINDINFO findInfo;

	memset(&findInfo,0,sizeof(findInfo));

	findInfo.flags= LVFI_PARAM;
    findInfo.lParam = (DWORD)pTimer;

	int ix = m_listTimer.FindItem(&findInfo,-1);
	return ix;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::SelectTimerByIndex(CSecTimer *pTimer)
{

	int ix = GetIndexOfTimer(pTimer);
	if (ix != -1) {
		DWORD dwState = m_listTimer.GetItemState(ix, LVIS_SELECTED);
		dwState |= LVIS_SELECTED;
		m_listTimer.SetItemState(ix,dwState, LVIS_SELECTED);	// will cause a SelectTimer
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::SelectTimer(CSecTimer *pTimer)
{
	m_pSelectedTimer = pTimer;

	if (m_pSelectedTimer) {
		if (m_pSelectedTimer->GetTimerCount()==0) {	// a pure timespan timer
			ShowSpanEditor();
/*			m_iTypeSelected = 0;
			if (m_pTimerEditorSpan) {
				m_pTimerEditorSpan->FillFromTimer(m_pSelectedTimer);
			}
*/
		} else {	// a combi timer
			ShowCombiEditor();
/*			// combiTimer
			m_iTypeSelected = 1;
*/		}

		m_sSelectedName = m_pSelectedTimer->GetName();

		// and set the lock according to the timer
		const CDWordArray & usedIDs = m_pSelectedTimer->GetLockIDs();
		for (int i=0;i<m_ctlLocks.GetCount();i++) {
			DWORD dwID = m_ctlLocks.GetItemData(i);
			BOOL bUsed=FALSE;
			for (int j=0;j<usedIDs.GetSize() && bUsed==FALSE;j++) {
				if (dwID==usedIDs[j]) {
					bUsed=TRUE;
				}
			}
			m_ctlLocks.SetCheck(i,bUsed);
		}
		m_ctlSpanType.EnableWindow(TRUE);
		if (m_listTimer.GetItemCount() > 1) {
			m_ctlCombinationType.EnableWindow(TRUE);
		}
		else {
			m_ctlCombinationType.EnableWindow(FALSE);
		}
	} else {
		// NULL for deselection
		if (m_pTimerEditorSpan) {
			m_pTimerEditorSpan->DestroyWindow();
			m_pTimerEditorSpan = NULL;
		}

		if (m_pTimerEditorCombi) {
			m_pTimerEditorCombi->DestroyWindow();
			m_pTimerEditorCombi = NULL;
		}

		m_sSelectedName = _T("");
		m_ctlSpanType.EnableWindow(FALSE);
		m_ctlCombinationType.EnableWindow(FALSE);
	}

	EnableExceptNew();
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnItemchangedTimerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (pNMListView->uNewState & LVIS_SELECTED) {
		SelectTimer((CSecTimer *)m_listTimer.GetItemData(pNMListView->iItem));
	}

	*pResult = 0;
}
///////////////////////////////////////////////////////////////////////
BOOL CTimerEditorPage::IsDataValid()
{
	BOOL bReturn = TRUE;
	CSecTimer* pTimer = NULL;
	CSecTimer* pTimer2 = NULL;
	for (int i=0;i<m_pTimers->GetSize();i++) {
		pTimer = m_pTimers->GetAt(i);

		// timer without time
		if (   pTimer->GetTimerCount() == 0
			&& pTimer->GetSpanCount() == 0
			)
		{
			bReturn = FALSE;
			SelectTimerByIndex(pTimer);
			AfxMessageBox(IDP_TIMER_NO_TIMESPAN,MB_ICONSTOP|MB_OK);
			break;
		}
		for (int j=i+1 ; j<m_pTimers->GetSize() ; j++) {
			pTimer2 = m_pTimers->GetAt(j);
			if (pTimer->GetName() == pTimer2->GetName())
			{
				bReturn = FALSE;
				SelectTimerByIndex(pTimer2);
				CString sMsg;
				sMsg.Format(IDP_TIMER_SAME_NAME, pTimer2->GetName());
				AfxMessageBox(sMsg, MB_ICONSTOP|MB_OK);
				break;
			}
		}
		if (bReturn == FALSE)
		{
			break;
		}
	}
	return bReturn;
}
///////////////////////////////////////////////////////////////////////
void CTimerEditorPage::SaveChanges()
{
	WK_TRACE_USER(_T("Zeitschaltuhren-Einstellungen wurden geändert\n"));
	m_pTimers->Save(GetProfile());
}
///////////////////////////////////////////////////////////////////////
void CTimerEditorPage::CancelChanges()
{
	m_pTimers->DeleteAll();
	m_pTimers->Load(CTime::GetCurrentTime(),GetProfile());

	FillListBox();
	FillLockListBox();

	// select the first timer
	if (m_pTimers->GetSize()) {
		SelectTimerByIndex(m_pTimers->GetAt(0));
	} else {
		SelectTimer(NULL);
	}
}
///////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnNew()
{
	CSecTimer *pNewTimer = new CSecTimer();
	CString sNewName,sL;
	sL.LoadString(IDS_NEW_TIMER);
	sNewName.Format(_T("%s %d"),sL,m_iNewCounter );
	pNewTimer->SetName(sNewName);
	DWORD dwMaxID = SECID_MIN_TIMER;
	for (int i=0;i<m_pTimers->GetSize();i++) {
		if (m_pTimers->GetAt(i)->GetID().GetID()>dwMaxID) {
			dwMaxID = m_pTimers->GetAt(i)->GetID().GetID();
		}
	}
	CSecID newID(dwMaxID+1);
	pNewTimer->SetID(newID);
	m_pTimers->Add(pNewTimer);

	FillListBox();
	SelectTimerByIndex(pNewTimer);
	m_ctlSelectedName.SetFocus();

	SetModified(TRUE,TRUE);	// needs server reset
	m_iNewCounter++;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnDelete()
{
	if (m_pSelectedTimer==NULL) {
		return;
	}

	// first check wether there is any Activation (CInputToOutput) with
	// notification
	CInputGroup* pInputGroup;
	CInput* pInput;
	CInputToOutput* pInputToOutput;
	int i,j,k;
	BOOL bModifiedActivations=FALSE;
	BOOL bDoDelete=FALSE;

	CSecID id = m_pSelectedTimer->GetID();

	CInputList *pInputList	= m_pParent->GetDocument()->GetInputs();
	for (i=0;i<pInputList->GetSize();i++) {
		pInputGroup = pInputList->GetGroupAt(i);
		for (j=0;j<pInputGroup->GetSize();j++) {
			pInput = pInputGroup->GetInput(j);
			// reverse loop, since we might delete an entry
			for (k=pInput->GetNumberOfInputToOutputs()-1;k>=0;k--) {
				pInputToOutput = pInput->GetInputToOutput(k);
				if (pInputToOutput->GetTimerID() == id)
				{
					// we still have one
					if (bDoDelete
						|| IDYES==AfxMessageBox(IDP_DELETE_TIMER_WITH_ACTIVATION,MB_YESNO)) {
						pInput->DeleteInputToOutput(pInputToOutput);
						bModifiedActivations |= TRUE;
						bDoDelete=TRUE;
					} else {
							// NOP OOPS 
					}
				}
					
			}	// end of k loop over InputsToOutputs
		} // inputs
	} //groups

	if (bModifiedActivations) {
		pInputList->Save(GetProfile(),IsLocal());
	}

	// we also have to check for combinations
	for (int t=0;t<m_pTimers->GetSize();t++) {
		if (m_pTimers->GetAt(t)->GetID()!=m_pSelectedTimer->GetID()) {
			m_pTimers->GetAt(t)->DropCombiTimer(*m_pTimers,m_pSelectedTimer->GetID());
		}
	}

	// delete the current entry
	m_pTimers->Remove(m_pSelectedTimer);
	WK_DELETE(m_pSelectedTimer);

	int ix = GetSelectedItemFromList(m_listTimer);

	int newIx=-1;
	if (ix<m_listTimer.GetItemCount()-1) {
		// move to next item
		newIx = ix;
	} else {
		// try to move one up/previous
		if (ix-1<m_listTimer.GetItemCount()-1) {
			newIx=ix-1;
		}
	}

	m_listTimer.DeleteItem(ix);
	if (newIx != -1) {
		DWORD dwState = m_listTimer.GetItemState(newIx, LVIS_SELECTED);
		dwState |= LVIS_SELECTED;
		m_listTimer.SetItemState(newIx,dwState, LVIS_SELECTED);	// will cause a SelectTimer
		m_ctlSelectedName.SetFocus();
	} else {
		SelectTimer(NULL);
	}

	SetModified(TRUE,TRUE);	// needs server reset
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTimerEditorPage::CanNew()
{
	return TRUE;	// always can create a new timer
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTimerEditorPage::CanDelete()
{
	return (m_listTimer.GetItemCount()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnSelchangeTimerLocks() 
{
	if (m_pSelectedTimer) {
		// drop all lock and add the used ones
		m_pSelectedTimer->RemoveAllLockIDs();
		for (int i=0;i<m_ctlLocks.GetCount();i++) {
			BOOL bUsed = m_ctlLocks.GetCheck(i);
			CSecID id = m_ctlLocks.GetItemData(i);
			if (bUsed) {
				m_pSelectedTimer->AddLockID(id);
			}
		}
		SetModified(TRUE,TRUE);	// needs server reset
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnUpdateTimerSelectedName() 
{
	CString sNewText;
	m_ctlSelectedName.GetWindowText(sNewText);
	// NOT YET any checks
	if ( m_pSelectedTimer ) {
		m_pSelectedTimer->SetName(sNewText);
		int ix = GetIndexOfTimer(m_pSelectedTimer);
		m_listTimer.SetItemText(ix,0,sNewText);
		SetModified(TRUE,TRUE);	// needs server reset
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnChangeTimerSelectedName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTimerEditorPage::StretchElements()
{
	// return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnTimerDoTest() 
{
	// call the modal Timer Test dialog to preview/test timers

	CTimerTestDialog dlg(m_pParent->GetDocument());

	dlg.DoModal();
	
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnTimerExport() 
{
	COemFileDialog dlg(this);
	CString sSave,sF;

	sSave.LoadString(IDS_TIMER_EXPORT);
	sF.LoadString(IDS_TIMER_SETTINGS);
	dlg.SetProperties(FALSE,sSave,sSave);
	dlg.SetInitialDirectory(theApp.m_sOpenDir);
	dlg.AddFilter(sF,_T("tim"));

	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		CStdioFileU f;
		
		f.SetFileUnicode();
		
		if (f.Open(sFileName,CFile::modeWrite|CFile::modeCreate)) 
		{
			f.WriteString(_T("#TIMER#\n"));
			// loop over all timers
			int c=0;
			for (int i=0;i<m_pTimers->GetSize();i++) 
			{
				CWK_String sOneLine;
				const CSecTimer *pCombiTimer=NULL;
				const CSecTimer *pOneTimer = m_pTimers->GetAt(i);
				sOneLine += CWK_String(pOneTimer->GetName());
				sOneLine += _T(":");
				if (pOneTimer->GetTimerCount()) 
				{
					{
					const CSecIDArray &ids=pOneTimer->GetPlusIDs();
					for (c=0;c<ids.GetSize();c++) {
						pCombiTimer = m_pTimers->GetTimerByID(ids[c]);
						if (pCombiTimer) {
							if (sOneLine[sOneLine.GetLength()-1] != _T(':')) {
								sOneLine += _T(',');
							}
							sOneLine += _T(" +");
							sOneLine += CWK_String(pCombiTimer->GetName());
						} else {
							WK_TRACE_ERROR(_T("Could not find combi timer %08x\n"),ids[c]);
						}
					}	// end of loop over ids
					}

					{
					const CSecIDArray &ids=pOneTimer->GetMinusIDs();
					for (c=0;c<ids.GetSize();c++) {
						pCombiTimer = m_pTimers->GetTimerByID(ids[c]);
						if (pCombiTimer) {
							if (sOneLine[sOneLine.GetLength()-1] != _T(':')) {
								sOneLine += _T(',');
							}
							sOneLine += _T(" -");
							sOneLine += pCombiTimer->GetName();
						} else {
							WK_TRACE_ERROR(_T("Could not find combi timer %08x\n"),ids[i]);
						}
					}	// end of loop over ids
					}

					{
					const CSecIDArray &ids=pOneTimer->GetRestrictIDs();
					for (c=0;c<ids.GetSize();c++) {
						pCombiTimer = m_pTimers->GetTimerByID(ids[c]);
						if (pCombiTimer) {
							if (sOneLine[sOneLine.GetLength()-1] != _T(':')) {
								sOneLine += _T(',');
							}
							sOneLine += _T(" /");
							sOneLine += pCombiTimer->GetName();
						} else {
							WK_TRACE_ERROR(_T("Could not find combi timer %08x\n"),ids[i]);
						}
					}	// end of loop over ids
					}

				} else {
					// spans
					const CSecTimeSpanArray & spans  = pOneTimer->GetTotalSpans();
					for (c=0;c<spans.GetSize();c++) {
						const CSecTimeSpan *pSpan = spans[c];
						if (sOneLine[sOneLine.GetLength()-1] != _T(':')) {
							sOneLine += _T(" ,");
						}
						CString sDay;
						CString sStart,sEnd;
						
						sStart = pSpan->GetStartString();
						sEnd = pSpan->GetEndString();

						// there are two type of date MO:... 1.1.1999,...
						int ix = 0;
						// CAVEAT order is important
						if (sStart.Find(_T(',')) != -1) {
							ix = sStart.Find(_T(','));
							sDay = sStart.Left(ix);
							sStart = sStart.Mid(ix+1); 
							sEnd = pSpan->GetEndString().Mid(ix+1);
						} else if (sStart.Find(_T(':')) != -1) {
							ix = sStart.Find(_T(':'));
							sDay = sStart.Left(ix);
							sStart = sStart.Mid(ix+1); 
							sEnd = pSpan->GetEndString().Mid(ix+1);
						} else {
							// wrong format
							WK_TRACE_ERROR(_T("Wrong timer format '%s\n"),sStart);
						}

						sOneLine += sDay;
						sOneLine += _T(":");
						sOneLine += sStart;
						sOneLine += _T("-");
						sOneLine += sEnd;
					}
				}
				sOneLine += _T("\n");
				f.WriteString(sOneLine);
			}
		} else {
			// could not open for writing
			WK_TRACE_ERROR(_T("Could not open %s for writing\n"),sFileName);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
static BOOL CheckTimerMagic(CStringArray& strings)
{
	BOOL bOkay = FALSE;

	// add all found names to the names array
	CString sOneLine;
	if (strings.GetSize()>0) 
	{
		sOneLine = strings.GetAt(0);
		if (sOneLine.Find(_T("#TIMER#"))!=-1) 
		{
			bOkay = TRUE;
		}
	}

	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
static void ReadStrings(CStdioFileU& f, CStringArray &strings)
{
	CString sOneLine;
	while (f.ReadString(sOneLine)) 
	{
		strings.Add(sOneLine);
	}
}
/////////////////////////////////////////////////////////////////////////////
static BOOL ReadTimerNames(CStringArray& strings, CStringArray& names)
{
	BOOL bOkay = TRUE;
	// add all found names to the names array
	CString sOneLine;
	for (int i=1;i<strings.GetSize();i++) 
	{
		sOneLine = strings[i];
		sOneLine.TrimLeft();
		sOneLine.TrimRight();
		if (   sOneLine[0]!=_T('#') 
			&& sOneLine[0]!=_T(';')) 
		{
			int ix=sOneLine.Find(_T(':'));
			if (ix != -1) 
			{
				names.Add(sOneLine.Left(ix));
			} 
			else 
			{
				// no : found
			}
		}
	}	// eof loop

	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
static BOOL ImportTimers(CStringArray& strings,
						 CSecTimerArray &root)
{
	BOOL bOkay = TRUE;

	CWK_String sOneLine;
	// fill all lines into an array, there might be multiple passes to resolve combi timers
	CWK_Strings allLines;
	for (int i=1;i<strings.GetSize();i++)
	{
		sOneLine = strings[i];
		sOneLine.TrimLeft();
		if (sOneLine[0]!=_T('#') && sOneLine[0]!=_T(';')) {
			allLines.Add(new CWK_String(sOneLine));
		}
	}	// end of input loop
	
	BOOL bRetry=TRUE;
	CTime ct = CTime::GetCurrentTime();
	while (bRetry) {
		BOOL bDidOne=FALSE;
		for (int i=0;i<allLines.GetSize();i++) {
			sOneLine = *allLines[i];

			BOOL bDidAdd=TRUE;

			int ix=sOneLine.Find(_T(':'));
			if (ix != -1) {
				CWK_String sName = sOneLine.Left(ix);
				sName.TrimLeft();
				sName.TrimRight();
				// TRACE(_T("Timer named '%s':\n"),sName);

				CSecTimer *pNewTimer = (CSecTimer *)root.GetTimerByName(sName);	// OOPS deconst
				if (pNewTimer) {
					TRACE(_T("Clear duplicate %s\n"),sName);
					pNewTimer->RemoveAllTimeSpans();
					pNewTimer->RemoveAllCombiIDs();
				} else {
					pNewTimer = new CSecTimer();
					pNewTimer->SetName(sName);
					DWORD dwMaxID = SECID_MIN_TIMER;
					for (int i=0;i<root.GetSize();i++) {
						if (root[i]->GetID().GetID()>dwMaxID) {
							dwMaxID = root[i]->GetID().GetID();
						}
					}
					CSecID newID(dwMaxID+1);
					pNewTimer->SetID(newID);
					root.Add(pNewTimer);
				}

				CWK_String sData = sOneLine.Mid(ix+1);
				CWK_Strings data;
				data.SplitArgs(sData, _T(','));	// split the , seperated llist
				data.TrimLeft();
				data.TrimRight();

				for (int d=0;d<data.GetSize();d++) {
					//TRACE(_T("%s,"),(LPCTSTR)*data[d]);
					CWK_String sTmp = *data[d];
					CString sStart,sEnd;
					CString sDay;
					TCHAR chFirstChar=0;

					BOOL bPlain=TRUE;
					if (sTmp.GetLength()) {
						chFirstChar= sTmp[0];
						if (chFirstChar==_T('+') 
							|| chFirstChar == _T('-') 
							|| chFirstChar ==_T('/')) {
							bPlain=FALSE;
						}
					}

					if (bPlain) {
						// not a combi timer
						// but we hae to convert date: to date,
						int ix = sTmp.Find(_T(':'));

						sDay = sTmp.Left(ix);

						char chDelim=_T(':');

						if (sDay.Find(_T('.')) != -1) {
							// a date format
							chDelim = _T(',');
						}

						sStart = sDay + chDelim;
						int ix2 = sTmp.Find(_T('-'));
						sStart += sTmp.Mid(ix+1,ix2-ix-1);

						sEnd = sDay + chDelim;
						sEnd += sTmp.Mid(ix2+1);
						// TRACE(_T("SPAN start '%s' end '%s'\n"),sStart,sEnd);

						CSecTimeSpan newSpan(ct,sStart,sEnd);
						if (newSpan.IsValid()) {
							pNewTimer->AddTimeSpan(newSpan);
						} else {
							WK_TRACE_ERROR(_T("Invalid timer span '%s' '%s'\n"),sStart,sEnd);
						}
					} else {
						// combi timer, might need multi pass
						CString sCombiName;
						sCombiName = sTmp.Mid(1);
						const CSecTimer *pCombi = root.GetTimerByName(sCombiName);
						if (pCombi) {
							CSecID id = pCombi->GetID();
							if (chFirstChar == _T('+')) {
								pNewTimer->AddPlusID(id);
							} else if (chFirstChar == _T('-')) {
								pNewTimer->AddMinusID(id);
							} else if (chFirstChar == _T('/')) {
								pNewTimer->AddRestrictID(id);
							} else {
								// should never happen
								WK_TRACE_ERROR(_T("Invalid timer leading combi char '%c'\n"), chFirstChar);
							}
						} else {
							// required combi timer not found
							// NOT YET  retry later
							bDidAdd=FALSE;
						}
					}
				}
				// TRACE(_T("\n"));
			} else {
				// no : found
			}

			if (bDidAdd==FALSE) {
				// try again later
				WK_TRACE_ERROR(_T("Did not add timer '%s'\n"),sOneLine);
			} else {
				delete allLines[i];
				allLines.RemoveAt(i);
				i--; // adjust loop
				bDidOne = TRUE;
			}
		} // end of for loop over all lines

		if (bDidOne) {
			bRetry=TRUE;	// NOT YET
		} else {
			bRetry=FALSE;
		}
	}	// end of while retry loop

	root.CalcTotalSpans(root);

	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerEditorPage::OnTimerImport() 
{
	COemFileDialog dlg(this);
	CString sTimerImport,sFilter;

	sTimerImport.LoadString(IDS_TIMER_IMPORT);
	sFilter.LoadString(IDS_TIMER_SETTINGS);
	dlg.SetProperties(TRUE,sTimerImport,sTimerImport);
	dlg.SetInitialDirectory(theApp.m_sOpenDir);
	dlg.AddFilter(sFilter,_T("tim"));

	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		CStdioFileU f;
	
		if (f.Open(sFileName,CFile::modeRead|CFile::shareDenyNone)) 
		{
			CStringArray strings;

			ReadStrings(f,strings);

			BOOL bOkay = CheckTimerMagic(strings);
			if (bOkay)
			{
				CStringArray names;
				CStringArray duplicateNames;

				if (ReadTimerNames(strings, names)) 
				{
					// check fo duplicates ....
					for (int i=0;i<names.GetSize();i++) 
					{
						CString sName = names[i];

						BOOL bDuplicate=FALSE;
						for (int j=0;j<m_pTimers->GetSize() && bDuplicate==FALSE;j++) 
						{
							if (m_pTimers->GetAt(j)->GetName()==sName) 
							{
								duplicateNames.Add(sName);
								bDuplicate = TRUE;
							}
						}
					}

					if (duplicateNames.GetSize() > 0)
					{
						CTimerImportDialog importDialog(duplicateNames);
						if (importDialog.DoModal()==IDOK) 
						{
							ImportTimers(strings, *m_pTimers);
							SetModified(TRUE,TRUE);	// needs server reset
						}
					}
					else
					{
						ImportTimers(strings, *m_pTimers);
						SetModified(TRUE,TRUE);	// needs server reset
					}

					FillListBox();
					// select the first timer
					if (m_pTimers->GetSize()) 
					{
						SelectTimerByIndex(m_pTimers->GetAt(0));
					} 
					else 
					{
						SelectTimer(NULL);
					}
				} 
				else 
				{
					// some error in ReadTimerNames
				}
			}
		} 
		else 
		{
			// file not found
		}
	}
}
