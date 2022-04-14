/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDITestView.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/SDITestView.cpp $
// CHECKIN:		$Date: 13.12.99 11:43 $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 13.12.99 11:40 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "ComDlg.h"

#include "SDITestView.h"
#include "StandardEntriesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDITestView
IMPLEMENT_DYNCREATE(CSDITestView, CFormView)
/////////////////////////////////////////////////////////////////////////////
// CSDITestView construction/destruction
CSDITestView::CSDITestView()
	: CFormView(CSDITestView::IDD)
{
	// Da wir mehrere Construktoren haben:
	// Die Initialisierung nur einmal aufrufen!!!
	// Hier keine weitere Initialisierung durchführen!!!
	ConstructorInit();
	// Hier keine weitere Initialisierung durchführen!!!
}
/////////////////////////////////////////////////////////////////////////////
CSDITestView::CSDITestView(UINT uIDTemplate)
			: CFormView(uIDTemplate)
{
	// Da wir mehrere Construktoren haben:
	// Die Initialisierung nur einmal aufrufen!!!
	// Hier keine weitere Initialisierung durchführen!!!
	ConstructorInit();
	// Hier keine weitere Initialisierung durchführen!!!
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::ConstructorInit()
{
	// Hier wird die Initialisierung durchgeführt!
	// Sowohl vom Wizzard als auch eigene

	//{{AFX_DATA_INIT(CSDITestView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_sizeOptimal.cx = 0;
	m_sizeOptimal.cy = 0;

	m_pStandardEntriesDlg = NULL;
	m_uTimerID = TIMER_UNKNOWN;
	m_uCounter = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestView::DestroyWindow() 
{
	KillTimer(m_uTimerID);
	m_uTimerID = TIMER_UNKNOWN;
	
	return CFormView::DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
CSDITestView::~CSDITestView()
{
	m_pStandardEntriesDlg = NULL;
}
/////////////////////////////////////////////////////////////////////////////
const CSize& CSDITestView::GetOptimalSize() const
{
	return m_sizeOptimal;
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::ShowHide()
{
	if (m_pStandardEntriesDlg) {
		m_pStandardEntriesDlg->ShowHide();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	// Alle wollen sie ToolTips haben
	if (m_ToolTip.Create(this) ) {
		m_ToolTip.Activate(TRUE);
		// Lasse den ToolTip die Zeilenumbrueche beachten
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
		// Wenn man will, kann man auch noch die Zeiten veraendern
		// Verweildauer des ToolTips (SHRT_MAX=dauernd)
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
		// Wie schnell soll er erscheinen, wenn der Cursor ueber einem Tool steht
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 200);
		// Mindest-Verzoegerungszeit bis zum naechsten Erscheinen
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 200);

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

	// Passe das MDIChildFrame an die Mindestgroesse des Views (Dialog) an
	// um einmal die optimale Groesse des Views zu berechnen
	// Sicherheitshalber vorher einmal das MDIChildFrame durchrechnen lassen
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

	EnlargeDlg();

	// Diese optimale Groesse wird abgespeichert
	// Sie entspricht der TotalSize in DeviceUnits des ScrollViews
	m_sizeOptimal = m_totalDev;

	// Falls wir den 3D-Look haben, muessen wir ein bischen mehr Platz opfern
	// Hole die Rahmenbreite für 3D-Look
	int i3DFrame = GetSystemMetrics(SM_CXEDGE);
	// Sie ist natuerlich auf beiden Seiten vorhanden
	i3DFrame += i3DFrame;
	m_sizeOptimal.cx += i3DFrame;
	m_sizeOptimal.cy += i3DFrame;

	// Maximiere das MDIChildWnd, damit es des Hauptfenster voll ausfuellt
	// und es keine unnoetigen Child-Rahmen gibt
	CWnd* pFrame = GetParentFrame();
	ASSERT(pFrame->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) ) );
	((CMDIChildWnd*)pFrame)->MDIMaximize();

	InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::EnlargeDlg()
{
	m_pStandardEntriesDlg = new CStandardEntriesDlg(GetDocument(), this);

	// Fenster- und Client koordinaten holen
	CRect rcWnd, rcClient, rcClientDlg;
	GetWindowRect(rcWnd);
	GetClientRect(rcClient);
	m_pStandardEntriesDlg->GetClientRect(rcClientDlg);

	// View anpassen
	int iWidthDiff = max(rcClientDlg.Width() - rcClient.Width(), 0);
	SetWindowPos(&wndTop, NULL, NULL,
					rcWnd.Width()+iWidthDiff, rcWnd.Height()+rcClientDlg.Height(),
					SWP_NOMOVE);

	// StandardEntryDlg positionieren
	m_pStandardEntriesDlg->SetWindowPos(&wndTop,
										rcClient.left,
										rcClient.bottom,
										NULL,
										NULL,
										SWP_NOSIZE);
	m_pStandardEntriesDlg->ShowWindow(SW_SHOW);
//	CenterWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::InitializeData()
{
	m_pStandardEntriesDlg->InitializeData();
	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDITestView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Let the ToolTip process this message.
	if ( m_ToolTip.GetSafeHwnd() ) {
		m_ToolTip.RelayEvent(pMsg);	
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDITestView)
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
	}

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CSDITestView diagnostics
#ifdef _DEBUG
void CSDITestView::AssertValid() const
{
	CFormView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CSDITestDoc* CSDITestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSDITestDoc)));
	return (CSDITestDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDITestView, CFormView)
	//{{AFX_MSG_MAP(CSDITestView)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDITestView message handlers
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	ShowHide();
	if ( GetDocument()->m_bStarted ) {
//		SetTimer(m_uTimerID, (GetDocument()->m_uRepeatTime)*1000, NULL);
		SetTimer(m_uTimerID, (GetDocument()->m_uRepeatTime), NULL);
	}
	else {
		KillTimer(m_uTimerID);
		m_uTimerID = TIMER_UNKNOWN;
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == m_uTimerID) {
		GetDocument()->Send();
	}
	else {
		CFormView::OnTimer(nIDEvent);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::OnOK() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestView::OnCancel() 
{
}
