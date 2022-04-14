#if !defined __CROOM_DESCRIPTION_DATA_H_INCLUDED_
#define __CROOM_DESCRIPTION_DATA_H_INCLUDED_

#include "CVector.h"
#include "Material.h"

void ReadData(void *, void *, int, long&);

class CGlLight;
struct ETS3DGL_Fan;

class CRoomObj
{
public:
   CRoomObj();
   ~CRoomObj();
   void        DeleteData();
   long        SetRoomData(void *pData);
   bool        Read(HANDLE);
   void        DrawRoom(int ,bool, bool, int, HPALETTE, double);
//   void        DrawRoomEx(int ,bool, bool, int, HPALETTE, double);
   void        DrawWall(int, CMaterial*, ETS3DGL_Fan&, bool, bool);
   void        DrawImage(int, int, bool, CVector&, ETS3DGL_Fan&);
   CMaterial  *GetMaterial(int);
   CMaterial  *GetMaterial(char *, bool);
   CMaterial  *GetMaterialFromIndex(int&, CMaterial*pMS=NULL);
   double      GetMaxDimension() {return m_dMaxDimRoom;};
   int         GetNoOfBoxes()    {return m_nNoOfBoxes;};
   void        GetRoomDimensions(CVector &vLBN, CVector &vRTF)
               {vLBN = m_vLeftBottomNear; vRTF = m_vRightTopFar;};
   CVector     GetCenter();
   bool        IsInside(CVector *pvEye);
   bool        SetPaletteAndIndexes(HPALETTE);
   void        GetBmpFile(int, BITMAPINFO**);
   void        CalculateShadowMatrixes(CGlLight *);
   float      *GetWallMatrix(int nWall);
   bool        IsValidMatrix(int nWall);
   int         GetNoOfWalls()    {return m_nNoOfWalls;};

   static void GLToCara(CVector*);
	static void CaraToGL(CVector*);
	static double Det1(CVector&, CVector&);
	static double Det2(CVector&, CVector&);
	static double Det3(CVector&, CVector&);

private:
	void        Init();
   bool        InitMaterials();
   void        CARAtoOpenGL();
	int         CheckIntersection(CEtsList *, int *);
   static void DrawFan(int , CVector *, void *);

private:
   double    m_dMaxDimRoom;                                    // max. Ausdehnung (Raumdiagonale) des Raumes in [m]
   int       m_nNoOfWalls;                                     // Anzahl der Raumwände (konvexe Polygone)
   int       m_nNoOfPoints;                                    // Gesamtanzahl der Definitionspunkte
   int       m_nNoOfAllImageCorners;                           // Anzahl aller Eckpunkte aller Bilder
   int       m_nNoOfBoxes;
   CVector  *m_pvPoints;                                       // Feld mit den Definitionspunkten
   CVector  *m_pvNormalWall;                                   // Feld mit den Normalenvektoren der Wände
   int      *m_pnNoOfCornerWalls;                              // Feld mit Anzahl der Eckpunkte (konvexes Polygon) der Raumwand[i]
   int     **m_ppnIndexCornerWall;                             // Array mit Indizes auf die Definitionspunkte (Polygonpunkt[j] der Raumwand[i])
   long     *m_pnIndexMaterialWall;                            // Materialnummer des Grundmaterials der Raumwand[i]
   int      *m_pnNoOfImagesWall;                               // Feld mit der Anzahl der Materialbilder der Raumwand[i]
   int     **m_ppnNoOfCornerImages;                            // Array mit Anzahl der Eckpunkte des Bildes[j] der Raumwand[i]
   CVector  *m_pvCornerImage;                                  // Feld mit dem Eckpunkt[k] des Bildes[j] der Raumwand[i]
   long    **m_ppnIndexMaterialImage;                          // Array mit der Materialnummer des Bildes[j] der Raumwand[i]
   CEtsList  m_Material;
   
   CVector   m_vLeftBottomNear,                                // Umhüllender Quader der Raumes
             m_vRightTopFar;
   float    *m_pfWallMatrix;                                   // Matrix für Schattenprojektion
public:
	CMaterial* GetMaterial(char*pszBmpFile);
protected:
	void DeleteMaterials();
};

#endif // __CROOM_DESCRIPTION_DATA_H_INCLUDED_
