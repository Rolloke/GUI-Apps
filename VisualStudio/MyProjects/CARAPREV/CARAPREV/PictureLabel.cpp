// PictureLabel.cpp: Implementierung der Klasse CPictureLabel.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DibSection.h"
#include "PictureLabel.h"
#include "TextLabel.h"
#include "resource.h"
#include "PreviewFileHeader.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CPictureLabel, CRectLabel, 1);

CPictureLabel::CPictureLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CPictureLabel::Constructor");
   #endif
   m_pDibSection     = NULL;
   m_hMetaFile       = NULL;
   m_State.scaling   = SCALE_ARBITRARY;
   m_nMode           = SRCCOPY;
   m_nStretchMode    = COLORONCOLOR;
   m_bSaveInFile     = false;
   m_bNoFile         = false;
   m_szPrintSize.cx  = 0;
   m_szPrintSize.cy  = 0;
}

#ifdef _DEBUG
void CPictureLabel::AssertValid() const
{
   CLabel::AssertValid();
//   ASSERT((m_pDibSection==NULL) || (m_hMetaFile==NULL));
}
#endif

CPictureLabel::~CPictureLabel()
{
   #ifdef CHECK_LABEL_DESTRUCTION
   REPORT("CPictureLabel::Destructor");
   #endif
   if (m_pDibSection) delete m_pDibSection;
   if (m_hMetaFile)  ::DeleteEnhMetaFile(m_hMetaFile);
}

bool CPictureLabel::Draw(CDC *pDC, int bAtr)
{
   if (!CLabel::Draw(pDC, bAtr)) return false;

   CRect rcDest;
   if (m_State.scaling==SCALE_ARBITRARY)
   {
      rcDest.left   = m_pP[0].x;
      rcDest.top    = m_pP[0].y;
      rcDest.right  = m_pP[1].x;
      rcDest.bottom = m_pP[1].y;
   }
   else
   {
      rcDest = CLabel::GetRect(pDC);
      if (IsNegativeY(pDC)) swap(rcDest.top, rcDest.bottom);
   }

   CRect rcDev = rcDest;
   ::LPtoDP(pDC->m_hAttribDC, (POINT*)&rcDev, 2);
   m_szPrintSize = rcDest.Size();
   if (IsNegativeY(pDC)) m_szPrintSize.cy = -m_szPrintSize.cy;

   if (m_pDibSection && m_pDibSection->GetBits())
   {
      CPoint ptOrg;
      ::GetBrushOrgEx(pDC->m_hDC, &ptOrg);
      int nStretchMode = ::SetStretchBltMode(pDC->m_hDC, m_nStretchMode);
      if (m_nStretchMode == HALFTONE) 
         ::SetBrushOrgEx(pDC->m_hDC, ptOrg.x, ptOrg.y, &ptOrg);
      bool bSecond = false;

      if (CLabel::gm_bColorMatching)
      {
         COLORREF color = RGB(0, 255, 0);
         COLORREF bwcol = pDC->GetNearestColor(color);
         if (color == bwcol);
         else if (m_pDibSection->GetSecondBitmapHandle())
         {
            bSecond = true;
         }
         else
         {
            bSecond = (m_pDibSection->CreateBmpBW(2, 0, 0, pDC->m_hAttribDC) != 0) ? true : false;
         }
      }

      if (m_pDibSection->StretchDIBits(pDC->m_hDC, rcDest, bSecond, m_nMode)==0)
      {
          m_pDibSection->StretchDIBits(pDC->m_hDC, rcDest, !bSecond, m_nMode);
      }
      ::SetStretchBltMode(pDC->m_hDC, nStretchMode);
   }
   else if (m_hMetaFile)
   {
      bool bSecond = false;
      if (CLabel::gm_bColorMatching)
      {
         CRect rcDev = GetRect(pDC, DEV_COORD);
         RECT  rcBmp = {0, 0, rcDev.Width(), rcDev.Height()};
         COLORREF color = RGB(0, 255, 0);
         COLORREF bwcol = pDC->GetNearestColor(color);
         if (color == bwcol);
         else if (m_pDibSection &&
                  m_pDibSection->GetSecondBitmapHandle()      && 
                  (abs(m_pDibSection->GetWidth()-rcBmp.right  ) < 5) &&
                  (abs(m_pDibSection->GetHeight()-rcBmp.bottom) < 5))
         {
            bSecond = true;
         }
         else
         {
            if (!m_pDibSection) m_pDibSection = new CDibSection;
            else                m_pDibSection->Destroy();

            bSecond = (m_pDibSection->CreateBmpBW(2, rcBmp.right, rcBmp.bottom, pDC->m_hAttribDC) != 0) ? true : false;
            if (bSecond)
            {
               PatBlt(m_pDibSection->GetDC(), 0, 0, rcBmp.right, rcBmp.bottom, WHITENESS);
               PlayEnhMetaFile(m_pDibSection->GetDC(), m_hMetaFile, &rcBmp);
            }
         }
      }
      if (bSecond) m_pDibSection->StretchDIBits(pDC->m_hDC, rcDest, bSecond, m_nMode);
      else pDC->PlayMetaFile( m_hMetaFile, &rcDest);
   }
   else
   {
      CRect rect = GetRect(pDC);
      CPoint pt  = rect.CenterPoint();
      CRectLabel::Draw(pDC, bAtr);
      if (m_strFileName.IsEmpty())
      {
         m_strFileName.LoadString(IDS_NO_PICTURE);
      }
      CTextLabel txt(&pt, (TCHAR*)LPCTSTR(m_strFileName));
      txt.SetTextAlign(TA_CENTER|TA_BASELINE);
      txt.SetClipRect(&rect, true);
      txt.Draw(pDC);
   }

   return true;
}

bool CPictureLabel::DrawShape(CDC * pDC, bool draw)
{
   return CRectLabel::DrawShape(pDC, draw);
}


void CPictureLabel::SetPoint(CPoint p)
{
   if (!IsFixed())
   {
      CLabel::SetPoint(p);
      CheckScale(m_pP[0], m_pP[1]);
   }
}

void CPictureLabel::AttachDIB(CDibSection *pDib, bool bSetSize)
{
   if (pDib)
   {
      m_pDibSection = pDib;
      if (bSetSize)
      {
         m_pP[0].x = 0;
         m_pP[0].y = 0;
         m_pP[1].x = m_pDibSection->GetWidth();
         m_pP[1].y = m_pDibSection->GetHeight();
         m_State.scaling  = SCALE_PROPORTIONAL;
         m_nMode = SRCCOPY;
      }
   }
}

void CPictureLabel::AttachMetaFile(HENHMETAFILE hm, bool bSetSize)
{
   if (hm)
   {
      int nSize = sizeof(ENHMETAHEADER);
      nSize = GetEnhMetaFileHeader(hm, 0, NULL);
      if (nSize == 0) return;
      ENHMETAHEADER *penmh = (ENHMETAHEADER*) new char[nSize];
      if (!penmh) return;
      penmh->nSize = nSize;
      penmh->iType = EMR_HEADER; 
      GetEnhMetaFileHeader(hm, nSize, penmh);
      if ((penmh->rclBounds.right == -1) || (penmh->rclBounds.bottom == -1));
      else
      {
          m_hMetaFile = hm;
          SetScaleSize(CSize(penmh->rclBounds.right  - penmh->rclBounds.left,
                             penmh->rclBounds.bottom - penmh->rclBounds.top));

          if (bSetSize)
          {
             m_pP[0].x = penmh->rclBounds.left;
             m_pP[0].y = penmh->rclBounds.top;
             m_pP[1].x = penmh->rclBounds.right;
             m_pP[1].y = penmh->rclBounds.bottom;
             m_State.scaling = SCALE_PROPORTIONAL;
          }
          else if (m_State.scaling == SCALE_ARBITRARY)
          {
             m_State.scaling = SCALE_NONPROPORTIONAL;
          }
      }
      if (penmh) delete[] (char*) penmh;
   }
}

CDibSection *CPictureLabel::DetachDIB()
{
   CDibSection *pDib = m_pDibSection;
   m_pDibSection = NULL;
   return pDib;
}

HENHMETAFILE CPictureLabel::DetachMetaFile()
{
   HENHMETAFILE hMeta = m_hMetaFile;
   m_hMetaFile = NULL;
   return hMeta;
}

void CPictureLabel::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CPictureLabel::Serialize()");
   #endif
   long nFormat = 0;
   CRectLabel::Serialize(ar);
   if (ar.IsStoring())                                         // Daten schreiben
   {
      ArchiveWrite(ar, &m_nMode, sizeof(DWORD));               // Modi
      ArchiveWrite(ar, &m_nStretchMode, sizeof(int));          // Stretching
      HANDLE hFile = (HANDLE) ar.GetFile()->m_hFile;
      if (m_pDibSection && m_pDibSection->GetBits())           // DibSection kann als
      {                                                        
         nFormat = DIB_FORMAT;                                 // Bitmapformat in der Datei,
         if      (hFile == (HANDLE)0xffffffff)                 // BitmapInfo mit angehängten Daten
            nFormat = CF_BITMAP;                               // für die Zwischenablage
         else if (m_bSaveInFile && !m_strFileName.IsEmpty())   // in einer separaten Datei als Bitmap
            nFormat = BITMAP_FILE;
         ArchiveWrite(ar, &nFormat, sizeof(long));             // gespeichert werden
         if (nFormat == CF_BITMAP)
         {
            UINT nSize;
            HGLOBAL  hMem = m_pDibSection->GetDIBData(&nSize);
            void    *pMem = ::GlobalLock(hMem);
            ArchiveWrite(ar, &nSize, sizeof(long));
            if (pMem && nSize) ArchiveWrite(ar, pMem, nSize);
            ::GlobalUnlock(hMem);
            ::GlobalFree(hMem);
         }
         else if (nFormat == BITMAP_FILE)
         {
            CFile bmpFile;                                     // wenn die Datei nicht existiert
            if (!bmpFile.Open(m_strFileName, CFile::modeRead, NULL))
            {                                                  // neue Datei erzeugen
               if (bmpFile.Open(m_strFileName, CFile::modeCreate|CFile::modeWrite))
               {                                               // und Speichern
                  m_pDibSection->SaveInFile((HANDLE)bmpFile.m_hFile);
                  m_strFileName = bmpFile.GetFilePath();       // Pfad und Dateinamen Speichern
               }
            }
            bmpFile.Close();
            long nLength = m_strFileName.GetLength();
            ArchiveWrite(ar, &nLength, sizeof(long));
            ArchiveWrite(ar, LPCTSTR(m_strFileName), nLength);
         }
         else                                                  // in der gleichen Datei speichern
         {
            DWORD dwPosition;
            ar.Flush();
            CFile *pFile = ar.GetFile();
            VERIFY(m_pDibSection->SaveInFile((HANDLE)pFile->m_hFile));
            dwPosition = ::SetFilePointer((HANDLE)pFile->m_hFile, 0, NULL, FILE_CURRENT);
         }
      }
      if (m_hMetaFile)                                         // Als MetaFile speichern
      {
         nFormat = META_FORMAT;
         ArchiveWrite(ar, &nFormat, sizeof(long));
         UINT nSize = GetEnhMetaFileBits(m_hMetaFile, 0, NULL);
         ArchiveWrite(ar, &nSize, sizeof(UINT));
         if (nSize>0)
         {
            BYTE * pBuff = new BYTE[nSize];
            if (pBuff)
            {
               GetEnhMetaFileBits(m_hMetaFile, nSize, pBuff);
               ArchiveWrite(ar, pBuff, nSize);
               delete[] pBuff;
            }
         }
      }
      if ((m_hMetaFile==NULL) && (m_pDibSection == NULL))
      {
         ArchiveWrite(ar, &nFormat, sizeof(long));             // Null-format schreiben
      }
   }
   else                                                        // Datei lesen
   {
      ArchiveRead(ar, &m_nMode, sizeof(DWORD));                // Modi
      ArchiveRead(ar, &m_nStretchMode, sizeof(int));           // Stretching
      ArchiveRead(ar, &nFormat, sizeof(long));                 // Format
      if (nFormat == DIB_FORMAT)                               // Bitmapformat in der Datei
      {
         DWORD dwPosition;
         ar.Flush();
         CFile *pFile = ar.GetFile();
         dwPosition = (DWORD)pFile->GetPosition();
         CDibSection *pDibSection = new CDibSection;
         if (pDibSection)
         {
            dwPosition = ::SetFilePointer((HANDLE)pFile->m_hFile, 0, NULL, FILE_CURRENT);
            pDibSection->LoadFromFile((HANDLE)pFile->m_hFile);
            dwPosition = ::SetFilePointer((HANDLE)pFile->m_hFile, 0, NULL, FILE_CURRENT);
            int nScale = m_State.scaling;
            AttachDIB(pDibSection);
            m_State.scaling = nScale;
         }
      }
      else if (nFormat == CF_BITMAP)                           // BitmapInfo mit angehängten Daten
      {                                                        // aus der Zwischenablage
         long nSize;
         ArchiveRead(ar, &nSize, sizeof(long));
         if (nSize > 0)
         {
            BITMAPINFO *pBmpInfo = (BITMAPINFO*) new char[nSize];
            if (pBmpInfo)
            {
               ArchiveRead(ar, pBmpInfo, nSize);
               CDibSection *pDibSection = new CDibSection;
               if (pDibSection)
               {
                  pDibSection->CreateIndirect(pBmpInfo);
                  int nScale = m_State.scaling;
                  AttachDIB(pDibSection);
                  m_State.scaling = nScale;
               }
               delete pBmpInfo;
            }
         }
      }
      else if (nFormat == META_FORMAT)                         // MetaFile Format
      {
         UINT nSize;
         ArchiveRead(ar, &nSize, sizeof(UINT));
         if (nSize>0)
         {
            BYTE * pBuff = new BYTE[nSize];
            if (pBuff)
            {
               ArchiveRead(ar, pBuff, nSize);
               HENHMETAFILE hMetaFile = ::SetEnhMetaFileBits(nSize, pBuff);
               delete[] pBuff;
               int nScale = m_State.scaling;
               AttachMetaFile(hMetaFile);
               m_State.scaling = nScale;
            }
         }
      }
      else if (nFormat == BITMAP_FILE)                         // aus einer separaten Bitmapdatei
      {
         long nLength;
         ArchiveRead(ar, &nLength, sizeof(long));
         TCHAR *pBuff = m_strFileName.GetBufferSetLength(nLength+1);
         ArchiveRead(ar, pBuff, nLength);
         pBuff[nLength] = 0;
         m_strFileName.ReleaseBuffer();
         CFile bmpFile;
         if (bmpFile.Open(m_strFileName, CFile::modeRead,NULL))// läßt sich die Datei öffnen
            m_strFileName = bmpFile.GetFilePath();             // den gesamten Pfad speichern
         else
         {
            int nIndex = m_strFileName.ReverseFind('\\');      // evtl. gespeicherten Pfad davor abschneiden,
            if (nIndex != -1)                                  // um im aktuellen Verzeichnins zu öffnen
            {
               m_strFileName = m_strFileName.Right(m_strFileName.GetLength()-nIndex-1);
               if (bmpFile.Open(m_strFileName, CFile::modeRead))
                  m_strFileName = bmpFile.GetFilePath();       // den gesamten Pfad speichern
            }
         }
         if (bmpFile.m_hFile != (HANDLE)0xffffffff)                    // ist die Datei vorhanden
         {
            CDibSection *pDibSection = new CDibSection;
            if (pDibSection)                                   // die Datei laden
            {
               pDibSection->LoadFromFile((HANDLE)bmpFile.m_hFile);
               int nScale = m_State.scaling;
               AttachDIB(pDibSection);
               m_State.scaling = nScale;
               m_bSaveInFile   = true;
            }
            bmpFile.Close();
         }
      }
      else ASSERT(nFormat == 0);
   }
}

UINT CPictureLabel::GetFormat()
{
   if      (m_pDibSection && m_pDibSection->GetBits()) return DIB_FORMAT;
   else if (m_hMetaFile)                               return META_FORMAT;
   else                    return 0;
}


void CPictureLabel::SetFileName(CString &fn)
{
   m_strFileName = fn;
}

CString & CPictureLabel::GetFileName()
{
   return m_strFileName;
}

int CPictureLabel::DeleteBWBmp(CLabel *pl, void *pParam)
{
   ASSERT_KINDOF(CPictureLabel, pl);
   CPictureLabel*ppl = (CPictureLabel*)pl;
   if (ppl->m_pDibSection) ppl->m_pDibSection->DestroySecondBmp();
   return 0;
}

CSize CPictureLabel::GetPrintSize()
{
   return m_szPrintSize;
}
