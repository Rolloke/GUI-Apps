// LevelColor.cpp: Implementierung der Klasse CLevelColor.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LevelColor.h"
#include "ColorView.h"

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

CLevelColor::CLevelColor()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CLevelColor()");
#endif
   m_dMinLevel = 0.0;
   m_dMaxLevel = 1.0;
   m_dOrigin   = 0.0;
   m_dGradient = 1.0;
   m_bValid = false;
}

COLORREF CLevelColor::GetColorFromLevel(double dLevel)
{
   if (m_pColors)    // Farbwerte
   {
      if (!m_bValid) Validate();
      double dIndex = (m_dOrigin + m_dGradient * dLevel);
      int    nIndex = (int) dIndex;
      dIndex -= (double) nIndex;
      if (dIndex >= 0.5) nIndex++;
      if (CheckRange(nIndex))
         return m_pColors[nIndex];
   }
   return 0;         // schwarz
}

void CLevelColor::SetMinMaxLevel(double dMin, double dMax)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CLevelColor::SetMinMaxLevel");
#endif
   m_dMinLevel = dMin;
   m_dMaxLevel = dMax;
   m_bValid    = false;
}

bool CLevelColor::ReadColors(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CLevelColor::ReadColors");
#endif
   bool bReturn = false;
   int nColors;
   SetArraySize(0);
   CLabel::ArchiveRead(ar, &nColors, sizeof(int), true);
   if ((nColors >= MIN_3D_COLORS) && (nColors <= MAX_3D_COLORS))
   {
      if (SetArraySize(nColors))
      {
         CLabel::ArchiveRead(ar, m_pColors, m_nColors * sizeof(COLORREF), true);
         bReturn = true;
      }
   }
   m_bValid    = false;
   return bReturn;
}

void CLevelColor::WriteColors(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CLevelColor::WriteColors");
#endif
   CLabel::ArchiveWrite(ar, &m_nColors, sizeof(int));
   CLabel::ArchiveWrite(ar, m_pColors, m_nColors * sizeof(COLORREF));
}

void CLevelColor::Validate()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CLevelColor::Validate");
#endif
   if (m_nColors && m_pColors)
   {
      if (m_dMinLevel == m_dMaxLevel) m_dMinLevel = m_dMaxLevel + 1.0;
      m_dGradient = (double)m_nColors / (m_dMaxLevel - m_dMinLevel);
      m_dOrigin   = m_nColors - m_dGradient * m_dMaxLevel;
   }
   else
   {
      m_dGradient = 1.0;
      m_dOrigin   = 0.0;
   }
   m_bValid = true;
}

void CLevelColor::operator = (CColorView& pCopy)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CLevelColor::=CColorView");
#endif
   if (pCopy.GetNumColor() > 0)
   {
      (*((CColorArray*)this)) = pCopy.GetColorArray();
      m_bValid = false;
   }
}

void CLevelColor::operator = (CColorArray &pCopy)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CLevelColor::=CColorArray");
#endif
   (*((CColorArray*)this)) = pCopy;
   m_bValid = false;
}

