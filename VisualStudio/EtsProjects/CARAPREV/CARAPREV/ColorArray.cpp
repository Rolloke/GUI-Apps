// ColorArray.cpp: Implementierung der Klasse CColorArray.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ColorArray.h"
#include "Ets3DGL.h"
#include "DibSection.h"
#include "ColorArray.h"
#include "Label.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
UINT       CColorArray::gm_nInstances   = 0;
HANDLE     CColorArray::gm_hHeap        = 0;
int        CColorArray::gm_nNeedPalette = -1;
COLORREF   CColorArray::gm_cDummy       = 0;

CColorArray::CColorArray()
{
   m_hPalette   = NULL;
   m_pColors    = NULL;
   m_nColors    = 0;
   m_nColorMode = 0;
   m_bChanged   = false;
   if (gm_nNeedPalette == -1) // ist die Variable noch nicht Initialisiert
   {
      HDC hdc   = ::GetDC(NULL);
      if (::GetDeviceCaps(hdc, BITSPIXEL) < 16)
         gm_nNeedPalette = 1;
      else
         gm_nNeedPalette = 0;
      ::ReleaseDC(NULL, hdc);

//      gm_nNeedPalette = 0;
   }
}

CColorArray::~CColorArray()
{
   if (m_pColors)
   {
      free(m_pColors);
      m_pColors = NULL;
   }
}

/******************************************************************************
* Name       : alloc                                                          *
* Zweck      : Allozieren bzw. Reallozieren von Speicherplatz auf dem Heap der*
*              Klasse CColorArray.                                             *
* Definition : void * alloc(size_t, void *pOld = NULL);                       *
* Aufruf     : alloc(nSize, pOld);                                            *
* Parameter  :                                                                *
* nSize   (E): Größe des angeforderten Speichers in BYTE             (size_t) *
* pOld    (E): Zeiger auf altes Array für Realloc()                  (void*)  *
* Funktionswert : Zeiger auf allozierten Speicher                    (void*)  *
******************************************************************************/
void* CColorArray:: alloc(size_t nSize, void *pOld)
{
   void * pointer = NULL;
   try
   {
      if(gm_nInstances==0)
      {
         if(gm_hHeap!=0) throw (int) IDE_HEAP_EXISTS; 
         gm_hHeap = HeapCreate(0, 1048576,0); // growable Heap
         if(gm_hHeap==0) throw (int) IDE_HEAP_CREATION; 
      }
      if (pOld) pointer = HeapReAlloc(gm_hHeap,0, pOld, nSize);
      else      pointer = HeapAlloc(gm_hHeap,0, nSize);

      if(pointer==NULL) throw (int) IDE_OUT_OF_MEMORY;

      if (pOld==NULL) gm_nInstances++;
   }
   catch (int nError)
   {
      AfxMessageBox(nError, MB_OK|MB_ICONEXCLAMATION);
   }
   return pointer;
}
/******************************************************************************
* Name       : free                                                           *
* Zweck      : Freigeben des Speicherplatzes auf dem Heap der Klasse          *
*              CColorArray.                                                   *
* Definition : void free(void* p);                                            *
* Aufruf     : free(p);                                                       *
* Parameter  :                                                                *
* p      (E) : Zeicher auf Speicherobjekt                            (void*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CColorArray:: free(void* p)
{
   try
   {
      if(gm_hHeap==0)                      throw (int) IDE_DESTRUCTION_WITHOUT_HEAP;
      else if(HeapFree(gm_hHeap, 0, p)==0) throw (int) IDE_HEAP_FREE;

      gm_nInstances--;

      if(gm_nInstances==0)
      {
         if(HeapDestroy(gm_hHeap)==0)      throw (int) IDE_HEAP_DESTROY;
         gm_hHeap = 0;
      }
   }
   catch (int nError)
   {
      AfxMessageBox(nError, MB_OK|MB_ICONEXCLAMATION);
   }
}

/******************************************************************************
* Name       : GetColors                                                      *
* Zweck      : Kopiert eine Anzahl von Farben aus diesem Array in das         *
*              übergebene Array.                                              *
* Definition : bool GetColors(COLORREF*, int, int nFirst=0);                  *
* Aufruf     : GetColors(pColors, nColors, [nFirst]);                         *
* Parameter  :                                                                *
* pColors (A): Array für die Farben                               (COLORREF)  *
* nColors (E): Anzahl der kopierten Farben                        (int)       *
* [nFirst](E): Startindex für die Farben aus diesem Array         (int)       *
* Funktionswert : Farben wurden kopiert (true, false)                         *
******************************************************************************/
bool CColorArray::GetColors(COLORREF *pColors, int nColors, int nFirst)
{
   if (pColors && m_pColors)
   {
      int minColor = nFirst + nColors;
      if (minColor>m_nColors) return false;

      for (int i=nFirst; i<minColor; i++)
         pColors[i] = m_pColors[i];

      return true;
   }
   return false;
}

/******************************************************************************
* Name       : SetColors                                                      *
* Zweck      : Kopiert eine Anzahl von Farben aus dem übergebenen  Array in   *
*              dieses Array.                                                  *
* Definition : bool SetColors(COLORREF*, int, int nFirst=0);                  *
* Aufruf     : SetColors(pColors, nColors, [nFirst]);                         *
* Parameter  :                                                                *
* pColors (A): Array für die übergebenen Farben                   (COLORREF)  *
* nColors (E): Anzahl der kopierten Farben                        (int)       *
* [nFirst](E): Startindex für die Farben in diesem Array          (int)       *
* Funktionswert : Farben wurden kopiert (true, false)                         *
******************************************************************************/
bool CColorArray::SetColors(COLORREF *pColors, int nColors, int nFirst)
{
   int minColor = nFirst + nColors;
   if (m_pColors == NULL)
   {
      m_pColors = (COLORREF*) alloc(sizeof(COLORREF)*minColor);
      if (m_pColors) m_nColors = minColor;
   }
   if (pColors && m_pColors)
   {
      if (minColor>m_nColors) return false;

      for (int i=nFirst; i<minColor; i++)
         m_pColors[i] = pColors[i];

      SetPaletteColors();
      m_bChanged = true;
      return true;
   }
   return false;
}

/******************************************************************************
* Name       : SetPaletteColors                                               *
* Zweck      : Setzt die Paletteneinträge für die Darstellung der Farben des  *
*              Farbarrays.                                                    *
* Definition : void SetPaletteColors();                                       *
* Aufruf     : SetPaletteColors();                                            *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CColorArray::SetPaletteColors()
{
   if (m_hPalette && m_pColors)
   {
      PALETTEENTRY PalEntry;
      PalEntry.peFlags = 0; // PC_NOCOLLAPSE;
      for (int i=0; i<m_nColors; i++)
      {
         PalEntry.peRed   = GetRValue(m_pColors[i]);
         PalEntry.peGreen = GetGValue(m_pColors[i]);
         PalEntry.peBlue  = GetBValue(m_pColors[i]);
         SetPaletteEntries(m_hPalette, i, 1, &PalEntry);
      }
  }
}

COLORREF& CColorArray::SetColor(int nIndex)
{
   if (CheckRange(nIndex))
   {
      m_bChanged = true;
      return m_pColors[nIndex];
   }
   return gm_cDummy;
}

COLORREF CColorArray::GetColor(int nIndex)
{
   if (CheckRange(nIndex))
   {
      return m_pColors[nIndex];
   }
   return INVALID_COLOR;
}

void CColorArray::InterpolateColors(int nFirstColor, int nLastColor, COLORREF FirstColor, COLORREF LastColor)
{
   if (CheckRange(nFirstColor) && CheckRange(nLastColor))
      ::InterpolateColors(nFirstColor, nLastColor, FirstColor, LastColor, m_pColors);
}


void CColorArray::SetIndexColors(int nFirstColor, int nLastColor, COLORREF FirstColor, COLORREF LastColor)
{
   if (CheckRange(nFirstColor) && CheckRange(nLastColor))
      ::SetIndexColors(nFirstColor, nLastColor, FirstColor, LastColor, m_pColors);
}

bool CColorArray::SetArraySize(int newSize)
{
   if (newSize < 0)              return false;                 // negative Anzahl: Fehler

   bool bOldColor = false;
   int  oldSize = m_nColors,                                   // alte Größe für die Berechnung sichern
        i, j1, j2;

   if (m_pColors)                                              // Wenn alte Farben vorhanden waren
   {
      if (newSize == 0)                                        // Arraygröße auf 0 setzen
      {
         free(m_pColors);                                      // Array löschen
         m_pColors = NULL;
         m_nColors = newSize;
         if (m_hPalette) ::DeleteObject(m_hPalette);
         m_hPalette = 0;
         m_bChanged = true;
         return true;
      }
      if (m_nColors == newSize)  return true;                  // Die gleiche Größe nochmal setzen ist quatsch aber macht auch nichts

      if (oldSize > newSize)                                   // ist das alte Array größer, Farben weglassen
      {
         for (i=0; i<newSize; i++)
         {
            j1 = MulDiv(i, oldSize, newSize);
            if ((j1 >= 0) && (j1 < oldSize))
               m_pColors[i] = m_pColors[j1];
         }
      }
      else bOldColor = true;
   }
   if (newSize == 0) return true;

   m_pColors = (COLORREF *) alloc(sizeof(DWORD)*newSize, m_pColors);// neues Array allozieren

   if (m_pColors)                                              // allozieren ok
   {
      m_nColors   = newSize;
      if ((bOldColor) && (oldSize < newSize))                  // wenn alte Farben vorhanden waren und
      {                                                        // das alte Array kleiner ist, Farben interpolieren
         oldSize--;
         j2 = newSize-1;
         for (i=oldSize; i>0; i--)
         {
            j1 = MulDiv(i-1, newSize, oldSize);
            if (m_nColorMode & CV_INTERPOLATE)
               InterpolateColors(j1, j2, m_pColors[i-1], m_pColors[i]);
            else
               SetIndexColors(   j1, j2, m_pColors[i-1], m_pColors[i]);
            j2 = j1;
         }
      }

      if (gm_nNeedPalette)                                     // wird eine Palette benötigt
      {
         if (m_hPalette == NULL)                               // Ist keine Palette vorhanden
         {
            LOGPALETTE *pPalette = (LPLOGPALETTE) new char[sizeof (LOGPALETTE) + (m_nColors * sizeof (PALETTEENTRY))];
            if (pPalette)
            {
               pPalette->palNumEntries = m_nColors;
               pPalette->palVersion    = 0x300;
               CDibSection::CalculatePaletteColors((RGBQUAD*)&pPalette->palPalEntry[0], m_nColors, false);
               m_hPalette = ::CreatePalette(pPalette);         // Palette mit Standardfarben erzeugen
               delete pPalette;
            }
         }
         else
         {
            if (::ResizePalette(m_hPalette, m_nColors))
            {
               ::DeleteObject(m_hPalette);
            }
            else SetPaletteColors();
         }
      }
      m_bChanged = true;
      return true;
   }
   return false;
}

void CColorArray::operator =(CColorArray &pCopy)
{
   SetArraySize(0);
   SetArraySize(pCopy.m_nColors);
   SetColors(pCopy.m_pColors, pCopy.m_nColors);
   m_nColorMode = pCopy.m_nColorMode;
}

void CColorArray::Draw(CDC *pDC, CRect *prc)
{
   CRect rc(*prc);
   if (pDC->GetMapMode() != MM_TEXT)
      pDC->LPtoDP(&rc);

   rc.NormalizeRect();
   double dGradient, dOrigin;
   int nWidth  = rc.Width(),
       nHeight = rc.Height(),
       nColors;
   div_t        divider;
   if ((nWidth > 1) && (nHeight > 1))
   {
      if (nWidth > nHeight)                                       // horizontal
      {
         int nTop = prc->top, i, 
             cy   = prc->bottom - prc->top;
         nColors  = GetNumColor();
         CPoint p;
         if (nWidth <= nColors)
         {
            divider = div(nColors, nWidth);
            if (divider.rem)
            {
               dGradient = (double)nColors / (double)(prc->right - prc->left);
               dOrigin   = nColors - dGradient * (double)prc->right;
               for (i=rc.left; i<rc.right; i++)
               {
                  p.x = i;
                  pDC->DPtoLP(&p);
                  FillSolidRect(pDC, p.x, nTop, 1, cy, GetColor((int)((dOrigin+((double)p.x)*dGradient))));
               }
            }
            else
            {
               for (i=rc.left; i<rc.right; i++)
               {
                  p.x = i;
                  pDC->DPtoLP(&p);
                  FillSolidRect(pDC, p.x, nTop, 1, cy, GetColor(i*divider.quot));
               }
            }
         }
         else
         {
            dGradient = (double)(prc->right - prc->left) / (double)nColors;
            p.x = prc->left;
            for (i=0; i<nColors; i++)
            {
               p.y = prc->left + (int)(dGradient*(double)(i+1));
               FillSolidRect(pDC, p.x, nTop, p.y-p.x, cy, GetColor(i));
               p.x = p.y;
            }
            if(p.y != prc->right)
               FillSolidRect(pDC, p.x, nTop, prc->right-p.x, cy, GetColor(nColors-1));
         }
      }
      else                                                        // vertikal
      {
         int nLeft = prc->left, i, 
             cx    = prc->right - prc->left;
         nColors   = GetNumColor();
         CPoint p;
         if (nHeight < nColors)
         {
            divider = div(nColors, nHeight);
            if (divider.rem)
            {
               dGradient = (double)nColors / (double)(prc->bottom - prc->top);
               dOrigin   = nColors - dGradient * (double)prc->bottom;
               for (i=rc.top; i<rc.bottom; i++)
               {
                  p.y = i;
                  pDC->DPtoLP(&p);
                  FillSolidRect(pDC, nLeft, p.y, cx, 1, GetColor(Round(dOrigin+((double)p.y)*dGradient)));
               }
            }
            else
            {
               for (i=rc.top; i<rc.bottom; i++)
               {
                  p.y = i;
                  pDC->DPtoLP(&p);
                  FillSolidRect(pDC, nLeft, p.y, cx, 1, GetColor(i*divider.quot));
               }
            }
         }
         else
         {
            dGradient = (double)(prc->bottom - prc->top) / (double)nColors;
            p.y = prc->top;
            for (i=0; i<nColors; i++)
            {
               p.x = prc->top + Round(dGradient*(double)(i+1));
               FillSolidRect(pDC, nLeft, p.y, cx, p.x-p.y, GetColor(i));
               p.y = p.x;
            }
            if (p.x != prc->bottom)
               FillSolidRect(pDC, nLeft, p.y, cx, prc->bottom-p.y, GetColor(nColors-1));
         }
      }
   }
}

bool CColorArray::CheckRange(int& nIndex)
{
   if (m_nColors == 0) return false;
   if (nIndex <          0) nIndex = 0;
   if (nIndex >= m_nColors) nIndex = m_nColors-1;
   return true;
}
int CColorArray::Round(double dIndex)
{
   int nIndex = (int) dIndex;
   dIndex -= (double)nIndex;
   if (dIndex >= 0.5) nIndex++;
   return nIndex;
}

bool CColorArray::IsChanged()
{
   bool bChanged = m_bChanged;
   m_bChanged = false;
   return bChanged;
}

const COLORREF* CColorArray::GetColorBuffer()
{
   return m_pColors;
}

void CColorArray::FillSolidRect(CDC *pDC, int l, int t, int w, int h, COLORREF col)
{
   if (CLabel::gm_bColorMatching) pDC->SetBkColor(col);
   else                           pDC->CDC::SetBkColor(col);
   CRect rect(l, t, l + w, t + h);
   ::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}
