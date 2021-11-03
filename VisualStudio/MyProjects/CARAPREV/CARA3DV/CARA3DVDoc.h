// CARA3DVDoc.h : Schnittstelle der Klasse CCARA3DVDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARA3DVDOC_H__30FA9E7E_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_CARA3DVDOC_H__30FA9E7E_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "3DGrid.h"
#include "ETS3DGL.h"	               // Hinzugefügt von der Klassenansicht
#include "LevelColor.h"	            // Hinzugefügt von der Klassenansicht
#include "ObjectTransformation.h"	// Hinzugefügt von der Klassenansicht
#include "Pins.h"
#include "..\CARADoc.h"

#define NO_OF_LIGHTS 3

// ViewTimerEvents
#define EVENT_UPDATETIMER    400 // Event Nr. des Updatetimers
#define TIME_UPDATETIMER    3000 // Zeit in [ms]
#define EVENT_EFFECTTIMER    401 // Event Nr. des Effecttimers
#define TIME_EFFECTTIMER    1000 // Zeit in [ms]
#define EVENT_INITIALIZE_GL  402 // Event Nr. für Initialisieren
#define EVENT_INVALIDATE_GL  403 // Event Nr. für Updaten

#define UPDATE_FRAME_CONTENT   0x1000

class CCARA3DVView;
class CBaseClass3D;
class CColorLabel;

struct Cara3DLight
{
   CGlLight glLight;
   float    fTheta;
   float    fPhi;
   float    fDistance;
};

class CCARA3DVDoc : public CCARADoc
{
protected: // Nur aus Serialisierung erzeugen
	CCARA3DVDoc();
	DECLARE_DYNCREATE(CCARA3DVDoc)

// Attribute
public:
#ifdef ETS_OLE_SERVER
protected:
	virtual COleServerItem* OnGetEmbeddedItem();

public:
   void SetModifiedFlag(BOOL bModified=true);
   bool m_bOleChanged;
#endif

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCARA3DVDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual void ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementierung
public:  
   // virtual
   virtual ~CCARA3DVDoc();
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
   
   // Members
   void CalculateLightPosition(int nLight = 0);
	void AttachLevelColor();
	void SetDefaultColors();
	void Read3DObject(CArchive&ar);
	void Write3DObject(CArchive&ar);
	void InitCriticalSectionOfDoc();
	void DeleteCriticalSectionOfDoc();
	void SetActiveHeadline();
	void SetHeadlinePositions(CPoint ptTopLeft);

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;


#endif

protected:
   void ReadLabelPositions(CArchive& ar);
	bool ReadLabelObjects1_00(CArchive& ar);
	bool ReadVersion1_00(CArchive&);
	bool ReadNetFromCalcProg(CArchive&);
	bool ReadDpoFromCalcProg(CArchive&);
   void WriteLabelPositions(CArchive& ar);

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CCARA3DVDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Public Data
public:
	LPCTSTR GetHeadLineText();
   CTextLabel          **m_ppHeadLines;   // Überschriften
   CPictureLabel        *m_pDibLabel;     // Rechteck mit der DibSection
   CColorLabel          *m_pColorLabel;   // Farbstufen des Objektes
   CBaseClass3D         *m_p3DObject;     // Darstellungsobjekt
   C3DGrid               m_Grid;          // Gitter und Achsen
   CPins                 m_Pins;          // Stecknadeln
	CObjectTransformation m_ObjTran;       // Transformation des Darstellungsobjektes
	CLevelColor           m_LevelColor;    // Farben für die Z-Werte
	Cara3DLight           m_Light[NO_OF_LIGHTS];// Beleuchtung des Objektes
   CRITICAL_SECTION     *m_pcsDocumentData;// CriticalSection für die Dokumentendaten
   long                  m_nHeadLines;    // Anzahl der Überschriften
   int                   m_nDetail;       // Grafikauflösung
   bool                  m_bInterpolate;  // Interpolation der Dreiecke
   bool                  m_bSmoothShading;// Weiche Farbübergänge
   bool                  m_bLighting;     // Beleuchtung
   bool                  m_bCullFace;     // Flächen nur von einer Seite zeigen
   bool                  m_bLightTwoSided;// Flächen von beiden Seiten beleuchten
   float                 m_pfAmbientColor[4];// Umgebungslicht
// Initialisierende Parameter
   COLORREF              m_ObjectColor;   // Farbe des Objektes
	int                   m_nProjection;   // ObjectProjection
   int                   m_nPictureIndex; // Bildindex
   bool                  m_bPrintInit;    // Initialisierung von Drucken und Druckvorschau ist erfolgt
   bool                  m_bNormalBug;    // Setzen des Normalenvectors
// Private Data
private:
	void InitLight(int nLight);
	void Init();
	char * m_pszTempFileName;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_CARA3DVDOC_H__30FA9E7E_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
