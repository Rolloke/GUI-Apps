// LineLabel.cpp: Implementierung der Klasse CLineLabel.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LineLabel.h"
#include "PreviewFileHeader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CLineLabel, CLabel,0);

CPen *CLineLabel::gm_pPen = NULL;

CLineLabel::CLineLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CLineLabel::Constructor");
   #endif
   Init(&CPoint(0,0));
}

CLineLabel::CLineLabel(CPoint *p)
{ 
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CLineLabel::Constructor(arg)");
   #endif
   Init(p);
   m_nAct = 1;
}

void CLineLabel::Init(CPoint *p)
{
   m_pP = new CPoint[2];
   m_nCount = 2;
   m_pP[0]  = *p;
   m_pP[1]  = *p;

   ZeroMemory(&m_LDDA, sizeof(sLineDDA));

   if (!gm_pPen)
   {  
      LOGPEN lp = {0, {0, 0}, 0};
      CLineLabel::gm_pPen = FindPen(&lp);
      ChangeGlobalObject(NULL, gm_pPen);
   }
   m_pPen = CLineLabel::gm_pPen;
}

CLineLabel::~CLineLabel()
{
   #ifdef CHECK_LABEL_DESTRUCTION
   TRACE("CLineLabel::Destructor\n");
   #endif
   if (m_LDDA.pcArray) delete[] m_LDDA.pcArray;
}

#ifdef _DEBUG
void CLineLabel::AssertValid() const
{
   CLabel::AssertValid();
   ASSERT(m_pPen!=NULL);
   AfxIsMemoryBlock(m_pPen, sizeof(CPen));
}
#endif

bool CLineLabel::Draw(CDC * pDC, int bAtr)
{
   if (!CLabel::Draw(pDC, bAtr)) return false;
   if (m_pP[0] == m_pP[1]) m_pP[1] += gm_PickPointSize;

   if (bAtr)
   {
      pDC->SetBkMode(TRANSPARENT);
      SelectPen(pDC, m_pPen);
   }

   if (m_LDDA.nSize > 0)
   {
      m_LDDA.nCount = 0;
      m_LDDA.pDC    = pDC;
      LineDDA(m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y,  (LINEDDAPROC) LineDDA_Proc, (LPARAM)&m_LDDA);
      m_LDDA.pDC    = NULL;
   }
   else
   {
      pDC->MoveTo(m_pP[1]);
      pDC->LineTo(m_pP[0]);
   }
   return true;
}

bool CLineLabel::DrawShape(CDC * pDC, bool draw)
{
   if (!CLabel::DrawShape(pDC, draw)) return FALSE;
   pDC->SaveDC();
   pDC->SelectObject(CLabel::gm_pShapePen);
   pDC->MoveTo(m_pP[1]);
   pDC->LineTo(m_pP[0]);
   pDC->RestoreDC(-1);
   return true;
}

void CLineLabel::SetLogPen(LOGPEN* plp) 
{
   SetChanged(true);
   m_pPen = CLabel::FindPen(plp);
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pPen, m_pPen);
      gm_pPen = m_pPen;
   }
}

LOGPEN CLineLabel::GetLogPen() 
{
   LOGPEN  LogPen ={0, {0, 0}, 0};
   if (m_pPen) m_pPen->GetLogPen(&LogPen);
   return LogPen;
}

int CLineLabel::IsOnLabel(CDC *pDC, CPoint *ppoint, CRect *prect)
{
   return HitLine(&m_pP[0], &m_pP[1], ppoint, prect);
}

void CLineLabel::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CLineLabel::Serialize()");
   #endif
   CLabel::Serialize(ar);
   long index = 0;
   if (ar.IsStoring())
   {
      index = FindPen(m_pPen);
      ASSERT(index >= 0);
      ArchiveWrite(ar, &index, sizeof(long));
   }
   else
   {
      ArchiveRead(ar, &index, sizeof(long));
      m_pPen = FindPen(index);
   }
}

CRect CLineLabel::GetRect(CDC * pDC, UINT nFlags)
{
   CRect rect = CLabel::GetRect(pDC, nFlags&INFLATE_RECT);
   if (nFlags&INFLATE_RECT)
   {
      LOGPEN pen = {0,{0,0},0};
      if (m_pPen)
      {
         m_pPen->GetLogPen(&pen);
         rect.InflateRect(pen.lopnWidth.x, pen.lopnWidth.x);
      }
   }
   if (rect.Width()  == 0) rect.left--, rect.right++;
   if (rect.Height() == 0) rect.top--,  rect.bottom++;

   if ((nFlags&DEV_COORD) && pDC)
   {
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
      rect.NormalizeRect();
   }
   return rect;
}

void CLineLabel::SetDDAPen(int nSize, char *pcArray, COLORREF color)
{
   m_LDDA.color = color;
   m_LDDA.nSize = nSize;
   if (m_LDDA.pcArray)
   {
      delete[] m_LDDA.pcArray;
      m_LDDA.pcArray = NULL;
   }
   if ((pcArray!= NULL) && (m_LDDA.nSize > 0))
   {
       m_LDDA.pcArray = new char[m_LDDA.nSize];
       if (m_LDDA.pcArray)
       {
          memcpy(m_LDDA.pcArray, pcArray, m_LDDA.nSize);
       }
   }
}

void CLineLabel::ResetGlobalObjects()
{
   gm_pPen = NULL;
}
