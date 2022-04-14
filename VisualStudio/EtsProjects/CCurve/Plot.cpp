// Plot.cpp: Implementierung der Klasse CPlot.
// 31.07.98 Tho
//////////////////////////////////////////////////////////////////////
#include <math.h>

#include "Plot.h"
#include "Curve.h"
#include "FileHeader.h"
#include "Debug.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CPlot::CPlot()
{
   memset(this, 0, sizeof(CPlot));
}

CPlot::~CPlot()
{
   if (m_pCurves)
   {
       delete[] m_pCurves;
   }
   if (m_pszHeading)
   {
      delete[] m_pszHeading;
   }
}

bool CPlot::SetSize(int n)
{
   if (n <= 0) return false;
   if (m_pCurves)
   {
       delete[] m_pCurves;
   }
   m_pCurves = new CCurve[n];
   if (!m_pCurves) return false;
   m_nNumCurves = n;
   return true;
}

void CPlot::Write(HANDLE hfile)
{
   DWORD dwactbytes=sizeof(int);
   int   i;
   auto size = strlen("");
   if (hfile==INVALID_HANDLE_VALUE)  return;
   // Überschrift
   if (m_pszHeading) size = strlen(m_pszHeading);
   else          size = 0;
   WriteFile(hfile, &size, sizeof(int), &dwactbytes, NULL);
   if (size) WriteFile(hfile, m_pszHeading, static_cast<DWORD>(size), &dwactbytes, NULL);
   // Zoombereiche
   int count = 1;
   WriteFile(hfile, &count,   sizeof(int),   &dwactbytes, NULL);
   WriteFile(hfile, &m_DefaultRange, sizeof(RANGE), &dwactbytes, NULL);
   // Kurven
   WriteFile(hfile, &m_nNumCurves,   sizeof(int),   &dwactbytes, NULL);
   for (i=0; i<m_nNumCurves; i++)
      m_pCurves[i].Write(hfile);
}

bool CPlot::Read(HANDLE hfile)
{
   DWORD dwactbytes = 0;
   bool  bRvalue = true;
   int   i, count, size;
   if (hfile==INVALID_HANDLE_VALUE) return false;
   // Überschrift
   if (!ReadFile(hfile, &size, sizeof(int), &dwactbytes, NULL)) return false;
   if (size)
   {
      if (m_pszHeading) delete[] m_pszHeading;
      m_pszHeading = new char[size+1];
      if (!ReadFile(hfile, m_pszHeading, size, &dwactbytes, NULL)) return false;
      m_pszHeading[size] = 0;
   }
   // Zoombereiche
   ReadFile(hfile, &count,   sizeof(int),   &dwactbytes, NULL);
   ReadFile(hfile, &m_DefaultRange, sizeof(RANGE), &dwactbytes, NULL);
   if ((count > 1) && (SetFilePointer(hfile, (count-1)*sizeof(RANGE), NULL, FILE_CURRENT)==0xFFFFFFFF)) return false;

   // Kurven
   ReadFile(hfile, &count,   sizeof(int),   &dwactbytes, NULL);
   SetSize(count);
   for (i=0; i<m_nNumCurves; i++)
      bRvalue &= m_pCurves[i].Read(hfile);

   return bRvalue;
}

void CPlot::GetChecksum(CFileHeader *pFH)
{
   if (!pFH) return;
   int   i;
   auto size = strlen("");

   if (m_pszHeading)
   {
      size = strlen(m_pszHeading);
      pFH->CalcChecksum(&size, sizeof(int));
      pFH->CalcChecksum(m_pszHeading, static_cast<long>(size));
   }
   else pFH->CalcChecksum(&size, sizeof(int));

   pFH->CalcChecksum(&m_DefaultRange, sizeof(RANGE));
   pFH->CalcChecksum(&m_nNumCurves,   sizeof(int));
   for (i=0; i<m_nNumCurves; i++)
      m_pCurves[i].GetChecksum(pFH);
}

void CPlot::DeterminRange()
{
   int      i;
   double   temp;
   m_DefaultRange.x2 = m_pCurves[0].GetXMax();
   m_DefaultRange.x1 = m_pCurves[0].GetXMin();
   m_DefaultRange.y2 = m_pCurves[0].GetYMax();
   m_DefaultRange.y1 = m_pCurves[0].GetYMin();

   for (i=1; i<m_nNumCurves; i++)
   {
      if ((temp=m_pCurves[i].GetXMax()) > m_DefaultRange.x2) m_DefaultRange.x2 = temp;
      if ((temp=m_pCurves[i].GetXMin()) < m_DefaultRange.x1) m_DefaultRange.x1 = temp;
      if ((temp=m_pCurves[i].GetYMax()) > m_DefaultRange.y2) m_DefaultRange.y2 = temp;
      if ((temp=m_pCurves[i].GetYMin()) < m_DefaultRange.y1) m_DefaultRange.y1 = temp;
   }
}

void CPlot::SetXDivision(int nd) {for (int i=0; i<m_nNumCurves; i++) m_pCurves[i].SetXDivision(nd);}
void CPlot::SetYDivision(int nd) {for (int i=0; i<m_nNumCurves; i++) m_pCurves[i].SetYDivision(nd);}
void CPlot::SetXNumMode(int nm)  {for (int i=0; i<m_nNumCurves; i++) m_pCurves[i].SetXNumMode(nm);}
void CPlot::SetYNumMode(int nm)  {for (int i=0; i<m_nNumCurves; i++) m_pCurves[i].SetYNumMode(nm);}
void CPlot::SetXUnit(const char *psz)  {for (int i=0; i<m_nNumCurves; i++) m_pCurves[i].SetXUnit(psz);}
void CPlot::SetYUnit(const char *psz)  {for (int i=0; i<m_nNumCurves; i++) m_pCurves[i].SetYUnit(psz);}

void CPlot::SetDefaultRange(double x1, double x2, double y1, double y2)
{
   RANGE range = {x1, x2, y1, y2};
   SetDefaultRange(range);
}

void CPlot::SetHeading(const char *s)
{
   if (m_pszHeading) delete[] m_pszHeading;
   m_pszHeading = NULL;
   if (s)
   {
      m_pszHeading = new char[strlen(s)+1];
      strcpy(m_pszHeading, s);
   }
}

CCurve *CPlot::GetCurve(int n)
{
   if ((n >= 0) && (n < m_nNumCurves))
      return &m_pCurves[n];

   return NULL;
}
 
bool Call2DView(const char *pszProgrPath, int nPlots, CPlot *pPlots, bool bDeterminRange, bool bEdit, HWND hWnd, int nMsgID, int nParam, char *pszFileName, PROCESS_INFORMATION* pPinfo)
{
   int         i;
   char        szTempPath[MAX_PATH*2];
   char        szCaraViewFileName[MAX_PATH*2];
   HANDLE      hFile;
   STARTUPINFO startupinfo;
   CFileHeader FileHeader("PLT", "2000", 110);

   ASSERT(pPlots != NULL);
   if (bEdit)
   {
      ASSERT(hWnd       != NULL);
      ASSERT(pszFileName!= NULL);
      ASSERT(nMsgID     != 0);
      ASSERT(nParam     != 0);
      FileHeader.SetType("EDT");
   }

   if (bDeterminRange)                                         // Automatisches ermitteln der Bereiche
   {
      for (i=0; i<nPlots; i++)                                 // für jeden Plot einzeln
      {
         pPlots[i].DeterminRange();                            // ermitteln
         RANGE range = pPlots[i].GetDefaultRange();
         if (fabs(range.x2 - range.x1) < 1e-13)                // und prüfen
         {
            range.x1 -= 0.001;
            range.x2 += 0.001;
         }
         if (fabs(range.y2 - range.y1) < 1e-13)
         {
            range.y1 -= 0.001;
            range.y2 += 0.001;
         }
         pPlots[i].SetDefaultRange(range);                     // erneutes setzen
      }
   }

   FileHeader.CalcChecksum(&nPlots, sizeof(int));              // CRC-Checksumme bilden
   for (i=0; i<nPlots; i++)
   {
      pPlots[i].GetChecksum(&FileHeader);                      // für jeden Plot
   }

   ZeroMemory(&startupinfo, sizeof(STARTUPINFO));              // StartupInfo Struktur füllen
   startupinfo.cb          = sizeof(STARTUPINFO);
   startupinfo.wShowWindow = SW_SHOW;

   GetTempPath( MAX_PATH*2, szTempPath );                      // Pfad der Übergabedatei basteln
   GetTempFileName(szTempPath, "C2D", 0, szCaraViewFileName);
   if (pszFileName) strcpy(pszFileName, szCaraViewFileName);   // Pfad kopieren
   hFile = CreateFile( szCaraViewFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);

   if (pszProgrPath == NULL) pszProgrPath = "";                // Ist der Programmpfad nicht belegt : Leerstring

   if( hFile != INVALID_HANDLE_VALUE )                         // Datei geöffnet
   {
      DWORD dwDummy;
      WriteFile( hFile, (LPCVOID) &FileHeader, sizeof(CFileHeader), (LPDWORD) &dwDummy, NULL );
      WriteFile( hFile, (LPCVOID) &nPlots, sizeof(int), (LPDWORD) &dwDummy, NULL );
      for (i=0; i<nPlots; i++)                                 // Plots Schreiben
      {
         pPlots[i].Write(hFile);
      }
      CloseHandle(hFile);
      if ((hWnd!=NULL) && (nMsgID != 0))
      {
         wsprintf(szTempPath, "%sETS2DV.EXE \"%s\" :H%d :M%d :W%d", pszProgrPath, szCaraViewFileName, (long)hWnd, nMsgID, nParam);
      }
      else
      {
         wsprintf(szTempPath, "%sETS2DV.EXE \"%s\"", pszProgrPath, szCaraViewFileName);
      }
      if (CreateProcess(NULL, szTempPath, NULL, NULL, false, 0, NULL, NULL, &startupinfo, pPinfo))
      {
         return true;
      }
   }
   return false;
}

CPlot *ReadPlots(const char *pszFileName, int &nPlots, bool &bCRC_OK)
{
   CPlot       *pPlots = NULL;
   CFileHeader  FileHeader;
   HANDLE       hFile;
   DWORD        dwCRC;

   bCRC_OK = false;
   hFile   = CreateFile(pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);

   if( hFile != INVALID_HANDLE_VALUE )                         // Datei geöffnet
   {
      DWORD   dwDummy;
      int     i;
      ReadFile(hFile, &FileHeader, sizeof(CFileHeader), (LPDWORD) &dwDummy, NULL );
      dwCRC = FileHeader.GetChecksum();
      FileHeader.CalcChecksum();
      ReadFile(hFile, &nPlots, sizeof(int), (LPDWORD) &dwDummy, NULL);
      if (nPlots > 0)
      {
         pPlots = new CPlot[nPlots];
         if (pPlots)
         {
            FileHeader.CalcChecksum(&nPlots, sizeof(int));
            for (i=0; i<nPlots; i++)                           // Plots Schreiben
            {
               pPlots[i].Read(hFile);
               pPlots[i].GetChecksum(&FileHeader);
            }
         }
         bCRC_OK = (FileHeader.IsValid(dwCRC) != 0) ? true : false;
      }
   }
   return pPlots;
}

