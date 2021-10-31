// Pins.cpp: Implementierung der Klasse CPins.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pins.h"
#include "Cara3DGL.h"
#include "3DGrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

char *CPins::gm_szPinText[NO_OF_PINTYPES] = { "B:%d", "L:%d", "E:%d" };

CPins::CPins()
{

}

CPins::~CPins()
{
   DeletePins();
}

void CPins::DeletePins()
{
   if (m_PinTypes.GetCount() > 0)
   {
      PinType *pt;
      while (m_PinTypes.GetHeadPosition())
      {
         pt = (PinType*)m_PinTypes.RemoveHead();
         if (pt->pvPositions) delete[] pt->pvPositions;
         delete pt;
      }
   }
}

void CPins::DrawPins()
{
   const ETS3DGL_Volume *pOV = GetObjectVolume();
   if (pOV)
   {
      double x,y,z;
      float pfColor[4] = {0, 0, 0, 1};
      y = pOV->Top;
      int i;
      bool bLighting = Lighting();
      POSITION pos = m_PinTypes.GetHeadPosition();

      while (pos)
      {
         PinType *pt = (PinType*)m_PinTypes.GetNext(pos);
         ASSERT(pt != NULL);
         ASSERT((pt->nType > 0) && (pt->nType <= NO_OF_PINTYPES));
         if (m_bShow[pt->nType-1])
         {
            COLORREF color, ObjColor = GetObjectColor();
            switch (pt->nType)
            {
               case PINTYPE_BOX:
                  color = ObjColor;
                  break;
               case PINTYPE_LISTENER: case PINTYPE_EXCITER:
                  color = RGB(255-GetRValue(ObjColor),255-GetGValue(ObjColor),255-GetBValue(ObjColor));
                  break;
            }
            glDisable(GL_LIGHTING);    // die Linien werden ohne Licht gezeichnet
            glDisable(GL_NORMALIZE);

            glColor4fv(CGlLight::ColorToFloat(color, pfColor));
            glBegin(GL_LINES);
            for (i=0; i<pt->nPins; i++)
            {
               x = Vy(pt->pvPositions[i]);
               z = Vx(pt->pvPositions[i]);
               glVertex3d(x, y, z);
               glVertex3d(x, pOV->Bottom, z);
            }
            glEnd();

            if (bLighting)             // Nur bei Beleuchtung müssen diese Parameter gesetzt werden
            {
               glEnable(GL_LIGHTING);
               glEnable(GL_NORMALIZE);
               CGlLight::MultnFloat(3, pfColor, LIGHT_COLOR_FACTOR);
               glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pfColor);
            }

            for (i=0; i<pt->nPins; i++)
            {
               x = Vy(pt->pvPositions[i]);
               z = Vx(pt->pvPositions[i]);
               glPushMatrix();
               glTranslated(x, y, z);
               glCallList(LIST_SPHERE);
               glPopMatrix();
            }
         }
      }
   }
}

void CPins::DrawPinsText(HDC hDC, int nOff)
{
   const ETS3DGL_Volume *pOV = GetObjectVolume();
   if (pOV)
   {
      double x,y,z;
      y = pOV->Top;
      int i;
      char pszText[64];

      C3DGrid *pG    = GetGrid();
      LOGFONT Font   = pG->GetFont();
      HFONT hOldFont = NULL,
            hFont    = ::CreateFontIndirect(&Font);
      ::SetBkMode(hDC, TRANSPARENT);
      hOldFont = (HFONT) ::SelectObject(hDC, hFont);

      POSITION pos = m_PinTypes.GetHeadPosition();
      while (pos)
      {
         PinType *pt = (PinType*)m_PinTypes.GetNext(pos);
         ASSERT(pt != NULL);
         ASSERT((pt->nType > 0) && (pt->nType <= NO_OF_PINTYPES));
         if (m_bShow[pt->nType-1])
         {
            COLORREF color, ObjColor = GetObjectColor();
            switch (pt->nType)
            {
               case PINTYPE_BOX:
                  color = RGB(255-GetRValue(ObjColor),255-GetGValue(ObjColor),255-GetBValue(ObjColor));
                  break;
               case PINTYPE_LISTENER: case PINTYPE_EXCITER:
                  color = ObjColor;
                  break;
            }
            ::SetTextColor(hDC, color);
            for (i=0; i<pt->nPins; i++)
            {
               x = Vy(pt->pvPositions[i]);
               z = Vx(pt->pvPositions[i]);
               wsprintf(pszText, gm_szPinText[pt->nType-1], i+1);
               CCara3DGL::TextOut(hDC, x, y, z, TA_VCENTER|TA_CENTER, nOff, pszText);
            }
         }
      }

      if (hOldFont)
      {
         ::SelectObject(hDC, hOldFont);
         if (hFont) ::DeleteObject(hFont);
      }
   }
}

int CPins::GetNoOfPins(int nType)
{
   POSITION pos = m_PinTypes.GetHeadPosition();
   while (pos)
   {
      PinType *pt = (PinType*)m_PinTypes.GetNext(pos);
      if (nType == pt->nType) return pt->nPins;
   }
   return 0;
}

void CPins::ShowPins(int nType, bool bShow)
{
   ASSERT((nType > 0) && (nType <= NO_OF_PINTYPES));
   m_bShow[nType-1] = bShow;
}

bool CPins::ShowPins(int nType)
{
   if (-1 == nType)
   {
      POSITION pos = m_PinTypes.GetHeadPosition();
      while (pos)
      {
         PinType *pt = (PinType*)m_PinTypes.GetNext(pos);
         if ((pt->nPins > 0) && (m_bShow[pt->nType-1]))
            return true;
      }
   }
   else
   {
      ASSERT((nType > 0) && (nType <= NO_OF_PINTYPES));
      POSITION pos = m_PinTypes.GetHeadPosition();
      while (pos)
      {
         PinType *pt = (PinType*)m_PinTypes.GetNext(pos);
         if ((nType == pt->nType) && (pt->nPins > 0))
            return m_bShow[nType-1];
      }
   }
   return false;
}

bool CPins::ReadPins(CArchive &ar, bool bDoc)
{
   int nPinTypes, i;
   CLabel::ArchiveRead(ar, &nPinTypes, sizeof(int), true);
   for (i=0; i<nPinTypes; i++)
   {
      PinType *pt = new PinType;
      if (pt == NULL) return false;
      m_PinTypes.AddHead(pt);
      CLabel::ArchiveRead(ar, &pt->nType, sizeof(int), true);
      CLabel::ArchiveRead(ar, &pt->nPins, sizeof(int), true);
      if (pt->nPins > 0)
      {
         pt->pvPositions = new CVector[pt->nPins];
         if (pt->pvPositions == NULL) return false;
         CLabel::ArchiveRead(ar, pt->pvPositions, sizeof(CVector)*pt->nPins, true);
      }
      else pt->pvPositions = NULL;
   }
   if (bDoc)
   {
      CLabel::ArchiveRead(ar, &nPinTypes, sizeof(int), true);
      if (nPinTypes != NO_OF_PINTYPES) return false;
      CLabel::ArchiveRead(ar, m_bShow, sizeof(bool)*nPinTypes, true);
   }
   return true;
}

void CPins::WritePins(CArchive &ar, bool bDoc)
{
   int nPinTypes = m_PinTypes.GetCount();
   CLabel::ArchiveWrite(ar, &nPinTypes, sizeof(int));
   if (nPinTypes > 0)
   {
      POSITION pos = m_PinTypes.GetHeadPosition();
      while (pos)
      {
         PinType *pt = (PinType*)m_PinTypes.GetNext(pos);
         ASSERT(pt!= NULL);
         CLabel::ArchiveWrite(ar, &pt->nType, sizeof(int));
         CLabel::ArchiveWrite(ar, &pt->nPins, sizeof(int));
         if (pt->nPins > 0)
         {
            ASSERT(pt->pvPositions != NULL);
            CLabel::ArchiveWrite(ar, pt->pvPositions, sizeof(CVector)*pt->nPins);
         }
      }
   }
   if (bDoc)
   {
      nPinTypes = NO_OF_PINTYPES;
      CLabel::ArchiveWrite(ar, &nPinTypes, sizeof(int));
      CLabel::ArchiveWrite(ar, m_bShow   , sizeof(bool)*nPinTypes);
   }
}


