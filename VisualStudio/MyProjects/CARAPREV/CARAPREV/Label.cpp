// Label.cpp: Implementierung der Klasse CLabel.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Label.h"
#include "CARADoc.h"
#include "PreviewFileHeader.h"
#include "Dibsection.h"
#include "resource.h"
#include "Transformation.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CLabel, CObject,1);


// Scope-Deklaration für static-Variablen
CPen      *CLabel::gm_pShapePen   = NULL;
LOGPEN    *CLabel::gm_pPenArray   = NULL;
LOGBRUSH  *CLabel::gm_pBrushArray = NULL;
LOGFONT   *CLabel::gm_pFontArray  = NULL;
CSize      CLabel::gm_PickPointSize = CSize(4,4);
CPenList   CLabel::gm_PenList;
int        CLabel::gm_nPens         = 0;
CBrushList CLabel::gm_BrushList;
int        CLabel::gm_nBrushs       = 0;
CFontList  CLabel::gm_FontList;
CPtrList   CLabel::gm_GlobalGdiObjects;
int        CLabel::gm_nFonts        = 0;
bool       CLabel::gm_bSaveGlobal   = false;
COLORREF   CLabel::gm_ShapePenColor = RGB(255,0,0);
int        CLabel::gm_nFileVersion  = 100;
bool       CLabel::gm_bColorMatching = true;
bool       CLabel::gm_bFillPath      = true;


#ifndef _DEBUG
 UINT       CLabel::gm_nInstances = 0;
 HANDLE     CLabel::gm_hHeap      = 0;
#endif


CLabel::~CLabel()
{
#ifdef CHECK_LABEL_DESTRUCTION
   REPORT("CLabel::Destructor");
#endif
/*
   if (IsKindOf(RUNTIME_CLASS(CBezierLabel   ))) REPORT("CBezierLabel");
   if (IsKindOf(RUNTIME_CLASS(CCircleLabel   ))) REPORT("CCircleLabel");
   if (IsKindOf(RUNTIME_CLASS(CCurveLabel    ))) REPORT("CCurveLabel");
   if (IsKindOf(RUNTIME_CLASS(CLabelContainer))) REPORT("CLabelContainer");
   if (IsKindOf(RUNTIME_CLASS(CLineLabel     ))) REPORT("CLineLabel");
   if (IsKindOf(RUNTIME_CLASS(CPictureLabel  ))) REPORT("CPictureLabel");
   if (IsKindOf(RUNTIME_CLASS(CPlotLabel     ))) REPORT("CPlotLabel");
   if (IsKindOf(RUNTIME_CLASS(CRectLabel     ))) REPORT("CRectLabel");
   if (IsKindOf(RUNTIME_CLASS(CTextLabel     ))) REPORT("CTextLabel");
*/
   if (m_pP) delete[] m_pP;
}

#ifdef _DEBUG
void CLabel::AssertValid() const
{
   CObject::AssertValid();
   ASSERT((m_pP!=NULL) && (m_nCount!=0));
   int nCount = m_nCount + m_State.scaleable? 1:0;
   AfxIsValidAddress(m_pP, sizeof(CPoint)*nCount, true);
}
#else
void* __cdecl CLabel::operator new(size_t nSize)
{
   void * pointer = NULL;
   try
   {
      DWORD dwSize     = CCaraWinApp::GetTotalPhysicalMem();
      if (nSize >= dwSize)
      {
         CString strMsg;
         strMsg.Format(IDS_FORMAT_LARGE_MEM_REQUEST, nSize >> 20, dwSize >> 20);
         if (AfxMessageBox(strMsg, MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
         {
            throw (int) IDE_OUT_OF_MEMORY;
         }
      }
      if(gm_nInstances==0)
      {
         if(gm_hHeap!=0) throw (int) IDE_HEAP_EXISTS;
         gm_hHeap = HeapCreate(0, 1048576,0); // growable Heap
         if(gm_hHeap==0) throw (int) IDE_HEAP_CREATION;
      }

      if((pointer = HeapAlloc(gm_hHeap,0, nSize))==NULL)
         throw (int) IDE_OUT_OF_MEMORY;

      gm_nInstances++;
   }
   catch(int nError)
   {
      AfxMessageBox(nError, MB_OK|MB_ICONEXCLAMATION);
   }
   return pointer;
}

void  __cdecl CLabel::operator delete(void* p)
{
   try
   {
      if(gm_hHeap==0) throw (int) IDE_DESTRUCTION_WITHOUT_HEAP;
      else if(HeapFree(gm_hHeap, 0, p)==0) throw (int) IDE_HEAP_FREE;

      gm_nInstances--;

      if(gm_nInstances==0)
      {
         if(HeapDestroy(gm_hHeap)==0) throw (int) IDE_HEAP_DESTROY;
         gm_hHeap = 0;
      }
   }
   catch (int nError)
   {
      AfxMessageBox(nError, MB_OK|MB_ICONEXCLAMATION);
   }
}

bool CLabel::HeapWatch()
{
   if ((gm_hHeap == 0) && (gm_nInstances > 0))
   {
      REPORT("% d Memory Leaks detected", gm_nInstances);
//      AfxMessageBox(IDE_MEMORY_LEAKS_CLABEL);
   }
   if (gm_hHeap) 
   {
      CString format;
      format.Format(IDE_MEMORY_OBJECTS_CLABEL, gm_nInstances);
      AfxMessageBox(format, MB_OK|MB_ICONINFORMATION);
      return true;
   }
   return false;
}
void CLabel::DestroyHeap()
{
   if(gm_hHeap && (HeapDestroy(gm_hHeap)==0))
      AfxMessageBox(IDE_HEAP_DESTROY, MB_OK|MB_ICONEXCLAMATION);
}
#endif

void CLabel::SetShapePen(COLORREF color)
{
   if (color != 0xFFFFFFFF) gm_ShapePenColor = color;
   LOGPEN lp = {PS_SOLID, {0, 0}, gm_ShapePenColor};
   CPen * pOld = gm_pShapePen;
   gm_pShapePen = FindPen(&lp);
   ChangeGlobalObject(pOld, gm_pShapePen);
}

void CLabel::Serialize(CArchive & ar)
{
   CObject::Serialize(ar);

   if (ar.IsStoring())
   {
      ArchiveWrite(ar, &m_State, sizeof(BSState));
      ArchiveWrite(ar, &m_nCount, sizeof(int));
      ArchiveWrite(ar, m_pP, sizeof(CPoint)*(m_nCount+m_State.scaleable));
   }
   else
   {
      BSState oldState = m_State;
      ArchiveRead(ar, &m_State, sizeof(BSState));
      int nCount;
      ArchiveRead(ar, &nCount, sizeof(int));
      if (m_pP)
      {
         if ((oldState.scaleable != m_State.scaleable) ||
             (nCount != m_nCount))
         {
            m_nCount = nCount;
            delete[] m_pP;
            m_pP = NULL;
         }
      }
      if (m_pP == NULL) m_pP = new CPoint[m_nCount+m_State.scaleable];
      if (m_pP) ArchiveRead(ar, m_pP, sizeof(CPoint)*(m_nCount+m_State.scaleable));
      ResetPickStates();
      SetShapeDrawn(false);
   }
}

void CLabel::GetChecksum(CFileHeader *pFH)
{
   pFH->CalcChecksum(&m_nCount, sizeof(int));
   pFH->CalcChecksum(m_pP, sizeof(CPoint)*m_nCount);
}

// Zeichenfunktionen der Basisklasse müssen vorher aufgerufen werden
/******************************************************************************
* Name       : Draw                                                           *
* Definition : virtual void Draw(CDC *)                                       *
* Zweck      : Virtuelle Zeichenfunktion der Basisklasse, setzt den Zustand   *
*              changed und drawn auf false und zeichnet den Umriß eines       *
*              gepickten Labels neu. Sie muß in der Zeichenfunktion der abge- *
*              leiteten Klasse aufgerufen werden.                             *
* Aufruf     : Draw(pDC);                                                     *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf CDC-Objekt                                  (CDC*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
bool CLabel::Draw(CDC* pDC, int bAtr)
{
//   CRect rc = GetRect(pDC);
//   if (!pDC->RectVisible(&rc)) return false;
   if (!IsVisible()) return false;
   if (IsPicked())
   {
      SetShapeDrawn(false);
      DrawShape(pDC, true);
      return false;
   }

   if (m_State.scaleable && IsChanged() && (m_State.scaling!=SCALE_ARBITRARY))
      CheckScale(m_pP[0], m_pP[1]);

   SetChanged(false);                                          // Label ist neu gezeichnet
   SetShapeDrawn(false);
//   pDC->SetROP2(R2_BLACK);                                   // Normal-Zeichenmodus
   if (bAtr)
   {
      if (m_State.drawbackgnd)
      {
         pDC->SetBkMode(OPAQUE);
         if (CLabel::gm_bColorMatching) pDC->SetBkColor(m_BkColor);
         else                           pDC->CDC::SetBkColor(m_BkColor);
      }
      else
      {
         pDC->SetBkMode(TRANSPARENT);
      }
      pDC->SetROP2(R2_COPYPEN);                                   // XOR-Zeichenmodus ausschalten
   }
   return true;
}

/******************************************************************************
* Name       : GetRgn                                                         *
* Definition : virtual bool GetRgn(int, CDC *, CRgn *, UINT nFlags=0);        *
* Zweck      : Definiert ein Object CRgn, daß ein Windows GDI-Handle auf eine *
*              Region enthält. Diese Region beinhaltet in der Basisklasse     *
*              CLabel eine einfache rechteckige Region, die durch die         *
*              Funktion GetRect() der Klasse definiert wird. Die abgeleitete  *
*              Klasse kann diese Funktion überschreiben und eine komplexere   *
*              Region erzeugen. Existiert noch keine Region in CRgn, so wird  *
*              eine neue Region erzeugt. Der Parameter (n_cm) hat dann keine  *
*              Bedeutung. Eine existierende Region wird so verändert, wie es  *
*              durch den Parameter n_cm definiert wird.                       *
* Aufruf     : GetRgn(n_cm, pDC, prgn,[INFLATE_RECT]);                        *
* Parameter  :                                                                *
* n_cm   (E) : Kombinationsmodus für das Zusammenfügen der Region   (int)     *
*              des Labels mit einer evtl. bestehenden im Objekt prgn.         *
*              RGN_AND  : Erzeugt eine überlappende Region aus beiden.        *
*              RGN_COPY : Kopiert die Region des Labels und löscht die        *
*                         bestehende.                                         *
*              RGN_DIFF : Entfernt die Region des Labels.                     *
*              RGN_OR   : Fügt die Region des Labels hinzu.                   *
*              RGN_XOR  : Fügt die Region des Labels hinzu, entfernt aber die *
*                         überlappende Region beider.                         *
* pDC    (E) : Zeiger auf CDC-Objekt                                (CDC*)    *
* prgn   (EA): Zeiger auf CRgn-Object                               (CRgn *)  *
* nFlags (E) : Flags für Vergrößerung der Region um Pickpunkte      (UINT)    *
* Funktionswert : (true, false)                                     (bool)    *
******************************************************************************/
bool CLabel::GetRgn(int n_cm, CDC * pDC, CRgn *prgn, UINT nFlags)
{
   if (!prgn)           return false;
   CRect rect = GetRect(pDC, nFlags|DEV_COORD);
   if (n_cm == RGN_COPY) prgn->DeleteObject();
   if (HRGN(*prgn) == NULL) return ((prgn->CreateRectRgnIndirect(&rect)) ? true : false);
   else
   {
      CRgn  rgn;
      if (rgn.CreateRectRgnIndirect(&rect))
      {
         return ((prgn->CombineRgn(prgn, &rgn, n_cm&0x0f) == ERROR) ? false : true);
      }
   }
   return false;   
}


/******************************************************************************
* Name       : SetPoint                                                       *
* Definition : virtual void SetPoint(p); oder void SetPoint(p, n);            *
* Zweck      : Setzen eines Definitionspunktes des Labels                     *
*              Ohne den Index n wird der aktuelle MatchPoint, der mit der     *
*              Funktion SetMatchPoint() gesetzt wird, verändert.              *
* Aufruf     : SetPoint(p); oder SetPoint(p, n);                              *
* Parameter  :                                                                *
* p      (E) : Koordinaten des Punktes                               (CPoint) *
* [n]    (E) : Index des Definitionspunktes                          (int)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CLabel::SetPoint(CPoint p)
{
   SetPoint(p, m_nAct);
}

/******************************************************************************
* Name       : MoveLabel                                                      *
* Definition : virtual void MoveLabel(CPoint);                                *
* Zweck      : Verschieben aller Definitionspunkte um dp in x-y-Richtung.     *
* Aufruf     : MoveLabel(dp);                                                 *
* Parameter  :                                                                *
* dp     (E) : Verschiebevektor                                      (CPoint) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CLabel::MoveLabel(CPoint dp)
{
   int i;
   if (IsFixed()) return;
   if ((dp.x != 0) || (dp.y != 0))
   {
      SetChanged(true);
      for (i=0; i<m_nCount; i++)
         m_pP[i] += dp;
   }
}
/******************************************************************************
* Name       : ScaleLabel                                                     *
* Definition : void ScaleLabel(CSize, CSize);                                 *
* Zweck      : Skalieren der Label in X- und Y-Richtung.                      *
* Aufruf     : ScaleLabel(Numerator, Denominator);                            *
* Parameter  :                                                                *
* Numerator   (E) : Multiplikator (neue Größe)                       (CSize)  *
* Denominator (E) : Divisor       (alte Größe)                       (CSize)  *
* Funktionswert : void                                                        *
******************************************************************************/
void CLabel::ScaleLabel(CSize Numerator, CSize Denominator)
{
   int i;
   if (IsFixed()) return;
   if ((!Numerator.cx)   || (!Numerator.cy))   return;         // mit 0 Multiplizieren : Fehler
   if ((!Denominator.cx) || (!Denominator.cy)) return;         // durch 0 Teilen       : Fehler
                                                               // alte und neue Größe gleich : keine Skalierung
   if ((Numerator.cx == Denominator.cx) && (Numerator.cy == Denominator.cy)) return;
   SetChanged(true);
   for (i=0; i<m_nCount; i++)
   {
      m_pP[i].x = MulDiv(m_pP[i].x, Numerator.cx, Denominator.cx);
      m_pP[i].y = MulDiv(m_pP[i].y, Numerator.cy, Denominator.cy);
   }
}

/******************************************************************************
* Name       : GetPoint                                                       *
* Definition : CPoint GetPoint() oder GetPoint(n);                            *
* Zweck      : Liefert einen Definitionspunkt des Labels                      *
*              Ohne den Index n wird der aktuelle MatchPoint, der mit der     *
*              Funktion SetMatchPoint() gesetzt wird, zurückgegeben           *
* Aufruf     : GetPoint(); oder GetPoint(n);                                  *
* Parameter  :                                                                *
* [n]    (E) : Index des Definitionspunktes                          (int)    *
* Funktionswert : Koordinaten des Definitionspunktes                 (CPoint) *
******************************************************************************/
CPoint CLabel::GetPoint()
{
   return GetPoint(m_nAct);
}
CPoint CLabel::GetPoint(int n)
{
   if ((n >= 0) && (n < m_nCount)) return m_pP[n];
   return CPoint(0,0);
}

bool CLabel::SetMatchPoint(int n)
{
   if ((n >= 0) && (n < m_nCount))
   {
      m_nAct = n;
      return true;
   }
   else return false;
}
void CLabel::ResetPickStates()
{
   if (IsKindOf(RUNTIME_CLASS(CLabelContainer)))
      ((CLabelContainer*)this)->SetPicked(false);
   else
      SetPicked(false);
   SetPointPicked(false);
   SetDoublePicked(false);
}

bool CLabel::CheckScale(CPoint &p1, CPoint &p2)
{
   if (m_State.scaleable && (m_State.scaling == SCALE_PROPORTIONAL) &&
       (m_pP[m_nCount].x != 0) && (m_pP[m_nCount].y != 0))
   {
      int    dx = p2.x - p1.x;
      int    dy = p2.y - p1.y;
      if (dx == 0) 
      {
         dx++;
         p2.x = p1.x + dx;
      }
      if (dy == 0)
      {
         dy++;
         p2.y = p1.y + dy;
      }      
      double dActScaleRatio = (double)dx               / (double)dy;
      double dScaleRatio    = (double)m_pP[m_nCount].x / (double)m_pP[m_nCount].y;
      double diff = dScaleRatio - dActScaleRatio;
      if (diff < 0) diff = -diff;
      if (diff < 1e-6) return false;

      if (dScaleRatio < dActScaleRatio)
      {
         double y  = (double)dx / dScaleRatio;
         if (m_nAct) p2.y = (long)(p1.y + y);
         else        p1.y = (long)(p2.y - y);
      }
      else 
      {
         double x  = (double)dy * dScaleRatio;
         if (m_nAct) p2.x = (long)(p1.x + x);
         else        p1.x = (long)(p2.x - x);
      }

      return true;
   }
   return false;
}

bool CLabel::SetScale(int b, bool bSetSize)
{
   if (!m_State.scaleable) return false;
   m_State.scaling = b;
   if (bSetSize)
   {
      return SetScaleSize(GetRect().Size());
   }
   return true;
}
bool CLabel::SetScaleSize(CSize szScale)
{
   if (!m_State.scaleable) return false;
   if ((szScale.cx!=0) && (szScale.cy!=0))
   {
      m_pP[m_nCount].x = szScale.cx;
      m_pP[m_nCount].y = szScale.cy;
      SetChanged(true);
      return true;
   }
   else
   {
      m_pP[m_nCount].x = 0;
      m_pP[m_nCount].y = 0;
      return false;
   }
}


CFont  *CLabel::FindFont(int index)
{
   if (gm_pFontArray)
   {
      if ((index >= 0) && (index < gm_nFonts))
         return FindFont(&gm_pFontArray[index]);
   }
   return NULL;
}

CPen   *CLabel::FindPen(int index)
{
   if (gm_pPenArray)
   {
      if ((index >= 0) && (index < gm_nPens))
         return FindPen(&gm_pPenArray[index]);
   }
   return NULL;
}

CBrush *CLabel::FindBrush(int index)
{
   if (gm_pBrushArray)
   {
      if ((index >= 0) && (index < gm_nBrushs))
         return FindBrush(&gm_pBrushArray[index]);
   }
   return NULL;
}

/******************************************************************************************
* Name      : FindPen                                                                     *
* Definition: CPen * FindPen(LOGPEN* plp);                                                *
* Zweck     : Sucht nach einem Zeichenstift mit denselben Eigenschaften wie in der Struk- *
*             tur plp in der globalen Liste CLabel::gm_PenList und liefert einen Zeiger   *
*             auf ein gültiges CPen-Objekt. Wird kein gleicher Zeichenstift gefunden, so  *
*             wird ein neuer Stift angelegt und in die Liste eingetragen.                 *
*             Bei Mißerfolg liefert diese Funktion NULL.                                  *
* Aufruf    : FindPen(plp)                                                                *
* Parameter :                                                                             * 
* plp    (E): Zeiger auf eine LOGPEN-Struktur                              (LOGPEN*)      *
* Funktonswert: Zeiger auf ein CPen-Objekt                                 (CPen*)        *
******************************************************************************************/
CPen *CLabel::FindPen(LOGPEN* plp)
{
   if (!plp) return NULL;
   if (plp->lopnWidth.x > 1) plp->lopnStyle = PS_INSIDEFRAME;
   if (!gm_PenList.IsEmpty())
   {
      CPen *ppen;
      LOGPEN lp;
      POSITION pos = gm_PenList.GetHeadPosition();
      while (pos)
      {
         ppen = gm_PenList.GetNext(pos);
         ppen->GetLogPen(&lp);
         if ((plp->lopnColor == lp.lopnColor) &&
             (plp->lopnStyle == lp.lopnStyle) &&
             (plp->lopnWidth.x == lp.lopnWidth.x))
             return ppen;
      }
   }
   CPen *ppen = new CPen;
   if (ppen)
   {
      if (ppen->CreatePenIndirect(plp))
      {
         gm_PenList.AddHead(ppen);
//         REPORT("NewPen %d", gm_PenList.GetCount());
         return ppen;
      }
      else
      {
         delete ppen;
         return NULL;
      }
   }
   return NULL;
}

/******************************************************************************************
* Name      : FindBrush                                                                   *
* Definition: CBrush * FindBrush(LOGBRUSH* plb);                                          *
* Zweck     : Sucht nach einem Zeichenpinsel mit denselben Eigenschaften wie in der Struk-*
*             tur plb in der globalen Liste CLabel::gm_BrushList und liefert einen Zeiger *
*             auf ein gültiges CBrush-Objekt. Wird kein gleicher Zeichenpinsel gefunden,  *
*             so wird ein neuer Pinsel angelegt und in die Liste eingetragen.             *
*             Bei Mißerfolg liefert diese Funktion NULL.                                  *
* Aufruf    : FindBrush(plp)                                                              *
* Parameter :                                                                             * 
* plp    (E): Zeiger auf eine LOGBRUSH-Struktur                            (LOGBRUSH*)    *
* Funktonswert: Zeiger auf ein CBrush-Objekt                               (CBrush*)      *
******************************************************************************************/
CBrush *CLabel::FindBrush(LOGBRUSH*  plb)
{
   if (!plb) return NULL;
   if (plb->lbStyle == BS_HOLLOW)
   {
      return NULL;
   }
   if (plb->lbStyle == BS_SOLID)
   {
      plb->lbHatch = 0;
   }
   if (!gm_BrushList.IsEmpty())
   {
      CBrush *pbrush;
      LOGBRUSH lb;
      POSITION pos = gm_BrushList.GetHeadPosition();
      while (pos)
      {
         pbrush = gm_BrushList.GetNext(pos);
         pbrush->GetLogBrush(&lb);
         if ((lb.lbStyle == BS_PATTERN) && (plb->lbStyle == BS_PATTERN))
         {
            if (lb.lbHatch == plb->lbHatch) return pbrush;
         }
         else
         if ((plb->lbColor == lb.lbColor) &&
             (plb->lbHatch == lb.lbHatch) &&
             (plb->lbStyle == lb.lbStyle))
            return pbrush;
      }
   }
   CBrush *pbrush = NULL;
   if (plb->lbStyle == BS_PATTERN)
   {
      ASSERT(false);
      pbrush = new CBrush[2];
   }
   else
      pbrush = new CBrush;

   if (pbrush)
   {
      if (pbrush->CreateBrushIndirect(plb))
      {
         if (plb->lbStyle == BS_PATTERN)
         {
            plb->lbHatch = 0;
            pbrush[1].m_hObject = (HGDIOBJ) plb->lbColor;
            plb->lbColor = 0;
         }
         gm_BrushList.AddHead(pbrush);
//         REPORT("NewBrush %d", gm_BrushList.GetCount());
         return pbrush;
      }
      else
      {
         delete pbrush;
         return NULL;
      }
   }
   return NULL;
}

/******************************************************************************************
* Name      : FindFont                                                                    *
* Definition: CFont * FindFont(LOGFONT* plf);                                             *
* Zweck     : Sucht nach einer Schriftart mit denselben Eigenschaften wie in der Struk-   *
*             tur plf in der globalen Liste CLabel::gm_FontList und liefert einen Zeiger  *
*             auf ein gültiges CFont-Objekt. Wird keine gleiche Schriftart gefunden,      *
*             so wird eine neue Schriftart angelegt und in die Liste eingetragen.         *
*             Bei Mißerfolg liefert diese Funktion NULL.                                  *
* Aufruf    : FindFont(plf)                                                               *
* Parameter :                                                                             * 
* plp    (E): Zeiger auf eine LOGFONT-Struktur                             (LOGFONT*)     *
* Funktonswert: Zeiger auf ein CFont-Objekt                                (CFont*)       *
******************************************************************************************/
CFont *CLabel::FindFont(LOGFONT* plf)
{
   if (!gm_FontList.IsEmpty())
   {
      CFont *pfont;
      LOGFONT lf;
      POSITION pos = gm_FontList.GetHeadPosition();
      while (pos)
      {
         pfont = gm_FontList.GetNext(pos);
         pfont->GetLogFont(&lf);
         if ((plf->lfHeight         == lf.lfHeight        ) &&
             (plf->lfWidth          == lf.lfWidth         ) &&
             (plf->lfEscapement     == lf.lfEscapement    ) &&
             (plf->lfOrientation    == lf.lfOrientation   ) &&
             (plf->lfWeight         == lf.lfWeight        ) &&
             (plf->lfItalic         == lf.lfItalic        ) &&
             (plf->lfUnderline      == lf.lfUnderline     ) &&
             (plf->lfStrikeOut      == lf.lfStrikeOut     ) &&
             (plf->lfCharSet        == lf.lfCharSet       ) &&
             (plf->lfOutPrecision   == lf.lfOutPrecision  ) &&
             (plf->lfClipPrecision  == lf.lfClipPrecision ) &&
             (plf->lfQuality        == lf.lfQuality       ) &&
             (plf->lfPitchAndFamily == lf.lfPitchAndFamily) &&
             (strcmp(plf->lfFaceName, lf.lfFaceName) == 0))  return pfont;
      }
   }
   CFont *pfont = new CFont;
   if (pfont)
   {
      if (pfont->CreateFontIndirect(plf))
      {
         gm_FontList.AddHead(pfont);
//         REPORT("NewFont %d", gm_FontList.GetCount());
         return pfont;
      }
      else
      {
         delete pfont;
         return NULL;
      }
   }
   return NULL;
}

/******************************************************************************************
* Namen : FindPen, FindBrush, FindFont                                                    *
* Zweck : Suchen nach der gleichen Adresse des Objekts in der Objektspezifischen Liste.   *
*         Verkürzung der Suchzeit bei Einfügen von GdiObjekt Zeigern.                     *
* Aufruf    : FindPen(p); FindBrush(p); FindFont(p);                                      *
* Parameter :                                                                             *
* p      (E): Zeiger das GdiObject                            (CPen*), (CBrush*), CFont*) *
* Funktonswert: Index des Objects in der Liste                                (int)       *
******************************************************************************************/
int CLabel::FindPen(CPen* pcp)
{
   if (pcp)
   {
      int count = 0;
      CPen *ptest;
      POSITION pos = gm_PenList.GetHeadPosition();
      while (pos)
      {
         ptest = gm_PenList.GetNext(pos);
         if (ptest == pcp) return count;
         count++;
      }
   }
   return false;
}
int CLabel::FindBrush(CBrush* pcb)
{
   if (pcb)
   {
      int count = 0;
      CBrush *ptest;
      POSITION pos = gm_BrushList.GetHeadPosition();
      while (pos)
      {
         ptest = gm_BrushList.GetNext(pos);
         if (ptest == pcb) return count;
         count++;
      }
   }
   return -1;
}
int CLabel::FindFont(CFont* pcf)
{
   if (pcf)
   {
      int count = 0;
      CFont *ptest;
      POSITION pos = gm_FontList.GetHeadPosition();
      while (pos)
      {
         ptest = gm_FontList.GetNext(pos);
         if (ptest == pcf) return count;
         count++;
      }
   }
   return false;
}

void CLabel::LoadGDIObjects(CArchive &ar)
{
   int i, count;
   DeleteLOGArray();

   ArchiveRead(ar, &count, sizeof(int));   // Pen
   if (count)
   {
      gm_pPenArray = new LOGPEN[count];
      gm_nPens     = count;
      ArchiveRead(ar, gm_pPenArray, sizeof(LOGPEN)*count);
   }

   ArchiveRead(ar, &count, sizeof(int));   // Brush
   if (count)
   {
      gm_pBrushArray = new LOGBRUSH[count];
      gm_nBrushs     = count;
      for (i=0; i<count; i++)
      {
         ArchiveRead(ar, &gm_pBrushArray[i], sizeof(LOGBRUSH));
         if (gm_pBrushArray[i].lbStyle == BS_PATTERN)
         {
            DWORD dwPosition;
            ar.Flush();
            CFile *pFile = ar.GetFile();
            dwPosition = (DWORD)pFile->GetPosition();
            CDibSection *pDibSection = new CDibSection;
            if (pDibSection)
            {
               dwPosition = ::SetFilePointer((HANDLE)pFile->m_hFile, 0, NULL, FILE_CURRENT);
               if (pDibSection->LoadFromFile((HANDLE)pFile->m_hFile))
               {
                  gm_pBrushArray[i].lbColor = (unsigned long) pDibSection;
                  gm_pBrushArray[i].lbHatch = (long) pDibSection->GetBitmapHandle();
               }
               dwPosition = ::SetFilePointer((HANDLE)pFile->m_hFile, 0, NULL, FILE_CURRENT);
            }
         }
      }
   }

   ArchiveRead(ar, &count, sizeof(int));   // Font
   if (count)
   {
      gm_pFontArray = new LOGFONT[count];
      gm_nFonts     = count;
      ArchiveRead(ar, gm_pFontArray, sizeof(LOGFONT)*count);
   }
}

void CLabel::SaveGDIObjects(CArchive &ar)
{
   POSITION pos;
   int      count;

   if (!gm_PenList.IsEmpty())
   {
      CPen *ppen;
      LOGPEN lp;
      count = gm_PenList.GetCount();
      ArchiveWrite(ar, &count, sizeof(int));
      pos = gm_PenList.GetHeadPosition();
      while (pos)
      {
         ppen = gm_PenList.GetNext(pos);
         ASSERT(ppen!=NULL);
         ppen->GetLogPen(&lp);
         // GetLogPen() wird auf WinNT und Win95/88 unterschiedlich behandelt !!!
         // Win95/98 setzt lp.lopnWidth.y = lp.lopnWidth.x
         // WinNT    setzt lp.lopnWidth.y = 0
         lp.lopnWidth.y = lp.lopnWidth.x;
         ArchiveWrite(ar, &lp, sizeof(LOGPEN));
      }
   }
   else 
   {
      count = 0;
      ArchiveWrite(ar, &count, sizeof(int));
   }

   if (!gm_BrushList.IsEmpty())
   {
      CBrush *pbrush;
      LOGBRUSH lb;
      count = gm_BrushList.GetCount();
      ArchiveWrite(ar, &count, sizeof(int));
      pos = gm_BrushList.GetHeadPosition();
      while (pos)
      {
         pbrush = gm_BrushList.GetNext(pos);
         ASSERT(pbrush!=NULL);
         pbrush->GetLogBrush(&lb);
         ArchiveWrite(ar, &lb, sizeof(LOGBRUSH));
         if (lb.lbStyle == BS_PATTERN) 
         {
            CDibSection *pDib = (CDibSection*)pbrush[1].m_hObject;
            if (pDib)
            {
               UINT nSize;
               HGLOBAL  hMem = pDib->GetDIBData(&nSize);
               void    *pMem = ::GlobalLock(hMem);
               ArchiveWrite(ar, &nSize, sizeof(long));
               if (pMem && nSize) ArchiveWrite(ar, pMem, nSize);
               ::GlobalUnlock(hMem);
               ::GlobalFree(hMem);
            }
         }
      }
   }
   else 
   {
      count = 0;
      ArchiveWrite(ar, &count, sizeof(int));
   }

   if (!gm_FontList.IsEmpty())
   {
      CFont *pfont;
      LOGFONT lf;
      count = gm_FontList.GetCount();
      ArchiveWrite(ar, &count, sizeof(int));
      pos = gm_FontList.GetHeadPosition();
      while (pos)
      {
         pfont = gm_FontList.GetNext(pos);
         ASSERT(pfont!=NULL);
         pfont->GetLogFont(&lf);
         ArchiveWrite(ar, &lf, sizeof(LOGFONT));
      }
   }
   else 
   {
      count = 0;
      ArchiveWrite(ar, &count, sizeof(int));
   }
}

void CLabel::DeleteLOGArray()
{
   if (gm_pPenArray)
   {
      delete[] gm_pPenArray;
      gm_nPens     = 0;
      gm_pPenArray = NULL;
   }
   if (gm_pBrushArray)
   {
      for (int i=0; i<gm_nBrushs; i++)
      {
         if ((gm_pBrushArray[i].lbStyle == BS_PATTERN) && (gm_pBrushArray[i].lbColor!=0))
         {
            CDibSection *pDib = (CDibSection*)gm_pBrushArray[i].lbColor;
            delete pDib;
         }
      }
      delete[] gm_pBrushArray;
      gm_nBrushs     = 0;
      gm_pBrushArray = NULL;
   }
   if (gm_pFontArray)
   {
      delete[] gm_pFontArray;
      gm_nFonts      = 0;
      gm_pFontArray  = NULL;
   }
}

void CLabel::DeleteGraficObjects(bool bAll)
{
   if (!gm_PenList.IsEmpty())
   {
      int i, nCount = gm_PenList.GetCount();
      for (i=0; i<nCount; i++)
      {
         CPen *pPen = (CPen*)gm_PenList.RemoveHead();
         if (!bAll && (gm_GlobalGdiObjects.Find(pPen) != NULL))
         {
            gm_PenList.AddTail(pPen);
            continue;
         }
         pPen->DeleteObject();
         delete pPen;
      }
   }
   if (!gm_BrushList.IsEmpty())
   {
      int i, nCount = gm_BrushList.GetCount();
      for (i=0; i<nCount; i++)
      {
         CBrush *pBrush = (CBrush*)gm_BrushList.RemoveHead();
         if (!bAll && (gm_GlobalGdiObjects.Find(pBrush) != NULL))
         {
            gm_BrushList.AddTail(pBrush);
            continue;
         }
         LOGBRUSH lb;
         pBrush->GetLogBrush(&lb);
         if (lb.lbStyle == BS_PATTERN)
         {
            if (pBrush[1].m_hObject)
            {
               CDibSection *pDib   = (CDibSection*)pBrush[1].m_hObject;
               pBrush[1].m_hObject = NULL;
               delete pDib;
            }
            delete[] pBrush;
         }
         else
         {
            pBrush->DeleteObject();
            delete pBrush;
         }
      }
   }
   if (!gm_FontList.IsEmpty())
   {
      int i, nCount = gm_FontList.GetCount();
      for (i=0; i<nCount; i++)
      {
         CFont  *pFont = (CFont*)gm_FontList.RemoveHead();
         if (!bAll && (gm_GlobalGdiObjects.Find(pFont) != NULL))
         {
            gm_FontList.AddTail(pFont);
            continue;
         }
         pFont->DeleteObject();
         delete pFont;
      }
   }
   if (bAll)
   {
      gm_pShapePen = NULL; 
   }
}

CPoint CLabel::PixelToLogical(CDC *pDC, int x, int y)
{
   CSize sizeWinExt = pDC->GetWindowExt();
   CSize sizeVpExt  = pDC->GetViewportExt();
   x = MulDiv(x, sizeWinExt.cx, sizeVpExt.cx);
   y = MulDiv(y, sizeWinExt.cy, sizeVpExt.cy);
   return CPoint(x,y);  
}

void CLabel::Rectangle(CDC * pDC, LPRECT pRect, bool bFill)
{
   CLabel::Rectangle(pDC, pRect->left, pRect->top, pRect->right, pRect->bottom, bFill);
}
/*
   if (bFill) pDC->PatBlt(pRect->left, pRect->top,
                          pRect->right-pRect->left,
                          pRect->bottom-pRect->top, PATCOPY);

   pDC->MoveTo(pRect->left , pRect->top);
   pDC->LineTo(pRect->right, pRect->top);
   pDC->LineTo(pRect->right, pRect->bottom);
   pDC->LineTo(pRect->left , pRect->bottom);
   pDC->LineTo(pRect->left , pRect->top);
}
*/
void CLabel::Rectangle(CDC * pDC, int left, int top, int right, int bottom, bool bFill)
{
   if (bFill) pDC->PatBlt(left, top, right-left, bottom-top, PATCOPY);
   pDC->MoveTo(left , top);
   pDC->LineTo(right, top);
   pDC->LineTo(right, bottom);
   pDC->LineTo(left , bottom);
   pDC->LineTo(left , top);
}

void CLabel::ArchiveWrite(CArchive &ar, const void *p, UINT nSize)
{
   if ((p == NULL) || (nSize == NULL))
	   ::AfxThrowArchiveException(CArchiveException::genericException, ar.GetFile()->GetFileName());
   ar.Write(p, nSize);
   if (ar.m_pDocument != NULL)
   {
      ASSERT_KINDOF(CCARADoc, ar.m_pDocument);
      ((CCARADoc*)ar.m_pDocument)->m_FileHeader.CalcChecksum(p, nSize);
   }
}

void CLabel::ArchiveRead(CArchive &ar, void *p, UINT nSize, bool bForce)
{
   ar.Read(p, nSize);
   if (((gm_nFileVersion > 100) || bForce) && (ar.m_pDocument != NULL))
   {
      ASSERT_KINDOF(CCARADoc, ar.m_pDocument);
      ((CCARADoc*)ar.m_pDocument)->m_FileHeader.CalcChecksum(p, nSize);
   }
}

CPen* CLabel::SelectPen(CDC* pDC, CPen *pPen)
{
   if (pPen)
   {
      if (gm_bColorMatching && (pDC->m_bPrinting || (pDC->m_hDC != pDC->m_hAttribDC)))
      {
         LOGPEN lp;
         pPen->GetLogPen(&lp);
         COLORREF color = pDC->GetNearestColor(lp.lopnColor);
         if (color != lp.lopnColor)
         {
            lp.lopnColor = color;
            return pDC->SelectObject(CLabel::FindPen(&lp));
         }
      }
      return pDC->SelectObject(pPen);
   }
   return NULL;
}

CBrush* CLabel::SelectBrush(CDC* pDC, CBrush *pBrush)
{
   if (pBrush)
   {
      if (gm_bColorMatching && (pDC->m_bPrinting || (pDC->m_hDC != pDC->m_hAttribDC)))
      {
         LOGBRUSH lb;
         pBrush->GetLogBrush(&lb);
         if (lb.lbStyle == BS_HOLLOW)
         {
            return (CBrush*)pDC->SelectStockObject(HOLLOW_BRUSH);
         }
         else if (lb.lbStyle == BS_PATTERN)
         {
            return (CBrush*)CGdiObject::FromHandle(::SelectObject(pDC->m_hDC, pBrush->GetSafeHandle()));
         }
         else
         {
            COLORREF color = pDC->GetNearestColor(lb.lbColor);
            if (color != lb.lbColor)
            {
               lb.lbColor= color;
               return pDC->SelectObject(CLabel::FindBrush(&lb));
            }
         }
      }
      return pDC->SelectObject(pBrush);
   }
   return (CBrush*)pDC->SelectStockObject(HOLLOW_BRUSH);
}

bool CLabel::IsNegativeY(CDC *pDC)
{
   if (pDC == NULL) return false;
   CSize szWnd = pDC->GetWindowExt();
   CSize szVp  = pDC->GetViewportExt();
   return ((szWnd.cy < 0)^(szVp.cy < 0)) ? true : false;
}

TCHAR * CLabel::ReadVariableString(CArchive& ar, bool bForce)
{
   int nSize;
   TCHAR *pszString = NULL;
   CLabel::ArchiveRead(ar, &nSize, sizeof(long), bForce);
   if (nSize > 0)
   {
      pszString = new TCHAR[nSize+1];
      if (pszString)
      {
         CLabel::ArchiveRead(ar, pszString, nSize, bForce);
         pszString[nSize] = 0;
      }
   }
   return pszString;
}

void CLabel::WriteVariableString(CArchive &ar, TCHAR * pszString)
{
   int nSize;
   if (pszString) nSize = _tcsclen(pszString);
   else           nSize = 0;
   CLabel::ArchiveWrite(ar, &nSize, sizeof(long));             // Einheit der Z-Werte
   if (nSize) CLabel::ArchiveWrite(ar, pszString, nSize);
}

int CLabel::MaxPickDistance()
{
   return abs(gm_PickPointSize.cx*gm_PickPointSize.cy);
}

CLabel::CLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CLabel::Constructor");
   #endif
   m_pP      = NULL;
   m_nCount  = 0;
   m_nAct    = 0;
   m_BkColor = 0;
   memset(&m_State, 0, sizeof(BSState));
   SetVisible(true);
   if (!CLabel::gm_pShapePen)
   {
      SetShapePen();
   }
}

/******************************************************************************
* Name       : HitPolygon                                                     *
* Definition : int HitPolygon(CPoint*, int, CPoint*, CRect*);                 *
* Zweck      : Prüft die Berührung des Punktes pp oder des Rechtecks pr mit   *
*              mit einem Polygon, übergegeben durch pPoly mit der Anzahl      *
*              nPolyPoints. Wird für den Punkt NULL übergeben, so wird nur    *
*              das Rechteck geprüft.                                          *
* Aufruf     : HitPolygon(pPoly, nPolyPoints, ppt, prc);                      *
* Parameter  :                                                                *
* pPoly   (E): Polygon-Punktfeld                                  (CPoint*)   *
* nPolyPoints (E): anzahl der Polygonpunkte                       (int)       *
* pp      (E): Koordinaten des Testpunktes                        (CPoint*)   *
* [pr]    (E): Koordinaten des Testrechtecks                      (CRect *)   *
* Funktionswert : >= 1 : Linie oder Rechteck getroffen,           (int)       *
*                 der Wert entspricht dem Betragsquadrat des Abstandsvektors  *
*                   -1 : nicht getroffen)                                     *
******************************************************************************/
int CLabel::HitPolygon(CPoint *pPoly, int nPolyPoints, CPoint *ppt, CRect *prc)
{
   bool bClosed = false;
   int i, nDist;
   if (nPolyPoints < 0)                                  // negative Anzahl
   {
      bClosed = true;                                    // Polygon geschlossen
      nPolyPoints = -nPolyPoints;
   }
   if (nPolyPoints < 2) return IsOutside();              // mindestens 2 Punkte für eine Linie
   nPolyPoints--;                                        // Linien = Punkte - 1
   for (i=0; i<nPolyPoints; i++)                         // für alle Linien
   {
      nDist = HitLine(&pPoly[i], &pPoly[i+1], ppt, prc);
      if (nDist!=IsOutside()) return nDist;              // Linien auf Punktberührung prüfen
   }
   if (bClosed)                                          // Polygon geschlossen ?
   {                                                     // letzte Linie testen
      return HitLine(&pPoly[nPolyPoints], &pPoly[0], ppt, prc);
   }
   return IsOutside();
}

/******************************************************************************
* Name       : HitLine                                                        *
* Definition : int HitLine(CPoint*, CPoint*, CPoint*, CRect pr=NULL);         *
* Zweck      : Prüft die Berührung des Punktes pp oder des Rechtecks pr mit   *
*              der Linie (p1, p2). Wird für den Punkt NULL übergeben, so wird *
*              nur das Rechteck geprüft.                                      *
* Aufruf     : HitLine(pp1, pp2, pp[, pr]);                                   *
* Parameter  :                                                                *
* pp1, pp2(E): Definitionspunkte der Linie                        (CPoint*)   *
* pp      (E): Koordinaten des Testpunktes                        (CPoint*)   *
* [pr]    (E): Koordinaten des Testrechtecks                      (CRect *)   *
* Funktionswert : >= 1 : Linie oder Rechteck getroffen,           (int)       *
*                 der Wert entspricht dem Betragsquadrat des Abstandsvektors  *
*                   -1 : nicht getroffen)                                     *
******************************************************************************/
int CLabel::HitLine(CPoint *pp1, CPoint *pp2, CPoint *pp, CRect*pr)
{
   if (pp)
   {
      if ((((pp1->x <= pp2->x) && (pp->x > pp1->x-gm_PickPointSize.cx) && (pp->x < pp2->x+gm_PickPointSize.cx)) ||
           ((pp1->x >  pp2->x) && (pp->x > pp2->x-gm_PickPointSize.cx) && (pp->x < pp1->x+gm_PickPointSize.cx)))&&
          (((pp1->y <= pp2->y) && (pp->y > pp1->y-gm_PickPointSize.cy) && (pp->y < pp2->y+gm_PickPointSize.cy)) ||
           ((pp1->y >  pp2->y) && (pp->y > pp2->y-gm_PickPointSize.cy) && (pp->y < pp1->y+gm_PickPointSize.cy))))
      {
         CPoint   a,b,c,d;
         int      scale,bq,db;

         a     = *pp  - *pp2;
         b     = *pp1 - *pp2;

         // Projeziere a auf b = c
         scale = a.x*b.x + a.y*b.y; // a*b
         bq    = b.x*b.x + b.y*b.y; // b^2

         c.x   = MulDiv(b.x,scale,bq);
         c.y   = MulDiv(b.y,scale,bq);

         // bestimme den Abstandsvektor zur Linie
         d     = c - a;

         // berechne das Betragsquadrat
         db    = d.x*d.x + d.y*d.y;

         // teste ob der betrag im bereich von range liegt
         if (db <= IsInside()) return db;
      }
   }
   if (pr)
   {
      CPoint p1 = *pp1, p2 = *pp2;
      if (ClipLine(&p1, &p2, pr)) return IsInside();
   }
   return IsOutside();
}

/******************************************************************************
* Name       : HitCurve                                                       *
* Definition : bool HitCurve(CPoint*, CRect *pr=NULL);                        *
* Zweck      : Prüft die Berührung des Punktes pp oder des Rechtecks pr mit   *
*              mit der Kurve, die durch die Funktion Draw() gezeichnet wird.  *
*              Wird für den Punkt NULL übergeben, so wird nur das Rechteck    *
*              geprüft.                                                       *
* Aufruf     : HitCurve(p);                                                   *
* Parameter  :                                                                *
* pp      (E): Koordinaten des Testpunktes                        (CPoint*)   *
* [pr]    (E): Koordinaten des Testrechtecks                      (CRect *)   *
* Funktionswert : >= 1 : Linie oder Rechteck getroffen,           (int)       *
*                 der Wert entspricht dem Betragsquadrat des Abstandsvektors  *
*                   -1 : nicht getroffen)                                     *
******************************************************************************/
int CLabel::HitCurve(CPoint *pp, CRect *pr)
{
   int nump, ndistance = -1, changed;
   CDC dc;
   dc.CreateCompatibleDC(NULL);
   if (dc.BeginPath())                                         // Pfad öffnen
   {
      changed = m_State.changed;                               // Zustand merken, da dieser in Draw verändert wird
      Draw(&dc, NO_PATH);                                      // Zeichnen, Pfad innerhalb von Draw verhindern
      m_State.changed = changed;
      BYTE bDummy = 0;
      if (dc.EndPath() && dc.FlattenPath() && ((nump = dc.GetPath(NULL, &bDummy, 0)) > 0))
      {
         CPoint *ppt;
         BYTE   *pbyte;
         ppt   = new CPoint[nump];                             // Speicherplatz allocieren
         if (ppt)
         {
            pbyte = new BYTE[nump];                            // Speicherplatz allocieren
            if (pbyte)
            {
               nump = dc.GetPath(ppt, pbyte, nump);            // Definitionspunkte der Kurve holen
               ndistance = HitPolygon(ppt, nump, pp, pr);
               delete[] pbyte;                                 // Speicherplatz freigeben
            }
            delete[] ppt;                                      // Speicherplatz freigeben
         }
      }
   }
   return ndistance;
}

#define LABEL_LEFT    0x08
#define LABEL_TOP     0x04
#define LABEL_RIGHT   0x02
#define LABEL_BOTTOM  0x01

/****************************************************************************************
* Name   : ClipLine                                                                     *
* Zweck  : Clippt eine Linie an dem Clipprechteck pr                                    *
* Aufruf : ClipLine(p1, p2);                                                            *
* Parameter :                                                                           *
* p1, p2(EA): Anfangs und Endpunkte der Linie                                 (CPoint)  *
* Funktonswert:                                                                         *
****************************************************************************************/
bool CLabel::ClipLine(CPoint * pp1, CPoint * pp2, CRect * pr)
{
   int ltrb1, ltrb2, count=0, dx(0), dy(0), temp;
   CRect clip=*pr;

   if (clip.top < clip.bottom)                                 // top und bottom tauschen, wenn nötig
   {
      temp = clip.top;
             clip.top = clip.bottom;
                        clip.bottom = temp;
   }

   do
   {
      ltrb1 = 0;
      if (pp1->x < clip.left)   ltrb1 |= LABEL_LEFT;
      if (pp1->y > clip.top)    ltrb1 |= LABEL_TOP;
      if (pp1->x > clip.right)  ltrb1 |= LABEL_RIGHT;
      if (pp1->y < clip.bottom) ltrb1 |= LABEL_BOTTOM;

      ltrb2 = 0;
      if (pp2->x < clip.left)   ltrb2 |= LABEL_LEFT;
      if (pp2->y > clip.top)    ltrb2 |= LABEL_TOP;
      if (pp2->x > clip.right)  ltrb2 |= LABEL_RIGHT;
      if (pp2->y < clip.bottom) ltrb2 |= LABEL_BOTTOM;

      if ((ltrb1|ltrb2)==0) return true;
      if ((ltrb1&ltrb2)!=0) return false;

      if (!count)
      {
         dx = 10 * (pp2->x - pp1->x);
         dy = 10 * (pp2->y - pp1->y);
      }

      if      (ltrb1 & LABEL_LEFT)
      {
         pp1->y = pp1->y + MulDiv(clip.left-pp1->x  , dy, dx);
         pp1->x = clip.left;
//         if (ltrb2 & (LABEL_LEFT|LABEL_RIGHT)) ltrb2 &= (LABEL_LEFT|LABEL_RIGHT);
//       REPORT("p1(%d,%d) : left ",pp1->x, pp1->y);
      }
      else if (ltrb1 & LABEL_TOP)
      {
         pp1->x = pp1->x + MulDiv(clip.top-pp1->y   , dx, dy);
         pp1->y = clip.top;
//         if (ltrb2 & (LABEL_BOTTOM|LABEL_TOP)) ltrb2 &= (LABEL_BOTTOM|LABEL_TOP);
//       REPORT("p1(%d,%d) : top ",pp1->x, pp1->y);
      }
      else if (ltrb1 & LABEL_RIGHT)
      {
         pp1->y = pp1->y + MulDiv(clip.right-pp1->x , dy, dx);
         pp1->x = clip.right;
//         if (ltrb2 & (LABEL_LEFT|LABEL_RIGHT)) ltrb2 &= (LABEL_LEFT|LABEL_RIGHT);
//       REPORT("p1(%d,%d) : right ",pp1->x, pp1->y);
      }
      else if (ltrb1 & LABEL_BOTTOM)
      {
         pp1->x = pp1->x + MulDiv(clip.bottom-pp1->y, dx, dy);
         pp1->y = clip.bottom;
//         if (ltrb2 & (LABEL_BOTTOM|LABEL_TOP)) ltrb2 &= (LABEL_BOTTOM|LABEL_TOP);
//       REPORT("p1(%d,%d) : bottom ",pp1->x, pp1->y);
      }
   
      if (ltrb2 & LABEL_LEFT)
      {
         pp2->y = pp2->y + MulDiv(clip.left  -pp2->x, dy, dx);
         pp2->x = clip.left;
//       REPORT("p2(%d,%d) : left ",pp2->x, pp2->y);
      }
      else if (ltrb2 & LABEL_TOP)
      {
         pp2->x = pp2->x + MulDiv(clip.top   -pp2->y, dx, dy);
         pp2->y = clip.top;
//       REPORT("p2(%d,%d) : top ",pp2->x, pp2->y);
      }  
      else if (ltrb2 & LABEL_RIGHT)
      {
         pp2->y = pp2->y + MulDiv(clip.right -pp2->x, dy, dx);
         pp2->x = clip.right;
//       REPORT("p2(%d,%d) : right ",pp2->x, pp2->y);
      }
      else if (ltrb2 & LABEL_BOTTOM)
      {
         pp2->x = pp2->x + MulDiv(clip.bottom-pp2->y, dx , dy);
         pp2->y = clip.bottom;
//       REPORT("p2(%d,%d) : bottom ",pp2->x, pp2->y);
      } 
   }
   while (count++ < 4);
//   ASSERT(false);
   return 0;
}
#undef LABEL_LEFT
#undef LABEL_TOP
#undef LABEL_RIGHT
#undef LABEL_BOTTOM

CPoint CLabel::GetNearestPoint(CRect*prc, CPoint*ppt)
{
   CPoint ptNearest;
   if (abs(ppt->x - prc->left) < abs(ppt->x - prc->right)) ptNearest.x = prc->left;
   else                                                    ptNearest.x = prc->right;
   if (abs(ppt->y - prc->top) < abs(ppt->y - prc->bottom)) ptNearest.y = prc->top;
   else                                                    ptNearest.y = prc->bottom;

   return ptNearest; 
}

void CALLBACK CLabel::LineDDA_Proc(int x, int y, LPARAM lParam)
{
   ASSERT(lParam != NULL);
   sLineDDA *plDDA = (sLineDDA*)lParam;
   if (plDDA->pcArray)
   {
      if (plDDA->pcArray[plDDA->nCount++])
         plDDA->pDC->SetPixelV(x, y, plDDA->color);
      if (plDDA->nCount >= plDDA->nSize) plDDA->nCount = 0;
   }
   else if (plDDA->nCount++ >= plDDA->nSize)
   {
      plDDA->nCount = 0;
      plDDA->pDC->SetPixelV(x, y, plDDA->color);
   }
}

/******************************************************************************
* Name       : IsOnLabel                                                      *
* Definition : virtual int IsOnLabel(CDC *pDC, CPoint*, CRect*);              *
* Zweck      : liefert 1 wenn Punkt p innerhalb des Rechtecks ist, das von    *
*              der Funktion GetRect() geliefert wird oder prect diese Rechteck*
*              schneidet.                                                     *
* Aufruf     : IsOnLabel(pDC, pP, prect);                                     *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf CDC-Objekt                                (CDC*)    *
* ppoint (E) : Zeiger auf CPoint-Objekt                             (CPoint*) *
* prect  (E) : Zeiger auf CRect-Objekt                              (CRect*)  *
* Funktionswert :                                                   (int)     *
*  1 wenn der Punkt innerhalb des Rechtecks ist                               *
* -1 wenn nicht                                                               *
******************************************************************************/
int CLabel::IsOnLabel(CDC *pDC, CPoint *ppoint, CRect *prect)
{
   CRect rect;
   if (ppoint || prect)
   {
      rect = GetRect(pDC);
      if (rect.IsRectEmpty())
      {
         rect.NormalizeRect();
         rect.InflateRect(1, 1);
      }
   }
   if (ppoint)
   {
      rect.InflateRect(gm_PickPointSize);
      if (rect.PtInRect(*ppoint))
      {
         int nDist = CLabel::IsInside();
         int nTemp;
         if ((nTemp=abs(rect.left   - ppoint->x)) < nDist) nDist = nTemp;
         if ((nTemp=abs(rect.right  - ppoint->x)) < nDist) nDist = nTemp;
         if ((nTemp=abs(rect.top    - ppoint->y)) < nDist) nDist = nTemp;
         if ((nTemp=abs(rect.bottom - ppoint->y)) < nDist) nDist = nTemp;
         return nDist;
      }
   }
   if (prect && rect.IntersectRect(rect, *prect))
      return CLabel::IsInside();
   return CLabel::IsOutside();
}

/******************************************************************************
* Name       : GetRect                                                        *
* Definition : virtual CRect GetRect(CDC* pDC = NULL);                        *
* Zweck      : liefert ein Rechteck, daß alle Definitionspunkte umhüllt.      *
* Aufruf     : GetRect([pDC]);                                                *
* Parameter  :                                                                *
* [pDC]  (E) : Zeiger auf CDC-Objekt [optional]                     (CDC*)    *
* [nFlags](E): Flags für Vergrößerung, Umrechnung in Device Coord   (UINT)    *
* Funktionswert : Umhüllendes Rechteck                              (CRect)   *
******************************************************************************/
CRect CLabel::GetRect(CDC *pDC, UINT nFlags)
{
   CRect r(0,0,0,0);
   if (m_nCount)
   {
      r = CRect(m_pP[0], m_pP[0]);
      int i;
      for (i=1; i<m_nCount; i++)
      {
         if (r.left   > m_pP[i].x) r.left   = m_pP[i].x;
         if (r.top    > m_pP[i].y) r.top    = m_pP[i].y;
         if (r.right  < m_pP[i].x) r.right  = m_pP[i].x;
         if (r.bottom < m_pP[i].y) r.bottom = m_pP[i].y;
      }
   }
   if (nFlags&INFLATE_RECT) r.InflateRect(gm_PickPointSize);
   if ((pDC!=NULL) && (nFlags&DEV_COORD))
   {
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&r, 2);
      r.NormalizeRect();
   }
   return r;
}

bool CLabel::IsBetween(int nPos, int nLower, int nUpper)
{
   return ((nLower <= nPos) && (nPos <= nUpper)) ? true : false;
}

int CLabel::DecreaseLimited(int nValue, int nDecrease, int nLimit)
{
   int i;
   for (i=0; i<nDecrease; i++)
   {
      if (nValue > nLimit) nValue--;
      else break;
   }
   return nValue;
}

int CLabel::IncreaseLimited(int nValue, int nIncrease, int nLimit)
{
   int i;
   for (i=0; i<nIncrease; i++)
   {
      if (nValue < nLimit) nValue++;
      else break;
   }
   return nValue;
}

void CLabel::ChangeGlobalObject(CGdiObject *pOld, CGdiObject *pNew)
{
   if (pOld != pNew)
   {
      if (pOld)
      {
         POSITION pos = gm_GlobalGdiObjects.Find(pOld);
         if (pos) gm_GlobalGdiObjects.RemoveAt(pos);
      }
      if (pNew) gm_GlobalGdiObjects.AddTail(pNew);
   }
}

/******************************************************************************
* Name       : DrawShape                                                      *
* Definition : virtual bool DrawShape(CDC *, bool);                           *
* Zweck      : Zeichnen der Definitionspunkte eines Labels. Im Zustand        *
*              doublepicked werden diese Punkte nicht gezeichnet.             *
*              Die Objekte BLACK_PEN und BLACK_BRUSH werden gewählt.          *
*              Der ROP-Mode R2_NOTXORPEN wird gesetzt um das Bewegen des      *
*              Labels auf dem Bildschirm zu ermöglichen. Dies wird durch      *
*              zeichen (invertieren) der Pixels und löschen (erneutes         *
*              invertieren) erreicht. Der Zustand "shapedrawn" wird ge-       *
*              speichert. Diese Funktion muß in der Umriß-Zeichenfunktion der *
*              abgeleiteten aufgerufen werden, liefert sie false, so darf das *
*              Label nicht ausgegeben werden.                                 *
* Aufruf     : DrawShape(pDC, draw);                                       *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf CDC-Objekt                                  (CDC*)  *
* draw   (E) : Zeichnen (true), löschen (false)                       (bool)  *
* Funktionswert :                                                     (bool)  *
* true = ok, false Objekt war schon gezeichnet oder gelöscht                  *
******************************************************************************/
bool CLabel::DrawShape(CDC *pDC, bool draw)
{
   int i;
   CRect r;
//   REPORT("CLabel::DrawShape(%d)", draw);
   if (IsShapeDrawn() == draw) return false;                   // wenn Zustand vorher gleich, Funktion mit false beenden
   if (!IsVisible() && draw)   return false;                   // ist es unsichtbar, darf es nicht gezeichnet aber gelöscht werden
   pDC->SelectStockObject(BLACK_PEN);
   pDC->SelectStockObject(GRAY_BRUSH);
   pDC->SetROP2(R2_NOTXORPEN);                                 // XOR-Zeichenmodus

   if (DoDrawPickPoints())                                     // Definitionspunkte zeichnen
   {
      for (i=0; i< m_nCount; i++)
      {  
         r = CRect(m_pP[i],m_pP[i]);
         r.InflateRect(CLabel::gm_PickPointSize);
         pDC->PatBlt(r.left,r.top,r.right-r.left,r.bottom-r.top,PATINVERT);
      }
   }

   SetShapeDrawn(draw);
   return true;
}
/******************************************************************************
* Name       : SetMatchPoint                                                  *
* Definition : virtual bool SetMatchPoint(CPoint*); oder                      *
*              bool SetMatchPoint(int);                                       *
* Zweck      : Setzt den aktuellen MatchPoint als Index oder "Pickt" einen    *
*              Punkt des Labels, der mit dem Punkt p übereinstimmt.           *
* Aufruf     : SetMatchPoint(p); oder SetMatchPoint(n);                       *
* Parameter  :                                                                *
* [p]    (E) : Koordinaten des "Pickpunktes"                         (CPoint) *
* [n]    (E) : Index eines Definitionspunktes                        (int)    *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CLabel::SetMatchPoint(CPoint *p)
{
   if (IsFixed()) return false;
   int i;
   CRect r;
   for (i=0; i< m_nCount; i++)
   {
      r = CRect(m_pP[i],m_pP[i]);
      r.InflateRect(gm_PickPointSize);
      if (r.PtInRect(*p)) break;
   }
   return SetMatchPoint(i);
}

void CLabel::SetPoint(CPoint p, int n)                         // setzt den Punkt mit Index n
{
   if (!IsFixed())
   {
      if (IsBetween(n, 0, m_nCount))
      {
         m_pP[n]  = p;
         SetChanged(true);
      }
   }
}

COLORREF CLabel::GetInvertedColor(COLORREF col)
{
   return RGB(255-GetRValue(col), 255-GetGValue(col), 255-GetBValue(col));
}
