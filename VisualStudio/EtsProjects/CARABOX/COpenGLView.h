// Cara3DGL.h: Schnittstelle für die Klasse COpenGLView.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_COPEN_GL_BOX_VIEW_H_INCLUDED_)
#define _COPEN_GL_BOX_VIEW_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "Ets3DGL.h"	// Hinzugefügt von der Klassenansicht
#include "CVector.h"	// Hinzugefügt von der Klassenansicht

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PRE_CMD             9      // ID für Preset Commands
#define LIST_LIGHT         10      // Basis ID für Lichtquellen
#define LIST_MATRIX        30      // ID für die Objekttransformationsmatrix
#define LIST_SCALE         35      // ID Skalierung
#define LIST_CABINET       40      // Basis ID für Gehäuse
#define LIST_CABINET_SIZE 100      // Anzahl der Listen für Objekte
#define LIST_CHASSIS      140      // Basis ID für Chassis
#define LIST_USER_REF_PT  141

#define TRANS_MOVE         0
#define TRANS_TURN         1
#define TRANS_ZOOM         2
#define TRANS_TURNLIGHT    3
#define TRANS_LIGHT_INTENS 4
#define TRANS_LIGHT_SPOT   5
#define TRANS_SETTINGS     6
#define TRANS_COLORS       7

#define SHOW_SCALE         0x0001
#define SCALE_DIR_R        0x0002
#define SCALE_SIDE1        0x0010
#define SCALE_SIDE1B       0x0020
#define SCALE_CENTER       0x0040
#define SCALE_SIDE2B       0x0080
#define SCALE_SIDE2        0x0100
#define SCALE_SIDE         (SCALE_SIDE1|SCALE_SIDE1B|SCALE_SIDE2B|SCALE_SIDE2)
#define SCALE_SIDES        MAKELONG(SCALE_SIDE, SCALE_SIDE) 

//#define UNIT_METER
#define UNIT_FACTOR   1000.0
#define UNIT_QUOTIENT 0.001

#define ALPHA_BLENDING     0x00010000
#define ALPHA_DRAW_DIR     0x00020000

class CBoxPropertySheet;

class COpenGLView : public CEts3DGL
{
public:
	COpenGLView();
	COpenGLView(COpenGLView*);

	virtual ~COpenGLView();
   virtual void      InitGLLists();
   virtual void      DestroyGLLists();
   virtual void      OnRenderSzene();
   virtual LRESULT   OnTimer(long, BOOL);
	virtual bool      OnMessage(UINT, WPARAM, LPARAM);
	virtual LRESULT   OnCommand(WORD, WORD, HWND);
   virtual void      AutoDelete();
   virtual void      OnDrawToRenderDC(HDC, int);

   // Memberfunctions
	void InitGLView();
	void DetachData();
	void OnChangeCabinet();
	void SetProjection();
	bool CreateCabinetList(int nCab);
	void InvalidateTransMode(bool bReset=true, int nMode=-1);
	void InvalidateAllCabinets();
	void InvalidateAllLists();
	void Reset();
	void SetScalesOff(bool bMove=true);
	bool GetAnglePhiDiff(double&);
	bool IsScaleActive(bool bMove=true);
	void TurnBox(int np, int nt);
	void MoveBox(int x, int y, int z);

   void SetPropertySheet(CBoxPropertySheet*p){m_pPropertySheet=p;}
	CGlLight* GetLight(int);

   // statische Funktionen
	static bool    TextOut(HDC, double, double, double, UINT, int, char*);   // (Hdc, x, y, z, Align, Offset, Text)

   CVector  m_vAnchor;        // Objekt Transformation
	CVector  m_vTranslation;
	CVector  m_vScale;

   double   m_dZoom;
   double   m_dVolumeXY;
   double   m_dVolumeZ;

	double   m_dPhi;
	double   m_dTheta;
	double   m_dXi;            // dummy

	double   m_dTranCabX;
	double   m_dTranCabY;
	double   m_dTranCabZ;

   double   m_dPhiCab;
	double   m_dThetaCab;
	double   m_dXiCab;         // dummy

	double   m_dPhiLight;
   double   m_dThetaLight;    // Lichttransformation
	double   m_dDistanceLight;

   double   m_dLightIntens[3];// Helligkeit, Farbe

   double   m_dSpotCutExponent;// Strahlerparameter: Bündelung
   double   m_dSpotCutOff;    // Öffnungswinkel

	bool     m_bLighting;      // Beleuchtung
	float    m_AmbientColor[3];
   CGlLight m_Light;

	bool     m_bPerspective;   // Perspectivische Darstellung

   double   m_dAlphaBlend;    // Alphablending
   double   m_dDummySet;
   double   m_dShinines;      // Glanzeffekt
	int      m_nAlphaBlend;

   int      m_nTransMode;     // Modus: Bewegen, Drehen, Zoom,...

   int      m_nScaleModeX;    // horizontale Verschiebung
   int      m_nScaleModeY;    // vertikale Verschiebung
   int      m_nScaleModeZ;    // tiefen Verschiebung

   int      m_nScaleModePhi;  // Drehung in Phi
   int      m_nScaleModeTheta;// Drehung in Theta
   
   bool     m_bChanged[3];

   int      m_nScale;
   int      m_nResolution;
   bool     m_bShowBackSide;

   COLORREF m_cBox;
   COLORREF m_cChassis;
   COLORREF m_cScale;
   COLORREF m_cSpecular;

private:
	bool DrawCabinet3D(int);
   
   static void PreCommand(   COpenGLView*, int, int);
   static void LightCommand( COpenGLView*, int, int);
	static void ListMatrix (  COpenGLView*, int, int);
	static void ListScale(    COpenGLView*, int, int);
	static void ListCabinet(  COpenGLView*, int, int);
	static void ListChassis(  COpenGLView*, int, int);
	static void ListUserRefPt(COpenGLView*, int, int);
	static void ListLight(    COpenGLView*, int, int);

   static int  CountValidChassis(void*, WPARAM, LPARAM);
   static int  DrawChassis3D(void*, WPARAM, LPARAM);
	static void DrawBoxFan(int nCount, CVector *pV, void *pParam);

   CBoxPropertySheet *m_pPropertySheet;
	float  m_fPolygonFactor;
	int    m_nPolygonOffset;
};

#endif // !defined(_COPEN_GL_BOX_VIEW_H_INCLUDED_)
