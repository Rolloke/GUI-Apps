// PolarGraph.h: Schnittstelle für die Klasse CPolarGraph.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLARGRAPH_H__5951DD23_A717_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_POLARGRAPH_H__5951DD23_A717_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseClass3D.h"

class CPolarGraph : public CBaseClass3D  
{
public:
	CPolarGraph();
   virtual ~CPolarGraph();
   void     Init();
   void     DeleteData();
	bool     ReadCaleData(CArchive &a, CString &);
	bool     CalculateAngleVectors();
	double   GetRadius(int);
   void     ReadPolarData(CArchive &ar);
   void     ReadLevelData(CArchive &ar);
	bool     MapLevelDataFile(char*);

   // ab Version 1.1 nur je eine Funktion
   virtual void  ReadObjectData(CArchive &ar);
   virtual void  WriteObjectData(CArchive &ar);
   // Update
   virtual void  CheckMinMax();
	virtual bool  SetMinMax(double, double);
   // Zeichnen
   virtual void  DrawObject();
   // Datenzugriff
   virtual const char *GetSpecificUnit();
   virtual double      GetSpecificValue(int);
   virtual const char *GetLevelUnit();
	virtual char       *GetUnit();

private:
   char     *m_pszUnitOfRadius;
   float    *m_pfRadiusValues;
   CVector  *m_pvAngleVectors;
   double   *m_pdSpecificUnit;       // Feld mit den Werten für die Spezifische Einheit der Filmbilder
   long      m_nTheta;
   long      m_nPhi;
   long      m_nRadiusValues;
   long      m_nFlags;
   float     m_fDeltaPhi;            // Delta für Phi Winkel
   float     m_fDeltaTheta;          // Delta für Theta Winkel
   char     *m_pszPhaseUnit;         // Spezifische Einheit für Zeitanimation
   char     *m_pszSpecificUnit;      // Spezifische Einheit der Filmbilder
   double   *m_pdNormfactor;         // Feld mit den Normalisierungswerten für die Zeitanimation

   HANDLE    m_hFileForMapping;      // Dateihandle der Mapdatei
   HANDLE    m_hFileMapping;         // Mappinghandle
};

#endif // !defined(AFX_POLARGRAPH_H__5951DD23_A717_11D3_B6EC_0000B458D891__INCLUDED_)
