#include "BoxStructures.h"
#include "BOXGLOBAL.H"
#include "CCabinet.h"
#include "Debug.h"
#include "resource.h"
#include "CEtsList.h"
#include "BoxPropertySheet.h"
#include "CEtsDiv.h"
#include "CSortFunctions.h"
#include "CustErr.h"

#ifdef _DEBUG_HEAP
 #ifdef Alloc
  #undef Alloc
 #endif
 #define Alloc(p1, p2) Alloc(p1, p2, __FILE__, __LINE__)
 #define new DLL_DEBUG_NEW
#endif

extern COLORREF g_rgbPickColor;

ChassisData::ChassisData()
{
   ZeroMemory(this, sizeof(*this));
   nPolung      = 1;       // positive Polung
   dCorrectAmpl = 1.0;     // Korrekturfaktor ist 1.0;
}

ChassisData::~ChassisData()
{
   if (m_ppCoupled)
   {
      Free(m_ppCoupled);
   }
   if (m_ppCabinets)
   {
      Free(m_ppCabinets);
   }
}

/*************************************************************************************
* Definition : void Draw(HDC, bool, CVector2D *)
* Zweck      : Zeichnet Chassis auf der Schallwand
* Parameter (E): hdc : Gerätekontext zum Zeichnen                          (HDC)
*           (E): bPicked : Chassis ist gepickt (markiert)                  (bool)
*           (E): pvrcWall: Zugehörige Schallwand                           (CVector2D*)
**************************************************************************************/
void ChassisData::Draw(HDC hdc, bool bPicked, CVector2D *pvrcWall)
{
   if (IsHidden()) return;

   if (GetMembrTyp())
   {
      HBRUSH hBr = NULL;
      HPEN   hPe = NULL;
      bool   bNoStockObject = false;
      if (!(dwFlagsCH & CHASSIS_POS_VALID))
      {
         LOGBRUSH lb = {BS_HATCHED, 0, HS_DIAGCROSS};
         if (bPicked) lb.lbColor = g_rgbPickColor;
         hBr = ::CreateBrushIndirect(&lb);
         bNoStockObject = true;
      }
      if (bPicked)
      {
         if (hBr==NULL) hBr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
         hPe = ::CreatePen(PS_SOLID, 2, g_rgbPickColor);
      }
      else
      {
         if (hBr==NULL) hBr = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
         hPe = ::CreatePen(PS_SOLID, 1, 0);
      }
      ::SaveDC(hdc);
      ::SelectObject(hdc, hPe);
      ::SelectObject(hdc, hBr);
      if (bNoStockObject)
      {
         ::SetBkColor(hdc, 0);
      }

      RECT rc = GetRect(hdc, false, pvrcWall);
      if (IsRect()) ::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
      else          ::Ellipse(  hdc, rc.left, rc.top, rc.right, rc.bottom);

      ::RestoreDC(hdc, -1);
      if (hBr && bNoStockObject) ::DeleteObject(hBr);
      if (hPe   ) ::DeleteObject(hPe);
   }
}
 
/*************************************************************************************
* Definition : RECT GetRect(HDC, bool, CVector2D *)
* Zweck      : liefert das umgebende Rechteck des Chassis
* Parameter (E): hdc : Gerätekontext zum Zeichnen                          (HDC)
*           (E): bDev: Geräte- oder Logische Koordinaten (true, false)     (bool)
*           (E): 2D Koordinaten der Schallwand                             (CVector2D*)
* Funktionswert : umgebendes Rechteck                                      (RECT)
**************************************************************************************/
RECT ChassisData::GetRect(HDC hdc, bool bDev, CVector2D *pvrcWall)
{
   RECT rc = {0,0,0,0}, rcOrg;
   SIZE szInfl = {0,0};
   double dCenterX = CCabinet::GetXCenterLine(pvrcWall);
   double dXpos    = dCenterX + dDistFromCenter,
          dYpos    = dHeightOnCabWall;

   dYpos += Vy(pvrcWall[2]);

   if (IsRect())
   {
      rc.top    = CEtsDiv::Round(dYpos + dEffHoehe  * 0.5);
      rc.bottom = CEtsDiv::Round(dYpos - dEffHoehe  * 0.5);
      rc.left   = CEtsDiv::Round(dXpos - dEffBreite * 0.5);
      rc.right  = CEtsDiv::Round(dXpos + dEffBreite * 0.5);
   }
   else if (GetMembrTyp())
   {
      rc.top    = CEtsDiv::Round(dYpos + dEffDiameter * 0.5);
      rc.bottom = CEtsDiv::Round(dYpos - dEffDiameter * 0.5);
      rc.left   = CEtsDiv::Round(dXpos - dEffDiameter * 0.5);
      rc.right  = CEtsDiv::Round(dXpos + dEffDiameter * 0.5);
   }
   rcOrg = rc;
   if (hdc) ::LPtoDP(hdc, (POINT*)&rc, 2);
   else return rcOrg;
   szInfl.cx = rc.right - rc.left;
   szInfl.cy = rc.bottom- rc.top;
   if ((szInfl.cx<2) || (szInfl.cy<2))
   {
      ::InflateRect(&rc, 2, 2);
      if (bDev) return rc;
      if (hdc) ::DPtoLP(hdc, (POINT*)&rc, 2);
      return rc;
   }

   if (bDev) return rc;
   else return rcOrg;
}

/*************************************************************************************
* Definition : bool IsRect(), bool IsRound(), bool IsSphere();
* Zweck      : liefert die Gehäuseform des Chassis.
* Funktionswert : (true, false)                                               (bool)
**************************************************************************************/
bool ChassisData::IsRect()
{
   switch (GetMembrTyp())
   {
      case CARABOX_MEMBRTYP_DIPOLE: case CARABOX_MEMBRTYP_FLAT2: case CARABOX_MEMBRTYP_HORN2:
         return true;
      default : return false;
   }
}
bool ChassisData::IsRound()
{
   switch (GetMembrTyp())
   {
      case CARABOX_MEMBRTYP_CON  : case CARABOX_MEMBRTYP_DOME : case CARABOX_MEMBRTYP_FLAT1:
      case CARABOX_MEMBRTYP_HORN1: case CARABOX_MEMBRTYP_SPHERE:
         return true;
      default : return false;
   }
}
bool ChassisData::IsSphere()
{
   return (GetMembrTyp() == CARABOX_MEMBRTYP_SPHERE) ? true : false;
}

/*************************************************************************************
* Definition : int CheckChassisParam();
* Zweck      : überprüft die Chassisparameter und liefert eine Fehlercode für nicht
*              korrekte Parameter.
* Funktionswert : (0): Kein Fehler                                            (int)
*                 (IDC_CH_EDT_WIDTH)   : Breite=0 bei rechteckigen Chassis
*                 (IDC_CH_EDT_HEIGHT)  : Höhe=0 bei rechteckigen Chassis
*                 (IDC_CH_EDT_DIAMETER): Durchmesser=0 bei runden Chassis
*                 (IDC_CH_R_M_FORM0)   : ChassisMembranTyp nicht definiert
*                 (IDC_CH_EDT_DESCRIPTION): Beschreibungstext fehlt
**************************************************************************************/
int ChassisData::CheckChassisParam()
{
   if (GetMembrTyp() != 0)
   {
      if (IsRect())
      {
         if      (dEffBreite   == 0.0) return IDC_CH_EDT_WIDTH;
         else if (dEffHoehe    == 0.0) return IDC_CH_EDT_HEIGHT;
         if (m_nCoupled)
         {
            if (dEffBreite > dEffHoehe)
            {
               dEffHoehe = dEffBreite + 1;
               return IDC_CH_EDT_HEIGHT;
            }
         }
      }
      else
      {
         if (dEffDiameter == 0.0) return IDC_CH_EDT_DIAMETER;
      }
   }
   else return IDC_CH_R_M_FORM0;
   if (szDescrpt[0] == 0) return IDC_CH_EDT_DESCRIPTION;
   return 0;
}

/*************************************************************************************
* Definition : int CheckPositionParam();
* Zweck      : Überprüft die Chassisposition
* Funktionswert : (0): Position korrekt                                        (int)
*                 (IDC_CD_LST_CHASSIS_POS) : noch keine Schallwand zugeordnet
*                 (IDC_CD_EDT_POS_HEIGHT)  : Position nicht korrekt
**************************************************************************************/
int ChassisData::CheckPositionParam()
{
   if (nCabWall == 0)      return IDC_CD_LST_CHASSIS_POS;
   if (!IsPositionValid()) return IDC_CD_EDT_POS_HEIGHT;
   return 0;
}

/*************************************************************************************
* Definition : ChassisData * GetCoupled(int);
* Zweck      : liefert die gekoppelten Chassis bei Dipollautsprechern
* Parameter (E): nC: Index des gekoppelten Chassis (0,..,m_nCoupled-1)           (int)
* Funktionswert : gekoppeltes Chassisdatenobjekt                        (ChassisData*)
**************************************************************************************/
ChassisData * ChassisData::GetCoupled(int nC)
{
   if ((nC >= 0) && (nC < m_nCoupled))
   {
      ASSERT(m_ppCoupled != NULL);
      return m_ppCoupled[nC];
   }
   return NULL;
}

/*************************************************************************************
* Definition : void SetMembrTyp(int)
* Zweck      : Setzt den Membrantyp für das Chassis
* Parameter (E): neuer MembranTyp                                                (int)
**************************************************************************************/
void ChassisData::SetMembrTyp(int nTyp)
{
   if ((nTyp == CARABOX_MEMBRTYP_SPHERE) && (nCabWall != CARABOX_CABWALL_TOP))
   {
      nCabWall = 0;
   }
   nMembrTyp = MAKELONG(nTyp, HIWORD(nMembrTyp));
}

/*************************************************************************************
* Definition : void SetHidden(bool)           , bool IsHidden();
*              void SetCalculation(bool)      , bool Calculate();
*              void SetDummy(bool)            , bool IsDummy();
*              void SetAdoptChassisEffic(bool), bool AdoptChassisEffic();
*              void SetShowZeroSpl(bool)      , bool ShowZeroSpl();
*              void SetCalcTransferFunc(bool) , bool CalcTransferFunc();
*              void SetCalcFilter(bool)       , bool CalcFilter();
*              void SetFirstDummy(bool)       , int  GetFirstDummy();
* Zweck      : Setzen bzw. Abfragen der Zusätzlichen Parameter für ein Dipolchassis
*              diese Parameter werden im Highword von (nMembrTyp) gespeichert.
* Parameter (E): [bXXX] Setzen bzw löschen des Parameters                     (bool)
* Funktionswert : (true, false)                                               (bool)
**************************************************************************************/
void ChassisData::SetHidden(bool bHidden)
{
   if (bHidden) nMembrTyp |=  0x00010000;
   else         nMembrTyp &= ~0x00010000;
}

bool ChassisData::IsHidden()
{
   return (nMembrTyp & 0x00010000) ? true : false;
}

void ChassisData::SetCalculation(bool bCalculate)
{
   if (bCalculate) nMembrTyp &= ~0x00020000;
   else            nMembrTyp |=  0x00020000;
}

bool ChassisData::Calculate()
{
   return (nMembrTyp & 0x00020000) ? false : true;
}

void ChassisData::SetDummy(bool bDummy)
{
   if (bDummy) nMembrTyp |=  0x00040000;
   else        nMembrTyp &= ~0x00040000;
}

bool ChassisData::IsDummy()
{
   return (nMembrTyp & 0x00040000) ? true : false;
}

void ChassisData::SetAdoptChassisEffic(bool bAdopt)
{
   if (bAdopt) nMembrTyp &= ~0x00080000;
   else        nMembrTyp |=  0x00080000;
}

bool ChassisData::AdoptChassisEffic()
{
   return (nMembrTyp & 0x00080000) ? false : true;
}

void ChassisData::SetShowZeroSpl(bool bShow)
{
   if (bShow) nMembrTyp &= ~0x00100000;
   else       nMembrTyp |=  0x00100000;
}

bool ChassisData::ShowZeroSpl()
{
   return (nMembrTyp & 0x00100000) ? false : true;
}

void ChassisData::SetCalcTransferFunc(bool bCalc)
{
   if (bCalc) nMembrTyp &= ~0x00200000;
   else       nMembrTyp |=  0x00200000;
}

bool ChassisData::CalcTransferFunc()
{
   return (nMembrTyp & 0x00200000) ? false : true;
}

void ChassisData::SetCalcFilter(bool bCalc)
{
   if (bCalc) nMembrTyp &= ~0x00400000;
   else       nMembrTyp |=  0x00400000;
}

bool ChassisData::CalcFilter()
{
   return (nMembrTyp & 0x00400000) ? false : true;
}

/*************************************************************************************
* Definition : void SetFirstDummy(bool)       , int  GetFirstDummy();
* Zweck      : Setzen bzw. Abfragen der ersten Dummyposition
* Parameter (E): nFirst: Setzen des Position                                 (int)
* Funktionswert : (1,..,n)                                                   (int)
**************************************************************************************/
void ChassisData::SetFirstDummy(int nFirst)
{
   nMembrTyp &= ~0xF0000000;
   nMembrTyp |=  nFirst<<28;
}

int ChassisData::GetFirstDummy()
{
   return (nMembrTyp >> 28);
}

/*************************************************************************************
* Definition : bool IsPositionValid();
* Zweck      : liefert das Flag (CHASSIS_POS_VALID)
* Funktionswert : (true, false)                                                (bool)
**************************************************************************************/
bool ChassisData::IsPositionValid()
{
   return (dwFlagsCH & CHASSIS_POS_VALID) ? true : false;
}

/*************************************************************************************
* Definition : void SetWall(int);
* Zweck      : Ändert die Schallwand des Chassis 
* Parameter (E): nWall: Schallwandnummer                                      (int)
*              (CARABOX_CABWALL_TOP,..,CARABOX_CABWALL_BOTTOM)
**************************************************************************************/
void ChassisData::SetWall(int nWall)
{
   nCabWall = MAKELONG(nWall, HIWORD(nCabWall));
}

/*************************************************************************************
* Definition : void SetCabinet(int);
* Zweck      : Ändert das Gehäusesegment des Chassis
* Parameter (E): nCabinet: Segmentnummer                                      (int)
**************************************************************************************/
void ChassisData::SetCabinet(int nCabinet)
{
   nCabWall = MAKELONG(LOWORD(nCabWall), nCabinet);
}

/*************************************************************************************
* Definition : void SetNoOfCoupledChassis(int, CEtsList*);
* Zweck      : Ändert die Anzahl der gekoppelten Chassis für ein Dipol Master Chassis
*              Die Chassis werden allociert bzw. gelöscht und in die Haupchassisliste
*              eingetragen. Die gekoppelten Chassis werden in einem Array gespeichert.
* Aufruf     : Darf nicht von einem Dummy()-Chassis aufgerufen werden !
* Parameter (E):nNewCoupled: neue Anzahl der gekoppleten Chassis.
*           (EA): pList: Hauptchassisliste, in der alle Chassis gespeichert und später
*                 gelöscht werden.
**************************************************************************************/
void ChassisData::SetNoOfCoupledChassis(int nNewCoupled, CEtsList*pList)
{
   int i;
   ASSERT(IsDummy()==false);                                   // Ein dummy darf diese Funktion nicht aufrufen

   for (i=nNewCoupled; i<m_nCoupled; i++)                      // wird die Anzahl der gekoppelten verringert,
   {
      ChassisData *pCD = (ChassisData*)GetCoupled(i);
      if (pCD)                                                 // müssen die gekoppelten Chassis
      {
         pList->Delete(pCD);                                   // aus der Liste gelöscht werden
      }
   }
   if (nNewCoupled == 0)                                       // alle löschen :
   {
      if (m_ppCoupled) Free(m_ppCoupled);                      // Pointer freigeben
      m_ppCoupled = NULL;
   }
   else                                                        // sonst : Größe anpassen
   {
      m_ppCoupled = (ChassisData**)Alloc(sizeof(void*)*nNewCoupled, m_ppCoupled);
   }
   for (i=m_nCoupled; i<nNewCoupled; i++)                      // wird die Anzahl der gekoppelten erhöht
   {
      ChassisData *pCD = new ChassisData;                      // Gekoppeltes Chassis anlegen
      pList->ADDHead(pCD);                                     // in die Liste einfügen
      m_ppCoupled[i] = pCD;                                    // Pointer gegenseitig koppeln
      pCD->m_nCoupled     = 1;                                 // Der Besitzer ES KANN NUR EINEN GEBEN
      pCD->m_ppCoupled    = (ChassisData**)Alloc(sizeof(void*)*pCD->m_nCoupled, NULL);
      pCD->m_ppCoupled[0] = this;                              // Das gekoppelte kennt seinen Besitzer
      if (i==0)                                                // erstes gekoppeltes ist das sichtbare Gegenstück (Dipol)
      {                                                        // der Besitzer ist flach und rechteckig und
//         ASSERT((GetMembrTyp()==CARABOX_MEMBRTYP_FLAT2) || (GetMembrTyp()==CARABOX_MEMBRTYP_DIPOLE));
         pCD->nChassisTyp = nChassisTyp;                       // Chassistyp wie Besitzer
         pCD->SetMembrTyp(GetMembrTyp());                      // Membrantyp wie Besitzer
         pCD->SetDummy(true);
      }
   }
   m_nCoupled = nNewCoupled;
}


/*************************************************************************************
* Definition : void AdaptCoupledSize();
* Zweck      : Anpassen der Größe des gekoppelten sichbaren Chassis.
**************************************************************************************/
void ChassisData::AdaptCoupledSize()
{
   ChassisData*pCDC = GetCoupled(0);
   if (pCDC)
   {
      ASSERT(!IsHidden());
      pCDC->dEffBreite = dEffBreite;
      pCDC->dEffHoehe  = dEffHoehe;
//      if (nCabWall == 0) pCDC->nCabWall = 0;
   }
}

/*************************************************************************************
* Definition : void ReadData(CBoxPropertySheet *);
* Zweck      : ließt die Chassisdaten aus einer Datei
* Parameter (EA): pps : CBoxPropertySheet-Objekt          (CBoxPropertySheet*)
*                 beinhaltet die Dateifunktionen und Variablen
**************************************************************************************/
void ChassisData::ReadData(CBoxPropertySheet *pps)
{
   int nVersion = pps->GetFileVersion();
   pps->ReadFileFH(szDescrpt, sizeof(char)*BOXTEXT_LEN);
   pps->ReadFileFH(&nChassisTyp, sizeof(int));
   pps->ReadFileFH(&nMembrTyp, sizeof(int));
   pps->ReadFileFH(&nCabWall, sizeof(int));
   pps->ReadFileFH(&nPolung, sizeof(int));
   pps->ReadFileFH(&dEffDiameter, sizeof(double));
   pps->ReadFileFH(&dEffBreite, sizeof(double));
   pps->ReadFileFH(&dEffHoehe, sizeof(double));
   pps->ReadFileFH(&vPosChassis, sizeof(CVector));
   pps->ReadFileFH(szDirectFileName, sizeof(char)*32);
   pps->ReadFileFH(&dResFreqMounted, sizeof(double));
   pps->ReadFileFH(&dResFreqFrei, sizeof(double));
   pps->ReadFileFH(&dQFact, sizeof(double));
   pps->ReadFileFH(&dInduReal, sizeof(double));
   pps->ReadFileFH(&dInduImag, sizeof(double));
   pps->ReadFileFH(&dInduExpReal, sizeof(double));
   pps->ReadFileFH(&dInduExpImag, sizeof(double));
   pps->ReadFileFH(&dCorrectAmpl, sizeof(double));
   pps->ReadFileFH(cAmplAccel, sizeof(Complex)*CALCNOOFFREQ);
   pps->ReadFileFH(cSPZeroStandard, sizeof(Complex)*CALCNOOFFREQ);

   if (nVersion == 100)
   {
      dwFlagsCH = CHASSIS_2D_POS_INVALID;
      bDataOk   = 0;
   }
   if (nVersion >= 110)
   {
      DWORD  dwDummy;
      pps->ReadFileFH(&dwFlagsCH, sizeof(DWORD));
      pps->ReadFileFH(&fVAS, sizeof(float));                   // Strukturalignment 8 Byte = 2 * DWORD
      pps->ReadFileFH(&dDistFromCenter, sizeof(double));
      pps->ReadFileFH(&dHeightOnCabWall, sizeof(double));
      pps->ReadFileFH(&dwDummy, sizeof(DWORD));                // Strukturalignment 8 Byte = 2 * DWORD
      if (dwDummy) bDataOk = true;
      else         bDataOk = false;
      pps->ReadFileFH(&dwDummy, sizeof(DWORD));
      dwFlagsCH &= ~CHASSIS_3D_POS_INVALID;
      dwFlagsCH &= ~CHASSIS_2D_POS_INVALID;
   }

   if (nVersion >= 210)
   {
      long nCoupled;
      pps->ReadFileFH(&nCoupled, sizeof(long));
      if (nCoupled)
      {
         SetNoOfCoupledChassis(nCoupled, pps->GetChassisList());// die Kopplung ist gegenseitig !!!
         ChassisData*pCDC = GetCoupled(0);
         if (pCDC== NULL) throw (DWORD)ALLOCATION_ERROR;
         pCDC->ReadData(pps);
      }
      dwFlagsCH &= ~CHASSIS_2D_POS_INVALID;
   }

   if (nVersion >= 220)                                        // es können weitere Gehäusesgmente
   {                                                           // vorhanden sein, die an das
      long i, nCabinets = 0;                                   // Chassis gekoppelt sind
      pps->ReadFileFH(&nCabinets, sizeof(long));               // Anzahl der gekoppelten Gehäusesegmente
      if (nCabinets > 0)
      {
         m_ppCabinets = (CCabinet**)Alloc(sizeof(void*)*nCabinets, NULL);
         for (i=0; i<nCabinets; i++)
         {                                                     // Nummer des Segments
            pps->ReadFileFH(&m_ppCabinets[i], sizeof(long));
         }
      }
   }
}

/*************************************************************************************
* Definition : void WriteData(CBoxPropertySheet *);
* Zweck      : schreibt die Chassisdaten in eine Datei
* Parameter (EA): pps : CBoxPropertySheet-Objekt          (CBoxPropertySheet*)
*                 beinhaltet die Dateifunktionen und Variablen
**************************************************************************************/
void ChassisData::WriteData(CBoxPropertySheet *pps)
{
   int nVersion = pps->GetFileVersion();
   pps->WriteFileFH(szDescrpt, sizeof(char)*BOXTEXT_LEN);
   pps->WriteFileFH(&nChassisTyp, sizeof(int));
   pps->WriteFileFH(&nMembrTyp, sizeof(int));
   pps->WriteFileFH(&nCabWall, sizeof(int));
   pps->WriteFileFH(&nPolung, sizeof(int));
   pps->WriteFileFH(&dEffDiameter, sizeof(double));
   pps->WriteFileFH(&dEffBreite, sizeof(double));
   pps->WriteFileFH(&dEffHoehe, sizeof(double));
   pps->WriteFileFH(&vPosChassis, sizeof(CVector));
   pps->WriteFileFH(szDirectFileName, sizeof(char)*32);
   pps->WriteFileFH(&dResFreqMounted, sizeof(double));
   pps->WriteFileFH(&dResFreqFrei, sizeof(double));
   pps->WriteFileFH(&dQFact, sizeof(double));
   pps->WriteFileFH(&dInduReal, sizeof(double));
   pps->WriteFileFH(&dInduImag, sizeof(double));
   pps->WriteFileFH(&dInduExpReal, sizeof(double));
   pps->WriteFileFH(&dInduExpImag, sizeof(double));
   pps->WriteFileFH(&dCorrectAmpl, sizeof(double));
   pps->WriteFileFH(cAmplAccel, sizeof(Complex)*CALCNOOFFREQ);
   pps->WriteFileFH(cSPZeroStandard, sizeof(Complex)*CALCNOOFFREQ);

   if (nVersion >= 110)
   {
      DWORD dwDummy = 0;
      pps->WriteFileFH(&dwFlagsCH, sizeof(DWORD));
      pps->WriteFileFH(&fVAS, sizeof(float));                   // Strukturalignment 8 Byte = 2 * DWORD
      pps->WriteFileFH(&dDistFromCenter, sizeof(double));
      pps->WriteFileFH(&dHeightOnCabWall, sizeof(double));
      dwDummy = bDataOk;
      pps->WriteFileFH(&dwDummy, sizeof(DWORD));               // Strukturalignment 8 Byte = 2 * DWORD
      dwDummy = 0;
      pps->WriteFileFH(&dwDummy, sizeof(DWORD));
      dwFlagsCH &= ~CHASSIS_3D_POS_INVALID;
   }

   if (nVersion >= 210)
   {
      long nCoupled = 0;
      if (!IsDummy() && (m_nCoupled > 0)) nCoupled = 1;
      pps->WriteFileFH(&nCoupled, sizeof(long));
      if (nCoupled)                                            // nur der Master darf
      {
         ((ChassisData*)GetCoupled(0))->WriteData(pps);        // seinen Slave speichern
      }
   }
   if (nVersion >= 220)                                        // es können weitere Gehäusesgmente
   {                                                           // vorhanden sein, die an das
      long i, nCabinets = 0;                                   // Chassis gekoppelt sind
      if (m_ppCabinets)
      {
         if      (m_ppCabinets[0]->nCountHT) nCabinets = m_ppCabinets[0]->nCountHT;
         else if (m_ppCabinets[0]->nCountTL) nCabinets = m_ppCabinets[0]->nCountTL;
      }
      pps->WriteFileFH(&nCabinets, sizeof(long));              // Anzahl der gekoppelten Gehäusesegmente
      for (i=0; i<nCabinets; i++)
      {
         pps->WriteFileFH(&m_ppCabinets[i]->nCountAllChassis, sizeof(long));
      }
   }
}

/*************************************************************************************
* Definition : static int CountValidListChassis(void *, WPARAM, LPARAM);
* Zweck      : zählt die gültigen Chassis in der Chassisliste
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*)                                              (WPARAM)
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int ChassisData::CountValidListChassis(void *p, WPARAM wParam, LPARAM /*lParam*/)
{
   ChassisData       *pCH   = (ChassisData*)p;
   if (!pCH->IsHidden() && !pCH->IsDummy()) (*((int*)wParam))++;
   return 1;
}

/*************************************************************************************
* Definition : static int CountSpheres(void *, WPARAM, LPARAM);
* Zweck      : zählt die Kugelchassis in der Chassisliste
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*) Zähler                                       (WPARAM)
*           (E): lParam: zu zählender Chassistyp (HT, MT1,...)                (LPARAM)
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int ChassisData::CountSpheres(void *p, WPARAM wParam, LPARAM lParam)
{
   ChassisData *pCH   = (ChassisData*)p;
   if (pCH->IsSphere() && (pCH->nChassisTyp == lParam)) (*((int*)wParam))++;
   return 1;
}

/*************************************************************************************
* Definition : static int CountDipoles(void *, WPARAM, LPARAM);
* Zweck      : zählt die Chassis mit dem Chassistyp in LOWORD(lParam), die Dipole
*              bzw. nicht Dipole sind.
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*)                                              (WPARAM)
*           (E): lParam: Chassistyp der Gezählt werden soll                   (LPARAM)
*                LOWORD: Chassistyp
*                HIWORD = 1: Dipolmebranen, HIWORD = 0: !Dipolmembranen
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int ChassisData::CountDipoles(void *p, WPARAM wParam, LPARAM lParam)
{
   ChassisData *pCH   = (ChassisData*)p;
   if (pCH->nChassisTyp == LOWORD(lParam))
   {
      if (HIWORD(lParam))
      {
         if (pCH->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE) (*((int*)wParam))++;
      }
      else if (pCH->GetMembrTyp())
      {
         if (pCH->GetMembrTyp() != CARABOX_MEMBRTYP_DIPOLE) (*((int*)wParam))++;
      }
   }
   return 1;
}

/*************************************************************************************
* Definition : static int CountChassisOnWall(void *, WPARAM, LPARAM);
* Zweck      : zählt die Chassis auf einer Schallwand
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*)                                              (WPARAM)
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int ChassisData::CountChassisOnWall(void *p, WPARAM wParam, LPARAM lParam)
{
   ChassisData *pCH   = (ChassisData*)p;
   if (pCH->nCabWall == lParam) (*((int*)wParam))++;
   return 1;
}

/*************************************************************************************
* Definition : static int ChangeCabinetTo(void *, WPARAM, LPARAM);
* Zweck      : Ändert die Schallwand eines Chassis
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*)                                              (WPARAM)
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int ChassisData::ChangeCabinetTo(void *p, WPARAM wParam, LPARAM lParam)
{
   ChassisData *pCH   = (ChassisData*)p;
   if (pCH->GetCabinet() == LOWORD(wParam))
   {
      if (lParam) pCH->nCabWall = 0;
      else        pCH->SetCabinet(HIWORD(wParam));
   }
   return 1;
}

/*************************************************************************************
* Definition : static int FindChassisOfType(void *, WPARAM, LPARAM);
* Zweck      : zählt die Chassis auf einer Schallwand
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*)                                              (WPARAM)
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int ChassisData::FindChassisOfType(void *p, WPARAM wP, LPARAM lP)
{
   ChassisData *pCD = (ChassisData*) p;
   if (pCD->IsDummy()) return 0;
   if (pCD->nChassisTyp == (int)wP)
   {
      *(ChassisData**)lP = pCD;
      if (pCD->CheckChassisParam() != 0)
         return 0;
   }
   return 1;
}

/*************************************************************************************
* Definition : static int CheckChassisVersion(void *, WPARAM, LPARAM);
* Zweck      : ermittelt die Version abhängig vom Membrantyp
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*)                                              (WPARAM)
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int ChassisData::CheckChassisVersion(void *p, WPARAM wParam, LPARAM /*lParam*/)
{
   ChassisData *pC = (ChassisData*)p;
   if (pC->GetMembrTyp() > CARABOX_MEMBRTYP_DOME)              // neuere Membrantypen als Kalotte
   {
      ((CFileHeader*)wParam)->SetVersion(210);                 // ab Version 2.10
      if (pC->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE)
      {
         ((CFileHeader*)wParam)->SetVersion(220);              // ab Version 2.20
         return 0;
      }
   }
   return 1;
}

/*************************************************************************************
* Definition : double& SetHeightOnCabWall();
* Zweck      : Ändert die Höhe des Chassis auf der 2D-Schallwand
* Aufruf     : pChassis->SetHeightOnCabWall() = dNewHeight;
* Funktionswert : Höhe des Chassis                                         (double&)
**************************************************************************************/
double& ChassisData::SetHeightOnCabWall()
{
   dwFlagsCH |= CHASSIS_3D_POS_INVALID;
   return dHeightOnCabWall;
}

/*************************************************************************************
* Definition : double& SetDistFromCenter();
* Zweck      : Ändert den Abstand des Chassis von der Mittellinie der 2D-Schallwand
* Aufruf     : pChassis->SetDistFromCenter() = dNewDistance;
* Funktionswert : Abstand des Chassis                                        (double&)
**************************************************************************************/
double& ChassisData::SetDistFromCenter()
{
   dwFlagsCH |= CHASSIS_3D_POS_INVALID;
   return dDistFromCenter;
}

/*************************************************************************************
* Definition : void SetCorrAmplToSlave();
* Zweck      : Kopiert die Korrekturamplituden vom Dipol Master zu den Slaves
**************************************************************************************/
void ChassisData::SetCorrAmplToSlave()
{
   for (int k=0; k<m_nCoupled; k++)                         // Korrekturamplituden kopieren
      m_ppCoupled[k]->dCorrectAmpl = dCorrectAmpl;
}

/*************************************************************************************
* Definition : void InitData();
* Zweck      : (Re)Initialisisert die Basisdaten
**************************************************************************************/
void BasicData::InitData()
{
   ZeroMemory(this, sizeof(*this));

   ResetFilterData();

   nBoxTyp       = CARABOX_BOXTYP_BR;
   nTypeTTResp   = CARABOX_TTRESPONSETYPNORM;

   nNennImped       = 8;                           // Seite 2 Basic Data
   dEffectivDBWm    = 88.0;
   dLowLimitFreq    = 0.0;
   dHighLimitFreq   = 0.0;
   dMainResFreq     = 0.0;
   dImpedLowFreqBR  = 0.0;
   dImpedHighFreqBR = 0.0;
   dImpedMidFreqBR  = 0.0;

   nXoverTyp    = CARABOX_CO_2W;
   sTT1.dTPFreq = 0.0;                             // Seite 3 X-Over
   sTT1.dHPFreq = 0.0;
   sMT1.dTPFreq = 0.0;
   sMT1.dHPFreq = 0.0;
   sMT2.dTPFreq = 0.0;
   sMT2.dHPFreq = 0.0;
   sMT3.dTPFreq = 0.0;
   sMT3.dHPFreq = 0.0;
   sHT.dTPFreq  = 0.0;
   sHT.dHPFreq  = 0.0;

// 2-3W oder 3-4W 2. Tieftöner
   sTT2.dTPFreq = 0.0;
   sTT2.dHPFreq = 0.0;
}

/*************************************************************************************
* Definition : void ResetFilterData();
* Zweck      : Setzt die Filterparameter auf Standardwerte zurück.
**************************************************************************************/
void BasicData::ResetFilterData()
{
   sTT1.nHPOrder = 0;
   sTT1.nTPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sTT1.nTPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen

   sTT2.nHPOrder = 0;
   sTT2.nTPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sTT2.nTPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen

   sMT1.nHPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sMT1.nHPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen
   sMT1.nTPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sMT1.nTPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen

   sMT2.nHPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sMT2.nHPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen
   sMT2.nTPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sMT2.nTPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen

   sMT3.nHPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sMT3.nHPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen
   sMT3.nTPOrder = 3;  // TP-Filter 3. Ordnung angenommen
   sMT3.nTPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen

   sHT.nHPOrder  = 3;   // TP-Filter 3. Ordnung angenommen
   sHT.nHPType   = 3;   // Tschebyscheff 0.5 dB Welligkeit angenommen
   sHT.nTPOrder  = 0;
}

/*************************************************************************************
* Definition : void WriteData(CBoxPropertySheet *);
* Zweck      : ließt die Basisdaten aus einer Datei
* Parameter (EA): pps : CBoxPropertySheet-Objekt          (CBoxPropertySheet*)
*                 beinhaltet die Dateifunktionen und Variablen
**************************************************************************************/
void BasicData::ReadData(CBoxPropertySheet *pps)
{
   int nVersion = pps->GetFileVersion();
   pps->ReadFileFH(&nBoxTyp, sizeof(int));
   pps->ReadFileFH(&nXoverTyp, sizeof(int));
   pps->ReadFileFH(&nTypeTTResp, sizeof(int));
   pps->ReadFileFH(&nNennImped, sizeof(int));
   pps->ReadFileFH(&dImpedLowFreqBR, sizeof(double));
   pps->ReadFileFH(&dImpedMidFreqBR, sizeof(double));
   pps->ReadFileFH(&dImpedHighFreqBR, sizeof(double));
   pps->ReadFileFH(&dMainResFreq, sizeof(double));
   pps->ReadFileFH(&dEffectivDBWm, sizeof(double));
   pps->ReadFileFH(&dLowLimitFreq, sizeof(double));
   pps->ReadFileFH(&dHighLimitFreq, sizeof(double));
   pps->ReadFileFH(&sTT1, sizeof(FilterBox));
   pps->ReadFileFH(&sTT2, sizeof(FilterBox));
   pps->ReadFileFH(&sMT1, sizeof(FilterBox));
   pps->ReadFileFH(&sMT2, sizeof(FilterBox));
   pps->ReadFileFH(&sMT3, sizeof(FilterBox));
   pps->ReadFileFH(&sHT, sizeof(FilterBox));
   pps->ReadFileFH(&dCorrectAmplTT12, sizeof(double));
   pps->ReadFileFH(&dCorrectAmplMT1, sizeof(double));
   pps->ReadFileFH(&dCorrectAmplMT2, sizeof(double));
   pps->ReadFileFH(&dCorrectAmplMT3, sizeof(double));
   pps->ReadFileFH(&dCorrectAmplHT, sizeof(double));
   pps->ReadFileFH(cTotalCorrect, sizeof(Complex)*CALCNOOFFREQ);
   pps->ReadFileFH(cZeroDegCorrect, sizeof(Complex)*CALCNOOFFREQ);
   pps->ReadFileFH(cSPTotalZeroStandard, sizeof(Complex)*CALCNOOFFREQ);
   if (nVersion >= 210)
   {
      pps->ReadFileFH(&nNoOfLowerHPFrequ, sizeof(long));
      pps->ReadFileFH(&nSelLowerHPFrequ , sizeof(long));
      pps->ReadFileFH(fLowerHPFrequ, sizeof(float)*nNoOfLowerHPFrequ);
      pps->ReadFileFH(&nNoOfUpperTPFrequ, sizeof(long));
      pps->ReadFileFH(&nSelUpperTPFrequ , sizeof(long));
      pps->ReadFileFH(fUpperTPFrequ, sizeof(float)*nNoOfUpperTPFrequ);
   }
}

/*************************************************************************************
* Definition : void WriteData(CBoxPropertySheet *);
* Zweck      : schreibt die Basissdaten in eine Datei
* Parameter (EA): pps : CBoxPropertySheet-Objekt          (CBoxPropertySheet*)
*                 beinhaltet die Dateifunktionen und Variablen
**************************************************************************************/
void BasicData::WriteData(CBoxPropertySheet *pps)
{
   int nVersion = pps->GetFileVersion();
   pps->WriteFileFH(&nBoxTyp, sizeof(int));
   pps->WriteFileFH(&nXoverTyp, sizeof(int));
   pps->WriteFileFH(&nTypeTTResp, sizeof(int));
   pps->WriteFileFH(&nNennImped, sizeof(int));
   pps->WriteFileFH(&dImpedLowFreqBR, sizeof(double));
   pps->WriteFileFH(&dImpedMidFreqBR, sizeof(double));
   pps->WriteFileFH(&dImpedHighFreqBR, sizeof(double));
   pps->WriteFileFH(&dMainResFreq, sizeof(double));
   pps->WriteFileFH(&dEffectivDBWm, sizeof(double));
   pps->WriteFileFH(&dLowLimitFreq, sizeof(double));
   pps->WriteFileFH(&dHighLimitFreq, sizeof(double));
   pps->WriteFileFH(&sTT1, sizeof(FilterBox));
   pps->WriteFileFH(&sTT2, sizeof(FilterBox));
   pps->WriteFileFH(&sMT1, sizeof(FilterBox));
   pps->WriteFileFH(&sMT2, sizeof(FilterBox));
   pps->WriteFileFH(&sMT3, sizeof(FilterBox));
   pps->WriteFileFH(&sHT, sizeof(FilterBox));
   pps->WriteFileFH(&dCorrectAmplTT12, sizeof(double));
   pps->WriteFileFH(&dCorrectAmplMT1, sizeof(double));
   pps->WriteFileFH(&dCorrectAmplMT2, sizeof(double));
   pps->WriteFileFH(&dCorrectAmplMT3, sizeof(double));
   pps->WriteFileFH(&dCorrectAmplHT, sizeof(double));
   pps->WriteFileFH(cTotalCorrect, sizeof(Complex)*CALCNOOFFREQ);
   pps->WriteFileFH(cZeroDegCorrect, sizeof(Complex)*CALCNOOFFREQ);
   pps->WriteFileFH(cSPTotalZeroStandard, sizeof(Complex)*CALCNOOFFREQ);
   if (nVersion >= 210)
   {
      pps->WriteFileFH(&nNoOfLowerHPFrequ, sizeof(long));
      pps->WriteFileFH(&nSelLowerHPFrequ , sizeof(long));
      pps->WriteFileFH(fLowerHPFrequ, sizeof(float)*nNoOfLowerHPFrequ);
      pps->WriteFileFH(&nNoOfUpperTPFrequ, sizeof(long));
      pps->WriteFileFH(&nSelUpperTPFrequ , sizeof(long));
      pps->WriteFileFH(fUpperTPFrequ, sizeof(float)*nNoOfUpperTPFrequ);
   }
}

/*************************************************************************************
* Definition : void BasicData::SortFilterArrays(bool)
* Zweck      : Sortiert die Filterarrays für Einwege LS
* Parameter (E): bUG: true : Filter für untere Grenzfrequenz                   (bool)
*                     false: Filter für obere Grenzfrequenz
**************************************************************************************/
void BasicData::SortFilterArrays(bool bUG)
{
   if (bUG)
   {
      CSortFunctions::SetSortDirection(true);
      qsort(fLowerHPFrequ, NO_OF_FILTER_FRQ, sizeof(float), CSortFunctions::SortFloat);
   }
   else
   {
      CSortFunctions::SetSortDirection(true);
      qsort(fUpperTPFrequ, NO_OF_FILTER_FRQ, sizeof(float), CSortFunctions::SortFloat);
   }
}

/*************************************************************************************
* Definition :void CheckFilterArrayRange();
* Zweck      : prüft die Filterarrays auf Konsistenz
**************************************************************************************/
void BasicData::CheckFilterArrayRange()
{
   long i; 
   if (nNoOfLowerHPFrequ)
   {
      for (i=0; i<nNoOfLowerHPFrequ; i++)
      {
         if ((fLowerHPFrequ[i] < dLowLimitFreq) || (fLowerHPFrequ[i] > dHighLimitFreq))
         {
            fLowerHPFrequ[i] = 0.0;
         }
      }
      SortFilterArrays(true);
   }

   if (nNoOfUpperTPFrequ)
   {
      for (i=0; i<nNoOfUpperTPFrequ; i++)
      {
         if ((fUpperTPFrequ[i] < dLowLimitFreq) || (fUpperTPFrequ[i] > dHighLimitFreq))
         {
            fUpperTPFrequ[i] = 0.0;
         }
      }
      SortFilterArrays(false);
   }

   CountFilterFrequencies();

   if (nNoOfLowerHPFrequ)
   {
      if (nSelLowerHPFrequ >= nNoOfLowerHPFrequ) nSelLowerHPFrequ = nNoOfLowerHPFrequ-1;
   }

   if (nNoOfUpperTPFrequ)
   {
      if (nSelUpperTPFrequ >= nNoOfUpperTPFrequ) nSelUpperTPFrequ = nNoOfUpperTPFrequ-1;
   }
}

/*************************************************************************************
* Definition : void CountFilterFrequencies()
* Zweck      : Ermittelt die Anzahlen der Filterfrequenzen
**************************************************************************************/
void BasicData::CountFilterFrequencies()
{
   for (nNoOfLowerHPFrequ=0; nNoOfLowerHPFrequ<NO_OF_FILTER_FRQ; nNoOfLowerHPFrequ++)
   {
      if (fLowerHPFrequ[nNoOfLowerHPFrequ] <= MIN_ACC_FLOAT) break;
   }

   for (nNoOfUpperTPFrequ=0; nNoOfUpperTPFrequ<NO_OF_FILTER_FRQ; nNoOfUpperTPFrequ++)
   {
      if (fUpperTPFrequ[nNoOfUpperTPFrequ] <= MIN_ACC_FLOAT) break;
   }
}

