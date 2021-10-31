// PlanGraph.h: Schnittstelle für die Klasse CPlanGraph.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANGRAPH_H__D4B90FC1_F8B6_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_PLANGRAPH_H__D4B90FC1_F8B6_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseClass3D.h"

#define SAVED_VALUES              6

class CCARA3DVDoc;

class CPlanGraph : public CBaseClass3D 
{
public:
   // Konstruktion / Destruktion
   CPlanGraph();
   virtual ~CPlanGraph();
   void  DeleteData();
   // Dateifunktionen, das Lesen wurde wegen Abwärtskompatibilität in mehrere Funktionen aufgeteilt
   void  ReadLevelData(CArchive &ar, long nPictures = -1);
	bool  MapLevelDataFile(char*, long);
   bool  ReadNetData(CArchive &);
   void  ReadUnits(CArchive &ar);
	int   ReadSpecificUnit(CArchive &);
   void  Read_110_Data(CArchive &ar);
   bool  ReadTextFile(CArchive &ar);

   bool  IsFlat();

   // Zeitverlaufdarstellung
	virtual void  CalculatePhaseValues();
	virtual void  DeletePhaseValues();
   // ab Version 1.1 nur je eine Funktion
   virtual void  ReadObjectData(CArchive &ar);
   virtual void  WriteObjectData(CArchive &ar);
   // Update
   virtual void  CheckMinMax();
	virtual bool  SetMinMax(double, double);
   // Zeichnen
   virtual void  DrawObject();
   // Datenzugriff
   virtual double                  GetSpecificValue(int);
   virtual const char             *GetSpecificUnit();
   virtual const char             *GetLevelUnit();
	virtual char                   *GetUnit();

   COLORREF                GetColor()                {return CGlLight::FloatsToColor(m_pfColor);}
   void                    SetColor(COLORREF c)      {CGlLight::ColorToFloat(c, m_pfColor);}
   COLORREF                GetAmbientAndDiffuse()    {return CGlLight::FloatsToColor(m_pfAmbientAndDiffuse);}
   void                    SetAmbientAndDiffuse(COLORREF c){CGlLight::ColorToFloat(c, m_pfAmbientAndDiffuse);}
   COLORREF                GetSpecular()             {return CGlLight::FloatsToColor(m_pfSpecular);}
   void                    SetSpecular(COLORREF c)   {CGlLight::ColorToFloat(c, m_pfSpecular);}


protected:
	void SwapTimeParams();

private:
	void Init();

   // Daten
   long        m_nPlaneKoord;             // Anzahl der Objektpunkte in der Ebene
   long        m_nBorderIndexes;          // Anzahl der Randpunktindizes
   long        m_nFloorTriangles;         // Anzahl der Bodendreiecke
   long        m_nWebTriangles;           // Anzahl der Dreiecke im Darstellungnetz

   double     *m_pdSpecificUnit;          // Feld mit den Werten für die Spezifische Einheit der Filmbilder
   double     *m_pdNormfactor;            // Feld mit den Normalisierungswerten für die Zeitanimation
   Vector2Df  *m_pPlaneKoord;             // Punkt Koordinaten
   Vectorl3   *m_pFloorTriangles;         // Indizes für die Bodendreiecke
   long       *m_pnBorderIndex;           // Index auf die Contourpunkte
   Vectorl3   *m_pWebTriangles;           // Indizes für das Dreiecknetz
   float      *m_pfLevels;                // Werte für die Objektpunkte in der Ebene
   long       *m_pnZeroLevelPoints;       // Punkte mit Null-Potential
   float      *m_pfPhaseValues;           // Phasenwerte vorberechnet

   char       *m_pszLevelUnit;            // Einheit der Werte für die Objektpunkte
   char       *m_pszSpecificUnit;         // Spezifische Einheit der Filmbilder
   char       *m_pszPhaseUnit;            // Spezifische Einheit für Zeitanimation

   HANDLE      m_hFileForMapping;         // Dateihandle der Mapdatei
   HANDLE      m_hFileMapping;            // Mappinghandle

   float       m_pfColor[4];              // Farbe des Objektes
   float       m_pfAmbientAndDiffuse[4];  // Lichtfarbe des Objektes
   float       m_pfSpecular[4];           // Reflektionsfarbe des Objektes

   int         m_nRoundingLevel;          // Zeitdarstellung Rundung
   int         m_nNumModeLevel;           // Zeitdarstellung Zahlendarstellung
   double      m_dGridStepLevel;          // Zeitdarstellung Gitterschrittweite
   // vorberechnete Daten
   float       m_fFloorLevel;             // Boden-Level
};

#endif // !defined(AFX_PLANGRAPH_H__D4B90FC1_F8B6_11D2_9E4E_0000B458D891__INCLUDED_)
