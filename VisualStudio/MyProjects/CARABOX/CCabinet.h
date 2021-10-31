#ifndef __CCABINET_H_INCLUDED
#define __CCABINET_H_INCLUDED

#define STRICT
#include <WINDOWS.H>

#include "CVector.H"
#include "BoxStructures.h"	// Hinzugefügt von der Klassenansicht
#include "CEtsList.h"

class ChassisData;

#define NO_OF_CABINET_POINTS 8
#define NO_OF_RECT_POINTS    4
#define NO_OF_WALLS          6

#define BASE_CABINET         0            // Index des Basisgehäuses

#define CAB_NAME_LEN 64

#define NO_BASE_CABINET        0x00000001 // Zusatzgehäusesegment
#define CHECK_ALL_WALLS        0x0000007e // Prüfe die Position der Chassis auf diesen Wänden
#define CAN_CONTAIN_CHASSIS    0x00000080 // Gehäuse kann Chassis enthalten

#define CALC_2D_CHASSISPOS     0x00000100 // 2D-Position muß neu berechnet werden
#define CALC_3D_CHASSISPOS     0x00000200 // 3D-Position muß neu berechnet werden

#define WIDTH_1_2_ON_BACK      0x00010000 // Breite des Quaders wird Hinten statt oben verändert
#define CANNOT_CONTAIN_CHASSIS 0x00020000 // Gehäuse kann keine Chassis enthalten

#define COMPLEX_CAB_DESIGN     0x00100000 // Gehäusekonstruktion ist "komplexer" (nicht Quaderförmig)
#define ZERO_DEG_REF           0x00e00000 // 3 Flags für Referenzschallwand
#define ZERO_DEG_REF_FRONT     0x00000000 // vorne ist Standard : 0 !
#define ZERO_DEG_REF_LEFT      0x00200000 // links
#define ZERO_DEG_REF_RIGHT     0x00400000 // rechts
#define ZERO_DEG_REF_BACK      0x00800000 // hinten
#define DISABLED_WALLS         0x3f000000 // 6 Flags, ob die Wände dargestellt werden müssen

#define NO_TRANSLATION     0x0001
#define NO_ROTATION        0x0002

#define CAB_PT_LEFT_TOP_FRONT     0
#define CAB_PT_RIGHT_TOP_FRONT    1
#define CAB_PT_RIGHT_TOP_BACK     2
#define CAB_PT_LEFT_TOP_BACK      3
#define CAB_PT_LEFT_BOTTOM_FRONT  4
#define CAB_PT_RIGHT_BOTTOM_FRONT 5
#define CAB_PT_RIGHT_BOTTOM_BACK  6
#define CAB_PT_LEFT_BOTTOM_BACK   7

#define  START_VAL 20

struct PointFloat;
union  Array44;
class  CBoxPropertySheet;

class CCabinet
{
public:
   CCabinet();
   ~CCabinet();
	void Init();

	void     AdaptCoupledPosition(ChassisData*);
   CVector  GetCabinetCenter();  // vCenter
	double   InitCabPoints();  // dVolume ()
	bool     CalcChassisPos2D(ChassisData*);
	bool     CalcChassisPos3D(ChassisData*);
	bool     GetCabinetWall(int, CVector2D*, CVector *pvNorm=NULL, CVector *pvWall3D=NULL);// (nCabinetWall, vrcWall2D, pvNorm, pvWall3D)
	void     SetChanged(DWORD);
	void     CountAllChassis();
	void     WriteData(CBoxPropertySheet*);
	void     ReadData(CBoxPropertySheet*);
	
   double      GetBottomZPos();   // dZPosition
	PointFloat  VectorToBDDpoint(CVector, double dFactor=0.001, int nCabinet=0);// (vIn, dFactor=0.001, nCabinet=0)
	double      GetVolume(double dThickness=0.0); // dVolume (dThickness=0.0)
	double      GetMaxDimension(); // dMaxdimension
	CCabinet*   GetCabinet(int);   // (nCabinet)

	bool        IsWallEnabled(int); // (nWall)
   bool        IsBaseCabinet() {return ((m_dwFlags & NO_BASE_CABINET) ? false : true);};
   bool        CheckWall(int); // (nWall)
   bool        CanContainChassis() {return ((m_dwFlags & CAN_CONTAIN_CHASSIS) ? true : false);};
	void        EnableWall(int, bool); // (nWall, bEnable)
   PointFloat *GetCabinetPolyPoints(int&); // (nCount)
	bool        GetCabinetPolygons(int &, int ***, int**); 
	CVector     GetCabPoint(int, bool bTransform=false); // vPoint (nCabinetWall, bTransform=false)
	int         GetOppositePoint(int, int); // nIndex (nWall, nPoint)
   CVector     GetShift() {return m_vShift;};
	double      GetYShift(WORD, CVector *pV=NULL);
	double      GetXShift(WORD, CVector *pV=NULL);
	void        SetTransformation();
	int         SetBaseCabinet(CCabinet*);    // nNoOfSegments (pBaseCabinet)

   bool        InitWallSegmentArray(DWORD);
//   bool        IsChassisOnWall(ChassisData*);
//   bool        IsPointInTriangle(CVector, CVector, CVector, CVector);
   int         GetNoOfWallSegments() {return m_nNoOfWallSegments;};
   const DWORD*GetWallSegmentArray() {return (const DWORD*)m_pdwWalls;};
	int         FindWallSegment(int);

	static double    GetXCenterLine(CVector2D*, int nTop=1); // dCenter (pvrcWall2D, nTop=1)
	static CVector2D Intersection(CVector2D&, CVector2D&, CVector2D&, CVector2D&); // vSection (vR1, vA1, vR2, vA2)
	static bool      IsOnWall(CVector2D*, ChassisData*, CVector2D *pvQ=NULL, bool bInit=false);
	static double    Distance(CVector2D, CVector2D, CVector2D); // dDistance (vR1, vR2, vQ)
	static bool      InitChassisPos(CVector2D *, ChassisData *, bool bTest=false); // (pvrcWall, pCD)
	static CVector2D GetWallCenter(CVector2D*);
	static double    GetWallArea(CVector2D*);

   double   dHeight;          // Höhe (alle Maße in mm)
   double   dWidth;           // Breite
   double   dDepth;           // Tiefe
   double   dWidth1;          // zusätzliche Maße für Pyramidenform
   double   dWidth2;          // verringert die Breite oben
   double   dDepth1;          // verringert die Tiefe oben
   double   dDepth2;
   int      nCountAllChassis; // Basis Quader   : Anzahl aller Chassis
                              // NO_BASE_CABINET: Nummer des Quaders
   int      nCountBR;         // Chassis-Typ-Index 0 (Insgesamt 9 Typen)
   int      nCountTL;         // Basis Quader   : Chassis-Typ-Index 1
                              // NO_BASE_CABINET: > 0: Quader ist der Abstandhalter (AH) für ein Chassis unten
                              // zeigt die Gesamtanzahl der gekoppelten Quader !
   int      nCountPM;         // Basis Quader   : Chassis-Typ-Index 8
                              // NO_BASE_CABINET: Position des Abstandhalters Format: 0x000A0B0C
                              // A: Gehäusesegment (0,..n), B: PunktIndex (0,..,3), C: WandIndex (0,1,..,6)
   int      nCountTT1;        // Chassis-Typ-Index 2
   int      nCountTT2;        // Chassis-Typ-Index 3
   int      nCountMT1;        // Chassis-Typ-Index 4
   int      nCountMT2;        // Chassis-Typ-Index 5
   int      nCountMT3;        // Chassis-Typ-Index 6
   int      nCountHT;         // Basis Quader   : Chassis-Typ-Index 7
                              // NO_BASE_CABINET: Quader ist der Abstandhalter (AH) für eine Kugel (Anzahl = 1 !)
   CVector  vPosRef;          // Basis Quader   : ZeroDegRefPunkt = Mitte aller HT auf Schallwand (vorne) in mm
                              // NO_BASE_CABINET: Verschiebevektor des Quaders
   CVector  vUserRef;         // Basis Quader   : UserRefPunkt 
                              // NO_BASE_CABINET: Drehwinkel Vy = Phi, [Vz = Theta]
   DWORD    m_dwFlags;        // Wandparameter
   double   m_pdTran[4][4];   // Basis Quader   : Rotationsmatrix für abweichende Bezugsschallwand
                              // NO_BASE_CABINET: Rotations- und Translationsmatrix
   CEtsList m_Cabinets;       // Weitere Quader
   char     m_szName[CAB_NAME_LEN];// Name des Quaders
   
   static const int gm_nWallPoints[NO_OF_WALLS][NO_OF_RECT_POINTS];
   static const int gm_nOppositePoints[NO_OF_WALLS][NO_OF_RECT_POINTS];

private:
   CVector  m_vCabPoints[NO_OF_CABINET_POINTS]; // Definitionspunkte des Gehäuses
   CVector  m_vShift;         // Verschiebevektor vom Zentrum der Box
   DWORD   *m_pdwWalls;       // Array für Gehäuseteil-/Wandkombinationen, die in einer Ebene liegen
                              // HIWORD Nummer des Gehäuses (0,1,2,...), LOWORD Nummer der Wand (1,..6)
   int      m_nNoOfWallSegments;// Anzahl der Wandsegmente, die in einer Ebene liegen

   static void DeleteFnc(void*);
};

#endif //__CCABINET_H_INCLUDED