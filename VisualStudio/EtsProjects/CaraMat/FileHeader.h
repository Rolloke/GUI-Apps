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

class CFileHeader  
{
public:
   CFileHeader();
   CFileHeader(char *, char *, unsigned long);

   int           IsValid(unsigned long ncrc) {return (ncrc == m_nCRC);};
   void          SetType(char*);
   int           IsType(char *);
   void          SetComment(char *);
   void          SetVersion(unsigned long nVersion) {m_nVersion = nVersion;};
   void          SetYear(char *);
   void          CalcChecksum(const void *buff = NULL, long length=0);

   const char *  GetComment()  const {return m_pszComment;};
   unsigned long GetVersion()  const {return m_nVersion;};
   unsigned long GetChecksum() const {return m_nCRC;}
   unsigned long GetLength()   const {return m_nDatalength;};

private:
   void          Init(char *, char *, unsigned long);

   char                 m_pszETS[4];
   char                 m_pszType[4];
   unsigned long        m_nVersion;
   unsigned long        m_nCRC;
   char                 m_pszCopyright[44];
   char                 m_pszComment[64];
   unsigned long        m_nDatalength;

   static unsigned long gm_CRC_Table[256];
   static int           gm_Init;
};

#endif // !defined(AFX_FILEHEADER_H__DD567502_3A3A_11D1_9DB9_B051418EA04A__INCLUDED_)
