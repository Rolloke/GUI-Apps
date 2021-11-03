// FileHeader.cpp: Implementierung der Klasse CFileHeader. für Windows !!!!
// Stand 15.09.98

//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PreviewFileHeader.h"
#include "CARAPREV.h"
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

unsigned long CFileHeader::gm_CRC_Table[256];
int           CFileHeader::gm_Init = 0;

CFileHeader::CFileHeader()
{
   Init(_T(""), NULL, 0);
}
CFileHeader::CFileHeader(TCHAR *pszType, TCHAR *pszYear, unsigned long nVersion)
{
   Init(pszType, pszYear, nVersion);
}

void CFileHeader::Init(TCHAR *pszType, TCHAR *pszYear, unsigned long nVersion)
{
   ZeroMemory(m_pszCopyright, 44);
   ZeroMemory(m_pszComment, 64);

   lstrcpy(m_pszETS, _T("ETS"));
   lstrcpy(m_pszCopyright, _T("(C) 1998 ELAC Technische Software GmbH Kiel"));
   SetYear(pszYear);
   SetType(pszType);
   m_nVersion    = nVersion;
   m_nCRC        = 0xffffffff;
   m_nDatalength = 0;

   if (!CFileHeader::gm_Init)
   {
      unsigned long shf;   /* crc shift register                        */
      int i;               /* counter for all possible eight bit values */
      int k;               /* counter for bit being shifted into crc    */
      for (i=0; i<256; i++)
      {
         shf = i;
         for (k=0; k<8; k++)
         {
            shf = (shf & 1) ? ((shf >> 1) ^ 0xedb88320L) : (shf >> 1);
         }

         CFileHeader::gm_CRC_Table[i] = shf;
      }
      CFileHeader::gm_Init = 1;
   }
}

void CFileHeader::SetType(TCHAR *pszType)
{
   if (pszType)
   {
      lstrcpyn(m_pszType, pszType, 4);
   }
}
void CFileHeader::SetComment(TCHAR *pszComment)
{
   if (pszComment) lstrcpyn(m_pszComment, pszComment, 64);
   else            memset(m_pszComment, 64, sizeof(TCHAR));
}

int CFileHeader::IsType(TCHAR * pszType)
{
   return ((lstrcmp(m_pszType, pszType) == 0) ? 1 : 0);
}

void CFileHeader::CalcChecksum( const void *buff, long length)
{
   long i;

   if (!buff)
   {
      m_nCRC        = 0xffffffff;
      m_nDatalength = 0;
   }
   unsigned char *buffer = (unsigned char *) buff;

   /* Update the crc with each byte */
   for (i=0; i<length; i++)
   {
      m_nCRC = CFileHeader::gm_CRC_Table[buffer[i] ^ ((unsigned char) m_nCRC)] ^ (m_nCRC >> 8);
   }
   m_nDatalength += length;

#ifdef _DEBUG
   if (g_bDoReport)
   {
      char fname[16];
      if (m_pszType[0] != 0)
         sprintf_s(fname, 16, "%ws.dbg", m_pszType);
      else
         sprintf_s(fname, 16, "fileheader.dbg");
      if (!buff) DeleteFileA(fname);
      FILE *fp = fopen(fname, "a+w");
      if (fp)
      {
         fprintf(fp, "%08d : %08x\n", m_nDatalength, m_nCRC);
         fclose(fp);
      }
   }
#endif
}

void CFileHeader::SetYear(TCHAR *pszYear)
{
   if (pszYear)
   {
      int i, j;
      for (i=4, j=0; i<=8; i++,j++) m_pszCopyright[i] = pszYear[j];
   }
}

int CFileHeader::IsFileHeader()
{
   return ((lstrcmp(m_pszETS, _T("ETS")) == 0) ? 1 : 0);
}
