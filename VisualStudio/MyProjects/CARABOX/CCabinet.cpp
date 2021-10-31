#include "CCabinet.h"
#include "BoxStructures.h"
#include "CSortFunctions.h"
#include "BOXGLOBAL.H"
#include "Debug.h"
#include "Curve.h"
#include "BoxPropertySheet.h"
#include "Cara3DTr.h"
#include "COpenGLView.h"

#ifdef _DEBUG_HEAP
 #ifdef Alloc
  #undef Alloc
 #endif
 #define Alloc(p1, p2) Alloc(p1, p2, __FILE__, __LINE__)
 #define new DLL_DEBUG_NEW
#endif

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define ACCURACY_POSITION 1.0e-6       //  1 µm

const int CCabinet::gm_nWallPoints[NO_OF_WALLS][NO_OF_RECT_POINTS] =
{
   {3,0,1,2},  // Top
   {0,4,5,1},  // Front
   {2,6,7,3},  // Back
   {3,7,4,0},  // Left
   {1,5,6,2},  // Right
   {4,7,6,5}   // Bottom
};

const int CCabinet::gm_nOppositePoints[NO_OF_WALLS][NO_OF_RECT_POINTS] =
{              // gegenüberliegende Wandpunkte
   {7,4,5,6},  // Top   : (Bottom)
   {3,7,6,2},  // Front : (Back)
   {1,5,4,0},  // Back  : (Front)
   {2,6,5,1},  // Left  : (Right)
   {0,4,7,3},  // Right : (Left)
   {0,3,2,1}   // Bottom: (Top)
};

CCabinet::CCabinet()
{
   m_Cabinets.SetDeleteFunction(DeleteFnc);
   m_Cabinets.SetDestroyMode(true);
   m_pdwWalls = NULL;
   Init();
}

CCabinet::~CCabinet()
{
   if (m_pdwWalls) Free(m_pdwWalls);
}

/******************************************************************************
* Definition : void Init();
* Zweck      : (Re)Initialisierung der Gehäusevariablen
******************************************************************************/
void CCabinet::Init()
{
   dHeight          = 0;
   dWidth           = 0;
   dDepth           = 0;
   dWidth1          = 0;
   dWidth2          = 0;
   dDepth1          = 0;
   dDepth2          = 0;
   nCountAllChassis = 0;
   nCountBR         = 0;
   nCountTL         = 0;
   nCountPM         = 0;
   nCountTT1        = 0;
   nCountTT2        = 0;
   nCountMT1        = 0;
   nCountMT2        = 0;
   nCountMT3        = 0;
   nCountHT         = 0; 
   m_dwFlags        = CAN_CONTAIN_CHASSIS;
   m_nNoOfWallSegments = 0;
   ZeroMemory(m_vCabPoints, sizeof(m_vCabPoints));
   ZeroMemory(m_pdTran, sizeof(m_pdTran));
   ZeroMemory(&vUserRef, sizeof(vUserRef));
   m_Cabinets.Destroy();
}

/******************************************************************************
* Definition : CVector GetCabinetCenter();
* Zweck      : Liefert das Zentrum des Gehäuses
* Funktionswert :                                                   (CVector)
******************************************************************************/
CVector CCabinet::GetCabinetCenter()
{
   return CVector();
}

/******************************************************************************
* Definition : double InitCabPoints();
* Zweck      : Initialisierung der 3D Gehäusepunkte aus den Definitions-
*              variablen Höhe, Breite, Tiefe, ...
* Aufruf     : Immer nach Änderung der Definitionsvariablen !
* Funktionswert : Volumen des Gehäuses in mm^3                     (double)
******************************************************************************/
double CCabinet::InitCabPoints()
{
   int i;   
   if ((dDepth > 0.0) && (dWidth > 0.0) && (dHeight > 0.0))
   {
      CFloatPrecision fp;
      if (m_dwFlags & WIDTH_1_2_ON_BACK)
      {
         double dTemp;
         CVector vDir (0.0, 1.0, 0.0), vNorm;
         m_vCabPoints[0] = CVector(dDepth-dDepth1, 0.0           , dHeight);// links , vorne , oben
         m_vCabPoints[1] = CVector(dDepth-dDepth1, dWidth        , dHeight);// rechts, vorne , oben
         CVector               vP2(       dDepth2, dWidth        , dHeight);// rechts, hinten, oben
         CVector               vP3(       dDepth2,    0.0        , dHeight);// links , hinten, oben

         m_vCabPoints[4] = CVector(dDepth        , 0.0           , 0.0    );// links , vorne , unten
         m_vCabPoints[5] = CVector(dDepth        , dWidth        , 0.0    );// rechts, vorne , unten
         m_vCabPoints[6] = CVector(0.0           , dWidth-dWidth2, 0.0    );// rechts, hinten, unten
         m_vCabPoints[7] = CVector(0.0           ,        dWidth1, 0.0    );// links , hinten, unten

         vNorm = Norm(Product(m_vCabPoints[7] - m_vCabPoints[4], m_vCabPoints[4] - m_vCabPoints[0]));
         dTemp = vNorm*vDir;                                      // Schnittpunkt mit der linken Wand
         if (dTemp)                                               // berechnen
         {
            m_vCabPoints[3] = vP2+vNorm*(m_vCabPoints[7]-vP2)*vDir/dTemp;
         }

         vNorm = Norm(Product(m_vCabPoints[1] - m_vCabPoints[5], m_vCabPoints[5] - m_vCabPoints[6]));
         dTemp = vNorm*vDir;                                      // Schnittpunkt mit der rechten Wand
         if (dTemp)                                               // berechnen
         {
            m_vCabPoints[2] = vP3+vNorm*(m_vCabPoints[1]-vP3)*vDir/dTemp;
         }
      }
      else
      {
         m_vCabPoints[0] = CVector(dDepth-dDepth1,        dWidth1, dHeight);// links , vorne , oben
         m_vCabPoints[1] = CVector(dDepth-dDepth1, dWidth-dWidth2, dHeight);// rechts, vorne , oben
         m_vCabPoints[2] = CVector(       dDepth2, dWidth-dWidth2, dHeight);// rechts, hinten, oben
         m_vCabPoints[3] = CVector(       dDepth2,        dWidth1, dHeight);// links , hinten, oben

         m_vCabPoints[4] = CVector(dDepth        , 0.0           , 0.0    );// links , vorne , unten
         m_vCabPoints[5] = CVector(dDepth        , dWidth        , 0.0    );// rechts, vorne , unten
         m_vCabPoints[6] = CVector(0.0           , dWidth        , 0.0    );// rechts, hinten, unten
         m_vCabPoints[7] = CVector(0.0           , 0.0           , 0.0    );// links , hinten, unten
      }

      m_vShift = CVector(0,0,0);
      for (i=0; i<NO_OF_CABINET_POINTS; i++)
      {
         m_vShift += m_vCabPoints[i];
      }
      m_vShift /= (double)NO_OF_CABINET_POINTS;

      for (i=0; i<NO_OF_CABINET_POINTS; i++)
      {
         m_vCabPoints[i] -= m_vShift;
      }

      if ((dDepth1 == 0.0) && (dDepth2 == 0.0) && (dWidth1 == 0.0) && (dWidth2 == 0.0))
      {
         m_dwFlags &= ~COMPLEX_CAB_DESIGN;
         return dWidth * dDepth * dHeight;
      }
      else
      {
         m_dwFlags |= COMPLEX_CAB_DESIGN;
         return GetVolume();
      }
   }
   return 0.0;
}

/******************************************************************************
* Definition : bool CalcChassisPos3D(ChassisData *);
* Zweck      : Berechnung der 3D Chassisposition aus der 2D Position auf der
*              Schallwand (Höhe, Abstand zur Mittellinie)
* Aufruf     : wenn das Flag (CHASSIS_3D_POS_INVALID) gesetzt ist.
* Parameter (EA): pCH : Chassisdatenobjekt                      (ChassisData*)
* Funktionswert : Position berechnet                            (bool)
******************************************************************************/
bool CCabinet::CalcChassisPos3D(ChassisData *pCH)
{
   if (!(pCH->dwFlagsCH & CHASSIS_3D_POS_INVALID)) return true;
   CFloatPrecision fp;                                         // Genauigkeit erhöhen
   CVector2D vrcWall[NO_OF_RECT_POINTS];
//   int nCorrect = START_VAL;                                   // Wand und Tranformation ermitteln
   if (GetCabinetWall(pCH->nCabWall, vrcWall))
   {
      double dCenterLine = GetXCenterLine(vrcWall);            // Mittelinie der Wand

      CalcInverseTran();                                       // Inverse Transformation berechnen
      CVector vPos(dCenterLine+pCH->dDistFromCenter,           // X-Komponente auf der Wand
                   pCH->dHeightOnCabWall,                      // y-Komponente auf der Wand
                   0.0);                                       // Z-Komponente ist immer 0 (Projection)
      ReTransformVector(vPos);                                 // 3D Position berechnen
      if (pCH->GetCabinet())
      {
         CCabinet *pC = NULL;
         if (IsBaseCabinet()) pC = GetCabinet(pCH->GetCabinet());
         else 
         {
            ASSERT(pC->nCountAllChassis == pCH->GetCabinet());
            pC = this;
         }
         if (pC)
         {
            SetMatrixPtr((Array44*)pC->m_pdTran, false);
            TransformVector(vPos);
            SetMatrixPtr(NULL, false);
         }
      }
      if (pCH->IsSphere())
      {                                                        // Kugeln haben eine Halterung
         CVector vNorm, pvWall3D[NO_OF_RECT_POINTS];
         GetCabinetWall(pCH->nCabWall, NULL, &vNorm, pvWall3D);// Normalenvektor der Wand ermitteln
         if (pCH->m_ppCabinets)                                // Fuß vorhanden ?
         {
            pCH->m_ppCabinets[0]->vPosRef = vPos + (vNorm * (pCH->m_ppCabinets[0]->dHeight*0.5));
            pCH->m_ppCabinets[0]->SetTransformation();
            vPos += vNorm * pCH->m_ppCabinets[0]->dHeight;     // auf den Fuß setzen
         }
         vPos += vNorm * (pCH->dEffDiameter*0.45);             // um halben Durchmesser absetzen
      }
      else if ((pCH->m_ppCabinets != NULL) &&                  // gekoppelte Abstandhalter (4 Stück)
               (pCH->m_ppCabinets[0]->nCountTL == NO_OF_RECT_POINTS))
      {
         CVector   pvWall[NO_OF_RECT_POINTS];                   // mit dem Basis Cabinet
         CCabinet *pCBase = (CCabinet*)pCH->m_ppCabinets[0]->m_Cabinets.GetFirst();
         CCabinet *pCfoot = NULL;
         pCBase->GetCabinetWall(pCH->nCabWall, NULL, NULL, pvWall);// die entsprechende Bodenplatte ermitteln
         for (int i=0; i<NO_OF_RECT_POINTS; i++)
         {  
            pCfoot =  pCH->m_ppCabinets[i];                    // und die Füße
            CVector vShift = pCfoot->GetCabPoint(pCfoot->GetOppositePoint(pCH->GetWall(), i));
            pCfoot->vPosRef = pvWall[i]-vShift;                // an den Eckpunkten ausrichten
            pCfoot->SetTransformation();
         }
      }
      pCH->vPosChassis = vPos;
      pCH->dwFlagsCH  &= ~CHASSIS_3D_POS_INVALID;
      return true;
   }
   return false;
}

/******************************************************************************
* Definition : bool CalcChassisPos2D(ChassisData*);
* Zweck      : Berechnung der 2D Position auf der Schallwand aus der gegebenen
*              3D Position im Gehäuse
* Aufruf     : wenn das Flag (CHASSIS_2D_POS_INVALID) gesetzt ist.
* Parameter (EA): pCH : Chassisdatenobjekt                      (ChassisData*)
* Funktionswert : Position berechnet                            (bool)
******************************************************************************/
bool CCabinet::CalcChassisPos2D(ChassisData *pCH)
{
   if (!(pCH->dwFlagsCH & CHASSIS_2D_POS_INVALID)) return true;
   CFloatPrecision fp;                                         // Genauigkeit erhöhen
   CVector2D vrcWall[NO_OF_RECT_POINTS];                       // Wand und Tranformation ermitteln
   if ((pCH->vPosChassis != CVector(0.0,0.0,0.0)) && GetCabinetWall(pCH->nCabWall, vrcWall))
   {
      double dCenterLine = GetXCenterLine(vrcWall);            // Mittellinie der Wand
      CVector vPos = pCH->vPosChassis;                         // 3D Position in die 2D Position
      if (pCH->GetCabinet())
      {
         CCabinet *pC = GetCabinet(pCH->GetCabinet());
         if (pC)
         {
            Array44 mReTran;
            SetMatrixPtr((Array44*)pC->m_pdTran, false);
            SetReMatrixPtr((Array44*)&mReTran, true);
            ReTransformVector(vPos);
            SetMatrixPtr(NULL, false);
            SetReMatrixPtr(NULL, false);
         }
      }
      TransformVector(vPos);                                   // auf der Wand transformieren
      pCH->dDistFromCenter  = Vx(vPos)-dCenterLine;            // Abstand von der Mittellinie berechnen
      pCH->dHeightOnCabWall = Vy(vPos);                        // Höhe auf der Wand berechnen
                                                               // Z-Komponente ist unwichtig (Projektion auf die X-Y-Ebene)
      pCH->dwFlagsCH  &= ~CHASSIS_2D_POS_INVALID;
      return true;
   }
   return false;
}

/******************************************************************************
* Definition : void AdaptCoupledPosition(ChassisData*);
* Zweck      : Anpassen der Position des gekoppelten Dipol Chassis auf der
*              Rückwand durch Projektion in parallel zur Z-Ebene.
* Aufruf     : Nach Änderung der Position des Dipol-Master-Chassis
* Parameter (EA): pCH : Chassisdatenobjekt                      (ChassisData*)
******************************************************************************/
void CCabinet::AdaptCoupledPosition(ChassisData*pCD)
{
   ChassisData * pCDC = pCD->GetCoupled(0);
   if (pCDC)
   {
      if (pCDC->nCabWall == 0)                                 // Wand setzen falls 0
      {
         if (pCDC->IsDummy())                                  // Der Dummy ist hinten
         {
            pCD->AdaptCoupledSize();
            pCDC->nCabWall = CARABOX_CABWALL_BACK;
         }
         else pCDC->nCabWall = CARABOX_CABWALL_FRONT;
      }
      if (pCD->GetCabinet() != pCDC->GetCabinet())
      {
         pCDC->SetCabinet(pCD->GetCabinet()); 
      }
      CalcChassisPos3D(pCD);                                   // 3D Position ermitteln
      CVector vrcWall1[NO_OF_RECT_POINTS], vrcWall2[NO_OF_RECT_POINTS];
      CVector vNorm1, vNorm2;
      GetCabinetWall(pCD->nCabWall , NULL, &vNorm1);           // Normalenvektoren und Wandpunkte
      GetCabinetWall(pCDC->nCabWall, NULL, &vNorm2, vrcWall2); // für die Ebenengleichungen holen
      vNorm1.SetZ() = 0;                                       // Z-Komponente muß 0 sein, da die Höhe gleich ist
      vNorm1 = Norm(vNorm1);                                   // normieren
      double dTemp = vNorm2*vNorm1;                            // Schnittpunkt mit der gegenüberliegenden Wand
      if (dTemp)                                               // berechnen
      {
         pCDC->vPosChassis = pCD->vPosChassis+vNorm2*(vrcWall2[0]-pCD->vPosChassis)*vNorm1/dTemp;
         pCDC->dwFlagsCH  |= CHASSIS_2D_POS_INVALID;
         CalcChassisPos2D(pCDC);                               // 2D Koordinaten ermitteln
      }
   }
}

/*******************************************************************************
* Definition : bool GetCabinetWall(int, CVector2D*, CVector*, CVector *);
* Zweck      : a: Berechnung der 2D Schallwandkoordinaten ((X, Y); Z=0)
*                 Berechnung der Transformationsmatrix 3D -> 2D. Für die
*                 Rücktransformaiton muß zuerst die Fkt. CalcInverseTran()
*                 gerufen werden!
*              b: Rückgabe der 3D-Koordinaten der Schallwand und des Flächen-
*                 Normalenvektors
*              Die Parameter in den eckigen Klammern sind optional und steuern
*              die Fälle (a: pvWall2D) und (b: pvWall3D, pvNorm)
* Parameter (E): nCabWall : Schallwand- und gehäusenummer               (int)
* LOWORD: (CARABOX_CABWALL_TOP, CARABOX_CABWALL_FRONT, CARABOX_CABWALL_BACK,
*          CARABOX_CABWALL_LEFT, CARABOX_CABWALL_RIGHT, CARABOX_CABWALL_BOTTOM)
* HIWORD: (0): Basisgehäuse, (1,..,n) weitere Gehäusesegmente
*           (A): [pvWall2D] : Array mit 2D-Koordinaten der Schallwand (CVector2D*)
*           (A): [pvNorm]   : Normalenvektor der Schallwand (3D)      (CVector*)
*           (A): [pvWall3D] : Array mit 3D-Koordinaten der Schallwand (CVector*)
*                Die Arrays haben die Dimension (NO_OF_RECT_POINTS) !
* Funktionswert : Koordinaten Ok!
*******************************************************************************/
bool CCabinet::GetCabinetWall(int nCabWall, CVector2D *pvWall2D, CVector *pvNorm, CVector *pvWall3D)
{
   BEGIN("GetCabinetWall");
   CFloatPrecision fp;
   int i, *n;
   int nCabinet = HIWORD(nCabWall);
   CVector vTransition, vWallPoint, vNorm;
   double  dCos = 0.0, dPhix = 0.0, dPhiy = 0.0, dPhiz = 0.0;
   const static double d90deg = 90.0*M_PI_D_180;

   nCabWall = LOWORD(nCabWall);
   if (nCabinet > 0)
   {
      CCabinet *pC = GetCabinet(nCabinet);
      if (pC) return pC->GetCabinetWall(nCabWall, pvWall2D, pvNorm, pvWall3D); 
   }

   if (nCabWall < 1)                                           // zur Sicherheit
   {
      if (pvNorm) *pvNorm = CVector();
      return false;
   }
   n = (int*)gm_nWallPoints[nCabWall-1];
                                                               // Normalenwector der Wand ermitteln
   if (pvNorm || pvWall3D)                                     // Normalenvektor der Wand zurückgeben
   {
      if (pvNorm) vNorm = Norm(Product(m_vCabPoints[n[0]] - m_vCabPoints[n[1]], m_vCabPoints[n[1]] - m_vCabPoints[n[2]]));
      bool bTransform = false;
      Array44 Tran;
      if (!IsBaseCabinet())
      {
         memcpy(&Tran, m_pdTran, sizeof(Array44));
         SetMatrixPtr((Array44*)&Tran, false);
         bTransform = true;
      }
      if (pvWall3D)                                            // Wandkoordinaten in 3D zurückgeben
      {
         for (i=0; i<NO_OF_RECT_POINTS; i++)
         {
            pvWall3D[i] = m_vCabPoints[n[i]];
            if (bTransform) TransformVector(pvWall3D[i]);
         }
      }
      if (bTransform && ((Vy(vUserRef) != 0.0) || (Vz(vUserRef) != 0.0)))
      {
         Tran.a.m14 = 0;
         Tran.a.m24 = 0;
         Tran.a.m34 = 0;
         TransformVector(vNorm);
      }
      if (pvNorm) *pvNorm = vNorm;
      if (bTransform) SetMatrixPtr(NULL, false);
      ASSERT(pvWall2D == NULL);                                // Achtung !! (kein echter Fehler)
                                                               // Nur ein Hinweis, daß die 2D-Koordinaten
      return true;                                             // hier nicht mehr ermittelt werden.
   }

   vNorm = Norm(Product(m_vCabPoints[n[0]] - m_vCabPoints[n[1]], m_vCabPoints[n[1]] - m_vCabPoints[n[2]]));

   if ((nCabWall!=CARABOX_CABWALL_BOTTOM)||(nCabWall!=CARABOX_CABWALL_TOP))
   {
      dCos  = Cosinus(vNorm, CVector(0.0, 0.0, 1.0));          // Neigungswinkel der Seitenwände ermitteln
      if (_isnan(dCos)) dCos = 0.0;
   }

   ASSERT(pvWall2D != NULL);                                   // Fehler 2D-Koordinate werden hier ermittelt

   switch (nCabWall)                   // Transformationen für die Wände ermitteln
   {
      case CARABOX_CABWALL_TOP:
         dPhiz = -d90deg;              // Box um 90° im Uhrzeigersinn um die Z-Achse drehen
         break;
      case CARABOX_CABWALL_FRONT:
         dPhiz = -d90deg;              // Box um 90° im Uhrzeigersinn um die Z-Achse drehen
         dPhix = -acos(dCos);          // Box um - Theta1° im Uhrzeigersinn um die X-Achse drehen
         break;
      case CARABOX_CABWALL_BACK:
         dPhiz = d90deg;               // Box um  90° gegen den Uhrzeigersinn um die Z-Achse drehen
         dPhix = -d90deg + asin(dCos); // Box um -90° + Theta2 um die X-Achse drehen
         break;
      case CARABOX_CABWALL_LEFT:
         dPhix = -acos(dCos);          // Box um -Theta1 um die X-Achse drehen
         dCos  = Cosinus(vNorm, CVector(1.0, 0.0, 0.0));
         dPhiz = acos(dCos)-d90deg;    // Box um Theta3 im Uhrzeigersinn um die Z-Achse drehen
         break;
      case CARABOX_CABWALL_RIGHT:
         dPhix = -d90deg + asin(dCos); // Box um -90° +Theta2 um die X-Achse drehen
         dCos  = Cosinus(vNorm, CVector(1.0, 0.0, 0.0));
         dPhiz = 3*d90deg-acos(dCos);  // Box um 270° -Theta3 um die Z-Achse drehen
         break;
      case CARABOX_CABWALL_BOTTOM:
         dPhiz = d90deg;               // Box um  90° gegen den Uhrzeigersinn um die Z-Achse drehen
         dPhiy = 2*d90deg;             // Box um 180° gegen den Uhrzeigersinn um die X-Achse drehen
         break;
      default : return false;
   }


   vTransition  = -m_vCabPoints[n[1]];                         // Verschiebungsvektor

   LoadIdentity();                                             // Einheitsmatrix setzen

   if (dPhix != 0.0) Rotate_x(dPhix);
   if (dPhiy != 0.0) Rotate_y(dPhiy);
   if (dPhiz != 0.0) Rotate_z(dPhiz);
   Translate(vTransition);
   for (i=0; i<NO_OF_RECT_POINTS; i++)
   {
      vWallPoint = m_vCabPoints[n[i]];                         // Boxen definitionspunkt
      TransformVector(vWallPoint);                             // in 2D umrechnen
//      ASSERT(fabs(Vz(vWallPoint))<1e-11);
      pvWall2D[i].SetX() = Vx(vWallPoint);                     // X-Koordinate
      pvWall2D[i].SetY() = Vy(vWallPoint);                     // Y-Koordinate
   }

   return true;
}

/*********************************************************************************
* Definition : static CVector2D Intersection(CVector2D&,CVector2D&,
*                                            CVector2D&,CVector2D&)
* Zweck      : Berechnung des Schnittpunktes zweier Geraden in 2D
* Aufruf     : Intersection(vr1, va1, vr2, va2);
* Parameter (E): vr1, vr2: Ortvektoren der beiden Geraden             (CVector2D&)
*           (E): va1, va2: Richtungsvektoren der beiden Geraden       (CVector2D&)
* Funktionswert : Schnittpunkt der beiden Graden                      (CVector2D&)
*********************************************************************************/
CVector2D CCabinet::Intersection(CVector2D &vr1, CVector2D &va1, CVector2D &vr2, CVector2D &va2)
{
   double det = Vx(va1)*Vy(va2) - Vy(va1)*Vx(va2);             // Schnittpunkt zweier Geraden in 2D
   if (fabs(det) > 1e-13)
   {
      CVector2D  vc = vr2-vr1;
      double    dL1 = (Vx(vc)*Vy(va2) - Vy(vc)*Vx(va2)) / det;
      return vr1 + dL1 * va1;
   }
   return CVector2D();
}

/******************************************************************************
* Definition : static double GetXCenterLine(CVector2D *, int nPos=1);
* Zweck      : liefert die Position der Mittelinie bezogen auf:
*              (nPos= 1) die Oberkante (Standard)
*              (nPos=-1) die Unterkante
*              (nPos= 0) die horizontale Mittellinie
* Parameter (E): prcWall: Die Schallwandkorrdinaten            (CVector2D*)
*           (E): [nPos] : Bezugskante der Mittellinie          (int)
* Funktionswert : Position der Mittellinie                     (double)
******************************************************************************/
double CCabinet::GetXCenterLine(CVector2D *prcWall, int nPos)
{
   if       (nPos>0) return (Vx(prcWall[0]) + Vx(prcWall[3])) * 0.5;
   else  if (nPos<0) return (Vx(prcWall[1]) + Vx(prcWall[2])) * 0.5;
   else  return (Vx(prcWall[0]) + Vx(prcWall[1]) + Vx(prcWall[2]) + Vx(prcWall[3])) * 0.25;
}

/******************************************************************************
* Definition : PointFloat *GetCabinetPolyPoints(int&);
* Zweck      : liefert die Definitionspunkte des Gehäuses
* Aufruf     : Nur durch das Basisgehäuseobjekt !
* Parameter  : nCountBoxPt: Anzahl der Gehäusepunkte                    (int&)
* Funktionswert : Array mit den Gehäusepunkten                   (PointFloat*)
******************************************************************************/
PointFloat * CCabinet::GetCabinetPolyPoints(int& nCountBoxPt)
{
   int i, j, k, nCabinets;

   ASSERT(IsBaseCabinet());
   nCabinets = m_Cabinets.GetCounter()+1;

   nCountBoxPt += (NO_OF_CABINET_POINTS * nCabinets);
   PointFloat *psBox_CALE  = new PointFloat[nCountBoxPt];

   CCabinet *pC = this;
   if (psBox_CALE)
   {
      i=0;
      for (j=0; j<nCabinets; j++)
      {
         for (k=0; k<NO_OF_CABINET_POINTS; k++, i++)
         {
            psBox_CALE[i] = VectorToBDDpoint(pC->m_vCabPoints[k], 0.001, j);
         }
         if (pC == this) pC = (CCabinet*)m_Cabinets.GetFirst();
         else            pC = (CCabinet*)m_Cabinets.GetNext();
      }
   }
   return psBox_CALE;
}

/******************************************************************************
* Definition : bool GetCabinetPolygons(int&, int ***, int **);
* Zweck      : liefert die Gehäusepolygone als Indizes bezogen auf das Array
*              mit den Gehäusepunkten.
* Aufruf     : Nur durch das Basisgehäuseobjekt !
* Parameter (EA): nCountBoxPoly: Anzahl der Gehäusepolygone             (int)
*           (EA): pppnIndexBoxPolyPt: Indizes der Polygonpunkte         (int***)
*           (EA): ppnCountBoxPolyPt: Anzahl der Polygonpunkte           (int**)
* Funktionswert : Polygonarrays OK                                      (bool)
******************************************************************************/
bool CCabinet::GetCabinetPolygons(int &nCountBoxPoly, int ***pppnIndexBoxPolyPt , int **ppnCountBoxPolyPt)
{
   bool      bReturn  = false;
   int       i, j, k, nBoxPoly = 0;
   CCabinet *pC;

   ASSERT(IsBaseCabinet());

   for (i=0; i<NO_OF_WALLS; i++)                               // Polygone zählen
      if (IsWallEnabled(i+1)) nBoxPoly++;

   pC = (CCabinet*)m_Cabinets.GetFirst();
   while (pC)
   {
      for (i=0; i<NO_OF_WALLS; i++)
         if (pC->IsWallEnabled(i+1)) nBoxPoly++;
      pC = (CCabinet*)m_Cabinets.GetNext();
   }

   nCountBoxPoly += nBoxPoly;
   int  *pnCountBoxPolyPt  = new int[nCountBoxPoly];
   int **ppnIndexBoxPolyPt = new int*[nCountBoxPoly];
   if ((pnCountBoxPolyPt != NULL) && (ppnIndexBoxPolyPt != NULL))
   {
      pC = this;
      i  = 0;
      nBoxPoly = 0;
      while (pC)
      {
         bReturn = true;
         for(k=0; k<NO_OF_WALLS; k++)
         {
            if (pC->IsWallEnabled(k+1))
            {
               pnCountBoxPolyPt[i]  = 4;                          // 4 Punkte pro Gehäuseseite, Rechteck
               ppnIndexBoxPolyPt[i] = new int[pnCountBoxPolyPt[i]];
               if (ppnIndexBoxPolyPt[i] != NULL)
               {
                  for (j=0; j<NO_OF_RECT_POINTS;j++)
                  {
                     ppnIndexBoxPolyPt[i][j] = pC->gm_nWallPoints[k][j]+nBoxPoly;
                  }
               }
               else bReturn = false;
               i++;
            }
         }
         nBoxPoly += NO_OF_CABINET_POINTS;
         if (pC == this) pC = (CCabinet*)m_Cabinets.GetFirst();
         else            pC = (CCabinet*)m_Cabinets.GetNext();
      }
   }
   *pppnIndexBoxPolyPt = ppnIndexBoxPolyPt;
   *ppnCountBoxPolyPt  = pnCountBoxPolyPt;
   return bReturn;
}

/******************************************************************************
* Definition : static double Distance(CVector2D, CVector2D, CVector2D);
* Zweck      : Berechnung des Abstandes eines Punktes zu einer Geraden (2D)
* Parameter (E): vR1, vR2: Punkte der Geraden                     (CVector2D)
*           (E): vQ      : Testpunkt                              (CVector2D)
* Funktionswert : Abstand des Punkte                              (double)
*                 (Achtung: der Abstand kann auch negativ sein !)
******************************************************************************/
double CCabinet::Distance(CVector2D vR1, CVector2D vR2, CVector2D vQ)
{
   CVector2D vA = vR2-vR1;
   return (Vx(vA)*(Vy(vQ)-Vy(vR1)) - Vy(vA)*(Vx(vQ)-Vx(vR1))) / Betrag(vA);
}

/******************************************************************************
* Definition : static bool IsOnWall(CVector2D *, ChassisData *);
* Zweck      : Testet, ob ein Chassis auf eine Schallwand passt.
* Parameter (E): pvWall2D : 2D Koordinaten der Schallwand          (CVector2D*)
*          (EA): pCH : Chassisdatenobjekt                        (ChassisData*)
*          (EA): [pvQ]  : Chassismittelpunkt zur Übergabe          (CVector2D*)
*           (E): [bInit]: true: Chassismittelpunkt initialisieren        (bool)
*                         false: Chassismittelpunkt verwenden
* Funktionswert : Chassis passt volständig auf die Schallwand            (bool)
******************************************************************************/
bool CCabinet::IsOnWall(CVector2D *pvWall2D, ChassisData *pCD, CVector2D *pvQ, bool bInit)
{
   CVector2D vQ(pCD->dDistFromCenter + GetXCenterLine(pvWall2D), pCD->dHeightOnCabWall);
   if (pvQ)
   {
      if (bInit) *pvQ = vQ;
      else        vQ  = *pvQ;
   }
   if (pCD->IsRect())
   {
      int i, j, k;//, nHit = 0;
      double dHeight2 = pCD->dEffHoehe  * 0.5;
      double dWidth2  = pCD->dEffBreite * 0.5;
      double dDistance;
      CVector2D vQR[4];
      vQR[0] = vQ + CVector2D(-dWidth2,  dHeight2);
      vQR[1] = vQ + CVector2D(-dWidth2, -dHeight2);
      vQR[2] = vQ + CVector2D( dWidth2, -dHeight2);
      vQR[3] = vQ + CVector2D( dWidth2,  dHeight2);
      for (i=0; i<NO_OF_RECT_POINTS; i++)
      {
         j = i+1;                                              // Seitenindex einstellen
         if (j>=NO_OF_RECT_POINTS) j=0;
         for (k=0; k<4; k++)                                   // alle Punkte des Rechtecks prüfen
         {
            dDistance = Distance(pvWall2D[i], pvWall2D[j], vQR[k]);
//            if (dDistance < 0.0)
            if (dDistance < -ACCURACY_POSITION)
            {
               pCD->dwFlagsCH &= ~CHASSIS_POS_VALID;
               return false;
            }
         }
      }
   }
   else if (pCD->GetMembrTyp())
   {
      int i, j;//, nHit = 0;
      double dRadius = pCD->dEffDiameter * 0.5;
      double dDistance;
      if (pCD->IsSphere()) dRadius = 5;
      dRadius += ACCURACY_POSITION;
      for (i=0; i<NO_OF_RECT_POINTS; i++)
      {
         j = i+1;
         if (j>=NO_OF_RECT_POINTS) j=0;
         dDistance = Distance(pvWall2D[i], pvWall2D[j], vQ);
//         if (dDistance < (dRadius-MIN_ACC_DOUBLE))
         if (dDistance < dRadius)
         {
            pCD->dwFlagsCH &= ~CHASSIS_POS_VALID;
            return false;
         }
      }
   }
   pCD->dwFlagsCH |=  CHASSIS_POS_VALID;
   pCD->dwFlagsCH &= ~CHASSIS_POS_ON_2_WALLS;
   return true;
}

/******************************************************************************
* Definition : static CVector2D GetWallCenter(CVector2D*);
* Zweck      : liefert das Zentrum der 2D Schallwand
* Parameter (E): pvWall2D : 2D Koordinaten der Schallwand         (CVector2D*)
* Funktionswert : Zentrum der 2D Schallwand                       (CVector2D)
******************************************************************************/
CVector2D CCabinet::GetWallCenter(CVector2D *pvWall2D)
{
   int i;
   CVector2D vCenter;
   for (i=0; i<NO_OF_RECT_POINTS; i++)
   {
      vCenter += pvWall2D[i];
   }
   return vCenter / (double) NO_OF_RECT_POINTS;
}

/******************************************************************************
* Definition : static bool InitChassisPos(CVector2D *, ChassisData *, [bool]);
* Zweck      : Initialisiert die Chassisposition auf der Schallwand
* Parameter (E): pvWall2D : 2D Koordinaten der Schallwand         (CVector2D*)
*          (EA): pCH : Chassisdatenobjekt                       (ChassisData*)
*           (E): bTest: nur testen !
* Funktionswert : Chassis passt auf die Wand                            (bool)
******************************************************************************/
bool CCabinet::InitChassisPos(CVector2D *pvWall2D, ChassisData *pCD, bool bTest)
{
   CVector2D vrcRange[NO_OF_RECT_POINTS];
   CVector2D vP1, vP2, vA1, vA2;
   double dRadius[NO_OF_RECT_POINTS];
   double dArea = GetWallArea(pvWall2D);
   
   if (!bTest) pCD->dwFlagsCH &= ~CHASSIS_POS_VALID;
   if (pCD->IsRect())
   {
      double dXL  = Vx(pvWall2D[0]) - Vx(pvWall2D[1]),           // Seitenlage links
             dXR  = Vx(pvWall2D[3]) - Vx(pvWall2D[2]);           // Seitenlage rechts
      double dChassisArea = pCD->dEffBreite * pCD->dEffHoehe;
      if (dChassisArea > dArea) return false;
      CVector2D vX;
      if (dXL > 0.0) vX = CVector2D(-pCD->dEffBreite*0.5,  pCD->dEffHoehe *0.5);
      else           vX = CVector2D(-pCD->dEffBreite*0.5, -pCD->dEffHoehe *0.5);
      dRadius[0] = Distance(pvWall2D[0], pvWall2D[1], pvWall2D[0]-vX) - ACCURACY_POSITION;

      if (dXR > 0.0) vX = CVector2D( pCD->dEffBreite*0.5, -pCD->dEffHoehe *0.5);
      else           vX = CVector2D( pCD->dEffBreite*0.5,  pCD->dEffHoehe *0.5);
      dRadius[2] = Distance(pvWall2D[2], pvWall2D[3], pvWall2D[2]-vX) - ACCURACY_POSITION;

      dRadius[1]= dRadius[3] = (pCD->dEffHoehe *0.5) - ACCURACY_POSITION;
   }
   else
   {
      double dChassisArea = pCD->dEffDiameter * 0.5;
      dChassisArea = dChassisArea * dChassisArea * M_PI;
      if (dChassisArea > dArea) return false;
      dRadius[0] = dRadius[1] = dRadius[2] = dRadius[3] = (pCD->dEffDiameter*0.5) - ACCURACY_POSITION;
//      REPORT("%f,%f,%f,%f", dRadius[0], dRadius[1], dRadius[2], dRadius[3]);
   }
   
   int i=0, j=1, k;
   vP1 = pvWall2D[i] + dRadius[i]*Norm(Ortho(pvWall2D[i], pvWall2D[j]));
   vA1 = pvWall2D[j] - pvWall2D[i];
   for (i=0; i<NO_OF_RECT_POINTS; i++)
   {
      j=i+1;
      if (j>=NO_OF_RECT_POINTS) j=0;
      k=j+1;
      if (k>=NO_OF_RECT_POINTS) k=0;
      vP2 = pvWall2D[j] + dRadius[j]*Norm(Ortho(pvWall2D[j], pvWall2D[k]));
      vA2 = pvWall2D[k] - pvWall2D[j];
      vrcRange[j] = Intersection(vP1, vA1, vP2, vA2);
      vP1 = vP2;
      vA1 = vA2;
   }
   if      ((Vx(vrcRange[3]) < Vx(vrcRange[0])) && (Vx(vrcRange[2]) > Vx(vrcRange[1])))
   {
      vrcRange[0] = vrcRange[3] = Intersection(vrcRange[1], vrcRange[0]-vrcRange[1], vrcRange[2], vrcRange[3]-vrcRange[2]);
   }
   else if ((Vx(vrcRange[2]) < Vx(vrcRange[1])) && (Vx(vrcRange[3]) > Vx(vrcRange[0])))
   {
      vrcRange[1] = vrcRange[2] = Intersection(vrcRange[0], vrcRange[1]-vrcRange[0], vrcRange[3], vrcRange[2]-vrcRange[3]);
   }

   dArea = GetWallArea(vrcRange);
   if (dArea > 0.0) // GetWallArea(vrcRange) > 0.0)
   {
      if (bTest) return true;
      CVector2D vCenter;
      double dCenter = GetXCenterLine(pvWall2D);
      if ((pCD->dHeightOnCabWall == 0.0) && (pCD->dDistFromCenter == 0.0))
      {
         vCenter = GetWallCenter(vrcRange);
      }
      else
      {
         double dDist, dMinDist = 100000.0;
         int nWallPt = 0;
         vCenter = CVector2D(pCD->dDistFromCenter + dCenter, pCD->dHeightOnCabWall);
         for (i=0; i<NO_OF_RECT_POINTS; i++)
         {
            dDist = Betrag(vCenter-vrcRange[i]);
            if (dMinDist > dDist) dMinDist = dDist, nWallPt = i;
         }
         vCenter = vrcRange[nWallPt];
      }

      pCD->SetHeightOnCabWall() = Vy(vCenter);
      pCD->SetDistFromCenter()  = Vx(vCenter) - dCenter;
      pCD->dwFlagsCH |=  CHASSIS_POS_VALID;
      return true;
   }
   return false;
}

/******************************************************************************
* Definition : static double GetWallArea(CVector2D *);
* Zweck      : Berechnung der Fläche der 2D-Schallwand
* Parameter (E): pvWall2D : 2D Koordinaten der Schallwand         (CVector2D*)
* Funktionswert : Fläche in mm^2                                      (double)
******************************************************************************/
double CCabinet::GetWallArea(CVector2D *pvWall2D)
{
   double dH  = Vy(pvWall2D[3]) - Vy(pvWall2D[2]),
          dG1 = Vx(pvWall2D[3]) - Vx(pvWall2D[0]),
          dG2 = Vx(pvWall2D[2]) - Vx(pvWall2D[1]);
   if (dH  < 0.0) return -1.0;
   if (dG1 < 0.0) return -1.0;
   if (dG2 < 0.0) return -1.0;
   return dH * (dG1 + dG2) * 0.5;
}

/******************************************************************************
* Definition : double GetVolume(double dThickness=0.0)
* Zweck      : Berechnung des Volumens des Gehäuses
*              Das Basisgehäuse liefert die Summe aller Volumina.
*              Die zusätzlichen Gehäusesegmente liefern nur Ihr eigenes
*              Volumen.
* Parameter (E): [dThickness] Dicke der Schallwände in mm             (double)
* Funktionswert : Volumen des Gehäuses in mm^3                        (double)
******************************************************************************/
double CCabinet::GetVolume(double dThickness)
{
   double    dVolume = 0.0;
/*
   // Alte Formel gilt nur für Pyramidenstümpfe
   dVolume = dWidth*dDepth*dHeight - 
             dHeight*(dWidth*(dDepth1+dDepth2)+dDepth*(dWidth1+dWidth2))*0.5+
             dHeight*(dWidth1+dWidth2)*(dDepth1+dDepth2)/3.0;
*/
   CVector *P = (CVector*)m_vCabPoints;                        // Die Flächen oben und unten sind parallel
   double dAbottom,
   dS1   = Vy(P[2])-Vy(P[3])-dThickness*2.0,                   // Grundseite 1
   dS2   = Vy(P[1])-Vy(P[0])-dThickness*2.0,                   // Grundseite 2
   dH    = Vx(P[0])-Vx(P[3])-dThickness*2.0,                   // Höhe
   dAtop = (dS1+dS2)*dH*0.5;                                   // Trapezfläche oben
   if (dAtop < 0.0) dAtop = 0.0;
          
   dS1   = Vy(P[6])-Vy(P[7])-dThickness*2.0;                   // Grundseite 1
   dS2   = Vy(P[5])-Vy(P[4])-dThickness*2.0;                   // Grundseite 2
   dH    = Vx(P[4])-Vx(P[7])-dThickness*2.0;                   // Höhe
   dAbottom = (dS1+dS2)*dH*0.5;                                // Trapezfläche unten

   dVolume = (dHeight-dThickness*2.0) * (dAtop + dAbottom + sqrt(dAtop*dAbottom)) / 3.0;

   if (IsBaseCabinet())
   {
      CCabinet *pC =(CCabinet*) m_Cabinets.GetFirst();
      while (pC)
      {
         ASSERT(!pC->IsBaseCabinet());
         dVolume += pC->GetVolume();
         pC =(CCabinet*) m_Cabinets.GetNext();
      }
   }
   return dVolume;
}

/******************************************************************************
* Definition : void CountAllChassis()
* Zweck      : Berechnet die Anzahl der vorhandenen Chassis im Gehäuse
* Aufruf     : Nur durch das Basisgehäuseobjekt !
******************************************************************************/
void CCabinet::CountAllChassis()
{
   ASSERT(IsBaseCabinet());
   nCountAllChassis = 0;
   int * pChassis = &nCountBR;
   for (int i=0; i<CARABOX_CHASSISTYPES; i++)
      nCountAllChassis += pChassis[i];
}

/******************************************************************************
* Definition : void ReadData(CBoxPropertySheet *);
* Zweck      : ließt die Gehäusedaten aus einer Datei.
* Aufruf     : Direkt nur durch das Basisgehäuseobjekt !
* Parameter (EA): pps : CBoxPropertySheet-Objekt          (CBoxPropertySheet*)
*                 beinhaltet die Dateifunktionen und Variablen
******************************************************************************/
void CCabinet::ReadData(CBoxPropertySheet *pps)
{
   int   nVersion = pps->GetFileVersion();
   DWORD dwDummy;
   pps->ReadFileFH(&dHeight, sizeof(double));
   pps->ReadFileFH(&dWidth, sizeof(double));
   pps->ReadFileFH(&dDepth, sizeof(double));
   if (nVersion >= 110)
   {
      pps->ReadFileFH(&dWidth1, sizeof(double));
      pps->ReadFileFH(&dWidth2, sizeof(double));
      pps->ReadFileFH(&dDepth1, sizeof(double));
      pps->ReadFileFH(&dDepth2, sizeof(double));
   }
   pps->ReadFileFH(&nCountAllChassis, sizeof(int));
   pps->ReadFileFH(&nCountBR, sizeof(int));
   pps->ReadFileFH(&nCountTL, sizeof(int));
   if (nVersion >= 110) pps->ReadFileFH(&nCountPM, sizeof(int));
   else                 nCountPM = 0;                          // Passisvmembranen ab 1.10
   pps->ReadFileFH(&nCountTT1, sizeof(int));
   pps->ReadFileFH(&nCountTT2, sizeof(int));
   pps->ReadFileFH(&nCountMT1, sizeof(int));
   pps->ReadFileFH(&nCountMT2, sizeof(int));
   pps->ReadFileFH(&nCountMT3, sizeof(int));
   pps->ReadFileFH(&nCountHT, sizeof(int));
   if (nVersion == 100) pps->ReadFileFH(&dwDummy, sizeof(DWORD));// Strukturalignment nötig bei 1.00
   pps->ReadFileFH(&vPosRef, sizeof(CVector));
   if (nVersion >= 220)
   {
      int i, nCount = 0;
      CCabinet *pC = NULL;
      pps->ReadFileFH(&m_dwFlags, sizeof(DWORD));
      if (IsBaseCabinet())
      {
         pps->ReadFileFH(&nCount, sizeof(int));
         for (i=0; i<nCount; i++)
         {
            pC = new CCabinet;
            pC->ReadData(pps);
            pC->SetBaseCabinet(this);
         }
      }
      else
      {
         pps->ReadFileFH(&vUserRef, sizeof(CVector));
         pps->ReadFileFH(m_szName, CAB_NAME_LEN);
      }
      InitCabPoints();
   }
}

/******************************************************************************
* Definition : void WriteData(CBoxPropertySheet *);
* Zweck      : Schreibt die Gehäusedaten in eine Datei.
* Aufruf     : Direkt nur durch das Basisgehäuseobjekt !
* Parameter (EA): pps : CBoxPropertySheet-Objekt          (CBoxPropertySheet*)
*                 beinhaltet die Dateifunktionen und Variablen
******************************************************************************/
void CCabinet::WriteData(CBoxPropertySheet *pps)
{
   int nVersion = pps->GetFileVersion();
   pps->WriteFileFH(&dHeight, sizeof(double));
   pps->WriteFileFH(&dWidth, sizeof(double));
   pps->WriteFileFH(&dDepth, sizeof(double));
   if (nVersion >= 110)
   {
      pps->WriteFileFH(&dWidth1, sizeof(double));
      pps->WriteFileFH(&dWidth2, sizeof(double));
      pps->WriteFileFH(&dDepth1, sizeof(double));
      pps->WriteFileFH(&dDepth2, sizeof(double));
   }
   pps->WriteFileFH(&nCountAllChassis, sizeof(int));
   pps->WriteFileFH(&nCountBR, sizeof(int));
   pps->WriteFileFH(&nCountTL, sizeof(int));
   if (nVersion >= 110) pps->WriteFileFH(&nCountPM, sizeof(int));
   pps->WriteFileFH(&nCountTT1, sizeof(int));
   pps->WriteFileFH(&nCountTT2, sizeof(int));
   pps->WriteFileFH(&nCountMT1, sizeof(int));
   pps->WriteFileFH(&nCountMT2, sizeof(int));
   pps->WriteFileFH(&nCountMT3, sizeof(int));
   pps->WriteFileFH(&nCountHT, sizeof(int));
   if (nVersion == 100) pps->WriteFileFH(&nVersion, sizeof(int));
   pps->WriteFileFH(&vPosRef, sizeof(CVector));
   if (nVersion >= 220)
   {
      pps->WriteFileFH(&m_dwFlags, sizeof(DWORD));
      if (IsBaseCabinet())
      {
         int nCount = m_Cabinets.GetCounter();
         pps->WriteFileFH(&nCount, sizeof(int));
         CCabinet *pC = (CCabinet*)m_Cabinets.GetFirst();
         while (pC)
         {
            pC->WriteData(pps);
            pC = (CCabinet*)m_Cabinets.GetNext();
         }
      }
      else 
      {
         pps->WriteFileFH(&vUserRef, sizeof(CVector));
         pps->WriteFileFH(m_szName, CAB_NAME_LEN);
      }
   }
}

/*****************************************************************************************
* Definition : PointFloat VectorToBDDpoint(CVector, double dFactor=0.001, int nCabinet=0);
* Zweck      : liefert die Koordinaten der Gehäuse bzw. Chassispunkte für die BDD-Datei.
*              Dreht die Gehäusekoordinaten, wenn die Bezugsschallwand nicht vorne liegt.
* Parameter (E): v : Punktkoordinate                                   (CVector)
*           (E): [dFactor] : Faktor für die Umwandlung in [m]           (double)
*           (E): [nCabinet]: Bezugsgehäuse für Koordinatentransformation   (int)
*                LOWORD(nCabinet) Gehäusenummer
*                HIWORD(nCabinet) (NO_ROTATION, NO_TRANSLATION)
*                Flags für die Transformation
* Funktionswert : Transformierte Koordinate 
******************************************************************************************/
PointFloat CCabinet::VectorToBDDpoint(CVector v, double dFactor, int nCabinet)
{
   CCabinet* pC = NULL;
   if (LOWORD(nCabinet))      pC = (CCabinet*)m_Cabinets.GetAt(LOWORD(nCabinet)-1);
   else if (!IsBaseCabinet()) pC = this;

   PointFloat pt;
   if (pC && ( (HIWORD(nCabinet)==0) || ((HIWORD(nCabinet)&NO_ROTATION) ^ (HIWORD(nCabinet)&NO_TRANSLATION)) ))
   {
      Array44 Tran;
      if (HIWORD(nCabinet)&NO_TRANSLATION)
      {
         memcpy(&Tran, pC->m_pdTran, sizeof(Array44));
         Tran.a.m14 = 0;
         Tran.a.m24 = 0;
         Tran.a.m34 = 0;
         SetMatrixPtr((Array44*)&Tran, false);
      }
      else if (HIWORD(nCabinet)&NO_ROTATION)
      {
         memcpy(&Tran, pC->m_pdTran, sizeof(Array44));
         Tran.a.m12 = 0;
         Tran.a.m13 = 0;
         Tran.a.m21 = 0;
         Tran.a.m23 = 0;
         Tran.a.m31 = 0;
         Tran.a.m32 = 0;
         SetMatrixPtr((Array44*)&Tran, false);
      }
      else SetMatrixPtr((Array44*) pC->m_pdTran, false);
      TransformVector(v);
      SetMatrixPtr(NULL, false);
   }
   if (m_dwFlags & (ZERO_DEG_REF_LEFT|ZERO_DEG_REF_BACK|ZERO_DEG_REF_RIGHT))
   {
      if (IsBaseCabinet()) pC = this;
      else                 
      {
         pC = (CCabinet*)m_Cabinets.GetFirst();
         ASSERT(pC != NULL);
      }
      pt.x = (float) (dFactor*(Vx(v)*pC->m_pdTran[0][0]+Vy(v)*pC->m_pdTran[0][1]));
      pt.y = (float) (dFactor*(Vx(v)*pC->m_pdTran[1][0]+Vy(v)*pC->m_pdTran[1][1]));
      pt.z = (float) (dFactor* Vz(v));
   }
   else
   {
      pt.x = (float) (dFactor*Vx(v));
      pt.y = (float) (dFactor*Vy(v));
      pt.z = (float) (dFactor*Vz(v));
   }
   return pt;
}

/******************************************************************************
* Definition : double GetBottomZPos();
* Zweck      : liefert den niedrigsten Punkt der Box.
* Funktionswert : niedrigster Punkt der Box                       (double)
******************************************************************************/
double CCabinet::GetBottomZPos()
{
   return Vz(m_vCabPoints[CAB_PT_LEFT_BOTTOM_FRONT]);
}

/******************************************************************************
* Definition : static void DeleteFnc(void *)
* Zweck      : Löschen eines Objektes aus der Liste
* Aufruf     : Durch CEtsList-Funktionen
* Parameter  : p : Zeiger auf das Objekt                             (void*)
*              wird auf (CCabinet*) gecasted !
******************************************************************************/
void CCabinet::DeleteFnc(void *p)
{
   CCabinet* pC = (CCabinet*)p;
   delete pC;
}

/******************************************************************************
* Definition : CVector GetCabPoint(int n, bool bTransform=false);
* Zweck      : liefert die 3D-Koordinate eines Gehäusepunktes
* Parameter (E): n : Gehäusepunktindex (0, NO_OF_CABINET_POINTS-1)
*  (CAB_PT_LEFT_TOP_FRONT, CAB_PT_RIGHT_TOP_FRONT, CAB_PT_RIGHT_TOP_BACK,
*   CAB_PT_LEFT_TOP_BACK, CAB_PT_LEFT_BOTTOM_FRONT, CAB_PT_RIGHT_BOTTOM_FRONT,
*   CAB_PT_RIGHT_BOTTOM_BACK, CAB_PT_LEFT_BOTTOM_BACK)
*           (E): [bTransform]: Transformation des Punktes, wenn er nicht vom
*                Basisgehäuse ist.
* Funktionswert : 3D-Koordinate des Gehäusepunktes                  (CVector)
******************************************************************************/
CVector CCabinet::GetCabPoint(int n, bool bTransform)
{
   if ((n>=0) && (n<NO_OF_CABINET_POINTS))
   {
      if (bTransform && !IsBaseCabinet())
      {
         CVector v = m_vCabPoints[n];
         SetMatrixPtr((Array44*)m_pdTran, false);
         TransformVector(v);
         SetMatrixPtr(NULL, false);
         return v;
      }
      return m_vCabPoints[n];
   }
   return CVector();
}

/******************************************************************************
* Definition : void EnableWall(int, bool);
* Zweck      : Setzen, Entfernen des WallEnable Flags.
* Parameter (E): nWall: Wandnummer des Gehäuses (1,..6)                 (int)
*           (E): bEnable : (true, false)                                (bool)
******************************************************************************/
void CCabinet::EnableWall(int nWall, bool bEnable)
{
   if ((nWall > 0) && (nWall <= NO_OF_WALLS))
   {
      DWORD dwWall = 0x00800000 << nWall;
      if (bEnable) m_dwFlags &= ~dwWall;
      else         m_dwFlags |=  dwWall;
   }
}

/******************************************************************************
* Definition : void EnableWall(int, bool);
* Zweck      : Abfragen des WallEnable Flags.
* Parameter (E): nWall: Wandnummer des Gehäuses (1,..6)                 (int)
* Funktionswert : (true, false)                                         (bool)
******************************************************************************/
bool CCabinet::IsWallEnabled(int nWall)
{
   if ((nWall > 0) && (nWall <= NO_OF_WALLS))
   {
      DWORD dwWall = 0x00800000 << nWall;
      return ((m_dwFlags & dwWall) ? false : true);
   }
   return false;
}

/******************************************************************************
* Definition : bool CheckWall(int, bool);
* Zweck      : Abfrage des CheckWall Flags.
*              Siehe Fkt.: CBoxPropertySheet::CheckChassisPos(..)
* Parameter (E): nWall: Wandnummer des Gehäuses (1,..6)                 (int)
* Funktionswert : (true, false)                                         (bool)
******************************************************************************/
bool CCabinet::CheckWall(int nWall)
{
   if ((nWall > 0) && (nWall <= NO_OF_WALLS))
   {
      DWORD dwWall = 0x00000001 << nWall;
      return ((m_dwFlags & dwWall) ? true : false);
   }
   return false;
}

/******************************************************************************
* Definition : double GetMaxDimension();
* Zweck      : Ermitteln der längsten Diagonale der Box
* Aufruf     : Nur durch das Basisgehäuseobjekt !
* Funktionswert : Größter Abstand zweier Gehäusepunkte
******************************************************************************/
double CCabinet::GetMaxDimension()
{
   int i, j, nStart = 1;
   CVector vTest, vMax, vMin;
   double *pdPoint    = (double*)&vTest,
          *pdMin      = (double*)&vMin,
          *pdMax      = (double*)&vMax;
   ASSERT(IsBaseCabinet());
   CCabinet *pC = this;
   vMax = vMin = pC->GetCabPoint(0, true);
   while (pC)
   {
      for (i=nStart; i<NO_OF_CABINET_POINTS; i++)
      {
         vTest = pC->GetCabPoint(i, true);
         for (j=0; j<3; j++)
         {
            if (pdMin[j] > pdPoint[j]) pdMin[j] = pdPoint[j];
            if (pdMax[j] < pdPoint[j]) pdMax[j] = pdPoint[j];
         }
      }
      if (IsBaseCabinet())
      {
         if (nStart) pC = (CCabinet*) m_Cabinets.GetFirst();
         else        pC = (CCabinet*) m_Cabinets.GetNext();
         nStart = 0;
      }
      else pC = NULL;
   }
   return Betrag(vMax - vMin);
}

/******************************************************************************
* Definition : void SetTransformation(); 
* Zweck      : Berechnet die Transformationsmatrix aus dem :
*              Verschiebevektor (vPosRef) und den
*              Rotationswinkeln [Theta] und Phi (Vy(vUserRef), Vz(vUserRef))
* Aufruf     : Darf nicht vom Basisobjekt aufgerufen werden !
******************************************************************************/
void CCabinet::SetTransformation()
{
   ASSERT(!IsBaseCabinet());
   SetMatrixPtr((Array44*)m_pdTran, false);
   LoadIdentity();                                  // Einheitsmatrix setzen
   Translate(vPosRef);
   if (Vy(vUserRef) != 0.0) Rotate_y(Vy(vUserRef)); // Theta
   if (Vz(vUserRef) != 0.0) Rotate_z(Vz(vUserRef)); // Phi
   SetMatrixPtr(NULL, false);
}

/******************************************************************************
* Definition : CCabinet* CCabinet::GetCabinet(int);
* Zweck      : liefert die Gehäusesegmente anhand ihrer Nummer bzw. Position
* Aufruf     : Nur durch das Basisgehäuseobjekt !
* Parameter  : nCab: Gehäusenummer (0,...,n)
* Funktionswert : Gehäusesegment-Objekt                        (CCabinet*)
******************************************************************************/
CCabinet* CCabinet::GetCabinet(int nCab)
{
   if (nCab)
   {
      CCabinet*pC = (CCabinet*)m_Cabinets.GetAt(nCab-1);
      if (pC != NULL)
      {
         ASSERT(!pC->IsBaseCabinet());
      }
      return pC; 
   }
   ASSERT(IsBaseCabinet());
   return this;
}

/******************************************************************************
* Definition : double GetYShift(WORD );
* Zweck      : liefert die Verschiebung für die Maßlinie in Y-Richtung
* Parameter (E): wPos: Flags für den Verschiebe-Referenzpunkt
*  (SCALE_SIDE1, SCALE_SIDE1B, SCALE_CENTER, SCALE_SIDE2B, SCALE_SIDE2)
* Funktionswert : Distanz von der Mitte des Gehäuses              (double)
******************************************************************************/
double CCabinet::GetYShift(WORD wPos, CVector *pV)
{
   CVector v;
   int i, nMax, nMin;
   double dMax, dMin;
   Array44 Tran;
   bool    bTran = false;
   if (!IsBaseCabinet())
   {
      memcpy(&Tran, m_pdTran, sizeof(Array44));
      Tran.a.m14 = 0;
      Tran.a.m24 = 0;
      Tran.a.m34 = 0;
      SetMatrixPtr((Array44*)&Tran, false);
      bTran = true;
   }

   if ( (m_dwFlags & WIDTH_1_2_ON_BACK)|| 
       !(m_dwFlags & COMPLEX_CAB_DESIGN)) // da nur in Phi gedreht wird bei einfachem
   {                                      // WIDTH_1_2_ON_BACK bevorzugen
      v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_FRONT-1][0]);
      if (bTran) TransformVector(v);
      dMax = dMin = Vy(v);
      nMax = nMin = 0;
      for (i=1; i<NO_OF_RECT_POINTS; i++)
      {
         v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_FRONT-1][i]);
         if (bTran) TransformVector(v);
         if (Vy(v) < dMin) {dMin = Vy(v); nMin = i;}
         if (Vy(v) > dMax) {dMax = Vy(v); nMax = i;}
      }
      if (wPos & SCALE_SIDE1)
      {
         if (pV) *pV = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_FRONT-1][nMin]);
         dMax = dMin;
      }
      else if (wPos & SCALE_SIDE1B)
      {
         v = GetCabPoint(GetOppositePoint(CARABOX_CABWALL_FRONT, nMin));
         if (pV) *pV = v;
         if (bTran) TransformVector(v);
         dMax = Vy(v);
      }
      else if (wPos & SCALE_SIDE2B)
      {
         v = GetCabPoint(GetOppositePoint(CARABOX_CABWALL_FRONT, nMax));
         if (pV) *pV = v;
         if (bTran) TransformVector(v);
         dMax = Vy(v);
      }
      else if (wPos & SCALE_SIDE2)
      {
         if (pV) *pV = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_FRONT-1][nMax]);
      }
      else dMax = 0;
   }
   else
   {
      v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][0]);
      if (bTran) TransformVector(v);
      dMax = dMin = Vy(v);
      nMax = nMin = 0;
      for (i=1; i<NO_OF_RECT_POINTS; i++)
      {
         v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][i]);
         if (bTran) TransformVector(v);
         if (Vy(v) < dMin) {dMin = Vy(v); nMin = i;}
         if (Vy(v) > dMax) {dMax = Vy(v); nMax = i;}
      }
      if (wPos & SCALE_SIDE1)
      {
         dMax = dMin;
         if (pV) *pV = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][nMin]);
      }
      else if (wPos & SCALE_SIDE1B)
      {
         v = GetCabPoint(GetOppositePoint(CARABOX_CABWALL_BOTTOM, nMin));
         if (pV) *pV = v;
         if (bTran) TransformVector(v);
         dMax = Vy(v);
      }
      else if (wPos & SCALE_SIDE2B)
      {
         v = GetCabPoint(GetOppositePoint(CARABOX_CABWALL_BOTTOM, nMax));
         if (pV) *pV = v;
         if (bTran) TransformVector(v);
         dMax = Vy(v);
      }
      else if (wPos & SCALE_SIDE2)
      {
         if (pV) *pV = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][nMax]);
      }
      else dMax = 0;
   }
   if (bTran) SetMatrixPtr(NULL, false);
   return dMax;
}

/******************************************************************************
* Definition : double GetXShift(WORD );
* Zweck      : liefert die Verschiebung für die Maßlinie in X-Richtung
* Parameter (E): wPos: Flags für den Verschiebe-Referenzpunkt
*  (SCALE_SIDE1, SCALE_SIDE1B, SCALE_CENTER, SCALE_SIDE2B, SCALE_SIDE2)
* Funktionswert : Distanz von der Mitte des Gehäuses              (double)
******************************************************************************/
double CCabinet::GetXShift(WORD wPos, CVector *pV)
{
   CVector v;
   int i, nMax, nMin;
   double dMax, dMin;
   Array44 Tran;
   bool    bTran = false;
   if (!IsBaseCabinet())
   {
      memcpy(&Tran, m_pdTran, sizeof(Array44));
      Tran.a.m14 = 0;
      Tran.a.m24 = 0;
      Tran.a.m34 = 0;
      SetMatrixPtr((Array44*)&Tran, false);
      bTran = true;
   }
   v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][0]);
   if (bTran) TransformVector(v);
   dMax = dMin = Vx(v);
   nMax = nMin = 0;
   for (i=1; i<NO_OF_RECT_POINTS; i++)
   {
      v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][i]);
      if (bTran) TransformVector(v);
      if (Vx(v) < dMin) {dMin = Vx(v); nMin = i;}
      if (Vx(v) > dMax) {dMax = Vx(v); nMax = i;}
   }

   if (wPos & SCALE_SIDE2)
   {
      if (pV) *pV = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][nMin]);
      dMax = dMin;
   }
   else if (wPos & SCALE_SIDE2B)
   {
      if (!(m_dwFlags & COMPLEX_CAB_DESIGN))    // Bei einfachem Design ist sonst keine Auswirkung
      {                                         // zu sehen, deshalb statt gegenüberliegendem Punkt oben
         nMin ^= 0x0001;                        // vorne / hinten vertauschen: Index (0->1, 1->0, 2->3, 3->2)
         v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][nMin]);
      }
      else
      {
         v = GetCabPoint(GetOppositePoint(CARABOX_CABWALL_BOTTOM, nMin));
      }
      if (pV) *pV = v;
      if (bTran) TransformVector(v);
      dMax = Vx(v);
   }
   else if (wPos & SCALE_SIDE1B)
   {
      if (!(m_dwFlags & COMPLEX_CAB_DESIGN))    // dito s.o.
      {
         nMax ^= 0x0001;   
         v = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][nMax]);
      }
      else
      {
         v = GetCabPoint(GetOppositePoint(CARABOX_CABWALL_BOTTOM, nMax));
      }
      if (pV) *pV = v;
      if (bTran) TransformVector(v);
      dMax =  Vx(v);
   }
   else if (wPos & SCALE_SIDE1)
   {
      if (pV) *pV = GetCabPoint(gm_nWallPoints[CARABOX_CABWALL_BOTTOM-1][nMax]);
   }
   else dMax = 0;

   if (bTran) SetMatrixPtr(NULL, false);
   return dMax;
}

/******************************************************************************
* Definition : int GetOppositePoint(int, int);
* Zweck      : liefert den Index des Punktes der gegenüberliegen Schallwand
* Parameter (E): nWall: Wand zu der der gegenüberliegende Wandpunkt     (int)
*                ermittelt werden soll.
*           (E): nPoint: Index des Wandpunktes (0,..,3)                 (int)
* Funktionswert : Index des gegenüberliegenden Punktes                  (int)
******************************************************************************/
int CCabinet::GetOppositePoint(int nWall, int nPoint)
{
   ASSERT((nWall>0) && (nWall <= NO_OF_WALLS));
   ASSERT((nPoint>=0) && (nPoint < NO_OF_RECT_POINTS));
   return gm_nOppositePoints[nWall-1][nPoint];
}

/*************************************************************************************
* Definition : int SetBaseCabinet(CCabinet *)
* Zweck      : Initialisiert die Gehäuseparameter für ein zusätzliches Gehäusesegment
* Aufruf     : Darf nicht durch das Basisgehäuseobjekt aufgerufen werden !
* Parameter (E): pCbase : Basisgehäusesegment                              (CCabinet*)
* Funktionswert : aktuelle Gehäusesegmentnummer                            (int)
**************************************************************************************/
int CCabinet::SetBaseCabinet(CCabinet *pCbase)
{
   ASSERT(pCbase->IsBaseCabinet());
   pCbase->m_Cabinets.ADDHead(this);                           // Basisgehäuse speichert alle (darf löschen)
   m_Cabinets.SetDestroyMode(false);                           // Eigene Liste darf CCabinetobjekte nicht löschen
   m_Cabinets.ADDHead(pCbase);                                 // Gehäusesegment kennt Basisgehäuse
   m_dwFlags |= NO_BASE_CABINET;                               // Dies ist kein Basisgehäuse !
   nCountAllChassis = pCbase->m_Cabinets.GetCounter();         // Gehäusenummer vergeben
   pCbase->m_pdwWalls = (DWORD*)Alloc(sizeof(DWORD)*(nCountAllChassis+1), pCbase->m_pdwWalls);
   SetTransformation();                                        // Basistransformation setzen
   return nCountAllChassis;
}

/******************************************************************************
* Definition : bool InitWallSegmentArray(DWORD);
* Zweck      : Sucht Schallwände auf anderen Gehäusesegmenten, die in der
*              gleichen Ebene liegen. Diese werden in einem Array (m_pdwWalls)
*              gespeichert.
*              Element (0) enthält immer den übergebenen Schallwandindex !
*              Element (1,..,m_nNoOfWallSegments-1) enthält die anderen SW.
*              Die Anzahl der weiteren Schallwände wird in der Variable
*              (m_nNoOfWallSegments) gespeichert.
* Aufruf     : Nur durch das Basisgehäuseobjekt ! Muß nach Änderung der
*              Gehäusemaße und der Gehäusetransformationen erneut aufgerufen
*              werden.
* Parameter (E): nCabWallNo: Nummer der Schallwand auf einem Gehäusesegment.
* Funktionswert : Segmentarray gültig                                   (bool)
******************************************************************************/
bool CCabinet::InitWallSegmentArray(DWORD nCabWallNo)
{
   int i, j, nNoOfCabinets = m_Cabinets.GetCounter();
   int       nDummy_int = 0;
   CVector   vBasisFLNV, vBasisPunkt[NO_OF_RECT_POINTS], vTestFLNV, vTestPunkt[NO_OF_RECT_POINTS];

   ASSERT(IsBaseCabinet());
   CCabinet *pC = GetCabinet(HIWORD(nCabWallNo));
   ASSERT(pC != NULL);

   if (nNoOfCabinets == 0)                     return false;
   if (!CanContainChassis())                   return false;
   if (!pC->IsWallEnabled(LOWORD(nCabWallNo))) return false;
   if (!GetCabinetWall(nCabWallNo, NULL, &vBasisFLNV, &vBasisPunkt[0])) return false;

   m_pdwWalls[nDummy_int] = nCabWallNo;

   for(i=0; i<=nNoOfCabinets; i++)
   {
      if (i == (int)HIWORD(nCabWallNo)) continue;
      for(j=CARABOX_CABWALL_TOP; j <= CARABOX_CABWALL_BOTTOM; j++)
      {
         DWORD dwCabWallNo = MAKELONG(j, i);

         if(!GetCabinetWall(dwCabWallNo, NULL, &vTestFLNV, &vTestPunkt[0])) return false;
         if(Betrag(vBasisFLNV-vTestFLNV) < 0.002 )          // FLNV beider Walls gleich? (bis auf 0.2%)
         {
            // Basis- und Testpunkt sollen nicht zusammenfallen
            if(Betrag( vBasisPunkt[0]-vTestPunkt[0]) < 0.1 )   vTestPunkt[0] = vTestPunkt[1];
            // liegt Testpunkt in der Basis-Ebene?
            if(fabs(vBasisFLNV*(vBasisPunkt[0]-vTestPunkt[0])) < 2)
            {
               nDummy_int++;
               m_pdwWalls[nDummy_int] = dwCabWallNo;
               break;                                        // keine andere Wand dieses Teilgehäuses geht mehr
            }
         }
      }
   }
   m_nNoOfWallSegments = nDummy_int + 1;
   return true;
}

/******************************************************************************
* Definition : int FindWallSegment(int);
* Zweck      : liefert die Position eines Schallwandindexes im aktuellen
*              SegmentArray.
* Aufruf     : Nur durch das Basisgehäuseobjekt !
* Parameter (E): nCabWall: Nummer der Schallwand auf einem Gehäusesegment.
* Funktionswert : (0,..,m_nNoOfWallSegments-1)                          (int)
*                 (-1) wenn die Schallwand nicht gefunden wurde.
******************************************************************************/
int CCabinet::FindWallSegment(int nCabWall)
{
   ASSERT(IsBaseCabinet());
   if (m_nNoOfWallSegments > 1)                                // sollen mehr Segmente angezeigt werden
   {
      for (int i=0; i<m_nNoOfWallSegments; i++)
         if (nCabWall == (int)m_pdwWalls[i])
            return i;
   }
   return -1;
}

/*
bool CCabinet::IsPointInTriangle(CVector vTest, CVector vP0, CVector vP1, CVector vP2)
{
   double   dLa, dMy;
   double   dDetxy, dDetxz, dDetyz;

   dDetxy = Vx(vP1-vP0)*Vy(vP2-vP0) - Vx(vP2-vP0)*Vy(vP1-vP0);
   dDetxz = Vx(vP1-vP0)*Vz(vP2-vP0) - Vx(vP2-vP0)*Vz(vP1-vP0);
   dDetyz = Vy(vP1-vP0)*Vz(vP2-vP0) - Vy(vP2-vP0)*Vz(vP1-vP0);

   if( (fabs(dDetxy)>fabs(dDetxz)) && (fabs(dDetxy)>fabs(dDetyz)) )       // dDetxy absolut am größten
   {
      dLa = (Vx(vTest-vP0)*Vy(vP2-vP0) - Vx(vP2-vP0)*Vy(vTest-vP0))/dDetxy;
      dMy = (Vx(vP1-vP0)*Vy(vTest-vP0) - Vx(vTest-vP0)*Vy(vP1-vP0))/dDetxy;
   }
   else if( (fabs(dDetxz)>fabs(dDetxy)) && (fabs(dDetxz)>fabs(dDetyz)) )  // dDetxz absolut am größten
   {
      dLa = (Vx(vTest-vP0)*Vz(vP2-vP0) - Vx(vP2-vP0)*Vz(vTest-vP0))/dDetxz;
      dMy = (Vx(vP1-vP0)*Vz(vTest-vP0) - Vx(vTest-vP0)*Vz(vP1-vP0))/dDetxz;
   }
   else                                                                   // dDetyz absolut am größten
   {
      dLa = (Vy(vTest-vP0)*Vz(vP2-vP0) - Vy(vP2-vP0)*Vz(vTest-vP0))/dDetyz;
      dMy = (Vy(vP1-vP0)*Vz(vTest-vP0) - Vy(vTest-vP0)*Vz(vP1-vP0))/dDetyz;
   }

   if( (dLa >= 0.0) && (dMy >= 0.0) && ((dLa+dMy) <= 1.0) )   return (true);
   else                                                       return (false);
}
*/
/*
bool CCabinet::IsChassisOnWall( ChassisData *pCD )
{
   bool      bReturn;
   int       i, nNoOfPoints;
   CVector   vxAxis2D, vyAxis2D, vWallFLNV, vWallPoints[4];
   CVector   *pvChassisPoints = NULL;

   if (!GetCabinetWall(m_pdwWalls[0], NULL, &vWallFLNV, &vWallPoints[0]))   return false;

   if ((fabs(Vx(vWallFLNV)) + fabs(Vy(vWallFLNV))) < 1E-20 ) // FLNV zeigt in z-Richtung, keine x,y-Komponenten
   {
      vxAxis2D  = vWallPoints[2] - vWallPoints[1] ;
      vxAxis2D /= Betrag(vxAxis2D);

      vyAxis2D  = CVector( -Vy(vxAxis2D)*Vz(vWallFLNV),  Vx(vxAxis2D)*Vz(vWallFLNV), 
                            Vx(vWallFLNV)*Vy(vxAxis2D) - Vx(vxAxis2D)*Vy(vWallFLNV) );
      vyAxis2D /= Betrag(vyAxis2D);
   }
   else
   {
      vxAxis2D  = CVector( -Vy(vWallFLNV), Vx(vWallFLNV), 0.0 );
      vxAxis2D /= Betrag(vxAxis2D);

      vyAxis2D  = CVector( -Vx(vWallFLNV)*Vz(vWallFLNV), -Vy(vWallFLNV)*Vz(vWallFLNV), 
                           Vx(vWallFLNV)*Vx(vWallFLNV) + Vy(vWallFLNV)*Vy(vWallFLNV) );
      vyAxis2D /= Betrag(vyAxis2D);
   }

   // Umrisspunkte des Chassis bestimmen
   if (pCD->IsRect())
   {
      int      nNoOfPointsH, nNoOfPointsB;
      double   dDeltaH, dDeltaB;

      nNoOfPointsH  = ((int) ((pCD->dEffHoehe)/3.0) );       // (ca.) alle 3 mm ein Testpunkt
      nNoOfPointsB  = ((int) ((pCD->dEffBreite)/3.0) );
      dDeltaH       = (pCD->dEffHoehe)/nNoOfPointsH;
      dDeltaB       = (pCD->dEffBreite)/nNoOfPointsB;
      nNoOfPoints   = 2*(nNoOfPointsH + nNoOfPointsB);

      pvChassisPoints = new CVector[nNoOfPoints];

      for (i=0; i<nNoOfPointsH; i++)
      {
         pvChassisPoints[i] = pCD->vPosChassis -             // linke Seite
                              vyAxis2D*(pCD->dEffHoehe)*0.5 -
                              vxAxis2D*(pCD->dEffBreite)*0.5 +
                              vyAxis2D*i*dDeltaH;
         pvChassisPoints[nNoOfPointsH+nNoOfPointsB+i] =      // rechte Seite
                              pCD->vPosChassis + 
                              vyAxis2D*(pCD->dEffHoehe)*0.5 +
                              vxAxis2D*(pCD->dEffBreite)*0.5 -
                              vyAxis2D*i*dDeltaH;
      }
      for(i=0; i<nNoOfPointsB; i++)
      {
         pvChassisPoints[nNoOfPointsH+i] =                   // oben
                              pCD->vPosChassis + 
                              vyAxis2D*(pCD->dEffHoehe)*0.5 -
                              vxAxis2D*(pCD->dEffBreite)*0.5 +
                              vxAxis2D*i*dDeltaB;
         pvChassisPoints[2*nNoOfPointsH+nNoOfPointsB+i] =    // unten
                              pCD->vPosChassis - 
                              vyAxis2D*(pCD->dEffHoehe)*0.5 +
                              vxAxis2D*(pCD->dEffBreite)*0.5 -
                              vxAxis2D*i*dDeltaB;
      }
   }
   else if (pCD->IsRound())
   {
      double   dDeltaBogen;
      
      nNoOfPoints = (int) ((pCD->dEffDiameter)*M_PI/3.0);
      dDeltaBogen = 2.0*M_PI/nNoOfPoints;

      pvChassisPoints = new CVector[nNoOfPoints];

      for(i=0; i<nNoOfPoints; i++)
      {
         pvChassisPoints[i] = pCD->vPosChassis + 
                              vxAxis2D*cos(i*dDeltaBogen)*(pCD->dEffDiameter)*0.5 +
                              vyAxis2D*sin(i*dDeltaBogen)*(pCD->dEffDiameter)*0.5;
      }
   }
   else if (pCD->IsSphere())
   {
      nNoOfPoints = 1;
      pvChassisPoints    = new CVector[nNoOfPoints];
      pvChassisPoints[0] = pCD->vPosChassis - (vWallFLNV * (pCD->m_pCabinet->dHeight + 0.5 * pCD->dEffDiameter));
   }
   else return false;                                          // Membran weder rund noch rechteckig

   // Liegt jeder Chassis-Umrisspunkt in mindestens einem Wandsegment
   for(i=0; i<nNoOfPoints; i++)
   {
      int j;
      bReturn = false;
      for(j=0; j<m_nNoOfWallSegments; j++)
      {
         if (!GetCabinetWall(m_pdwWalls[j], NULL, NULL, &vWallPoints[0]))   break;

         if (IsPointInTriangle(pvChassisPoints[i], vWallPoints[0], vWallPoints[1], vWallPoints[2]))
         {
            bReturn = true;
            break;
         }
         if (IsPointInTriangle(pvChassisPoints[i], vWallPoints[2], vWallPoints[3], vWallPoints[0]))
         {
            bReturn = true;
            break;
         }
      }
      if (!bReturn)   break;
   }

   if (pvChassisPoints) delete[] pvChassisPoints;

   if (bReturn) pCD->dwFlagsCH |=  (CHASSIS_POS_VALID|CHASSIS_POS_ON_2_WALLS);
   else         pCD->dwFlagsCH &= ~CHASSIS_POS_VALID;

   return bReturn;
}
*/

