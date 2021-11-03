// CARA3DVView.cpp : Implementierung der Klasse CCARA3DVView
//

#include "stdafx.h"
#include "MainFrm.h"
#include "ColorView.h"
#include "CARA3DV.h"
#include "CARA3DVDoc.h"
#include "CARA3DVView.h"
#include "LightDlg.h"
#include "ViewParams.h"
#include "StandardDlg.h"
#include "PlanGraph.h"
#include "DibSection.h"
#include "ETS3DGLRegKeys.h"

#include "..\CaraPreview.h"
#include "..\CaraToolBar.h"
#include "..\Resource.h"
#include "..\PictureLabel.h"
#include "..\ColorLabel.h"

#include "MultiMon.h"

#ifdef ETS_OLE_SERVER
#include "IpFrame.h"
#include "SrvrItem.h"
#endif

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  FILM_PAUSE     0
#define  FILM_START      1
#define  FILM_STARTR    -1

#define VIEW_FACTOR_X   2.0
#define VIEW_FACTOR_Y   2.0
#define VIEW_FACTOR_Z  10.0

#define REGKEY_DEFAULT_PERSPECTIVE  "Perspective"
#define REGKEY_DEFAULT_BITMAPSIZE   "BMP-Size"
#define REGKEY_DEFAULT_FX           "FX"
#define REGKEY_DEFAULT_TIMESTEP     "Timestep"
#define REGKEY_DEFAULT_REPEAT       "Repeat"
#define REGKEY_DEFAULT_TIMER        "Timer"
#define REGKEY_DEFAULT_COLOR_DIR    "ColorDir"
/////////////////////////////////////////////////////////////////////////////
// CCARA3DVView

IMPLEMENT_DYNCREATE(CCARA3DVView, CCaraView)

BEGIN_MESSAGE_MAP(CCARA3DVView, CCaraView)
	//{{AFX_MSG_MAP(CCARA3DVView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIEW_PHASEVALUES, OnViewPhasevalues)
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_COMMAND(ID_OPTIONS_RESET_COLORS, OnOptionsResetColors)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMMANDHELP    , OnCommandHelp)
   ON_MESSAGE(WM_APPLY_TO_VIEW  , OnApplyToView)

// Commands
// File
   ON_COMMAND(ID_FILE_PRINT,          OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT,   OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW,  OnFilePrintPreview)
	ON_COMMAND(ID_FILE_SAVE_AS_BITMAP, OnFileSaveAsBitmap)
// Edit
   ON_COMMAND(ID_EDIT_COPY_AS_DIB   , OnEditCopyAsDib)
// Functions
	ON_COMMAND(ID_FUNC_FILM_TIMER+ID_TB_RIGHT_EXT, OnFuncFilmTimeStep)
   ON_COMMAND(ID_VIEW_AXIS+ID_TB_RIGHT_EXT, OnViewOptionAxis)
   ON_COMMAND(ID_VIEW_GRID+ID_TB_RIGHT_EXT, OnViewOptionGrid)
// View
	ON_COMMAND(ID_VIEW_FULL       , OnViewFull)
// Options
   ON_COMMAND(ID_OPTIONS_SET_DEFAULT, OnOptionsSetDefault)
	ON_COMMAND(ID_OPTIONS_LOAD_COLORS, OnOptionsLoadColors)
	ON_COMMAND(ID_OPTIONS_SAVE_COLORS, OnOptionsSaveColors)
	ON_COMMAND(ID_OPTIONS_LIGHT      , OnOptionsLight)
	ON_COMMAND(ID_OPTIONS_VIEW       , OnOptionsView)
	ON_COMMAND(ID_OPTIONS_STANDARD   , OnOptionsStandard)

// Update Commands
// View
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCARA3DVView Konstruktion/Destruktion

CCARA3DVView::CCARA3DVView()
{
   BEGIN("CCARA3DVView");
   m_pLightDlg        = NULL;
   m_pViewParams      = NULL;
   m_pStdDlg          = NULL;
   // Gitterparameter
   m_dwAxes           = 0;       // R
   // Vollbilddarstellung
   m_hMenu            = NULL;
   ZeroMemory(&m_WndPlacement, sizeof(WINDOWPLACEMENT));
   // Effect: Reduzierung der Auflösung
	m_nEffectTimer     = 0;       // R
   m_bLightOnSave     = false;
   m_nDetailSave      = -1;
   m_bMouseButtonDown = false;
   // Filmsteuerung
   m_nFilmTimeMax     = 5;       // R
   m_bFilmRepeat      = false;   // R
   m_bFilmTimer       = false;   // R
   m_nUpdateTimer     = 0;
   m_nFilmRun         = 0;
   m_nFilmTimeCount   = 0;
   m_nPictureIndexOld = 0;
#ifdef ETS_OLE_SERVER
   m_bIPframeRecalcLayOut = false;
#endif
   CWinApp *pApp = AfxGetApp();
   m_strDefaultColorDir = pApp->GetProfileString(REGKEY_DEFAULT, REGKEY_DEFAULT_COLOR_DIR);
   m_pGLView = new CCara3DGL;
}

CCARA3DVView::~CCARA3DVView()
{
   BEGIN("~CCARA3DVView()");
   if (m_pLightDlg)   delete m_pLightDlg;                      // Löschen der Dialogobjekte
   if (m_pViewParams) delete m_pViewParams;
   if (m_pStdDlg)     delete m_pStdDlg;

   // Registryeinträge speichern
   CWinApp *pApp = AfxGetApp();
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_PERSPECTIVE, (m_pGLView->GetModes() & ETS3D_PR_PERSPECTIVE) ? 1:0);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_REPEAT     , m_bFilmRepeat   ? 1:0);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_TIMER      , m_bFilmTimer    ? 1:0);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_TIMESTEP   , m_nFilmTimeMax);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_FX         , (m_nEffectTimer!=0) ? 1:0);
   
   if (!m_strDefaultColorDir.IsEmpty())
   {
      CWinApp *pApp = AfxGetApp();
      pApp->WriteProfileString(REGKEY_DEFAULT, REGKEY_DEFAULT_COLOR_DIR, m_strDefaultColorDir);
   }
   delete m_pGLView;
   m_GLDummy.Destructor();
}

/////////////////////////////////////////////////////////////////////////////
// Initialisierungen

int CCARA3DVView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   BEGIN("CCARA3DVView::OnCreate");
   // Darstellung im RGB-Modus möglich ?
   HDC hdc   = ::GetDC(NULL);                                  
   int nBits = ::GetDeviceCaps(hdc, BITSPIXEL);                // Anzahl der Bits pro Pixel ermitteln
   ::ReleaseDC(NULL, hdc);
   if (nBits < 8)
   {
      AfxMessageBox(IDS_16_COLOR_FAILURE, MB_OK|MB_ICONSTOP);
      PostQuitMessage(-1);
      return -1;                                               // RGB nicht möglich
   }

   if (CView::OnCreate(lpCreateStruct) == -1)
   {
      return -1;
   }
   // Registryeinträge lesen
   CWinApp *pApp = AfxGetApp();
   bool bGLDirect        = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_GLDIRECT   , 1) != 0) ? true : false;
   bool bDoublebuffer    = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_GLDOUBLE   , 1) != 0) ? true : false;
   bool bPerspective  = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_PERSPECTIVE, 0) != 0) ? true : false;
   m_bFilmRepeat      = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_REPEAT     , 0) != 0) ? true : false;
   m_bFilmTimer       = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_TIMER      , 0) != 0) ? true : false;
   m_nFilmTimeMax     =  pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_TIMESTEP   , 5);

   bool bGeneric      = (pApp->GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_GLGENERIC, SETTINGS_GLGENERIC_DEFAULT) != 0) ? true : false;

   // OpenGL Parameter initialisieren
   CCARA3DVDoc *pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   m_dwAxes = pDoc->m_Grid.GetAxes();
   m_pGLView->SetDocument(pDoc);
   m_pGLView->SetBkColor(((CCaraWinApp*)AfxGetApp())->GetBackgroundColor());
   m_pGLView->SetGeneric(bGeneric);
   DWORD nModes = 0;
   if (bGLDirect) nModes = (bDoublebuffer) ? ETS3D_DC_DIRECT_2 : ETS3D_DC_DIRECT_1;
   else           nModes = (bDoublebuffer) ? ETS3D_DC_DIB_2    : ETS3D_DC_DIB_1;
   if (bPerspective)nModes |= ETS3D_PR_PERSPECTIVE;
   else             nModes |= ETS3D_PR_ORTHOGONAL;
   m_pGLView->SetModes(nModes, ETS3D_DC_CLEAR|ETS3D_PR_CLEAR);

   pDoc->m_nProjection = !bPerspective;

   // Timer zum Updaten initialisieren
   m_nUpdateTimer = SetTimer(EVENT_UPDATETIMER, TIME_UPDATETIMER, NULL);
   if (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_FX, 0))
   {
	   m_nEffectTimer = SetTimer(EVENT_EFFECTTIMER, TIME_EFFECTTIMER, NULL);
   }
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVView Nachrichten-Handler

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVView Zeichnen
void CCARA3DVView::OnSize(UINT nType, int cx, int cy) 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnSize");
#endif

   CView::OnSize(nType, cx, cy);
   if (cx < 2) cx = 2;
   if (cy < 2) cy = 2;

//   m_pGLView->MoveWindow(0, 0, cx, cy, true);

#ifdef ETS_OLE_SERVER
   CCARA3DVApp *pApp = (CCARA3DVApp*) AfxGetApp();
   if (pApp->m_CmdInfo.m_bRunEmbedded)
   {
      if (m_bIPframeRecalcLayOut)
      {
         CDC *pDC = GetDC();
         CSize size(cx, cy);
         pDC->DPtoHIMETRIC(&size);
         ReleaseDC(pDC);
         CCARA3DVDoc        *pDoc  = GetDocument();
         COleServerSrvrItem *pItem = pDoc->GetEmbeddedItem();
         pItem->m_sizeExtent.cx = abs(size.cx);
         pItem->m_sizeExtent.cy = abs(size.cy);              // MM_HIMETRIC Koordinaten
         pDoc->SetModifiedFlag(true);
      }
   }
#endif
}

LRESULT CCARA3DVView::SendMsgToGLView(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::SendMsgToGLView");
#endif
   return m_pGLView->SendMessage(nMsg, wParam, lParam);
}

void CCARA3DVView::OnDraw(CDC* pDC)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnDraw");
#endif
   if (pDC->IsPrinting())
      CCaraView::OnDraw(pDC);
}
/////////////////////////////////////////////////////////////////////////////
// CCARA3DVView Drucken
/////////////////////////////////////////////////////////////////////////////
// CCARA3DVView Diagnose
#ifdef _DEBUG
void CCARA3DVView::AssertValid() const
{
   CView::AssertValid();
}
void CCARA3DVView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCARA3DVDoc* CCARA3DVView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCARA3DVDoc)));
   return (CCARA3DVDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVView Nachrichten-Handler
BOOL CCARA3DVView::OnEraseBkgnd(CDC* pDC) 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnEraseBkgnd");
#endif
   if (m_pGLView->IsInitialized()) return 1;
   CBrush brush(((CCaraWinApp*)AfxGetApp())->GetBackgroundColor());
   CRect rcClient;
   GetClientRect(&rcClient);
   pDC->FillRect(&rcClient, &brush);
   brush.DeleteObject();
   return 1;
}

void CCARA3DVView::OnDestroy() 
{
   BEGIN("CCARA3DVView::OnDestroy");
   // Timer löschen
   KillTimerSave(m_nUpdateTimer);
   KillTimerSave(m_nEffectTimer);

   // 3D-View entkoppeln und zerstören
   m_pGLView->Destroy();

   CRITICAL_SECTION *pCS = m_pGLView->DetachExternalCS();
   CCARA3DVDoc *pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   ASSERT(pCS == pDoc->m_pcsDocumentData);
   pDoc->DeleteCriticalSectionOfDoc();

   CView::OnDestroy();
}

void CCARA3DVView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   BEGIN("CCARA3DVView::OnUpdate");
   if (lHint & (UPDATE_INSERT_NEWDOC|UPDATE_INSERT_DOC))
   {
      CCARA3DVDoc *pDoc = (CCARA3DVDoc*) GetDocument();
      CAutoCriticalSection  csAuto(pDoc->m_pcsDocumentData);
      CalculateViewVolume();
      m_pGLView->SetBkColor(((CCaraWinApp*)AfxGetApp())->GetBackgroundColor());

      m_pGLView->PerformGridList(pDoc->m_Grid.DoDrawGrid(), true);
      pDoc->m_Grid.InvalidateGrid();
      pDoc->m_Grid.InvalidateStep();
      if (lHint == UPDATE_INSERT_NEWDOC)                    // von CaraCalc muß initialisiert werden
      {
         if (pDoc->m_p3DObject)                                // Das Gitter richtet sich nach dem 3D-Objekt
         {
            switch (m_pGLView->GetObjectType())
            {
               case PLAN_GRAPH:  m_dwAxes &= ~SHOW_POLAR_GRID; break;
               case POLAR_GRAPH: m_dwAxes |=  SHOW_POLAR_GRID; break;
            }
            pDoc->m_Grid.SetAxes(m_dwAxes);
            m_pGLView->PerformList(LIST_AXES, ((m_dwAxes&SHOW_AXIS) != 0) ? true : false);
            m_pGLView->InvalidateList(LIST_AXES);
            AdaptLightDistance();
         }
         OnOptionsSetDefault();
      }
      else                                                  // Parameter vom Document übernehmen
      {
         m_pGLView->SetModes((pDoc->m_nProjection == 0) ? ETS3D_PR_PERSPECTIVE : ETS3D_PR_ORTHOGONAL, ETS3D_PR_CLEAR);
         m_dwAxes = pDoc->m_Grid.GetAxes();
      }

      pDoc->m_bLighting = pDoc->m_bLighting && m_pGLView->IsALightOn();
      if (pDoc->m_bLighting)
      {
         pDoc->CalculateLightPosition();
      }

      if (m_pGLView->Is3DObjectAvailable() && !m_pGLView->IsInitialized())
      {
         PostMessage(WM_TIMER, EVENT_INITIALIZE_GL, 0);
      }
      else
      {
         PostMessage(WM_TIMER, EVENT_INVALIDATE_GL, 0);
      }

      SetFilmIndexToStatusPane();
      InvalidateFrameContent();
   }
   else if (lHint &UPDATE_DELETE_LABEL)
   {
      if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CLabel)))
      {
         CCARA3DVDoc *pDoc = (CCARA3DVDoc*) GetDocument();     // Feste Label Überprüfen
         for (int i=0; i<pDoc->m_nHeadLines; i++)
         {
            if (pHint==pDoc->m_ppHeadLines[i])                  // Überschriften
            {
               pDoc->m_ppHeadLines[i]->SetNoUndoCmd(true);
               pDoc->m_ppHeadLines[i] = NULL;
            }
         }
         if (pHint==pDoc->m_pDibLabel)                         // 3D-Bild
         {
            pDoc->m_pDibLabel->SetNoUndoCmd(true);
            CDibSection *pDib = pDoc->m_pDibLabel->DetachDIB();
            if (pDib)
            {
               m_pGLView->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_DIB|DESTROY_DIBSECTION, (LPARAM)pDib);
            }
            pDoc->m_pDibLabel = NULL;
         }
         if (pHint==pDoc->m_pColorLabel)                       // Farbstufen
         {
            VERIFY(pDoc->m_pColorLabel->Detach() == &pDoc->m_LevelColor);
            pDoc->m_pColorLabel->SetNoUndoCmd(true);
            pDoc->m_pColorLabel = NULL;
         }
      }
   }
   else if (lHint & UPDATE_DELETE_CONTENTS)
   {
      m_pGLView->InvalidateList(LIST_OBJECT);
      m_pGLView->InvalidateList(LIST_AXES);
      m_pGLView->InvalidateGridList();
      m_pGLView->InvalidateList(LIST_PINS);
   }
   else if (lHint & UPDATE_FRAME_CONTENT)
   {
      UpdateLightDlg();
      UpdateStandardDlg();
      UpdateViewParamsDlg();
   }
}

void CCARA3DVView::OnEditCopyAsDib() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnEditCopyAsDib");
#endif
   if (m_pGLView->Is3DObjectAvailable())
   {
      HGLOBAL hGl = NULL;
      m_pGLView->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_HGLOBAL, (LPARAM)&hGl);
      if (hGl && OpenClipboard() && EmptyClipboard())
      {
         if (SetClipboardData(CF_DIB, hGl) == NULL)
            GlobalFree(hGl);
         CloseClipboard();
      }
   }
}

void CCARA3DVView::OnFileSaveAsBitmap()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFileSaveAsBitmap");
#endif
   if (m_pGLView->Is3DObjectAvailable())
   {
      CFileDialog fd(false);
      CString strFilter, strTitle;
      fd.m_ofn.nFilterIndex   = 1;
      fd.m_ofn.nFileExtension = 1;
      fd.m_ofn.Flags         |= OFN_SHOWHELP;
      strTitle.LoadString(IDS_SAVE_AS_BMP);
      fd.m_ofn.lpstrTitle     = LPCTSTR(strTitle);
      strFilter.LoadString(IDS_CARA_BITMAP);
      fd.m_ofn.lpstrFilter    = LPCTSTR(strFilter);
      for (int i=strFilter.GetLength()-1; i>=0; i--) if (fd.m_ofn.lpstrFilter[i] == '\n') ((char*)fd.m_ofn.lpstrFilter)[i] = 0;
      fd.m_ofn.nMaxCustFilter = 1;
      fd.m_ofn.lpstrDefExt    = &fd.m_ofn.lpstrFilter[strlen(fd.m_ofn.lpstrFilter)];
      if ((fd.DoModal() == IDOK) && (fd.m_ofn.lpstrFileTitle != NULL))
      {
         m_pGLView->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_FILE, (LPARAM)fd.m_ofn.lpstrFileTitle);
      }
   }
}

CLabel *CCARA3DVView::Paste3DLabel(int nPaste)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::Paste3DLabel");
#endif
   CCARA3DVDoc *pDoc = (CCARA3DVDoc*) GetDocument();

   switch (nPaste)
   {
      case PASTECOLORSCALE:
         if (pDoc->m_pColorLabel == NULL)
         {
            pDoc->m_pColorLabel = new CColorLabel;
            if (pDoc->m_pColorLabel)
            {
               pDoc->m_pColorLabel->SetRect(CRect(0, 0, 500, -150));
               pDoc->m_pColorLabel->SetDrawPickPoints(true);
               pDoc->AttachLevelColor();
               return pDoc->m_pColorLabel;
            }
         }break;
      case PASTE3DPICTURE:
         if (m_pGLView->Is3DObjectAvailable() && (pDoc->m_pDibLabel == NULL))
         {
            pDoc->m_pDibLabel = new CPictureLabel;
            if (pDoc->m_pDibLabel)
            {
               pDoc->m_pDibLabel->m_bNoFile = true;
               CDibSection *pDib = new CDibSection;
               m_pGLView->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_DIB, (LPARAM)pDib);
               pDoc->m_pDibLabel->AttachDIB(pDib, true);
               pDoc->m_pDibLabel->SetScaleSize(CSize(pDib->GetWidth(), pDib->GetHeight()));
               pDoc->m_pDibLabel->SetDrawPickPoints(true);
               return pDoc->m_pDibLabel;
            }
         }break;
      case PASTEPICTUREINDEX:
         {
            int nP = m_pGLView->GetPictureIndex();
            CString str;
            char text[32] = "";
            ETSDIV_NUTOTXT ntx = {0.0, ETSDIV_NM_STD, 2, 32, 0};
            C3DGrid * pG = m_pGLView->GetGrid();
            ntx.nmode  = pG->GetNumModeLevel();
            ntx.nround = pG->GetRoundingLevel();
            ntx.fvalue = m_pGLView->GetSpecificValue(nP);
            CEtsDiv::NumberToText(&ntx, text);
            str.Format(IDS_FMT_PICTUREINDEX, nP+1, text, m_pGLView->GetSpecificUnit());
            if (!m_PictureTextOld.IsEmpty()) str += m_PictureTextOld;
            CRect rect(0,0, 1000, -300);
            CTextLabel *ptl = new CTextLabel(&rect, (char*)LPCTSTR(str));
            return ptl;
         }break;
      default:
         break;
   }
   return NULL;
}

void CCARA3DVView::InitDibSection()
{
   BEGIN("CCARA3DVView::InitDibSection");
   CCARA3DVDoc *pDoc = (CCARA3DVDoc*) GetDocument();
   if (m_pGLView->Is3DObjectAvailable() && pDoc->m_bPrintInit)
   {
      if (pDoc->m_pDibLabel == NULL)
      {
         pDoc->m_pDibLabel = new CPictureLabel;
         pDoc->GetLabelContainer()->InsertLabel(pDoc->m_pDibLabel, 0);
      }
   }
   if (pDoc->m_pDibLabel)
   {
      pDoc->m_pDibLabel->m_bNoFile = true;
      CDibSection *pDib = pDoc->m_pDibLabel->DetachDIB();
      if (!pDib) pDib = new CDibSection;
      if (pDib)
      {
         bool bCopied = false;
/*
         if (m_pGLView->GetModes() & ETS3D_DC_DIB)
         {
            CSize szPrint = pDoc->m_pDibLabel->GetPrintSize();
            if ((szPrint.cx != 0) && (szPrint.cy != 0))
            {
               CSize szOld = m_pGLView->GetViewSize();
               m_pGLView->SetSize(szPrint);
               m_pGLView->SendMessage(WM_COPY_FRAME_BUFFER, RENDER_TO_DIP, (LPARAM)pDib);
               m_pGLView->SetSize(szOld);
               bCopied = true;
            }
         }
         Beim Rendern in eine Dibsection kann die Größe übergeben werden
         und der HBITMAP in den MemoryDC selektiert werden. Direkt rendern, kein Kopieren;
*/
         if (!bCopied)
            m_pGLView->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_DIB, (LPARAM)pDib);
         
         pDoc->m_pDibLabel->AttachDIB(pDib);
         pDoc->m_pDibLabel->SetScaleSize(CSize(pDib->GetWidth(), pDib->GetHeight()));
      }
   }
}

void CCARA3DVView::OnFilePrintPreview() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFilePrintPreview");
#endif
   InitDibSection();
   CCaraView::OnFilePrintPreview();
}

void CCARA3DVView::OnFilePrint()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFilePrint");
#endif
   InitDibSection();
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL);    // must be set
	if (LOWORD(GetCurrentMessage()->wParam) == ID_FILE_PRINT_DIRECT)
	{
		CCommandLineInfo* pCmdInfo = AfxGetApp()->m_pCmdInfo;

		if (pCmdInfo != NULL)
		{
			if (pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo)
			{
            // DC initialisieren
			}
		}

		printInfo.m_bDirect = TRUE;
	}

	if (OnPreparePrinting(&printInfo))
	{
      CDC dcPrint;
		ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);
		OnBeginPrinting(&dcPrint, &printInfo);
		OnPrepareDC(&dcPrint, &printInfo);
      if (m_nCurrentPage != -1)
      {
         int nFrom = printInfo.GetFromPage();
         int nTo   = printInfo.GetToPage();
         for (m_nCurrentPage=nFrom; m_nCurrentPage<=nTo; m_nCurrentPage++)
         {
            if ((m_nCurrentPage>nFrom) && (m_nCurrentPage<=nTo))
            {
               dcPrint.StartPage();
               dcPrint.EndPage();
            }
		      OnPrint(&dcPrint, &printInfo);
         }
      }
      else OnPrint(&dcPrint, &printInfo);

		OnEndPrinting(&dcPrint, &printInfo);
   }
}

void CCARA3DVView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnBeginPrinting");
#endif
   SMarginDlgParam *pMdp = (SMarginDlgParam*)pInfo->m_lpUserData;
   ASSERT(pMdp!=NULL);
   CCARA3DVDoc *pDoc = GetDocument();
   if (pDoc->m_bPrintInit && m_pGLView->Is3DObjectAvailable())   // Label positionieren
   {
      CRect rcPicture, rcColors, rcView(pMdp->rcDrawLoMetric);
      rcView.DeflateRect(CSize(3, -3));
      CSize szView = rcView.Size();
      if (abs(szView.cx) < abs(szView.cy))                     // Hochformat
      {
         rcPicture.left   = rcColors.left   = rcView.left;
         rcPicture.right  = rcColors.right  = rcView.right;
         rcPicture.top                      = rcView.top;
         rcPicture.bottom                   = rcView.bottom + 200;
         rcColors.top                       = rcPicture.bottom;
         rcColors.bottom                    = rcView.bottom;
      }
      else                                                     // Querformat
      {
         rcPicture.top    = rcColors.top    = rcView.top;
         rcPicture.bottom = rcColors.bottom = rcView.bottom;
         rcColors.left                      = rcView.left;
         rcColors.right                     = rcView.left + 200;
         rcPicture.left                     = rcColors.right;
         rcPicture.right                    = rcView.right;
      }
      if (pDoc->m_pColorLabel == NULL)
      {
         pDoc->m_pColorLabel = new CColorLabel;
         if (pDoc->m_pColorLabel)
         {
            pDoc->m_pColorLabel->SetRect(rcColors);
            pDoc->GetLabelContainer()->InsertLabel(pDoc->m_pColorLabel, 0);
            pDoc->m_pColorLabel->SetDrawPickPoints(true);
         }
      }
      if (pDoc->m_pDibLabel)
      {
         pDoc->m_pDibLabel->SetRect(rcPicture);
         pDoc->m_pDibLabel->SetScale(SCALE_NONPROPORTIONAL, false);
         pDoc->m_pDibLabel->SetDrawPickPoints(true);
         pDoc->SetHeadlinePositions(rcPicture.TopLeft());
      }
      CRect rc(pMdp->rcPageLoMetric);
      swap(rc.top, rc.bottom);
      for (int i=0; i<pDoc->m_nHeadLines; i++)
      {
         if (pDoc->m_ppHeadLines[i])
         {
            pDoc->m_ppHeadLines[i]->SetClipRect(&rc);
            pDoc->m_ppHeadLines[i]->InvalidateTextRect();
            pDoc->m_ppHeadLines[i]->SetDrawPickPoints(true);
         }
      }
      pDoc->m_bPrintInit = false;
   }
   pDoc->SetActiveHeadline();

   CCaraView::OnBeginPrinting(pDC, pInfo);

   if (m_pGLView->Is3DObjectAvailable())                         // 
   {
      pDoc->AttachLevelColor();
   }
}

void CCARA3DVView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnEndPrinting");
#endif
   CCaraView::OnEndPrinting(pDC, pInfo);
   CCARA3DVDoc *pDoc = GetDocument();

   if (pDoc->m_pColorLabel)
   {
      VERIFY(pDoc->m_pColorLabel->Detach() == &pDoc->m_LevelColor);
   }
   if (pDoc->m_pDibLabel)
   {
      CDibSection *pDib = pDoc->m_pDibLabel->DetachDIB();
      if (pDib)
      {
         m_pGLView->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_DIB|DESTROY_DIBSECTION, (LPARAM)pDib);
         delete pDib;
      }
   }
}

void CCARA3DVView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnEndPrintPreview");
#endif
   CCaraView::OnEndPrintPreview(pDC, pInfo, point, pView);
   CMainFrame *pWnd = (CMainFrame*)AfxGetMainWnd();
   if (pWnd)
   {
      pWnd->SetMainPaneInfo();
   }
}

void CCARA3DVView::OnOptionsSaveColors()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnOptionsSaveColors");
#endif
   CFileDialog fd(false);
   CString strFilter, strTitle;
   fd.m_ofn.nFilterIndex   = 1;
   fd.m_ofn.nFileExtension = 1;
   fd.m_ofn.Flags         |= OFN_SHOWHELP;
   strTitle.LoadString(IDS_SAVE_COLORS);
   fd.m_ofn.lpstrTitle     = LPCTSTR(strTitle);
   strFilter.LoadString(IDS_CARA_COLORS);
   fd.m_ofn.lpstrFilter    = LPCTSTR(strFilter);
   for (int i=strFilter.GetLength()-1; i>=0; i--) if (fd.m_ofn.lpstrFilter[i] == '\n') ((char*)fd.m_ofn.lpstrFilter)[i] = 0;
   fd.m_ofn.nMaxCustFilter = 1;
   fd.m_ofn.lpstrDefExt    = &fd.m_ofn.lpstrFilter[strlen(fd.m_ofn.lpstrFilter)];

   if (!m_strDefaultColorDir.IsEmpty()) SetCurrentDirectory(m_strDefaultColorDir);

   CFile file;
   if ((fd.DoModal() == IDOK) && (fd.m_ofn.lpstrFileTitle != NULL) && file.Open(fd.m_ofn.lpstrFileTitle, CFile::modeCreate|CFile::modeWrite))
   {
      COLORREF color;
      CCARA3DVDoc *pDoc = (CCARA3DVDoc*) GetDocument();
      CArchive ar(&file, CArchive::store);
      ar.m_pDocument = pDoc;
      pDoc->m_FileHeader.SetVersion(100);
      pDoc->WriteFileHeader(ar);
      pDoc->m_FileHeader.CalcChecksum();
      pDoc->m_LevelColor.WriteColors(ar);
      color =  m_pGLView->GetObjectColor();
      CLabel::ArchiveWrite(ar, &color, sizeof(COLORREF));
      color =  m_pGLView->GetBkColor();
      CLabel::ArchiveWrite(ar, &color, sizeof(COLORREF));
      pDoc->WriteFileHeader(ar, false);
      ar.Close();
      file.Close();
      int nCount = GetCurrentDirectory(0,NULL);
      if (nCount)
      {
         GetCurrentDirectory(nCount, m_strDefaultColorDir.GetBufferSetLength(nCount));
      }
   }
}

void CCARA3DVView::OnOptionsLoadColors()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnOptionsLoadColors");
#endif
   CString strFilter, strTitle;
   CFileDialog fd(true);
   fd.m_ofn.nFilterIndex   = 1;
   fd.m_ofn.nFileExtension = 1;
   fd.m_ofn.Flags         |= OFN_SHOWHELP;
   strTitle.LoadString(IDS_LOAD_COLORS);
   fd.m_ofn.lpstrTitle     = LPCTSTR(strTitle);
   strFilter.LoadString(IDS_CARA_COLORS);
   fd.m_ofn.lpstrFilter    = LPCTSTR(strFilter);
   for (int i=strFilter.GetLength()-1; i>=0; i--) if (fd.m_ofn.lpstrFilter[i] == '\n') ((char*)fd.m_ofn.lpstrFilter)[i] = 0;
   fd.m_ofn.nMaxCustFilter = 1;
   fd.m_ofn.lpstrDefExt    = &fd.m_ofn.lpstrFilter[strlen(fd.m_ofn.lpstrFilter)];

   if (!m_strDefaultColorDir.IsEmpty()) SetCurrentDirectory(m_strDefaultColorDir);

   CFile file;
   if ((fd.DoModal() == IDOK) && (fd.m_ofn.lpstrFileTitle != NULL) && file.Open(fd.m_ofn.lpstrFileTitle, CFile::modeRead))
   {
      CCARA3DVDoc *pDoc = (CCARA3DVDoc*) GetDocument();
      CAutoCriticalSection csAuto(pDoc->m_pcsDocumentData);
      COLORREF bkcolor, objcolor;
      CArchive ar(&file, CArchive::load);
      ar.m_pDocument = pDoc;

      if (pDoc->ReadFileHeader(ar))    // Version >= 1.10
      {
         unsigned long lChecksum = pDoc->m_FileHeader.GetChecksum();
         pDoc->m_FileHeader.CalcChecksum();
         pDoc->m_LevelColor.ReadColors(ar);
         CLabel::ArchiveRead(ar, &objcolor, sizeof(COLORREF), true);
         CLabel::ArchiveRead(ar, &bkcolor, sizeof(COLORREF), true);
         if (pDoc->m_FileHeader.IsValid(lChecksum))
         {
            m_pGLView->SetObjectColor(objcolor);
         }
         else
         {
            pDoc->SetDefaultColors();
            bkcolor = RGB(255,255,255);
         }
         m_pGLView->SetBkColor(bkcolor);
         ((CCaraWinApp*)AfxGetApp())->SetBackgroundColor(bkcolor);
      }
      else                             // alte Version
      {
         if (pDoc->m_LevelColor.ReadColors(ar))
         {
            ar.Read(&objcolor, sizeof(COLORREF));
            ar.Read(&bkcolor, sizeof(COLORREF));
            m_pGLView->SetObjectColor(objcolor);
            m_pGLView->SetBkColor(bkcolor);
            ((CCaraWinApp*)AfxGetApp())->SetBackgroundColor(bkcolor);
         }
         else
         {
            pDoc->SetDefaultColors();
            bkcolor = RGB(255,255,255);
         }
         m_pGLView->SetBkColor(bkcolor);
         ((CCaraWinApp*)AfxGetApp())->SetBackgroundColor(bkcolor);
      }
      ar.Close();
      file.Close();
      pDoc->SetModifiedFlag();
      m_pGLView->InvalidateList(LIST_OBJECT);
      m_pGLView->InvalidateGridList();
      int nCount = GetCurrentDirectory(0,NULL);
      if (nCount)
      {
         GetCurrentDirectory(nCount, m_strDefaultColorDir.GetBufferSetLength(nCount));
      }
      
      CCARA3DVApp *pApp = (CCARA3DVApp*) AfxGetApp();
      if (pApp->m_pMainWnd)
         ((CMainFrame*)pApp->m_pMainWnd)->InvalidateFrameContent();
   }
}

void CCARA3DVView::OnOptionsResetColors() 
{
   CCARA3DVDoc *pDoc = (CCARA3DVDoc*) GetDocument();
   CAutoCriticalSection csAuto(pDoc->m_pcsDocumentData);
   double dMinLevel = pDoc->m_LevelColor.GetMinLevel();
   double dMaxLevel = pDoc->m_LevelColor.GetMaxLevel();
   pDoc->SetDefaultColors();
   pDoc->SetModifiedFlag();
   pDoc->m_LevelColor.SetMinMaxLevel(dMinLevel, dMaxLevel);
   m_pGLView->InvalidateList(LIST_OBJECT);
   m_pGLView->InvalidateGridList();
   CCARA3DVApp *pApp = (CCARA3DVApp*) AfxGetApp();
   if (pApp->m_pMainWnd)
      ((CMainFrame*)pApp->m_pMainWnd)->InvalidateFrameContent();
}

void CCARA3DVView::SetDefaultOptions()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::SetDefaultOptions");
#endif
   const  ETS3DGL_Volume *pOV = m_pGLView->GetObjectVolume();
   const  ETS3DGL_Volume *pVV = m_pGLView->GetVolume();
   if (pOV && pVV)
   {
      CObjectTransformation *pOT = m_pGLView->GetObjectTransformation();
      switch (m_pGLView->GetObjectType())
      {
         case PLAN_GRAPH:
            pOT->m_vScale  = CVector(3.0, 4.0, 3.0);
            break;
         case POLAR_GRAPH:
            pOT->m_vScale  = CVector(4.0, 4.0, 4.0);
            m_pGLView->InvalidateGridList();
            break;
         default:
            break;
      }

      pOT->m_vAnchor = CVector(0.5 * (pOV->Far   + pOV->Near  ),
                               0.5 * (pOV->Top   + pOV->Bottom),
                               0.5 * (pOV->Right + pOV->Left  ));

      pOT->m_vTranslation= CVector(- Vx(pOT->m_vAnchor),
                                   - Vy(pOT->m_vAnchor),
                                   -(Vz(pOT->m_vAnchor) + 0.5 * (pVV->Near + pVV->Far)));
      
      m_pGLView->DefaultLevel();
      m_pGLView->InvalidateList(LIST_MATRIX);

      GetDocument()->CalculateLightPosition();
   }
}

void CCARA3DVView::OnOptionsSetDefault()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnOptionsSetDefault");
#endif
   SetDefaultOptions();
}

void CCARA3DVView::OnOptionsView() 
{
   BEGIN("CCARA3DVView::OnOptionsView");
   if (!m_pViewParams)
   {
      m_pViewParams = new CViewParams;                         // ViewDialog erstellen
      if (!m_pViewParams) return;
      m_pViewParams->m_ShadeColors.Attach(new CColorArray);
      m_pViewParams->m_BkGndColor.Attach( new CColorArray);
      m_pViewParams->m_ObjectColor.Attach(new CColorArray);
      m_pViewParams->m_BkGndColor.SetColorMode(0);    // Farbdialog Initialisieren
      m_pViewParams->m_BkGndColor.SetArraySize(1);
      m_pViewParams->m_ObjectColor.SetColorMode(0);
      m_pViewParams->m_ObjectColor.SetArraySize(1);
   }
   if (m_pViewParams)
   {
      if (m_pViewParams->IsActive())
      {
         if (m_pViewParams->IsIconic()) m_pViewParams->ShowWindow(SW_RESTORE);
         else                           m_pViewParams->MiniMize();
      }
      else
      {
         UpdateViewParamsDlg();
         m_pViewParams->Create(IDD_VIEW_DIALOG, AfxGetApp()->m_pMainWnd);
      }
   }
}

void CCARA3DVView::UpdateViewParamsDlg()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::UpdateViewParamsDlg");
#endif
   if (m_pViewParams)
   {
      CCARA3DVDoc *pDoc = GetDocument();
      ASSERT_VALID(pDoc);
      m_pViewParams->m_ShadeColors          = pDoc->m_LevelColor;
      COLORREF color = ((CCaraWinApp*)AfxGetApp())->GetBackgroundColor();
      m_pViewParams->m_BkGndColor.SetColors(&color, 1);
      color          = m_pGLView->GetObjectColor();
      m_pViewParams->m_ObjectColor.SetColors(&color, 1);
      m_pViewParams->m_nNumColors           = pDoc->m_LevelColor.GetNumColor();
      m_pViewParams->m_bInterpolate         = pDoc->m_bInterpolate;
      m_pViewParams->m_bInterpolateColor    = (pDoc->m_LevelColor.GetColorMode()==CV_INTERPOLATE) ? true : false;
      m_pViewParams->m_bShadingSmooth       = pDoc->m_bSmoothShading;
      m_pViewParams->m_nDetail              = pDoc->m_nDetail;
      m_pViewParams->m_nProjection          = ((m_pGLView->GetModes() & ETS3D_PR_ORTHOGONAL)!=0) ? 1 : 0;
      m_pViewParams->m_dObjectZoom          = Vx(pDoc->m_ObjTran.m_vScale);
      m_pViewParams->m_dValueZoom           = Vy(pDoc->m_ObjTran.m_vScale);
      m_pViewParams->m_dMinLevel            = pDoc->m_LevelColor.GetMinLevel();
      m_pViewParams->m_dMaxLevel            = pDoc->m_LevelColor.GetMaxLevel();
      m_pViewParams->m_strZ_Unit            = m_pGLView->GetLevelUnit();
      C3DGrid *pG = m_pGLView->GetGrid();
      m_pViewParams->m_Scale.m_ScalePainter.m_Ntx.nmode  = pG->GetNumModeLevel();
      m_pViewParams->m_Scale.m_ScalePainter.m_Ntx.nround = pG->GetRoundingLevel();
      if (m_pViewParams->IsActive() && !m_pViewParams->IsIconic())
      {
         m_pViewParams->UpdateData(false);
         m_pViewParams->m_Scale.InvalidateRect(NULL);
         m_pViewParams->m_ShadeColors.InvalidateRect(NULL);
         m_pViewParams->m_BkGndColor.InvalidateRect(NULL);
         m_pViewParams->m_ObjectColor.InvalidateRect(NULL);
      }
   }
}

void CCARA3DVView::OnOptionsLight() 
{
   BEGIN("CCARA3DVView::OnOptionsLight");
   if (!m_pLightDlg)
   {
      m_pLightDlg = new CLightDlg;
      if (!m_pLightDlg) return;
   }
   if (m_pLightDlg)
   {
      if (m_pLightDlg->IsActive())
      {
         if (m_pLightDlg->IsIconic()) m_pLightDlg->ShowWindow(SW_RESTORE);
         else                         m_pLightDlg->MiniMize();
      }
      else
      {
         UpdateLightDlg();
         m_pLightDlg->Create(IDD_LIGHT_DIALOG, AfxGetApp()->m_pMainWnd);
      }
   }
}

void CCARA3DVView::UpdateLightDlg()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::UpdateLightDlg");
#endif
   if (m_pLightDlg)
   {
      CCARA3DVDoc *pDoc = GetDocument();
      ASSERT_VALID(pDoc);
      m_pLightDlg->m_fMaxDim   = (float)m_pGLView->GetObjectDimension();
      m_pLightDlg->m_pLights   = (Cara3DLight*)pDoc->m_Light;
      m_pLightDlg->m_pfAmbient = pDoc->m_pfAmbientColor;
      if (m_pLightDlg->m_nSelectedLight == 1)
      {
         m_pLightDlg->m_fLightAnglePhi   = pDoc->m_Light[0].fPhi;
         m_pLightDlg->m_fLightAngleTheta = pDoc->m_Light[0].fTheta;
      }
      if (m_pLightDlg->IsActive() && !m_pLightDlg->IsIconic())
      {
         m_pLightDlg->UpdateData(false);
      }
   }
}

void CCARA3DVView::OnOptionsStandard()
{
   BEGIN("CCARA3DVView::OnOptionsStandard");
   if (!m_pStdDlg)
   {
      m_pStdDlg= new CStandardDlg;
      if (!m_pStdDlg) return;
   }
   if (m_pStdDlg)
   {
      if (m_pStdDlg->IsActive())
      {
         if (m_pStdDlg->IsIconic()) m_pStdDlg->ShowWindow(SW_RESTORE);
         else                       m_pStdDlg->MiniMize();
      }
      else
      {
         UpdateStandardDlg();
         m_pStdDlg->Create(IDD_STANDARD, AfxGetApp()->m_pMainWnd);
      }
   }
}

void CCARA3DVView::UpdateStandardDlg()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::UpdateStandardDlg");
#endif
   if (m_pStdDlg)
   {
      CCARA3DVDoc *pDoc = GetDocument();
      ASSERT_VALID(pDoc);
      m_pStdDlg->m_nNumMode         = pDoc->m_Grid.GetNumMode();
      m_pStdDlg->m_nNumModeZ        = pDoc->m_Grid.GetNumModeLevel();
      m_pStdDlg->m_nRounding        = pDoc->m_Grid.GetRounding();
      m_pStdDlg->m_nRoundingZ       = pDoc->m_Grid.GetRoundingLevel();
      m_pStdDlg->m_dGridStepX       = pDoc->m_Grid.GetGridStepX();
      m_pStdDlg->m_dGridStepZ       = pDoc->m_Grid.GetGridStepZ();
      m_pStdDlg->m_dGridStepLevel   = pDoc->m_Grid.GetGridStepLevel();
      m_pStdDlg->m_GridFont         = pDoc->m_Grid.GetFont();
      m_pStdDlg->m_bEffect          = (m_nEffectTimer != 0) ? 1 : 0;
      m_pStdDlg->m_bIntelliGrid     = ((pDoc->m_Grid.GetAxes()&SHOW_INTELLI_GRID)!=0) ? true : false;
      if (m_pStdDlg->IsActive() && !m_pStdDlg->IsIconic())
      {
         m_pStdDlg->UpdateData(false);
      }
   }
}

LRESULT CCARA3DVView::OnApplyToView(WPARAM wParam, LPARAM lParam)
{
   BEGIN("CCARA3DVView::OnApplyToView");
   if (lParam)
   {
      CCARA3DVDoc *pDoc = GetDocument();
      CNonModalDlg *pDlg = (CNonModalDlg*)lParam;
      if (pDlg == m_pViewParams)
      {
         if (pDlg->m_dwChanges & VIEWPARAMS_BKGNDCOLOR)
         {
            REPORT("VIEWPARAMS_BKGNDCOLOR");
            COLORREF BkGndColor = m_pViewParams->m_BkGndColor.GetColor(0);
            ((CCaraWinApp*)AfxGetApp())->SetBackgroundColor(BkGndColor);
            m_pGLView->SetBkColor(BkGndColor);
            m_pGLView->InvalidateGridList();
            m_pGLView->InvalidateList(LIST_AXES);
         }

         CAutoCriticalSection csAuto(pDoc->m_pcsDocumentData);

         if (pDlg->m_dwChanges & VIEWPARAMS_PROJECTION)
         {
            REPORT("VIEWPARAMS_PROJECTION");
            m_pGLView->SetModes((m_pViewParams->m_nProjection == 0) ? ETS3D_PR_PERSPECTIVE : ETS3D_PR_ORTHOGONAL, ETS3D_PR_CLEAR);
            pDoc->m_nProjection = m_pViewParams->m_nProjection;
         }
         if (pDlg->m_dwChanges & VIEWPARAMS_ZOOM)
         {
            REPORT("VIEWPARAMS_ZOOM");
            pDoc->m_ObjTran.m_vScale = CVector(m_pViewParams->m_dObjectZoom, m_pViewParams->m_dValueZoom, m_pViewParams->m_dObjectZoom);
            m_pGLView->InvalidateList(LIST_MATRIX);
         }
         if (pDlg->m_dwChanges & VIEWPARAMS_OBJECTCOLOR)
         {
            REPORT("VIEWPARAMS_OBJECTCOLOR");
            m_pGLView->SetObjectColor(m_pViewParams->m_ObjectColor.GetColor(0));
            m_pGLView->InvalidateList(LIST_OBJECT);
         }
         if (pDlg->m_dwChanges & VIEWPARAMS_INTERPOLATE)
         {
            REPORT("VIEWPARAMS_INTERPOLATE");
            pDoc->m_bInterpolate = (m_pViewParams->m_bInterpolate != 0) ? true : false;
            m_pGLView->InvalidateList(LIST_OBJECT);
         }
         if (pDlg->m_dwChanges & VIEWPARAMS_SHADECOLORS)
         {
            REPORT("VIEWPARAMS_SHADECOLORS");
            pDoc->m_LevelColor = m_pViewParams->m_ShadeColors;
            m_pGLView->InvalidateList(LIST_OBJECT);
            m_pGLView->InvalidateGridList();
         }
         if (pDlg->m_dwChanges & VIEWPARAMS_MINMAXLEVEL)
         {
            REPORT("VIEWPARAMS_MINMAXLEVEL");
//            double dNorm   = m_pGLView->GetNormFactor();
//            double dOffset = m_pGLView->GetOffset();
//            pDoc->m_LevelColor.SetMinMaxLevel(dOffset+m_pViewParams->m_dMinLevel*dNorm, dOffset+m_pViewParams->m_dMaxLevel*dNorm);
            pDoc->m_LevelColor.SetMinMaxLevel(m_pViewParams->m_dMinLevel, m_pViewParams->m_dMaxLevel);
            m_pGLView->InvalidateList(LIST_OBJECT);
            m_pGLView->InvalidateGridList();
         }
         if (pDlg->m_dwChanges & VIEWPARAMS_DETAIL)
         {
            REPORT("VIEWPARAMS_DETAIL");
            if (pDoc->m_nDetail != m_pViewParams->m_nDetail)
            {
               pDoc->m_nDetail = m_pViewParams->m_nDetail;
               m_pGLView->InvalidateList(LIST_OBJECT);
            }
         }
         if (pDlg->m_dwChanges & VIEWPARAMS_SHADING)
         {
            REPORT("VIEWPARAMS_SHADING");
            pDoc->m_bSmoothShading = (m_pViewParams->m_bShadingSmooth!=0) ? true : false;
            m_pGLView->InvalidateCommand(PRE_CMD);
            m_pGLView->InvalidateList(LIST_OBJECT);
         }
         return IDD_VIEW_DIALOG;
      }
      else if (pDlg == m_pLightDlg)
      {
         if (0 == m_pLightDlg->m_nSelectedLight)
         {
            m_pLightDlg->ReturnAmbientLight(pDoc->m_pfAmbientColor);
            m_pGLView->InvalidateCommand(PRE_CMD);
         }
         else if ((m_pLightDlg->m_nSelectedLight > 0) && (m_pLightDlg->m_nSelectedLight <= NO_OF_LIGHTS))
         {
            CAutoCriticalSection csAuto(pDoc->m_pcsDocumentData);

            int nLight = m_pLightDlg->m_nSelectedLight - 1;
            m_pLightDlg->ReturnLightParam(&pDoc->m_Light[nLight]);
            if (pDoc->m_bLighting)
            {
               pDoc->CalculateLightPosition(nLight);
               m_pGLView->InvalidateCommand(LIST_LIGHT + nLight);
               m_pGLView->InvalidateList(   LIST_LIGHT + nLight);
            }
         }
         return IDD_LIGHT_DIALOG;
      }
      else if (pDlg == m_pStdDlg)
      {
         bool bUpdateObject = false,
              bProtected    = false;
         if (pDoc->m_Grid.DoDrawGrid() || m_pGLView->ShowPins())
            m_pGLView->ProtectData(), bProtected = true;

         {  // probably protected Block
            if (pDlg->m_dwChanges & STDDLG_GRID_TYPE)
            {
               if (m_pStdDlg->m_bIntelliGrid) m_dwAxes |=  SHOW_INTELLI_GRID;
               else                           m_dwAxes &= ~SHOW_INTELLI_GRID;
               pDoc->m_Grid.SetAxes(m_dwAxes);
               m_pGLView->PerformGridList(true, true);
            }
            if (pDlg->m_dwChanges & STDDLG_GRIDSTEP)
            {
               pDoc->m_Grid.SetStep(m_pStdDlg->m_dGridStepX, m_pStdDlg->m_dGridStepLevel, m_pStdDlg->m_dGridStepZ);
               bUpdateObject = true;
            }
            if (pDlg->m_dwChanges & STDDLG_GRIDROUND)
            {
               pDoc->m_Grid.SetNumMode(      m_pStdDlg->m_nNumMode );
               pDoc->m_Grid.SetNumModeLevel( m_pStdDlg->m_nNumModeZ);
               pDoc->m_Grid.SetRounding(     m_pStdDlg->m_nRounding);
               pDoc->m_Grid.SetRoundingLevel(m_pStdDlg->m_nRoundingZ);
            }
            if (pDlg->m_dwChanges & STDDLG_GRIDFONT)
            {
               pDoc->m_Grid.SetFont(m_pStdDlg->m_GridFont);
            }
            if (bProtected)
            {
               m_pGLView->ReleaseData();
               m_pGLView->InvalidateGridList();
               if (bUpdateObject) m_pGLView->InvalidateList(LIST_OBJECT);
            }
         }
         if (pDlg->m_dwChanges & STDDLG_EFFECTS)
         {
            if (m_pStdDlg->m_bEffect) 
               m_nEffectTimer = SetTimer(EVENT_EFFECTTIMER, TIME_EFFECTTIMER, NULL);
            else
               KillTimerSave(m_nEffectTimer);
         }
         return IDD_STANDARD;
      }
      if (pDlg->m_dwChanges)
      {
         pDoc->SetModifiedFlag();
      }
   }
   else if (wParam == INVALIDATE_VIEW)
   {
      m_pGLView->InvalidateView();
   }
   return 0;
}

void CCARA3DVView::OnViewLight()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnViewLight");
#endif
   CCARA3DVDoc          *pDoc = GetDocument();
   CAutoCriticalSection  csAuto(pDoc->m_pcsDocumentData);
   bool                  bOldState = pDoc->m_bLighting;

   pDoc->m_bLighting = !pDoc->m_bLighting;
   if (!m_pGLView->IsALightOn()) pDoc->m_bLighting = false;

   m_pGLView->PerformList(LIST_LIGHT, pDoc->m_bLighting);
   m_pGLView->PerformCommand(LIST_LIGHT, pDoc->m_bLighting);

   if (bOldState != pDoc->m_bLighting)
      InvalidateLists();
}

void CCARA3DVView::InvalidateLists()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::InvalidateLists");
#endif
   CCARA3DVDoc *pDoc = GetDocument();
   int i;
   for (i=0; i<NO_OF_LIGHTS; i++)
   {
      m_pGLView->PerformList(LIST_LIGHT+i, pDoc->m_bLighting && pDoc->m_Light[i].glLight.m_bEnabled);
      m_pGLView->InvalidateCommand(LIST_LIGHT+i);
      m_pGLView->InvalidateList(LIST_LIGHT+i);
   }

   m_pGLView->PerformList(LIST_PINS, m_pGLView->ShowPins(-1));
   C3DGrid *pG = m_pGLView->GetGrid();
   m_pGLView->PerformList(LIST_AXES, pG->DoDrawAxes());
   m_pGLView->PerformGridList(pG->DoDrawGrid(), true);

   m_pGLView->InvalidateCommand(PRE_CMD);
   m_pGLView->InvalidateList(LIST_OBJECT);
   m_pGLView->InvalidateList(LIST_MATRIX);
   m_pGLView->InvalidateGridList();
   m_pGLView->InvalidateList(LIST_PINS);
   m_pGLView->InvalidateList(LIST_AXES);
}

BOOL CCARA3DVView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   if ((nCode == CN_COMMAND) && (pExtra == NULL))
   {
      CCARA3DVDoc          *pDoc = GetDocument();
      CAutoCriticalSection  csAuto(pDoc->m_pcsDocumentData);
      if ((nID >= ID_VIEW_GRID) && (nID <= ID_VIEW_NO_UNITS))    // Visibility of Grid
      {
         switch (nID)
         {
            case ID_VIEW_GRID:
               if (m_dwAxes&SHOW_GRID) m_dwAxes &= ~SHOW_GRID;
               else                    m_dwAxes |=  SHOW_GRID;
               m_pGLView->InvalidateList(LIST_OBJECT);
               break;
            case ID_VIEW_X_UNIT:
               if (m_dwAxes&SHOW_X_UNIT) m_dwAxes &= ~SHOW_X_UNIT;
               else                      m_dwAxes |=  SHOW_X_UNIT;
               break;
            case ID_VIEW_Y_UNIT:
               if (m_dwAxes&SHOW_Y_UNIT) m_dwAxes &= ~SHOW_Y_UNIT;
               else                      m_dwAxes |=  SHOW_Y_UNIT;
               break;
            case ID_VIEW_Z_UNIT:
               if (m_dwAxes&SHOW_Z_UNIT) m_dwAxes &= ~SHOW_Z_UNIT;
               else                      m_dwAxes |=  SHOW_Z_UNIT;
               break;
            case ID_VIEW_ALL_UNITS:
               m_dwAxes |= (SHOW_X_UNIT|SHOW_Y_UNIT|SHOW_Z_UNIT);
               break;
            case ID_VIEW_NO_UNITS:
               m_dwAxes &= ~(SHOW_X_UNIT|SHOW_Y_UNIT|SHOW_Z_UNIT);
               break;
         }
         pDoc->m_Grid.SetAxes(m_dwAxes);
         m_pGLView->PerformGridList(((m_dwAxes&SHOW_GRID) != 0) ? true : false, true);
         m_pGLView->InvalidateGridList();
         return true;
      }
      else if ((nID >= ID_VIEW_AXIS) && (nID <= ID_VIEW_NO_CHARACTERS))
      {
         // Visibility of Axes and Grid
         switch (nID)
         {
            case ID_VIEW_AXIS:
               if (m_dwAxes&SHOW_AXIS) m_dwAxes &= ~SHOW_AXIS;
               else                    m_dwAxes |=  SHOW_AXIS;
               break;
            case ID_VIEW_X_CHARACTER:
               if (m_dwAxes&SHOW_X_CHARACTER) m_dwAxes &= ~SHOW_X_CHARACTER;
               else                           m_dwAxes |=  SHOW_X_CHARACTER;
               break;
            case ID_VIEW_Y_CHARACTER:
               if (m_dwAxes&SHOW_Y_CHARACTER) m_dwAxes &= ~SHOW_Y_CHARACTER;
               else                           m_dwAxes |=  SHOW_Y_CHARACTER;
               break;
            case ID_VIEW_Z_CHARACTER:
               if (m_dwAxes&SHOW_Z_CHARACTER) m_dwAxes &= ~SHOW_Z_CHARACTER;
               else                           m_dwAxes |=  SHOW_Z_CHARACTER;
               break;
            case ID_VIEW_ALL_CHARACTERS:
               m_dwAxes |= (SHOW_X_CHARACTER|SHOW_Y_CHARACTER|SHOW_Z_CHARACTER);
               break;
            case ID_VIEW_NO_CHARACTERS:
               m_dwAxes &= ~(SHOW_X_CHARACTER|SHOW_Y_CHARACTER|SHOW_Z_CHARACTER);
               break;
         }
         pDoc->m_Grid.SetAxes(m_dwAxes);
         m_pGLView->PerformList(LIST_AXES, ((m_dwAxes&SHOW_AXIS) != 0) ? true : false);
         m_pGLView->InvalidateList(LIST_AXES);
         return true;
      }
      bool bFound = true;
      switch (nID)
      {
         // Moving the Object
         case ID_FUNC_MOVE_DOWN:    return OnFuncMoveObject(CVector( 0,-2, 0));
         case ID_FUNC_MOVE_UP:      return OnFuncMoveObject(CVector( 0, 2, 0));
         case ID_FUNC_MOVE_LEFT:    return OnFuncMoveObject(CVector(-2, 0, 0));
         case ID_FUNC_MOVE_RIGHT:   return OnFuncMoveObject(CVector( 2, 0, 0));
         case ID_FUNC_FORWARD:      return OnFuncMoveObject(CVector( 0, 0, 2));
         case ID_FUNC_BACKWARD:     return OnFuncMoveObject(CVector( 0, 0,-2));
         // Rotating the Object
         case ID_FUNC_ROTATE_LEFT:  return OnFuncRotateObject(-1, 0);
         case ID_FUNC_ROTATE_RIGHT: return OnFuncRotateObject( 1, 0);
         case ID_FUNC_ROTATE_UP:    return OnFuncRotateObject( 0,-1);
         case ID_FUNC_ROTATE_DOWN:  return OnFuncRotateObject( 0, 1);
         // Zooming the Object
         case ID_FUNC_ZOOM:         return OnFuncZoomObject(CVector( 0.1, 0.0,  0.1));
         case ID_FUNC_REZOOM:       return OnFuncZoomObject(CVector(-0.1, 0.0, -0.1));
         case ID_FUNC_ZOOM_LEVEL:   return OnFuncZoomObject(CVector( 0.0, 0.1,  0.0));
         case ID_FUNC_REZOOM_LEVEL: return OnFuncZoomObject(CVector( 0.0,-0.1,  0.0));
         // Controlling the Pictures of the Object
         case ID_FUNC_FILMSTEPPLUS:  OnFuncFilmstepplus();           break;
         case ID_FUNC_FILMSTEPMINUS: OnFuncFilmstepminus();          break;
         case ID_FUNC_FILMPAUSE:     m_nFilmRun = FILM_PAUSE;        break;
         case ID_FUNC_FILMREPEAT:    m_bFilmRepeat = !m_bFilmRepeat; break;
         case ID_FUNC_FILM_TIMER:
            m_bFilmTimer = !m_bFilmTimer;
            if ((m_nFilmRun != FILM_PAUSE) && !m_bFilmTimer)
            {
               PostMessage(WM_TIMER, EVENT_RENDER_READY, 0);
            } break;
         case ID_FUNC_FILMSTART:
            m_nFilmRun = FILM_START;
            if (!m_bFilmTimer) OnFuncFilmstepplus();
            break;
         case ID_FUNC_FILMSTARTR:
            m_nFilmRun = FILM_STARTR;
            if (!m_bFilmTimer) OnFuncFilmstepminus();
            break;
         case ID_FUNC_FILMSTEPBEGIN:
            m_pGLView->SetPictureIndex(0);
            m_pGLView->InvalidateList(LIST_OBJECT);
            SetFilmIndexToStatusPane();
            break;
         case ID_FUNC_FILMSTEPEND:
            m_pGLView->SetPictureIndex(m_pGLView->GetNoOfPictures()-1);
            m_pGLView->InvalidateList(LIST_OBJECT);
            SetFilmIndexToStatusPane();
            break;
         case ID_VIEW_LIGHT: OnViewLight(); break;
         case ID_VIEW_BOXPOS:
            m_pGLView->ShowPins(PINTYPE_BOX, !m_pGLView->ShowPins(PINTYPE_BOX));
            m_pGLView->PerformList(LIST_PINS, m_pGLView->ShowPins(-1));
            m_pGLView->InvalidateList(LIST_PINS);
            break;
         case ID_VIEW_LISTENERPOS:
            m_pGLView->ShowPins(PINTYPE_LISTENER, !m_pGLView->ShowPins(PINTYPE_LISTENER));
            m_pGLView->PerformList(LIST_PINS, m_pGLView->ShowPins(-1));
            m_pGLView->InvalidateList(LIST_PINS);
            break;
         case ID_VIEW_EXITERPOS:
            m_pGLView->ShowPins(PINTYPE_EXCITER, !m_pGLView->ShowPins(PINTYPE_EXCITER));
            m_pGLView->PerformList(LIST_PINS, m_pGLView->ShowPins(-1));
            m_pGLView->InvalidateList(LIST_PINS);
            break;
         case ID_VIEW_INTELLI_GRID:
            if (m_dwAxes&SHOW_INTELLI_GRID) m_dwAxes &= ~SHOW_INTELLI_GRID;
            else                            m_dwAxes |=  SHOW_INTELLI_GRID;
            m_pGLView->GetGrid()->SetAxes(m_dwAxes);
            m_pGLView->PerformGridList(m_pGLView->GetGrid()->DoDrawGrid(), true);
            m_pGLView->InvalidateGridList();
         default: bFound = false;
      }
      if (bFound) return true;
   }

   if (((nCode == CN_UPDATE_COMMAND_UI)||(nCode ==  0xFFFF)) && (pExtra != NULL))
   {
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
      if ((nID >= ID_FUNC_MOVE_UP) && (nID <= ID_FUNC_REZOOM))
      {
         pCmdUI->Enable(true);
         pCmdUI->m_bContinueRouting = FALSE;
         return true;
      }
      if ((nID >= ID_TIME_1) && (nID <= ID_TIME_10))
      {
         pCmdUI->Enable(true);
         if (pCmdUI->m_pMenu)
            pCmdUI->m_pMenu->CheckMenuRadioItem(ID_TIME_1, ID_TIME_10, m_nFilmTimeMax + ID_TIME_1, 0);
         return true;
      }
      bool bFound = true;
      bool bEnable;
      switch (nID)
      {
         case ID_FUNC_FILMSTART:
            bEnable = ((m_nFilmRun != FILM_START) && (m_pGLView->GetPictureIndex() < (m_pGLView->GetNoOfPictures()-1))) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && (m_nFilmRun == FILM_START));
            break;
         case ID_FUNC_FILMSTARTR:
            bEnable = ((m_nFilmRun != FILM_STARTR) && (m_pGLView->GetPictureIndex() > 0)) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && (m_nFilmRun == FILM_STARTR));
            break;
         case ID_FUNC_FILMSTEPPLUS: case ID_FUNC_FILMSTEPEND:
            pCmdUI->Enable((m_nFilmRun == FILM_PAUSE ) && (m_pGLView->GetPictureIndex() < (m_pGLView->GetNoOfPictures()-1)));
            break;
         case ID_FUNC_FILMSTEPMINUS: case ID_FUNC_FILMSTEPBEGIN:
            pCmdUI->Enable((m_nFilmRun == FILM_PAUSE) && (m_pGLView->GetPictureIndex() > 0));
            break;
         case ID_FUNC_FILMPAUSE:
            bEnable = (m_nFilmRun != FILM_PAUSE) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && (m_nFilmRun == FILM_PAUSE));
            break;
         case ID_FUNC_FILM_TIMER: pCmdUI->Enable(); pCmdUI->SetCheck(m_bFilmTimer);                                            break;
         case ID_FUNC_FILMREPEAT: pCmdUI->Enable(); pCmdUI->SetCheck(m_bFilmRepeat);                                           break;
         case ID_VIEW_AXIS:       pCmdUI->Enable(); pCmdUI->SetCheck((m_dwAxes&SHOW_AXIS)!=0);                                 break;
         case ID_VIEW_GRID:       pCmdUI->Enable(); pCmdUI->SetCheck((m_dwAxes&SHOW_GRID)!=0);                                 break; 
         case ID_OPTIONS_LIGHT:   pCmdUI->Enable(); pCmdUI->SetCheck((m_pLightDlg   != NULL)&& m_pLightDlg->IsActive());       break;
         case ID_OPTIONS_STANDARD:pCmdUI->Enable(); pCmdUI->SetCheck((m_pStdDlg     != NULL)&& m_pStdDlg->IsActive()); break;
         case ID_OPTIONS_VIEW:    pCmdUI->Enable(); pCmdUI->SetCheck((m_pViewParams != NULL)&& m_pViewParams->IsActive());     break;
         case ID_VIEW_LIGHT:      pCmdUI->Enable(); pCmdUI->SetCheck(GetDocument()->m_bLighting ? true : false);               break;
         case ID_VIEW_FULL:       pCmdUI->Enable(); pCmdUI->SetCheck(m_hMenu!=NULL);                                           break;
         case IDCANCEL: pCmdUI->Enable(); break;
         case ID_VIEW_X_UNIT:
            bEnable = (m_dwAxes&SHOW_GRID) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && ((m_dwAxes&SHOW_X_UNIT)!=0));
            break;
         case ID_VIEW_Y_UNIT:
            bEnable = (m_dwAxes&SHOW_GRID) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && ((m_dwAxes&SHOW_Y_UNIT)!=0));
            break;
         case ID_VIEW_Z_UNIT:     
            bEnable = (m_dwAxes&SHOW_GRID) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && ((m_dwAxes&SHOW_Z_UNIT)!=0));
            break;
         case ID_VIEW_ALL_UNITS:     
            bEnable = (m_dwAxes&SHOW_GRID) ? true : false;
            pCmdUI->Enable(bEnable);
            break;
         case ID_VIEW_NO_UNITS:     
            bEnable = (m_dwAxes&SHOW_GRID) ? true : false;
            pCmdUI->Enable(bEnable);
            break;
         case ID_VIEW_INTELLI_GRID:     
            bEnable = (m_dwAxes&SHOW_GRID) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && ((m_dwAxes&SHOW_INTELLI_GRID)!=0));
            break;
         case ID_VIEW_X_CHARACTER:
            bEnable = (m_dwAxes&SHOW_AXIS) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && ((m_dwAxes&SHOW_X_CHARACTER)!=0));
            break;
         case ID_VIEW_Y_CHARACTER:
            bEnable = (m_dwAxes&SHOW_AXIS) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && ((m_dwAxes&SHOW_Y_CHARACTER)!=0));
            break;
         case ID_VIEW_Z_CHARACTER:
            bEnable = (m_dwAxes&SHOW_AXIS) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && ((m_dwAxes&SHOW_Z_CHARACTER)!=0));
            break;
         case ID_VIEW_ALL_CHARACTERS:     
            bEnable = (m_dwAxes&SHOW_AXIS) ? true : false;
            pCmdUI->Enable(bEnable);
            break;
         case ID_VIEW_NO_CHARACTERS:     
            bEnable = (m_dwAxes&SHOW_AXIS) ? true : false;
            pCmdUI->Enable(bEnable);
            break;
         case ID_VIEW_BOXPOS:
            bEnable = (m_pGLView->GetNoOfPins(PINTYPE_BOX) > 0) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(m_pGLView->ShowPins(PINTYPE_BOX)!=0);
            break;
         case ID_VIEW_LISTENERPOS:
            bEnable = (m_pGLView->GetNoOfPins(PINTYPE_LISTENER) > 0) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(m_pGLView->ShowPins(PINTYPE_LISTENER)!=0);
            break;
         case ID_VIEW_EXITERPOS:
            bEnable = (m_pGLView->GetNoOfPins(PINTYPE_EXCITER) > 0) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(m_pGLView->ShowPins(PINTYPE_EXCITER));
            break;
         case ID_VIEW_PHASEVALUES:
         {
            CCARA3DVDoc *pDoc = GetDocument();
            bEnable = ((FILM_PAUSE == m_nFilmRun) && (NULL != pDoc->m_p3DObject) && pDoc->m_p3DObject->PhaseValuesAvailable()) ? true : false;
            pCmdUI->Enable(bEnable);
            pCmdUI->SetCheck(bEnable && pDoc->m_p3DObject->PhaseValuesCalculated());
            break;
         }
         case ID_FILE_SAVE:
         {
            CCARA3DVDoc *pDoc = GetDocument();
            bEnable = (pDoc->IsModified() && (FILM_PAUSE == m_nFilmRun)) ? true : false;
            pCmdUI->Enable(bEnable);
            break;
         }
         case ID_EDIT_COPY_AS_DIB:    pCmdUI->Enable(m_pGLView->Is3DObjectAvailable()&&(m_nFilmRun == FILM_PAUSE)); break;
         case ID_FILE_SAVE_AS_BITMAP: pCmdUI->Enable(m_pGLView->Is3DObjectAvailable()&&(m_nFilmRun == FILM_PAUSE)); break;
         case ID_FILE_PRINT:          pCmdUI->Enable(m_pGLView->AreLabelsAvailable()&&(m_nFilmRun == FILM_PAUSE));  break;
         case ID_FILE_PRINT_PREVIEW:  pCmdUI->Enable((m_hMenu == NULL) && (m_nFilmRun == FILM_PAUSE)); break;
         case ID_FILE_PRINT_SETUP:    pCmdUI->Enable(true); break;
         default: bFound = false;
      }
      if (bFound)
      {
         pCmdUI->m_bContinueRouting = FALSE;
         return true;
      }
   }
	return CCaraView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CCARA3DVView::OnFuncFilmstepplus()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFuncFilmstepplus");
#endif
   CCARA3DVDoc* pDoc = GetDocument();
   pDoc->m_nPictureIndex++;
   if (pDoc->m_nPictureIndex < m_pGLView->GetNoOfPictures())
   {
      m_pGLView->InvalidateList(LIST_OBJECT);
   }
   else if (m_bFilmRepeat)
   {
      pDoc->m_nPictureIndex = 0;
      m_pGLView->InvalidateList(LIST_OBJECT);
   }
   else
   {
      pDoc->m_nPictureIndex = m_pGLView->GetNoOfPictures()-1;
      m_nFilmRun = FILM_PAUSE;
   }
   pDoc->SetModifiedFlag();
   SetFilmIndexToStatusPane();
   SaveReduceEffect();
}

void CCARA3DVView::OnFuncFilmstepminus()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFuncFilmstepminus");
#endif
   CCARA3DVDoc* pDoc = GetDocument();
   pDoc->m_nPictureIndex--;
   if (pDoc->m_nPictureIndex >= 0)
   {
      m_pGLView->InvalidateList(LIST_OBJECT);
   }
   else if (m_bFilmRepeat)
   {
      pDoc->m_nPictureIndex = m_pGLView->GetNoOfPictures()-1;
      m_pGLView->InvalidateList(LIST_OBJECT);
   }
   else
   {
      pDoc->m_nPictureIndex = 0;
      m_nFilmRun = FILM_PAUSE;
   }
   pDoc->SetModifiedFlag();
   SetFilmIndexToStatusPane();
   SaveReduceEffect();
}

void CCARA3DVView::OnFuncFilmTimeStep() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFuncFilmTimeStep");
#endif
   CMenu Menu;
   CWnd *pMainWnd = AfxGetApp()->m_pMainWnd;
   if (!pMainWnd) pMainWnd = this;
   Menu.LoadMenu(IDR_TIMER_MENU);
   CMenu *pContext = Menu.GetSubMenu(0);
   CPoint pt;
   GetCursorPos(&pt);
//   pContext->CheckMenuRadioItem(ID_TIME_1, ID_TIME_10, m_nFilmTimeMax + ID_TIME_1, 0);
   int nCmd = pContext->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, pMainWnd);
   if ((nCmd >= ID_TIME_1) && (nCmd <= ID_TIME_10))
      m_nFilmTimeMax = nCmd - ID_TIME_1;
   UpdateMousePos();
}

void CCARA3DVView::OnViewOptionAxis()
{
   CMenu Menu;
   CWnd *pMainWnd = AfxGetApp()->m_pMainWnd;
   if (!pMainWnd) pMainWnd = this;
   Menu.LoadMenu(IDR_VIEW_OPTION_AXIS);
   CMenu *pContext = Menu.GetSubMenu(0);
   CPoint pt;
   GetCursorPos(&pt);
//   pContext->CheckMenuItem(ID_VIEW_X_CHARACTER, MF_BYCOMMAND|((m_dwAxes&SHOW_X_CHARACTER) ? MF_CHECKED:MF_UNCHECKED));
//   pContext->CheckMenuItem(ID_VIEW_Y_CHARACTER, MF_BYCOMMAND|((m_dwAxes&SHOW_Y_CHARACTER) ? MF_CHECKED:MF_UNCHECKED));
//   pContext->CheckMenuItem(ID_VIEW_Z_CHARACTER, MF_BYCOMMAND|((m_dwAxes&SHOW_Z_CHARACTER) ? MF_CHECKED:MF_UNCHECKED));
   int nCmd = pContext->TrackPopupMenu(0, pt.x, pt.y, pMainWnd);
//   UpdateMousePos();
}

void CCARA3DVView::OnViewOptionGrid()
{
   CMenu Menu;
   CWnd *pMainWnd = AfxGetApp()->m_pMainWnd;
   if (!pMainWnd) pMainWnd = this;
   Menu.LoadMenu(IDR_VIEW_OPTION_GRID);
   CMenu *pContext = Menu.GetSubMenu(0);
   CPoint pt;
   GetCursorPos(&pt);
//   pContext->CheckMenuItem(ID_VIEW_X_UNIT, MF_BYCOMMAND|((m_dwAxes&SHOW_X_UNIT) ? MF_CHECKED:MF_UNCHECKED));
//   pContext->CheckMenuItem(ID_VIEW_Y_UNIT, MF_BYCOMMAND|((m_dwAxes&SHOW_Y_UNIT) ? MF_CHECKED:MF_UNCHECKED));
//   pContext->CheckMenuItem(ID_VIEW_Z_UNIT, MF_BYCOMMAND|((m_dwAxes&SHOW_Z_UNIT) ? MF_CHECKED:MF_UNCHECKED));
//   pContext->CheckMenuItem(ID_VIEW_INTELLI_GRID, MF_BYCOMMAND|((m_dwAxes&SHOW_INTELLI_GRID) ? MF_CHECKED:MF_UNCHECKED));
   pContext->TrackPopupMenu(0, pt.x, pt.y, pMainWnd);
//   UpdateMousePos();
}

void CCARA3DVView::SetFilmIndexToStatusPane() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::SetFilmIndexToStatusPane");
#endif
   CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->m_pMainWnd;
   CCARA3DVDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   if (pMainWnd)
   {
      CString format((LPCSTR)IDS_FORMAT_C_VALUE);
      CString text;
      double dVal = m_pGLView->GetSpecificValue(pDoc->m_nPictureIndex);
      if ((dVal > 1.0) || (fabs(dVal) < 1e-13))
      {
         int nPos = format.Find("%.3", 0);
         if (nPos != -1) format.SetAt(nPos+2, '1');
      }
      text.Format(format, dVal, m_pGLView->GetSpecificUnit());
      if (!m_PictureTextOld.IsEmpty()) text += m_PictureTextOld;
      pMainWnd->SetStatusPaneText(1, (char*)LPCSTR(text));
   }
}

BOOL CCARA3DVView::OnFuncMoveObject(CVector& vDir) 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFuncMoveObject");
#endif
   CCARA3DVDoc *pDoc = GetDocument();
   const  ETS3DGL_Volume *pVV = m_pGLView->GetVolume();
   CRect    rcClient;
   GetClientRect(&rcClient);
   double   dFactor = (pVV->Right-pVV->Left) * VIEW_FACTOR_X / (Vx(pDoc->m_ObjTran.m_vScale)*(rcClient.right-rcClient.left));
   pDoc->m_ObjTran.m_vTranslation += (vDir*dFactor);

   m_pGLView->InvalidateList(LIST_MATRIX);
   if (pDoc->m_Light[0].fDistance != 0.0f)
   {
      pDoc->CalculateLightPosition();
      m_pGLView->InvalidateList(LIST_LIGHT);
   }
   if (m_pGLView->GetObjectType() == POLAR_GRAPH)
   {
      m_pGLView->InvalidateGridList();
   }

   CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->m_pMainWnd;
   if (pMainWnd)
   {
      CString format((LPCSTR)IDS_FORMAT_OBJECT_POSITION);
      CString text;
      text.Format(format, (int)Vx(pDoc->m_ObjTran.m_vTranslation), ABS((int)(Vx(pDoc->m_ObjTran.m_vTranslation)*10)%10),
                          (int)Vy(pDoc->m_ObjTran.m_vTranslation), ABS((int)(Vy(pDoc->m_ObjTran.m_vTranslation)*10)%10),
                          (int)Vz(pDoc->m_ObjTran.m_vTranslation), ABS((int)(Vz(pDoc->m_ObjTran.m_vTranslation)*10)%10) );
      pMainWnd->SetStatusPaneText(1,(char*)LPCSTR(text));
   }
   SaveReduceEffect();
   pDoc->SetModifiedFlag();
   return true;
}

BOOL CCARA3DVView::OnFuncRotateObject(double dPhi, double dTheta)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFuncRotateObject");
#endif
   CCARA3DVDoc *pDoc = GetDocument();
   if (dPhi != 0.0)
   {
      pDoc->m_ObjTran.m_dPhi += dPhi;
      if (pDoc->m_ObjTran.m_dPhi > 359.0) pDoc->m_ObjTran.m_dPhi -= 360.0;
      if (pDoc->m_ObjTran.m_dPhi <   0.0) pDoc->m_ObjTran.m_dPhi += 360.0;
   }
   if (dTheta != 0.0)
   {
      pDoc->m_ObjTran.m_dTheta += dTheta;
      if (pDoc->m_ObjTran.m_dTheta >  90.0) pDoc->m_ObjTran.m_dTheta = 90.0;
      if (pDoc->m_ObjTran.m_dTheta < -90.0) pDoc->m_ObjTran.m_dTheta = -90.0;
   }
   m_pGLView->InvalidateList(LIST_MATRIX);
   if (pDoc->m_Grid.DoDrawGrid())
   {
      if (pDoc->m_Grid.GetAxes() & SHOW_INTELLI_GRID) m_pGLView->PerformGridList(true);
      pDoc->m_Grid.InvalidateGrid();
   }
   CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->m_pMainWnd;
   if (pMainWnd)
   {
      CString format((LPCSTR)IDS_FORMAT_OBJECT_ANGLE);
      CString text;
      text.Format(format, (int)pDoc->m_ObjTran.m_dTheta, ABS((int)(pDoc->m_ObjTran.m_dTheta*10)%10),
                          (int)pDoc->m_ObjTran.m_dPhi  , ABS((int)(pDoc->m_ObjTran.m_dPhi*10)%10));
      pMainWnd->SetStatusPaneText(1,(char*)LPCSTR(text));
   }
   SaveReduceEffect();
   pDoc->SetModifiedFlag();
   return true;
}

BOOL CCARA3DVView::OnFuncZoomObject(CVector& vZoom)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnFuncZoomObject");
#endif
   CCARA3DVDoc *pDoc = GetDocument();
   CVector vOld = pDoc->m_ObjTran.m_vScale;

   if (m_pGLView->GetObjectType() == POLAR_GRAPH)
   {
      vZoom = CVector(Vx(vZoom), Vx(vZoom), Vx(vZoom));
      m_pGLView->InvalidateGridList();
   }

   pDoc->m_ObjTran.m_vScale += vZoom;

   bool bChange = true;

   if (Vx(pDoc->m_ObjTran.m_vScale) < 0.0) bChange = false;
   if (Vy(pDoc->m_ObjTran.m_vScale) < 0.0) bChange = false;
   if (Vz(pDoc->m_ObjTran.m_vScale) < 0.0) bChange = false;

   if (!bChange)
   {
      pDoc->m_ObjTran.m_vScale = vOld;
      return false;
   }

   m_pGLView->InvalidateList(LIST_MATRIX);

   CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->m_pMainWnd;
   if (pMainWnd)
   {
      CString format((LPCSTR)IDS_FORMAT_OBJECT_ZOOM);
      CString text;
      text.Format(format, (int)Vx(pDoc->m_ObjTran.m_vScale), ABS((int)(Vx(pDoc->m_ObjTran.m_vScale)*100)%100),
                          (int)Vy(pDoc->m_ObjTran.m_vScale), ABS((int)(Vy(pDoc->m_ObjTran.m_vScale)*100)%100),
                          (int)Vz(pDoc->m_ObjTran.m_vScale), ABS((int)(Vz(pDoc->m_ObjTran.m_vScale)*100)%100));

      pMainWnd->SetStatusPaneText(1,(char*)LPCSTR(text));
   }
   if (pDoc->m_Light[0].fDistance != 0.0f)
   {
      pDoc->CalculateLightPosition();
      m_pGLView->InvalidateList(LIST_LIGHT);
   }
   SaveReduceEffect();
   UpdateViewParamsDlg();
   pDoc->SetModifiedFlag();
   return true;
}

void CCARA3DVView::OnMouseMove(UINT nFlags, CPoint point) 
{
   int      nSomethingHappened = 0;
   static CPoint OldPoint(point);

   CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->m_pMainWnd;
   if (pMainWnd->m_bHelpMode)
   {
      CView::OnMouseMove(nFlags, point);
      return;
   }

   CPoint diff = point - OldPoint;

   if ((nFlags & MK_CONTROL) && (nFlags &MK_LBUTTON) && (m_pGLView->Lighting()))
   {
      CCARA3DVDoc *pDoc = GetDocument();
      if (pDoc->m_bLighting)
      {
         pDoc->m_Light[0].fPhi += 0.5f*diff.x;
         if (pDoc->m_Light[0].fPhi > 180.0)
         {
            pDoc->m_Light[0].fPhi = -(360.0f-pDoc->m_Light[0].fPhi);
         }
         else if (pDoc->m_Light[0].fPhi < -180.0)
         {
            pDoc->m_Light[0].fPhi = 360.0f + pDoc->m_Light[0].fPhi;
         }

         if (diff.y < 0)
         {
            if (pDoc->m_Light[0].fTheta <  90.0) pDoc->m_Light[0].fTheta -= 0.5f*diff.y;
            if (pDoc->m_Light[0].fTheta >  90.0) pDoc->m_Light[0].fTheta = 90.0;
         }
         else  // diff.y < 0
         {
            if (pDoc->m_Light[0].fTheta > -90.0) pDoc->m_Light[0].fTheta -= 0.5f*diff.y;
            if (pDoc->m_Light[0].fTheta < -90.0) pDoc->m_Light[0].fTheta = -90.0;
         }
         if (pMainWnd)
         {
            CString format((LPCSTR)IDS_FORMAT_LIGHT_ANGLE);
            CString text;
            text.Format(format, (int)pDoc->m_Light[0].fTheta, ABS((int)(pDoc->m_Light[0].fTheta)*100)%100,
                                (int)pDoc->m_Light[0].fPhi  , ABS((int)(pDoc->m_Light[0].fPhi  )*100)%100);
            pMainWnd->SetStatusPaneText(1,(char*)LPCSTR(text));
         }
         UpdateLightDlg();
         pDoc->CalculateLightPosition();
         m_pGLView->InvalidateList(LIST_LIGHT);
         pDoc->SetModifiedFlag();
      }
   }
   else if (nFlags &MK_SHIFT)
   {
      // nSomethingHappened = 8;
   }
   else if (nFlags &MK_RBUTTON)
   {
      OnFuncMoveObject(CVector(diff.x, -diff.y, 0.0));
   }
   else if (nFlags &MK_LBUTTON)
   {
      CRect    rcClient;
      CCARA3DVDoc *pDoc = GetDocument();
      GetClientRect(&rcClient);
      CPoint ptCenter = rcClient.CenterPoint();
      double dFac = 0;
      if ((ptCenter.y > point.y)^(pDoc->m_ObjTran.m_dTheta <0.0)) dFac = -0.5;
      else                                                        dFac =  0.5;
      OnFuncRotateObject(dFac*diff.x, 0.5 * diff.y);
   }

   OldPoint = point;
   CView::OnMouseMove(nFlags, point);
}

BOOL CCARA3DVView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnMouseWheel");
#endif
   if ((nFlags & MK_CONTROL) && m_pGLView->Lighting())
   {
/*
      if (zDelta > 0) m_LP.fLightDistance += 1.0;
      else            m_LP.fLightDistance -= 1.0;
      NewLightPosition( LP_DISTANCE);
      CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->m_pMainWnd;
      if (pMainWnd)
      {
         CString format((LPCSTR)IDS_FORMAT_LIGHT_DISTANCE);
         CString text;
         text.Format(format, (int)m_LP.fLightDistance);
         pMainWnd->SetStatusPaneText(1,(char*)LPCSTR(text));
         m_nShowFilmIndex = 0;
      }
*/
   }
   else if (nFlags & MK_SHIFT)
   {
      if (zDelta > 0) OnFuncMoveObject(CVector(0,0,-1));
      else            OnFuncMoveObject(CVector(0,0,-1));
   }
   else
   {
      if (zDelta > 0) OnFuncZoomObject(CVector( 1, 0, 1));
      else            OnFuncZoomObject(CVector(-1, 0,-1));
   }
   return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CCARA3DVView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CCaraView::OnLButtonDown(nFlags, point);
   m_bMouseButtonDown = true;
}

void CCARA3DVView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   OnLButtonDown(nFlags, point);
}

void CCARA3DVView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   CCaraView::OnLButtonUp(nFlags, point);
   m_bMouseButtonDown = false;
}

void CCARA3DVView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CCaraView::OnRButtonDown(nFlags, point);
   m_bMouseButtonDown = true;
}

void CCARA3DVView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	OnRButtonDown(nFlags, point);
}

void CCARA3DVView::OnRButtonUp(UINT nFlags, CPoint point) 
{
   CCaraView::OnRButtonUp(nFlags, point);
   m_bMouseButtonDown = false;
}


// Film-Funktionen
void CCARA3DVView::SaveReduceEffect()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::SaveReduceEffect");
#endif
   if (m_nEffectTimer != 0)                                    // wenn Effektreduzierung
   {                                                           // eingeschaltet ist
      CCARA3DVDoc *pDoc = GetDocument();
      if (m_nDetailSave == -1)                                 // nur dann stimmt der
      {                                                        // Zustand, der im Dokument
         m_nDetailSave     = pDoc->m_nDetail;                  // gespeichert ist
         m_bLightOnSave    = pDoc->m_bLighting;
      }
      if (pDoc->m_bLighting)
      {
         pDoc->m_bLighting = false;                            // Licht aus
         InvalidateLists();
      }
      if (pDoc->m_nDetail > 0)                                 // Teilungsgrand auf 0
      {
         pDoc->m_nDetail = 0;
         m_pGLView->InvalidateList(LIST_OBJECT);
      }
   }
}

void CCARA3DVView::OnTimer(UINT nIDEvent) 
{
   switch (nIDEvent)
   {
      case EVENT_RENDER_TIMER:
      if (m_bFilmTimer && (m_nFilmRun != FILM_PAUSE) && (++m_nFilmTimeCount >= m_nFilmTimeMax))
      {
         if (m_nFilmRun == FILM_START)  OnFuncFilmstepplus();  // Vorwärts
         else                           OnFuncFilmstepminus(); // Rückwärts
         m_nFilmTimeCount = 0;
      } break;

      case EVENT_RENDER_READY:
      if (!m_bFilmTimer && (m_nFilmRun != FILM_PAUSE))
      {
         if (m_nFilmRun == FILM_START)  OnFuncFilmstepplus();  // Vorwärts
         else                           OnFuncFilmstepminus(); // Rückwärts
      }
      if (m_pStdDlg && m_pStdDlg->IsActive() && (m_pStdDlg->m_dwChanges&STDDLG_UPDATEGRIDSTEP) && !m_pStdDlg->IsIconic())
      {
         CCARA3DVDoc *pDoc = GetDocument();
         m_pStdDlg->m_dGridStepX       = pDoc->m_Grid.GetGridStepX();
         m_pStdDlg->m_dGridStepZ       = pDoc->m_Grid.GetGridStepZ();
         m_pStdDlg->m_dGridStepLevel   = pDoc->m_Grid.GetGridStepLevel();
         m_pStdDlg->m_dwChanges &= ~STDDLG_UPDATEGRIDSTEP;
         m_pStdDlg->UpdateData(false);
      } break;

      case EVENT_UPDATETIMER:
      {
         SetFilmIndexToStatusPane();
      } break;

      case EVENT_EFFECTTIMER:
      if (!m_bMouseButtonDown && !m_pGLView->IsRendering() && (m_nFilmRun == FILM_PAUSE))
      {
         if (m_nDetailSave != -1)
         {
            CCARA3DVDoc *pDoc = GetDocument();
            if (m_bLightOnSave && !pDoc->m_bLighting)
            {
               pDoc->m_bLighting = m_bLightOnSave;
               InvalidateLists();
            }
            if (pDoc->m_nDetail < m_nDetailSave)
            {
               pDoc->m_nDetail++;
               m_pGLView->InvalidateList(LIST_OBJECT);
            }
            else if (pDoc->m_nDetail == m_nDetailSave) m_nDetailSave = -1;
         }
      } break;
      
      case EVENT_INITIALIZE_GL:
      {
         CRect rcClient;
         CCARA3DVDoc *pDoc = GetDocument();
         GetClientRect(&rcClient);
         pDoc->InitCriticalSectionOfDoc();
         m_pGLView->AttachExternalCS(pDoc->m_pcsDocumentData);
         m_pGLView->SetModes(ETS3D_DESTROY_AT_DLL_TERMINATION, 0);
         m_pGLView->Create("ETS3DView", 0, &rcClient, m_hWnd);
      } break;
      
      case EVENT_INVALIDATE_GL:
      {
         InvalidateLists();
      } break;
      
      default: break;
   }

   CView::OnTimer(nIDEvent);
}

void CCARA3DVView::KillTimerSave(UINT& nID)
{
   BEGIN("CCARA3DVView::KillTimerSave");
   if (nID)
   {
      KillTimer(nID);
	   MSG msg;
      _asm CLD;
      ::PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE|PM_NOYIELD);
      nID = 0;
   }
}

LRESULT CCARA3DVView::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnCommandHelp");
#endif
   if (lParam != 0)
   {
      AfxGetApp()->WinHelp(lParam);
   }
   return true;
}

// View-Funktionen

void CCARA3DVView::OnViewPhasevalues() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnViewPhasevalues");
#endif
   CCARA3DVDoc *pDoc = GetDocument();
   if (pDoc->m_p3DObject && pDoc->m_p3DObject->PhaseValuesAvailable())
   {
      WaitOnRenderer();
      CAutoCriticalSection csAuto(pDoc->m_pcsDocumentData);
      if (pDoc->m_p3DObject->PhaseValuesCalculated())
      {
         pDoc->m_p3DObject->DeletePhaseValues();
         m_pGLView->SetPictureIndex(m_nPictureIndexOld);
         m_PictureTextOld.Empty();
      }
      else
      {
         m_nPictureIndexOld = m_pGLView->GetPictureIndex();
         m_PictureTextOld.Format(", %.3f [%s]", m_pGLView->GetSpecificValue(m_nPictureIndexOld), m_pGLView->GetSpecificUnit());
         pDoc->m_p3DObject->CalculatePhaseValues();
         m_pGLView->SetPictureIndex(0);
      }

      CalculateViewVolume();

      pDoc->m_Grid.InvalidateGrid();
      pDoc->m_Grid.InvalidateStep();
      CVector vScale = m_pGLView->GetObjectTransformation()->m_vScale;
      OnOptionsSetDefault();
      m_pGLView->GetObjectTransformation()->m_vScale = vScale;
      if (pDoc->m_bLighting)
      {
         pDoc->CalculateLightPosition();
      }

      PostMessage(WM_TIMER, EVENT_INVALIDATE_GL, 0);

      SetFilmIndexToStatusPane();

      ((CMainFrame*)AfxGetMainWnd())->InvalidateFrameContent();
      pDoc->SetModifiedFlag();
   }
}

void CCARA3DVView::OnViewFull() 
{
   BEGIN("CCARA3DVView::OnViewFull");
   if (m_hMenu == NULL)
   {
      CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
      if (pFrame != NULL)
      {
         WaitOnRenderer();                                     // habe fertig ?
         m_pGLView->SendMessage(WM_ENTERSIZEMOVE, 0, 0);         // SuspendRendering

         pFrame->EnableTBDocking(0);                           // Toolbars undocken
         pFrame->EnableDocking(0);

         CRect rect(0,0,0,0);                                  // maximales Rechteck ermitteln
         MONITORINFOEX sMI;
         if (pFrame->GetMonitorInfo(sMI, pFrame->m_hWnd))
         {
            rect = sMI.rcMonitor;
            rect.InflateRect(2, 2);
         }

         m_hMenu = pFrame->GetMenu()->m_hMenu;                 // Menuhandle speichern
         pFrame->SetMenu(NULL);                                // Menu entfernen

         pFrame->GetWindowPlacement(&m_WndPlacement);          // Windowgröße merken
                                                               // Windowstyles (Rand, Titelzeile) entfernen
         pFrame->ModifyStyle(WS_BORDER|WS_DLGFRAME|WS_SYSMENU|WS_THICKFRAME|WS_OVERLAPPED|WS_MINIMIZEBOX|WS_MAXIMIZEBOX, WS_POPUP, 0);
         pFrame->MoveWindow(rect, true);                       // Frame vergrößern
         pFrame->GetClientRect(&rect);

         MoveWindow(rect, true);                               // View vergrößern
         m_pGLView->SendMessage(WM_EXITSIZEMOVE, 0, 0);          // Resume Rendering
      }
   }
   else OnKeyUp(27, 1, 0);
}

void CCARA3DVView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::OnKeyUp");
#endif
   if (nChar == VK_ESCAPE)
   {
      CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
      if (pFrame && m_hMenu)
      {
         WaitOnRenderer();
         m_pGLView->SendMessage(WM_ENTERSIZEMOVE,0,0);           // SuspendRendering

         pFrame->EnableTBDocking(CBRS_ALIGN_ANY);              // Toolbars Docken
         pFrame->EnableDocking(CBRS_ALIGN_ANY);
                                                               // Windowstyles setzen
         pFrame->ModifyStyle(WS_POPUP, WS_BORDER|WS_DLGFRAME|WS_SYSMENU|WS_THICKFRAME|WS_OVERLAPPED|WS_MINIMIZEBOX|WS_MAXIMIZEBOX, 0);

         pFrame->SetMenu(CMenu::FromHandle(m_hMenu));          // Menu setzen
         m_hMenu = NULL;                                       // d.h.: kein Vollbild

         pFrame->SetWindowPlacement(&m_WndPlacement);          // Framegröße zurücksetzen
         m_pGLView->SendMessage(WM_EXITSIZEMOVE,0,0);            // Resume Rendering
      }
   }
   CCaraView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCARA3DVView::WaitOnRenderer()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::WaitOnRenderer");
#endif
   while (m_pGLView->IsRendering())
      Sleep(0);
}

void CCARA3DVView::CalculateViewVolume()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::CalculateViewVolume");
#endif
   const  ETS3DGL_Volume *pOV = m_pGLView->GetObjectVolume();
   if (pOV)
   {
      double dWidthY = pOV->Right - pOV->Left;
      double dWidthZ = pOV->Far   - pOV->Near;
      double dWidth  = (dWidthY > dWidthZ) ? dWidthY : dWidthZ;

      ETS3DGL_Volume vView;
      vView.Right  = (float)(dWidth * VIEW_FACTOR_X);
      vView.Left   = -vView.Right;
      vView.Top    = (float)(dWidth * VIEW_FACTOR_Y);
      vView.Bottom = -vView.Top;
      vView.Near   = 0.5f;
      vView.Far    = (float)(vView.Near + dWidth * VIEW_FACTOR_Z);
      m_pGLView->SetVolume(vView);
   }
}

void CCARA3DVView::AdaptLightDistance()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::AdaptLightDistance");
#endif
   int          i;
   float        fOptDist;
   double       dObjDim = m_pGLView->GetObjectDimension();
   CCARA3DVDoc *pDoc = GetDocument();
   Cara3DLight *pL;

   for (i=0; i<NO_OF_LIGHTS; i++)
   {
      pL = &pDoc->m_Light[i];
      if (pL->fDistance > 0)
      {
         fOptDist = (float)CLightDlg::GetOptDistance(dObjDim, pL->glLight.m_fSpotCutOff);
         if ((fOptDist > pL->fDistance) || (fOptDist < 0.25*pL->fDistance))
            pL->fDistance = fOptDist;
      }
   }
}

void CCARA3DVView::UpdateMousePos()
{
   CPoint ptCursor;
   ::GetCursorPos(&ptCursor);
   ScreenToClient(&ptCursor);
   OnMouseMove(0, ptCursor);
}

void CCARA3DVView::InvalidateFrameContent()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVView::InvalidateFrameContent");
#endif
   CWinApp *pApp = AfxGetApp();
   if (pApp->m_pMainWnd)
   {
      ((CMainFrame*)pApp->m_pMainWnd)->InvalidateFrameContent();
   }
}

