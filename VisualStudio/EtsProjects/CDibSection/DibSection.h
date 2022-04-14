/*********************************************************************
                              CDibSection
  Zweck:
   CDibSection kapselt Funktionen zum Arbeiten mit DibSections und
   Paletten.

  Copyright:
   (C) 1999 ELAC Technische Software GmbH

  Version:
   1.00, Stand 13.09.1999

                                          programmed by Rolf Ehlers
*********************************************************************/
#if !defined __DIBSECTION_H_INCLUDED_
#define __DIBSECTION_H_INCLUDED_

#ifndef STRICT
 #define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>

#define ALIGN_LONG(I) ( ( ( ( (I) >> 3 ) + 3 ) >> 2 ) << 2 )

class CDibSection  
{
struct RGBTRIPPLE
{
   BYTE rgbRed;
   BYTE rgbGreen;
   BYTE rgbBlue;
};
struct RGB555
{
   unsigned char rgbRed   :5;
   unsigned char rgbGreen :5;
   unsigned char rgbBlue  :5;
   unsigned char dummy    :1;
};
struct RGB565
{
   unsigned char rgbRed   :5;
   unsigned char rgbGreen :6;
   unsigned char rgbBlue  :5;
};
public:
   // Konstruktion, Destruktion
   CDibSection();
   virtual ~CDibSection();
   bool     Create(HDC hdc, int cx, int cy, HANDLE hFile=NULL, DWORD dwOffset=0);
   bool     CreateIndirect(BITMAPINFO *);
	int      CreateBmpBW(int, int, int, HDC);
	int      Convert32to24bit();
   int      ConvertBitCount(int);
   HBITMAP  Detach();
   BOOL     IsValid() {return ((m_hDC!=NULL) && (m_hBmp!=NULL) && (m_pBmi!=NULL));};
   void     Destroy();
	void     DestroySecondBmp();
   // Zugriff auf Daten und Members
   HDC      GetDC()           {return m_hDC;};
   void    *GetBits(bool bFirst=true);
   HBITMAP  GetBitmapHandle() {return m_hBmp;};
   HBITMAP  GetSecondBitmapHandle() {return m_hSecondBmp;};
   bool     SwapBitmaps();
   bool     DuplicateBitmap();
   HPALETTE GetPaletteHandle(){return m_hPalette;};
   int      GetWidth();
   int      GetHeight();
   int      GetNumColors()    {return ((m_nColorBits !=0) ? (1 << m_nColorBits) : 0);};
   UINT     GetColorUsage()   {return m_iUsage;};
   DWORD    GetImageSize();
   HGLOBAL  GetDIBData(UINT*pnSize=NULL, RECT*prcBmp=NULL);
	void     SetColorUsage(HDC hDC);
   void     SetColorUsage(int u, int nNumColors=0);
   bool     SetNewSize(SIZE *);
   const BITMAPINFO *GetBitmapInfo() {return m_pBmi;};
   const BITMAPINFO *GetSecondBitmapInfo() {return m_pSecondBmi;};
   COLORREF GetPixel(int, int);
   void     SetPixel(int x, int y, COLORREF);
   // Ausgabe auf Gerätekontext
   int      StretchDIBits(HDC, RECT, bool bSecond=false, DWORD dwRop = SRCCOPY);
   int      SetDIBitsToDevice(HDC, RECT, bool bSecond=false);
   bool     BitBlt(HDC, int, int, int, int, DWORD);
   bool     StretchBlt(HDC, int, int, int, int, DWORD);
   // Resourcenfunktionen
   bool        LoadFromResource(HINSTANCE, int nID);
   // Dateifunktionen
   bool        LoadFromFile(HANDLE);
   bool        SaveInFile(HANDLE);
   // Palettenfunktionen
   UINT        RealizePalette();
   void        OnPaletteChanged();
   UINT        SetPaletteColor(int i, COLORREF c);
   UINT        GetPaletteEntries(UINT, UINT, LPPALETTEENTRY);
   UINT        GetNearestPaletteIndex(COLORREF);
   static BYTE  GetColorForPalette(int ,BYTE ,BYTE );
   static DWORD GetImageSize(DWORD, int, int);
   static void  CalculatePaletteColors(RGBQUAD *pColors, int nColors, bool bOverWrite = false);
   static void  DefaultOverwrite(RGBQUAD *);
   static void  CalculatePaletteColors(PALETTEENTRY*pColors, int nColors, bool bOverWrite = false);
   static void  DefaultOverwrite(PALETTEENTRY *);

private:
   static OSVERSIONINFO gm_OsVersionInfo;
public:
	bool InitPalette();
   static bool  IsOnWin95()      {return (gm_OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ? true : false;};
   static bool  IsOnWinNT()      {return (gm_OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)      ? true : false;};
     
private:

   HDC         m_hDC;
   HBITMAP     m_hBmp, m_hSecondBmp,
               m_hOldBmp;
   HPALETTE    m_hPalette,
               m_hOldPalette;
   void       *m_pvBits;
   void       *m_pvSecondBits;
   BITMAPINFO *m_pBmi;
   BITMAPINFO *m_pSecondBmi;
   UINT        m_iUsage;
   int         m_nColorBits;
   HANDLE      m_hFile;
   DWORD       m_dwOffset;

   static int        defaultOverride[13];
   static RGBQUAD    defaultEntry[20];
};

/******************************************************************************
* Name       : Destroy                                                        *
* Definition : void Destroy();                                                *
* Zweck      : Freigeben des Speichers und der Handle für die DIBSection.     *
*              Das Object kann nur nach einem Aufruf von Destroy() wieder neu *
*              Initialisiert werden.                                          *
* Aufruf     : Destroy();                                                     *
* Parameter  :    keine                                                       *
* Funktionswert : keiner                                                      *
******************************************************************************/

/******************************************************************************
* Name       : Create                                                         *
* Definition : bool Create(HDC,int,int,CFile *pFile=NULL,DWORD dwOffset=0);   *
* Zweck      : Erzeugt eine Dib-Section mit den übergebenen Parametern.       *
               Mit den Funktionen SetColorMode() und SetColorUsage() kann die *
               Farbnutzung eingestellt werden.                                *
* Aufruf     : Create(HDC hdc, int cx, int cy, CFile *pFile, DWORD dwOffset)  *
* Parameter  :                                                                *
* hdc    (E) : Devicecontext  für die Dib-Section.                   (HDC)    *
* cx, cy (E) : Breite und Höhe der Dib-Section                       (int )   *
* hFile  (E) : Handle für ein File Mapping-Objekt.                   (HANDLE) *
* dwOffset(E): Offset vom Anfang des File Mapping-Objektes           (DWORD ) *
* Funktionswert : Funktion ausgeführt (true, false)                  (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : DuplicateBitmap                                                *
* Definition : bool DuplicateBitmap();                                        *
* Zweck      : Erzeugen einer zweiten unabhängigen DIB-Section, um z.B.: das  *
*              Bearbeiten des Inhalts der unabhängigen DIB-Section in einem   *
*              anderen Thread zu ermöglichen. Die Haupt-DIB-Section ist an den*
*              Device-Context gebunden. Die DIB-Sections können mit der       *
*              Funktion SwapBitmaps vertauscht werden.                        *
* Aufruf     : DuplicateBitmap();                                             *
* Parameter  :                                                                *
* Funktionswert : Funktionserfolg (true, false)                               *
******************************************************************************/

/******************************************************************************
* Name       : SwapBitmaps                                                    *
* Definition : bool SwapBitmaps();                                            *
* Zweck      : Vertauschen der Haupt- und der unabhängigen DIB-Section.       *
* Aufruf     : SwapBitmaps();                                                 *
* Parameter  :                                                                *
* Funktionswert : Funktionserfolg (true, false)                               *
******************************************************************************/

/******************************************************************************
* Name       : SetNewSize                                                     *
* Definition : bool SetNewSize(SIZE *);                                       *
* Zweck      : Ändern der Größe einer bestehenden DIB-Section.                *
*              !! Der Inhalt geht verloren !!                                 *
* Aufruf     : SetNewSize(szNew);                                             *
* Parameter  :                                                                *
* szNew  (E) : neue Größe der DIB-Section                             (SIZE*) *
* Funktionswert : Funktionserfolg (true, false)                               *
******************************************************************************/

/******************************************************************************
* Name       : GetColorForPalette                                             *
* Definition : static BYTE GetColorForPalette(int ,BYTE ,BYTE);               *
* Zweck      : Berechnen einer Rot-, Grün-, Blau-Farbintensität aus einem     *
*              Index für eine Farbpalette.                                    *
* Aufruf     : GetColorForPalette(i, cShift, byMask);                         *
* i      (E) : Index des Farbwertes.                                          *
* cShift (E) : Bitverschiebung.                                               *
* byMask (E) : Maskierung.                                                    *
* Parameter  :                                                                *
* Funktionswert : Farbwert (0,..,255)                                         *
******************************************************************************/

/******************************************************************************
* Name       : CalculatePaletteColors                                         *
* Definition : static void CalculatePaletteColors(RGBQUAD,int,bool);          *
* Zweck      : Berechnen eine Farbtabelle für eine DIB oder eine Palette.     *
* Aufruf     : CalculatePaletteColors(pColors, nColors, bOverWrite);          *
* Parameter  :                                                                *
* pColors (E): Feld für die berechnete Farbtabelle.               (RGBQUAD*)  *
* nColors (E): Größe des Feldes.                                  (int)       *
* bOverWrite(E): Überschreiben einiger Werte mit Default-Farben   (bool)      *
* Funktionswert : keiner                                                      *
******************************************************************************/

/******************************************************************************
* Name       : StretchDIBits                                                  *
* Definition : int StretchDIBits(HDC, RECT);                                  *
* Zweck      : Ausgabe der Dib-Section Bits auf einen Gerätekontext mit       *
*              Skalierung auf die angegebene Rechteckgröße:                   *
* Aufruf     : StretchDIBits(hdc, rcDest);                                    *
* Parameter  :                                                                *
* hdc    (E) : Handle auf  einen Gerätekontext                       (HDC)    *
* rcDest (E) : Ausgaberechteck                                       (RECT)   *
* [bSecond](E):Ausgabe der Bits aus der vom Gerätekontext            (bool)   *
*              unabhängigen DIB-Section.
* Funktionswert : Funktion ausgeführt (1, 0)                         (int)    *
******************************************************************************/

/******************************************************************************
* Name       : SetDIBitsToDevice                                              *
* Definition : int SetDIBitsToDevice(HDC , RECT);                             *
* Zweck      : Ausgabe Dib-Section Bits ohne Skalierung in ein Zielrechteck.  *
*              !! Darf nicht größer sein als die Dib-Section !!               *
* Aufruf     : SetDIBitsToDevice(hdc, rcDest);                                *
* Parameter  :                                                                *
* hdc    (E) : Handle auf  einen Gerätekontext                       (HDC)    *
* rcDest (E) : Ausgaberechteck                                       (RECT)   *
* [bSecond](E):Ausgabe der Bits aus der vom Gerätekontext            (bool)   *
*              unabhängigen DIB-Section.
* Funktionswert : Funktion ausgeführt (1, 0)                         (int)    *
******************************************************************************/

/******************************************************************************
* Name       : BitBlt                                                         *
* Definition : BitBlt(HDC, int, int, int, int, DWORD);                        *
* Zweck      : Ausgabe Dib-Section ohne Skalierung in ein Zielrechteck.       *
*              !! Darf nicht größer sein als die Dib-Section !!               *
* Aufruf     : BitBlt(hdc, left, top, nWidth, nHeight, dwRop);                *
* Parameter  :                                                                *
* hdc    (E) : Handle auf  einen Gerätekontext                       (HDC)    *
* left, top       (E) : linke obere Ecke des Ausgaberechtecks        (int)    *
* nWidth, nHeight (E) : Größe des Ausgaberechtecks                   (int)    *
* dwRop  (E) : Raster Operation Mode siehe ::BitBlt(..)              (DWORD)  *
* Funktionswert : Funktion ausgeführt (1, 0)                         (int)    *
******************************************************************************/

/******************************************************************************
* Name       : StretchBlt                                                     *
* Definition : StretchBlt(HDC, int, int, int, int, DWORD);                    *
* Zweck      : Ausgabe Dib-Section mit Skalierung in ein Zielrechteck.        *
* Aufruf     : StretchBlt(hdc, left, top, nWidth, nHeight, dwRop);            *
* Parameter  :                                                                *
* hdc    (E) : Handle auf  einen Gerätekontext                       (HDC)    *
* left, top       (E) : linke obere Ecke des Ausgaberechtecks        (int)    *
* nWidth, nHeight (E) : Größe des Ausgaberechtecks                   (int)    *
* dwRop  (E) : Raster Operation Mode siehe ::BitBlt(..)              (DWORD)  *
* Funktionswert : Funktion ausgeführt (1, 0)                         (int)    *
******************************************************************************/

/******************************************************************************
* Name       : GetDIBData                                                     *
* Definition : HGLOBAL GetDIBData();                                          *
* Zweck      : Kopiert die Daten der Dib-Section in ein globales              *
*              Speicherobjekt.                                                *
* Aufruf     : GetDIBData();                                                  *
* Parameter  : keine                                                          *
* [prcBmp] (E): [optional] Größe eines Rechtecks innerhalb der     (RECT*)    *
*              DibSection, sonst wird die gesamte DibSection kopiert.         *
* Funktionswert : Handle auf ein globales Speicherobjekt                      *
*                 bei Fehler NULL.                                            *
******************************************************************************/

/******************************************************************************
* Name       : LoadFromFile                                                   *
* Definition : bool LoadFromFile(CFile *);                                    *
* Zweck      : Erzeugt eine Dib-Section aus eine bmp-Datei.                   *
* Aufruf     : LoadFromFile(pFile);                                           *
* Parameter  :                                                                *
* hFile  (E) : Zeiger auf ein geöffnetes CFile-Objekt                (HANDLE) *
* Funktionswert : Funktion ausgeführt (true, false)                  (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : SaveInFile                                                     *
* Definition : bool SaveInFile(CFile *);                                      *
* Zweck      : Speichert eine DibSection in einer bmp-Datei.                  *
* Aufruf     : SaveInFile(pFile);                                             *
* Parameter  :                                                                *
* hFile  (E) : Zeiger auf ein geöffnetes CFile-Objekt                (HANDLE) *
* Funktionswert : Funktion ausgeführt (true, false)                  (bool)   *
******************************************************************************/

#endif // !defined(AFX_DIBSECTION_H__3760DB42_B8DE_11D1_9DB9_0000B458D891__INCLUDED_)
