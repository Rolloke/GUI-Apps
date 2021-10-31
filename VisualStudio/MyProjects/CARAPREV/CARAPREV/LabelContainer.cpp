// LabelContainer.cpp: Implementierung der Klasse CLabelContainer.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LabelContainer.h"
#include "CurveLabel.h"
#include "PlotLabel.h"
#include "PreviewFileHeader.h"

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
IMPLEMENT_SERIAL(CLabelContainer, CLabel,0);

CLabelContainer::CLabelContainer()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CLabelContainer::Constructor");
   #endif
   m_pL                  = NULL;
   m_State.scaleable     = true; //   Seitenverh‰ltnis wird angeh‰ngt
   m_State.deletecontent = false;
   int nCount            = 2;
   m_nCount              = nCount;
   if (m_State.scaleable) nCount++;
   m_pP                  = new CPoint[nCount];
   m_pP[0]               = CPoint(0,0);
   m_pP[1]               = CPoint(0,0);
   SetDrawPickPoints(false);
};

#ifdef _DEBUG
void CLabelContainer::AssertValid() const
{
   CLabel::AssertValid();
   CLabel     *pL;
   POSITION    pos;

   pos = m_List.GetHeadPosition();
   while (pos)
   {
      pL = m_List.GetNext(pos);
      ASSERT(pL!=NULL);
      ASSERT_VALID(pL);
   }
}
#endif

CLabelContainer::~CLabelContainer()
{
   #ifdef CHECK_LABEL_DESTRUCTION
   REPORT("CLabelContainer::Destructor");
   #endif
   if (m_State.deletecontent)
      DeleteAll();
   else
      RemoveAll();
}

/******************************************************************************
* Name       : Draw                                                           *
* Definition : bool Draw(CDC *);                                              *
* Zweck      : Zeichnen aller sichtbaren Labels in der Liste                  *
* Aufruf     : Draw(pDC)                                                      *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* Funktionswert : (true, false)                                     (bool)    *
******************************************************************************/
bool CLabelContainer::Draw(CDC *pDC, int bAtr)
{
   if (CLabel::Draw(pDC, bAtr))
   {
      CLabel     *pLabel;
      POSITION    pos;
  
      pos = m_List.GetHeadPosition();
      while (pos)
      {
         pLabel = m_List.GetNext(pos);
         if (pLabel) pLabel->Draw(pDC, bAtr);
      }
      return true;
   }
   return false;
}
/******************************************************************************
* Name       : DrawShape                                                      *
* Definition : bool DrawShape(CDC *, bool);                                   *
* Zweck      : Zeichnen und Lˆschen des Umrisses aller Labels                 *
* Aufruf     : DrawShape(pDC, draw);                                          *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* draw   (E) : true  : Zeichnen der Umrisse,                        (bool)    *
*              false : Lˆschen der Umrisse                                    *
* Funktionswert : (true, false)                                     (bool)    *
******************************************************************************/
bool CLabelContainer::DrawShape(CDC *pDC, bool draw)
{
//   if (!CLabel::DrawShape(pDC, draw)) return false;
   bool bDraw = CLabel::DrawShape(pDC, draw);
   if (m_List.IsEmpty() && bDraw)                                       // Ziehen eines Bereichsrechtecks
   {
      pDC->SaveDC();
      pDC->SelectObject(CLabel::gm_pShapePen);
      pDC->SelectStockObject(HOLLOW_BRUSH);
      CLabel::Rectangle(pDC, m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y, false);
      pDC->RestoreDC(-1);
   }
   else if (IsVisible())
   {
      CLabel *pl = NULL;
      POSITION pos;
      if (draw)                                                // beim zeichnen der Label von hinten nach vorne
      {
         pos = m_List.GetHeadPosition();
         while (pos)
         {
            pl = m_List.GetNext(pos);
            if (pl)
            {
               if (pl->IsShapeDrawn()) pl->SetShapeDrawn(false);
               pl->DrawShape(pDC, draw);
            }
         }
      }
      else                                                     // beim lˆschen der Labels von vorne nach hinten
      {
         pos = m_List.GetTailPosition();
         while (pos)
         {
            pl = m_List.GetPrev(pos);
            if (pl)
            {
               if (!pl->IsShapeDrawn()) pl->SetShapeDrawn(true);
               pl->DrawShape(pDC, draw);
            }
         }
      }
   }
   return bDraw;
}

/******************************************************************************
* Name       : SetPoint                                                       *
* Definition : void SetPoint(CPoint);                                         *
* Zweck      : ƒndern eines Eckpunktes des Containers und Skalierung der      *
*              enthaltenen Label.                                             *
* Aufruf     : SetPoint(p);                                                   *
* Parameter  :                                                                *
* p       (E): Neue Koordinaten des Eckpunktes                       (CPoint) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CLabelContainer::SetPoint(CPoint p)
{
   if (IsFixed()) return;
   if (IsPointPicked() && DoDrawPickPoints())
   {
      CRect  rcDef(m_pP[0],m_pP[1]);
      CPoint ptCenter = rcDef.CenterPoint();
      CSize szOld(m_pP[1].x-m_pP[0].x, m_pP[1].y-m_pP[0].y);
      CSize szNew;

      if (m_nAct == 0)
      {
         CheckScale(p, m_pP[1]);
         rcDef.left = p.x;
         rcDef.top  = p.y;
         szNew.cx   = m_pP[1].x - p.x;
         szNew.cy   = m_pP[1].y - p.y;
      }
      else
      {
         CheckScale(m_pP[0], p);
         rcDef.right  = p.x;
         rcDef.bottom = p.y;
         szNew.cx     = p.x - m_pP[0].x;
         szNew.cy     = p.y - m_pP[0].y;
      }

      if ((abs(szNew.cx) > 15)&&(abs(szNew.cy) > 15))
      {
         MoveLabel(-ptCenter);
         ScaleLabel(szNew, szOld);
         ptCenter = rcDef.CenterPoint();
         MoveLabel(ptCenter);
         m_pP[0] = rcDef.TopLeft();
         m_pP[1] = rcDef.BottomRight();
      }
   }
   else CLabel::SetPoint(p);
}

/******************************************************************************
* Name       : MoveLabel                                                      *
* Definition : void MoveLabel(CPoint);                                        *
* Zweck      : Addieren eines Offsets zu allen Punkten aller Labels in der    *
*              Liste.                                                         *
* Aufruf     : MoveLabel(dp)                                                  *
* Parameter  :                                                                *
* dp     (E) : Offsetvector                                       (CPoint)    *
* Funktionswert : void                                                        *
******************************************************************************/
void CLabelContainer::MoveLabel(CPoint dp)
{
   if (!IsFixed())
   {
      POSITION pos = m_List.GetHeadPosition();
      while (pos) m_List.GetNext(pos)->MoveLabel(dp);

      if (DoDrawPickPoints()) CLabel::MoveLabel(dp);
   }
}

/******************************************************************************
* Name       : ScaleLabel                                                     *
* Definition : void ScaleLabel(CSize, CSize);                                 *
* Zweck      : Skalieren der Label in X- und Y-Richtung.                      *
* Aufruf     : ScaleLabel(Numerator, Denominator);                            *
* Parameter  :                                                                *
* Numerator   (E) : Multiplikator (neue Grˆﬂe)                       (CSize)  *
* Denominator (E) : Divisor       (alte Grˆﬂe)                       (CSize)  *
* Funktionswert : void                                                        *
******************************************************************************/
void CLabelContainer::ScaleLabel(CSize Numerator, CSize Denominator)
{
   if (!IsFixed())
   {
      POSITION pos = m_List.GetHeadPosition();
      while (pos) m_List.GetNext(pos)->ScaleLabel(Numerator, Denominator);

      if (DoDrawPickPoints()) CLabel::ScaleLabel(Numerator, Denominator);
   }
}

/******************************************************************************
* Name       : PickLabel                                                      *
* Definition : PickLabel(CPoint *,CLabelContainer *,CDC *,bool,int,CRgn *);   *
* Zweck      : Picken eines Labels oder mehrer Labels durch den Punkt p aus   *
*              der Labelliste plc. Definieren des Anfangspunktes f¸r ein      *
*              f¸r ein Rechteck zum Picken mehrerer Labels.                   *
*              Kann z.B. in der Funktion OnLButtonDown() implementiert werden.*
* Aufruf     : PickLabel(p, plc, pDC, b_add);                                 *
* Parameter  :                                                                *
* p      (E) : z.B.: Koordinate des Cursors                         (CPoint*) *
* plc    (E) : Zeiger auf eine Liste aus der gepickt wird  (CLabelContainer*) *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* b_add  (E) : Addieren eines Labels (true, false)                  (bool)    *
* n_cm   (E) : Kombinationsmodus f¸r die Updateregion               (int)     *
*              siehe Funktion GetRgn().                                       *
* prgn   (EA): Zeiger auf Objekt CRgn mit der Region der Labels     (CRgn*)   *
* bDrSh  (E) : Umriﬂ zeichnen oder lˆschen (true, false)            (bool)    *
* Funktionswert  : Flags                                            (int)     *
* CL_LABELUNPICKED : Es wurden Label "losgelassen".                           *
* CL_TRACKPICKRECT : Nichts wurde gepickt, es wird ein Bereichsrechteck       *
*                  "gezogen".                                                 *
* CL_NEWLABELPICKED: Ein neues Label wurde gepickt                            *
* CL_LABELPICKED   : Ein oder mehrere Label sind gepickt                      *
* CL_LABELCHANGED  : Ein oder mehrere Label wurden ver‰ndert                  *
******************************************************************************/
int CLabelContainer::PickLabel(CPoint *p, CLabelContainer *plc, CDC *pDC, bool b_add, int n_cm, CRgn *prgn, bool bDrSh)
{
   CLabel *plnew=NULL, *plold=NULL;
   int     rvalue = 0;

   if (plc && p)
   {
      plnew = plc->HitLabel(p, pDC);
      if (plnew && plnew->IsFixed()) return rvalue;
   }

   if ((!b_add) && (!IsEmpty()))                               // wenn das Label nicht addiert werden soll
   {
      if (m_List.Find(plnew)) rvalue |= CL_LABELPICKED;
      POSITION pos = m_List.GetHeadPosition();                 // Liste leeren
      while (pos)
      {
         plold = m_List.GetNext(pos);                          // n‰chste Position und n‰chstes Element der Liste holen
         if (plold != plnew)
         {
            if (plold->IsChanged()) rvalue |= CL_LABELCHANGED;
            plold->ResetPickStates();                          // Pickstates zur¸cksetzen
            if (bDrSh) plold->DrawShape(pDC, false);
            plold->GetRgn(n_cm, pDC, prgn, INFLATE_RECT);
            rvalue |= CL_LABELUNPICKED;
         }
      }
      RemoveAll();
      if (rvalue & CL_LABELPICKED) InsertLabel(plnew);
   }

   if (p && (!plnew) && (IsEmpty()))                           // wenn nichts gepickt wurde und die Liste leer ist
   {
      CLabel::SetPoint(*p, 0);                                 // ersten Punkt setzen
      CLabel::SetPoint(*p, 1);                                 // zweiten Punkt setzen
      SetChanged(false);
      SetPicked(true);                                         // gepickt setzen
      if (bDrSh) DrawShape(pDC, true);                         // Umriﬂ zeichen
      rvalue |= CL_TRACKPICKRECT;
   }
   else if (InsertLabel(plnew))                                // gepicktes Label einf¸gen
   {
      if (plnew->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         ((CLabelContainer*)plnew)->SetPicked(true);           // gepickt setzen
      else
         plnew->SetPicked(true);
      if (bDrSh) plnew->DrawShape(pDC, true);
      rvalue |= (CL_NEWLABELPICKED|CL_LABELPICKED);
   }  
   else m_pL = plnew;                                          // ein gepicktes Label, daﬂ nicht eingef¸gt wird
                                                               // war schon gepickt und wird in der Funktion OnPickRect wieder losgelassen.
   return  rvalue;
}

/******************************************************************************
* Name       : EditLabelPoints                                                *
* Definition : void EditLabelPoints(CPoint *, CPoint *, CDC *);               *
* Zweck      : Editieren der Label-Punkte abh‰ngig von den Zust‰nden der Liste*
*              und ihrer enthaltenen Labels. Kann z.B.: in der Funktion       *
*              OnMouseMove() implementiert werden.                            *
*              Ist die Liste leer, wird ein rechteckiger Bereich gezogen, der *
*              einen Pickbereich definiert. Labels, die  sich in diesen       *
*              Bereich befinden, werden in der Funktion OnPickRect() aus einer*
*              ¸bergebenen LabelListe in diese Liste selektiert. Der Anfangs- *
*              Punkt des Rechtecks wird in der Funktion PickLabel() definiert.*
*                                                                             *
*              Enth‰lt die Liste nur ein Objekt Label, so werden einzelne     *
*              Punkte - je nach Labelzustand - durch den Punkt p editiert.    *
*                                                                             *
*              Enth‰lt die Liste ein oder mehrere Objekte Label, die gepickt  *
*              sind aber nicht DoublePicked oder PointPicked, so werden die   *
*              in der Liste enhalten Label um einen in dem Punkt dp definier- *
*              ten Offset verschoben.                                         *
* Aufruf     : EditLabelPoints(CPoint *p, CPoint *dp, CDC *pDC)               *
* Parameter  :                                                                *
* p      (E) : z.B.: Koordinate des Cursors                         (CPoint*) *
* dp     (E) : Verschiebevektor                                     (CPoint*) *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* Funktionswert : Durchgef¸hrte Editier-Option                      (int)     *
*                 1 : Bereichsrechteck ziehen                                 *
*                 2 : Definitionspunkt ver‰ndern                              *
*                 3 : Label bewegen                                           *
******************************************************************************/
int CLabelContainer::EditLabelPoints(CPoint *p, CPoint *dp, CDC *pDC, CRect *pClipRect)
{
   SetChanged(true);
   CLabel *pl;
   if (IsEmpty())                                              // Ziehen eines Bereichs
   {
      if (!IsPicked()) return 0;                               // Nicht gepickt: Fehler
      DrawShape(pDC, false);                                   // lˆschen
      CLabel::SetPoint(*p, 1);
      DrawShape(pDC, true);                                    // darstellen
      return 1;
   }
   else pl = m_List.GetHead();

   if (GetCount() == 1)
   {
      if (pl->IsPointPicked() && !pl->IsFixed())               // bei Objekt‰nderung oder Punktbewegungen
      {
         if (pClipRect)
         {
            CRect ClipRect(*pClipRect);
            ClipRect.NormalizeRect();
            if      (p->x <= ClipRect.left  ) p->x = ClipRect.left+3;
            else if (p->x >= ClipRect.right ) p->x = ClipRect.right-3;
            if      (p->y <= ClipRect.top   ) p->y = ClipRect.top+3;
            else if (p->y >= ClipRect.bottom) p->y = ClipRect.bottom-3;
         }
         pl->DrawShape(pDC, false);                            // lˆschen
         pl->SetPoint(*p);
         pl->DrawShape(pDC, true);                             // darstellen
         return 2;
      }
      if (pl && pl->IsDoublePicked())
      {
      }
   }
   if (pl && pl->IsPicked() || IsPicked())                     // Bewegen von einem oder mehreren Labels
   {
      if (pClipRect)
      {
         CRect ClipRect(*pClipRect);
         ClipRect.NormalizeRect();
         CRect rect = GetRect(pDC);
         rect.NormalizeRect();
         rect.OffsetRect(*dp);
         if      (rect.left   <= ClipRect.left  ) dp->x = 0;
         else if (rect.right  >= ClipRect.right ) dp->x = 0;
         if      (rect.top    <= ClipRect.top   ) dp->y = 0;
         else if (rect.bottom >= ClipRect.bottom) dp->y = 0;
      }
      DrawShape(pDC, false);                                   // lˆschen
      MoveLabel(*dp);
      DrawShape(pDC, true);                                    // darstellen
      return 3;
   }
   return 0;
}
/******************************************************************************
* Name       : OnPickRect                                                     *
* Definition : bool OnPickRect(CPoint *,CLabelContainer *,CDC *,int,CRgn *);  *
* Zweck      : F¸gt Labels aus der Liste plc ein, die sich innerhalb des      *
*              definierten Rechtecks befinden.                                *
*              Liefert die Updateregion, wenn sich die Label in der Liste     *
*              ge‰ndert haben.                                                *
*              Entfernt Label aus der Liste, wenn sie zum zweiten mal gepickt *
*              werden.                                                        *
*              Die Funktion kann z.B.: in der Funktion on LButtonUp implemen- *
*              tiert werden.                                                  *
* Aufruf     : OnPickRect(p, plc, pDC, n_cm, prgn);                           *
* Parameter  :                                                                *
* p      (E) : z.B.: Koordinate des Cursors                         (CPoint*) *
* plc    (E) : Zeiger auf eine Liste aus der gepickt wird  (CLabelContainer*) *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* n_cm   (E) : Kombinationsmodus f¸r die Updateregion               (int)     *
*              siehe Funktion GetRgn().                                       *
* prgn   (EA): Zeiger auf Objekt CRgn mit der Region der Labels     (CRgn*)   *
* Funktionswert : Flags                                             (int)     *
* CL_LABELUNPICKED: Es wurde ein Label "losgelassen".                         *
* CL_TRACKPICKRECT: Die Label innerhalb eines Bereichsrechtecks wurden gepickt*
* CL_LABELPICKED  : ein oder mehrere Label wurden bewegt oder ver‰ndert       *
******************************************************************************/
int CLabelContainer::OnPickRect(CPoint *p, CLabelContainer *plc, CDC *pDC, int n_cm, CRgn *prgn, bool bDrSh)
{
   int rvalue = 0;
   CLabel  *pl;
   if (IsEmpty() && IsPicked() && IsChanged())
   {
      ResetPickStates();                                       // Pickstates zur¸cksetzen
      DrawShape(pDC, false);                        // Umriﬂ lˆschen
      SetChanged(false);
      if (!plc)                    return  false;
      if (plc->m_List.IsEmpty())   return  false;
      POSITION pos  = plc->m_List.GetTailPosition();           // vom Listenende an
      CRect    rect = GetRect(pDC);
      rect.NormalizeRect();
      CRect    urect;
      while (pos)                                              // alle Listenelemente pr¸fen
      {
         pl = plc->m_List.GetPrev(pos);
         if (!pl->IsFixed() && pl->IsVisible() && (pl->IsOnLabel(pDC, NULL, &rect)>=1))
         {
            if (InsertLabel(pl))                               // einf¸gen
            {
               if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))// gepickt setzen
                  ((CLabelContainer*)pl)->SetPicked(true);
               else
                  pl->SetPicked(true);
               if (bDrSh) pl->DrawShape(pDC, true);
               rvalue |= CL_NEWLABELPICKED;
            }
         }
      }
   }
   else if (IsChanged())                                       // ein oder mehrere Label wurden bewegt oder ver‰ndert
   {
      SetChanged(false);
      rvalue |= CL_LABELCHANGED;
      if (GetRgn(n_cm, pDC, prgn, INFLATE_RECT))               // Updateregion merken
         rvalue |= CL_LABELPICKED;
      if (GetCount() == 1) GetLabel(0)->SetPointPicked(false);
   }
   else
   {
      if (RemoveLabel(m_pL))                                   // gepicktes Label aus der Liste nehmen
      {
         m_pL->ResetPickStates();                              // Pickstates zur¸cksetzen
         m_pL->GetRgn(n_cm, pDC, prgn, INFLATE_RECT);
         if (bDrSh) m_pL->DrawShape(pDC, false);
         m_pL = NULL;
         rvalue |= CL_LABELUNPICKED;
      }
   }
   return rvalue;
}

/******************************************************************************
* Name       : SetMatchPoint                                                  *
* Definition : bool SetMatchPoint(CPoint*);                                   *
* Zweck      : Setzen des ƒnderungs-Punktes eines Labels, zum Editieren eines *
*              einzelnen Punktes eines einzelnen Labels in der Pickliste.     *
* Aufruf     : SetMatchPoint(CPoint * p)                                      *
* p      (E) : z.B.: Koordinate des Cursors                         (CPoint*) *
* Parameter  :                                                                *
* Funktionswert : Punkt gesetzt (true, false)                       (bool)    *
******************************************************************************/
bool CLabelContainer::SetMatchPoint(CPoint * p)
{
   if (DoDrawPickPoints())
   {
      if (CLabel::SetMatchPoint(p))
      {
         SetPointPicked(true);
         return true;
      }
   }
   else if (m_List.GetCount() == 1)
   {
      CLabel *pl =m_List.GetHead();
      bool bReturn;
      if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         bReturn = ((CLabelContainer*)pl)->SetMatchPoint(p);
      else if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         bReturn = ((CCurveLabel*)pl)->SetMatchPoint(p);
      else
         bReturn = pl->SetMatchPoint(p);
      if (bReturn)
      {
         pl->SetPointPicked(true);
         return true;
      }
   }
   return false;
}

/******************************************************************************
* Name       : InsertLabel                                                    *
* Definition : bool InsertLabel(CLabel *);                                    *
* Zweck      : Einf¸gen eines Labels in die Liste, dabei wird die G¸ltigkeit  *
*              des Labels ¸berpr¸ft. Ein Label kann nicht zweimal in die Liste*
*              eingef¸gt werden.                                              *
* Aufruf     : InsertLabel(pl)                                                *
* Parameter  :                                                                *
* pl     (E) : Einzuf¸gendes Label                                  (CLabel*) *
* nPos   (E) : Einf¸geposition des Labels                           (int)     *
*              -1 : Am Ende                                                   *
*               0 : Am Anfang                                                 *
*              >0 : An der angegebenen Stelle, wenn mˆglich                   *
* Funktionswert : Label eingef¸gt (true, false)                     (bool)    *
******************************************************************************/
bool CLabelContainer::InsertLabel(CLabel *pl, int nPos)
{
   if (!pl)                                  return false;     // Ist es ein Pointer
   if (m_List.Find(pl))                      return false;     // ist es schon vorhanden ?
   if (nPos != -1)                                             // Label an die Position setzen, wenn mˆglich
   {
      if (nPos==0) m_List.AddHead(pl);                         // Label am Listenanfang einf¸gen
      else
      {
         POSITION pos = m_List.FindIndex(nPos-1);
         if (pos)
         {
            m_List.InsertAfter(pos, pl);
         }
         else return false;
      }
   }
   else m_List.AddTail(pl);                                    // Label hinten einf¸gen
   return true;
}
/******************************************************************************
* Name       : InsertList                                                     *
* Definition : bool InsertList(CLabelContainer *)                             *
* Zweck      : Einf¸gen einer LabelListe in diese Liste. ‹berpr¸fung wie bei  *
*              InsertLabel.                                                   *
* Aufruf     : InsertList(plc);                                               *
* Parameter  :                                                                *
* plc    (E) : Einzuf¸gende Liste                         (CLabelContainer *) *
* Funktionswert : gesamte Liste eingef¸gt (true, false)                (bool) *
******************************************************************************/
bool CLabelContainer::InsertList(CLabelContainer * plc, bool bReverse)
{
   CLabel *pl;
   bool    b_insert = true;
   if (!plc)                                 return false;
   POSITION pos = NULL;
   if (bReverse) pos = plc->m_List.GetTailPosition();
   else          pos = plc->m_List.GetHeadPosition();
   if (!pos)                                 return false;
   while (pos)
   {
      if (bReverse) pl = plc->m_List.GetPrev(pos);
      else          pl = plc->m_List.GetNext(pos);
      b_insert &= InsertLabel(pl);
   }
   return b_insert;
}
/******************************************************************************
* Name       : RemoveLabel                                                    *
* Definition : bool RemoveLabel(CLabel *);                                    *
* Zweck      : Entfernen eines Labels aus der Liste.                          *
* Aufruf     : RemoveLabel(pl);                                               *
* Parameter  :                                                                *
* pl     (E) : Zu entfernendes Label                                (CLabel*) *
* Funktionswert : Label gefunden und entfernt (true, false)         (bool)    *
******************************************************************************/
bool CLabelContainer::RemoveLabel(CLabel *pl)
{
   POSITION    pos;
   pos = m_List.Find(pl);
   int nCount = m_List.GetCount();
   if (pos) 
   {
      m_List.RemoveAt(pos);
      ASSERT(m_List.Find(pl) == NULL);
      return (m_List.GetCount() < nCount) ? true : false;
   }
   return false;
}
/******************************************************************************
* Name       : RemoveList                                                     *
* Definition : bool RemoveList(CLabelContainer *)                             *
* Zweck      : Entfernen einer LabelListe in diese Liste.                     *  
* Aufruf     : RemoveList(plc);                                               *
* Parameter  :                                                                *
* plc    (E) : zu Entfernende Liste                       (CLabelContainer *) *
* Funktionswert : gesamte Liste entfernt (true, false)                 (bool) *
******************************************************************************/
bool CLabelContainer::RemoveList(CLabelContainer * plc)
{
   CLabel *pl;
   bool    b_insert = true;
   if (!plc)                                 return false;
   POSITION pos = plc->m_List.GetHeadPosition();
   if (!pos)                                 return false;
   while (pos)
   {
      pl = plc->m_List.GetNext(pos);
      b_insert &= RemoveLabel(pl);
   }
   return b_insert;
}
/******************************************************************************
* Name       : GetLabel                                                       *
* Definition : CLabel * GetLabel(int);                                        *
* Zweck      : Liefert ein Label mit dem Index [npos] aus der Liste.          *
* Aufruf     : GetLabel(npos);                                                *
* Parameter  :                                                                *
* npos   (E) : Index des Labels in der Liste.                       (int)     *
* Funktionswert : Zeiger auf Label oder NULL bei Fehler             (CLabel*) *
******************************************************************************/
CLabel * CLabelContainer::GetLabel(int npos)
{
   if (npos < 0) return NULL;
   POSITION pos = m_List.FindIndex(npos);
   if (pos) return m_List.GetAt(pos);
   else     return NULL;
}
/******************************************************************************
* Name       : RemoveAll                                                      *
* Definition : void RemoveAll();                                              *
* Zweck      : Entfernen aller Listen Elemente                                *
* Aufruf     : RemoveAll();                                                   *
* Parameter  :                                                                *
* Funktionswert : void                                                        *
******************************************************************************/
void CLabelContainer::RemoveAll()
{
   if (m_List.IsEmpty()) return;
   m_List.RemoveAll();
}
/******************************************************************************
* Name       : DeleteAll                                                      *
* Definition : void DeleteAll()                                               *
* Zweck      : Lˆschen aller Listenelemente, wenn sie allociert wurden.       *
*              !!! Nur anwenden, wenn sie ALLOCIERT wurden !!!                *
* Aufruf     : DeleteAll();                                                   *
* Parameter  :                                                                *
* Funktionswert : void                                                        *
******************************************************************************/
void CLabelContainer::DeleteAll()
{
   if (m_List.IsEmpty()) return;
#ifdef _DEBUG
   CLabel *pl;
   while (m_List.GetHeadPosition())
   {
      pl = m_List.RemoveHead();
      ASSERT_VALID(pl);
      delete pl;
   }
#else
   while (m_List.GetHeadPosition())
      delete m_List.RemoveHead();
#endif
}

/******************************************************************************
* Name       : GetRect                                                        *
* Definition : CRect GetRect(CDC *);                                          *
* Zweck      : Liefert das umh¸llende Rechteck aller Listen-Labels.           *
* Aufruf     : GetRect(CDC *pDC);                                             *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* Funktionswert : Umh¸llendes Rechteck                              (CRect)   *
******************************************************************************/
CRect CLabelContainer::GetRect(CDC *pDC, UINT nFlags)
{
   CRect rect(0,0,0,0);
   CLabel     *pLabel;
   POSITION    pos;
   if (m_List.IsEmpty()) 
   {
      return CLabel::GetRect(pDC, nFlags&INFLATE_RECT);
   }
   pos = m_List.GetHeadPosition();
   while (pos)
   {
      pLabel = m_List.GetNext(pos);
      if (pLabel && pLabel->IsVisible())
         rect.UnionRect(rect, pLabel->GetRect(pDC, nFlags&INFLATE_RECT));
   }
   if (DoDrawPickPoints()) rect.UnionRect(rect, CLabel::GetRect(pDC, nFlags&INFLATE_RECT));

   if ((nFlags&DEV_COORD) && pDC)
   {
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
      rect.NormalizeRect();
   }
   return rect;
}
/******************************************************************************
* Name       : GetRgn                                                         *
* Definition : bool GetRgn(int, CDC *, CRgn *);                               *
* Zweck      : Definiert ein Object CRgn, daﬂ ein Windows GDI-Handle auf eine *
*              Region enth‰lt. Existiert noch keine Region in CRgn, so wird   *
*              eine neue Region erzeugt. Die einzelnen Regionen der Labels in *
*              der Liste werden so zusammengef¸gt, wie es durch den Parameter *
*              n_cm definiert wird.                                           *
* Aufruf     : GetRgn(n_cm, pDC, prgn);                                       *
* Parameter  :                                                                *
* n_cm   (E) : Kombinationsmodus f¸r das Zusammenf¸gen der Region   (int)     *
*              des Labels mit einer evtl. bestehenden im Objekt prgn.         *
*              RGN_AND  : Erzeugt ¸berlappende Region.                        *
*              RGN_COPY : Kopiert die Region des Labels.                      *
*              RGN_DIFF : Entfernt die Region des Labels.                     *
*              RGN_OR   : F¸gt die Region des Labels hinzu.                   *
*              RGN_XOR  : F¸gt die Region des Labels hinzu, entfernt aber die *
*                         ¸berlappende Region beider.                         *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* prgn   (EA): Zeiger auf CRgn-Object                               (CRgn *)  *
* Funktionswert : (true, false)                                     (bool)    *
******************************************************************************/
bool CLabelContainer::GetRgn(int n_cm, CDC * pDC, CRgn *prgn, UINT nFlags)
{
   POSITION    pos;
   CRgn        rgn;

   if (!prgn)                                            return false;
   if (m_List.IsEmpty())                                 return false;

   if (DoDrawPickPoints()) return CLabel::GetRgn(n_cm, pDC, prgn, nFlags&INFLATE_RECT);

   pos = m_List.GetHeadPosition();
   if ((HRGN(*prgn) == NULL) && (pos))
   {
      if (!m_List.GetNext(pos)->GetRgn(RGN_OR|(n_cm&0x0f0), pDC, prgn, nFlags&INFLATE_RECT)) return false;
   }
   while (pos)
   {
      if (!m_List.GetNext(pos)->GetRgn(RGN_OR|(n_cm&0x0f0), pDC, &rgn, nFlags&INFLATE_RECT)) return false;
   }
   if (HRGN(rgn) == NULL) return true;
   return ((prgn->CombineRgn(prgn, &rgn, n_cm&0x0f) == ERROR) ? false : true);
}

/******************************************************************************
* Name       : ResetStates                                                    *
* Definition : void ResetStates();                                            *
* Zweck      : R¸cksetzen der Pickzust‰nde aller enthaltenen Label            *
* Aufruf     : ResetStates();                                                 *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CLabelContainer::ResetStates()
{
   CLabel::ResetPickStates();
   CLabel   *pl;
   POSITION  pos = m_List.GetHeadPosition();
   while (pos)
   {
      pl = m_List.GetNext(pos);
      pl->ResetPickStates();
   }
}
bool CLabelContainer::IsFirst(CLabel *pl)
{
   if (m_List.IsEmpty()) return false;
   if (m_List.GetHead() == pl) return true;
   return false;
}
bool CLabelContainer::IsLast(CLabel *pl)
{
   if (m_List.IsEmpty()) return false;
   if (m_List.GetTail() == pl) return true;
   return false;
}

/******************************************************************************
* Name       : SwapLabels                                                     *
* Definition : bool SwapLabels(POSITION,POSITION,int,CDC *,CRgn *);           *
* Zweck      : Vertauscht die Position zweier Labels in der Liste.            *
* Aufruf     : SwapLabels(pos1, pos2[, n_update, pDC, prgn]);                 *
* Parameter  :                                                                *
* pos1   (E) : Position Label 1 in der Liste                       (POSITION) *
* pos2   (E) : Position Label 2 in der Liste                       (POSITION) *
* [n_cm] (E) : Kombinationsmodus f¸r die Updateregion               (int)     *
*              siehe Funktion GetRgn().                                       *
* [pDC]  (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* [prgn] (EA): Zeiger auf Objekt CRgn mit der Region der Labels     (CRgn*)   *
* Funktionswert : Label vertauscht (true, false)                    (bool)    *
******************************************************************************/
bool CLabelContainer::SwapLabels(POSITION pos1, POSITION pos2, int n_cm, CDC *pDC, CRgn *prgn)
{
   if (pos1 && pos2 && (pos1 != pos2))
   {
      CLabel *pl1 = m_List.GetAt(pos1);
      CLabel *pl2 = m_List.GetAt(pos2);
      m_List.SetAt(pos1, pl2);
      m_List.SetAt(pos2, pl1);
      if ((!pDC)||(!prgn)) return false;
      CRgn rgn;
      pl1->GetRgn(RGN_OR, pDC, &rgn, INFLATE_RECT);
      pl2->GetRgn(RGN_OR, pDC, prgn, INFLATE_RECT);
      prgn->CombineRgn( prgn, &rgn, n_cm|0x0f);
      return true;
   }
   return false;
}

/******************************************************************************
* Name       : Swap**** (Up, Down, First, Last)                               *
* Definition : bool Swap****(CLabel *, CDC *, CRgn *);                        *
* Zweck      : Bewegen der Listenelemente an o.a. Positionen.                 *
* Aufruf     : Swap****(pl[, pDC, prgn]);                                     *
* Parameter  :                                                                *
* pl     (E) : Zeiger auf ein Label in der Liste                    (CLabel*) *
* [pDC]  (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* [prgn] (EA): Zeiger auf Objekt CRgn mit der Region der Labels,    (CRgn*)   *
*              die sich ver‰ndert hat.                                        *
* Funktionswert : Label vertauscht (true, false)                    (bool)    *
******************************************************************************/
bool CLabelContainer::SwapDown(CLabel *pl, CDC *pDC, CRgn * prgn)
{
   POSITION pos1 = m_List.Find(pl);
   if (!pos1) return false;
   POSITION pos2=pos1;
   m_List.GetPrev(pos2);
   return SwapLabels(pos1, pos2, RGN_AND, pDC, prgn);
}
bool CLabelContainer::SwapUp(CLabel *pl, CDC *pDC, CRgn * prgn)
{
   POSITION pos1 = m_List.Find(pl);
   if (!pos1) return false;
   POSITION pos2=pos1;
   m_List.GetNext(pos2);
   return SwapLabels(pos1, pos2, RGN_AND, pDC, prgn);
}
bool CLabelContainer::SwapFirst(CLabel *pl, CDC *pDC, CRgn * prgn)
{
   if (IsFirst(pl)) return false;
   if (RemoveLabel(pl) && m_List.AddHead(pl))
   {
      return pl->GetRgn(RGN_COPY, pDC, prgn, INFLATE_RECT);
   }
   return false;
}
bool CLabelContainer::SwapLast(CLabel *pl, CDC *pDC, CRgn * prgn)
{
   if (IsLast(pl)) return false;
   if (RemoveLabel(pl) && m_List.AddTail(pl))
   {
      return pl->GetRgn(RGN_COPY, pDC, prgn, INFLATE_RECT);
   }
   return false;
}

/******************************************************************************
* Name       : SetPicked                                                      *
* Definition : void SetPicked(bool);                                          *
* Zweck      : Setzen des Zustandes gepickt f¸r alle enthaltenen Label        *
* Aufruf     : SetPicked(b);                                                  *
* Parameter  :                                                                *
* b       (E): (true, false)                                           (bool) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CLabelContainer::SetPicked(bool b)
{
   POSITION pos = m_List.GetHeadPosition();
   CLabel *pl;
   while (pos)
   {
      pl = m_List.GetNext(pos);
      ASSERT_VALID(pl);
      if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         ((CLabelContainer*)pl)->SetPicked(b);           // gepickt setzen
      else
         pl->SetPicked(b);
   }
   CLabel::SetPicked(b);
}

/******************************************************************************
* Name       : Serialize                                                      *
* Definition : void Serialize(CArchive &);                                    *
* Zweck      : Speichern oder Laden des Objektinhaltes                        *
* Aufruf     : Serialize(ar);                                                 *
* Parameter  :                                                                *
* ar      (E): Archiv mit der Datei zum Speichern oder Laden                  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CLabelContainer::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CLabelContainer::Serialize()");
   #endif
   CLabel::Serialize(ar);

   if (ar.IsStoring())
   {
      int count = m_List.GetCount();
      CLabel *pl;
      ArchiveWrite(ar, &count, sizeof(int));
      POSITION pos = m_List.GetHeadPosition();
      while (pos)
      {
         pl = m_List.GetNext(pos);
         ar.WriteObject(pl);
      }
   }
   else
   {
      int      count, i;
      CObject *po;
      ArchiveRead(ar, &count, sizeof(int));
      for (i=0; i<count; i++)
      {
         po = ar.ReadObject(RUNTIME_CLASS(CLabel));
         if (po && po->IsKindOf(RUNTIME_CLASS(CLabel)))
         {
            InsertLabel((CLabel*)po);
         }
      }
   }
}

/******************************************************************************
* Name       : GetChecksum                                                    *
* Definition : void GetChecksum(CFileHeader *);                               *
* Zweck      : Liefert die Checksumme des Objektinhaltes aller enthaltenen    *
*              Label.                                                         *
* Aufruf     : void GetChecksum(pFH);                                         *
* Parameter  :                                                                *
* pFH    (EA): Zeiger auf CFileHeader-Objekt                                  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CLabelContainer::GetChecksum(CFileHeader *pFH)
{
   CLabel::GetChecksum(pFH);
   int count = m_List.GetCount();
   pFH->CalcChecksum(&count, sizeof(int));
   POSITION pos = m_List.GetHeadPosition();
   while (pos)
   {
      CLabel *pl = m_List.GetNext(pos);
      if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
      {
         ((CLabelContainer*)pl)->GetChecksum(pFH);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         ((CCurveLabel*)pl)->GetChecksum(pFH);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
      {
         ((CPlotLabel*)pl)->GetChecksum(pFH);
      }
      else
      {
         pl->GetChecksum(pFH);
      }
   }
}

/******************************************************************************
* Name       : GetIndex                                                       *
* Definition : int GetIndex(CLabel *);                                        *
* Zweck      : Liefert die Position des Labels als Index in der Liste         *
* Aufruf     : GetIndex(pl);                                                  *
* Parameter  :                                                                *
* pl      (E): Zeiger auf CLabel-Objekt                                       *
* Funktionswert : Position des Labels                                (int)    *
******************************************************************************/
int CLabelContainer::GetIndex(CLabel *pl)
{
   int count = 0;
   CLabel *pltest;
   POSITION pos = m_List.GetHeadPosition();
   while (pos)
   {
      pltest = m_List.GetNext(pos);
      if (pltest == pl) return count;
      count++;
   }
   return -1;
}

/******************************************************************************
* Name       : ProcessWithLabels                                              *
* Definition : int ProcessWithLabels(CRuntimeClass*,                          *
                                     void*,int(*)(CLabel*,void*));            *
* Zweck      : Durchl‰uft den Container und alle enthaltenen Container        *
*              rekursiv und ruft die ¸bergebene Callbackfunktion f¸r die ent- *
*              haltenen Label auf.
* Aufruf     : ProcessWithLabels(pRtC, pParam, pCallBack);                    *
* Parameter  :                                                                *
* pRtC     (E): Zeiger auf CRuntimeClass-Objekt zur Auswahl der Labelklasse,  *
*               f¸r die die Callbackfunktion aufgerufen wird. Wird NULL       *
*               ¸begeben, so wird sie f¸r alle Label-klassen aufgerufen.      *
* pParam  (EA): Zeiger auf Parameter f¸r die Callbackfunktion. Hier kann z.B.:*
*               die Struktur ProcessLabel ¸bergeben werden, um Informationen  *
*               auszutauschen.                                                *
*  Parameter der Struktur ProcessLabel:                                       *
*   pl       : ¸bergebenes Labelobjekt                             (CLabel*)  *
*   nPos     : Z‰hlvariable, bzw. Zustandsspeicher                 (int)      *
*   pParam1  : zus‰tzliche Parameter 1                             (void*)    *
*   pParam2  : zus‰tzliche Parameter 2                             (void*)    *
* pCallBack(E): Zeiger auf Callbackfunktion           (int(*)(CLabel*,void*)) *
*  Definition : int(*)(CLabel *pl, void *pParam);                             *
*  Parameter  :                                                               *
*  pl      (E): ‹bergebener Label                                 (CLabel*)   *
*  pParam  (E): Parameter f¸r die Callbackfunktion                (void*)     *
*  Funktionswert : Code f¸r die Weiterverarbeitung des Labels     (int)       *
*     STOP_PROCESSING      : Abbruch der Bearbeitung der Label                *
*     DELETE_LABEL         : entfernen und lˆschen des ¸bergebenen Labels aus *
*                            der Liste                                        *
*     REMOVE_LABEL         : entfernen des Labels aus der Liste               *
*     INSERT_LABEL_BEFORE  : einf¸gen des Labels vor der aktuellen Position   *
*     INSERT_LABEL_AFTER   : einf¸gen des Labels hinter der aktuellen Position*
*                            Das einzf¸gende Label wird in der Struktur       *
*                            ProcessLabel ¸bergeben.                          *
*     DONT_PROCESS_CONTENT : Den Inhalt des ¸bergebenen Labelcontainers nicht *
*                            bearbeiten                                       *
* Funktionswert : R¸ckgabewert der zuletzt aufgerufenen Callbackfunktion (int)*
******************************************************************************/
int CLabelContainer::ProcessWithLabels(CRuntimeClass *pRtC, void *pParam, int (*pCallBack)(CLabel*, void*))
{
   ASSERT(pCallBack != NULL);
   POSITION pos = m_List.GetHeadPosition();
   POSITION oldpos;
   CLabel *pLabel;
   int nReturn = 0;
   while (pos)
   {
      oldpos = pos;
      pLabel = m_List.GetNext(pos);
      if (pLabel)
      {
         nReturn = 0;                                          // R¸ckgabewert f¸r jeden neuen Wert zur¸cksetzen
         if (pLabel->IsKindOf(RUNTIME_CLASS(CLabelContainer))) // ist es ein CLabelContainer
         {
            CLabelContainer *plc = (CLabelContainer*) pLabel;
            if (pRtC == NULL)                                  // keine spezielle Typangabe
               nReturn = pCallBack(pLabel, pParam);            // CallbackFn auch f¸r den Container

            if ((nReturn & DONT_PROCESS_CONTENT)==0)           // diesen Container durchsuchen wenn erw¸nscht
            {
               nReturn = plc->ProcessWithLabels(pRtC, pParam, pCallBack)&STOP_PROCESSING;
            }

            int nCount = plc->GetCount();                      // Wenn ein label gelˆscht wurde
            if ((nCount == 0) || (nCount == 1))                // kann nur noch einer oder auch keiner drin sein
            {
               int nPos = GetIndex(plc);                       // Position ermitteln
               ASSERT(m_List.GetAt(oldpos) == plc);            // darf der Container entfernt werden
               m_List.RemoveAt(oldpos);                        // Container entfernen
               if (nCount == 1)                                // ist noch einer drin
               {
                  pLabel = plc->GetLabel(0);                   // Label holen,
                  plc->RemoveLabel(pLabel);                    // entfernen
                  InsertLabel(pLabel, nPos);                   // und in den aufrufenden Container einf¸gen
               }
               delete plc;                                     // leeren Container lˆschen
            }
            if (nReturn & STOP_PROCESSING) return nReturn;
         }
         else if ((pRtC == NULL) ||                            // keine spezielle Typangabe, alle Label durchsuchen
                 ((pRtC != NULL) && pLabel->IsKindOf(pRtC)))   // oder spezielle Typen durchsuchen
         {
            nReturn = pCallBack(pLabel, pParam);               // CallbackFn auf Label anwenden
         }

         if (nReturn & (DELETE_LABEL|REMOVE_LABEL))         // soll der Label gelˆscht werden
         {
            ASSERT(m_List.GetAt(oldpos) == pLabel);         // darf der Label gelˆscht werden ?
            POSITION temppos = oldpos;
            if (temppos)
            {
               if (nReturn & INSERT_LABEL_BEFORE)     m_List.GetNext(temppos);
               else if (nReturn & INSERT_LABEL_AFTER) m_List.GetPrev(temppos);
               else temppos = NULL;
            }
            m_List.RemoveAt(oldpos);                        // aus der Liste entfernen
            if (nReturn & DELETE_LABEL) delete pLabel;      // und lˆschen
            oldpos = temppos;
         }
         if (nReturn & INSERT_LABEL_BEFORE)
         {
            ProcessLabel *plt = (ProcessLabel*) pParam;
            if (oldpos) m_List.InsertBefore(oldpos, plt->pl);
            else        m_List.AddTail(plt->pl);
         }
         if (nReturn & INSERT_LABEL_AFTER)
         {
            ProcessLabel *plt = (ProcessLabel*) pParam;
            if (oldpos) m_List.InsertAfter(oldpos, plt->pl);
            else        m_List.AddHead(plt->pl);
         }
         if (nReturn & STOP_PROCESSING) return nReturn;
      }
   }
   return nReturn;
}

/******************************************************************************
* Name       : CountLabel                                                     *
* Definition : static int CountLabel(CLabel *pl, void *pParam);               *
* Zweck      : Z‰hlen aller Label(gruppen) in dem Container selbst, sowie auch*
*              in den enthaltenen Containern.                                 *
* Aufruf     : als Callback-Funktion in der Funktion ProcessWithLabels(..);   *
* Parameter  :                                                                *
* pl     (E) : Label aus dem Container, durch die Funktion          (CLabel*) *
*              ProcessWithLabels(..) ¸bergeben.                               *
* pParam (EA): Zeiger auf Z‰hler f¸r die Label                       (int*)   *
* Funktionswert : R¸ckgabeFlags                                      (int)    *
* 0               = weitersuchen,                                             *
******************************************************************************/
int CLabelContainer::CountLabel(CLabel*, void *pParam)
{
   if (pParam) (*((int*)pParam))++;
   return 0;
}
/******************************************************************************
* Name       : CountVisibleLabel                                              *
* Definition : static int CountVisibleLabel(CLabel *pl, void *pParam);        *
* Zweck      : wie oben, jedoch f¸r sichtbare Label                           *
* 0               = weitersuchen,                                             *
******************************************************************************/
int CLabelContainer::CountVisibleLabel(CLabel*pl, void *pParam)
{
   if (pl && pl->IsVisible() && pParam) (*((int*)pParam))++;
   return 0;
}

/******************************************************************************
* Name       : GetIndexFromLabel                                              *
* Definition : static int GetIndexFromLabel(CLabel *pl, void *pParam);        *
* Zweck      : liefert den Index des in der Struktur ProcessLabel ¸bergebenen *
*              Labels von allen enthaltenen Labeln(gruppen) in dem Container  *
*              selbst, sowie auch in den enthaltenen Containern.              *
* Aufruf     : als Callback-Funktion in der Funktion ProcessWithLabels(..);   *
* Parameter  :                                                                *
* pl     (E) : Label aus dem Container, durch die Funktion          (CLabel*) *
*              ProcessWithLabels(..) ¸bergeben.                               *
* pParam (EA): Zeiger auf Struktur ProcessLabel *plt          (ProcessLabel*) *
*  plt->pl   (EA) : gesuchter Label wird auf NULL gesetzt,    (CLabel*)       *
*                   wenn er gefunden wurde.                                   *
*  plt->nPos (EA) : Position des Labels im Container          (int)           *
* Funktionswert : R¸ckgabeFlags                               (int)           *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label lˆschen                                      *
******************************************************************************/
int CLabelContainer::GetIndexFromLabel(CLabel *pl, void *pParam)
{
   if (pl && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      if (plt->pl == pl)
      {
         plt->pl = NULL;
         return STOP_PROCESSING;
      }
      plt->nPos++;
   }
   return 0;
}

int CLabelContainer::GetFirstLabel(CLabel *pl, void *pParam)
{
   if (pl && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      plt->pl = pl;
      return STOP_PROCESSING;
   }
   return 0;
}

/******************************************************************************
* Name       : GetNextLabel                                                   *
* Definition : static int GetNextLabel(CLabel *pl, void *pParam);             *
* Zweck      : liefert den n‰chsten Label der sich im (Sub)Container an der   *
*              in der Struktur ProcessLabel ¸bergebenen Folgeposition         *
*              befindet.                                                      *
* Aufruf     : als Callback-Funktion in der Funktion ProcessWithLabels(..);   *
* Parameter  :                                                                *
* pl     (E) : Label aus dem Container, durch die Funktion          (CLabel*) *
*              ProcessWithLabels(..) ¸bergeben.                               *
* pParam (EA): Zeiger auf Struktur ProcessLabel *plt          (ProcessLabel*) *
*  plt->pl     (E) : der StartLabel                            (CLabel*)      *
*  plt->nPos   (E) : der wievielte n‰chste Label               (int)          *
*  plt->pParam1(EA): gefundener Label                          (void*)        *
*  plt->pParam2(E) : muﬂ mit NULL initialisiert werden         (void*)        *
* Funktionswert : R¸ckgabeFlags                                (int)          *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
******************************************************************************/
int CLabelContainer::GetNextLabel(CLabel *pl, void *pParam)
{
   if (pl && pl->IsVisible() && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      if (plt->pl == pl)
      {
         plt->pParam2 = pl;
      }
      if ((plt->pParam2) && (plt->nPos-- == 0))
      {
         plt->pParam1 = pl;
         return STOP_PROCESSING;
      }
   }
   return 0;
}

/******************************************************************************
* Name       : GetLabelFromIndex                                              *
* Definition : static int GetLabelFromIndex(CLabel *pl, void *pParam);        *
* Zweck      : liefert den Label an der in der Struktur ProcessLabel          *
*              ¸bergebenen Position aus dem Container und dessen enthaltenen  *
*              Containern.                                                    *
* Aufruf     : als Callback-Funktion in der Funktion ProcessWithLabels(..);   *
* Parameter  :                                                                *
* pl     (E) : Label aus dem Container, durch die Funktion          (CLabel*) *
*              ProcessWithLabels(..) ¸bergeben.                               *
* pParam (EA): Zeiger auf Struktur ProcessLabel *plt          (ProcessLabel*) *
*  plt->pl    (A) : gefundener Label                          (CLabel*)       *
*  plt->nPos (EA) : Position des Labels im Container          (int)           *
* Funktionswert : R¸ckgabeFlags                               (int)           *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
******************************************************************************/
int CLabelContainer::GetLabelFromIndex(CLabel *pl, void *pParam)
{
   if (pl && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      if (plt->nPos == 0)
      {
         plt->pl = pl;
         return STOP_PROCESSING;
      }
      plt->nPos--;
   }
   return 0;
}

/******************************************************************************
* Name       : HitLabel                                                       *
* Definition : static int HitLabel(CLabel *pl, void *pParam);                 *
* Zweck      : liefert den Label an der in der Struktur ProcessLabel          *
*              ¸bergebenen Cursorposition innerhalb eines Fangbereichs aus    *
*              dem Container und dessen enthaltenen Containern.               *
* Aufruf     : als Callback-Funktion in der Funktion ProcessWithLabels(..);   *
* Parameter  :                                                                *
* pl     (E) : Label aus dem Container, durch die Funktion          (CLabel*) *
*              ProcessWithLabels(..) ¸bergeben.                               *
* pParam (EA): Zeiger auf Struktur ProcessLabel *plt          (ProcessLabel*) *
*  plt->pl     (A): gefundener Label                          (CLabel*)       *
*  plt->nPos   (E): Fangbereich in logischen Koordianten      (int)           *
*  plt->pParam1(E): Zeiger auf DeviceContext Klasse           (CDC*)          *
*  plt->pParam2(E): Zeiger auf CPoint mit der Cursorposition  (CPoint*)       *
* Funktionswert : R¸ckgabeFlags                               (int)           *
* 0               = weitersuchen,                                             *
******************************************************************************/
int CLabelContainer::HitLabel(CLabel *pl, void *pParam)
{
   if (pl && pParam && pl->IsVisible())
   {
      if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer))) return 0;
      ProcessLabel *ppl = (ProcessLabel*)pParam;
      int nDist = pl->IsOnLabel((CDC*)ppl->pParam1, (CPoint*)ppl->pParam2, NULL);
      if ((nDist != -1) && (nDist < ppl->nPos))
      {
         ppl->nPos = nDist;
         ppl->pl = pl;
      }
   }
   else if (pl && pl->IsKindOf(RUNTIME_CLASS(CLabelContainer))) return DONT_PROCESS_CONTENT;
   return 0;
}

/******************************************************************************
* Name       : ClipLabel                                                      *
* Definition : static int ClipLabel(CLabel *pl, void *pParam);                 *
* Zweck      : Anpassen der Grˆﬂe und Position der Label in dem Container um  *
*              sie in einem definierbaren Cliprechteck darstellen zu kˆnnen.  *
*              Die Label werden proportional skaliert. Wenn sie in das        *
*              Cliprechteck passen, so werden sie nur verschoben.             *
* Aufruf     : als Callback-Funktion in der Funktion ProcessWithLabels(..);   *
* Parameter  :                                                                *
* pl     (E) : Label aus dem Container, durch die Funktion          (CLabel*) *
*              ProcessWithLabels(..) ¸bergeben.                               *
* pParam (EA): Zeiger auf Struktur ProcessLabel *plt          (ProcessLabel*) *
*  plt->pl     (A): NULL                                      (CLabel*)       *
*  plt->nPos   (E): 0 : nicht zeichnen, 1: zeichnen           (int)           *
*  plt->pParam1(E): Zeiger auf DeviceContext Klasse           (CDC*)          *
*  plt->pParam2(E): Zeiger auf CRect mit dem Cliprechteck     (CRect*)        *
* Funktionswert : R¸ckgabeFlags                               (int)           *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label lˆschen                                      *
******************************************************************************/
int CLabelContainer::ClipLabel(CLabel *pl, void *pParam)
{
   if (pl && pParam)
   {
      ProcessLabel *ppl = (ProcessLabel*) pParam;
      CDC          *pDC = (CDC*)          ppl->pParam1;
      CRect        rcClip(*((CRect*)ppl->pParam2));
      rcClip.NormalizeRect();
      CPoint ptMove(0, 0);
      pl->CheckScale();
      CRect  rcLabel = pl->GetRect(pDC);
      CSize  szLabel = rcLabel.Size();
      CSize  szClip  = rcClip.Size() - CSize(6,6);
      CSize  szDiff  = szLabel - szClip;

      if ((szDiff.cx > 1) || (szDiff.cy > 1))                // Ist der Label zu groﬂ
      {
         if      (szDiff.cx <= 0)          szClip.cx = MulDiv(szClip.cy-1, szLabel.cx, szLabel.cy);
         else if (szDiff.cy <= 0)          szClip.cy = MulDiv(szClip.cx-1, szLabel.cy, szLabel.cx);
         else if (szDiff.cx <  szDiff.cy)  szClip.cy = MulDiv(szClip.cx-1, szLabel.cy, szLabel.cx);
         else                              szClip.cx = MulDiv(szClip.cy-1, szLabel.cx, szLabel.cy);
         CPoint ptLabel = rcLabel.CenterPoint();
         if (ppl->nPos) pl->DrawShape(pDC, false);
         pl->MoveLabel(-ptLabel);
         pl->ScaleLabel(szClip, szLabel);
         pl->MoveLabel(ptLabel);
         if (ppl->nPos) pl->DrawShape(pDC, true);
         rcLabel = pl->GetRect(pDC);
      }

      if (rcLabel.left   <= (rcClip.left   + 3)) ptMove.x = rcClip.left  - rcLabel.left  + 3;
      if (rcLabel.right  >= (rcClip.right  - 3)) ptMove.x = rcClip.right - rcLabel.right - 3;

      if (rcLabel.top    <= (rcClip.top    + 3)) ptMove.y = rcClip.top    - rcLabel.top    + 3;
      if (rcLabel.bottom >= (rcClip.bottom - 3)) ptMove.y = rcClip.bottom - rcLabel.bottom - 3;
      if ((ptMove.x != 0) || (ptMove.y != 0))
      {
         if (ppl->nPos) pl->DrawShape(pDC, false);
         pl->MoveLabel(ptMove);
         if (ppl->nPos) pl->DrawShape(pDC, true);
      }
   }
   return 0;
}

/******************************************************************************
* Name       : CheckScaling                                                   *
* Definition : static int CheckScaling(CLabel *pl, void *pParam);             *
* Zweck      : Anpassen der Scalierungsart des Containers an die in ihm ent-  *
*              haltenen Labelobjekte unter Ber¸cksichtigung der Reihenfolge:  *
*              (SCALE_PROPORTIONAL, SCALE_NONPROPORTIONAL, SCALE_ARBITRARY)   *
* Aufruf     : als Callback-Funktion in der Funktion ProcessWithLabels(..);   *
* Parameter  :                                                                *
* pl     (E) : Label aus dem Container, durch die Funktion          (CLabel*) *
*              ProcessWithLabels(..) ¸bergeben.                               *
* pParam (EA): Zeiger auf Struktur ProcessLabel *plt          (ProcessLabel*) *
*  plt->pl     (A): Zeiger auf den zu untersuchenden Container (CLabel*)      *
*  plt->nPos   (E): Am Anfang 0 ! oder die Skalierungsart     (int)           *
*  plt->pParam1(E): NULL                                                      *
*  plt->pParam2(E): NULL                                                      *
* Funktionswert : R¸ckgabeFlags                               (int)           *
* 0               = weitersuchen,                                             *
******************************************************************************/
int CLabelContainer::CheckScaling(CLabel *pl, void *pParam)
{
   if (pParam)
   {
      ProcessLabel *ppl = (ProcessLabel*) pParam;
      if (pl && ppl->pl)
      {
         pl->SetShapeDrawn(false);
         ppl->nPos |= pl->GetScaling();
         if      (ppl->nPos & SCALE_PROPORTIONAL   ) ppl->pl->SetScaling(SCALE_PROPORTIONAL);
         else if (ppl->nPos & SCALE_NONPROPORTIONAL) ppl->pl->SetScaling(SCALE_NONPROPORTIONAL);
      }
      if (ppl->nPos == SCALE_ARBITRARY) ppl->pl->SetScaling(SCALE_ARBITRARY);
   }
   return 0;
}

/******************************************************************************
* Name       : IsOnLabel                                                      *
* Definition : virtual int IsOnLabel(CDC *pDC, CPoint*, CRect*);              *
* Zweck      : liefert den Abstand von Punkt p zum Objekt oder 1, wenn das    *
*              Rechteck prect das  Objekt schneidet.                          *
* Aufruf     : IsOnLabel(pDC, pP, prect);                                     *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf CDC-Objekt                                (CDC*)    *
* ppoint (E) : Zeiger auf CPoint-Objekt                             (CPoint*) *
* prect  (E) : Zeiger auf CRect-Objekt                              (CRect*)  *
* Funktionswert :                                                   (int)     *
*  Abstand des Punktes vom Label oder 1, wenn das Rechtecks getroffen ist     *
* -1 wenn nicht                                                               *
******************************************************************************/
int CLabelContainer::IsOnLabel(CDC *pDC, CPoint *ppoint, CRect *prect)
{
   CLabel *ptest;
   int     ndistance, nbest = CLabel::MaxPickDistance();
   bool    bHit = false;
   POSITION pos   = m_List.GetTailPosition();                  // vom Listenende anfangen
   while (pos)  
   {
      ptest = m_List.GetPrev(pos);
      if (ptest->IsVisible())
      {
         ndistance = ptest->IsOnLabel(pDC, ppoint, prect);
         if ((ndistance != CLabel::IsOutside()) && (ndistance < nbest))
         {
            nbest = ndistance;
            bHit  = true;
         }
      }
   }
   if (bHit) return nbest;
   else      return CLabel::IsOutside();
}

/******************************************************************************
* Name       : HitLabel                                                       *
* Definition : CLabel *HitLabel(CPoint *, CDC *);                             *
* Zweck      : Picken eines Labels                                            *
* Aufruf     : HitLabel(p, pDC);                                              *
* Parameter  :                                                                *
* p      (E) : z.B.: Koordinate des Cursors                         (CPoint*) *
* pDC    (E) : Zeiger auf Devicecontextklasse                       (CDC*)    *
* Funktionswert : gepicktes Label                                   (CLabel*) *
******************************************************************************/
CLabel* CLabelContainer::HitLabel(CPoint *p, CDC *pDC, int *pnDist)
{
   CLabel *ptest, *pbest=NULL;
   int     ndistance, nbest = CLabel::MaxPickDistance();

   POSITION pos   = m_List.GetTailPosition();                  // vom Listenende anfangen
   while (pos)  
   {
      ptest = m_List.GetPrev(pos);
      if (ptest->IsVisible())
      {
         ndistance = ptest->IsOnLabel(pDC, p, NULL);
         if ((ndistance != CLabel::IsOutside()) && (ndistance < nbest))
         {
            nbest = ndistance;
            pbest = ptest;
         }
      }
   }
   if (pnDist) *pnDist = nbest;
   return pbest;
}
