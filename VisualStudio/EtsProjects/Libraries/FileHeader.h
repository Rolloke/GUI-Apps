/*******************************************************************************
                                 CFileHeader
  Zweck:
   Klasse zur Validierung von Dateien und Dateiabschnitten. Zur Validierung der
   Abschnitte wird eine Kennung und eine Dateiversionsnummer eingetragen.
   Über den gespeicherten Dateiabschnitt wird zusätzlich eine Checksumme gebildet.

   Validiert werden kann:
   - Die Checksumme (CRC)
   - Der Dateityp
   - Die Dateiversion
   - Ein Dateikommentar

   Datenbeschreibung:

      char           m_pszETS[4];         Kennung der Firma "ETS"          (char[4])
      char           m_pszType[4];        Dateiabschnittstyp z.B. "CAD"    (char[4])
      unsigned long  m_nVersion;          Dateiversion z.B. 201 für 2.01   (ULONG)
      unsigned long  m_nCRC;              Checksumme (CRC)                 (ULONG)
      char           m_pszCopyright[44];  Copyright der Firma              (char[44])
                                          "(C) 2001 ELAC Technische Software GmbH Kiel"
      char           m_pszComment[64];    Kommentar                        (char[64])
      unsigned long  m_nDatalength;       Länge des Dateiabschnitts        (ULONG)

   Funktionsbeschreibung:

   - Konstruktoren: 
      CFileHeader(); Standardkonstruktor
      CFileHeader(const char *pszType, const char *pszYear, unsigned long nVersion);
      Argumente :
         pszType : Typ des Dateiabschnitts z.B.: "NET", "CAD", "BOX"    (char[4])
         pszYear : Aktuelles Jahr im Copyrighttext z.B.: "2001"         (char[5])
         nVersion: Dateiversion                                         (ULONG)

   - Funktionen setzen der Daten der Klasse
      void SetType(const char*pszType);
      Argumente :
         pszType : Typ des Dateiabschnitts z.B.: "NET", "CAD", "BOX"    (char[4])

      void SetComment(const char *pszComment);
      Argumente :
         pszComment: Kommentar für den Dateiabschnitt [optional]        (char[64])

      void SetVersion(unsigned long nVersion)
      Argumente :
         nVersion: Dateiversion                                         (ULONG)

      void SetYear(const char *);
      Argumente :
         pszYear : Aktuelles Jahr im Copyrighttext z.B.: "2001"         (char[5])

  - Berechnung der Checksumme und der Datenlänge
      void CalcChecksum(const void *buff = NULL, long length=0);
      Argumente :
         buff  : Zeiger auf den Puffer der die Daten enthält            (const void *)
         lenght: Länge des Puffers                                      (long)
      Am Anfang eines Datenabschnitts wird die Checksumme und die Datenlänge durch
      Aufruf der Funktion CalcChecksum() ohne Argumente initialisiert.

   - Zugriff auf die Daten der Klasse
   bool          IsFileHeader()const;           // Prüft den "ETS" String
   int           IsValid(unsigned long ncrc);   // Vergleicht die Checksumme
   int           IsType(const char *) const;    // Vergleicht den Typ
   const char *  GetComment();                  // Liefert den Kommentar
   unsigned long GetVersion();                  // Liefert die Dateiversion
   unsigned long GetChecksum();                 // Liefert die Checksumme
   unsigned long GetLength();                   // Liefert die Datenlänge

  Copyright:
   (C) 2001 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 23.07.2001
                                                   programmed by Rolf Ehlers
*******************************************************************************/

#if !defined(AFX_FILEHEADER_H__DD567502_3A3A_11D1_9DB9_B051418EA04A__INCLUDED_)
#define AFX_FILEHEADER_H__DD567502_3A3A_11D1_9DB9_B051418EA04A__INCLUDED_

#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#ifndef NULL
   #define NULL 0l
#endif

class CFileHeader  
{
public:
   CFileHeader();
   CFileHeader(const char *, const char *, unsigned long);

   int           IsValid(unsigned long ncrc) const {return (ncrc == m_nCRC);};
   void          SetType(const char*);
   void          SetComment(const char *);
   void          SetVersion(unsigned long nVersion) {m_nVersion = nVersion;};
   void          SetYear(const char *);
   void          CalcChecksum(const void *buff = NULL, long length=0);

   bool          IsFileHeader()const;
   int           IsType(const char *) const;
   const char *  GetComment()  const {return m_pszComment;};
   unsigned long GetVersion()  const {return m_nVersion;};
   unsigned long GetChecksum() const {return m_nCRC;}
   unsigned long GetLength()   const {return m_nDatalength;};

private:
   void          Init(const char *, const char *, unsigned long);

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
