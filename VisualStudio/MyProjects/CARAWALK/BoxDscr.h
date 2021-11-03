#if !defined __CBOX_DESCRIPTION_DATA_H_INCLUDED_
#define __CBOX_DESCRIPTION_DATA_H_INCLUDED_

#include "CVector.h"
#include "Material.h"
#include "CEtsList.h"

#define BOX_NAMELENGTH 32
#define BOX_MAIN_MATERIAL -2

struct SPolygon
{
   int        nPoints;                    // Anzahl der Polygonpunkte
   CVector    vFlnv;                      // Flächennormalenvektor des Polygons
   int       *pnPoints;                   // Array mit Punktindizes
   int        nShapePoints;               // Anzahl der Rotationssymmetriepunkte
   CVector2D *pvShapePoints;              // Array mit Koordinaten
};

struct CARAWALK_BoxPos                    // Struktur für die Boxenposition
{
   long          nPhi;                    // Phiwinkel der Positionen
   long          nTheta;                  // Thetawinkel der Positionen
   long          nPsi;                    // Psiwinkel der Positionen
   double        pdPos[3];                // Positionen der Box
};

struct BoxPositions
{
   int nBox, nPosition;
};

class CBoxPosition
{
public:
   CBoxPosition()
   {
      ZeroMemory(this, sizeof(CBoxPosition));
      m_pfMatrix[0] = m_pfMatrix[5] = m_pfMatrix[10] = m_pfMatrix[15] = 1.0f;
      m_bSelected   = false;
   };
   static void DeleteFunction(void *p)
   {
      delete ((CBoxPosition*)p);
   }
   long          m_nPhi;                    // Phiwinkel der Positionen
   long          m_nTheta;                  // Thetawinkel der Positionen
   long          m_nPsi;                    // Psiwinkel der Positionen
   bool          m_bSelected;
   double        m_pdPos[3];                // Positionen der Box
   float         m_pfMatrix[16];
};

class  CFileHeader;
class  CCaraWalkDll;

class CBoxObj
{
public:
   CBoxObj();
   ~CBoxObj();
   void              DeleteData();
   void              DrawBox(bool, bool, int, HPALETTE, bool, double);
   void              DrawSelection(bool , int);
   void              SetGlobalBoxMaterial(bool, bool, int, HPALETTE, bool, double);
   bool              ReadType(HANDLE, CFileHeader *);
   bool              ReadData(HANDLE, CFileHeader *);
	bool              ReadMaterial(HANDLE, CFileHeader*, CCaraWalkDll*);
	bool              WriteMaterial(HANDLE, CFileHeader*);
   bool              AddPosition(CARAWALK_BoxPos&);
   void              DeleteAllPositions();
   int               GetNoOfPositions() {return m_BoxPosition.GetCounter();};
   bool              SetPosition(int, CVector, long, long, long);
   bool              SetPosition(int, CARAWALK_BoxPos&);
   bool              GetPosition(int, CARAWALK_BoxPos&);
   CBoxPosition     *GetpPosition(int);
   bool              SetMatrix(int nPos, double *pfM);
   bool              GetMatrix(int nPos, double *pfM);
   bool              MoveToPosition(int);
   void              Select(int, bool);
   CVector           GetURP()  {return m_vURP;};
   CVector           GetMYRP() {return m_vMYRP;};
   CVector           GetZDRP() {return m_vZDRP;};
   void              CalcMinMaxDimension(int nPos = -1, CVector *pvMin = NULL, CVector *pvMax = NULL, bool bInit = true, double dScale = 1.0);
   void              GetMinMaxDimension(CVector &vMin, CVector &vMax) {vMin = m_vMin; vMax = m_vMax;};
   void              SetFileName(char *szFn) {strncpy(m_szFileName, szFn, _MAX_FNAME);};
   const char       *GetBoxName()  {return m_szBoxName;};
   const char       *GetFileName() {return m_szFileName;};
   long              GetRotate()         {return m_nRotate;};
   long              GetBoxType()        {return m_nBoxType;};
   double            GetHeight() {return -Vy(m_vMin);};
	void              SetMaterial(CMaterial*);
   int               GetMaterialNo() {return (m_pMaterial!=NULL) ? m_pMaterial->nMatNo : -1;};

   int               GetNoOfPolygons() {return m_nPolygons + m_nChassisPolygons;}
	void              SetMaterial(int, CMaterial*);
   CMaterial *       GetMaterial(int);

   CVector           SelectPolygon(int);
   int               GetSelectedPolygon() {return m_nSelectedPolygon;};

   static  void      SetSpecularMatComp(float fRed, float fGreen, float fBlue) {gm_pfSpecular[0] = fRed; gm_pfSpecular[1] = fGreen; gm_pfSpecular[2] = fBlue;};

public:
	static void DeleteFunction(void*);
	void Init();
   bool           m_bShowURP;
private:
   static void    DrawFan(int, CVector*, void*);
   long           m_nBoxType;                                  // STAND_ALONE, MASTER, ADD_ON1/2/3, SLAVE
   long           m_nRotate;                                   // 0 alle Drehwinkel zugelassen
                                                               // 1 kein Psi (Kippen nicht zugelassen)
                                                               // kein Theta (Schwenken nicht zugelassen)
                                                               // nur Phi zugelassen
   char           m_szFileName[_MAX_FNAME];                    // Name der Boxendatei
   char           m_szBoxName[BOX_NAMELENGTH];                 // Name der Box
   int            m_nPolyPoints;                               // Anzahl aller vorhandener Punkte
   int            m_nBoxPoints;                                // Anzahl der Punkte die für das Gehäuse benötigt werden
   CVector       *m_pvPolyPoints;                              // Definitionspunkte
   int            m_nPolygons;                                 // Anzahl aller vorhandener GehäusePolygone
   int            m_nChassisPolygons;                          // Anzahl aller vorhandener ChassisPolygone
   SPolygon      *m_pPolygons;                                 // Polygone
   int            m_nSpheres;                                  // Anzahl der Kugeln 
   CVector       *m_pvSphereLocations;                         // Mittelpunktsvektoren der Kugeln
   float         *m_pfSphereRadius;		                        // die Radien der vorhandenen Kugeln
   CVector        m_vZDRP;                                     // Zero Degree Reference Point
   CVector        m_vURP;                                      // User Reference Point
   CVector        m_vMYRP;                                     // Maximum Y Reference Point
   CVector        m_vMin, m_vMax;                              // Maximale Boxdimensionen
   CEtsList       m_BoxPosition;
   CMaterial     *m_pMaterial;                                 // Grundmaterial
   CMaterial    **m_ppMaterial;                                // Material pro Polygon

   int            m_nSelectedPolygon;

   static float   gm_pfBoxColor[4];
   static float   gm_pfSelBoxColor[4];
   static float   gm_pfChassisColor[4];
   static float   gm_pfSpecular[4];
};

#endif // __CBOX_DESCRIPTION_DATA_H_INCLUDED_
