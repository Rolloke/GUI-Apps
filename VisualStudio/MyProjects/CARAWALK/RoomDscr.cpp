/********************************************************************/
/* Funktionen der Klasse CRoomObj                       */
/********************************************************************/
#include "StdAfx.h"

#include "RoomDscr.h"
#include "FileHeader.h"
#include "CCaraMat.h"
#include "Cara3DTr.h"
#include "LightDlg.h"
#include "Ets3dGL.h"
#include "CaraWalkDll.h"
#include <float.h>

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define ROOMDD_CFG               "RoomDD.cfg"
#define CARA_TYPE                0x41524143
#define EPSILON_DET              1e-13
#define EPSILON_INSIDE           1e-6

void ReadData(void *pData, void *ptr, int nSize, long &nptr)
{
   char * p = (char*)pData;
   if (pData && ptr) memcpy(ptr, &p[nptr], nSize);
   nptr += nSize;
}

CRoomObj::CRoomObj()
{
   BEGIN("CRoomObj()");
   Init();
   m_Material.SetDestroyMode(true);
   m_Material.SetDeleteFunction(CMaterial::DeleteFunction);
}

void CRoomObj::Init()
{
   m_dMaxDimRoom            = 0;
   m_nNoOfWalls             = 0;
   m_nNoOfPoints            = 0;
   m_nNoOfAllImageCorners   = 0;
   m_nNoOfBoxes             = 0;
   m_pvPoints               = NULL;
   m_pvNormalWall           = NULL;
   m_pnNoOfCornerWalls      = NULL;
   m_ppnIndexCornerWall     = NULL;
   m_pnIndexMaterialWall    = NULL;
   m_pnNoOfImagesWall       = NULL;
   m_ppnNoOfCornerImages    = NULL;
   m_pvCornerImage          = NULL;
   m_ppnIndexMaterialImage  = NULL;
   m_pfWallMatrix           = NULL;
}

CRoomObj::~CRoomObj()
{
   BEGIN("~CRoomObj()");
   DeleteData();
   DeleteMaterials();
}

void CRoomObj::DeleteData()
{
   BEGIN("DeleteData");
   int i;
   if (m_pvPoints)            delete[] m_pvPoints;
   if (m_pnNoOfCornerWalls)   delete[] m_pnNoOfCornerWalls;
   if (m_pnIndexMaterialWall) delete[] m_pnIndexMaterialWall;
   if (m_pvCornerImage)       delete[] m_pvCornerImage;
   if (m_pnNoOfImagesWall)    delete[] m_pnNoOfImagesWall;
   if (m_pvNormalWall)        delete[] m_pvNormalWall;
   if (m_pfWallMatrix)        delete[] m_pfWallMatrix;

   if (m_ppnIndexCornerWall)             // Array mit Indizes auf die Definitionspunkte (Polygonpunkt[j] der Raumwand[i])
   {
      for (i=0; i<m_nNoOfWalls; i++)
      {
         if (m_ppnIndexCornerWall[i])    // Feld mit Indizes auf die Definitionspunkte der Raumwand[i])
         {
            delete[] m_ppnIndexCornerWall[i];
         }
      }
      delete[] m_ppnIndexCornerWall;
   }

   if (m_ppnNoOfCornerImages)            // Array mit Anzahl der Eckpunkte des Bildes[j] der Raumwand[i]
   {
      for (i=0; i<m_nNoOfWalls; i++)
      {
         if (m_ppnNoOfCornerImages[i])   // Feld mit Anzahl der Eckpunkte des Bildes[j]
         {
            delete[] m_ppnNoOfCornerImages[i];
         }
      }
      delete[] m_ppnNoOfCornerImages;
   }

   if (m_ppnIndexMaterialImage)          // Array mit der Materialnummer des Bildes[j] der Raumwand[i]
   {
      for (i=0; i<m_nNoOfWalls; i++)
      {
         if (m_ppnIndexMaterialImage[i]) // Feld mit der Materialnummer des Bildes
         {
            delete[] m_ppnIndexMaterialImage[i];
         }
      }
      delete[] m_ppnIndexMaterialImage;
   }

   Init();
}

void CRoomObj::DeleteMaterials()
{
   if (m_Material.GetCounter()>0)
   {
      m_Material.Destroy();
   }
}

bool CRoomObj::Read(HANDLE hFile)
{
   BEGIN("Read");
   int i, j, nCornerImage = 0;
   double dDummy;
   CFileHeader fh;

   DeleteData();
   try
   {
      CCaraWalkDll::ReadFileFH(hFile, &fh, sizeof(CFileHeader));
      if (!fh.IsType("CAD"))                                    throw (DWORD)2;
      SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);

      CCaraWalkDll::ReadFileFH(hFile, &fh, sizeof(CFileHeader));
      if (!fh.IsType("BGP"))                                    throw (DWORD)4;
      SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);

      CCaraWalkDll::ReadFileFH(hFile, &fh, sizeof(CFileHeader));
      if (!fh.IsType("NET"))                                    throw (DWORD)6;
      SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);

      CCaraWalkDll::ReadFileFH(hFile, &fh, sizeof(CFileHeader));
      if (!fh.IsType("RDD"))                                    throw (DWORD)8;

      CCaraWalkDll::ReadFileFH(hFile, &m_nNoOfPoints, sizeof(int));
      CCaraWalkDll::ReadFileFH(hFile, &m_nNoOfAllImageCorners, sizeof(int));
      CCaraWalkDll::ReadFileFH(hFile, &m_nNoOfWalls, sizeof(int));
      CCaraWalkDll::ReadFileFH(hFile, &m_dMaxDimRoom, sizeof(double));

      CCaraWalkDll::ReadFileFH(hFile, &dDummy, sizeof(double));

      m_pvPoints = new CVector[m_nNoOfPoints];
      CCaraWalkDll::ReadFileFH(hFile, m_pvPoints, sizeof(CVector)*m_nNoOfPoints);

      m_pnNoOfCornerWalls   = new int[m_nNoOfWalls];
      m_ppnIndexCornerWall  = new int*[m_nNoOfWalls];
      ZeroMemory(m_ppnIndexCornerWall, sizeof(int*)*m_nNoOfWalls);

      m_pnIndexMaterialWall = new long[m_nNoOfWalls];
      m_pnNoOfImagesWall    = new int[m_nNoOfWalls];

      m_ppnNoOfCornerImages = new int*[m_nNoOfWalls];
      ZeroMemory(m_ppnNoOfCornerImages, sizeof(int*)*m_nNoOfWalls);

      if (m_nNoOfAllImageCorners>0)
      {
         m_pvCornerImage = new CVector[m_nNoOfAllImageCorners];
      }

      m_ppnIndexMaterialImage = new long*[m_nNoOfWalls];
      ZeroMemory(m_ppnIndexMaterialImage, sizeof(long*)*m_nNoOfWalls);

      for (i=0; i<m_nNoOfWalls; i++)
      {
         CCaraWalkDll::ReadFileFH(hFile, &m_pnNoOfCornerWalls[i], sizeof(int));

         m_ppnIndexCornerWall[i] = new int[m_pnNoOfCornerWalls[i]];
         CCaraWalkDll::ReadFileFH(hFile, m_ppnIndexCornerWall[i], sizeof(int)*m_pnNoOfCornerWalls[i]);
         CCaraWalkDll::ReadFileFH(hFile, &m_pnIndexMaterialWall[i], sizeof(int));
         CCaraWalkDll::ReadFileFH(hFile, &m_pnNoOfImagesWall[i], sizeof(int));

         if (m_pnNoOfImagesWall[i])
         {
            m_ppnNoOfCornerImages[i]   = new int[m_pnNoOfImagesWall[i]];
            m_ppnIndexMaterialImage[i] = new long[m_pnNoOfImagesWall[i]];

            for (j=0; j<m_pnNoOfImagesWall[i]; j++)
            {
               CCaraWalkDll::ReadFileFH(hFile, &m_ppnNoOfCornerImages[i][j], sizeof(int));

               CCaraWalkDll::ReadFileFH(hFile, &m_pvCornerImage[nCornerImage], sizeof(CVector)*m_ppnNoOfCornerImages[i][j]);
               nCornerImage += m_ppnNoOfCornerImages[i][j];
               CCaraWalkDll::ReadFileFH(hFile, &m_ppnIndexMaterialImage[i][j], sizeof(long));
            }
         }
      }
   }
   catch (DWORD nError)
   {
      REPORT("Read()-Error %d", nError);
      return false;
   }

   CARAtoOpenGL();
   for (i=0; i<m_nNoOfWalls; i++)
   {
      m_pnIndexMaterialWall[i] = -1;         // Materialindex für Boxenview
   }
   return true;
}


long CRoomObj::SetRoomData(void *pData)
{
   BEGIN("SetRoomData");
   int      i, j, nCornerImage = 0;
   long     nDataPtr = 0;

   DeleteData();
   try
   {
      if (pData == NULL)                                    throw (DWORD)1;

      ReadData(pData, &m_dMaxDimRoom, sizeof(double)      , nDataPtr);
      ReadData(pData, &m_nNoOfPoints, sizeof(int)         , nDataPtr);
      ReadData(pData, &m_nNoOfAllImageCorners, sizeof(int), nDataPtr);
      ReadData(pData, &m_nNoOfWalls, sizeof(int)          , nDataPtr);
      ReadData(pData, &m_nNoOfBoxes, sizeof(int)          , nDataPtr);

      m_pvPoints = new CVector[m_nNoOfPoints];
      ReadData(pData, m_pvPoints, sizeof(CVector)*m_nNoOfPoints, nDataPtr);

      m_pnNoOfCornerWalls  = new int[m_nNoOfWalls];
      m_ppnIndexCornerWall = new int*[m_nNoOfWalls];
      ZeroMemory(m_ppnIndexCornerWall, sizeof(int*)*m_nNoOfWalls);

      m_pnIndexMaterialWall = new long[m_nNoOfWalls];
      m_pnNoOfImagesWall    = new int[m_nNoOfWalls];
      m_ppnNoOfCornerImages = new int*[m_nNoOfWalls];
      ZeroMemory(m_ppnNoOfCornerImages, sizeof(int*)*m_nNoOfWalls);

      if (m_nNoOfAllImageCorners>0)
      {
         m_pvCornerImage = new CVector[m_nNoOfAllImageCorners];
      }

      m_ppnIndexMaterialImage = new long*[m_nNoOfWalls];
      ZeroMemory(m_ppnIndexMaterialImage, sizeof(long*)*m_nNoOfWalls);

      for (i=0; i<m_nNoOfWalls; i++)
      {
         ReadData(pData, &m_pnNoOfCornerWalls[i], sizeof(int), nDataPtr);
         ReadData(pData, &m_pnIndexMaterialWall[i], sizeof(long), nDataPtr);
         ReadData(pData, &m_pnNoOfImagesWall[i], sizeof(int), nDataPtr);

         m_ppnIndexCornerWall[i] = new int[m_pnNoOfCornerWalls[i]];

         ReadData(pData, m_ppnIndexCornerWall[i], sizeof(int)*m_pnNoOfCornerWalls[i], nDataPtr);

         if (m_pnNoOfImagesWall[i])
         {
            m_ppnNoOfCornerImages[i]   = new int[m_pnNoOfImagesWall[i]];
            m_ppnIndexMaterialImage[i] = new long[m_pnNoOfImagesWall[i]];

            for (j=0; j<m_pnNoOfImagesWall[i]; j++)
            {
               ReadData(pData, &m_ppnNoOfCornerImages[i][j], sizeof(int),nDataPtr);
               ReadData(pData, &m_ppnIndexMaterialImage[i][j], sizeof(long),nDataPtr);

               ReadData(pData, &m_pvCornerImage[nCornerImage], sizeof(CVector)*m_ppnNoOfCornerImages[i][j], nDataPtr);
               nCornerImage += m_ppnNoOfCornerImages[i][j];
            }
         }
      }
   }
   catch (DWORD nError)
   {
      REPORT("SetRoomData()-Error %08x", nError);
      return 0;
   }

   CARAtoOpenGL();
   return nDataPtr;
}

void CRoomObj::CARAtoOpenGL()
{
   BEGIN("CARAtoOpenGL");
   CVector  vTemp;
   int      i, j;

   for (i=0; i<m_nNoOfPoints; i++)
   {
      CaraToGL(&m_pvPoints[i]);
   }

   for (i=0; i<m_nNoOfAllImageCorners; i++)
   {
      CaraToGL(&m_pvCornerImage[i]);
   }

   m_vLeftBottomNear = m_vRightTopFar = m_pvPoints[0];
   double *pdTest,
          *pdLTN = (double*) &m_vLeftBottomNear,
          *pdRBF = (double*) &m_vRightTopFar;

   for (i=1; i<m_nNoOfPoints; i++)                               // Umhüllenden Quader bestimmen
   {
      pdTest = (double*) &m_pvPoints[i];
      for (j=0; j<3; j++)
      {
         if (pdLTN[j] > pdTest[j]) pdLTN[j] = pdTest[j];
         if (pdRBF[j] < pdTest[j]) pdRBF[j] = pdTest[j];
      }
   }
}

bool CRoomObj::InitMaterials()
{
   BEGIN("InitMaterials");
   if (m_pvNormalWall)
   {
      return true;
   }
   int         nWall = 0, m, nImage, nCornerImageCount = 0, nImagesWall;
   CMaterial  *pWallMat = NULL, *pImageMat;
   CEtsList    WallImages;

   WallImages.SetDestroyMode(false);
   try
   {
      if (m_pvNormalWall == NULL) m_pvNormalWall = new CVector[m_nNoOfWalls];

      for (nWall=0; nWall<m_nNoOfWalls; nWall++)               // für alle Wände
      {
         CVector vPn, vP1, vP2, vP3;
         for (m=2; m<m_pnNoOfCornerWalls[nWall]; m++)
         {
            vP1 = m_pvPoints[m_ppnIndexCornerWall[nWall][m-2]];
            vP2 = m_pvPoints[m_ppnIndexCornerWall[nWall][m-1]];
            vP3 = m_pvPoints[m_ppnIndexCornerWall[nWall][m  ]];
            vPn = Product(vP2-vP1, vP3-vP1);
            double db = Betrag(vPn);
            if (db > 1e-13)                                    // Betrag != 0
            {
               vPn /= db;
               break;
            }
         }
         m_pvNormalWall[nWall] = - vPn;

         ASSERT((m_pnIndexMaterialWall[nWall] == -1) || (HIWORD(m_pnIndexMaterialWall[nWall]) == 0));
         pWallMat = GetMaterial(m_pnIndexMaterialWall[nWall]);
         if (pWallMat == NULL) throw (DWORD)2;
         m_pnIndexMaterialWall[nWall] = (long) pWallMat;

         pWallMat->m_Walls.ADDHead((void*) (nWall+1));         // eine 0 kann in der Liste nicht gespeichert werden

         WallImages.Destroy();                                 // Bilder auf der Wand löschen
         int *pbSections = NULL;
         nImagesWall = m_pnNoOfImagesWall[nWall];
         if (nImagesWall > 1)
         {
            pbSections = new int[nImagesWall];
         }
         for (nImage=0; nImage<nImagesWall; nImage++)          // für alle Bilder auf einer Wand
         {
            ASSERT(HIWORD(m_ppnIndexMaterialImage[nWall][nImage]) == 0);
            pImageMat = GetMaterial(m_ppnIndexMaterialImage[nWall][nImage]);
            if (pImageMat == NULL) throw (DWORD)4;
            m_ppnIndexMaterialImage[nWall][nImage] = (long) pImageMat;
            Image *pImage = new Image;
            pImageMat->m_Images.ADDHead(pImage);
            WallImages.ADDHead(pImage);
            pImage->nImageWall             = nWall;
            pImage->nNoOfImagePoints       = m_ppnNoOfCornerImages[nWall][nImage];
            pImage->nStartIndexImagePoints = nCornerImageCount;
            pImage->nImageNo = 1;                              // Ebene = 1
            if (nImage>0)
            {
               ZeroMemory(pbSections, sizeof(int) * nImage);   // Array mit den Überschneidungen löschen
               int  i, nSection = CheckIntersection(&WallImages, pbSections);
               if (nSection > 0)                               // Überschneidungen vorhanden
               {
                  Image *Image_i;
                  for (i=0; i<nImage; i++)
                  {
                     Image_i = (Image*)WallImages.GetAt(i);    // Bild(i)
                     if (pbSections[i])                        // nach überschneidenden Flächen suchen
                     {
                        if (pImage->nImageNo < (Image_i->nImageNo+1))
                           pImage->nImageNo = Image_i->nImageNo+1;
                        //REPORT("Schneidet %d", i+1);
                     }
                     else
                     {
                        //REPORT("Schneidet %d nicht", i+1);
                     }
                  }
               }
            }
            //REPORT("Bild %d hat Ebene %d", nImage+1, pImage->nImageNo);
            nCornerImageCount += m_ppnNoOfCornerImages[nWall][nImage];
         }
         if (pbSections)
         {
            delete[] pbSections;
         }
      }
   }
   catch (DWORD nError)
   {
      REPORT("InitMaterials()-Error %d", nError);
      return false;
   }
   return true;
}

double CRoomObj::Det1(CVector &A, CVector &B)
{
   return Vx(A)*Vy(B) - Vy(A)*Vx(B);
}
double CRoomObj::Det2(CVector &A, CVector &B)
{
   return Vy(A)*Vz(B) - Vz(A)*Vy(B);
}
double CRoomObj::Det3(CVector &A, CVector &B)
{
   return Vx(A)*Vz(B) - Vz(A)*Vx(B);
}

int CRoomObj::CheckIntersection(CEtsList *pImageList, int *pbSections)
{
   int      i, k, nSections = 0, 
            nMueAbove, nLamdaAbove,
            nMueUnder, nLamdaUnder,
            nImages = pImageList->GetCounter()-1;
   CVector  vA, vB, vO, vTO;
   double   (*pfDet)(CVector&, CVector&) = NULL;
   double   dDet, dMue, dLamda;
   Image   *pImage, *pImageTest;
   pImageTest = (Image*)pImageList->GetLast();
   UINT nOldFp = _controlfp(_PC_64, _MCW_PC);
   for (i=0; i<nImages; i++)
   {
      pImage = (Image*)pImageList->GetAt(i);
      ASSERT(pImageTest != pImage);
      ASSERT(pImageTest->nImageWall == pImage->nImageWall);
      vO = m_pvCornerImage[pImage->nStartIndexImagePoints+1];  // Ursprungsvektor
      vA = m_pvCornerImage[pImage->nStartIndexImagePoints]  -vO;// A-Vektor
      vB = m_pvCornerImage[pImage->nStartIndexImagePoints+2]-vO;// B-Vektor
      pfDet = NULL;                                            // Determinatenfunktion auf 0 setzen
      dDet  = Det1(vA, vB);                                    // Determinante(1) bilden
      if (fabs(dDet) > EPSILON_DET)                            // Determinate(1) != 0
      {
         pfDet = Det1;                                         // Funktion 1 nehmen
      }
      else                                                     // weiter testen
      {
         dDet = Det2(vA, vB);                                  // Determinante(2) bilden
         if (fabs(dDet) > EPSILON_DET)                         // Determinate(2) != 0
         {
            pfDet = Det2;                                      // Funktion 2 nehmen
         }
         else                                                  // weiter testen
         {
            dDet = Det3(vA, vB);                               // Determinante(3) bilden
            if (fabs(dDet) > EPSILON_DET)                      // Determinate(3) != 0
            {
               pfDet = Det3;                                   // Funktion 3 nehmen
            }
         }
      }
      if (pfDet)                                               // ist eine Determinante vorhanden
      {
         dDet = 1.0 / dDet;                                    // Kehrwert bilden
         nMueUnder    = 0;
         nLamdaUnder  = 0;
         nMueAbove    = 0;
         nLamdaAbove  = 0;
         for (k=0; k<pImageTest->nNoOfImagePoints; k++)
         {
            vTO = m_pvCornerImage[pImageTest->nStartIndexImagePoints+k] - vO;
            dMue   = pfDet(vTO, vB ) * dDet;                   // Lambda und Mue bestimmen
            dLamda = pfDet(vA , vTO) * dDet;                   // und auf innerhalb der Fläche testen
            if      (dMue  <     EPSILON_INSIDE) nMueUnder++;  // Punkte unterhalb zählen
            else if (dMue  >(1.0-EPSILON_INSIDE))nMueAbove++;  // Punkte oberhalb zählen
            if      (dLamda<     EPSILON_INSIDE) nLamdaUnder++;// Punkte unterhalb zählen
            else if (dLamda>(1.0-EPSILON_INSIDE))nLamdaAbove++;// Punkte oberhalb zählen
         }
         if ((nLamdaUnder!=4) && (nLamdaAbove!=4) && (nMueUnder!=4) && (nMueAbove!=4))
         {
            if (pbSections) pbSections[i]++;                   // Image markieren
            nSections++;                                       // Überschneidungen zählen
         }
         else if (pbSections)                                  // (pbSections) verhindert die Rekursion !!
         {                                                     // den umgekehrten Fall prüfen
            CEtsList list;
            list.SetDestroyMode(false);
            list.ADDHead(pImageTest);                          // Fläche B
            list.ADDHead(pImage);                              // Fläche A
            if (CheckIntersection(&list, NULL))                // ist Fläche B in A
            {
               pbSections[i]++;                                // Image markieren
               nSections++;                                    // Überschneidungen zählen
            }
         }
         //REPORT("Mue: a:%d u:%d, Lamda:a:%d u:%d (%c)", nMueAbove, nMueUnder, nLamdaAbove, nLamdaUnder, ( (pbSections!=NULL) ? ((pbSections[i] != 0) ? 'X' : '0') : '#'));
      }
   }
   _controlfp(nOldFp, _MCW_PC);
   return nSections;
}

void CRoomObj::CalculateShadowMatrixes(CGlLight *pLight)
{
   BEGIN("CalculateShadowMatrixes");
   double   dDistanceFactor = m_dMaxDimRoom * 0.001;             // Den Schatten von der Wand abheben
   CVector  vOrt;
   int      nWall;
   double   a,b,c,d,dx,dy,dz,adx,bdy,cdz;
   float   *pfMatrix;

   InitMaterials();

   if (m_pfWallMatrix == NULL)
   {
      m_pfWallMatrix = new float[m_nNoOfWalls*16];
   }

   CVector vLightDir = -pLight->GetDirection();

   for (nWall=0; nWall<m_nNoOfWalls; nWall++)                  // für alle Wände
   {
      pfMatrix = &m_pfWallMatrix[nWall*16];
      d = Cosinus(vLightDir, m_pvNormalWall[nWall]);
      if ((d > 1e-13))                                         // wenn ein Schatten fällt
      {                                                        // Matrix ausrechnen
         vOrt = m_pvPoints[m_ppnIndexCornerWall[nWall][0]] + m_pvNormalWall[nWall] * dDistanceFactor;
         a  = Vx(m_pvNormalWall[nWall]);
         b  = Vy(m_pvNormalWall[nWall]);
         c  = Vz(m_pvNormalWall[nWall]);

         dx = Vx(vLightDir);
         dy = Vy(vLightDir);
         dz = Vz(vLightDir);

         d  = -(m_pvNormalWall[nWall]*vOrt);

         // Ebenen Gleichung : ax+by+cz+d=0; d = -Abstand zur Ebene
         // dx,dy,dz = Richtungsvektor des Lichtes
         // ergibt folgende Matrix für die Projection in OpenGL

         // |b*dy+c*dz  -b*dx      -c*dx      -d*dx         |
         // |-a*dy      a*dx+c*dz  -c*dy      -d*dy         |
         // |-a*dz      -b*dz      a*dx+b*dy  -d*dz         |
         // |0          0          0          a*dx+b*dy+c*dz|

         // In OpenGL werden Matrizen folgendermaßen gespeichert
         // | 0   4   8  12 |
         // | 1   5   9  13 |
         // | 2   6  10  14 |
         // | 3   7  11  15 |

         adx = a*dx;
         bdy = b*dy;
         cdz = c*dz;

         Array44 m;
         m.a.m11 = bdy+cdz;
         m.a.m21 = -a*dy;
         m.a.m31 = -a*dz;
         m.a.m41 = 0;
         m.a.m12 = -b*dx;
         m.a.m22 = adx+cdz;
         m.a.m32 = -b*dz;
         m.a.m42 = 0;
         m.a.m13 = -c*dx;
         m.a.m23 = -c*dy;
         m.a.m33 = adx+bdy;
         m.a.m43 = 0;
         m.a.m14 = -d*dx;
         m.a.m24 = -d*dy;
         m.a.m34 = -d*dz;
         m.a.m44 = adx+bdy+cdz;
         // da die W-Komponente eines Vertex (x,y,z,w) nicht negativ werden darf eventuell alle Vorzeichen invertieren
         int i;
         if (m.a.m44<0.0)
         {
            REPORT("Negating");
            for (i=0;i<16;i++) m.v[i] = -m.v[i];
         }

         for (i=0;i<16;i++) pfMatrix[i] = (float) m.v[i];
      }
      else                                                     // Fällt kein Schatten
      {
         pfMatrix[15] = -1;                                    // Matrixelement 15 auf -1 setzen
      }                                                        // W-Komponente darf nicht negativ sein, s.o.
   }
}  

bool CRoomObj::IsValidMatrix(int nWall)
{
   if (m_pfWallMatrix!=NULL)
   {
      return (m_pfWallMatrix[nWall*16+15]==-1) ? false : true;
   }
   else return false;
}

float * CRoomObj::GetWallMatrix(int nWall)
{
   if (m_pfWallMatrix) return &m_pfWallMatrix[nWall*16];
   return NULL;
}
/*
void CRoomObj::DrawRoomEx(int nBaseID, bool bLight, bool bTexture, int nDetail, HPALETTE hPal, double dDist)
{
   try
   {
      int         nWall, nImage, nImagesWall, nStencilVal, nTemp,
                  nCornerImageCount = 0;  // auf 0 setzen, wird gezählt
      CVector     vDist;
      float       fAlpha = 1.0;
      CMaterial  *pM;
      ETS3DGL_Fan Fan;
      double      dMax = 0.2 + m_dMaxDimRoom / (double)(8 << nDetail); // Dreiecksgröße

      InitMaterials();

      Fan.dStep  = dMax;
      Fan.pFn    = DrawFan;

      if (bTexture)
      {
         glEnable(GL_TEXTURE_GEN_S);
         glEnable(GL_TEXTURE_GEN_T);
         glEnable(GL_TEXTURE_2D);
         for (pM=(CMaterial*)m_Material.GetFirst(); pM != NULL; pM=(CMaterial*)m_Material.GetNext())
         {
            if (pM->IsTextureObject()) continue;
            pM->CreateTextureObject();
            ASSERT(glGetError() == GL_NO_ERROR);
         }
      }

      glNewList(nBaseID, GL_COMPILE);

      if (bTexture)
      {
         glEnable(GL_TEXTURE_GEN_S);
         glEnable(GL_TEXTURE_GEN_T);
         glEnable(GL_TEXTURE_2D);
      }
      else
      {
         glDisable(GL_TEXTURE_GEN_S);
         glDisable(GL_TEXTURE_GEN_T);
         glDisable(GL_TEXTURE_2D);
      }

      if (bLight) glEnable( GL_LIGHTING);
      else        glDisable(GL_LIGHTING);

      // erst Bilder, dann die Wand
      for (nWall=0; nWall<m_nNoOfWalls; nWall++)               // für alle Wände
      {
         nImagesWall = m_pnNoOfImagesWall[nWall];
         if (nImagesWall>0)
         {
            glClearStencil(0);
            nStencilVal = nImagesWall;
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_GREATER, nStencilVal, 0xffffffff);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
         }
         for (nImage=0; nImage<nImagesWall; nImage++)          // für alle Bilder auf einer Wand
         {
            nCornerImageCount += m_ppnNoOfCornerImages[nWall][nImage];
         }
         nTemp = nCornerImageCount;

         for (nImage=nImagesWall-1; nImage>=0; nImage--)          // für alle Bilder auf einer Wand
         {
            nCornerImageCount -= m_ppnNoOfCornerImages[nWall][nImage];
            pM = (CMaterial*) m_ppnIndexMaterialImage[nWall][nImage];
            pM->ImageMaterialAndLight(bTexture, bLight, hPal, fAlpha);
            DrawImage(nCornerImageCount, m_ppnNoOfCornerImages[nWall][nImage], bLight, vDist, Fan);
            glStencilFunc(GL_LESS, --nStencilVal, 0xffffffff);
         }

         pM = (CMaterial*) m_pnIndexMaterialWall[nWall];
         pM->ImageMaterialAndLight(bTexture, bLight, hPal);
         ASSERT(glGetError() == GL_NO_ERROR);
         DrawWall(nWall, pM, Fan, bLight, bTexture);

         nCornerImageCount = nTemp;
      }
      glDisable(GL_STENCIL_TEST);

      // erst Wand, dann die Bilder
      for (nWall=0; nWall<m_nNoOfWalls; nWall++)               // für alle Wände
      {
         pM = (CMaterial*) m_pnIndexMaterialWall[nWall];
         pM->ImageMaterialAndLight(bTexture, bLight, hPal);
         ASSERT(glGetError() == GL_NO_ERROR);
         DrawWall(nWall, pM, Fan, bLight, bTexture);
         nImagesWall = m_pnNoOfImagesWall[nWall];

         for (nImage=0; nImage<nImagesWall; nImage++)          // für alle Bilder auf einer Wand
         {
            pM = (CMaterial*) m_ppnIndexMaterialImage[nWall][nImage];
            pM->ImageMaterialAndLight(bTexture, bLight, hPal, fAlpha);
            DrawImage(nCornerImageCount, m_ppnNoOfCornerImages[nWall][nImage], bLight, vDist, Fan);
            nCornerImageCount += m_ppnNoOfCornerImages[nWall][nImage];
         }
      }

      glFlush();
      glEndList();
   }
   catch (DWORD nError)
   {
      REPORT("DrawRoomEx()-Error %d", nError);
   }
}
*/
void CRoomObj::DrawRoom(int nBaseID, bool bLight, bool bTexture, int nDetail, HPALETTE hPal, double dDist)
{
   BEGIN("DrawRoom");
   int         nWall = 0, m, nW,
//               nCornerImageCount = 0,
               nNoOfWalls, nNoOfImages;
   CVector     vPoint, vDist(0,0,0);
   double      dMax = 0.2 + m_dMaxDimRoom / (double)(8 << nDetail); // Dreiecksgröße
   ETS3DGL_Fan Fan;
   CEtsList    list;

   InitMaterials();

   Fan.dStep  = dMax;
   Fan.pFn    = DrawFan;
   list.SetDestroyMode(false);

   CMaterial *pM = NULL;
   for (pM=(CMaterial*)m_Material.GetFirst(); pM != NULL; pM=(CMaterial*)m_Material.GetNext())
   {
      nNoOfWalls  = pM->m_Walls.GetCounter();
      nNoOfImages = pM->m_Images.GetCounter();
      REPORT("Material: %d, Walls: %d, Images: %d", pM->nMatNo, nNoOfWalls, nNoOfImages);
      if ((0 == nNoOfWalls) && (0 == nNoOfImages)) continue;
      m = m_Material.GetActualPosition();
      glNewList(nBaseID+1+m, GL_COMPILE);
      pM->ImageMaterialAndLight(bTexture, bLight, hPal);
      nWall=(int)pM->m_Walls.GetFirst();
      for (nW=0; nW<nNoOfWalls; nWall=(int)pM->m_Walls.GetNext(), nW++)
      {
         DrawWall(nWall-1, pM, Fan, bLight, bTexture);
      }

      if (CCaraWalkDll::gm_nPolygonOffset) glEnable(GL_POLYGON_OFFSET_FILL);
      Image *pImage;
      for (pImage = (Image*)pM->m_Images.GetFirst(); pImage != NULL; pImage = (Image*)pM->m_Images.GetNext())
      {
         nWall = pImage->nImageWall;                           // die zugehörige Wand des Bildes
         if (CCaraWalkDll::gm_nPolygonOffset)                  // Offset im Depthbuffer
            glPolygonOffset(CCaraWalkDll::gm_fPolygonFactor, (float)(-pImage->nImageNo*CCaraWalkDll::gm_nPolygonOffset));
         else                                                  // Offset als Abstand
            vDist = (dDist*pImage->nImageNo)*m_pvNormalWall[nWall];// Das Bild um 1 mm von der Wand abheben

         if (CCaraWalkDll::gm_bBugFixNormalVector)             // Normalenvectoren immer setzen
         {
            Fan.pParam = &m_pvNormalWall[nWall];
         }
         else                                                  // Normalenvector nur einmal setzen
         {
            Fan.pParam = NULL;
            if (bLight) glNormal3dv((double*) &m_pvNormalWall[nWall]);
         }

         if (bTexture)                                         // Texturen anzeigen, ja :
         {
            if (!pM->m_bGenerateCoordinates && (pImage->nNoOfImagePoints == 4))
            {
               CVector vPoint;
               glDisable(GL_TEXTURE_GEN_S);
               glDisable(GL_TEXTURE_GEN_T);
               pM->AlignTexture(m_pvNormalWall[nWall]);           // Texturkoordinaten anhand des Normalenvektors der Wand ausrichten
               glBegin(GL_QUADS);
                  vPoint = m_pvCornerImage[pImage->nStartIndexImagePoints+3] + vDist;
                  glTexCoord2d(0.0                , 0.0);
                  glVertex3dv((double*)&vPoint);
                  vPoint = m_pvCornerImage[pImage->nStartIndexImagePoints+2] + vDist;
                  glTexCoord2d(pM->m_fsScaleFactor, 0.0);
                  glVertex3dv((double*)&vPoint);
                  vPoint = m_pvCornerImage[pImage->nStartIndexImagePoints+1] + vDist;
                  glTexCoord2d(pM->m_fsScaleFactor, pM->m_ftScaleFactor);
                  glVertex3dv((double*)&vPoint);
                  vPoint = m_pvCornerImage[pImage->nStartIndexImagePoints+0] + vDist;
                  glTexCoord2d(0.0                , pM->m_ftScaleFactor);
                  glVertex3dv((double*)&vPoint);
               glEnd();
               glEnable(GL_TEXTURE_GEN_S);
               glEnable(GL_TEXTURE_GEN_T);
               continue;
            }
            else
               pM->AlignTexture(m_pvNormalWall[nWall]);           // Texturkoordinaten anhand des Normalenvektors der Wand ausrichten
         }

         DrawImage(pImage->nStartIndexImagePoints, pImage->nNoOfImagePoints, bLight, vDist, Fan);
      }
      if (CCaraWalkDll::gm_nPolygonOffset) glDisable(GL_POLYGON_OFFSET_FILL);
      glFlush();
      glEndList();
   }

   glNewList(nBaseID, GL_COMPILE);

   if (bTexture)
   {
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      glEnable(GL_TEXTURE_2D);
   }
   else
   {
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_2D);
   }

   if (bLight) glEnable( GL_LIGHTING);
   else        glDisable(GL_LIGHTING);

   for (m=0; m<m_Material.GetCounter(); m++)
   {
      glCallList(nBaseID+1+m);
   }
   glFlush();
   glEndList();
}

void CRoomObj::DrawImage(int nCornerImageCount, int nNoOfImagePoints, bool bLight, CVector &vDist, ETS3DGL_Fan &Fan)
{
   CVector vP1, vP2, vP3, vPoint;
   int nCornerImage;
   CEtsList list;
   list.SetDestroyMode(false);

   vPoint = m_pvCornerImage[nCornerImageCount];
   list.Destroy();
   list.ADDHead(&vPoint);
   for (nCornerImage=0; nCornerImage<nNoOfImagePoints-2; nCornerImage++)
   {                                                  // für alle Ecken eines Bildes
      nCornerImageCount++;
      ASSERT((nCornerImageCount+1)<m_nNoOfAllImageCorners);
      if (CCaraWalkDll::IsTriangleNull(vPoint, m_pvCornerImage[nCornerImageCount], m_pvCornerImage[nCornerImageCount+1]))
      {
         continue;
      }

      if (bLight)                                           // Beleuchtung an :
      {                                                     // Die Oberflächen müssen für Beleuchtung 
         vP1 = vPoint + vDist;
         vP2 = m_pvCornerImage[nCornerImageCount  ] + vDist;
         vP3 = m_pvCornerImage[nCornerImageCount+1] + vDist;
         CEts3DGL::DrawTriangleFan(&vP1, &vP3, &vP2, &Fan);// aufgeteilt werden
      }
      else
      {
         if (!list.InList(&m_pvCornerImage[nCornerImageCount]  )) list.ADDHead(&m_pvCornerImage[nCornerImageCount]  );
         if (!list.InList(&m_pvCornerImage[nCornerImageCount+1])) list.ADDHead(&m_pvCornerImage[nCornerImageCount+1]);
      }
   }
   if (!bLight)
   {
      int i, nCount = list.GetCounter();
      CVector *p;
      if (list.GetCounter() == 3) glBegin(GL_TRIANGLES);
      else                        glBegin(GL_TRIANGLE_FAN);
      p = (CVector*)list.GetFirst();
      vPoint = *p + vDist;
      glVertex3dv((double*)&vPoint);
      for (p = (CVector*)list.GetLast(),i=1; (p!= NULL) && (i<nCount); p = (CVector*)list.GetPrevious(),i++)
      {
         vPoint = *p + vDist;
         glVertex3dv((double*)&vPoint);
      }
      glEnd();
   }
}

void CRoomObj::DrawWall(int nWall, CMaterial *pM, ETS3DGL_Fan &Fan, bool bLight, bool bTexture)
{
   int nP1, nP2, nP3;                                 // aufgeteilt werden
   int  nNoOfCornerWalls  = m_pnNoOfCornerWalls[nWall];
   int *pnIndexCornerWall = m_ppnIndexCornerWall[nWall];
   int  nCornerWall = nNoOfCornerWalls-1;
   CEtsList list;
   list.SetDestroyMode(false);

   if (bTexture)                                         // Texturen anzeigen, ja :
      pM->AlignTexture(m_pvNormalWall[nWall]);           // Texturkoordinaten anhand des Normalenvektors der Wand ausrichten

   nP1 = pnIndexCornerWall[nCornerWall];
   ASSERT(nP1<m_nNoOfPoints);

   if (CCaraWalkDll::gm_bBugFixNormalVector)             // Normalenvectoren immer setzen
   {
      Fan.pParam = &m_pvNormalWall[nWall];
   }
   else                                                  // Normalenvector nur einmal setzen
   {
      Fan.pParam = NULL;
      if (bLight) glNormal3dv((double*) &m_pvNormalWall[nWall]);
   }

   list.Destroy();
   list.ADDHead(&m_pvPoints[nP1]);

   for (; nCornerWall>=2; nCornerWall--)
   {
      nP2 = pnIndexCornerWall[nCornerWall-1];
      ASSERT(nP2<m_nNoOfPoints);
      nP3 = pnIndexCornerWall[nCornerWall-2];
      ASSERT(nP3<m_nNoOfPoints);
      if (CCaraWalkDll::IsTriangleNull(m_pvPoints[nP1], m_pvPoints[nP2], m_pvPoints[nP3]))
      {
         REPORT("Null-Dreieck[%d](%d,%d)", nWall, nCornerWall-1, nCornerWall-2);
         continue;
      }
      if (bLight)                                           // Beleuchtung an :
      {
/*
         HWND hwnd = (HWND)0x0fe4;
         HDC  hdc  = NULL;
         if ((nWall == 0) && (nCornerWall == 10))
         {
            if (::IsWindow(hwnd))
            {
               hdc = ::GetDC(hwnd);
               POINT pt = {300, 200};
               SIZE  sz = {300, 700};
               ::SetMapMode(hdc,MM_ANISOTROPIC );
               ::SetViewportOrgEx(hdc, pt.x, pt.y, &pt);
               ::SetViewportExtEx(hdc, sz.cx, sz.cy, &sz);
               ::MoveToEx(hdc, pt.x, pt.y, &pt);
               Fan.pParam = hdc;
            }
         }
*/
         CEts3DGL::DrawTriangleFan(&m_pvPoints[nP1], &m_pvPoints[nP2], &m_pvPoints[nP3], &Fan);

//         if (hdc) ::ReleaseDC(hwnd, hdc);
      }
      else
      {
         if (!list.InList(&m_pvPoints[nP2])) list.ADDHead(&m_pvPoints[nP2]);
         if (!list.InList(&m_pvPoints[nP3])) list.ADDHead(&m_pvPoints[nP3]);
      }
   }
   if (!bLight)                                           // Beleuchtung an :
   {
      double *p;
      if (list.GetCounter() == 3) glBegin(GL_TRIANGLES);
      else                        glBegin(GL_TRIANGLE_FAN);
      for (p = (double*)list.GetFirst(); p!= NULL; p = (double*)list.GetNext())
      {
         glVertex3dv(p);
      }
      glEnd();
   }
}

void CRoomObj::GetBmpFile(int nMatNo, BITMAPINFO **ppBmi)
{
   char szFilePath[_MAX_PATH];
   char szCurDir[_MAX_PATH];
   CCaraWalkDll::GetCaraDirectory(_MAX_PATH, szCurDir);
   if (szCurDir[lstrlen(szCurDir)-1] != BACK_SLASH_SIGN)
      strcat(szCurDir, BACK_SLASH_STR);
   if (nMatNo == -1)
      wsprintf(szFilePath, "%sTurnBox.BMP", szCurDir);
   else
      wsprintf(szFilePath, "%sBMP\\Bitmap%d.BMP", szCurDir, nMatNo);
   REPORT("%s", szFilePath);
   CCaraWalkDll::GetBmpFile(szFilePath, ppBmi);
}

CMaterial* CRoomObj::GetMaterial(char *pszBmpFile, bool bNew)
{
   BITMAPINFO *pBmi = NULL;
   CMaterial  *pM   = NULL;
   if (!bNew)
   {
      for (pM=(CMaterial*)m_Material.GetFirst(); pM != NULL; pM=(CMaterial*)m_Material.GetNext())
      {
         if (pszBmpFile == NULL) break;
         const char *pszFN = pM->GetBitmapFileName();
         if (pszFN == NULL) continue;
         if (strcmp(pszBmpFile, pszFN) == 0)
            return pM;
      }
   }
   pM = new CMaterial;
   if (pM)
   {
      if (pszBmpFile) CCaraWalkDll::GetBmpFile(pszBmpFile, &pBmi);
      pM->nMatNo = -2;
      m_Material.ADDHead(pM);
      if ((pBmi == NULL) || !pM->SetTexture(pBmi))              // Keine Textur : Zufallsgenerator für Farbe
      {
         pM->RandomMaterial();
      }
   }

   if (pBmi) CCaraWalkDll::Free(pBmi);
   return pM;
}

CMaterial *CRoomObj::GetMaterialFromIndex(int &nMatNo, CMaterial*pMS)
{
   CMaterial  *pM   = NULL;
   int i;
   for (pM=(CMaterial*)m_Material.GetFirst(), i=0; pM != NULL; pM=(CMaterial*)m_Material.GetNext(), i++)
   {
      if (pMS == pM) break;
      else if (nMatNo == i) 
         return pM;
   }
   if (pMS) nMatNo = i;
   return NULL;
}

CMaterial *CRoomObj::GetMaterial(int nMatNo)
{
   BITMAPINFO *pBmi = NULL;
   CMaterial  *pM   = NULL;
   for (pM=(CMaterial*)m_Material.GetFirst(); pM != NULL; pM=(CMaterial*)m_Material.GetNext())
   {
      if (nMatNo == pM->nMatNo)
         return pM;
   }

   GetBmpFile(nMatNo, &pBmi);                                  // nach Bitmap mit Materialnummer suchen

   pM = new CMaterial;
   if (pM)
   {
      pM->nMatNo = nMatNo;
      char szFilePath[_MAX_PATH];
      char szCurDir[_MAX_PATH];
      CCaraWalkDll::GetCaraDirectory(_MAX_PATH, szCurDir);
      if (szCurDir[lstrlen(szCurDir)-1] != BACK_SLASH_SIGN)
         strcat(szCurDir, BACK_SLASH_STR);
      wsprintf(szFilePath, "%sBMP\\Material%d.cmp", szCurDir, nMatNo);
      HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
      if (pM->ReadFile(hFile))
         CloseHandle(hFile);
      m_Material.ADDHead(pM);
      if (!pBmi && pM->GetBitmapFileName())                    // nach Bitmapdatei suchen
      {
         CCaraWalkDll::GetBmpFile((char*)pM->GetBitmapFileName(), &pBmi);
      }
   }

   if (!pBmi)                                                  // nach BitmapInfoTextur suchen
   {
      CCaraMat::GetBitmapInfo(nMatNo, CARAMAT_BITMAPTYPE32X32+1, CCaraWalkDll::GetHeapHandle(), &pBmi);
      if (pBmi)  CCaraWalkDll::IncrementHeapInstances();
   }

   if (pM)
   {
      if ((pBmi == NULL) || !pM->SetTexture(pBmi))              // Keine Textur : Zufallsgenerator für Farbe
      {
         pM->RandomMaterial();
      }
   }

   if (pBmi) CCaraWalkDll::Free(pBmi);
   
   return pM;
}

bool CRoomObj::SetPaletteAndIndexes(HPALETTE hPal)
{
   BEGIN("SetPaletteAndIndexes");
   int            nr, nRamp, nColors, nEntries, nBaseCol;
   PALETTEENTRY  *pPe;
   float         *pfAverageColor, fFactor;

   nColors = GetPaletteEntries(hPal, 0, 0, NULL) - 16;
   pPe = (PALETTEENTRY*) CCaraWalkDll::Alloc(sizeof(PALETTEENTRY) * nColors);

   nRamp = nColors / m_Material.GetCounter();
   if (nRamp == 0) nRamp = 1;

   nEntries = 0;
   CMaterial *pM;
   for (pM=(CMaterial*)m_Material.GetFirst(); pM != NULL; pM=(CMaterial*)m_Material.GetNext())
   {
      pfAverageColor = (float*)&pM->m_pfAverageImageColor;
      nBaseCol = nEntries+16;
      for (nr=0; nr<nRamp; nr++)
      {
         fFactor = (float)nr / (float)nRamp;
         pPe[nEntries].peRed   = (BYTE)(pfAverageColor[0]*(0.5+fFactor*0.6));
         pPe[nEntries].peGreen = (BYTE)(pfAverageColor[1]*(0.5+fFactor*0.6));
         pPe[nEntries].peBlue  = (BYTE)(pfAverageColor[2]*(0.5+fFactor*0.6));
         pPe[nEntries].peFlags = PC_NOCOLLAPSE;
         nEntries++;
      }
      pfAverageColor[0] = (float) nBaseCol;
      pfAverageColor[1] = (float)(nEntries-17);
      pfAverageColor[2] = pfAverageColor[1];
   }

   nEntries = SetPaletteEntries(hPal, 16, nEntries, pPe);
   if (pPe) CCaraWalkDll::Free(pPe);
   return (nEntries > 0) ? true : false;
}


CVector CRoomObj::GetCenter()
{
   return (m_vLeftBottomNear + m_vRightTopFar) * 0.5;
}

bool CRoomObj::IsInside(CVector *pvEye)
{
   double *pdTest,
          *pdLTN = (double*) &m_vLeftBottomNear,
          *pdRBF = (double*) &m_vRightTopFar;

   pdTest = (double*) pvEye;
   for (int j=0; j<3; j++)
   {
      if (pdLTN[j] > pdTest[j]) return false;
      if (pdRBF[j] < pdTest[j]) return false;
   }

   return true;
}

/******************************************************************************
* Name       : DrawFan                                                        *
* Definition : DrawFan(int, CVector *, void *);                               *
* Zweck      : Ausgabe von Vertexes für OpenGL für den Dreieck-Fan.           *
* Aufruf     : Als Callback-Funktion in der Funktion glcTriangleFan(..);      *
* Parameter  :                                                                *
* nCount (E) : Anzahl der übergebenen Punkte                       (int)      *
* pV     (E) : Punkte als CVector-Array                            (CVector*) *
* pParam (E) : Parameter für die Funktion                          (void*)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CRoomObj::DrawFan(int nCount, CVector *pV, void *pParam)
{
   int i;
   if (nCount == 3) glBegin(GL_TRIANGLES);
   else             glBegin(GL_TRIANGLE_FAN);

   for (i=0; i<nCount; i++)
   {
      if (pParam) glNormal3dv((double*)pParam);
      glVertex3dv((double*)&pV[i]);
   }
   glEnd();
}

#undef M_PI
#undef ROOMDD_CFG
#undef CARA_TYPE

void CRoomObj::CaraToGL(CVector *pV)
{
   ASSERT(pV != NULL);
   double dTemp = Vx(*pV);
   pV->SetX()   = Vy(*pV);
   pV->SetY()   = Vz(*pV);
   pV->SetZ()   = dTemp;
}

void CRoomObj::GLToCara(CVector *pV)
{
   ASSERT(pV != NULL);
   double dTemp = Vx(*pV);
   pV->SetX()   = Vz(*pV);
   pV->SetZ()   = Vy(*pV);
   pV->SetY()   = dTemp;
}

/*
int CRoomObj::CheckIntersection(CEtsList *pImageList, Image *pImageTest, int *pbSections)
{
   int      i, j, k, nSections = 0,
            nV11, nV12, nV21, nV22,
            nImages = pImageList->GetCounter()-1;
   CVector  vSect;
   Image   *pImage;
   for (i=0; i<nImages; i++)
   {
      pImage = (Image*)pImageList->GetAt(i);
      ASSERT(pImageTest != pImage);
      ASSERT(pImageTest->nImageWall == pImage->nImageWall);

      for (j=0; j<pImageTest->nNoOfImagePoints; j++)
      {
         if (j==0) nV11 = pImageTest->nStartIndexImagePoints+pImageTest->nNoOfImagePoints-1;
         else      nV11 = pImageTest->nStartIndexImagePoints+j-1;
                   nV12 = pImageTest->nStartIndexImagePoints+j;
         for (k=0; k<pImage->nNoOfImagePoints; k++)
         {
            if (k==0) nV21 = pImage->nStartIndexImagePoints+pImage->nNoOfImagePoints-1;
            else      nV21 = pImage->nStartIndexImagePoints+k-1;
            nV22 = pImage->nStartIndexImagePoints+k;
            pbSections[i] = (Intersection(m_pvCornerImage[nV11], m_pvCornerImage[nV12], m_pvCornerImage[nV21], m_pvCornerImage[nV22], vSect) == 7) ? true : false;
            if (pbSections[i]) break;
         }
         if (pbSections[i]) break;
      }
      if (pbSections[i])                                       // Überschneidungen von zwei Materialien
      {
         nSections++;
      }
   }
   return nSections;
}
#define PRECISION                1.0e-13
int CRoomObj::Intersection(CVector& vL11, CVector& vL12, CVector& vL21, CVector& vL22, CVector& vSect)
{
   int nReturn = 0;
   double rx[2], ax[2][2], det, l1, l2;
   CVector rd, r2;
   CVector vDir1 = vL12 - vL11;
   CVector vDir2 = vL22 - vL21;
   rd = vL11 - vL21;

   ax[0][0] = Quadbetrag(vDir1);
   ax[0][1] = -(vDir1*vDir2);
   ax[1][0] = ax[0][1];
   ax[1][1] = Quadbetrag(vDir2);

   det = ax[0][0]*ax[1][1]-ax[0][1]*ax[1][0];

   if (fabs(det)<PRECISION) return nReturn;                      // Fehler

   rx[0] = -(vDir1*rd);
   rx[1] =   vDir2*rd;

   det = 1.0/det;

   l1 = (rx[0] * ax[1][1] - rx[1] * ax[0][1]) * det;
   if ((l1 > PRECISION) && (l1 < (1.0-PRECISION))) nReturn |= 0x02;
   l2 = (rx[1] * ax[0][0] - rx[0] * ax[1][0]) * det;
   if ((l2 > PRECISION) && (l2 < (1.0-PRECISION))) nReturn |= 0x04;

   vSect = vL11 + vDir1 * l1;
   r2    = vL21 + vDir2 * l2;
   
   if (vSect==r2) nReturn |= 0x01;

   if (nReturn==7)
   {
      REPORT("Section (%f, %f)", l1, l2);
   }

   return nReturn; 
}
*/
