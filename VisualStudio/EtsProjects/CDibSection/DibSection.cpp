/********************************************************************/
/*                                                                  */
/*                     Library CDibSection                          */
/*                                                                  */
/*     Klasse zur Darstellung und Bearbeitung von DibSections       */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// DibSection.cpp: Implementierung der Klasse CDibSection.
//
//////////////////////////////////////////////////////////////////////
#include "DibSection.h"

#if defined _DEBUG
   #include <crtdbg.h>
   #define ASSERT(expr) \
        do { if (!(expr) && \
             (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
             _CrtDbgBreak(); } while (0)

   #define VERIFY(f) ASSERT(f)
#else
   #define ASSERT(f)    ((void)(0))
   #define VERIFY(f)    ((void)(f))
#endif

#define TRACE        ((void)(0))
#define REPORT       ((void)(0))

#define abs(a) (((a) < 0) ? -a : a)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
OSVERSIONINFO CDibSection::gm_OsVersionInfo = {0 , 0, 0, 0, 0, ""};
int CDibSection::defaultOverride[13] = {0,3,24,27,64,67,88,173,181,236,247,164,91}; // OpenGL Farben der 3-3-2 Palette die durch Systemfarben ersetzt werden
RGBQUAD CDibSection::defaultEntry[20] =
{
   {  0,  0,  0,  0},
   {128,  0,  0,  0},
   {  0,128,  0,  0},
   {128,128,  0,  0},
   {  0,  0,128,  0},
   {128,  0,128,  0},
   {  0,128,128,  0},
   {192,192,192,  0},
   {192,220,192,  0},
   {166,202,240,  0},
   {255,251,240,  0},
   {160,160,164,  0},
   {128,128,128,  0},
   {255,  0,  0,  0},
   {  0,255,  0,  0},
   {255,255,  0,  0},
   {  0,  0,255,  0},
   {255,  0,255,  0},
   {  0,255,255,  0},
   {255,255,255,  0}
};

CDibSection::CDibSection()
{
   m_hDC         = NULL;
   m_hBmp        = NULL;
   m_pvBits      = NULL;
   m_hSecondBmp  = NULL;
   m_pvSecondBits= NULL;
   m_hOldBmp     = NULL;
   m_hPalette    = NULL;
   m_hOldPalette = NULL;
   m_pBmi        = NULL;
   m_pSecondBmi  = NULL;
   m_iUsage      = DIB_RGB_COLORS;
   m_nColorBits  = 0;
   m_hFile       = NULL;
   m_dwOffset    = 0;
   if (gm_OsVersionInfo.dwOSVersionInfoSize==0)
   {
      gm_OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      VERIFY(::GetVersionEx(&gm_OsVersionInfo)!=0);
   }
}

CDibSection::~CDibSection()
{
   Destroy();
}

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
void CDibSection::Destroy()
{
   if (m_hDC)
   {
      if (m_hOldBmp)
      {
         ::SelectObject(m_hDC, m_hOldBmp);
         m_hOldBmp = NULL;
      }
      if (m_hOldPalette)
      {
         ::SelectPalette(m_hDC, m_hOldPalette, FALSE);
         m_hOldPalette = NULL;
      }
      HWND hwnd = ::WindowFromDC(m_hDC);
      if (hwnd) ReleaseDC(hwnd, m_hDC);
      else
      {
         if (DeleteDC(m_hDC)) m_hDC = NULL;
         else ASSERT(false);
      }
   }
   if (m_hPalette)
   {
      if (::DeleteObject(m_hPalette)) m_hPalette = NULL;
      else ASSERT(false);
   }
   if (m_hBmp)
   {
      if (::DeleteObject(m_hBmp))
      {
         m_hBmp   = NULL;
         m_pvBits = NULL;
      }
      else ASSERT(false);
   }
   if (m_pBmi) 
   {
      delete[] ((char*)m_pBmi);
      m_pBmi = NULL;
   }
   DestroySecondBmp();
   if (m_hFile)
   {
      if (::CloseHandle(m_hFile)) m_hFile = NULL;
      else ASSERT(false);
   }
}

void CDibSection::DestroySecondBmp()
{
   if (m_pSecondBmi) 
   {
      delete[] ((char*)m_pSecondBmi);
      m_pSecondBmi = NULL;
   }
   if (m_hSecondBmp)
   {
      if (::DeleteObject(m_hSecondBmp))
      {
         m_hSecondBmp   = NULL;
         m_pvSecondBits = NULL;
      }
      else ASSERT(false);
   }
}
/******************************************************************************
* Name       : Create                                                         *
* Definition : bool Create(HDC,int,int,HANDLE *hFile=NULL,DWORD dwOffset=0);  *
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
bool CDibSection::Create(HDC hdc, int cx, int cy, HANDLE hFile, DWORD dwOffset)
{
   ASSERT((m_hDC==NULL) && (m_hFile==NULL) && (m_hBmp==NULL) && (m_hSecondBmp==NULL) &&(m_pBmi==NULL));
   int nColorBits, nColors;
   m_hDC      = hdc;
   m_hFile    = hFile;
   m_dwOffset = dwOffset;

   // wenn eine Palette benötigt wird oder wenn eine Palette gewünscht wird,
   // ist in m_nColorBits die Anzahl der Farbbits gespeichert !!!
   if (m_iUsage==DIB_PAL_COLORS)
   {
      nColorBits = m_nColorBits;
      nColors    = 1 << nColorBits;

      m_pBmi = (LPBITMAPINFO) new char[sizeof(BITMAPINFOHEADER) + (nColors * sizeof(RGBQUAD))];
      int i;
      LOGPALETTE *pPalette;
                                                               // Farbpalette erstellen
      pPalette = (LPLOGPALETTE) new char[sizeof (LOGPALETTE) + (nColors * sizeof (PALETTEENTRY))];
      pPalette->palVersion    = 0x300;
      pPalette->palNumEntries = (WORD) nColors;                // Standard Farbtabelle für die Palette berechnen
      CalculatePaletteColors((RGBQUAD*)&pPalette->palPalEntry[0], nColors, false);

      m_hPalette = ::CreatePalette(pPalette);
      delete[] ((char*)pPalette);

      WORD *pColors = (WORD*) &m_pBmi->bmiColors[0];
      for (i=0; i<nColors; i++)                                // Indextabelle belegen
         pColors[i] = (WORD) i;

      m_pBmi->bmiHeader.biSizeImage     = GetImageSize(cx, cy, nColorBits);
      m_pBmi->bmiHeader.biBitCount      = nColorBits;
      m_pBmi->bmiHeader.biClrUsed       = 0;
      m_pBmi->bmiHeader.biClrImportant  = 0;
   }
   else
   {
      m_pBmi   = (LPBITMAPINFO) new char[sizeof(BITMAPINFOHEADER)];
      m_pBmi->bmiHeader.biSizeImage     = 0;
      if (m_nColorBits != 0)  
         m_pBmi->bmiHeader.biBitCount   = m_nColorBits;
      else
         m_pBmi->bmiHeader.biBitCount   = 24;
      m_pBmi->bmiHeader.biClrUsed       = 0;
      m_pBmi->bmiHeader.biClrImportant  = 0;
   }

   m_pBmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
   m_pBmi->bmiHeader.biCompression   = BI_RGB;
   m_pBmi->bmiHeader.biPlanes        = 1;
   m_pBmi->bmiHeader.biWidth         = cx;
   m_pBmi->bmiHeader.biHeight        = cy;
   m_pBmi->bmiHeader.biXPelsPerMeter = MulDiv(GetDeviceCaps(m_hDC, LOGPIXELSX), 10000, 254);
   m_pBmi->bmiHeader.biYPelsPerMeter = MulDiv(GetDeviceCaps(m_hDC, LOGPIXELSY), 10000, 254);

   m_hBmp = ::CreateDIBSection(m_hDC, m_pBmi, m_iUsage, &m_pvBits, m_hFile, m_dwOffset);

   ASSERT(m_hBmp!=NULL);

   if (m_hBmp)
   {
      m_hOldBmp = (HBITMAP) ::SelectObject(m_hDC, m_hBmp);
      if (m_hPalette != NULL)                               // Palette in den Gerätekontext selektieren
      {
         m_hOldPalette = ::SelectPalette(m_hDC, m_hPalette, FALSE);
         int nCol = ::RealizePalette(m_hDC);
      }
      return true;
   }
   m_hDC      = NULL;
   m_hFile    = NULL;
   return false;
}

bool CDibSection::CreateIndirect(BITMAPINFO *pBmi)
{
   ASSERT((m_hDC==NULL) && (m_hFile==NULL) && (m_hBmp==NULL) && (m_hSecondBmp==NULL) &&(m_pBmi==NULL));

   int nColors = 0;
   if (pBmi->bmiHeader.biBitCount<9) nColors = 1 << pBmi->bmiHeader.biBitCount;
   if (pBmi->bmiHeader.biClrUsed!=0) nColors = pBmi->bmiHeader.biClrUsed;

   HDC hdc;
   int size = sizeof(BITMAPINFOHEADER) + (nColors * sizeof(RGBQUAD));

   m_pBmi = (BITMAPINFO*) new char[size];
   if (!m_pBmi)
   {
      return false;
   }
   memcpy(m_pBmi, pBmi, size);

   hdc    = ::GetDC(NULL);
   m_hDC  = ::CreateCompatibleDC(hdc);
   m_hBmp = ::CreateDIBSection(m_hDC, m_pBmi, m_iUsage, &m_pvBits, NULL, 0);
   ::ReleaseDC(NULL, hdc);

   if (m_hBmp && m_pvBits)
   {
      ::SelectObject(m_hDC, m_hBmp);
      if (m_pBmi->bmiHeader.biSizeImage==0)
      {
         m_pBmi->bmiHeader.biSizeImage = GetImageSize(m_pBmi->bmiHeader.biWidth, m_pBmi->bmiHeader.biHeight, m_pBmi->bmiHeader.biBitCount);
      }
      memcpy(m_pvBits, &((char*)pBmi)[size], m_pBmi->bmiHeader.biSizeImage);
   }
   else
   {
      return false;
   }
   return true;
}

DWORD CDibSection::GetImageSize(DWORD cx, int cy, int nColorBits)
{
   return ALIGN_LONG(cx * nColorBits) * cy;
}

bool CDibSection::LoadFromResource(HINSTANCE hInstance, int nID)
{
   HDC         hdc;
   int         size, 
               nColors = 0;
   bool        rValue  = false;

   hdc    = ::GetDC(NULL);

   HBITMAP hBmp = (HBITMAP) ::LoadImage(hInstance, MAKEINTRESOURCE(nID), IMAGE_BITMAP, 0,0, LR_DEFAULTCOLOR);

   if (hBmp)
   {
      BITMAP bmp;
      size = sizeof(BITMAPINFOHEADER);
      GetObject(hBmp, sizeof(BITMAP), &bmp);
      m_pBmi = (BITMAPINFO*) new char[size];
      if (m_pBmi)
      {
         m_hDC                             = ::CreateCompatibleDC(hdc);
         m_pBmi->bmiHeader.biSize          = size;
         m_pBmi->bmiHeader.biWidth         = bmp.bmWidth;      // Größe der Bitmap eintragen
         m_pBmi->bmiHeader.biHeight        = bmp.bmHeight;
         m_pBmi->bmiHeader.biPlanes        = 1;                // eine Bitplane
         m_pBmi->bmiHeader.biBitCount      = 24;               // 24-Bit Colors
         m_pBmi->bmiHeader.biCompression   = BI_RGB;           // als RGB Farbewerte ablegen (->keine Palette=
         m_pBmi->bmiHeader.biSizeImage     = GetImageSize(bmp.bmWidth, bmp.bmHeight, m_pBmi->bmiHeader.biBitCount);
         m_pBmi->bmiHeader.biXPelsPerMeter = MulDiv(GetDeviceCaps(m_hDC, LOGPIXELSX), 10000, 254);
         m_pBmi->bmiHeader.biYPelsPerMeter = MulDiv(GetDeviceCaps(m_hDC, LOGPIXELSY), 10000, 254);
         m_pBmi->bmiHeader.biClrUsed       = 0;                // alle Farben Wichtig
         m_pBmi->bmiHeader.biClrImportant  = 0;
         SetColorUsage(hdc);
         m_hBmp = ::CreateDIBSection(m_hDC, m_pBmi, m_iUsage, &m_pvBits, NULL, 0);
         if (m_hBmp)
         {
            if(GetDIBits(hdc, hBmp, 0, bmp.bmHeight, m_pvBits, m_pBmi, DIB_RGB_COLORS))
            {
               rValue = true;
            }
         }
      }
   }
   ReleaseDC(NULL,hdc);                                        // ScreenDC freigeben
   DeleteObject(hBmp);                                         // Bitmap löschen
   if (!rValue) Destroy();                                     // Fehler : Resourcen freigeben
   return rValue;
}

HBITMAP  CDibSection::Detach()
{
   if (m_hDC)
   {
      if (m_hOldBmp)
      {
         ::SelectObject(m_hDC, m_hOldBmp);
         m_hOldBmp = NULL;
      }
   }
   if (m_hBmp)
   {
      HBITMAP hBmp = m_hBmp; 
      m_hBmp   = NULL;
      m_pvBits = NULL;
      return hBmp;
   }
   return NULL;
}

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
bool CDibSection::DuplicateBitmap()
{
   if ((m_hSecondBmp==NULL) && m_hDC && m_pBmi && m_hBmp)
   {
      m_hSecondBmp = ::CreateDIBSection(m_hDC, m_pBmi, m_iUsage, &m_pvSecondBits, m_hFile, m_dwOffset);
      ASSERT(m_hSecondBmp!=NULL);
      if (m_hSecondBmp)
      {
         return true;
      }
   }
   return false;
}

/******************************************************************************
* Name       : SwapBitmaps                                                    *
* Definition : bool SwapBitmaps();                                            *
* Zweck      : Vertauschen der Haupt- und der unabhängigen DIB-Section.       *
* Aufruf     : SwapBitmaps();                                                 *
* Parameter  :                                                                *
* Funktionswert : Funktionserfolg (true, false)                               *
******************************************************************************/
bool CDibSection::SwapBitmaps()
{
   if ((m_hSecondBmp != NULL) && (m_pSecondBmi == NULL)) // Nur tauschen, wenn die BIMAPINFOHEADER gleich sind
   {
      void * pTemp = m_pvBits;
                     m_pvBits = m_pvSecondBits;
                                m_pvSecondBits = pTemp;

      m_hBmp       = m_hSecondBmp;
                     m_hSecondBmp = (HBITMAP) ::SelectObject(m_hDC, m_hSecondBmp);
      return true;
   }
   return false;
}

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
bool CDibSection::SetNewSize(SIZE *szNew)
{
   if (m_hDC && m_hBmp && m_pBmi && m_hOldBmp)
   {
      DWORD dwOffset = 0;
      bool bSecond   = ((m_hSecondBmp != NULL) && (m_pSecondBmi == NULL)) ? true : false;
      ::SelectObject(m_hDC, m_hOldBmp);
      ::DeleteObject(m_hBmp);

      DestroySecondBmp();

      m_pBmi->bmiHeader.biWidth  = szNew->cx;
      m_pBmi->bmiHeader.biHeight = szNew->cy;

      m_hBmp = ::CreateDIBSection(m_hDC, m_pBmi, m_iUsage, &m_pvBits, m_hFile, m_dwOffset);

      if (m_hBmp)
      {
         if (bSecond)
         {
            DuplicateBitmap();
         }
         m_hOldBmp = (HBITMAP) ::SelectObject(m_hDC, m_hBmp);
         return true;
      }
   }
   ASSERT(false);
   return false;
}

/******************************************************************************
* Name       : GetColorForPalette                                             *
* Definition : BYTE GetColorForPalette(int ,BYTE ,BYTE);                      *
* Zweck      : Berechnen einer Rot-, Grün-, Blau-Farbintensität aus einem     *
*              Index für eine Farbpalette.                                    *
* Aufruf     : GetColorForPalette(i, cShift, byMask);                         *
* i      (E) : Index des Farbwertes.                                          *
* cShift (E) : Bitverschiebung.                                               *
* byMask (E) : Maskierung.                                                    *
* Parameter  :                                                                *
* Funktionswert : Farbwert (0,..,255)                                         *
******************************************************************************/
BYTE CDibSection::GetColorForPalette(int i,BYTE cShift,BYTE byMask)
{
    return (BYTE) ((((i >> cShift) & byMask) * 255) / ((double) byMask)+0.5);
}
/******************************************************************************
* Name       : CalculatePaletteColors                                         *
* Definition : void CalculatePaletteColors(RGBQUAD,int,bool);                 *
* Zweck      : Berechnen eine Farbtabelle für eine DIB oder eine Palette.     *
* Aufruf     : CalculatePaletteColors(pColors, nColors, bOverWrite);          *
* Parameter  :                                                                *
* pColors (E): Feld für die berechnete Farbtabelle.               (RGBQUAD*)  *
* nColors (E): Größe des Feldes.                                  (int)       *
* bOverWrite(E): Überschreiben einiger Werte mit Default-Farben   (bool)      *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CDibSection::CalculatePaletteColors(RGBQUAD *pColors, int nColors, bool bOverWrite)
{
   for (int i=0; i<nColors; i++)
   {
      pColors[i].rgbRed      = GetColorForPalette(i,0,7);
      pColors[i].rgbGreen    = GetColorForPalette(i,3,7);
      pColors[i].rgbBlue     = GetColorForPalette(i,6,3);
      pColors[i].rgbReserved = 0;
   }
   if (bOverWrite) DefaultOverwrite(pColors);
}

void CDibSection::DefaultOverwrite(RGBQUAD *pColors)
{
   for (int i=1;i <= 12 ; i++) pColors[defaultOverride[i]] = defaultEntry[i];
}

void CDibSection::CalculatePaletteColors(PALETTEENTRY *pColors, int nColors, bool bOverWrite)
{
   for (int i=0; i<nColors; i++)
   {
      pColors[i].peRed   = GetColorForPalette(i,0,7);
      pColors[i].peGreen = GetColorForPalette(i,3,7);
      pColors[i].peBlue  = GetColorForPalette(i,6,3);
      pColors[i].peFlags = 0;
   }
   if (bOverWrite) DefaultOverwrite(pColors);
}
void CDibSection::DefaultOverwrite(PALETTEENTRY *pColors)
{
   for (int i=1;i <= 12 ; i++)
   {
      pColors[defaultOverride[i]].peBlue  = defaultEntry[i].rgbBlue;
      pColors[defaultOverride[i]].peGreen = defaultEntry[i].rgbGreen;
      pColors[defaultOverride[i]].peRed   = defaultEntry[i].rgbRed;
      pColors[defaultOverride[i]].peFlags = defaultEntry[i].rgbReserved;
   }
}
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
int CDibSection::StretchDIBits(HDC hdc, RECT rcDest, bool bSecond, DWORD dwRop)
{
   int nResult = 0;
   void      *pvBits = NULL;
   BITMAPINFO*pBmi   = NULL;
   if (bSecond)
   {
      pvBits = m_pvSecondBits;
      if (m_pSecondBmi) pBmi = m_pSecondBmi;
      else              pBmi = m_pBmi;
   }
   else
   {
      pvBits = m_pvBits;
      pBmi   = m_pBmi;
   }
   if (pvBits && pBmi)
   {
      nResult = ::StretchDIBits(hdc,
      rcDest.left             , rcDest.top,
      rcDest.right-rcDest.left, rcDest.bottom-rcDest.top,       // Ziel
      0                       , 0            ,
      pBmi->bmiHeader.biWidth , abs(pBmi->bmiHeader.biHeight),// Quelle
      pvBits, pBmi, m_iUsage, dwRop);
   }
   return nResult;
}
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
int CDibSection::SetDIBitsToDevice(HDC hdc, RECT rcDest, bool bSecond)
{
   int rValue = 0;
   if (((m_pBmi       != NULL) && (rcDest.bottom <= abs(m_pBmi->bmiHeader.biHeight      ))) ||
       ((m_pSecondBmi != NULL) && (rcDest.bottom <= abs(m_pSecondBmi->bmiHeader.biHeight))))
   {
      void *pvBits;
      BITMAPINFO*pBmi;
      if (bSecond)
      {
         pvBits = m_pvSecondBits;
         if (m_pSecondBmi) pBmi = m_pSecondBmi;
         else              pBmi = m_pBmi;
      }
      else
      {
         pvBits = m_pvBits;
         pBmi   = m_pBmi;
      }
      if (pvBits && pBmi)
      {
         SIZE szDest;
         szDest.cx = rcDest.right - rcDest.left;
         szDest.cy = rcDest.bottom - rcDest.top;
         if (pBmi->bmiHeader.biHeight < 0)                  // Top-Down-DIB
         {
            rValue = ::SetDIBitsToDevice(hdc, 
            rcDest.left, rcDest.top,                        // Ziel   : links, oben
            szDest.cx  , szDest.cy,                         // Quelle : Größe
            rcDest.left, rcDest.top,                        // Quelle : links, oben
            0          , szDest.cy,                         // 1. Scanline, Anzahl Scanlines
            pvBits, pBmi, m_iUsage);
         }
         else                                               // Bottom-Up-DIB
         {
            if (IsOnWinNT())
            {
               rValue = ::SetDIBitsToDevice(hdc, 
               rcDest.left, rcDest.top,                     // Ziel   : links, oben
               szDest.cx  , szDest.cy,                      // Quelle : Größe
               rcDest.left, pBmi->bmiHeader.biHeight-szDest.cy,// Quelle : links,unten
               pBmi->bmiHeader.biHeight-szDest.cy, szDest.cy,// 1. Scanline, Anzahl Scanlines
               pvBits, pBmi, m_iUsage);
            }
            else
            {
               rValue = ::SetDIBitsToDevice(hdc, 
               rcDest.left  , rcDest.top,                   // Ziel   : links, oben
               szDest.cx    , szDest.cy,                    // Quelle : Größe
               rcDest.left  , rcDest.bottom,                // Quelle : links,unten
               rcDest.bottom, szDest.cy,                    // 1. Scanline, Anzahl Scanlines
               pvBits, pBmi, m_iUsage);
            }
         }
      }
   }
   return rValue;
}

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
bool CDibSection::BitBlt(HDC hdc, int left, int top, int nWidth, int nHeight, DWORD dwRop)
{
   if (m_hDC!=NULL)
   {               // (Destination                    , Source
      return (::BitBlt(hdc, left, top, nWidth, nHeight, m_hDC, left, top, dwRop) !=0) ? true : false;
   }
   return false;
}
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
*              !! Darf nicht größer sein als die Dib-Section !!               *
* Funktionswert : Funktion ausgeführt (1, 0)                         (int)    *
******************************************************************************/
bool CDibSection::StretchBlt(HDC hdc, int left, int top, int nWidth, int nHeight, DWORD dwRop)
{
   if ((NULL != m_hDC) && (NULL != m_pBmi))
   {
      return(::StretchBlt(hdc, left, top, nWidth, nHeight,                        // Ziel
          m_hDC, 0, 0, m_pBmi->bmiHeader.biWidth, abs(m_pBmi->bmiHeader.biHeight),// Quelle
          dwRop) != 0 ? true : false);
   }
   return false;
}
/******************************************************************************
* Name       : GetDIBData                                                     *
* Definition : HGLOBAL GetDIBData();                                          *
* Zweck      : Kopiert die Daten der Dib-Section in ein globales              *
*              Speicherobjekt.                                                *
* Aufruf     : GetDIBData();                                                  *
* Parameter  :                                                                *
* [pnSize] (A): Größe des allozierten Speicherbereiches in Bytes       (int*) *
* [prcBmp] (E): Größe des Rechteckes das innerhalb des Bitmaprechtecks (RECT*)*
*               kopiert werden soll.
* Funktionswert : Handle auf ein globales Speicherobjekt                      *
*                 bei Fehler NULL.                                            *
******************************************************************************/
HGLOBAL CDibSection::GetDIBData(UINT*pnSize, RECT*prcBmp)
{
   ASSERT((m_hDC!=NULL) && (m_hBmp!=NULL) && (m_pBmi!=NULL));
   int           nColors, nScanlines;
   HGLOBAL       hMem = NULL;
   void         *pMem;
   char         *pTemp;
   int           oldWidth     = m_pBmi->bmiHeader.biWidth,     // sichern der alten Größen
                 oldHeight    = m_pBmi->bmiHeader.biHeight,
                 oldSizeImage = m_pBmi->bmiHeader.biSizeImage;
   // Größe der Bitmap ermitteln
   UINT nSize = 0;
   
   nScanlines = abs(m_pBmi->bmiHeader.biHeight);
   if (nScanlines != 0)
   {
      m_pBmi->bmiHeader.biSizeImage = GetImageSize(m_pBmi->bmiHeader.biWidth, m_pBmi->bmiHeader.biHeight, m_pBmi->bmiHeader.biBitCount);

      if (prcBmp)                                              // Berechnen des Speicherbedarfs bei kopieren einer Teilgröße
      {
         if ((prcBmp->right  > m_pBmi->bmiHeader.biWidth)  || (prcBmp->bottom > m_pBmi->bmiHeader.biHeight) ||
             (prcBmp->left   < 0)                          || (prcBmp->top    < 0))
         {
            return hMem;
         }
         m_pBmi->bmiHeader.biHeight    = prcBmp->bottom-prcBmp->top;
         m_pBmi->bmiHeader.biWidth     = prcBmp->right-prcBmp->left;
         m_pBmi->bmiHeader.biSizeImage = GetImageSize(m_pBmi->bmiHeader.biWidth, m_pBmi->bmiHeader.biHeight, m_pBmi->bmiHeader.biBitCount);
      }
      if (m_hPalette) nColors = GetPaletteEntries(0, 0, NULL);
      else            nColors = m_pBmi->bmiHeader.biClrUsed;

      // globalen Speicher allocieren
      nSize = sizeof(BITMAPINFOHEADER) + (nColors * sizeof(RGBQUAD)) + m_pBmi->bmiHeader.biSizeImage;
      hMem    = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, nSize);
      if (hMem)
      {
         pMem  = ::GlobalLock(hMem);                           // globale Speicheradresse festlegen
         if (pMem)
         {                                                     // BITMAPINFOHEADER kopieren
            pTemp = (char*) memcpy(pMem, m_pBmi, sizeof(BITMAPINFOHEADER));
            BITMAPINFOHEADER *pBmi = (BITMAPINFOHEADER *)pTemp;
            pBmi->biClrUsed = nColors;

            pTemp += sizeof(BITMAPINFOHEADER);
            if (m_hPalette)                                    // Kopieren der Pallette
            {
               PALETTEENTRY *pPe = NULL;
               pPe = new PALETTEENTRY[nColors];
               GetPaletteEntries(0, nColors, pPe);
               for (int i=0; i<nColors; i++)                   // kopieren und vertauschen der Farbreihenfolge RGB -> BGR
               {
                  ((RGBQUAD*)pTemp)[i].rgbBlue     = pPe[i].peBlue;
                  ((RGBQUAD*)pTemp)[i].rgbGreen    = pPe[i].peGreen;
                  ((RGBQUAD*)pTemp)[i].rgbRed      = pPe[i].peRed;
                  ((RGBQUAD*)pTemp)[i].rgbReserved = 0;
               }
               delete[] pPe;
            }
            else if ((m_iUsage == DIB_RGB_COLORS) && (nColors>0))               // Kopieren der Farben im Anhang des BITMAPINFOHEADER
            {
               memcpy(pTemp, &m_pBmi->bmiColors[0], (nColors * sizeof(RGBQUAD)));
            }

            if (nColors>0) pTemp += (nColors * sizeof(RGBQUAD));

            if (prcBmp)                                        // Kopieren der Daten bei Teilgröße
            {
               int nBmpLineSize  = ALIGN_LONG(oldWidth                  * m_pBmi->bmiHeader.biBitCount); // Bmp Zeilengröße in Bytes
               int nCopyLineSize = ALIGN_LONG(m_pBmi->bmiHeader.biWidth * m_pBmi->bmiHeader.biBitCount); // Copier Zeilengröße in Bytes
               int nBytes        = m_pBmi->bmiHeader.biBitCount / 8;
               char *ptemp = (char*) m_pvBits;
               for (int i=prcBmp->top; i<prcBmp->bottom; i++)  // für jede Zeile
               {
                  if ((i>=0) && (i<abs(oldHeight)))
                  {
                     memcpy(pTemp, &ptemp[i*nBmpLineSize+prcBmp->left*nBytes], nCopyLineSize);
                     pTemp += nCopyLineSize;
                  }
               }
            }
            else                                               // Kopieren der gesamten Größe
            {
               memcpy(pTemp, m_pvBits, m_pBmi->bmiHeader.biSizeImage);
            }
            ::GlobalUnlock(hMem);                              // Speicheradresse freigeben
         }
         else                                                  // Fehler !! Speicher freigeben
         {
            ::GlobalFree(hMem);
            hMem = NULL;
         }
      }
      m_pBmi->bmiHeader.biWidth     = oldWidth;                // Rücksichern der alten Größen
      m_pBmi->bmiHeader.biHeight    = oldHeight;
      m_pBmi->bmiHeader.biSizeImage = oldSizeImage;
   }
   if (pnSize) *pnSize = nSize;
   return hMem;
}
/******************************************************************************
* Name       : LoadFromFile                                                   *
* Definition : bool LoadFromFile(CFile *);                                    *
* Zweck      : Erzeugt eine Dib-Section aus eine bmp-Datei.                   *
* Aufruf     : LoadFromFile(pFile);                                           *
* Parameter  :                                                                *
* hFile  (E) : Zeiger auf ein geöffnetes CFile-Objekt                (HANDLE) *
* Funktionswert : Funktion ausgeführt (true, false)                  (bool)   *
******************************************************************************/
bool CDibSection::LoadFromFile(HANDLE hFile)
{
   ASSERT((m_hDC==NULL) && (m_hFile==NULL) && (m_hBmp==NULL) && (m_pBmi==NULL));
   BITMAPFILEHEADER bmfh;
   BITMAPINFOHEADER bmih;
   BITMAPCOREHEADER bmch;
   bool             isPM = false;
   int              nColors = 0;
   HDC              hdc;
   DWORD            dwNoOfBytes = 0;
   LONG             dwLoFileBegin, dwHiFileBegin=0;

   try
   {
      if (!hFile) throw 1;
      dwLoFileBegin = ::SetFilePointer(hFile, 0, &dwHiFileBegin, FILE_CURRENT);
      ::ReadFile(hFile, &bmfh,sizeof(BITMAPFILEHEADER), &dwNoOfBytes, NULL);
      if (bmfh.bfType!=0x4d42) throw 2;                        // Check that we have the magic ´BM´ at the start
      // Make a wild guess that the file is in Windows DIB format
      // and read the BITMAPINFOHEADER. If the file turns out to be a
      // PM DIB file, we´ll convert it later.
      ::ReadFile(hFile, &bmih,sizeof(BITMAPINFOHEADER), &dwNoOfBytes, NULL);// Check that we got a real Windows DIB file.
      if (bmih.biSize != sizeof(BITMAPINFOHEADER))             // oh, it´s a PM file
      {
         isPM = true;
         memcpy(&bmch, &bmih, sizeof(BITMAPCOREHEADER));       // copy read bytes from BITMAPINFOHEADER-structure to BITMAPCOREHEADER-structure
         if (bmch.bcSize!=sizeof(BITMAPCOREHEADER)) throw 3;   // Check that we got a real PM (Presentation Manager) file

         memset(&bmih,0,sizeof(BITMAPINFOHEADER));             // create BITMAPINFOHEADER from BITMAPCOREHEADER
         bmih.biSize        = sizeof(BITMAPINFOHEADER);
         bmih.biWidth       = (int) bmch.bcWidth;
         bmih.biHeight      = (int) bmch.bcHeight;
         bmih.biPlanes      = bmch.bcPlanes;
         bmih.biBitCount    = bmch.bcBitCount;
         bmih.biCompression = BI_RGB;
      }

      if (isPM)   // colors in Colortable ?
      {
         if (bmch.bcBitCount<9) nColors = 1 << bmch.bcBitCount;
      }
      else
      {
         if (bmih.biBitCount<9) nColors = 1 << bmih.biBitCount;
         if (bmih.biClrUsed!=0) nColors = bmih.biClrUsed;
      }

      if (nColors)                                                // read in the Colortable
      {
         m_pBmi = (BITMAPINFO*) new char[sizeof(BITMAPINFOHEADER) + (nColors * sizeof(RGBQUAD))];
         if (!m_pBmi) throw 4;
         ::ReadFile(hFile, &m_pBmi->bmiColors, nColors * sizeof(RGBQUAD), &dwNoOfBytes, NULL);
      }
      else
         m_pBmi = (BITMAPINFO*) new char[sizeof(BITMAPINFOHEADER)];

      if (!m_pBmi) throw 5;
      memcpy(&m_pBmi->bmiHeader, &bmih, sizeof(BITMAPINFOHEADER)); // Copy bmi-Data to Class-Member

      hdc    = ::GetDC(NULL);
      m_hDC  = ::CreateCompatibleDC(hdc);
      m_pBmi->bmiHeader.biXPelsPerMeter = MulDiv(GetDeviceCaps(m_hDC, LOGPIXELSX), 10000, 254);
      m_pBmi->bmiHeader.biYPelsPerMeter = MulDiv(GetDeviceCaps(m_hDC, LOGPIXELSY), 10000, 254);
      m_hBmp = ::CreateDIBSection(m_hDC, m_pBmi, m_iUsage, &m_pvBits, NULL, 0);
      ::ReleaseDC(NULL, hdc);
      if (m_pBmi->bmiHeader.biSizeImage == 0)
      {
         m_pBmi->bmiHeader.biSizeImage = GetImageSize();
      }
      if (m_hBmp && m_pvBits)
      {
         ::SelectObject(m_hDC, m_hBmp);
         ::SetFilePointer(hFile, dwLoFileBegin+bmfh.bfOffBits, &dwHiFileBegin, FILE_BEGIN);
         unsigned int nSize = bmfh.bfSize - bmfh.bfOffBits;
         if (nSize > m_pBmi->bmiHeader.biSizeImage)
            nSize = m_pBmi->bmiHeader.biSizeImage;
         ::ReadFile(hFile, m_pvBits, nSize, &dwNoOfBytes, NULL);
         ASSERT(dwNoOfBytes == (nSize));
      }
      else throw 6;
   }
   catch (int error)
   {
      error;
      #ifdef DFI_CLASSNAME                                     // Reports nur wenn sie eingebunden werden sollen !
      switch(error)
      {
         case  1: REPORT("No Filehandle");    break;
         case  2: REPORT("No BM-File");       break;
         case  3: REPORT("Wrong Headersize"); break;
         case  4:
         case  5: REPORT("Allocation Error"); break;
         case  6: REPORT("No Bitmaphandle");  break;
      }
      #endif
      return false;
   }
   return true;
}

bool CDibSection::SaveInFile(HANDLE hFile)
{
   if (m_hDC && m_hBmp && m_pBmi && hFile)
   {
      BITMAPFILEHEADER bmfh;
      int              nColors = 0, nScanlines;
      DWORD            dwNoOfBytes = 0;
      if (m_pBmi->bmiHeader.biBitCount<9) nColors = 1 << m_pBmi->bmiHeader.biBitCount;
      if (m_pBmi->bmiHeader.biClrUsed!=0) nColors = m_pBmi->bmiHeader.biClrUsed;
      nScanlines = ::GetDIBits(m_hDC, m_hBmp, 0, abs(m_pBmi->bmiHeader.biHeight), NULL, m_pBmi, m_iUsage);

      // Fileheader füllen
      bmfh.bfType = 0x4d42;                                    // BM
      bmfh.bfSize = sizeof(BITMAPFILEHEADER) +                 // Fileheader
                    sizeof(BITMAPINFOHEADER) +                 // Bitmapinfo
                    (nColors * sizeof(RGBQUAD)) +              // Palettenfarben
                    m_pBmi->bmiHeader.biSizeImage;             // Länge der Daten

      bmfh.bfReserved1 = 0;
      bmfh.bfReserved2 = 0;
      bmfh.bfOffBits   = bmfh.bfSize - m_pBmi->bmiHeader.biSizeImage;

      ::WriteFile(hFile, &bmfh,              sizeof(BITMAPFILEHEADER), &dwNoOfBytes, NULL);
      ::WriteFile(hFile, &m_pBmi->bmiHeader, sizeof(BITMAPINFOHEADER), &dwNoOfBytes, NULL);
      if (m_iUsage == DIB_RGB_COLORS)
      {
         WriteFile(hFile, &m_pBmi->bmiColors[0], sizeof(RGBQUAD) * nColors, &dwNoOfBytes, NULL);
      }
      else if (m_hPalette)
      {
         PALETTEENTRY *pPe = NULL;
         RGBQUAD      *pColors;
         RGBQUAD       Color;
         pPe = new PALETTEENTRY[nColors];
         GetPaletteEntries(0, nColors, pPe);
         pColors = (RGBQUAD*) pPe;
         Color.rgbReserved = 0;

         for (int i=0; i<nColors; i++)
         {
            Color.rgbBlue  = pPe[i].peBlue;
            Color.rgbGreen = pPe[i].peGreen;
            Color.rgbRed   = pPe[i].peRed;
            pColors[i]     = Color;
         }

         ::WriteFile(hFile, pColors, sizeof(RGBQUAD) * nColors, &dwNoOfBytes, NULL);
         delete[] pPe;
      }

      ::WriteFile(hFile, m_pvBits, m_pBmi->bmiHeader.biSizeImage, &dwNoOfBytes, NULL);
      return true;
   }
   return false;
}

void CDibSection::SetColorUsage(int u, int nColors)
{
   m_iUsage = u;
   if (m_iUsage == DIB_RGB_COLORS)
   {
      m_nColorBits = nColors;
   }
   else if (m_iUsage == DIB_PAL_COLORS)
   {
      if      (nColors ==    0) m_nColorBits =    8;
      else if (nColors <=    2) m_nColorBits =    1;
      else if (nColors <=   16) m_nColorBits =    4;
      else if (nColors <=  256) m_nColorBits =    8;
      else                      m_nColorBits =   12;
   }
}

void CDibSection::SetColorUsage(HDC hDC)
{
   int nRastCaps = ::GetDeviceCaps(hDC, RASTERCAPS);
   m_nColorBits  = ::GetDeviceCaps(hDC, BITSPIXEL);            // Anzahl der Farben ermitteln
   if (nRastCaps&RC_PALETTE)                                   // wird eine Palette benötigt
   {
      if (m_nColorBits > 12) m_nColorBits = 12;                // nicht mehr als 4096 farben
      m_iUsage = DIB_PAL_COLORS;                               // benutzung der DIB-Section mit Palettenindizes
   }
   else
   {
      m_iUsage = DIB_RGB_COLORS;                               // benutzung der DIB-Section mit Palettenindizes
   }
}

void CDibSection::OnPaletteChanged()
{
   if (RealizePalette()!=0) ::UpdateColors(m_hDC);
}

UINT CDibSection::RealizePalette()
{
   if (m_hPalette) return ::RealizePalette(m_hDC);
   else return false;
}

UINT CDibSection::SetPaletteColor(int i, COLORREF c)
{
   if ((m_hPalette!=NULL) && (i > 15) && (i < GetNumColors()))
   {
      PALETTEENTRY PalEntry;
      PalEntry.peRed   = GetRValue(c);
      PalEntry.peGreen = GetGValue(c);
      PalEntry.peBlue  = GetBValue(c);
      PalEntry.peFlags = 0; // PC_NOCOLLAPSE;
      return ::SetPaletteEntries(m_hPalette, i, 1, &PalEntry);
   }
   return 0;
}

UINT CDibSection::GetPaletteEntries(UINT iStartIndex, UINT nEntries, LPPALETTEENTRY lppe)
{
   if (m_hPalette) return ::GetPaletteEntries(m_hPalette, iStartIndex, nEntries, lppe);
   else            return 0;
}

UINT CDibSection::GetNearestPaletteIndex(COLORREF c)
{
   if (m_hPalette) return ::GetNearestPaletteIndex(m_hPalette, c);
   else            return 0;
}

COLORREF CDibSection::GetPixel(int x, int y)
{
   if ((m_pBmi != NULL) && (m_pvBits != NULL) && 
       (x >= 0) && (y >= 0) &&
       (x < abs(m_pBmi->bmiHeader.biWidth )) && 
       (y < abs(m_pBmi->bmiHeader.biHeight)))
   {
      char *pBits = (char*)m_pvBits;
      switch(m_pBmi->bmiHeader.biBitCount)
      {
         case  1:
         {
            BYTE nCol = (BYTE) pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth)*y+(x>>3)];
            BYTE nBit = 1 << (x&0x07);
            RGBQUAD rgbq = *((RGBQUAD*)&m_pBmi->bmiColors[(nCol&nBit) ? 1 : 0]);
            return RGB(rgbq.rgbBlue, rgbq.rgbGreen, rgbq.rgbRed);
         }
         case  4:
         {
            int nWidth = m_pBmi->bmiHeader.biWidth + (m_pBmi->bmiHeader.biWidth&0x01);
            BYTE nCol = (BYTE) pBits[ALIGN_LONG(nWidth*4)*y+(x>>1)];
            RGBQUAD rgbq = *((RGBQUAD*)&m_pBmi->bmiColors[(x&0x01) ? (nCol&0x0f):((nCol&0xf0)>>4)]);
            return RGB(rgbq.rgbBlue, rgbq.rgbGreen, rgbq.rgbRed);
         }
         case  8:
         {
            BYTE nCol = (BYTE) pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth*8)*y+x];
            RGBQUAD rgbq = *((RGBQUAD*)&m_pBmi->bmiColors[nCol]);
            return RGB(rgbq.rgbBlue, rgbq.rgbGreen, rgbq.rgbRed);
         }
         case 16:
         {
            // biCompression == BI_BITFIELDS
            // RGB555 rgbq = *((RGB555*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth*16)*y+(x*2)]);
            // return RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
            RGB555 rgbq = *((RGB555*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth<<4)*y+(x<<1)]);
            return RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
         }
         case 24:
         {
            RGBTRIPPLE rgbq = *((RGBTRIPPLE*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth*24)*y+x*3]);
            return RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
         }
         case 32:
         {
            RGBQUAD rgbq = *((RGBQUAD*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth<<5)*y+(x<<2)]);
            return RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
         }
         default: break;
      }
   }
   return 0;
}

void CDibSection::SetPixel(int x, int y, COLORREF color)
{
   if ((m_pBmi != NULL) && (m_pvBits != NULL) && 
       (x >= 0) && (y >= 0) &&
       (x < abs(m_pBmi->bmiHeader.biWidth )) && 
       (y < abs(m_pBmi->bmiHeader.biHeight)))
   {
      char *pBits = (char*)m_pvBits;
      switch(m_pBmi->bmiHeader.biBitCount)
      {
         case  1:
         {
            BYTE *pnCol = (BYTE*) &pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth)*y+(x>>3)];
            BYTE nBit = 1 << (x&0x07);
            if (GetNearestPaletteIndex(color)) *pnCol |=  nBit;
            else                               *pnCol &= ~nBit;
            break;
         }
         case  4:
         {
            int nWidth = m_pBmi->bmiHeader.biWidth + (m_pBmi->bmiHeader.biWidth&0x01);
            BYTE *pnCol = (BYTE*) &pBits[ALIGN_LONG(nWidth*4)*y+(x>>1)];
            if (x&0x01)
            {
               *pnCol &= 0xf0;
               *pnCol |= 0x0f &  GetNearestPaletteIndex(color);
            }
            else        
            {
               *pnCol &= 0x0f;
               *pnCol |= 0xf0 & (GetNearestPaletteIndex(color)<<4);
            }
            break;
         }
         case  8:
         {
            pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth*8)*y+x] = GetNearestPaletteIndex(color);
            break;
         }
         case 16:
         {
            // biCompression == BI_BITFIELDS
            // RGB555 rgbq = *((RGB555*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth*16)*y+(x*2)]);
            // return RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
            RGB555 *prgbq   = ((RGB555*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth<<4)*y+(x<<1)]);
            prgbq->rgbBlue  = GetBValue(color);
            prgbq->rgbGreen = GetGValue(color);
            prgbq->rgbRed   = GetRValue(color);
            break;
         }
         case 24:
         {
            RGBTRIPPLE *prgbq = ((RGBTRIPPLE*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth*24)*y+x*3]);
            prgbq->rgbBlue    = GetBValue(color);
            prgbq->rgbGreen   = GetGValue(color);
            prgbq->rgbRed     = GetRValue(color);
            break;
         }
         case 32:
         {
            RGBQUAD *prgbq  = ((RGBQUAD*)&pBits[ALIGN_LONG(m_pBmi->bmiHeader.biWidth<<5)*y+(x<<2)]);
            prgbq->rgbBlue  = GetBValue(color);
            prgbq->rgbGreen = GetGValue(color);
            prgbq->rgbRed   = GetRValue(color);
            break;
         }
         default: break;
      }
   }
}

int CDibSection::CreateBmpBW(int nColors, int cx, int cy, HDC hdcOrg)
{
   bool bReturn = true;
   HDC  memdc  = NULL;
   HDC  memdc2 = NULL;
   HDC  hdc    = NULL;
   try
   {
      int i, nBWColors = 0;

      if (hdcOrg) hdc = hdcOrg;
      else        hdc = ::GetDC(NULL);                         // Desktop Wnd !
      if(!hdc)     throw (DWORD) 2;

      memdc = CreateCompatibleDC(hdc);                         // Compatiblen DC erzeugen
      if(!memdc) throw (DWORD) 3;
      SaveDC(memdc);

      DestroySecondBmp();                                      // alte Bitmap zerstören
      if (hdcOrg)
      {
         COLORREF color, bwcol;
         for (i=0; i<256; i++)
         {
            color = RGB(i,i,i);
            bwcol = ::GetNearestColor(hdcOrg, color);
            if (color == bwcol) nBWColors++;
         }
         if (nBWColors > nColors) nColors = nBWColors;
      }
                                                               // neue erzeugen
      if      (nColors < 2) throw (DWORD) 4;
      m_pSecondBmi = (BITMAPINFO*) new char[sizeof(BITMAPINFOHEADER) + (nColors * sizeof(RGBQUAD))];
      if (!m_pSecondBmi) throw (DWORD) 5;

      if (m_pBmi)
      {
         m_pSecondBmi->bmiHeader = m_pBmi->bmiHeader;
      }
      else if ((cx > 0) && (cy > 0))
      {
         ZeroMemory(&m_pSecondBmi->bmiHeader, sizeof(BITMAPINFOHEADER));
         m_pSecondBmi->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
         m_pSecondBmi->bmiHeader.biWidth  = cx;
         m_pSecondBmi->bmiHeader.biHeight = cy;
         m_pSecondBmi->bmiHeader.biXPelsPerMeter = 100;
         m_pSecondBmi->bmiHeader.biYPelsPerMeter = 100;
      }
      else  throw (DWORD) 1;

      m_pSecondBmi->bmiHeader.biPlanes        = 1;

      if      (nColors <=    2) m_pSecondBmi->bmiHeader.biBitCount = 1, nColors =   2;
      else if (nColors <=   16) m_pSecondBmi->bmiHeader.biBitCount = 4, nColors =  16;
      else if (nColors <=  256) m_pSecondBmi->bmiHeader.biBitCount = 8, nColors = 256;
      else throw (DWORD) 6;

      m_pSecondBmi->bmiHeader.biCompression   = BI_RGB;
      m_pSecondBmi->bmiHeader.biSizeImage     = GetImageSize(m_pSecondBmi->bmiHeader.biWidth, m_pSecondBmi->bmiHeader.biHeight, m_pSecondBmi->bmiHeader.biBitCount);
      m_pSecondBmi->bmiHeader.biClrUsed       = 0;
      m_pSecondBmi->bmiHeader.biClrImportant  = nColors;

      ZeroMemory(m_pSecondBmi->bmiColors, nColors*sizeof(RGBQUAD));

      for (i=1; i<nColors; i++)
      {
         if      (m_pSecondBmi->bmiHeader.biBitCount ==  1)
         {
            nBWColors = (i << 8) -1;
         }
         else if (m_pSecondBmi->bmiHeader.biBitCount ==  4)
         {
            COLORREF color, bwcol;
            nBWColors = (i << 4) -1;
            color = RGB(nBWColors,nBWColors,nBWColors);
            bwcol = ::GetNearestColor(hdcOrg, color);
            GetRValue(bwcol);
         }
         else nBWColors = i;
         m_pSecondBmi->bmiColors[i].rgbBlue    = nBWColors;
         m_pSecondBmi->bmiColors[i].rgbGreen   = nBWColors;
         m_pSecondBmi->bmiColors[i].rgbRed     = nBWColors;
      } 
                                           // erzeuge ein 256 Farb Bitmap mit 256 Graustufen
      m_hSecondBmp = CreateDIBSection(memdc, (BITMAPINFO*)m_pSecondBmi, DIB_RGB_COLORS, &m_pvSecondBits, NULL, 0);
      if (!m_hSecondBmp)  throw (DWORD) 7;
      if (!m_pBmi)
      {
         RestoreDC(memdc,-1);
         m_hDC = memdc;
         m_hOldBmp = (HBITMAP) ::SelectObject(m_hDC, m_hSecondBmp);
         memdc = NULL;
         throw (DWORD)0;
      }

      memdc2 = CreateCompatibleDC(hdc);
      if(!memdc2) throw (DWORD) 8;

      SaveDC(memdc2);
      SetStretchBltMode(memdc2,HALFTONE);
      SetBrushOrgEx(memdc2,0,0,NULL);

      SelectObject(memdc2, m_hSecondBmp);
      RECT rcDest = {0,0, m_pSecondBmi->bmiHeader.biWidth, m_pSecondBmi->bmiHeader.biHeight};
      if (StretchDIBits(memdc2, rcDest, false, SRCCOPY) == 0)  throw (DWORD) 9;
   }
   catch (DWORD dwError)
   {
      if (dwError)
      {
         DestroySecondBmp();                                      // alte Bitmap zerstören
         bReturn = false;
      }
   }

   if (memdc2)
   {
      RestoreDC(memdc2,-1);
      DeleteDC(memdc2);
   }
   if (memdc)
   {
      RestoreDC(memdc,-1);
      DeleteDC(memdc);
   }
   if ((hdc != NULL) && (hdcOrg == NULL))
   {
      ReleaseDC(NULL, hdc);
   }

   return bReturn;
}

int CDibSection::GetWidth()
{
   if (m_pBmi)       return m_pBmi->bmiHeader.biWidth;
   if (m_pSecondBmi) return m_pSecondBmi->bmiHeader.biWidth;
   return 0;
}

int CDibSection::GetHeight()
{
   if (m_pBmi)       return m_pBmi->bmiHeader.biHeight;
   if (m_pSecondBmi) return m_pSecondBmi->bmiHeader.biHeight;
   return 0;
};

DWORD CDibSection::GetImageSize()
{
   if (m_pBmi)       return GetImageSize(m_pBmi->bmiHeader.biWidth      , m_pBmi->bmiHeader.biHeight      , m_pBmi->bmiHeader.biBitCount      );
   if (m_pSecondBmi) return GetImageSize(m_pSecondBmi->bmiHeader.biWidth, m_pSecondBmi->bmiHeader.biHeight, m_pSecondBmi->bmiHeader.biBitCount);
   return 0;
};

void *CDibSection::GetBits(bool bFirst)
{
   if (bFirst) return m_pvBits;
   else        return m_pvSecondBits;
};

int CDibSection::ConvertBitCount(int nDest)
{
   int nReturn = -1;
   if (m_pBmi)
   {
      int nOld = m_pBmi->bmiHeader.biBitCount;
      HDC memdc1 = NULL, memdc2 = NULL;
      HBITMAP hbit = NULL;
      try
      {
         if (nOld == nDest) throw (int)0;
         m_pBmi->bmiHeader.biBitCount = nDest;

         memdc1 = ::CreateCompatibleDC(m_hDC);
         if (!memdc1) throw (int)1;
         ::SaveDC(memdc1);
         void *pvBits;
         hbit = ::CreateDIBSection(memdc1, m_pBmi, m_iUsage, &pvBits, NULL, NULL);
         if (!hbit)   throw (int)2;

         memdc2 = ::CreateCompatibleDC(m_hDC);
         if(!memdc2) throw (int) 3;
         ::SaveDC(memdc2);

         ::SetStretchBltMode(memdc2, HALFTONE);
         ::SetBrushOrgEx(memdc2,0,0,NULL);
         ::SelectObject(memdc2, hbit);
         RECT rcDest = {0,0, m_pBmi->bmiHeader.biWidth, m_pBmi->bmiHeader.biHeight};
         m_pBmi->bmiHeader.biBitCount = nOld;
         if (StretchDIBits(memdc2, rcDest, false, SRCCOPY) == 0) throw (int)4;
         m_pBmi->bmiHeader.biBitCount = nDest;

         ::SelectObject(m_hDC, hbit);
         ::DeleteObject(m_hBmp);
         m_hBmp   = hbit;
         m_pvBits = pvBits;
         hbit     = NULL;
         nReturn  = 0;
      }
      catch (int nError)
      {
         m_pBmi->bmiHeader.biBitCount = nOld;
         nReturn = nError;
      }

      if (memdc2)
      {
         ::RestoreDC(memdc2, -1);
         ::DeleteDC(memdc2);
      }
      if (memdc1)
      {
         ::RestoreDC(memdc1, -1);
         ::DeleteDC(memdc1);
      }
      if (hbit) ::DeleteObject(hbit);
   }
   return nReturn;
}

int CDibSection::Convert32to24bit()
{
   if (m_pBmi && (m_pBmi->bmiHeader.biBitCount == 32))
      return ConvertBitCount(24);
   return 0;
}

bool CDibSection::InitPalette()
{
   if (m_pBmi && (m_hPalette == NULL))
   {
      int i, nColors = max(m_pBmi->bmiHeader.biClrUsed, m_pBmi->bmiHeader.biClrImportant);
      if (nColors > 0)
      {
         LOGPALETTE *pPalette;
         pPalette = (LPLOGPALETTE) new char[sizeof (LOGPALETTE) + (nColors * sizeof (PALETTEENTRY))];
         pPalette->palVersion    = 0x300;
         pPalette->palNumEntries = (WORD) nColors;                // Standard Farbtabelle für die Palette berechnen
         for (i=0; i<nColors; i++)
         {
            pPalette->palPalEntry[i].peRed   = m_pBmi->bmiColors[i].rgbRed;
            pPalette->palPalEntry[i].peGreen = m_pBmi->bmiColors[i].rgbGreen;
            pPalette->palPalEntry[i].peBlue  = m_pBmi->bmiColors[i].rgbBlue;
            pPalette->palPalEntry[i].peFlags = 0;
         }
         m_hPalette = ::CreatePalette(pPalette);
         delete[] pPalette;
         return true;
      }
   }
   return false;
}
