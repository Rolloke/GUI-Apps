/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigView.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigView.cpp $
// CHECKIN:		$Date: 10.02.98 11:10 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 4.02.98 11:43 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"
#include "MainFrm.h"

#include "SDIConfigDoc.h"
#include "SDIConfigView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigView

IMPLEMENT_DYNCREATE(CSDIConfigView, CFormView)

BEGIN_MESSAGE_MAP(CSDIConfigView, CFormView)
	//{{AFX_MSG_MAP(CSDIConfigView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigView construction/destruction
CSDIConfigView::CSDIConfigView()
	: CFormView(CSDIConfigView::IDD)
{
	// Da wir mehrere Construktoren haben:
	// Die Initialisierung nur einmal aufrufen!!!
	// Hier keine weitere Initialisierung durchführen!!!
	ConstructorInit();
	// Hier keine weitere Initialisierung durchführen!!!
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigView::CSDIConfigView(UINT uIDTemplate)
	: CFormView(uIDTemplate)
{
	// Da wir mehrere Construktoren haben:
	// Die Initialisierung nur einmal aufrufen!!!
	// Hier keine weitere Initialisierung durchführen!!!
	ConstructorInit();
	// Hier keine weitere Initialisierung durchführen!!!
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::ConstructorInit()
{
	// Hier wird die Initialisierung durchgeführt!
	// Sowohl vom Wizzard als auch eigene

	//{{AFX_DATA_INIT(CSDIConfigView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_sizeOptimal.cx = 0;
	m_sizeOptimal.cy = 0;
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigView::~CSDIConfigView()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDIConfigView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Let the ToolTip process this message.
	if ( m_ToolTip.GetSafeHwnd() ) {
		m_ToolTip.RelayEvent(pMsg);	
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigView diagnostics
#ifdef _DEBUG
void CSDIConfigView::AssertValid() const
{
	CFormView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigDoc* CSDIConfigView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSDIConfigDoc)));
	return (CSDIConfigDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
const CSize& CSDIConfigView::GetOptimalSize() const
{
	return m_sizeOptimal;
//	return m_totalDev;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigView::ConfigureEnabled()
{
	WK_TRACE_WARNING("CSDIConfigView::EnableConfiguring() NOT OVERWRITTEN\n");
	ASSERT(0);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::Configure()
{
	WK_TRACE_WARNING("CSDIConfigView::Configure() NOT OVERWRITTEN\n");
	ASSERT(0);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::ClearData()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::AddSDIGroup(CSecID id, CString& sComment)
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::DeleteSDIGroup(CSecID id)
{
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigView message handlers
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigView::OnInitialUpdate() 
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

	// Passe das MDIChildFrame an die Mindestgroesse des Views (Dialog) an
	// um einmal die optimale Groesse des Views zu berechnen
	// Sicherheitshalber vorher einmal das MDIChildFrame durchrechnen lassen
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

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
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	WK_TRACE_WARNING("CSDIConfigView::OnCommand() NOT OVERWRITTEN\n");
	ASSERT(0);
	
	return CFormView::OnCommand(wParam, lParam);
}
