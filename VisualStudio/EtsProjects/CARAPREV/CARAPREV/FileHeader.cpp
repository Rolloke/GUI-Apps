// FileHeader.cpp: Implementierung der Klasse CFileHeader. für Windows !!!!
// Stand 15.09.98

//////////////////////////////////////////////////////////////////////
#include "FileHeader.h"
#ifdef _DEBUG
   #include <stdio.h>
#endif
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

unsigned long CFileHeader::gm_CRC_Table[256];
int           CFileHeader::gm_Init = 0;

CFileHeader::CFileHeader()
{
   Init(_T(""), NULL, 0);
}
CFileHeader::CFileHeader(const TCHAR *pszType, const TCHAR *pszYear, unsigned long nVersion)
{
   Init(pszType, pszYear, nVersion);
}

void CFileHeader::Init(const TCHAR *pszType, const TCHAR *pszYear, unsigned long nVersion)
{
   char * pThis = (char *) this;
   for (int i=0;i<sizeof(CFileHeader);i++) *(pThis++) = 0;

   strcpy(m_pszETS, "ETS");
   strcpy(m_pszCopyright, "(C) 1998 ELAC Technische Software GmbH Kiel");
   SetYear(pszYear);
   SetType(pszType);
   m_nVersion        = nVersion;
   m_nCRC            = 0xffffffff;
   m_nDatalength     = 0;

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

void CFileHeader::SetType(const TCHAR *pszType)
{
   if (pszType)
   {
      int len = l_tcsclen(pszType);
      if ((len > 0) && (len < 4))
      {
         lstrcpy(m_pszType, pszType);
      }
   }
}
void CFileHeader::SetComment(const TCHAR *pszComment)
{
   if (!pszComment) return;
   int len = l_tcsclen(pszComment);
   if ((len > 0) || (len < 64))
      lstrcpy(m_pszComment, pszComment);
}

int CFileHeader::IsType(const TCHAR * pszType) const
{
   return ((lstrcmp(m_pszType, pszType) == 0) ? 1 : 0);
}

void CFileHeader::CalcChecksum(const void *buff, long length)
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
   TCHAR fname[16];
   wsprintf(fname, _T("%s.dbg"), m_pszType);
   if (!buff) DeleteFile(fname);
   FILE *fp = fopen(fname, _T("a+t"));
   if (fp)
   {
      fprintf(fp, _T("%08d : %08x\n"), m_nDatalength, m_nCRC);
      fclose(fp);
   }
#endif
}

void CFileHeader::SetYear(const TCHAR *pszYear)
{
   if (pszYear)
   {
      int len = l_tcsclen(pszYear);
      if (len == 4)
      {
         for (int i=4; i<8; i++) m_pszCopyright[i] = pszYear[i-4];
      }
   }
}