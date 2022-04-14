// CARA3DDocFriend.h: Schnittstelle für die Klasse CCARA3DDocFriend.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARA3DDOCFRIEND_H__A9355060_02C6_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_CARA3DDOCFRIEND_H__A9355060_02C6_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define PLAN_GRAPH     1
#define POLAR_GRAPH    2
#define WATER_FALL     3
#define BARREL_DIAGRAM 4
#define WAVE_LET       5

// Deklarationen
class CCARA3DVDoc;
class CLevelColor;
class C3DGrid;
class CGlLight;
class CObjectTransformation;
struct ETS3DGL_Volume;

class CCARA3DDocFriend  
{
public:
	CCARA3DDocFriend();
   void  SetDocument(CCARA3DVDoc* p);
   // Datenzugriff
	void                   ReleaseDocData();                    // Dokumentendaten schützen
	void                   ProtectDocData();                    // Dokumentendaten freigeben
	CLevelColor           *GetLevelColors();                    // Farben
	C3DGrid               *GetGrid();                           // Gitter
	CGlLight              *GetLight(int);                       // Licht
	bool                   IsALightOn();                        // Licht
	CObjectTransformation *GetObjectTransformation();           // Transformation
	int                    GetObjectType();                     // Objekt
	const ETS3DGL_Volume  *GetObjectVolume() const;             // Objekt
	COLORREF               GetObjectColor();                    // Objekt
	double                 GetNormFactor();                     // Objekt
	double                 GetInverseNormFactor();              // Objekt
	double                 GetOffset();                         // Objekt
	double                 GetObjectDimension();                // Objekt
	double                 GetSpecificValue(int);               // Objekt
	const char *           GetSpecificUnit();                   // Objekt
	const char *           GetLevelUnit();                      // Objekt
	const char *           GetUnit();                           // Objekt
   int                    GetPictureIndex();                   // Objekt
	void                   SetPictureIndex(int);                // Objekt
   int                    GetNoOfPictures();                   // Objekt
	void                   SetObjectColor(COLORREF);            // Objekt
	void                   DefaultLevel();                      // Objekt
	void                   ShowPins(int, bool);                 // Pins
	bool                   ShowPins(int nType = -1);            // Pins
	int                    GetNoOfPins(int);                    // Pins
	bool                   Is3DObjectAvailable();               // Dokument
	bool                   AreLabelsAvailable();                // Dokument
	bool                   DoInterpolate();                     // Dokument
	int                    GetDetail();                         // Dokument
	bool                   CullFace();                          // Dokument
	bool                   SmoothShading();                     // Dokument
	bool                   LightTwoSided();                     // Dokument
	bool                   Lighting();                          // Dokument
	bool                   NormalBug();                         // Dokument
	float*                 GetAmbientColor();                   // Dokument
   // Funktionszugriff
	void                   DrawObject();                        // Object
	void                   DrawPinsText(HDC, int);              // Pins
	void                   DrawPins();                          // Pins

private:
   CCARA3DVDoc *m_pDoc;
};

#endif // !defined(AFX_CARA3DDOCFRIEND_H__A9355060_02C6_11D3_B6EC_0000B458D891__INCLUDED_)
