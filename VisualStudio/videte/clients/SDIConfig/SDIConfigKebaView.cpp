/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigKebaView.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigKebaView.cpp $
// CHECKIN:		$Date: 8.07.98 11:47 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 8.07.98 11:05 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"
#include "SDIConfigKebaDoc.h"
#include "SDIConfigKebaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bExtraDebugTrace;

/////////////////////////////////////////////////////////////////////////////
// define fuer die Mindest-Verweildauer der Animationen in ms
#define TIMER_KEBA_ANIM		((DWORD)1000)

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaView
IMPLEMENT_DYNCREATE(CSDIConfigKebaView, CSDIConfigView)
/////////////////////////////////////////////////////////////////////////////
CSDIConfigKebaView::CSDIConfigKebaView()
	: CSDIConfigView(CSDIConfigKebaView::IDD)
{
	//{{AFX_DATA_INIT(CSDIConfigKebaView)
	m_sConfigState = _T("");
	m_sTime = _T("");
	//}}AFX_DATA_INIT
	// privata data
	m_dwTimeAnim = GetCurrentTime();
	m_timeTimerStarted = CTime::GetCurrentTime();
	m_uElapsedTimerID = 0;
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigKebaView::~CSDIConfigKebaView()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::DoDataExchange(CDataExchange* pDX)
{
	if (!pDX->m_bSaveAndValidate) {
		BOOL bEnable = FALSE;
		// Dialog-Daten sollen gesetzt werden
		if (m_lbControls.GetSafeHwnd()) {
			bEnable = !(GetDocument()->IsConfiguring());
			m_lbControls.EnableWindow(bEnable);
		}
	}
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDIConfigKebaView)
	DDX_Control(pDX, IDC_TXT_TIME_ELAPSED, m_txtTimeElapsed);
	DDX_Control(pDX, IDC_TXT_TIME, m_txtTime);
	DDX_Control(pDX, IDC_ANIMATE, m_animState);
	DDX_Control(pDX, IDC_LB_CONTROLS, m_lbControls);
	DDX_Control(pDX, IDC_EDIT_CONFIG_STATE, m_editConfigState);
	DDX_Text(pDX, IDC_EDIT_CONFIG_STATE, m_sConfigState);
	DDX_Text(pDX, IDC_TXT_TIME, m_sTime);
	//}}AFX_DATA_MAP
	if (m_editConfigState.GetSafeHwnd()) {
		m_editConfigState.LineScroll(m_editConfigState.GetLineCount());
	}
	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIConfigKebaView, CFormView)
	//{{AFX_MSG_MAP(CSDIConfigKebaView)
	ON_LBN_SELCANCEL(IDC_LB_CONTROLS, OnSelcancelLbControls)
	ON_LBN_SELCHANGE(IDC_LB_CONTROLS, OnSelchangeLbControls)
	ON_LBN_DBLCLK(IDC_LB_CONTROLS, OnDblclkLbControls)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaView diagnostics
#ifdef _DEBUG
void CSDIConfigKebaView::AssertValid() const
{
	CFormView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigKebaView::ConfigureEnabled()
{
	return ( LB_ERR != m_lbControls.GetCurSel() );
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CSDIConfigKebaDoc* CSDIConfigKebaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSDIConfigKebaDoc)));
	return (CSDIConfigKebaDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::ClearData()
{
	m_lbControls.ResetContent();
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::Configure()
{
	int iSelected = m_lbControls.GetCurSel();
	if (LB_ERR != iSelected) {
		CSecID id = m_lbControls.GetItemData(iSelected);
		GetDocument()->Configurate(id);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::AddSDIGroup(CSDIConfigRecord* pRecord)
{
	int iIndex = m_lbControls.AddString( pRecord->GetComment() );
	if ( (LB_ERR != iIndex) && (LB_ERRSPACE != iIndex) ) {
		if (g_bExtraDebugTrace) {
			WK_TRACE("AddSDIGroup %s %08lx added\n",
						pRecord->GetComment(), pRecord->GetID().GetID() );
		}
		if ( LB_ERR == m_lbControls.SetItemData(iIndex, pRecord->GetID().GetID() ) ) {
			// Falls es nicht geklappt hat, sicherheitshalber wieder rausschmeissen
			m_lbControls.DeleteString(iIndex);
			WK_TRACE_ERROR("AddSDIGroup %s %08lx rejected NO DATA\n",
						pRecord->GetComment(), pRecord->GetID().GetID() );
		}
		// Wenn es der erste Eintrag ist
		if ( 1 == m_lbControls.GetCount() ) {
			m_lbControls.SetCurSel(iIndex);
			// Gilt nur, wenn nicht gerade was anderes passiert
			if ( !(GetDocument()->IsTransfering())
					&& !(GetDocument()->IsConfiguring()) )
			{
				m_animState.Open(IDR_AVI_HAS_INFO);
				m_animState.Play(0,((UINT)-1),((UINT)-1));
				m_animState.RedrawWindow();
			}
		}
	}
	else {
		WK_TRACE_ERROR("AddSDIGroup %s %08lx not added\n",
						pRecord->GetComment(), pRecord->GetID().GetID() );
	}
	// Auf jeden Fall die Kopie des Records zerstoeren
	WK_DELETE(pRecord);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::UpdateSDIGroup(CSDIConfigRecord* pRecord)
{
	CSecID id = pRecord->GetID();
	int iCount = m_lbControls.GetCount();
	// Merke den selektierten Index
	int iSelected = m_lbControls.GetCurSel();
	// Suche nach Eintrag mit der id und ersetze den String
	BOOL bFound = FALSE;
	for ( int i=0 ; !bFound && i<iCount ; i++) {
		// Wenn die id gefunden wurde
		if ( id == m_lbControls.GetItemData(i) ) {
			bFound = TRUE;
			m_lbControls.DeleteString(i);
			int iNewIndex = m_lbControls.AddString( pRecord->GetComment() );
			if ( (LB_ERR != iNewIndex) && (LB_ERRSPACE != iNewIndex) ) {
				if (g_bExtraDebugTrace) {
					WK_TRACE("UpdateSDIGroup %s %08lx updated\n",
								pRecord->GetComment(), pRecord->GetID().GetID() );
				}
				if (LB_ERR == m_lbControls.SetItemData(iNewIndex, pRecord->GetID().GetID())) {
					// Falls es nicht geklappt hat, sicherheitshalber wieder rausschmeissen
					m_lbControls.DeleteString(iNewIndex);
					WK_TRACE_ERROR("UpdateSDIGroup %s %08lx rejected NO DATA\n",
									pRecord->GetComment(), pRecord->GetID().GetID() );
				}
				else {
					// Falls der bisherige Eintrag selektiert war
					if (i == iSelected) {
						// muss der neue Eintrag selektiert werden
						m_lbControls.SetCurSel(iNewIndex);
					}
				}
			}
			else {
				// Eintragen hat nicht geklappt
				WK_TRACE_ERROR("UpdateSDIGroup %s %08lx not updated\n",
							pRecord->GetComment(), pRecord->GetID().GetID() );
			}
		}
	}
	if (!bFound) {
		// id nicht gefunden
		WK_TRACE_ERROR("UpdateSDIGroup %s %08lx not found\n",
						pRecord->GetComment(), pRecord->GetID().GetID() );
	}

	// Auf jeden Fall die Kopie des Records zerstoeren
	WK_DELETE(pRecord);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::DeleteSDIGroup(CSecID id)
{
	for (int i=0 ; i<m_lbControls.GetCount() ; i++) {
		if ( id == m_lbControls.GetItemData(i) ) {
			m_lbControls.DeleteString(i);
			GetDocument()->ControlWasDeleted(id);
			if (g_bExtraDebugTrace) {
				WK_TRACE("DeleteSDIGroup %08lx deleted\n", id.GetID() );
			}
			break;
		}
	}
	// Wenn kein Eintrag mehr da ist
	if ( 0 == m_lbControls.GetCount() ) {
		// Gilt nur, wenn nicht gerade was anderes passiert
		if ( !(GetDocument()->IsTransfering())
				&& !(GetDocument()->IsConfiguring()) )
		{
			m_animState.Open(IDR_AVI_HAS_NO_INFO);
			m_animState.Play(0,((UINT)-1),((UINT)-1));
			m_animState.RedrawWindow();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::StartElapsedTimer()
{
	m_uElapsedTimerID = SetTimer(1, 100, NULL);
	if (m_uElapsedTimerID) {
		m_timeTimerStarted = CTime::GetCurrentTime();
		CTime time = CTime::GetCurrentTime();
		CTimeSpan timeSpan = time - m_timeTimerStarted;
		m_sTime = timeSpan.Format("%M:%S");
		m_txtTime.SetWindowText(m_sTime);
		m_txtTimeElapsed.ShowWindow(SW_SHOW);
		m_txtTime.ShowWindow(SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::UpdateElapsedTimer()
{
	if ( GetDocument()->IsConfiguring() || GetDocument()->IsTransfering() ) {
		CTime time = CTime::GetCurrentTime();
		CTimeSpan timeSpan = time - m_timeTimerStarted;
		m_sTime = timeSpan.Format("%M min %S s");
		m_txtTime.SetWindowText(m_sTime);
//		m_txtTime.ShowWindow(SW_SHOW);
	}
	else {
		StopElapsedTimer();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::StopElapsedTimer()
{
	if ( KillTimer(m_uElapsedTimerID) ) {
		m_uElapsedTimerID = 0;
	}
	m_txtTime.ShowWindow(SW_HIDE);
	m_txtTimeElapsed.ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::AskForDisconnect()
{
	// Nur, wenn auch Verbindung bestehtq
	if ( GetDocument()->IsConnected() ) {
		int iReturn = IDCANCEL;
		iReturn = AfxMessageBox(IDP_ASK_FOR_DISCONNECT, MB_YESNO|MB_ICONQUESTION);
		if (IDYES == iReturn) {
			GetDocument()->EndConnection(TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaView message handlers
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::OnInitialUpdate() 
{
	// Immer Basisklasse aufrufen, um die allgemeinen Sachen einzurichten
	CSDIConfigView::OnInitialUpdate();

/*
	// Alle wollen sie ToolTips haben
	if (m_ToolTip.Create(this) ) {
		m_ToolTip.Activate(TRUE);
		// Lasse den ToolTip die Zeilenumbrueche beachten
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
		// Wenn man will, kann man auch noch die Zeiten veraendern
		// Verweildauer des ToolTips (SHRT_MAX=dauernd)
		// m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
		// Wie schnell soll er erscheinen, wenn der Cursor ueber einem Tool steht
		// m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 200);
		// Mindest-Verzoegerungszeit bis zum naechsten Erscheinen
		// m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 200);

		// Alle Child-Windows durchklappern, um deren ToolTips einzulesen
		UINT uID = 0;
		CString sTip;
		int iIndex = 0;
		CWnd* pItem = GetWindow(GW_CHILD);
		// Wir wollen keine Endlosschleife
		while ( pItem && IsChild(pItem) ) {
			// die ToolTip-Texte aus der Resource laden,
			uID = pItem->GetDlgCtrlID();
			// falls Text vorhanden ins Array schreiben und das Tool anhaengen
			if ( sTip.LoadString(uID) && !sTip.IsEmpty() ) {
				iIndex = m_sTTArray.Add(sTip);
				m_ToolTip.AddTool( pItem, m_sTTArray.GetAt(iIndex) );
			}
			// Haben wir noch ein Child?
			pItem = pItem->GetNextWindow();
		}
	}
*/
	// Am Anfang haben wir nichts zu tun
	m_animState.Open(IDR_AVI_IDLE);

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigKebaView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (MSG_SDI_START_CONNECTION==wParam) {
		m_animState.Open(IDR_AVI_CONNECT);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		// Setze die Zeit fuer die Animation
		m_dwTimeAnim = GetCurrentTime();
		return TRUE;
	}
	else if (MSG_SDI_CONNECTION_FAILED==wParam) {
//		// Aber ein bisschen warten, damit die Animation beendet wird
//		while (GetCurrentTime() < m_dwTimeAnim+TIMER_KEBA_ANIM) {}
		ClearData();
		m_animState.Open(IDR_AVI_CONNECT_FAILED);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		return TRUE;
	}
	else if (MSG_SDI_STOP_CONNECTION==wParam) {
		ClearData();
		m_animState.Open(IDR_AVI_IDLE);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		return TRUE;
	}
	else if (MSG_SDI_GET_INFO==wParam) {
//		// Aber ein bisschen warten, damit die Animation beendet wird
//		while (GetCurrentTime() < m_dwTimeAnim+TIMER_KEBA_ANIM) {}
		m_animState.Open(IDR_AVI_GET_INFO);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		// Setze die Zeit fuer die Animation
		m_dwTimeAnim = GetCurrentTime();
		return TRUE;
	}
	else if (MSG_SDI_FILE_TRANSFER_START==wParam) {
		m_animState.Open(IDR_AVI_FILETRANSFER);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		// Setze die Zeit fuer die Animation
		m_dwTimeAnim = GetCurrentTime();
		StartElapsedTimer();
		return TRUE;
	}
	else if (MSG_SDI_FILE_TRANSFER_FAILED==wParam) {
		m_animState.Open(IDR_AVI_CONFIGURATION_FAILED);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		StopElapsedTimer();
		return TRUE;
	}
	else if (MSG_SDI_FILE_TRANSFER_READY==wParam) {
//		// Aber ein bisschen warten, damit die Animation beendet wird
//		while (GetCurrentTime() < m_dwTimeAnim+TIMER_KEBA_ANIM) {}
		StopElapsedTimer();
		GetDocument()->StartConfiguration();
		return TRUE;
	}
	else if (MSG_SDI_CONFIGURATION_TRY==wParam) {
//		// Aber ein bisschen warten, damit die Animation beendet wird
//		while (GetCurrentTime() < m_dwTimeAnim+TIMER_KEBA_ANIM) {}
		m_animState.Open(IDR_AVI_CONFIGURATION_TRY);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		// Setze die Zeit fuer die Animation
		m_dwTimeAnim = GetCurrentTime();
		StartElapsedTimer();
		return TRUE;
	}
	else if (MSG_SDI_CONFIGURATION_START==wParam) {
//		// Aber ein bisschen warten, damit die Animation beendet wird
//		while (GetCurrentTime() < m_dwTimeAnim+TIMER_KEBA_ANIM) {}
		m_animState.Open(IDR_AVI_CONFIGURATION);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		// Setze die Zeit fuer die Animation
		m_dwTimeAnim = GetCurrentTime();
		StartElapsedTimer();
		return TRUE;
	}
	else if (MSG_SDI_CONFIGURATION_READY==wParam) {
//		// Aber ein bisschen warten, damit die Animation beendet wird
//		while (GetCurrentTime() < m_dwTimeAnim+TIMER_KEBA_ANIM) {}
		m_animState.Open(IDR_AVI_CONFIGURATION_OK);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		StopElapsedTimer();
		AskForDisconnect();
		return TRUE;
	}
	else if (MSG_SDI_CONFIGURATION_FAILED==wParam) {
//		// Aber ein bisschen warten, damit die Animation beendet wird
//		while (GetCurrentTime() < m_dwTimeAnim+TIMER_KEBA_ANIM) {}
		m_animState.Open(IDR_AVI_CONFIGURATION_FAILED);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		StopElapsedTimer();
		AskForDisconnect();
		return TRUE;
	}
	else if (MSG_SDI_EXTERN_PROGRAM_START==wParam) {
		m_animState.Open(IDR_AVI_EXTERN_PROGRAM);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		return TRUE;
	}
	else if (MSG_SDI_EXTERN_PROGRAM_STOPPED==wParam) {
		m_animState.Open(IDR_AVI_IDLE);
		m_animState.Play(0,((UINT)-1),((UINT)-1));
		m_animState.RedrawWindow();
		return TRUE;
	}
	else if (MSG_SDI_UPDATE_CONTROL==wParam) {
		UpdateSDIGroup( (CSDIConfigRecord*)lParam );
		return TRUE;
	}
	else if (MSG_SDI_ADD_CONTROL==wParam) {
		AddSDIGroup( (CSDIConfigRecord*)lParam );
		return TRUE;
	}
	else if (MSG_SDI_DELETE_CONTROL==wParam) {
		DeleteSDIGroup( CSecID((DWORD)lParam) );
		return TRUE;
	}
	else if (MSG_SDI_UPDATE_STATE==wParam) {
		GetDocument()->GetConfigState(m_sConfigState);
		UpdateData(FALSE);
		return TRUE;
	}
	
	if ( (MSG_SDI_FIRST<=wParam) && (MSG_SDI_LAST>=wParam) ) {
		WK_TRACE_WARNING("CSDIConfigKebaView::OnCommand wParam:%08lx lParam:%08lx NOT HANDLED\n",
															wParam, lParam);
	}

//	return CSDIConfigView:::OnCommand(wParam, lParam);
	return CFormView::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::OnSelcancelLbControls() 
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::OnSelchangeLbControls() 
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::OnDblclkLbControls() 
{
	// TODO: Add your control notification handler code here
	
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigKebaView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Let the ToolTip process this message.
	if ( m_ToolTip.GetSafeHwnd() ) {
		m_ToolTip.RelayEvent(pMsg);	
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == m_uElapsedTimerID) {
		UpdateElapsedTimer();
	}
	
	CFormView::OnTimer(nIDEvent);
}
