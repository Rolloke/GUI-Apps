// Cara3DGL.h: Schnittstelle für die Klasse CCara3DGL.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARA3DGL_H__059A99E0_F99F_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_CARA3DGL_H__059A99E0_F99F_11D2_9E4E_0000B458D891__INCLUDED_

#include "Ets3DGL.h"	// Hinzugefügt von der Klassenansicht
#include "CARA3DDocFriend.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define EVENT_RENDER_TIMER 501   // Event Nr. des Rendertimers
#define EVENT_RENDER_READY 502   // Event Nr. des Renderers
#define EVENT_PRINT_READY  503   // Event Nr. des Printrenderers

#define PRE_CMD           9      // ID für Preset Commands
#define LIST_LIGHT       10      // Basis ID für Lichtquellen
#define LIST_MATRIX      30      // ID für die Objekttransformationsmatrix
#define LIST_PINS        32      // ID für die Stecknadeln
#define LIST_AXES        33      // ID für die Koordiantenachsen
#define LIST_CONE        34      // ID für den Pfeilkonus
#define LIST_SPHERE      35      // ID für Stecknadelkopf
#define LIST_OBJECT      40      // Basis ID für Objekte
#define LIST_OBJECT_SIZE 59      // Anzahl der Listen für Objekte
#define LIST_GRID        100     // Basis ID für die Gitterlisten
#define LIST_GRID_LEFT   101     // ID für Liste links
#define LIST_GRID_TOP    102     // ID für Liste oben
#define LIST_GRID_RIGHT  103     // ID für Liste rechts
#define LIST_GRID_BOTTOM 104     // ID für Liste unten
#define LIST_GRID_FRONT  105     // ID für Liste vorn
#define LIST_GRID_BACK   106     // ID für Liste hinten
#define LIST_GRID_SIZE     7     // Anzahl der Gitterlisten

#define CONE_SIZEFACTOR       0.02f
#define SPERE_RADIUSFACTOR    0.03f
#define NO_TRIANGLE_DIVISION       -4
#define TRIANGLE_DIVISION_BY_STEP  -3

typedef void (*TRIANGLEFUNC) (CVector *, CVector *, CVector *, CVector *, CVector *, CVector *, void *);

// Strukturen
struct STriangle
{
   double       dStep;        // maximale Seitenlänge
   int          nDetail;      // maximale Rekursionstiefe
   bool         bInterpolate; // Interpolieren
   TRIANGLEFUNC pFn;          // Zeiger auf Funktion für die erzeugten Punkte
   void        *pParam;       // Zeiger auf Parameter [optional] der Funktion
};

class CCara3DGL : public CEts3DGL, public CCARA3DDocFriend 
{
public:
	CCara3DGL();
	CCara3DGL(CCara3DGL*);
	virtual ~CCara3DGL();
   virtual void      InitGLLists();
   virtual void      DestroyGLLists();
   virtual void      OnRenderSzene();
   virtual LRESULT   OnTimer(long, BOOL);
	virtual bool      OnMessage(UINT, WPARAM, LPARAM);
	virtual LRESULT   OnCommand(WORD, WORD, HWND);
   virtual void      AutoDelete();
   virtual void      OnDrawToRenderDC(HDC, int);
   // Memberfunctions
	void InitCara3D();
	void DetachData();
	void CreateConeList();
   void SetGeneric(bool bGeneric) {m_Bits.nGeneric = bGeneric ? 1 : 0;};
   bool GetGeneric()     {return (m_Bits.nGeneric != 0) ? true : false;};
	void InvalidateGridList();
	void PerformGridList(bool, bool bChangeType=false);

   // statische Funktionen
	static void    CreateSphereList(float, int);                             // (Radius, Detail)
	static void    Triangle(STriangle *, CVector *, CVector *,CVector *, CVector *pn1=NULL, CVector *pn2=NULL, CVector *pn3=NULL, int nseite=0);
   static CVector Intersection(CVector *, CVector *, CVector *, CVector *); // (PlanOrigin, PlaneOrthogonal, LineOrigin, LineDirection)
	static bool    TextOut(HDC, double, double, double, UINT, int, char*);   // (Hdc, x, y, z, Align, Offset, Text)
	static void    LightedFans(int, CVector*, void*);

private:
   static void PreCommand(  CCara3DGL*, int, int);
   static void LightCommand(CCara3DGL*, int, int);
	static void ListMatrix ( CCara3DGL*, int, int);
	static void ListObject(  CCara3DGL*, int, int);
	static void ListPins(    CCara3DGL*, int, int);
	static void ListGrid(    CCara3DGL*, int, int);
	static void ListAxes(    CCara3DGL*, int, int);
	static void ListLight(   CCara3DGL*, int, int);

   static void DirectPostCallListCommands(CCara3DGL*, int, int);

   GLUquadricObj    *m_pCone;
};

#endif // !defined(AFX_CARA3DGL_H__059A99E0_F99F_11D2_9E4E_0000B458D891__INCLUDED_)
