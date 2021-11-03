// FileHeader.h: Schnittstelle für die Klasse CFileHeader. für Windows !!!
// Stand 15.09.98
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEHEADER_H__DD567502_3A3A_11D1_9DB9_B051418EA04A__INCLUDED_)
#define AFX_FILEHEADER_H__DD567502_3A3A_11D1_9DB9_B051418EA04A__INCLUDED_

#ifndef _WINDOWS_
   #ifndef STRICT
      #define STRICT
   #endif
   #include <windows.h>
#endif

#ifndef NULL
   #define NULL 0l
#endif

class AFX_EXT_CLASS CFileHeader  
{
public:
   CFileHeader();
   CFileHeader(TCHAR *, TCHAR *, unsigned long);
   void          Init(TCHAR *, TCHAR *, unsigned long);

   int           IsValid(unsigned long ncrc) {return (ncrc == m_nCRC);};
   void          SetType(TCHAR*);
   int           IsFileHeader();
   int           IsType(TCHAR *);
   void          SetComment(TCHAR *);
   void          SetVersion(unsigned long nVersion) {m_nVersion = nVersion;};
   void          SetYear(TCHAR *);
   void          CalcChecksum(const void *buff = NULL, long length=0);

   const TCHAR *  GetComment()  const {return m_pszComment;};
   unsigned long GetVersion()  const {return m_nVersion;};
   unsigned long GetChecksum() const {return m_nCRC;}
   void          SetChecksum(unsigned long nCRC) {m_nCRC = nCRC;};
   unsigned long GetLength()   const {return m_nDatalength;};
   void          SetLength(unsigned long nLen) {m_nDatalength = nLen;};

private:

   TCHAR                m_pszETS[4];
   TCHAR                m_pszType[4];
   unsigned long        m_nVersion;
   unsigned long        m_nCRC;
   TCHAR                m_pszCopyright[44];
   TCHAR                m_pszComment[64];
   unsigned long        m_nDatalength;

   static unsigned long gm_CRC_Table[256];
   static int           gm_Init;
};

#endif // !defined(AFX_FILEHEADER_H__DD567502_3A3A_11D1_9DB9_B051418EA04A__INCLUDED_)
