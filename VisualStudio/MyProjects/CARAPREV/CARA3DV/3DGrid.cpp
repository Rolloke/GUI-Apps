// 3DGrid.cpp: Implementierung der Klasse C3DGrid.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DGrid.h"
#include "CVector.h"
#include "CEtsDiv.h"
#include "Cara3DGL.h"
#include "ObjectTransformation.h"
#include "LevelColor.h"
#include "..\CaraWinApp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
#define AXIS_EXTENDER    1.05
#define AXIS_EXTEND_FACT 0.05

C3DGrid::C3DGrid()
{
	m_dXmin        = 0.0;
	m_dMinLevel    = 0.0;
	m_dZmin        = 0.0;
	m_dXmax        = 1.0;
	m_dMaxLevel    = 1.0;
	m_dZmax        = 1.0;
	m_dXstep       = 0.0;
	m_dLevelStep   = 0.0;
	m_dZstep       = 0.0;
   m_nXValues     = -1;
   m_nLevelValues = 0;
   m_nZValues     = 0;
   m_dwAxis       = 0;;
   m_dwNewAxis    = SHOW_X_UNIT|SHOW_Y_UNIT|SHOW_Z_UNIT|SHOW_X_CHARACTER|SHOW_Y_CHARACTER|SHOW_Z_CHARACTER|GRID_INVALID|SHOW_INTELLI_GRID;
   m_nNumMode     = ETSDIV_NM_STD;
   m_nRounding    = 1;
   m_nNumModeL    = ETSDIV_NM_STD;
   m_nRoundingL   = 1;
   ZeroMemory(&m_Font,sizeof(LOGFONT));
   HDC hdc = ::GetDC(NULL);
   int nLogPixels = ::GetDeviceCaps(hdc, LOGPIXELSY);
   ::ReleaseDC(NULL, hdc);
   m_Font.lfHeight = -MulDiv(10, nLogPixels, 72);
   m_Font.lfWidth  =  MulDiv( 4, nLogPixels, 72);
   m_Font.lfWeight = 400;
   strcpy(m_Font.lfFaceName, "Arial");
}

void C3DGrid::DrawGrid()
{
   CLevelColor *pLC = GetLevelColors();
   if (m_dwNewAxis & GRID_INVALID) CheckGridUnits(m_dwNewAxis);
   if ((pLC != NULL) && (m_dwAxis&SHOW_GRID))                  // anzeigen des Gitters
   {
      if (m_nXValues ==           -1) CalculateSteps();
      int nX, nY, nZ;
      double x, y, z;
      double dLevelStep     = m_dLevelStep * GetNormFactor(),
             dOffset        = GetOffset(),
             dInvNormFactor = GetInverseNormFactor();
      float fColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

      glShadeModel(GL_SMOOTH);                                 // Shading für die Linien ist immer smooth
      glDisable(GL_LIGHTING);                                  // die Linien werden nicht beleuchtet
      glDisable(GL_NORMALIZE);

      if (m_dwAxis&SHOW_POLAR_GRID)
      {
         double pdSinPhi[72];
         double pdCosPhi[72];
         int    i, nLevel;
         double dRadius, dPhi;
         CObjectTransformation *pOT = GetObjectTransformation();

         if (m_nXValues ==           -1) CalculateSteps();

         for (i=0, dPhi=0; i<72; i++, dPhi+=5)
         {
            pdCosPhi[i] = cos(dPhi*M_PI_D_180);
            pdSinPhi[i] = sin(dPhi*M_PI_D_180);
         }
         z = Vz(pOT->m_vAnchor);

         glPushMatrix();
         glLoadIdentity();
         pOT->SetPolarGridTransformation();

         glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel(0), fColor));
         for (nLevel=0, dRadius=m_dMinLevel; nLevel<m_nLevelValues; nLevel++, dRadius+=dLevelStep)  // für alle Y-Werte
         {                                                        // Die horizontalen Linien zeichnen
            if (dRadius > 0)
            {
               glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((dRadius-dOffset)*dInvNormFactor), fColor));
               glBegin(GL_LINE_LOOP);                             // Linien zeichnen
               for (i=0; i<72; i++)
               {
                  x = pdCosPhi[i] * dRadius;
                  y = pdSinPhi[i] * dRadius;
                  glVertex3d(x, y, z);
               }
               glEnd();
            }
         }
/*
         // Blendfunktion Testprogrammierung
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         fColor[0] = 1;
         fColor[1] = 1;
         fColor[2] = 1;
         fColor[3] = 0.2;
         glColor4fv((float*)fColor);
         glBegin(GL_POLYGON);                             // Linien zeichnen
            glVertex3d(-dRadius,  dRadius, z);
            glVertex3d(-dRadius, -dRadius, z);
            glVertex3d( dRadius, -dRadius, z);
            glVertex3d( dRadius,  dRadius, z);
            glVertex3d(-dRadius,  dRadius, z);
         glEnd();
         glDisable(GL_BLEND);
*/
         glPopMatrix();
      }
      else
      {
         switch (m_nCurrentID)
         {
            case LIST_GRID:                                    // Basisliste für alle Linien
               glBegin(GL_LINES);                              // Linien zeichnen
               for (nY=0, y=m_dMinLevel; nY<m_nLevelValues; nY++, y+=dLevelStep)  // für alle Y-Werte
               {                                               // Die horizontalen Linien zeichnen
                  glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                  for (nX=0, x=m_dXmin; nX<m_nXValues; nX++, x+=m_dXstep)// X-Linien der X-Z-Ebene
                  {
                     glVertex3d(x, y, m_dZmin);
                     glVertex3d(x, y, m_dZmax);
                  }
                  for (nZ=0, z=m_dZmin; nZ<m_nZValues; nZ++, z+=m_dZstep)// Z-Linien der X-Z-Ebene
                  {
                     glVertex3d(m_dXmin, y, z);
                     glVertex3d(m_dXmax, y, z);
                  }
                  if (nY>0)                                    // Die senkrechten Linien zeichnen
                  {
                     for (nZ=0, z=m_dZmin; nZ<m_nZValues; nZ++, z+=m_dZstep)// für alle Z-Werte
                     {
                        for (nX=0, x=m_dXmin; nX<m_nXValues; nX++, x+=m_dXstep)// und alle X-Werte
                        {
                           glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                           glVertex3d(x, y           , z);

                           glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dLevelStep-dOffset)*dInvNormFactor), fColor));
                           glVertex3d(x, y-dLevelStep, z);
                        }
                     }
                  }
               }
               glEnd();
               break;
            case LIST_GRID_LEFT:                               // Liste für Linien links
               glBegin(GL_LINES);                              // Linien zeichnen
               for (nY=0, y=m_dMinLevel; nY<m_nLevelValues; nY++, y+=dLevelStep)  // für alle Y-Werte
               {                                               // Die horizontalen Linien zeichnen
                  glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));

                  x=m_dXmin;
                  glVertex3d(x, y, m_dZmin);
                  glVertex3d(x, y, m_dZmax);
                  if (nY>0)                                    // Die senkrechten Linien zeichnen
                  {
                     for (nZ=0, z=m_dZmin; nZ<m_nZValues; nZ++, z+=m_dZstep)// für alle Z-Werte
                     {
                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y           , z);

                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dLevelStep-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y-dLevelStep, z);
                     }
                  }
               }
               glEnd();
               break;
            case LIST_GRID_TOP:                                // Liste für Linien oben
               glBegin(GL_LINES);                              // Linien zeichnen
               y=m_dMinLevel+dLevelStep*(m_nLevelValues-1);
               glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
               for (nX=0, x=m_dXmin; nX<m_nXValues; nX++, x+=m_dXstep)// X-Linien der X-Z-Ebene
               {
                  glVertex3d(x, y, m_dZmin);
                  glVertex3d(x, y, m_dZmax);
               }
               for (nZ=0, z=m_dZmin; nZ<m_nZValues; nZ++, z+=m_dZstep)// Z-Linien der X-Z-Ebene
               {
                  glVertex3d(m_dXmin, y, z);
                  glVertex3d(m_dXmax, y, z);
               }
               glEnd();
               break;
            case LIST_GRID_RIGHT:                              // Basisliste für alle Linien
               glBegin(GL_LINES);                              // Linien zeichnen
               for (nY=0, y=m_dMinLevel; nY<m_nLevelValues; nY++, y+=dLevelStep)  // für alle Y-Werte
               {                                               // Die horizontalen Linien zeichnen
                  glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                  
                  x=m_dXmin + m_dXstep * (m_nXValues - 1);
                  glVertex3d(x, y, m_dZmin);
                  glVertex3d(x, y, m_dZmax);
                  if (nY>0)                                    // Die senkrechten Linien zeichnen
                  {
                     for (nZ=0, z=m_dZmin; nZ<m_nZValues; nZ++, z+=m_dZstep)// für alle Z-Werte
                     {
                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y           , z);

                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dLevelStep-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y-dLevelStep, z);
                     }
                  }
               }
               glEnd();
               break;
            case LIST_GRID_BOTTOM:                             // Liste für Linien Unten
               glBegin(GL_LINES);                              // Linien zeichnen
               y=m_dMinLevel;
               glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
               for (nX=0, x=m_dXmin; nX<m_nXValues; nX++, x+=m_dXstep)// X-Linien der X-Z-Ebene
               {
                  glVertex3d(x, y, m_dZmin);
                  glVertex3d(x, y, m_dZmax);
               }
               for (nZ=0, z=m_dZmin; nZ<m_nZValues; nZ++, z+=m_dZstep)// Z-Linien der X-Z-Ebene
               {
                  glVertex3d(m_dXmin, y, z);
                  glVertex3d(m_dXmax, y, z);
               }
               glEnd();
               break;
            case LIST_GRID_FRONT:                              // Basisliste für alle Linien
               glBegin(GL_LINES);                              // Linien zeichnen
               for (nY=0, y=m_dMinLevel; nY<m_nLevelValues; nY++, y+=dLevelStep)  // für alle Y-Werte
               {                                               // Die horizontalen Linien zeichnen
                  glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                  z=m_dZmin + m_dZstep * (m_nZValues - 1);
                  glVertex3d(m_dXmin, y, z);
                  glVertex3d(m_dXmax, y, z);
                  if (nY>0)                                    // Die senkrechten Linien zeichnen
                  {
                     for (nX=0, x=m_dXmin; nX<m_nXValues; nX++, x+=m_dXstep)// und alle X-Werte
                     {
                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y           , z);

                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dLevelStep-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y-dLevelStep, z);
                     }
                  }
               }
               glEnd();
               break;
            case LIST_GRID_BACK:                               // Basisliste für alle Linien
               glBegin(GL_LINES);                              // Linien zeichnen
               for (nY=0, y=m_dMinLevel; nY<m_nLevelValues; nY++, y+=dLevelStep)  // für alle Y-Werte
               {                                               // Die horizontalen Linien zeichnen
                  glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                  z=m_dZmin;
                  glVertex3d(m_dXmin, y, z);
                  glVertex3d(m_dXmax, y, z);
                  if (nY>0)                                    // Die senkrechten Linien zeichnen
                  {
                     for (nX=0, x=m_dXmin; nX<m_nXValues; nX++, x+=m_dXstep)// und alle X-Werte
                     {
                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y           , z);

                        glColor4fv(CGlLight::ColorToFloat(pLC->GetColorFromLevel((y-dLevelStep-dOffset)*dInvNormFactor), fColor));
                        glVertex3d(x, y-dLevelStep, z);
                     }
                  }
               }
               glEnd();
               break;
         }
      }
   } 
}

bool C3DGrid::DoDrawAxes() 
{
   if (m_dwNewAxis&GRID_INVALID) return ((m_dwNewAxis&SHOW_AXIS)!=0) ? true : false;
   else                          return ((m_dwAxis   &SHOW_AXIS)!=0) ? true : false;
}

bool C3DGrid::DoDrawGrid() 
{
   if (m_dwNewAxis&GRID_INVALID) return ((m_dwNewAxis&SHOW_GRID)!=0) ? true : false;
   else                          return ((m_dwAxis   &SHOW_GRID)!=0) ? true : false;
}

DWORD C3DGrid::GetAxes()
{
   if (m_dwNewAxis&GRID_INVALID) return m_dwNewAxis;
   else                          return m_dwAxis;
}

void C3DGrid::DrawText(HDC hDC, int nOff)
{
   if (m_nXValues ==           -1) CalculateSteps();
   if (m_dwNewAxis & GRID_INVALID) CheckGridUnits(m_dwNewAxis);

   CLevelColor          *pLC = GetLevelColors();
   const ETS3DGL_Volume *pOV = GetObjectVolume();
   if (m_dwAxis && hDC && pLC && pOV)
   {
      char         text[64] = " ";
      int          nX, nY, nZ, nAlign;
      double       dNormFactor  = GetNormFactor();
      double       dInverseNorm = GetInverseNormFactor();
      double       dOffset      = GetOffset();
      double       dLevelStep   = m_dLevelStep * dNormFactor;
      double       x,y,z;
      ETSDIV_NUTOTXT ntx = {0.0, ETSDIV_NM_STD, 2, 62, 0};
      COLORREF cBkGnd  = ((CCaraWinApp*)AfxGetApp())->GetBackgroundColor();
      COLORREF cInvers = RGB(255-GetRValue(cBkGnd), 255-GetGValue(cBkGnd), 255-GetBValue(cBkGnd));
      HFONT hOldFont   = NULL,
            hFont      = ::CreateFontIndirect(&m_Font);

      ::SetBkMode(hDC, TRANSPARENT);
      hOldFont = (HFONT) ::SelectObject(hDC, hFont);

      if (m_dwAxis&SHOW_GRID)                                  // soll ein Gitter angezeigt werden
      {
         if ((m_dwAxis&SHOW_POLAR_GRID) && (m_dwAxis&SHOW_Z_UNIT))// anzeigen des Gitters und der Z-Einheit
         {
            int nLevel;
            double dRadius;
            CObjectTransformation *pOT = GetObjectTransformation();
            glPushMatrix();                                    // Transformation sichern
            glLoadIdentity();
            pOT->SetPolarGridTransformation();                 // neue Transformation setzen
            nAlign = TA_LEFT;
            ntx.nmode  = m_nNumModeL;
            ntx.nround = m_nRoundingL;

            y = (m_dZmin+m_dZmax) * 0.5;
            z = (m_dXmin+m_dXmax) * 0.5;
            for (nLevel=0, dRadius=m_dMinLevel; nLevel<m_nLevelValues; nLevel++, dRadius+=dLevelStep)
            {                                                  // Die TextLabel für die Radien 
               if (dRadius > 0)
               {
                  ntx.fvalue = dRadius; // ntx.fvalue;         // Textfarbe setzen
                  SetTextColor(hDC, pLC->GetColorFromLevel((dRadius-dOffset)*dInverseNorm));
                  if (CEtsDiv::NumberToText(&ntx, &text[1]))   // TextLabel formatieren
                  {
                     CCara3DGL::TextOut(hDC, dRadius, y, z, nAlign, nOff, text);
                  }
               }
            }
            glPopMatrix();                                     // gesicherte Transformation zurück
         }
         else
         {
            ntx.nmode  = m_nNumMode;
            ntx.nround = m_nRounding;

            if (m_dwAxis&SHOW_X_UNIT)                          // anzeigen des Gitters und der X-Einheit
            {
               nAlign = TA_LEFT;
               nZ = 0;
               if      ((m_dwAxis&SHOW_X_UNIT)==SHOW_X_UNIT1) x = m_dXmin, y = m_dMinLevel;
               else if ((m_dwAxis&SHOW_X_UNIT)==SHOW_X_UNIT2) x = m_dXmax, y = m_dMinLevel;
               else if ((m_dwAxis&SHOW_X_UNIT)==SHOW_X_UNIT3) x = m_dXmin, y = m_dMaxLevel, nAlign |= TA_BOTTOM;
               else if ((m_dwAxis&SHOW_X_UNIT)==SHOW_X_UNIT4) x = m_dXmax, y = m_dMaxLevel, nAlign |= TA_BOTTOM;
               else nZ = m_nZValues, y = m_dMinLevel;          // anderenfalls die Schleife nicht durchlaufen

               SetTextColor(hDC, pLC->GetColorFromLevel((y-dOffset)*dInverseNorm));
               for (ntx.fvalue=m_dZmin; nZ<m_nZValues; nZ++, ntx.fvalue+=m_dZstep)// für alle Y-Werte
               {
                  if (CEtsDiv::NumberToText(&ntx, &text[1]))
                  {
                     CCara3DGL::TextOut(hDC, x, y, ntx.fvalue, nAlign, nOff, text);
                  }
               }
            }

            if (m_dwAxis & SHOW_Y_UNIT)                        // anzeigen des Gitters und der Y-Einheit
            {
               nAlign = TA_LEFT;
               nX = 0;
               if      ((m_dwAxis&SHOW_Y_UNIT)==SHOW_Y_UNIT1) y = m_dMinLevel, z = m_dZmin;
               else if ((m_dwAxis&SHOW_Y_UNIT)==SHOW_Y_UNIT2) y = m_dMinLevel, z = m_dZmax;
               else if ((m_dwAxis&SHOW_Y_UNIT)==SHOW_Y_UNIT3) y = m_dMaxLevel, z = m_dZmin, nAlign |= TA_BOTTOM;
               else if ((m_dwAxis&SHOW_Y_UNIT)==SHOW_Y_UNIT4) y = m_dMaxLevel, z = m_dZmax, nAlign |= TA_BOTTOM;
               else nX = m_nXValues, y = m_dMinLevel;          // anderenfalls die Schleife nicht durchlaufen

               SetTextColor(hDC, pLC->GetColorFromLevel((y-dOffset)*dInverseNorm));
               for (ntx.fvalue=m_dXmin; nX<m_nXValues; nX++, ntx.fvalue+=m_dXstep)// für alle X-Werte
               {
                  if (CEtsDiv::NumberToText(&ntx, &text[1]))
                  {
                     CCara3DGL::TextOut(hDC, ntx.fvalue, y, z, nAlign, nOff, text);
                  }
               }
            }

            ntx.nmode  = m_nNumModeL;
            ntx.nround = m_nRoundingL;
            if (m_dwAxis&SHOW_Z_UNIT)        // anzeigen des Gitters und der Z-Einheit
            {
               double dtemp;
               nY = 0;
               if      ((m_dwAxis&SHOW_Z_UNIT)==SHOW_Z_UNIT1) x = m_dXmax, z = m_dZmin;//, TRACE("z1\n");
               else if ((m_dwAxis&SHOW_Z_UNIT)==SHOW_Z_UNIT2) x = m_dXmin, z = m_dZmin;//, TRACE("z2\n");
               else if ((m_dwAxis&SHOW_Z_UNIT)==SHOW_Z_UNIT3) x = m_dXmin, z = m_dZmax;//, TRACE("z3\n");
               else if ((m_dwAxis&SHOW_Z_UNIT)==SHOW_Z_UNIT4) x = m_dXmax, z = m_dZmax;//, TRACE("z4\n");
               else nY = m_nLevelValues;                                   // anderenfalls die Schleife nicht durchlaufen
               for (dtemp=m_dMinLevel; nY<m_nLevelValues; nY++, dtemp+=dLevelStep)// für alle Z-Werte
               {
                  SetTextColor(hDC, pLC->GetColorFromLevel((dtemp-dOffset)*dInverseNorm));
                  ntx.fvalue = dtemp * dInverseNorm;
                  if (CEtsDiv::NumberToText(&ntx, &text[1]))
                  {
                     CCara3DGL::TextOut(hDC, x, dtemp, z, TA_RIGHT|TA_BASELINE, nOff, text);
                  }
               }
            }
         }
      }
      if ((m_dwAxis&SHOW_AXIS) && (m_dwAxis&SHOW_CHARACTER))     // anzeigen der Achseneinheiten
      {
         double axe_x, axe_y, axe_ymin, axe_ymax, axe_z;
         SetTextColor(hDC, cInvers);
         GetAxlePositions(axe_x, axe_y, axe_ymin, axe_ymax, axe_z);

         if (m_dwAxis&SHOW_Z_CHARACTER)
         {
            wsprintf(text, " Z [%s] ", GetLevelUnit());
            CCara3DGL::TextOut(hDC, 0.0, axe_ymax, 0.0, TA_BOTTOM, nOff, text);
         }

         if (m_dwAxis&SHOW_Y_CHARACTER)
         {
            nAlign = 0;
            if (m_dwAxis&SHOW_Y_CHARACTER_RIGHT ) nAlign |= TA_RIGHT;
            if (m_dwAxis&SHOW_Y_CHARACTER_BOTTOM) nAlign |= TA_BOTTOM;
            wsprintf(text, " Y [%s] ", GetUnit());
            CCara3DGL::TextOut(hDC, axe_x, axe_y, 0.0, nAlign, nOff, text);
         }
         if (m_dwAxis&SHOW_X_CHARACTER)
         {
            nAlign = 0;
            if (m_dwAxis&SHOW_X_CHARACTER_RIGHT ) nAlign |= TA_RIGHT;
            if (m_dwAxis&SHOW_X_CHARACTER_BOTTOM) nAlign |= TA_BOTTOM;
            wsprintf(text, " X [%s] ", GetUnit());
            CCara3DGL::TextOut(hDC, 0.0, axe_y, axe_z, nAlign, nOff, text);
         }
      }
      if (hOldFont)
      {
         ::SelectObject(hDC, hOldFont);
         if (hFont) ::DeleteObject(hFont);
      }
   }
}

void C3DGrid::DrawAxes()
{
   if (m_nXValues ==           -1) CalculateSteps();
   if (m_dwNewAxis & GRID_INVALID) CheckGridUnits(m_dwNewAxis);
   if (m_dwAxis&SHOW_AXIS)                   // anzeigen der Koordinatenachsen
   {
      double axe_x, axe_y, axe_ymin, axe_ymax, axe_z;
      double dConeSize = GetObjectDimension() * CONE_SIZEFACTOR;
      bool bLightOn    = Lighting();
      COLORREF cBkGnd  = ((CCaraWinApp*)AfxGetApp())->GetBackgroundColor();
      COLORREF cInvers = RGB(255-GetRValue(cBkGnd), 255-GetGValue(cBkGnd), 255-GetBValue(cBkGnd));
      float fColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
      
      GetAxlePositions(axe_x, axe_y, axe_ymin, axe_ymax, axe_z);

      glDisable(GL_LIGHTING);
      glColor4fv(CGlLight::ColorToFloat(cInvers, fColor));

      glBegin(GL_LINES);                                       // Linien zeichnen
         glVertex3d(  0.0, axe_ymin,   0.0);                   // Y-Achse
         glVertex3d(  0.0, axe_ymax,   0.0);

         glVertex3d(  0.0,    axe_y,   0.0);                   // Z-Achse
         glVertex3d(  0.0,    axe_y, axe_z);

         glVertex3d(axe_x,    axe_y,   0.0);                   // X-Achse
         glVertex3d(  0.0,    axe_y,   0.0);
      glEnd();

      glDisable(GL_CULL_FACE);                                 // Cull-Face aus wegen der Coneliste

      glPushMatrix();                                          // Z-Achse
      glTranslated(0.0, axe_ymax-dConeSize, 0.0);
      glRotated(-90.0, 1.0, 0.0, 0.0);
      glCallList(LIST_CONE);
      glPopMatrix();

      glPushMatrix();                                          // X-Achse
      glTranslated(axe_x-dConeSize, axe_y,   0.0);
      glRotated( 90.0, 0.0, 1.0, 0.0);
      glCallList(LIST_CONE);
      glPopMatrix();

      glPushMatrix();                                          // Y-Achse
      glTranslated(  0.0, axe_y, axe_z-dConeSize);
      glCallList(LIST_CONE);
      glPopMatrix();
   }
}

void C3DGrid::GetAxlePositions(double &axe_x, double &axe_y, double &axe_ymin, double &axe_ymax, double &axe_z)
{
   const ETS3DGL_Volume *pOV = GetObjectVolume();
   if (pOV != NULL)
   {
      if (pOV->Bottom < 0.0)
      {
         if (m_dwAxis&SHOW_GRID) axe_ymin = pOV->Bottom * AXIS_EXTENDER;
         else                    axe_ymin = pOV->Bottom + AXIS_EXTEND_FACT * (pOV->Top - pOV->Bottom);
      }
      else axe_ymin = pOV->Bottom;

      if (pOV->Top > 0.0)
      {
         if (m_dwAxis&SHOW_GRID) axe_ymax = pOV->Top * AXIS_EXTENDER;
         else                    axe_ymax = pOV->Top + AXIS_EXTEND_FACT * (pOV->Top - pOV->Bottom);
      }
      else axe_ymax = pOV->Top;

      if (m_dwAxis&SHOW_GRID) axe_x = pOV->Far * AXIS_EXTENDER;
      else                    axe_x = pOV->Far + AXIS_EXTEND_FACT * pOV->Far;

      if      (pOV->Bottom < 0.0)
      {
         if (pOV->Top > 0.0)  axe_y = 0.0;
         else                 axe_y = pOV->Top;
      }
      else
      {
         if (pOV->Top > 0.0)  axe_y = pOV->Bottom;
         else                 axe_y = 0.0;
      }
      
      if (m_dwAxis&SHOW_GRID) axe_z = pOV->Right * AXIS_EXTENDER;
      else                    axe_z = pOV->Right + pOV->Right * AXIS_EXTEND_FACT;
   }
   else
   {
      axe_x = axe_y = axe_ymin = axe_ymax = axe_z = 0.0;
   }
}


void C3DGrid::SetStep(double dXstep, double dYstep, double dZstep)
{
	if(dXstep > 0.0) m_dXstep     = dXstep;
	if(dYstep > 0.0) m_dLevelStep = dYstep;
	if(dZstep > 0.0) m_dZstep     = dZstep;
   m_nXValues   = -1;
}

void C3DGrid::CalculateSteps()
{
   if (m_dwAxis&SHOW_POLAR_GRID)
   {
      double dNormFactor = GetNormFactor();
      m_dMinLevel = GetLevelColors()->GetMinLevel()+GetOffset();
      m_dMaxLevel = GetLevelColors()->GetMaxLevel()+GetOffset();
      m_nLevelValues = CalcDiv(m_dMinLevel, m_dLevelStep, m_dMaxLevel, dNormFactor);
      m_nXValues  = 0;
   }
   else
   {
      const ETS3DGL_Volume *pOV = GetObjectVolume();
      if (pOV)
      {
         double dNormFactor = GetNormFactor();
         m_dXmin     = pOV->Near;
         m_dXmax     = pOV->Far;
         m_dMinLevel = pOV->Bottom;
         m_dMaxLevel = pOV->Top;
         m_dZmin     = pOV->Left;
         m_dZmax     = pOV->Right;
         m_nXValues     = CalcDiv(m_dXmin, m_dXstep, m_dXmax, 0);
         m_nLevelValues = CalcDiv(m_dMinLevel, m_dLevelStep, m_dMaxLevel, dNormFactor);
         m_nZValues     = CalcDiv(m_dZmin, m_dZstep, m_dZmax, 0);
      }
      else
      {
         m_nXValues = 0;
         m_nLevelValues = 0;
         m_nZValues = 0;
      }
   }
}

int C3DGrid::CalcDiv(double &dStart, double &dStep, double &dEnd, double dNorm)
{
   ETSDIV_DIV div;
   ZeroMemory(&div, sizeof(ETSDIV_DIV));
   int nVal      = 0;
   div.l2        = 5;
   div.stepwidth = 1;

   if (dNorm != 0)
   {
      div.f1   = dStart / dNorm;
      div.f2   = dEnd   / dNorm;
   }
   else
   {
      div.f1   = dStart;
      div.f2   = dEnd;
   }
   if (dStep == 0.0) div.l2 = 7;
   else              div.l2 = CEtsDiv::Round((div.f2 - div.f1) / dStep);
   if (div.l2 <  2) div.l2 =  2;
   if (div.l2 > 20) div.l2 = 20;
   if (CEtsDiv::CalculateLinDivision(&div, dStep, dStart))
   {
      for ( ;dStart > div.f1; dStart -= dStep);                // vor dem ersten Wert beginnen

      nVal = (int)((div.f2 - dStart) / dStep);
      dEnd = dStart + dStep * nVal;
      for ( ;dEnd < div.f2; dEnd += dStep, nVal++);            // hinter dem letzten Wert aufhören

      nVal++;                                                  // den letzten Wert auch
   }
   if (dNorm != 0)
   {
      dStart *= dNorm; 
      dEnd   *= dNorm;
   }
   return nVal;
}

void C3DGrid::SetAxes(DWORD dwAxes)
{
   m_dwNewAxis = dwAxes|GRID_INVALID;
}

void C3DGrid::CheckGridUnits(DWORD dwNewAxes)
{
   CObjectTransformation *pOT = GetObjectTransformation();
   double dPhi   = pOT->m_dPhi;
   double dTheta = pOT->m_dTheta;

   if (m_dwAxis&SHOW_POLAR_GRID)
   {

   }
   else
   {
      if (dwNewAxes&SHOW_Y_UNIT)
      {
         DWORD dwAxes = SHOW_Y_UNIT;
         if ((dTheta >= 10.0) && (dTheta <= 90.0))
         {
            if      ((dPhi >=   0.0) && (dPhi <  90.0)) dwAxes = SHOW_Y_UNIT2;
            else if ((dPhi >=  90.0) && (dPhi < 180.0)) dwAxes = SHOW_Y_UNIT4;
            else if ((dPhi >= 180.0) && (dPhi < 270.0)) dwAxes = SHOW_Y_UNIT1;
            else if ((dPhi >= 270.0) && (dPhi < 360.0)) dwAxes = SHOW_Y_UNIT3;
         }
         else if ((dTheta >= -90.0) && (dTheta <= -10.0))
         {
            if      ((dPhi >=   0.0) && (dPhi <  90.0)) dwAxes = SHOW_Y_UNIT4;
            else if ((dPhi >=  90.0) && (dPhi < 180.0)) dwAxes = SHOW_Y_UNIT2;
            else if ((dPhi >= 180.0) && (dPhi < 270.0)) dwAxes = SHOW_Y_UNIT3;
            else if ((dPhi >= 270.0) && (dPhi < 360.0)) dwAxes = SHOW_Y_UNIT1;
         }
         if ((dwNewAxes&SHOW_Y_UNIT) != dwAxes)
         {
            dwNewAxes &= ~SHOW_Y_UNIT;
            dwNewAxes |= dwAxes;
         }
      }

      if (dwNewAxes&SHOW_X_UNIT)
      {
         DWORD dwAxes = SHOW_X_UNIT;
         if ((dTheta >= 10.0) && (dTheta <= 90.0))
         {
            if      ((dPhi >=   0.0) && (dPhi <  90.0)) dwAxes = SHOW_X_UNIT4;
            else if ((dPhi >=  90.0) && (dPhi < 180.0)) dwAxes = SHOW_X_UNIT1;
            else if ((dPhi >= 180.0) && (dPhi < 270.0)) dwAxes = SHOW_X_UNIT3;
            else if ((dPhi >= 270.0) && (dPhi < 360.0)) dwAxes = SHOW_X_UNIT2;
         }
         if ((dTheta >= -90.0) && (dTheta <= -10.0))
         {
            if      ((dPhi >=   0.0) && (dPhi <  90.0)) dwAxes = SHOW_X_UNIT2;
            else if ((dPhi >=  90.0) && (dPhi < 180.0)) dwAxes = SHOW_X_UNIT3;
            else if ((dPhi >= 180.0) && (dPhi < 270.0)) dwAxes = SHOW_X_UNIT1;
            else if ((dPhi >= 270.0) && (dPhi < 360.0)) dwAxes = SHOW_X_UNIT4;
         }
         if ((dwNewAxes&SHOW_X_UNIT) != dwAxes)
         {
            dwNewAxes &= ~SHOW_X_UNIT;
            dwNewAxes |= dwAxes;
         }
      }

      if (dwNewAxes&SHOW_Z_UNIT)
      {
         DWORD dwAxes = SHOW_Z_UNIT;
         if ((dTheta > -70.0) && (dTheta < 70.0))
         {
            if      ((dPhi >=   0.0) && (dPhi <  90.0)) dwAxes = SHOW_Z_UNIT2;
            else if ((dPhi >=  90.0) && (dPhi < 180.0)) dwAxes = SHOW_Z_UNIT1;
            else if ((dPhi >= 180.0) && (dPhi < 270.0)) dwAxes = SHOW_Z_UNIT4;
            else if ((dPhi >= 270.0) && (dPhi < 360.0)) dwAxes = SHOW_Z_UNIT3;
         }
         if ((dwNewAxes&SHOW_Z_UNIT) != dwAxes)
         {
            dwNewAxes &= ~SHOW_Z_UNIT;
            dwNewAxes |= dwAxes;
         }
      }

      if (dwNewAxes&SHOW_CHARACTER)
      {
         if ((dPhi >= 0.0) && (dPhi < 180.0))
            dwNewAxes &= ~SHOW_X_CHARACTER_RIGHT;
         else
            dwNewAxes |=  SHOW_X_CHARACTER_RIGHT;

         if ((dPhi >= 90.0) && (dPhi < 270.0))
            dwNewAxes |=  SHOW_Y_CHARACTER_RIGHT;
         else
            dwNewAxes &= ~SHOW_Y_CHARACTER_RIGHT;

         if (dTheta >= 0.0)
         {
            dwNewAxes &= ~SHOW_X_CHARACTER_BOTTOM;
            dwNewAxes &= ~SHOW_Y_CHARACTER_BOTTOM;
         }
         else
         {
            dwNewAxes |=  SHOW_X_CHARACTER_BOTTOM;
            dwNewAxes |=  SHOW_Y_CHARACTER_BOTTOM;
         }
      }
   }
   m_dwAxis     = dwNewAxes;
   m_dwNewAxis &= ~GRID_INVALID;
}

void C3DGrid::GetGridParam(C3DGrid::GridParam&gp)
{
   gp.dXstep     = m_dXstep;
   gp.dZstep     = m_dZstep;
   gp.dLevelStep = m_dLevelStep;
   gp.nNumMode   = m_nNumMode;
   gp.nNumModeL  = m_nNumModeL;
   gp.nRounding  = m_nRounding;
   gp.nRoundingL = m_nRoundingL;
   gp.dwAxis     = GetAxes();
}

void C3DGrid::SetGridParam(C3DGrid::GridParam&gp)
{
   m_dXstep     = gp.dXstep;
   m_dZstep     = gp.dZstep;
   m_dLevelStep = gp.dLevelStep;
   m_nNumMode   = gp.nNumMode;
   m_nNumModeL  = gp.nNumModeL;
   m_nRounding  = gp.nRounding;
   m_nRoundingL = gp.nRoundingL;
   SetAxes(gp.dwAxis);
   InvalidateStep();
}

