// Material.h: Schnittstelle für die Klasse CMaterial.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIAL_H__6B306F65_E044_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_MATERIAL_H__6B306F65_E044_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CEtsList.h"

struct Image
{
   int     nImageWall;            // Indizes der Wände mit diesem Bild (Normalenvektor ermitteln)
   int     nImageNo;              // Nummer dieses Bildes auf der Wand
   int     nNoOfImagePoints;      // Anzahl der Punkte dieses Bildes
   int     nStartIndexImagePoints;// Startindex für die Punkte des Bildes
};

class CVector;

#define DEFAULT_MATERIAL 10000

class CMaterial  
{
public:
	CMaterial();
	~CMaterial();
   bool  SetTexture(BITMAPINFO*);
	void  RandomMaterial();
   BYTE *GetTexImage() const {return m_pTexImage;};
	void  ImageMaterialAndLight(bool, bool, HPALETTE, float fAlpha = 1.0f);
   void  AlignTexture(CVector &);
	void  CreateTextureObject();
	bool  IsTextureObject();
   bool  ReadFile(HANDLE);
   bool  SaveFile(HANDLE);
	const char * GetBitmapFileName()const {return (const char*)m_pszFileName;};
	void  SetBitmapFileName(const char *);

//   void  SaveFile(HANDLE);

   static void DeleteFunction(void*);

   int      nMatNo;                    // Materialnummer
   CEtsList m_Walls;                   // Wände mit diesem Material
   CEtsList m_Images;                  // Bilder mit diesem Material
   // Materialparameter
   float    m_pfAverageImageColor[3];  // Durchschnittsfarbe für Darstellung ohne Texturen
   float    m_pfAmbient[4];            // Umgebungslichtkomponente des Materials
   float    m_pfDiffuse[4];            // Diffuse Lichtkomponente des Materials
   float    m_pfSpecular[4];           // Reflexionseigenschaften des Materials
   float    m_pfEmision[4];            // Eigenlichtausstrahlung des Materials
   float    m_fShininess;              // Durchsichtigkeit
   // Texturparameter
   int      m_nWidth, m_nHeight;       // Breite, Höhe der Textur
   BOOL     m_bBorder;                 // Rand für die Materialstruktur
   COLORREF m_cBorder;                 // Farbe des Randes
   BOOL     m_bGenerateCoordinates;    // Erzeugen von Texturkoordinaten
   float    m_fsScaleFactor;           // Skalierungsfaktor für die Bitmap
   float    m_ftScaleFactor;           // Skalierungsfaktor für die Bitmap

   bool     m_bSaveBmpAsFile;
   // Voreinstellungen
   static float gm_pfAmbient[4];       // Umgebungslichtkomponente des Materials
   static float gm_pfDiffuse[4];       // Diffuse Lichtkomponente des Materials
   static float gm_pfSpecular[4];      // Reflexionseigenschaften des Materials
   static float gm_fb;                 // Komprimierung des Intensitätsbereichs
   static float gm_fm;       
   static float gm_fAmbient;           // Faktor für Umgebungslichtkomponente des Materials

private :
   char*    m_pszFileName;             // Name einer Bitmap-Datei für die Materialstruktur
   BYTE*    m_pTexImage;               // Textur
   GLuint   m_nTexture;
};

#endif // !defined(AFX_MATERIAL_H__6B306F65_E044_11D2_9E4E_0000B458D891__INCLUDED_)
