// PolarGraph.cpp: Implementierung der Klasse CPolarGraph.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PolarGraph.h"

#include "Cara3DGL.h"
#include "LevelColor.h"
#include "CEtsDiv.h" 
#include "..\CARADoc.h"

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

CPolarGraph::CPolarGraph()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph()");
#endif
   Init();
   m_nClass3DType = POLAR_GRAPH;
}

CPolarGraph::~CPolarGraph()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("~CPolarGraph()");
#endif
   DeleteData();
}

void CPolarGraph::Init()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::Init");
#endif
   m_nRadiusValues   = 0;
   m_nTheta          = 0;
   m_nPhi            = 0;
   m_fDeltaPhi       = 0;
   m_fDeltaTheta     = 0;
   m_pszUnitOfRadius = NULL;
   m_pfRadiusValues  = NULL;
   m_pvAngleVectors  = NULL;
   m_pdSpecificUnit  = NULL;
   m_hFileForMapping = INVALID_HANDLE_VALUE;
   m_hFileMapping    = INVALID_HANDLE_VALUE;
   m_pszPhaseUnit    = NULL;
   m_pszSpecificUnit = NULL;
   m_pdNormfactor    = NULL;
}

void CPolarGraph::DeleteData()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::DeleteData");
#endif
   if (m_pszUnitOfRadius) delete[] m_pszUnitOfRadius;
   if (m_pvAngleVectors)  delete[] m_pvAngleVectors;
   if (m_pdSpecificUnit)  delete[] m_pdSpecificUnit;

   if ((m_hFileMapping !=NULL) && (m_hFileMapping != INVALID_HANDLE_VALUE))
   {                                                        // bei MemoryMappedFile
      if (m_pfRadiusValues)                                       // Zeiger unmappen
      {
         ::UnmapViewOfFile((void*)m_pfRadiusValues);
      }
      ::CloseHandle(m_hFileMapping);                        // File-Mapping schließen
   }
   else                                                     // durch new alloziert
   {
      if (m_pfRadiusValues)  delete[] m_pfRadiusValues;
   }

   if (m_hFileForMapping != INVALID_HANDLE_VALUE)
   {
      ::CloseHandle(m_hFileForMapping);
   }

   if (m_pszPhaseUnit)    delete[] m_pszPhaseUnit;
   if (m_pszSpecificUnit) delete[] m_pszSpecificUnit;
   if (m_pdNormfactor)    delete[] m_pdNormfactor;

   Init();
}

void CPolarGraph::ReadObjectData(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::ReadObjectData");
#endif
   ReadPolarData(ar);
   ReadLevelData(ar);
}

void CPolarGraph::WriteObjectData(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::WriteObjectData");
#endif
   CLabel::ArchiveWrite(ar, &m_nFilmPictures, sizeof(long) );
   CLabel::ArchiveWrite(ar, &m_fDeltaTheta  , sizeof(float));
   CLabel::ArchiveWrite(ar, &m_fDeltaPhi    , sizeof(float));
   CLabel::ArchiveWrite(ar, &m_nTheta       , sizeof(long) );
   CLabel::ArchiveWrite(ar, &m_nPhi         , sizeof(long) );
   CLabel::WriteVariableString(ar, m_pszUnitOfRadius);
   CLabel::ArchiveWrite(ar, &m_nFlags       , sizeof(long) );
 
   if (m_nFlags&SPECIFICVALUES_AVAILABLE)
   {
      if (m_pdSpecificUnit)
      {
         CLabel::ArchiveWrite(ar, m_pdSpecificUnit, sizeof(double)*m_nFilmPictures);
      }
      CLabel::WriteVariableString(ar, m_pszSpecificUnit);
   }

   if (PhaseValuesAvailable())
   {
      long nSize;
      CLabel::WriteVariableString(ar, m_pszPhaseUnit);

      if      (m_nFlags & NO_VALUE_NORMALIZATION)   nSize = 0;
      else if (m_nFlags & ONE_NORMALIZATION_FACTOR) nSize = 1;
      else if (m_nFlags & N_NORMALIZATION_FACTORS)  nSize = m_nFilmPictures;
      if (nSize)
      {
         if (m_pdNormfactor)
         {
            CLabel::ArchiveWrite(ar, m_pdNormfactor, sizeof(double)*nSize);
         }
      }
   }

   if ((m_nFilmPictures > 0) && (m_nRadiusValues > 0) && (m_pfRadiusValues))
   {
      long nSize = m_nFilmPictures * m_nRadiusValues * sizeof(float);
      if (PhaseValuesAvailable()) nSize *= 2;
      CLabel::ArchiveWrite(ar, m_pfRadiusValues, nSize);
   }
}

bool CPolarGraph::ReadCaleData(CArchive &ar, CString &strHeadline)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::ReadCaleData");
#endif
   float fMinValue, fMaxValue;
   short nDeltaPhi, nDeltaTheta;
   int   i;
   CString dst;

   ASSERT(m_pszUnitOfRadius == NULL);
   ASSERT(m_pfRadiusValues  == NULL);
   char text[64];
   for(i=0; i<12; i++)
   {
      CLabel::ArchiveRead(ar, text, 20, true);                 // Texte
      CCARADoc::DosToWindowText(text);
      strHeadline += _T(text);
      strHeadline += _T(" ");
   }

   m_pszUnitOfRadius = new char[6];
   CLabel::ArchiveRead(ar, m_pszUnitOfRadius, 6, true);        // Einheit

   CLabel::ArchiveRead(ar, &nDeltaPhi, sizeof(short), true);   // gdriver
   if ((nDeltaPhi != 3) && (nDeltaPhi != 9)) return false;     // DOS (VGA, EGA)
   CLabel::ArchiveRead(ar, &nDeltaPhi, sizeof(short), true);   // gmode
   if ((nDeltaPhi != 1) && (nDeltaPhi != 2)) return false;     // DOS (VGA, EGA)

   CLabel::ArchiveRead(ar, &fMinValue, sizeof(float), true);   // Min Max
   CLabel::ArchiveRead(ar, &fMaxValue, sizeof(float), true);
   m_dMinValue = (double)fMinValue;
   m_dMaxValue = (double)fMaxValue;

   CLabel::ArchiveRead(ar, &nDeltaPhi, sizeof(short), true);   // DeltaPhi
   m_fDeltaPhi   = (float) nDeltaPhi;
   CLabel::ArchiveRead(ar, &nDeltaTheta, sizeof(short), true); // DeltaTheta
   m_fDeltaTheta = (float) nDeltaTheta;
   if ((nDeltaPhi   <  6) || (nDeltaPhi   > 180)) return false;// Bereichsprüfung
   if ((nDeltaTheta <  6) || (nDeltaTheta >  90)) return false;
   if ((nDeltaPhi   < 10) && (nDeltaTheta <  10)) return false;

   m_nPhi   = (360 / nDeltaPhi  ) + 1;
   m_nTheta = (180 / nDeltaTheta);

   m_nRadiusValues  = m_nPhi * (m_nTheta-1) + 2;
   if (m_nRadiusValues > 1200) return false;
   m_pfRadiusValues = new float[m_nRadiusValues];             // Radiuswerte
   CLabel::ArchiveRead(ar, m_pfRadiusValues, m_nRadiusValues*sizeof(float), true);

   m_nFilmPictures = 1;
   m_nFlags       |= PHI_VALUES_COMPLETE;
   return CalculateAngleVectors();
}

bool CPolarGraph::CalculateAngleVectors()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::CalculateAngleVectors");
#endif
/*
      Cale Koordinatensystem
      xi[i] = cos(theta_rad) * cos(phi_rad) * radius( ri[(n + 1) * anzphi + m] );
      yi[i] = cos(theta_rad) * sin(phi_rad) * radius( ri[(n + 1) * anzphi + m] );
      zi[i] = sin(theta_rad) * radius( ri[(n + 1) * anzphi + m] );
*/
   ASSERT(m_pvAngleVectors == NULL);
   if (m_nRadiusValues)
   {
      m_pvAngleVectors = new CVector[m_nRadiusValues];
   }
   if (m_pvAngleVectors)
   {
      int    i, j,  nAngle = 0;
      double dTheta = 90, dPhi = 0;
      double dCosTheta, dSinTheta;
      double *pdSinPhi = new double[m_nPhi];
      double *pdCosPhi = new double[m_nPhi];
      for (i=0, dPhi=0; i<m_nPhi; i++, dPhi+=m_fDeltaPhi)
      {
         pdCosPhi[i] = cos(dPhi*M_PI_D_180);
         pdSinPhi[i] = sin(dPhi*M_PI_D_180);
      }
      m_pvAngleVectors[nAngle++] = CVector(0, 1, 0);
      for (j=1, dTheta-=m_fDeltaTheta; (j<m_nTheta-1)/* && (dTheta>-90)*/; j++, dTheta-=m_fDeltaTheta)
      {
         dCosTheta = cos(dTheta*M_PI_D_180);
         dSinTheta = sin(dTheta*M_PI_D_180);
         for (i=0; i<m_nPhi; i++)
         {
            ASSERT(nAngle < m_nRadiusValues);
            m_pvAngleVectors[nAngle++] = CVector(dCosTheta*pdSinPhi[i], dSinTheta, dCosTheta*pdCosPhi[i]);
         }
      }
      dTheta = -90;
      dPhi   = 0;
      ASSERT(nAngle < m_nRadiusValues);
      m_pvAngleVectors[nAngle++] = CVector(0,-1, 0);
      delete[] pdCosPhi;
      delete[] pdSinPhi;
      return true;
   }
   return false;
}

void CPolarGraph::DrawObject()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::DrawObject");
#endif
   if (m_pvAngleVectors && m_pfRadiusValues)
   {
      int        i, j, nOffset, nLastOffset, nOffAngle, 
                 nDetail        = GetDetail(),
                 nFilmPicture   = GetPictureIndex(),
                 nMax           = m_nFilmPictures * m_nRadiusValues;
      bool       bInterpolate   = DoInterpolate(),
                 bLightOn       = Lighting(),
                 bSmoothShading = SmoothShading(),
                 bCullFace      = CullFace();

      CVector    vec[7], vNormal[7];
      ETS3DGL_Fan Fan = {0, NULL, NULL};

      if (bLightOn)                                            // Das Ojekt kann Beleuchtet sein
      {
         glEnable(GL_LIGHTING);
         glEnable(GL_NORMALIZE);                               // wegen Skalierung
      }
      else
      {
         glDisable(GL_LIGHTING);
         glDisable(GL_NORMALIZE);
      }
      if (bSmoothShading) glShadeModel(GL_SMOOTH);             // Shading für das Objekt kann smooth sein
      else                glShadeModel(GL_FLAT);
      if (bCullFace)      glEnable(GL_CULL_FACE);              // Objektflächen können einseitig sein
      else                glDisable(GL_CULL_FACE);

      STriangle Tr = {0.0, NO_TRIANGLE_DIVISION, bInterpolate, NULL, this};

      if (nDetail)
      {
         Tr.dStep   = ((double)m_fMaxDimension)/(double)(2 << nDetail);
         Tr.nDetail = TRIANGLE_DIVISION_BY_STEP;
      }

      if (Tr.pParam)                                           // sind die Farben vorhanden ?
      {
         if (bSmoothShading)                                   // Dreiecksfunktion zuweisen
         {
            if (bLightOn) Tr.pFn = SmoothLightRadiusTr;
            else          Tr.pFn = SmoothRadiusTr;
         }
         else
         {
            if (bLightOn) Tr.pFn = FlatLightRadiusTr;
            else          Tr.pFn = FlatRadiusTr;
         }
      }
      glBegin(GL_TRIANGLES);
      {
         nOffAngle = nOffset = nFilmPicture*m_nRadiusValues;
         ASSERT(nOffset < nMax);
         vNormal[0] = m_pvAngleVectors[nOffset-nOffAngle];
         vec[0]     = vNormal[0] * GetRadius(nOffset);
         nOffset++;
         ASSERT(nOffset < nMax);
         vNormal[1] = m_pvAngleVectors[nOffset-nOffAngle];
         vec[1]     = vNormal[1] * GetRadius(nOffset);
         for (j=0; j<m_nPhi-1; j++)
         {
            ASSERT((nOffset+j+1) < nMax);
            vNormal[2] = m_pvAngleVectors[nOffset+j+1-nOffAngle];
            vec[2]     = vNormal[2] * GetRadius(nOffset+j+1);
            CCara3DGL::Triangle(&Tr, &vec[0], &vec[1], &vec[2], &vNormal[0], &vNormal[1], &vNormal[2]);
            vNormal[1] = vNormal[2];
            vec[1]     = vec[2];
         }
         if (!(m_nFlags&PHI_VALUES_COMPLETE))
         {
            vNormal[2] = m_pvAngleVectors[nOffset-nOffAngle];
            vec[2]     = vNormal[2] * GetRadius(nOffset);
            CCara3DGL::Triangle(&Tr, &vec[0], &vec[1], &vec[2], &vNormal[0], &vNormal[1], &vNormal[2]);
         }

         for (i=1; i<m_nTheta-2; i++)
         {
            nLastOffset  = nOffset;
            nOffset     += m_nPhi;

            vNormal[4] = vNormal[0] = m_pvAngleVectors[nLastOffset-nOffAngle];
            vec[4]     = vec[0]     = vNormal[0] * GetRadius(nLastOffset);
            ASSERT(nOffset < nMax);
            vNormal[5] = vNormal[1] = m_pvAngleVectors[nOffset-nOffAngle];
            vec[5]     = vec[1]     = vNormal[1] * GetRadius(nOffset);

            for (j=0; j<m_nPhi-1; j++)
            {
               ASSERT((nOffset+j+1) < nMax);
               vNormal[2] = m_pvAngleVectors[nOffset+j+1-nOffAngle];
               vec[2]     = vNormal[2] * GetRadius(nOffset+j+1);
               vNormal[3] = m_pvAngleVectors[nLastOffset+j+1-nOffAngle];
               vec[3]     = vNormal[3] * GetRadius(nLastOffset+j+1);
               vNormal[6] = (vNormal[0] + vNormal[1] + vNormal[2] + vNormal[3]) * 0.25;
               vec[6]     = (vec[0]     + vec[1]     + vec[2]     + vec[3])     * 0.25;
               CCara3DGL::Triangle(&Tr, &vec[0], &vec[1], &vec[6], &vNormal[0], &vNormal[1], &vNormal[6]);
               CCara3DGL::Triangle(&Tr, &vec[1], &vec[2], &vec[6], &vNormal[1], &vNormal[2], &vNormal[6]);
               CCara3DGL::Triangle(&Tr, &vec[2], &vec[3], &vec[6], &vNormal[2], &vNormal[3], &vNormal[6]);
               CCara3DGL::Triangle(&Tr, &vec[3], &vec[0], &vec[6], &vNormal[3], &vNormal[0], &vNormal[6]);
               vNormal[0] = vNormal[3];
               vec[0]     = vec[3];
               vNormal[1] = vNormal[2];
               vec[1]     = vec[2];
            }
            
            if (!(m_nFlags&PHI_VALUES_COMPLETE))
            {
               vNormal[6] = (vNormal[2] + vNormal[3] + vNormal[4] + vNormal[5]) * 0.25;
               vec[6]     = (vec[2]     + vec[3]     + vec[4]     + vec[5])     * 0.25;
               CCara3DGL::Triangle(&Tr, &vec[3], &vec[2], &vec[6], &vNormal[3], &vNormal[2], &vNormal[6]);
               CCara3DGL::Triangle(&Tr, &vec[2], &vec[5], &vec[6], &vNormal[2], &vNormal[5], &vNormal[6]);
               CCara3DGL::Triangle(&Tr, &vec[5], &vec[4], &vec[6], &vNormal[5], &vNormal[4], &vNormal[6]);
               CCara3DGL::Triangle(&Tr, &vec[4], &vec[3], &vec[6], &vNormal[4], &vNormal[3], &vNormal[6]);
            }
/*
            for (j=0; j<m_nPhi-1; j++)
            {
               ASSERT((nOffset+j+1) < nMax);
               vNormal[2] = m_pvAngleVectors[nOffset+j+1-nOffAngle];
               vec[2]     = vNormal[2] * GetRadius(nOffset+j+1);
               vNormal[3] = m_pvAngleVectors[nLastOffset+j+1-nOffAngle];
               vec[3]     = vNormal[3] * GetRadius(nLastOffset+j+1);

               CCara3DGL::Triangle(&Tr, &vec[0], &vec[1], &vec[2], &vNormal[0], &vNormal[1], &vNormal[2]);
               CCara3DGL::Triangle(&Tr, &vec[0], &vec[2], &vec[3], &vNormal[0], &vNormal[2], &vNormal[3]);
               vNormal[0] = vNormal[3];
               vec[0]     = vec[3];
               vNormal[1] = vNormal[2];
               vec[1]     = vec[2];
            }
            if (!(m_nFlags&PHI_VALUES_COMPLETE))
            {
               CCara3DGL::Triangle(&Tr, &vec[3], &vec[2], &vec[5], &vNormal[0], &vNormal[2], &vNormal[5]);
               CCara3DGL::Triangle(&Tr, &vec[3], &vec[5], &vec[4], &vNormal[0], &vNormal[5], &vNormal[4]);
            }
*/
         }

         nLastOffset  = nOffset;
         nOffset     += m_nPhi;
         ASSERT(nOffset < nMax);
         vNormal[0] = m_pvAngleVectors[nOffset-nOffAngle];
         vec[0]     = vNormal[0] * GetRadius(nOffset);

         ASSERT(nLastOffset < nMax);
         vNormal[1] = m_pvAngleVectors[nLastOffset-nOffAngle];
         vec[1]     = vNormal[1] * GetRadius(nLastOffset);
         for (j=0; j<m_nPhi-1; j++)
         {
            ASSERT((nLastOffset+j+1) < nMax);
            vNormal[2] = m_pvAngleVectors[nLastOffset+j+1-nOffAngle];
            vec[2]     = vNormal[2] * GetRadius(nLastOffset+j+1);
            CCara3DGL::Triangle(&Tr, &vec[0], &vec[2], &vec[1], &vNormal[0], &vNormal[2], &vNormal[1]);
            vNormal[1] = vNormal[2];
            vec[1]     = vec[2];
         }
         if (!(m_nFlags&PHI_VALUES_COMPLETE))
         {
            vNormal[2] = m_pvAngleVectors[nLastOffset-nOffAngle];
            vec[2]     = vNormal[2] * GetRadius(nLastOffset);
            CCara3DGL::Triangle(&Tr, &vec[0], &vec[2], &vec[1], &vNormal[0], &vNormal[2], &vNormal[1]);
         }
      }
      glEnd();
   }
}

double CPolarGraph::GetRadius(int nIndex)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::GetRadius");
#endif
   double dRadius = m_dOffset + (double)m_pfRadiusValues[nIndex]*m_dNormFactor;
   if      (dRadius < 0)
   {
      dRadius    = 0;
      m_bClipped = true;
   }
   else if (dRadius > (double)m_fMaxDimension)
   {
      dRadius    = m_fMaxDimension;
      m_bClipped = true;
   }
   return dRadius;
}

void CPolarGraph::CheckMinMax()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::CheckMinMax");
#endif
   if (m_pfRadiusValues)
   {
      int        i, nValues = m_nRadiusValues * m_nFilmPictures;
      float      fMax, fMin, fRange;
      ETSDIV_DIV div;
      ZeroMemory(&div, sizeof(ETSDIV_DIV));

      fMin = fMax = m_pfRadiusValues[0];
      if (m_dMinValue == m_dMaxValue)                        // nicht initialisiert
      {
         m_dMinValue = m_dMaxValue = fMax;                   // initialisieren
      }
      
      for (i=1; i<nValues; i++)                              // Min/Max-Werte suchen
      {
         if (m_pfRadiusValues[i]>fMax) fMax = m_pfRadiusValues[i];
         if (m_pfRadiusValues[i]<fMin) fMin = m_pfRadiusValues[i];
      }

      if (m_dMinValue > fMin) m_dMinValue = fMin;            // Bereich checken
      if (m_dMaxValue < fMax) m_dMaxValue = fMax;

      div.f1        = (double)fMin;
      div.f2        = (double)fMax;
      div.stepwidth = 10;
      div.divfnc    = ETSDIV_LINEAR;
      if (CEtsDiv::RoundMinMaxValues(&div))
      {
         fMin = (float)div.f1;
         fMax = (float)div.f2;
      }

      fRange = fMax - fMin;                                    // gesamten Radius ermitteln

      m_ObjectVolume.Left   = -fRange;
      m_ObjectVolume.Right  =  fRange;
      m_ObjectVolume.Bottom = -fRange;
      m_ObjectVolume.Top    =  fRange;
      m_ObjectVolume.Near   = -fRange;
      m_ObjectVolume.Far    =  fRange;
      m_fMaxDimension       =  fRange * 2;

      m_dOffset = -m_dMinValue;
      SetNormFactor(1.0);
      CBaseClass3D::CheckMinMax();
   }
}

const char *CPolarGraph::GetLevelUnit()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::GetLevelUnit");
#endif
   if (m_pszUnitOfRadius) return m_pszUnitOfRadius;
   return SPACE_STRING;
}

char *CPolarGraph::GetUnit()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::GetUnit");
#endif
   if (m_pszUnitOfRadius) return m_pszUnitOfRadius;
   return SPACE_STRING;
}

bool CPolarGraph::SetMinMax(double dMin, double dMax)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::SetMinMax");
#endif
   if (dMin < dMax)
   {
      double dNorm = (dMax - dMin) / m_ObjectVolume.Right;
      SetNormFactor(dNorm);
      m_dOffset = m_ObjectVolume.Right - dNorm * dMax;
      return false;
   }
   return false;
}

void CPolarGraph::ReadPolarData(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::ReadPolarData");
#endif
   CLabel::ArchiveRead(ar, &m_nFilmPictures, sizeof(long) , true);
   CLabel::ArchiveRead(ar, &m_fDeltaTheta  , sizeof(float), true);
   CLabel::ArchiveRead(ar, &m_fDeltaPhi    , sizeof(float), true);
   CLabel::ArchiveRead(ar, &m_nTheta       , sizeof(long) , true);
   CLabel::ArchiveRead(ar, &m_nPhi         , sizeof(long) , true);
   m_pszUnitOfRadius = CLabel::ReadVariableString(ar, true);
   CLabel::ArchiveRead(ar, &m_nFlags       , sizeof(long) , true);
 
   if ((m_nTheta == 0) && (m_nPhi == 0))
   {
      if ((m_fDeltaTheta <= 0.0) || (m_fDeltaPhi <= 0.0))
         AfxThrowArchiveException(CArchiveException::generic, NULL);

      m_nPhi   = (int)(360.0 / (double)m_fDeltaPhi);
      if (m_nFlags & PHI_VALUES_COMPLETE) m_nPhi++;
      m_nTheta = (int)(180.0 / (double)m_fDeltaTheta);
   }
   else if ((m_fDeltaTheta == 0.0) || (m_fDeltaPhi == 0.0))
   {
      if ((m_nTheta <= 0) || (m_nPhi <= 0))
         AfxThrowArchiveException(CArchiveException::generic, NULL);
      m_fDeltaTheta = (float)(180 / (m_nTheta-1));
      if (m_nFlags & PHI_VALUES_COMPLETE)
         m_fDeltaPhi   = (float)(360 / (m_nPhi-1));
      else
         m_fDeltaPhi   = (float)(360 / m_nPhi);
   }

   m_nRadiusValues   = m_nPhi * (m_nTheta-2) + 2;
   if (m_nFlags&SPECIFICVALUES_AVAILABLE)
   {
      ASSERT(m_pdSpecificUnit == NULL);
      m_pdSpecificUnit = new double[m_nFilmPictures];
      if (m_pdSpecificUnit)
      {
         CLabel::ArchiveRead(ar, m_pdSpecificUnit, sizeof(double)*m_nFilmPictures, true);
      }
      m_pszSpecificUnit = CLabel::ReadVariableString(ar, true);
   }

   if (PhaseValuesAvailable())
   {
      long nSize;
      m_pszPhaseUnit = CLabel::ReadVariableString(ar, true);

      if      (m_nFlags & NO_VALUE_NORMALIZATION)   nSize = 0;
      else if (m_nFlags & ONE_NORMALIZATION_FACTOR) nSize = 1;
      else if (m_nFlags & N_NORMALIZATION_FACTORS)  nSize = m_nFilmPictures;
      if (nSize)
      {
         m_pdNormfactor = new double[nSize];
         if (m_pdNormfactor)
         {
            CLabel::ArchiveRead(ar, m_pdNormfactor, sizeof(double)*nSize, true);
         }
      }
      if (PhaseValuesCalculated())
      {
         m_nFlags &= ~PHASEVALUES_CALCULATED;
         m_nFlags |= CALCULATE_PHASEVALUES;
      }
   }
   CalculateAngleVectors();
}

void CPolarGraph::ReadLevelData(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::ReadLevelData");
#endif
   if ((m_nFilmPictures > 0) && (m_nRadiusValues > 0))
   {
      long nSize = m_nFilmPictures * m_nRadiusValues * sizeof(float);
      if (PhaseValuesAvailable()) nSize *= 2;
      m_hFileMapping = ::CreateFileMapping((HANDLE) 0xFFFFFFFF,
                       NULL, PAGE_READWRITE, 0, nSize, NULL);
      if(m_hFileMapping != INVALID_HANDLE_VALUE)
      {
         float * pfZ_Values = (float*) ::MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE, 0, 0, 0);
         if (pfZ_Values)
         {
            CLabel::ArchiveRead(ar, pfZ_Values, nSize, true);
            ::UnmapViewOfFile(pfZ_Values);
         }                  
         m_pfRadiusValues = (float*) ::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
         ASSERT(m_pfRadiusValues != NULL);
         m_ObjectVolume.Left = m_ObjectVolume.Right = 0.0;
      }
   }
}

bool CPolarGraph::MapLevelDataFile(char *pszFileName)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::MapLevelDataFile");
#endif
   if (pszFileName)
   {                                                           // Filemapping erzeugen
      m_hFileForMapping  = ::CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
      if(m_hFileForMapping != INVALID_HANDLE_VALUE)
      {
         if((m_hFileMapping = ::CreateFileMapping(m_hFileForMapping, NULL, PAGE_READONLY|SEC_COMMIT, 0, 0, NULL)) != NULL )
         {
            m_pfRadiusValues = (float*) ::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
            ASSERT(m_pfRadiusValues != NULL);
            m_ObjectVolume.Left = m_ObjectVolume.Right = 0.0;
            return true;
         }
      }
   }
   return false;
}

double CPolarGraph::GetSpecificValue(int nIndex)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::GetSpecificValue");
#endif
   if (PhaseValuesCalculated())
   {
      return nIndex * 360.0 / NO_OF_PHASEVALUES;
   }
   else
   {
      if (m_pdSpecificUnit && (nIndex >= 0) && (nIndex < m_nFilmPictures)) return m_pdSpecificUnit[nIndex];
   }
   return (double) nIndex;
}

const char* CPolarGraph::GetSpecificUnit()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPolarGraph::GetSpecificUnit");
#endif
   if (PhaseValuesCalculated())
   {
      return PHASE_SPECIFIC_UNIT;
   }
   else
   {
      if (m_pszSpecificUnit) return m_pszSpecificUnit;
   }
   return SPACE_STRING;
}
