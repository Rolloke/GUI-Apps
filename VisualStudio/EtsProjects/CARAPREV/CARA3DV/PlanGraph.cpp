// PlanGraph.cpp: Implementierung der Klasse CPlanGraph.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlanGraph.h"
#include "CARA3DVDoc.h"
#include "Cara3DGL.h"
#include "LevelColor.h"
#include "CEtsDiv.h" 

#include "..\CARAPrev.h" 

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*
#ifdef ASSERT_GL_ERROR        // 
   #define ASSERT_GL_ERROR
#endif
*/
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
#define PLANGRAPH_UNIT           "m"            // Einheit des Grundrisses ist Meter
#define LIGHT_COLOR_FACTOR       0.7f           // Korrekturfaktor für die Beleuchtung

char * NewString(const CString &s)
{
   int n = s.GetLength();
   char * psz = new char[n+1];
   strcpy(psz, LPCTSTR(s));
   return psz;
}

CPlanGraph::CPlanGraph()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph()");
#endif
   Init();
   m_nClass3DType = PLAN_GRAPH;
}

void CPlanGraph::Init()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::Init");
#endif
   m_nPlaneKoord        = 0;
   m_nBorderIndexes     = 0;
   m_nFloorTriangles    = 0;
   m_nWebTriangles      = 0;

   m_pdSpecificUnit     = NULL;
   m_pdNormfactor       = NULL;
   m_pPlaneKoord        = NULL;
   m_pFloorTriangles    = NULL;
   m_pnBorderIndex      = NULL;
   m_pWebTriangles      = NULL;
   m_pfLevels           = NULL;
   m_pnZeroLevelPoints  = NULL;
   m_pfPhaseValues      = NULL;

   m_pszLevelUnit       = NULL;
   m_pszSpecificUnit    = NULL;
   m_pszPhaseUnit       = NULL;

   m_hFileForMapping    = INVALID_HANDLE_VALUE;
   m_hFileMapping       = INVALID_HANDLE_VALUE;

   m_ObjectVolume.Bottom= 0.0f;
   m_ObjectVolume.Top   = 0.0f;
   m_ObjectVolume.Left  = 0.0f;
   m_ObjectVolume.Right = 0.0f;
   m_ObjectVolume.Near  = 0.0f;
   m_ObjectVolume.Far   = 0.0f;
   m_fMaxDimension      = 0.0f;
   m_fFloorLevel        = 0.0f;

   m_nRoundingLevel     = 1;
   m_nNumModeLevel      = ETSDIV_NM_SCI;
   m_dGridStepLevel     = 1;

   m_pfSpecular[0]          = 0.2f;
   m_pfSpecular[1]          = 0.2f;
   m_pfSpecular[2]          = 0.2f;
   m_pfSpecular[3]          = 1.0f;
   m_pfAmbientAndDiffuse[0] = 0.0f;
   m_pfAmbientAndDiffuse[1] = 0.0f;
   m_pfAmbientAndDiffuse[2] = 0.8f;
   m_pfAmbientAndDiffuse[3] = 1.0f;
   m_pfColor[0]             = 0.0f;
   m_pfColor[1]             = 0.0f;
   m_pfColor[2]             = 1.0f;
   m_pfColor[3]             = 1.0f;

}

CPlanGraph::~CPlanGraph()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("~CPlanGraph()");
#endif
   DeletePhaseValues();
   DeleteData();
}

void CPlanGraph::DeleteData()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::DeleteData");
#endif
   if (m_pdSpecificUnit) delete[] m_pdSpecificUnit;
   if (m_pdNormfactor)   delete[] m_pdNormfactor;
   if (m_pPlaneKoord)    delete[] m_pPlaneKoord;
   if (m_pFloorTriangles)
   {
      if (m_pFloorTriangles == m_pWebTriangles)
         m_pWebTriangles = NULL;
      delete[] m_pFloorTriangles;
   }
   if (m_pWebTriangles)     delete[] m_pWebTriangles;
   if (m_pnBorderIndex)     delete[] m_pnBorderIndex;
   if (m_pszSpecificUnit)   delete[] m_pszSpecificUnit;
   if (m_pszPhaseUnit)      delete[] m_pszPhaseUnit;
   if (m_pszLevelUnit)      delete[] m_pszLevelUnit;
   if (m_pnZeroLevelPoints) delete[] m_pnZeroLevelPoints;
   if (m_pfPhaseValues)     delete[] m_pfPhaseValues;

   if (m_hFileMapping != INVALID_HANDLE_VALUE)
   {                                                        // bei MemoryMappedFile
      if (m_pfLevels)                                       // Zeiger unmappen
      {
         ::UnmapViewOfFile((void*)m_pfLevels);
      }
      ::CloseHandle(m_hFileMapping);                        // File-Mapping schließen
   }
   else                                                     // durch new alloziert
   {
      if (m_pfLevels) delete[] m_pfLevels;                  // Zeiger löschen
   }

   if (m_hFileForMapping != INVALID_HANDLE_VALUE)
   {
      ::CloseHandle(m_hFileForMapping);
   }
   Init();                                                     // Variablen initialisieren
}

bool CPlanGraph::ReadNetData(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::ReadNetData");
#endif
   CLabel::ArchiveRead(ar, &m_nPlaneKoord, sizeof(long), true);// Anzahl der Punkte in der Ebene
   if ((m_nPlaneKoord <= 0) || (m_nPlaneKoord > 50000))       return false;
   CLabel::ArchiveRead(ar, &m_nBorderIndexes, sizeof(long), true);// Anzahl der Randpunkte
   if ((m_nBorderIndexes < 0) || (m_nBorderIndexes > 1000))   return false;
   CLabel::ArchiveRead(ar, &m_nFloorTriangles, sizeof(long), true);// Anzahl der Bodendreiecke
   if ((m_nFloorTriangles < 0) || (m_nFloorTriangles > 1000)) return false;
   CLabel::ArchiveRead(ar, &m_nWebTriangles, sizeof(long), true);// Anzahl der Netzdreiecke
   if ((m_nWebTriangles <= 0) || (m_nWebTriangles >  50000))  return false;

   double  Dummy;
   CLabel::ArchiveRead(ar, &Dummy, sizeof(double), true);            // Dummy: Hier steht die Höhe der folgenden Koordinaten
        
   m_pPlaneKoord = new Vector2Df[m_nPlaneKoord];
   if (!m_pPlaneKoord)  return false;
   CLabel::ArchiveRead(ar, m_pPlaneKoord, sizeof(Vector2Df)*m_nPlaneKoord, true);

   if (m_nFloorTriangles > 0)
   {
      m_pFloorTriangles = new Vectorl3[m_nFloorTriangles];
      if (!m_pFloorTriangles) return false;
      CLabel::ArchiveRead(ar, m_pFloorTriangles, sizeof(Vectorl3)*m_nFloorTriangles, true);
   }
   if (m_nBorderIndexes > 0)
   {
      m_pnBorderIndex = new long[m_nBorderIndexes];
      if (!m_pnBorderIndex) return false;
      CLabel::ArchiveRead(ar, m_pnBorderIndex, sizeof(long)*m_nBorderIndexes, true);
   }
   m_pWebTriangles   = new Vectorl3[m_nWebTriangles];
   if (!m_pWebTriangles) return false;
   CLabel::ArchiveRead(ar, m_pWebTriangles, sizeof(Vectorl3)*m_nWebTriangles, true);

   m_pnZeroLevelPoints = new long[m_nPlaneKoord];
   if (!m_pnZeroLevelPoints) return false;
   CLabel::ArchiveRead(ar, m_pnZeroLevelPoints, sizeof(long)*m_nPlaneKoord, true);

   m_ObjectVolume.Left = m_ObjectVolume.Right = 0.0;

   return true;
}

void CPlanGraph::ReadLevelData(CArchive &ar, long nPictures)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::ReadLevelData");
#endif
   if (nPictures == -1)                                        // Version >= 1.10 !
   {
      CLabel::ArchiveRead(ar, &m_nFilmPictures, sizeof(long), true);
      CLabel::ArchiveRead(ar, &m_nFlags   , sizeof(long), true);
   }
   else  m_nFilmPictures = nPictures;                              // Version 1.00 !
   if ((m_nFilmPictures > 0) && (m_nPlaneKoord > 0))
   {
      long nSize = m_nFilmPictures * m_nPlaneKoord * sizeof(float);
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
         m_pfLevels = (float*) ::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
         ASSERT(m_pfLevels != NULL);
         m_ObjectVolume.Left = m_ObjectVolume.Right = 0.0;
      }
   }
}

bool CPlanGraph::ReadTextFile(CArchive&ar)
{
   int nX=0, nY=0, nZ, nT, iX, iY, iZ, n, nP=0, nRow;
   int nFx = 1, nStartX = 0;
   int nFy = 1, nStartY = 0;
   float  dZLimit;
   bool   bZLimited = false;
   DWORD nType = -1; // can read also DOS
   CString sFind[] = 
   {
      "Dimension:",  // 0
      "Unit:",       // 1
      "Film:",       // 2
      "Factors:",    // 3
      "ZLimit:",     // 4
      "Start:"       // 5
   };
   CArchiveEx*pAr = (CArchiveEx*)&ar;
   CString s;
   iX = 0;
   
   while (iX != -1)
   {
      pAr->ReadString(nType, s);
      for (iX=0; iX<6; iX++)
      {
         n = s.Find(sFind[iX]);
         if (n != -1)
         {
            s = s.Mid(n+sFind[iX].GetLength());
            break;
         }
      }
      switch (iX)
      {
         case 0:  // Dimension
            n = sscanf(s, "%d, %d, %d", &nX, &nY, &nP);
            if (n == 2)
            {
               m_nFilmPictures = 1;
            }
            else if (n == 3)
            {
               m_nFilmPictures = nP;
            }
            else
            {
               return false;
            }
            break;
         case 1:  // Unit
            m_pszLevelUnit = NewString(s);
            break;
         case 2:  // Film
            m_pszSpecificUnit = NewString(s);
            break;
         case 3:  // Factors
            n = sscanf(s, "%d, %d", &nFx, &nFy);
            if (n != 2)
            {
               nFx = 1;
               nFy = 1;
            }
            break;
         case 4:  // Z Limit
            n = sscanf(s, "%f", &dZLimit);
            if (n == 1) bZLimited = true;
            break;
         case 5:  // Start of reading
            n = sscanf(s, "%d, %d", &nStartX, &nStartY);
            if (n != 2)
            {
               nStartX = 0;
               nStartY = 0;
            }
            break;
         default:
            iX = -1;
            break;
      }
   }

   nZ = nX * nY;           // Anzahl der Punkte
   m_nPlaneKoord = nZ;
   nZ *= m_nFilmPictures;
   m_pfLevels = new float[nZ];

   for (n=0; n<nStartY; n++)
   {
      if (pAr->ReadString(nType, s) == EOF)
      {
         return false;
      }
   }
   n = 0;
   nRow = 0;
   const char *psz;
   for (iZ=0; iZ<nZ; iZ++, nRow++)
   {
      psz = &(LPCTSTR(s))[n]; 
      m_pfLevels[iZ] = (float)atof(psz);
      //m_pfLevels[iZ] = (float)strtod(psz, NULL);
      if (bZLimited)
      {
         if (m_pfLevels[iZ] > dZLimit)
         {
            m_pfLevels[iZ] = dZLimit;
         }
      }
      n = s.Find(" ", n);
      if (n != -1 && nRow < nX)
      {
         n++;
         continue;
      }
      if (pAr->ReadString(nType, s) == EOF && iZ < (nZ-1))
      {
         return false;
      }
      n = 0;
      nRow = 0;
   }

   m_pPlaneKoord = new Vector2Df[m_nPlaneKoord];
   for (iY=0, iZ=0; iY<nY; iY++)
   {
      for (iX=0; iX<nX; iX++)
      {
         m_pPlaneKoord[iZ].x = (float)(nFx*iX);
         m_pPlaneKoord[iZ].y = (float)(nFy*iY);
         iZ++;
      }
   }
   
   nT = (nX-1)*(nY-1)*2;   // Anzahl der Dreiecke
   m_nWebTriangles = nT;
   m_pWebTriangles = new Vectorl3[nT];

   iX = 0;
   iY = 0;
   for (n=0; n<nT; n++)
   {
      if (n&1)
      {
         m_pWebTriangles[n].p[2] = (iX+1) +  iY   *nX;
         m_pWebTriangles[n].p[1] = (iX+1) + (iY+1)*nX;
         m_pWebTriangles[n].p[0] =  iX    + (iY+1)*nX;
         iX++;
      }
      else
      {
         m_pWebTriangles[n].p[2] =  iX +     iY   *nX;
         m_pWebTriangles[n].p[1] = (iX+1) +  iY   *nX;
         m_pWebTriangles[n].p[0] =  iX    + (iY+1)*nX;
      }
      if (iX == nX-1)
      {
         iY++;
         iX = 0;
      }
   }
   return true;
}

bool CPlanGraph::MapLevelDataFile(char *pszFileName, long nPictures)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::MapLevelDataFile");
#endif
   ASSERT(nPictures!=0);
   if (pszFileName)
   {                                                           // Filemapping erzeugen
      m_hFileForMapping  = ::CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
      if(m_hFileForMapping != INVALID_HANDLE_VALUE)
      {
         if((m_hFileMapping = ::CreateFileMapping(m_hFileForMapping, NULL, PAGE_READONLY|SEC_COMMIT, 0, 0, NULL)) != NULL )
         {
            m_pfLevels = (float*) ::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
            ASSERT(m_pfLevels != NULL);
            m_ObjectVolume.Left = m_ObjectVolume.Right = 0.0;
            m_nFilmPictures = nPictures;
            return true;
         }
      }
   }
   return false;
}

void CPlanGraph::ReadUnits(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::ReadUnits");
#endif
   m_pszLevelUnit    = CLabel::ReadVariableString(ar, true);
   m_pszSpecificUnit = CLabel::ReadVariableString(ar, true);
}

int CPlanGraph::ReadSpecificUnit(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::ReadSpecificUnit");
#endif
   long nSpecUnits;
   CLabel::ArchiveRead(ar, &nSpecUnits, sizeof(long), true);   // Anzahl der Spezifischen Einheit für die Filmbilder lesen
   ASSERT(m_pdSpecificUnit == NULL);
   if (nSpecUnits > 0) m_pdSpecificUnit = new double[nSpecUnits];
   if (m_pdSpecificUnit)
   {
      CLabel::ArchiveRead(ar, m_pdSpecificUnit, sizeof(double)*nSpecUnits, true);
      return nSpecUnits;
   }
   return 0;
}

void CPlanGraph::Read_110_Data(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::Read_110_Data");
#endif
   CLabel::ArchiveRead(ar, &m_nFlags, sizeof(long), true);
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
}

void CPlanGraph::ReadObjectData(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::ReadObjectData");
#endif
   ReadNetData(     ar);         // Netzdaten
   ReadLevelData(   ar);         // Level
   ReadUnits(       ar);         // Einheiten
	ReadSpecificUnit(ar);         // spezifische Werte für die Einheit
   Read_110_Data(   ar);         // Version 110 spezifisch
   // Farben
   CLabel::ArchiveRead(ar, m_pfColor            , sizeof(float) * 4, true);// Farbe des Objektes
   CLabel::ArchiveRead(ar, m_pfAmbientAndDiffuse, sizeof(float) * 4, true);// Lichtfarbe des Objektes
   CLabel::ArchiveRead(ar, m_pfSpecular         , sizeof(float) * 4, true);// Reflektionsfarbe des Objektes
}

void CPlanGraph::WriteObjectData(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::WriteObjectData");
#endif
   // Netzdaten
   double Dummy = 0.0;
   CLabel::ArchiveWrite(ar, &m_nPlaneKoord    , sizeof(long));  // Anzahl der Punkte in der Ebene
   CLabel::ArchiveWrite(ar, &m_nBorderIndexes , sizeof(long));  // Anzahl der Randpunkte
   CLabel::ArchiveWrite(ar, &m_nFloorTriangles, sizeof(long));  // Anzahl der Bodendreiecke
   CLabel::ArchiveWrite(ar, &m_nWebTriangles  , sizeof(long));  // Anzahl der Netzdreiecke
   CLabel::ArchiveWrite(ar, &Dummy            , sizeof(double));// Dummy: Hier steht die Höhe der folgenden Koordinaten
   if (m_pPlaneKoord)       CLabel::ArchiveWrite(ar, m_pPlaneKoord, sizeof(Vector2Df)*m_nPlaneKoord);
   if (m_pFloorTriangles)   CLabel::ArchiveWrite(ar, m_pFloorTriangles, sizeof(Vectorl3)*m_nFloorTriangles);
   if (m_pnBorderIndex)     CLabel::ArchiveWrite(ar, m_pnBorderIndex, sizeof(long)*m_nBorderIndexes);
   if (m_pWebTriangles)     CLabel::ArchiveWrite(ar, m_pWebTriangles, sizeof(Vectorl3)*m_nWebTriangles);
   if (m_pnZeroLevelPoints) CLabel::ArchiveWrite(ar, m_pnZeroLevelPoints, sizeof(long)*m_nPlaneKoord);
   // Level
   CLabel::ArchiveWrite(ar, &m_nFilmPictures, sizeof(long));
   CLabel::ArchiveWrite(ar, &m_nFlags   , sizeof(long));
   long nSize = m_nFilmPictures * m_nPlaneKoord * sizeof(float);
   if (PhaseValuesAvailable()) nSize *= 2;           // Phasenwerte stehen hinter den Levelwerten
   if (m_pfLevels) CLabel::ArchiveWrite(ar, m_pfLevels, nSize);
   
   // Units
   CLabel::WriteVariableString(ar, m_pszLevelUnit);
   CLabel::WriteVariableString(ar, m_pszSpecificUnit);

   // SpecificUnits
   CLabel::ArchiveWrite(ar, &m_nFilmPictures, sizeof(long));       // Anzahl der Spezifischen Einheit für die Filmbilder lesen
   if (m_pdSpecificUnit) CLabel::ArchiveWrite(ar, m_pdSpecificUnit, sizeof(double)*m_nFilmPictures);

   // 110-Data
   CLabel::ArchiveWrite(ar, &m_nFlags, sizeof(int));
   if (PhaseValuesAvailable())
   {
      CLabel::WriteVariableString(ar, m_pszPhaseUnit);

      if      (m_nFlags & NO_VALUE_NORMALIZATION)   nSize = 0;
      else if (m_nFlags & ONE_NORMALIZATION_FACTOR) nSize = 1;
      else if (m_nFlags & N_NORMALIZATION_FACTORS)  nSize = m_nFilmPictures;
      if (nSize) CLabel::ArchiveWrite(ar, m_pdNormfactor, sizeof(double)*nSize);
   }

   // Colors
   CLabel::ArchiveWrite(ar, m_pfColor            , sizeof(float) * 4);// Farbe des Objektes
   CLabel::ArchiveWrite(ar, m_pfAmbientAndDiffuse, sizeof(float) * 4);// Lichtfarbe des Objektes
   CLabel::ArchiveWrite(ar, m_pfSpecular         , sizeof(float) * 4);// Reflektionsfarbe des Objektes
}

void CPlanGraph::DrawObject()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::DrawObject");
#endif
   if (m_nPlaneKoord && m_pPlaneKoord && m_pWebTriangles && m_pfLevels)
   {
      int        i, j, nIndex[3], nSeite, 
                 nDetail      = GetDetail(),
                 nPlaneKoord  = m_nPlaneKoord,                      // kopieren in lokale Variablen
                 nBorderIndex = m_nBorderIndexes,                   // für Performance
                 nWebTr       = m_nWebTriangles,
                 nFloorTr     = m_nFloorTriangles,
                 nFilmPicture = GetPictureIndex();
      Vector2Df *pPlaneKoord  = m_pPlaneKoord;
      Vectorl3  *pFloorTriang = m_pFloorTriangles;
      long      *pnBorderInd  = m_pnBorderIndex;
      Vectorl3  *pWebTriangle = m_pWebTriangles;
      CVector    vec[3], vNorm, *pNormal = NULL;
      double     dZNorm       = m_dNormFactor,
                 dFloorLevel  = (double) m_fFloorLevel,
                 dStep        = (double) m_fMaxDimension;
      bool       bInterpolate   = DoInterpolate(),
                 bLightOn       = Lighting(),
                 bSmoothShading = SmoothShading(),
                 bCullFace      = CullFace(),
                 bNormalBug     = NormalBug();
      ETS3DGL_Fan Fan = {0, NULL, NULL};

      if (nFilmPicture <            0) nFilmPicture = 0;
      float     *pfLevel = NULL;
      if (PhaseValuesCalculated())
      {
         if (m_pfPhaseValues == NULL) return;
         if (nFilmPicture >= NO_OF_PHASEVALUES) nFilmPicture = NO_OF_PHASEVALUES-1;
         pfLevel = &m_pfPhaseValues[nFilmPicture*nPlaneKoord];
      }
      else
      {
         if (nFilmPicture >= m_nFilmPictures) nFilmPicture = m_nFilmPictures-1;
         pfLevel = &m_pfLevels[nFilmPicture*nPlaneKoord];
      }

      if (nDetail) dStep = ((double)m_fMaxDimension)/(double)(1 << nDetail);
      else         bInterpolate = false;                       // keine Aufteilung = keine Interpolation nötig

      if (bSmoothShading) glShadeModel(GL_SMOOTH);             // Shading für das Objekt kann smooth sein
      else                glShadeModel(GL_FLAT);
      if (bCullFace)      glEnable(GL_CULL_FACE);              // Objektflächen können einseitig sein
      else                glDisable(GL_CULL_FACE);

      glColor4fv(m_pfColor);

      if (bLightOn)
      {
         glEnable(GL_LIGHTING);
         glEnable(GL_NORMALIZE);                               // wegen Skalierung
         glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, m_pfAmbientAndDiffuse);
         glMaterialfv(GL_FRONT, GL_SPECULAR,            m_pfSpecular);

         Fan.dStep  = dStep;
         Fan.pFn    = CCara3DGL::LightedFans;
         if (bNormalBug) Fan.pParam = &vNorm;
         else            Fan.pParam = NULL;
      }
      else
      {
         glDisable(GL_LIGHTING);
         glDisable(GL_NORMALIZE);
      }
///////////////////////////////////////////////////////////////////////////////
      if (pFloorTriang)                                        // Bodendreiecke
      {
         for (i=0; i<nFloorTr; i++)
         {
            for (j=0; j<3; j++)
            {
               vec[j] = CVector(pPlaneKoord[pFloorTriang[i].p[j]].y, dFloorLevel, pPlaneKoord[pFloorTriang[i].p[j]].x);
            }

            if (bLightOn)
            {
               vNorm = Norm(Product(vec[1]-vec[0], vec[2]-vec[0]));
               if (!bNormalBug) glNormal3dv((double*)&vNorm);
            }

            CEts3DGL::DrawTriangleFan(&vec[0], &vec[1], &vec[2], &Fan);
         }
      }
///////////////////////////////////////////////////////////////////////////////
      if (pnBorderInd)
      {
         for (i=0; i<nBorderIndex; i++)                        // Seitendreiecke
         {
            j = i+1;
            if (j>=nBorderIndex) j=0;
            vec[0] = CVector(pPlaneKoord[pnBorderInd[i]].y, pfLevel[pnBorderInd[i]]*dZNorm, pPlaneKoord[pnBorderInd[i]].x);
            vec[1] = CVector(pPlaneKoord[pnBorderInd[j]].y, pfLevel[pnBorderInd[j]]*dZNorm, pPlaneKoord[pnBorderInd[j]].x);
            vec[2] = CVector(pPlaneKoord[pnBorderInd[j]].y, dFloorLevel                   , pPlaneKoord[pnBorderInd[j]].x);

            if (bLightOn)
            {
               vNorm = Norm(Product(vec[1]-vec[0], vec[2]-vec[0]));
               if (!bNormalBug) glNormal3dv((double*)&vNorm);
            }

            CEts3DGL::DrawTriangleFan(&vec[0], &vec[1], &vec[2], &Fan);
            vec[1] = CVector(pPlaneKoord[pnBorderInd[i]].y, dFloorLevel, pPlaneKoord[pnBorderInd[i]].x);
            CEts3DGL::DrawTriangleFan(&vec[2], &vec[1], &vec[0], &Fan);
         }
      }
///////////////////////////////////////////////////////////////////////////////
      STriangle Tr = {0.0, NO_TRIANGLE_DIVISION, bInterpolate, NULL, this};
      if (nDetail)
      {
         Tr.dStep   = ((double)m_fMaxDimension)/(double)(1 << nDetail);
         Tr.nDetail = TRIANGLE_DIVISION_BY_STEP;
      }
      if (Tr.pParam)                                           // sind die Farben vorhanden ?
      {
         if (bSmoothShading)                                   // Dreiecksfunktion zuweisen
         {
            if (bLightOn) Tr.pFn = SmoothLightLevelTr;
            else          Tr.pFn = SmoothLevelTr;
         }
         else
         {
            if (bLightOn) Tr.pFn = FlatLightLevelTr;
            else          Tr.pFn = FlatLevelTr;
         }
      }
      else nWebTr = 0;

      if (bLightOn || bInterpolate)                            // Normalenvektoren mitteln
      {
         pNormal = new CVector[nPlaneKoord];
         memset(pNormal, 0, sizeof(CVector)*nPlaneKoord);      // Initialisieren mit 0
         for (i=0; i<nWebTr; i++)                              // für alle Dreiecke
         {
            for (j=0;j<3;j++)
            {
               nIndex[j] = pWebTriangle[i].p[j];
               vec[j] = CVector(pPlaneKoord[nIndex[j]].y, pfLevel[nIndex[j]]*dZNorm, pPlaneKoord[nIndex[j]].x);
            }
            vNorm = Product(vec[1]-vec[2], vec[0]-vec[2]);     // Normalenvektor berechnen
            if (Vy(vNorm) < 0.0)
            {
               TRACE("Normalenvectoren tauschen");
               vNorm = -vNorm;
               CVector vTemp = vec[1];
                               vec[1] = vec[2];
                                        vec[2] = vTemp;
            }
            for (j=0;j<3;j++) 
               pNormal[nIndex[j]] += vNorm;                    // Normalenvektoren addieren
         }

         for (i=0; i<nPlaneKoord; i++) 
            pNormal[i] = Norm(pNormal[i]);                     // Normieren
      }
///////////////////////////////////////////////////////////////////////////////
      glBegin(GL_TRIANGLES);
      {
         for (i=0; i<nWebTr; i++)                              // Dreiecke zeichnen
         {
            nSeite = 0;
            for (j=0; j<3; j++)
            {
               nIndex[j] = pWebTriangle[i].p[j];
               if (bInterpolate && pnBorderInd)                // suchen nach den Randdreiecken
               {
                  for (int k=0; k<nBorderIndex; k++)           // Contourpunkte vergleichen
                  {
                     if (pnBorderInd[k]==nIndex[j])
                     {
                        nSeite |= 1<<j;
                        break;
                     }
                  }
               }
               vec[j] = CVector(pPlaneKoord[nIndex[j]].y, pfLevel[nIndex[j]]*dZNorm, pPlaneKoord[nIndex[j]].x);
            }
            if (pNormal)
               CCara3DGL::Triangle(&Tr, &vec[2], &vec[1], &vec[0], &pNormal[nIndex[2]], &pNormal[nIndex[1]], &pNormal[nIndex[0]], nSeite);
            else
               CCara3DGL::Triangle(&Tr, &vec[2], &vec[1], &vec[0]);
         }
         if (pNormal) delete[] pNormal;
      }
      glEnd();
   }
}

void CPlanGraph::CheckMinMax()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::CheckMinMax");
#endif
   if (m_pfLevels && m_pPlaneKoord)
   {
      int         i, j,
                  nPK       = m_nPlaneKoord,                   // lokale Variablen werden schneller bearbeitet
                  nPictures = m_nFilmPictures;
      Vector2Df  *pfPK      = m_pPlaneKoord;
      float      *pfL       = m_pfLevels;
      float       fXMin, fXMax, fYMin, fYMax, fZMin, fZMax;
      ETSDIV_DIV    div;
      ZeroMemory(&div, sizeof(ETSDIV_DIV));

      fXMin = fXMax = pfPK[0].x;
      fYMin = fYMax = pfPK[0].y;
      fZMin = fZMax = pfL[0];

      float  *pfZValue_i;
      for (i=0; i<nPictures; i++)
      {
         pfZValue_i = &pfL[i*nPK];
         for (j=0; j<nPK; j++)
         {
            if ((i==0) && (j>0))
            {
               if (fXMin > pfPK[j].x) fXMin = pfPK[j].x;
               if (fXMax < pfPK[j].x) fXMax = pfPK[j].x;

               if (fYMin > pfPK[j].y) fYMin = pfPK[j].y;
               if (fYMax < pfPK[j].y) fYMax = pfPK[j].y;
            }
            if (fZMin > pfZValue_i[j]) fZMin = pfZValue_i[j];
            if (fZMax < pfZValue_i[j]) fZMax = pfZValue_i[j];
         }
      }

      if (fabs(fZMax - fZMin) < 1e-6)
      {
         fZMin -= 0.0005f;
         fZMax += 0.0005f;
      }

      div.f1        = (double)fZMin;
      div.f2        = (double)fZMax;
      div.divfnc    = ETSDIV_LINEAR;
      div.stepwidth = 10;
      if (CEtsDiv::RoundMinMaxValues(&div))
      {
         fZMin = (float)div.f1;
         fZMax = (float)div.f2;
      }

      float  fWidthX = fXMax - fXMin,
             fWidthY = fYMax - fYMin,
             fWidthZ = fZMax - fZMin,
             fNorm, fWidth;

      if (fWidthX > fWidthY) fWidth = fWidthX;
      else                   fWidth = fWidthY;

      fNorm = 0.75f * fWidth / fWidthZ;
      m_dMinValue = (double) fZMin;
      m_dMaxValue = (double) fZMax;

      if (fNorm != 1.0)
      {
         fZMax *= fNorm;
         fZMin *= fNorm;
      }

      m_fMaxDimension = (float) hypot((double)fWidthX ,(double)fWidthY);
      //m_fMaxDimension = (float) sqrt(fWidthX*fWidthX +fWidthY*fWidthY);
      SetNormFactor((double)fNorm);
      m_fFloorLevel   = fZMin - (fZMax-fZMin)*0.001f;

      m_ObjectVolume.Left   = fXMin;
      m_ObjectVolume.Right  = fXMax;
      m_ObjectVolume.Near   = fYMin;
      m_ObjectVolume.Far    = fYMax;
      m_ObjectVolume.Bottom = fZMin;
      m_ObjectVolume.Top    = fZMax;
      CBaseClass3D::CheckMinMax();
   }
}

const char* CPlanGraph::GetSpecificUnit()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::GetSpecificUnit");
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

const char* CPlanGraph::GetLevelUnit()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::GetLevelUnit");
#endif
   if (PhaseValuesCalculated())
   {
      if (m_pszPhaseUnit)    return m_pszPhaseUnit;
   }
   else
   {
      if (m_pszLevelUnit) return m_pszLevelUnit;
   }
   return SPACE_STRING;
}

double CPlanGraph::GetSpecificValue(int nIndex)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::GetSpecificValue");
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

char * CPlanGraph::GetUnit()
{
   return PLANGRAPH_UNIT;
}

bool CPlanGraph::IsFlat()
{
   return ((m_nBorderIndexes == 0) && (m_nFloorTriangles == 0));
}

void CPlanGraph::CalculatePhaseValues()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::CalculatePhaseValues");
#endif
   if (PhaseValuesAvailable())
   {
      int    i, j, nPicture = GetPictureIndex(),
             nValues = m_nPlaneKoord * NO_OF_PHASEVALUES;
      bool   bInit = true;
      float *pfLevels = (float*) &m_pfLevels[ nPicture                 *m_nPlaneKoord];
      float *pfPhase  = (float*) &m_pfLevels[(nPicture+m_nFilmPictures)*m_nPlaneKoord];
      float *pfPhaseValues = NULL;
      double dFactor = M_PI / 180.0;
      double dPhaseStep = 360.0 / NO_OF_PHASEVALUES;
      float  fZMax, fZMin;
      m_pfPhaseValues = new float[nValues + SAVED_VALUES];     // alte Werte speichern
      
      if (m_pfPhaseValues)
      {
         m_pfPhaseValues[nValues  ] = m_ObjectVolume.Bottom;
         m_pfPhaseValues[nValues+1] = m_ObjectVolume.Top;
         m_pfPhaseValues[nValues+2] = (float)m_dNormFactor;
         m_pfPhaseValues[nValues+3] = m_fFloorLevel;
         m_pfPhaseValues[nValues+4] = (float)m_dMinValue;
         m_pfPhaseValues[nValues+5] = (float)m_dMaxValue;

         SwapTimeParams();
         
         double dNormFactor = 1.0;
         if      (m_nFlags & ONE_NORMALIZATION_FACTOR) dNormFactor = m_pdNormfactor[0];
         else if (m_nFlags & N_NORMALIZATION_FACTORS ) dNormFactor = m_pdNormfactor[nPicture];
         if (m_nFlags & LEVELVALUES_LOG)
         {
            double dFactordB = 1.0;
            if      (m_nFlags & LEVELVALUES_IN_DB10) dFactordB = 0.1;
            else if (m_nFlags & LEVELVALUES_IN_DB20) dFactordB = 0.05;
            for (i=0; i<m_nPlaneKoord; i++)
            {
               m_pfPhaseValues[i] =(float)(pow(10.0, (pfLevels[i] * 0.05)) * dNormFactor);
            }
         }
         else
         {
            if (m_nFlags & NO_VALUE_NORMALIZATION)
               for (i=0; i<m_nPlaneKoord; i++) m_pfPhaseValues[i] =pfLevels[i];
            else
               for (i=0; i<m_nPlaneKoord; i++) m_pfPhaseValues[i] =(float)(pfLevels[i] * dNormFactor);
         }

         for (j=NO_OF_PHASEVALUES-1; j>=0; j--)
         {
            pfPhaseValues = &m_pfPhaseValues[m_nPlaneKoord*j];
            for (i=0; i<m_nPlaneKoord; i++)
            {
               pfPhaseValues[i] = (float)((double)m_pfPhaseValues[i] * cos((pfPhase[i] + j * dPhaseStep) * dFactor));
               if (bInit)
               {
                  fZMin = fZMax = pfPhaseValues[i];
                  bInit = false;
               }
               else
               {
                  if (pfPhaseValues[i] < fZMin) fZMin = pfPhaseValues[i];
                  if (pfPhaseValues[i] > fZMax) fZMax = pfPhaseValues[i];
               }
            }
         }
         ETSDIV_DIV    div;
         ZeroMemory(&div, sizeof(ETSDIV_DIV));
         div.l2        = 10;
         div.stepwidth = 1;

         if (fabs(fZMax - fZMin) < 1e-6)
         {
            fZMin -= 0.0005f;
            fZMax += 0.0005f;
         }

         div.f1 = fZMin;
         div.f2 = fZMax;
         double      fstart, fstep, fend;
         if (CEtsDiv::CalculateLinDivision(&div, fstep, fstart))
         {
            for ( ;fstart > fZMin; fstart -= fstep);
            fZMin = (float)fstart;
            i = (int)((fZMax - fZMin) / fstep);
            fend = fZMin + fstep * i;
            for ( ;fend < fZMax; fend += fstep, i++);
            fZMax = (float) fend;
         }

         float  fWidthX = m_ObjectVolume.Right - m_ObjectVolume.Left,
                fWidthY = m_ObjectVolume.Far   - m_ObjectVolume.Near,
                fWidthZ = fZMax - fZMin,
                fNorm, fWidth;

         if (fWidthX > fWidthY) fWidth = fWidthX;
         else                   fWidth = fWidthY;

         fNorm = 0.75f * fWidth / fWidthZ;
         m_dMinValue = (double) fZMin;
         m_dMaxValue = (double) fZMax;
         if (fNorm != 1.0)
         {
            fZMax *= fNorm;
            fZMin *= fNorm;
         }

         m_ObjectVolume.Bottom = fZMin;
         m_ObjectVolume.Top    = fZMax;
         SetNormFactor((double)fNorm);
         m_fFloorLevel         = fZMin - (fZMax-fZMin)*0.001f;

         m_nFlags |= PHASEVALUES_CALCULATED;
      }
   }
}

void CPlanGraph::DeletePhaseValues()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::DeletePhaseValues");
#endif
   if (m_pfPhaseValues)
   {
      int nValues = m_nPlaneKoord * NO_OF_PHASEVALUES;
      m_ObjectVolume.Bottom = m_pfPhaseValues[nValues  ];      // alte Werte wiederherstellen
      m_ObjectVolume.Top    = m_pfPhaseValues[nValues+1];
      SetNormFactor((double)m_pfPhaseValues[nValues+2]);
      m_fFloorLevel         = m_pfPhaseValues[nValues+3];
      m_dMinValue           = (double)m_pfPhaseValues[nValues+4];
      m_dMaxValue           = (double)m_pfPhaseValues[nValues+5];

      SwapTimeParams();

      m_nFlags &= ~PHASEVALUES_CALCULATED;
      delete[] m_pfPhaseValues;
      m_pfPhaseValues = NULL;
   }
}

void CPlanGraph::SwapTimeParams()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CPlanGraph::SwapTimeParams");
#endif
   C3DGrid * pG = GetGrid();
   int nTemp = pG->GetRoundingLevel();
   pG->SetRoundingLevel(m_nRoundingLevel);
   m_nRoundingLevel = nTemp;
   nTemp = pG->GetNumModeLevel();
   pG->SetNumModeLevel(m_nNumModeLevel);
   m_nNumModeLevel = nTemp;
   double dTemp = pG->GetGridStepLevel();
   pG->SetStep(-1, m_dGridStepLevel, -1);
   m_dGridStepLevel = dTemp;
}

bool CPlanGraph::SetMinMax(double, double)
{
   return true;
}
