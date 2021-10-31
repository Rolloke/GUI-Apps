// WaveGenView.cpp : Implementierung der Klasse CWaveGenView
//

#include "stdafx.h"
#include "WaveGen.h"

#include "WaveGenDoc.h"
#include "WaveGenView.h"

#include "Curve.h"
#include "CurveLabel.h"
#include "PlotLabel.h"
#include "CaraViewProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveGenView
#define IDS_PROPERTIES                  50007

IMPLEMENT_DYNCREATE(CWaveGenView, CCaraView)

BEGIN_MESSAGE_MAP(CWaveGenView, CCaraView)
	//{{AFX_MSG_MAP(CWaveGenView)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLOTS, OnUpdateViewPlots)
	ON_COMMAND(ID_VIEW_PLOTS, OnViewPlots)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCaraView::OnFilePrintPreview)
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveGenView Konstruktion/Destruktion

CWaveGenView::CWaveGenView()
{
   m_hCommon  = NULL;
   m_hWave    = NULL;
   m_hFourier = NULL;
}

CWaveGenView::~CWaveGenView()
{
   DetachAttachedPlotArrays();
}

BOOL CWaveGenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CCaraView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWaveGenView Zeichnen

void CWaveGenView::OnDraw(CDC* pDC)
{
	CWaveGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   if (pDoc->m_bShowPlot)
   {
      pDoc->GetLabelContainer()->Draw(pDC);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CWaveGenView Drucken
void CWaveGenView::OnFilePrint()
{
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
		OnPrint(&dcPrint, &printInfo);
		OnEndPrinting(&dcPrint, &printInfo);
   }
}

void CWaveGenView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   SMarginDlgParam *pMdp = (SMarginDlgParam*)pInfo->m_lpUserData;
   ASSERT(pMdp!=NULL);
   CWaveGenDoc *pDoc = GetDocument();
   
   CMetaFileDC  MetaDC;
   CDC *pDC1 = GetDC();

   if (MetaDC.CreateEnhanced(pDC1, NULL, NULL, "WAVE"))
   {
      MetaDC.SetMapMode(MM_TEXT);
      MetaDC.SetTextColor(0);
      MetaDC.SelectObject(m_Tree.GetFont());
      m_Tree.SendMessage(WM_PRINTCLIENT, (WPARAM)MetaDC.m_hDC, PRF_OWNED);
      HENHMETAFILE hmeta = MetaDC.CloseEnhanced();
      if (hmeta)
      {
         CPictureLabel *pPicture = new CPictureLabel;
         pPicture->SetDrawPickPoints(true);
         pPicture->SetDrawFrame(true);
         pPicture->SetPreviewMode(true);
         pPicture->AttachMetaFile(hmeta, true);
         pDoc->GetLabelContainer()->InsertLabel(pPicture);
      }
   }
   ReleaseDC(pDC1);

   CCaraView::OnBeginPrinting(pDC, pInfo);

   SetPlotSizes((CRect*)&pMdp->rcDrawLoMetric);           // neue Abmessungen setzen
}

void CWaveGenView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   CCaraView::OnEndPrinting(pDC, pInfo);
   SetPlotSizes();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveGenView Diagnose

#ifdef _DEBUG
void CWaveGenView::AssertValid() const
{
	CCaraView::AssertValid();
}

void CWaveGenView::Dump(CDumpContext& dc) const
{
	CCaraView::Dump(dc);
}

CWaveGenDoc* CWaveGenView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWaveGenDoc)));
	return (CWaveGenDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWaveGenView Nachrichten-Handler

void CWaveGenView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CWaveGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   CDC*pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point, 1);
   CLabel *plabel = pDoc->GetLabelContainer()->HitLabel(&point, pDC);
   if (plabel)
   {
      CCaraViewProperties dlg(IDS_PROPERTIES, theApp.m_pMainWnd);
      dlg.InitDialog(0, plabel);
      dlg.SetApplyNowButton(false);
      dlg.m_pView = this;
      if (dlg.DoModal()==IDOK)
      {
         InvalidateRect(NULL);
      }
   }
}

void CWaveGenView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   static const char *szFilterType[] =
   {
      "Butterworth",
      "Butterworth (entdämpft) Q variabel",
      "Bessel",
      "Tschebyscheff",
      "Linkwitz-Riley"
   };
   if (lHint == 0) return; // CView::OnInitialUpdate()
	CWaveGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   switch (LOWORD(lHint))
   {
      case UPDATE_DELETE_CONTENTS:
         DetachAttachedPlotArrays();
         DeleteSubItems(m_hHistory);
         DeleteSubItems(m_hCommon);
         DeleteSubItems(m_hWave);
         DeleteSubItems(m_hFourier);
         break;
      case UPDATE_INSERT_HISTORY:
      if (pHint)
      {
         CString *pstr = (CString*)pHint;
         HTREEITEM hHistory = m_Tree.InsertItem(*pstr, m_hHistory);
         pstr->Empty();
         if (lHint & UPDATE_HISTORY_FILTER)
         {
            int nImage, nSelected;
            HTREEITEM hItem = m_Tree.GetChildItem(m_hCommon);
            while (hItem)
            {
               m_Tree.GetItemImage(hItem, nImage, nSelected);
               if ((nImage == 1) && (nSelected == 1))
               {
                  TV_INSERTSTRUCT tvis;
                  char text[MAX_PATH];
                  tvis.hParent         = hHistory;
                  tvis.hInsertAfter    = TVI_LAST;
                  tvis.item.hItem      = hItem;
                  tvis.item.mask       = TVIF_HANDLE|TVIF_TEXT;
                  tvis.item.pszText    = text;
                  tvis.item.cchTextMax = MAX_PATH;
                  m_Tree.GetItem(&tvis.item);
                  CopyChildItems(m_Tree.InsertItem(&tvis), hItem);
               }
               hItem = m_Tree.GetNextItem(hItem, TVGN_NEXT);
            }
            m_Tree.Expand(m_hHistory, TVE_EXPAND);
         }
      } break;
      case UPDATE_INSERT_FILTERFUNCTION:
      if (pDoc->m_bShowPlot)
      {
         DetachAttachedPlotArrays();
         pDoc->GetLabelContainer()->DeleteAll();
         SetFilterPlot();
         SetPlotSizes();
      }  // Filterfunktion ende
      case UPDATE_INSERT_CONTENTS:
      case UPDATE_CHANGE_CONTENTS:
      if (!pDoc->m_bShowPlot)
      {
         if (m_hCommon == NULL) return;
         CString string;
         float *pfFourierL = (float*)pDoc->m_mmfL.MapViewOfFile(0,0,0);
         float *pfFourierR = (float*)pDoc->m_mmfR.MapViewOfFile(0,0,0);

         DeleteSubItems(m_hCommon);
         string.Format("Ordnung : %d", pDoc->m_nOrder);
         m_Tree.InsertItem(string, m_hCommon);
         HTREEITEM hFilter = NULL;

         if ((pDoc->m_FilterL.nTPOrder == 0) && (pDoc->m_FilterL.nHPOrder == 0))
         {
            hFilter = m_Tree.InsertItem("Tiefpass Ordnung 0.5", 1, 1, m_hCommon);
            if (lHint == UPDATE_INSERT_FILTERFUNCTION)
               ExpandAllSubItems(hFilter, TVE_EXPAND);
         }
         else 
         {
            if (pDoc->m_FilterL.nHPOrder)
            {
               hFilter = m_Tree.InsertItem("Hochpass", 1, 1, m_hCommon);
               m_Tree.InsertItem(szFilterType[pDoc->m_FilterL.nHPType], hFilter);
               string.Format("Ordnung : %d", pDoc->m_FilterL.nHPOrder);
               m_Tree.InsertItem(string, hFilter);
               string.Format("Q-Faktor : %f", pDoc->m_FilterL.dHPQFaktor);
               m_Tree.InsertItem(string, hFilter);
               string.Format("Eckfrequenz: %f", pDoc->m_FilterL.dHPFreqSelected);
               m_Tree.InsertItem(string, hFilter);
               if (lHint == UPDATE_INSERT_FILTERFUNCTION)
                  ExpandAllSubItems(hFilter, TVE_EXPAND);
            }
            if (pDoc->m_FilterL.nTPOrder)
            {
               hFilter = m_Tree.InsertItem("Tiefpass", 1, 1, m_hCommon);
               m_Tree.InsertItem(szFilterType[pDoc->m_FilterL.nTPType], hFilter);
               string.Format("Ordnung : %d", pDoc->m_FilterL.nTPOrder);
               m_Tree.InsertItem(string, hFilter);
               string.Format("Q-Faktor : %f", pDoc->m_FilterL.dTPQFaktor);
               m_Tree.InsertItem(string, hFilter);
               string.Format("Eckfrequenz: %f", pDoc->m_FilterL.dTPFreqSelected);
               m_Tree.InsertItem(string, hFilter);
               if (lHint == UPDATE_INSERT_FILTERFUNCTION)
                  ExpandAllSubItems(hFilter, TVE_EXPAND);
            }
         }
         if (pDoc->m_lWaveFileSize)
         {
            HTREEITEM hWaveAttr = m_Tree.InsertItem(pDoc->GetTitle(), m_hCommon);
            string.Format("Größe der Datei: %d", pDoc->m_lWaveFileSize);
            m_Tree.InsertItem(string, hWaveAttr);
            string.Format("Kanäle : %d", pDoc->m_wfEX.nChannels);
            m_Tree.InsertItem(string, hWaveAttr);
            string.Format("Sample Frequenz : %d", pDoc->m_wfEX.nSamplesPerSec);
            m_Tree.InsertItem(string, hWaveAttr);
            string.Format("Bytes pro Sek : %d", pDoc->m_wfEX.nAvgBytesPerSec);
            m_Tree.InsertItem(string, hWaveAttr);
            string.Format("Blockgröße : %d", pDoc->m_wfEX.nBlockAlign);
            m_Tree.InsertItem(string, hWaveAttr);
            string.Format("Sample Bits: %d", pDoc->m_wfEX.wBitsPerSample);
            m_Tree.InsertItem(string, hWaveAttr);
            string.Format("Länge (Samples): %d", pDoc->m_wfEX.nSampleLength);
            m_Tree.InsertItem(string, hWaveAttr);
            string.Format("Länge, Zeit (s): %.3f", (float)pDoc->m_wfEX.nSampleLength/(float)pDoc->m_wfEX.nSamplesPerSec);
            m_Tree.InsertItem(string, hWaveAttr);
         }         
         if (lHint == UPDATE_INSERT_FILTERFUNCTION) break;
         else m_Tree.Expand(m_hCommon, TVE_EXPAND);

         if (pDoc->m_nValues != 0)
         {
            double dCrestFactor, dRMSMagnitude;
            CString strStar = _T(" *");
            if (pDoc->m_bTimeFunction)
            {
               DeleteSubItems(m_hWave);
               string.Format("Anzahl der Werte : %d", pDoc->m_nValues);
               m_Tree.InsertItem(string, m_hWave);
               if (pfFourierL)                            // linker Kanal
               {
                  HTREEITEM hWaveL    = m_Tree.InsertItem("linker Kanal", m_hWave);
                  dCrestFactor  = pDoc->m_dMaxValueL / pDoc->m_dRMS_ValueL;
                  dRMSMagnitude = pDoc->m_dMaxMagnitudeL / dCrestFactor;
                  string.Format("DC-Offset : %f", pDoc->m_dDCOffsetL);
                  m_Tree.InsertItem(string, hWaveL);
                  string.Format("Maximalwert : %e, [dB] : %f", pDoc->m_dMaxValueL, 20.0*log10(pDoc->m_dMaxValueL));
                  m_Tree.InsertItem(string, hWaveL);
                  string.Format("RMS-Wert : %e, [dB] : %f", pDoc->m_dRMS_ValueL, 20.0*log10(pDoc->m_dRMS_ValueL));
                  m_Tree.InsertItem(string, hWaveL);
                  string.Format("Crest-Faktor : %f, %f dB", dCrestFactor, 20.0*log10(dCrestFactor));
                  m_Tree.InsertItem(string, hWaveL);
                  string.Format("Wave-Pegel (Max) : %f %%, %f dB", pDoc->m_dMaxMagnitudeL*100.0, 20.0*log10(pDoc->m_dMaxMagnitudeL));
                  if (pDoc->m_dMaxMagnitudeL >= 1.0) string += strStar;
                  m_Tree.InsertItem(string, hWaveL);
                  string.Format("Wave-Pegel (RMS) : %f %%, %f dB", dRMSMagnitude*100.0, 20.0*log10(dRMSMagnitude));
                  if (pDoc->m_dMaxMagnitudeL >= 1.0) string += strStar;
                  m_Tree.InsertItem(string, hWaveL);
               }
               if (pfFourierR)                            // rechter Kanal
               {
                  HTREEITEM hWaveR    = m_Tree.InsertItem("rechter Kanal", m_hWave);
                  dCrestFactor  = pDoc->m_dMaxValueR / pDoc->m_dRMS_ValueR;
                  dRMSMagnitude = pDoc->m_dMaxMagnitudeR / dCrestFactor;
                  string.Format("DC-Offset : %f", pDoc->m_dDCOffsetR);
                  m_Tree.InsertItem(string, hWaveR);
                  string.Format("Maximalwert : %e, [dB] : %f", pDoc->m_dMaxValueR, 20.0*log10(pDoc->m_dMaxValueR));
                  m_Tree.InsertItem(string, hWaveR);
                  string.Format("RMS-Wert : %e, [dB] : %f", pDoc->m_dRMS_ValueR, 20.0*log10(pDoc->m_dRMS_ValueR));
                  m_Tree.InsertItem(string, hWaveR);
                  string.Format("Crest-Faktor : %f, %f dB", dCrestFactor, 20.0*log10(dCrestFactor));
                  m_Tree.InsertItem(string, hWaveR);
                  string.Format("Wave-Pegel (Max) : %f %%, %f dB", pDoc->m_dMaxMagnitudeR*100.0, 20.0*log10(pDoc->m_dMaxMagnitudeR));
                  if (pDoc->m_dMaxMagnitudeR >= 1.0) string += strStar;
                  m_Tree.InsertItem(string, hWaveR);
                  string.Format("Wave-Pegel (RMS) : %f %%, %f dB", dRMSMagnitude*100.0, 20.0*log10(dRMSMagnitude));
                  if (pDoc->m_dMaxMagnitudeR >= 1.0) string += strStar;
                  m_Tree.InsertItem(string, hWaveR);
               }
               ExpandAllSubItems(m_hWave, TVE_EXPAND);
               ExpandAllSubItems(m_hFourier, TVE_COLLAPSE);
            }
            else
            {
               DeleteSubItems(m_hFourier);
               if (pDoc->m_nValues != 0)
               {
                  string.Format("Anzahl der Werte : %d", pDoc->m_nValues/2 - 1);
                  m_Tree.InsertItem(string, m_hFourier);
                  string.Format("Frequenzschrittweite : %f Hz", pDoc->GetMinFrqStep());
                  m_Tree.InsertItem(string, m_hFourier);
                  if (pfFourierL)                            // linker Kanal
                  {
                     string.Format("Linker Kanal");
                     HTREEITEM hFourierL = m_Tree.InsertItem("linker Kanal", m_hFourier);
                     string.Format("DC-Offset : %f", pfFourierL[0]);
                     m_Tree.InsertItem(string, hFourierL);
                     string.Format("Maximalwert : %e, [dB] : %f", pDoc->m_dMaxValueL, 20.0*log10(pDoc->m_dMaxValueL));
                     m_Tree.InsertItem(string, hFourierL);
                     string.Format("RMS-Wert : %e, [dB] : %f", pDoc->m_dRMS_ValueL, 20.0*log10(pDoc->m_dRMS_ValueL));
                     m_Tree.InsertItem(string, hFourierL);
                  }
                  if (pfFourierR)                            // rechter Kanal
                  {
                     HTREEITEM hFourierR = m_Tree.InsertItem("rechter Kanal", m_hFourier);
                     string.Format("DC-Offset : %f", pfFourierR[0]);
                     m_Tree.InsertItem(string, hFourierR);
                     string.Format("Maximalwert : %e, [dB] : %f", pDoc->m_dMaxValueR, 20.0*log10(pDoc->m_dMaxValueR));
                     m_Tree.InsertItem(string, hFourierR);
                     string.Format("RMS-Wert : %e, [dB] : %f", pDoc->m_dRMS_ValueR, 20.0*log10(pDoc->m_dRMS_ValueR));
                     m_Tree.InsertItem(string, hFourierR);
                  }
               }
               ExpandAllSubItems(m_hFourier, TVE_EXPAND);
               ExpandAllSubItems(m_hWave, TVE_COLLAPSE);
            }
         }
      }
      if (lHint != UPDATE_INSERT_CONTENTS) break;
      case UPDATE_CHANGE_PLOTS:
      if (pDoc->m_bShowPlot)
      {
         float *pfFourierL = (float*)pDoc->m_mmfL.MapViewOfFile(0,0,0);
         float *pfFourierR = (float*)pDoc->m_mmfR.MapViewOfFile(0,0,0);
         if (pDoc->m_bTimeFunction)
         {
            DetachAttachedPlotArrays();
            if  (!pDoc->m_bFourierArrayAttachedL && !pDoc->m_bFourierArrayAttachedR)
            {
               pDoc->GetLabelContainer()->DeleteAll();
            }
            if ((pfFourierL != NULL) && !pDoc->m_bFourierArrayAttachedL)
            {
               SetTimePlot(pfFourierL, pDoc->m_nValues-1, "Zeitwerte linker Kanal");
               pDoc->m_bFourierArrayAttachedL = true;
            }
            if ((pfFourierR != NULL) && !pDoc->m_bFourierArrayAttachedR)
            {
               SetTimePlot(pfFourierR, pDoc->m_nValues-1, "Zeitwerte rechter Kanal");
               pDoc->m_bFourierArrayAttachedR = true;
            }
         }
         else
         {
            DetachAttachedPlotArrays();
            pDoc->GetLabelContainer()->DeleteAll();
            if ((pfFourierL != NULL) && !pDoc->m_bFourierArrayAttachedL)
            {
               SetFrqPlot(pfFourierL, "Frequenzwerte linker Kanal");
               pDoc->m_bFourierArrayAttachedL = true;
            }
            if ((pfFourierR != NULL) && !pDoc->m_bFourierArrayAttachedR)
            {
               SetFrqPlot(pfFourierR, "Frequenzwerte rechter Kanal");
               pDoc->m_bFourierArrayAttachedR = true;
            }
         }
         SetPlotSizes();
      } break;
   }

   m_Tree.ShowWindow(pDoc->m_bShowPlot ? SW_HIDE:SW_SHOW);
   InvalidateRect(NULL);
}

void CWaveGenView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CCaraView::OnPrepareDC(pDC, pInfo);
}

void CWaveGenView::OnSize(UINT nType, int cx, int cy) 
{
	CWaveGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CCaraView::OnSize(nType, cx, cy);
   if (pDoc->GetLabelContainer()->GetCount())
   {
      SetPlotSizes();
   }
   m_Tree.MoveWindow(0, 0, cx, cy, true);
}

void CWaveGenView::SetPlotSizes(CRect *prcPlot)
{
	CWaveGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   int nHeight, nPlots = pDoc->GetLabelContainer()->GetCount();
   if (nPlots)
   {
      CRect rect;
      CPlotLabel *pplot = NULL;
      CDC *pDC = GetDC();
      OnPrepareDC(pDC);
      CPoint size;
      if (prcPlot)
      {
         rect = *prcPlot;
         size = rect.Size();
         nHeight     = (size.y / nPlots) - nPlots;
         rect.bottom = nHeight+rect.top;
      }
      else
      {
         GetClientRect(&rect);
         size  = CLabel::PixelToLogical(pDC, rect.right, rect.bottom);
         nHeight     = (size.y / nPlots) - nPlots;
         rect.left   = 5;
         rect.right  = size.x-5;
         rect.top    = -5;
         rect.bottom = nHeight+5;
      }
      ReleaseDC(pDC);
      ProcessLabel pl = {NULL, nHeight, &rect, prcPlot};
      pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, SetPlotSizes);
   }
}

int CWaveGenView::SetPlotSizes(CLabel*pl, void*p)
{
   ASSERT_KINDOF(CPlotLabel, pl);
   ProcessLabel *procl = (ProcessLabel*)p;
   CPlotLabel   *ppl   = (CPlotLabel*) pl;
   CRect        *prc   = (CRect*)procl->pParam1;
   ppl->SetRect(*prc);
   if (procl->pParam2)
   {
      ppl->SetPreviewMode(true);
      ppl->SetDrawPickPoints(true);
   }
   else
   {
      ppl->SetPreviewMode(false);
      ppl->SetDrawPickPoints(false);
   }
   prc->top    = prc->bottom;
   prc->bottom = prc->top + procl->nPos - 2;

   return 0;
}

void CWaveGenView::OnViewPlots() 
{
	CWaveGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   pDoc->m_bShowPlot = !pDoc->m_bShowPlot;
   OnUpdate(this, UPDATE_INSERT_CONTENTS, NULL);
}

void CWaveGenView::OnUpdateViewPlots(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
   pCmdUI->SetCheck(GetDocument()->m_bShowPlot);
}

void CWaveGenView::SetTimePlot(float *pArray, int nValues, char *pszText)
{
	CWaveGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   CPlotLabel *ppl = new CPlotLabel;
   if (ppl)
   {
      pDoc->GetLabelContainer()->InsertLabel(ppl);
      CCurveLabel *pcl = new CCurveLabel;
      if (pcl)
      {
         pcl->AttachArray(pArray, nValues, CF_SINGLE_FLOAT);
         pcl->SetDescription(pszText);
         pcl->SetXUnit("ms");
         pcl->SetYUnit("A");
         pcl->SetXNumMode(ETSDIV_NM_SCI);
         pcl->SetYNumMode(ETSDIV_NM_SCI);
         LOGPEN lp = {0, {1,1},RGB(255, 0, 0)};
         pcl->SetLogPen(&lp);
         double dStep  = 1000.0 / (double)pDoc->m_wfEX.nSamplesPerSec;
         pcl->SetXStep(dStep);
         pcl->SetXOrigin(0);
         RANGE range;
         double dMaxY = max(pDoc->m_dMaxValueL, pDoc->m_dMaxValueR);
         range.y1 = -dMaxY;
         range.y2 =  dMaxY;
         range.x1 = pcl->GetXMin();
         range.x2 = pcl->GetXMax();
         if (range.x2 > 1000) range.x2 = 1000;
         dStep = (range.y2 - range.y1) * 0.1;
         range.y1 -=  dStep;
         range.y2 += dStep;
         ppl->InsertCurve(pcl);
         ppl->SetRange(&range);
         ppl->SetHeading(pcl->GetText());
      }
   }
}

void CWaveGenView::DetachAttachedPlotArrays()
{
	CWaveGenDoc* pDoc = GetDocument();

   ProcessLabel pl = {NULL, 0, pDoc, NULL};
   pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, DetachAttachedPlotArrays);
}

int CWaveGenView::DetachAttachedPlotArrays(CLabel*pl, void*p)
{
   ASSERT_KINDOF(CPlotLabel, pl);
   ProcessLabel *procl = (ProcessLabel*)p;
   CPlotLabel   *ppl   = (CPlotLabel*) pl;
   CWaveGenDoc* pDoc   = (CWaveGenDoc*)procl->pParam1;
   float *pfFourierL = (float*)pDoc->m_mmfL.MapViewOfFile(0,0,0);
   float *pfFourierR = (float*)pDoc->m_mmfR.MapViewOfFile(0,0,0);
   int  j;
   CCurveLabel *pcl;
   for (j=0, pcl=(CCurveLabel*)ppl->GetCurveList()->GetLabel(j); pcl!=NULL; pcl=(CCurveLabel*)ppl->GetCurveList()->GetLabel(++j))
   {
      if (pcl->GetArrayPointer() == pfFourierL)
      {
         pcl->DetachArray();
         pDoc->m_bFourierArrayAttachedL = false;
      }
      if (pcl->GetArrayPointer() == pfFourierR)
      {
         pcl->DetachArray();
         pDoc->m_bFourierArrayAttachedR = false;
      }
   }
   return 0;
}

int CWaveGenView::GetFourierFrqValue(CF_GET_CURVE_PARAMS *pgcp)
{
   CWaveGenView *pThis = (CWaveGenView*)pgcp->pValue;
   bool bOnlyY = true;
   switch(pgcp->nWhat)
   {
      case CF_GET_SIZE:
         return pgcp->pCurve->GetNumValues() * sizeof(float) * 2 + 1;
      case CF_GET_VALUE: 
         bOnlyY = false;
      case CF_GET_YVALUE:
      {
         int i = pgcp->nIndex * 2 + 1;
         float *pVal = (float*)pgcp->pCurve->GetArrayPointer();
         pgcp->pValue->y = 20*log10(hypot((double)pVal[i], (double)pVal[i+1]));
      }
      if (bOnlyY) return 1;
      case CF_GET_XVALUE:
      {
         pgcp->pValue->x = pgcp->pCurve->GetXOrigin() + pgcp->pCurve->GetXStep() * pgcp->nIndex;
         return 1;
      }
      case CF_GET_Y_VALUE:
         return (int) ((pgcp->pValue->x - pgcp->pCurve->GetXOrigin()) / pgcp->pCurve->GetXStep());
      case CF_GET_X_MIN_STEP:
         pgcp->pValue->x = pgcp->pCurve->GetXStep();
         return 1;
      case CF_SORT_XASC:
      case CF_SET_VALUE:
      case CF_SET_XVALUE:
      case CF_SET_YVALUE:
         return 1;
      default:break;
   }
   return 0;
}

void CWaveGenView::SetFrqPlot(float *pArray, char *pszText)
{
   CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
	CWaveGenDoc* pDoc = GetDocument();
   CPlotLabel *ppl = new CPlotLabel;

   if (ppl)
   {
      pDoc->GetLabelContainer()->InsertLabel(ppl);

      CCurveLabel *pcl = new CCurveLabel;
      if (pcl)
      {
         int n = pDoc->GetNoOfFrqSteps();
         pcl->AttachArray(pArray, n, CF_ARBITRARY);
         pcl->SetCrvParamFkt(GetFourierFrqValue, (void*)this);
         pcl->SetYNumMode(2);
         pcl->SetDescription(pszText);
         pcl->SetXUnit("Hz");
         pcl->SetYUnit("dB");
         pcl->SetYNumMode(ETSDIV_NM_ENG);
         pcl->SetSorted(true);
         LOGPEN lp = {0, {1,1},RGB(255, 0, 0)};
         pcl->SetLogPen(&lp);

         double dStep = (double)pDoc->GetMinFrqStep();
         pcl->SetXStep(dStep);
         pcl->SetXOrigin(dStep);

         RANGE range;
         double dMaxY = 20*log(max(pDoc->m_dMaxValueL, pDoc->m_dMaxValueR));
         range.y1 = -dMaxY;
         range.y2 =  dMaxY;
//         range.y1 = pcl->GetYMin();
//         range.y2 = pcl->GetYMax();
         range.x1 = pcl->GetXMin();
         range.x2 = pcl->GetXMax();
         range.x1 = dStep;
         dStep = (range.y2 - range.y1) * 0.1;
         range.y1 -= dStep;
         range.y2 += dStep;
         ppl->InsertCurve(pcl);
         ppl->SetXDivision(ETSDIV_FREQUENCY);
         ppl->SetRange(&range);
         ppl->SetHeading(pcl->GetText());
      }
   }
}

/*
void CWaveGenView::SetFrqPlot(float *pArray, char *pszText)
{
   CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
	CWaveGenDoc* pDoc = GetDocument();
   CPlotLabel *ppl = new CPlotLabel;
   CPlotLabel *pplPhase = new CPlotLabel;

   if (ppl)
   {
      m_Plots.InsertLabel(ppl);

      m_Plots.InsertLabel(pplPhase);

      CCurveLabel *pcl = new CCurveLabel;
      CCurveLabel *pclPhase;
      if (pcl)
      {
         int i, j, n = pDoc->GetNoOfFrqSteps();
         pcl->SetFormat(CF_SINGLE_FLOAT);
         pcl->SetSize(n);
         pcl->SetYNumMode(2);
         pcl->SetDescription(pszText);
         pcl->SetXUnit("Hz");
         pcl->SetYUnit("dB");
         pcl->SetYNumMode(ETSDIV_NM_ENG);
         LOGPEN lp = {0, {1,1},RGB(255, 0, 0)};
         pcl->SetLogPen(&lp);
         double dStep = (double)pDoc->GetMinFrqStep();
         double dVal;
         pcl->SetXStep(dStep);
         pcl->SetXOrigin(dStep);
         pclPhase = new CCurveLabel(pcl);
         pclPhase->SetYUnit("Deg");
         pclPhase->SetSize(n);

         pApp->CreateProgressDlg(pszText);
         pApp->ProgressDlgSetRange(n);
         for (i=1, j=0; j<n; i+=2, j++)
         {
            Complex cVal((double)pArray[i], (double)pArray[i+1]);
            dVal = Betrag(cVal);

            dVal = pArray[i]*pArray[i] + pArray[i+1]*pArray[i+1];
            if (dVal > 0) dVal = sqrt(dVal);
            else if (dVal <= 0) dVal = 1e-20;
            pcl->SetY_Value(j, 20*log10(dVal));
            pclPhase->SetY_Value(j, Phase(cVal));
            pApp->ProgressDlgStep();
         }
         pApp->DestroyProgressDlg();

         RANGE range;
         range.y1 = pcl->GetYMin();
         range.y2 = pcl->GetYMax();
         range.x1 = pcl->GetXMin();
         range.x2 = pcl->GetXMax();
         range.x1 = dStep;
         dStep = (range.y2 - range.y1) * 0.1;
         range.y1 -= dStep;
         range.y2 += dStep;
         ppl->InsertCurve(pcl);
         ppl->SetXDivision(ETSDIV_FREQUENCY);
         ppl->SetRange(&range);
         ppl->SetHeading(pcl->GetText());


         range.y1 = pclPhase->GetYMin();
         range.y2 = pclPhase->GetYMax();
         range.x1 = pclPhase->GetXMin();
         range.x2 = pclPhase->GetXMax();
         range.x1 = (double)pDoc->GetMinFrqStep();;
         dStep = (range.y2 - range.y1) * 0.1;
         range.y1 -= dStep;
         range.y2 += dStep;
         pplPhase->InsertCurve(pclPhase);
         pplPhase->SetXDivision(ETSDIV_FREQUENCY);
         pplPhase->SetRange(&range);
         pplPhase->SetHeading(pcl->GetText());

      }
   }
}
*/
void CWaveGenView::SetFilterPlot()
{
	CWaveGenDoc* pDoc = GetDocument();
   pDoc->CalcFilterArray();
   CPlotLabel *ppl = new CPlotLabel;
   if (ppl)
   {
      pDoc->GetLabelContainer()->InsertLabel(ppl);
      ppl->SetXGridStep(100);
      ppl->SetYGridStep(50);
      CCurveLabel *pcl = new CCurveLabel;
      if (pcl)
      {
         int i;
         pcl->SetSize(NO_OF_FREQUENCIES);
         pcl->SetYNumMode(2);
         pcl->SetDescription("Magnitude");
         pcl->SetXUnit("Hz");
         pcl->SetYUnit("dB");
         pcl->SetXDivision(ETSDIV_FREQUENCY);
         pcl->SetYNumMode(ETSDIV_NM_ENG);
         LOGPEN lp = {0, {1,1},RGB(255, 0, 0)};
         pcl->SetLogPen(&lp);
         
         for (i=0; i<NO_OF_FREQUENCIES; i++)
         {
            pcl->SetX_Value(i) = 5.0 * pow(2.0, (double)i/9.0);
            pcl->SetY_Value(i) = 20*log10(Betrag(pDoc->m_FilterL.pcFilterBox[i]));
         }

         RANGE range;
         range.y1 = pcl->GetYMin();
         range.y2 = pcl->GetYMax();
         range.x1 = pcl->GetXMin();
         range.x2 = pcl->GetXMax();
         double dStep = (range.y2 - range.y1) * 0.1;
         range.y1 -=  dStep;
         range.y2 += dStep;
         ppl->InsertCurve(pcl);
         ppl->SetRange(&range);
         ppl->SetHeading(pcl->GetText());
      }
   }
   ppl = new CPlotLabel;
   if (ppl)
   {
      pDoc->GetLabelContainer()->InsertLabel(ppl);
      CCurveLabel *pcl = new CCurveLabel;
      if (pcl)
      {
         int i;
         pcl->SetSize(NO_OF_FREQUENCIES);
         pcl->SetYNumMode(2);
         pcl->SetDescription("Phase");
         pcl->SetXUnit("Hz");
         pcl->SetYUnit("Deg");
         pcl->SetXDivision(ETSDIV_FREQUENCY);
         pcl->SetYNumMode(ETSDIV_NM_STD);
         LOGPEN lp = {0, {1,1},RGB(255, 0, 0)};
         pcl->SetLogPen(&lp);
         
         for (i=0; i<NO_OF_FREQUENCIES; i++)
         {
            pcl->SetX_Value(i) = 5.0 * pow(2.0, (double)i/9.0);
            pcl->SetY_Value(i) = Phase(pDoc->m_FilterL.pcFilterBox[i]);
         }

         RANGE range;
         range.y1 = pcl->GetYMin();
         range.y2 = pcl->GetYMax();
         range.x1 = pcl->GetXMin();
         range.x2 = pcl->GetXMax();
         double dStep = (range.y2 - range.y1) * 0.1;
         range.y1 -=  dStep;
         range.y2 += dStep;
         ppl->InsertCurve(pcl);
         ppl->SetRange(&range);
         ppl->SetHeading(pcl->GetText());
      }
   }
}

int CWaveGenView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCaraView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rcClient;
   GetClientRect(&rcClient);
   m_Tree.Create(WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_SHOWSELALWAYS|TVS_HASBUTTONS, rcClient, this, 1);

   HTREEITEM hParent  = m_Tree.InsertItem("Parameter");
   m_hCommon  = m_Tree.InsertItem("allgemein", hParent);
   m_hWave    = m_Tree.InsertItem("Wave"     , hParent);
   m_hFourier = m_Tree.InsertItem("Fourier"  , hParent);
   m_hHistory = m_Tree.InsertItem("Verlauf"  , hParent);

   m_Tree.Expand(hParent, TVE_EXPAND);
	return 0;
}

void CWaveGenView::OnDestroy() 
{
	CCaraView::OnDestroy();
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	
}

void CWaveGenView::DeleteSubItems(HTREEITEM hParent)
{
   if (m_Tree.ItemHasChildren(hParent))
   {
      HTREEITEM hNextItem;
      HTREEITEM hChildItem = m_Tree.GetChildItem(hParent);
      while (hChildItem != NULL)
      {
         hNextItem = m_Tree.GetNextItem(hChildItem, TVGN_NEXT);
         m_Tree.DeleteItem(hChildItem);
         hChildItem = hNextItem;
      }
   }
}

void CWaveGenView::ExpandAllSubItems(HTREEITEM hParent, UINT nCode)
{
   if (m_Tree.ItemHasChildren(hParent))
   {
      m_Tree.Expand(hParent, nCode);
      HTREEITEM hChildItem = m_Tree.GetChildItem(hParent);
      while (hChildItem != NULL)
      {
         ExpandAllSubItems(hChildItem, nCode);
         hChildItem = m_Tree.GetNextItem(hChildItem, TVGN_NEXT);
      }
   }
}

void CWaveGenView::CopyChildItems(HTREEITEM hParent, HTREEITEM hInsertFrom)
{
   if (m_Tree.ItemHasChildren(hInsertFrom))
   {
      TV_INSERTSTRUCT tvis;
      HTREEITEM hChildItem = m_Tree.GetChildItem(hInsertFrom);
      while (hChildItem != NULL)
      {
         char text[MAX_PATH];
         tvis.hParent         = hParent;
         tvis.hInsertAfter    = TVI_LAST;
         tvis.item.hItem      = hChildItem;
         tvis.item.mask       = TVIF_HANDLE|TVIF_TEXT;
         tvis.item.pszText    = text;
         tvis.item.cchTextMax = MAX_PATH;
         m_Tree.GetItem(&tvis.item);
         CopyChildItems(m_Tree.InsertItem(&tvis), hChildItem);
         hChildItem = m_Tree.GetNextItem(hChildItem, TVGN_NEXT);
      }
   }
}

