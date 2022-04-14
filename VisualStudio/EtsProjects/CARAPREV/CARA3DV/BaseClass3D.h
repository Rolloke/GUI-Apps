// BaseClass3D.h: Schnittstelle für die Klasse CBaseClass3D.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASECLASS3D_H__5951DD22_A717_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_BASECLASS3D_H__5951DD22_A717_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ets3DGL.h"
#include "CARA3DDocFriend.h"

#define PHASE_SPECIFIC_UNIT      "deg"          // Einheit für die Zeitanimation in Grad (0,..,360)
#define PHASEVALUES_AVAILABLE    0x00000001     // Anfangsphasen für die Zeitanimation vorhanden ?
#define LEVELVALUES_LOG          0x00000002     // Amplitudenwerte logarithmiert
#define LEVELVALUES_IN_DB10      0x00000004     // Amplitudenwerte (10 * log(P1/p2))
#define LEVELVALUES_IN_DB20      0x00000008     // Amplitudenwerte (20 * log(P1/p2))
#define NO_VALUE_NORMALIZATION   0x00000010     // Kein Normalisierungsfaktor
#define ONE_NORMALIZATION_FACTOR 0x00000020     // Ein Normalisierungsfaktor für alle Bilder
#define N_NORMALIZATION_FACTORS  0x00000040     // n Normalisierungsfaktoren für n Bilder
#define PHASEVALUES_CALCULATED   0x00020000     // Phasenwerte für die Animation berechnet
#define CALCULATE_PHASEVALUES    0x00040000     // Phasenwerte müssen für die Animation berechnet werden
// Polarspezifisch
#define PHI_VALUES_COMPLETE      0x00100000     // Phiwerte 0°,..,360°, sonst 0°,..,360°-(360/nPhi)°
#define SPECIFICVALUES_AVAILABLE 0x00200000     // Werte für die Spezifische Einheit der Filmbilder (5,..,40960) Hz

#define NO_OF_PHASEVALUES        72             // 5°-Schritte für die Animation

struct Vector2Df
{
   float x, y;
};

struct Vectorl3
{
   long p[3];
};

class CBaseClass3D : public CCARA3DDocFriend  
{
public:
	CBaseClass3D();
	virtual ~CBaseClass3D();
   int GetClass3DType() {return m_nClass3DType;};

   const ETS3DGL_Volume   *GetObjectVolume();
   double                  GetMaxDimension()     {return m_fMaxDimension;}
   double                  GetNormFactor()       {return m_dNormFactor;};
   double                  GetInverseNormFactor(){return m_dInvNormFactor;};
   double                  GetOffset()           {return m_dOffset;};
	void                    SetNormFactor(double);
	int                     GetNoOfPictures();
	void                    InvalidateVolume();
   
   bool DoCalculatePhaseValues() {return ((m_nFlags & CALCULATE_PHASEVALUES) !=0 ) ? true : false;}
   bool PhaseValuesAvailable()   {return ((m_nFlags & PHASEVALUES_AVAILABLE) !=0 ) ? true : false;}
   bool PhaseValuesCalculated()  {return ((m_nFlags & PHASEVALUES_CALCULATED) !=0 ) ? true : false;}

   // Archivierung
   virtual void        ReadObjectData(CArchive &ar)   = 0;
   virtual void        WriteObjectData(CArchive &ar)  = 0;
   // Update
   virtual void        CheckMinMax() {m_bClipped = false;};
   virtual void        DefaultLevel();
	virtual bool        SetMinMax(double, double)      = 0;
   // Zeichnen
   virtual void        DrawObject()                   = 0;
   // Datenzugriff
   virtual double      GetSpecificValue(int nIndex) {return (double) nIndex;};
   virtual const char *GetSpecificUnit()  {return SPACE_STRING;};
   virtual const char *GetLevelUnit()     {return SPACE_STRING;};
	virtual char       *GetUnit()          {return SPACE_STRING;};
   // Zeitverlaufdarstellung
   virtual void        CalculatePhaseValues() {};
   virtual void        DeletePhaseValues()    {};
protected:
   // statische  Funktionen
   static void FlatLevelTr(       CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);
   static void FlatLightLevelTr(  CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);
   static void SmoothLevelTr(     CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);
   static void SmoothLightLevelTr(CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);

   static void FlatRadiusTr(       CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);
   static void FlatLightRadiusTr(  CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);
   static void SmoothRadiusTr(     CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);
   static void SmoothLightRadiusTr(CVector*,CVector*,CVector*,CVector*,CVector*,CVector*,void *);

   // Daten
   int            m_nClass3DType;         // Object type

   long           m_nFlags;               // Flags für Zeitanimation
   long           m_nFilmPictures;        // Anzahl der Film-Bilder

   double         m_dNormFactor;          // Normierungsfaktor
   double         m_dInvNormFactor;       // Normierungsfaktor invertiert
   double         m_dOffset;              // Offset
   double         m_dMinValue;            // Minimaler Darstellungswert
   double         m_dMaxValue;            // Maximaler Darstellungswert
   bool           m_bClipped;
   // vorberechnete Daten
   float          m_fMaxDimension;        // Maximale Objektausdehnung
   ETS3DGL_Volume m_ObjectVolume;         // Ausdehnung des Objektes
};

#endif // !defined(AFX_BASECLASS3D_H__5951DD22_A717_11D3_B6EC_0000B458D891__INCLUDED_)
