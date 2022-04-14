// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "Factura.h"

#include "MainFrm.h"
#include "FacturaDoc.h"

#include "TreeDlg.h"
#include "TextLabel.h"
#include "LabelContainer.h"
#include "CaraMenu.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CCaraFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CCaraFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Globale Hilfebefehle
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

UINT CMainFrame::gm_nMainIndicators[NO_OF_MAIN_INDICATORS] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_SEPARATOR,
	ID_SEPARATOR,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler

CMainFrame::CMainFrame()
{
	// ZU ERLEDIGEN: Hier Code zur Member-Initialisierung einfügen
	
}

CMainFrame::~CMainFrame()
{
   BEGIN("~CMainFrame()");
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCaraFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // Fehler beim Erzeugen
	}

   if (!m_wndStatusBar.Create(this) || !SetMainPaneInfo())
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // Fehler beim Erzeugen
   }

   m_wndToolBar.GetToolBarCtrl().ModifyStyle(0,TBSTYLE_FLAT); // ToolBar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

/*
   CMenu*pMenu = GetMenu();
   m_wndMenuBar.Create(this, IDD_MENUBAR, CBRS_TOP|CBRS_FLYBY|CBRS_SIZE_DYNAMIC, (UINT)pMenu->Detach());
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
 	DockControlBar(&m_wndMenuBar);
*/
	// ZU ERLEDIGEN: Löschen Sie diese drei Zeilen, wenn Sie nicht wollen, dass die Symbolleiste
	//  andockbar ist.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

   if (!m_pMainMenu) m_pMainMenu = new CCaraMenu;
   m_pMainMenu->AddToolBar(&m_wndToolBar);

   m_nIDMenuResource = IDR_MAINFRAME;
   SetMenuBitmaps();
	return 0;
}

bool CMainFrame::SetMainPaneInfo()
{
   bool bResult = (m_wndStatusBar.SetIndicators(gm_nMainIndicators, NO_OF_MAIN_INDICATORS)!=0) ? true : false;
   if (bResult)
   {
      m_wndStatusBar.SetPaneInfo(0,0,SBPS_STRETCH|SBPS_NOBORDERS,0);
      m_wndStatusBar.SetPaneInfo(1,1,0, 50);
      m_wndStatusBar.SetPaneInfo(2,2,0, 50);
      m_nActualIndicatorSize = NO_OF_MAIN_INDICATORS;
   }
   else ASSERT(false);
   return bResult;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Destruktion

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

void CMainFrame::OnDestroy() 
{
	CCaraFrameWnd::OnDestroy();
}

CLabel * CMainFrame::PasteSpecial(CPtrList *ptr)
{
   UNUSED(ptr);
   CLabel *pl = NULL;
   CTreeDlg Tdlg;
   char text[64];
   Tdlg.m_strWndTitle = _T("Einfügen von :");
   ZeroMemory(&Tdlg.m_TvItem, sizeof(Tdlg.m_TvItem));
   Tdlg.m_pCallBack         = DataTree;
   Tdlg.m_TvItem.mask       = TVIF_PARAM|TVIF_TEXT;
   Tdlg.m_TvItem.pszText    = text;
   Tdlg.m_TvItem.cchTextMax = 63;
   Tdlg.m_pParam = this;

   if ((Tdlg.DoModal() == IDOK) && (Tdlg.m_TvItem.lParam != 1))
   {
      int n = Tdlg.m_TvItem.lParam / 100;
      Tdlg.m_TvItem;
      char szText[128] = "";
      wsprintf(szText, "$%d{%s", Tdlg.m_TvItem.lParam, Tdlg.m_TvItem.pszText);
      CRect rect(0,0,400,-200);
      CTextLabel *pTL = new CTextLabel(&rect, (char*)LPCTSTR(szText));
      if (pTL)
      {
         pTL->SetDrawPickPoints(true);
         pl = pTL;
      }
   }
   if (pl) return pl;
   return NULL;
}


int CMainFrame::DataTree(CTreeDlg *pDlg, int nReason)
{
   if (nReason != ONINITDIALOG) return 0;
   int i;
   char *szItems[] = 
   {
      "Kunde",
      "Rechnung",
      "Artikel",
      "Währungen",
      NULL
   };

   char *szCustomer[] =
   {
      "%s}Vorname",     // 0
      "%s}Name",        // 1
      "%s}Titel",       // 2
      "%s}Fon.Nr.",     // 3
      "%s}Fax.Nr.",     // 4
      "%s}Ort",         // 5
      "%s}Land",        // 6
      "%s}Straße",      // 7
      "%s}Firma",       // 8
      "%s}PLZ",         // 9
      "%s}HNr.",        // 10
      "%s}E-Mail",      // 11
      "%s}Kommentar",   // 12
      "%d}Kd.Nr.",      // 13
      "%d}Kunden",      // 14
      "%d}Kunde",       // 15
      NULL
   };

   char *szInvoice[] = 
   {
      "%04d.%02d.%04d}Rg.Nr.",         // 0
      "%d}Kd.Nr.",                     // 1
      "%04d}Jahr",                     // 2
      "%02d}Monat",                    // 3
      "%s}Monat",                      // 4
      "%02d}Tag",                      // 5
      "%d}Positionen",                 // 6
      "%d}Position",                   // 7
      "%s}Kommentar",                  // 8
      "%.2f%%}MwSt.",                  // 9
      "%.2f}MwSt.vom Ges. Preis",      // 10
      "%.2f}Ges. Preis(Netto)",        // 11
      "%.2f}Ges. Preis",               // 12
      "%.2f}Ges. Preis Bezugs Währung",// 13
      "%.2f}Ges. Preis andere Währung",// 14
      "%s}Bezahlungsart",              // 15
      "%d}Rechnungen",                 // 16
      "%d}Rechnung",                   // 17
      "%.2f}Ges. Preis(Netto) Bezugswährung",// 18
      "%.2f}Ges. Preis(Netto) andere Währung",// 19
      "%.2f}Rabatt Prozent",             // 20
      "%.2f}Rabatt (Netto) Betrag",      // 21
      "%.2f}Rabatt Betrag",              // 22
      NULL
   };

   char *szProduct[] = 
   {
      "%d}Nummer",                     // 0
      "%d}Anzahl",                     // 1
      "%s}Beschreibung",               // 2
      "%.2f}Preis(Netto)",             // 3
      "%.2f}Ges.Preis(Netto)",         // 4
      "%.2f}Preis",                    // 5
      "%.2f}Ges.Preis",                // 6
      "%.2f}MwSt. vom Preis",          // 7
      "%.2f}MwSt. vom Ges.Preis",      // 8
      "%.2f}Ges.Preis Bezugs Währung", // 9
      "%d}Produkte",                   // 10
      "%d}Produkt",                    // 11
      "%1d}Release x.",                // 12
      "%02d}Release .xx",              // 13
      "%d}Artikelgruppe",              // 14
      "%s}Artikelgruppe",              // 15
      "%d}Preisgruppe",                // 16
      "%s}Preisgruppe",                // 17
      "%d}Firmen EAN",                 // 18
      "%.2f}Ges.Preis andere Währung", // 19
      NULL
   };

   char *szCurrency[] = 
   {
      "%s}Währungseinheit",            // 0
      "%s}Basis Währungseinheit",      // 1
      "%s}Bezugs Währungseinheit",     // 2
      "%s}Währung",                    // 3
      "%s}Basis Währung",              // 4
      "%s}Bezugs Währung",             // 5
      "%f}Eigener Kurs",               // 6
      "%f}Kurs",                       // 7
      NULL
   };

   ASSERT(pDlg != NULL);
   ASSERT(pDlg->m_pParam != NULL);
   CMainFrame *pMF = (CMainFrame*) pDlg->m_pParam;

   TV_INSERTSTRUCT tv_is;

   if (!pDlg->m_TreeCtrl.DeleteAllItems()) return 0;

   memset(&tv_is, 0, sizeof(TV_INSERTSTRUCT));
   tv_is.hParent      = TVI_ROOT;
   tv_is.hInsertAfter = TVI_LAST;
   tv_is.item.mask    = TVIF_PARAM|TVIF_TEXT;
   tv_is.item.lParam  = 1;

   tv_is.item.pszText = (char*) szItems[0];
   tv_is.hParent = pDlg->m_TreeCtrl.InsertItem(&tv_is);
   for (i=0; szCustomer[i] != NULL; i++)
   {
      tv_is.item.lParam  = CUSTOMER_IDS+i;
      tv_is.item.pszText = (char*) szCustomer[i];
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.item.lParam  = 1;
   tv_is.hParent      = TVI_ROOT;
   tv_is.item.pszText = (char*) szItems[1];
   tv_is.hParent = pDlg->m_TreeCtrl.InsertItem(&tv_is);
   for (i=0; szInvoice[i] != NULL; i++)
   {
      tv_is.item.lParam  = INVOICE_IDS+i;
      tv_is.item.pszText = (char*) szInvoice[i];
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.item.lParam  = 1;
   tv_is.hParent      = TVI_ROOT;
   tv_is.item.pszText = (char*) szItems[2];
   tv_is.hParent = pDlg->m_TreeCtrl.InsertItem(&tv_is);
   for (i=0; szProduct[i] != NULL; i++)
   {
      tv_is.item.lParam  = PRODUCT_IDS+i;
      tv_is.item.pszText = (char*) szProduct[i];
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.item.lParam  = 1;
   tv_is.hParent      = TVI_ROOT;
   tv_is.item.pszText = (char*) szItems[3];
   tv_is.hParent = pDlg->m_TreeCtrl.InsertItem(&tv_is);
   for (i=0; szCurrency[i] != NULL; i++)
   {
      tv_is.item.lParam  = CURRENCY_IDS+i;
      tv_is.item.pszText = (char*) szCurrency[i];
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }
   return 1;
}
