/********************************************************************/
/*                                                                  */
/*                    Programm PrintAscii                           */
/*                                                                  */
/*     Drukt ASCII Dateien, Textdateien, Zwischenablage             */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// PrintAsc.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "PrintAsc.h"
#include "PrintAscDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SECTION_KEY  "Ascii-Printer"
#define PRINTER_NAME "PrinterName"
#define TAB_LENGTH   "Tab-Length"
/////////////////////////////////////////////////////////////////////////////
// CPrintAscApp

BEGIN_MESSAGE_MAP(CPrintAscApp, CWinApp)
	//{{AFX_MSG_MAP(CPrintAscApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintAscApp Konstruktion

CPrintAscApp::CPrintAscApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CPrintAscApp-Objekt

CPrintAscApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPrintAscApp Initialisierung

BOOL CPrintAscApp::InitInstance()
{
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   SetRegistryKey(_T("ETS"));                                  // Firmenname

   CPrintAscDlg dlg;
   m_pMainWnd = &dlg;
   if (m_lpCmdLine[0] == '\0')
   {
      dlg.m_bPrintClipboard = ::IsClipboardFormatAvailable(CF_TEXT);
   }

   dlg.m_SelectPrinterName = GetProfileString(SECTION_KEY, PRINTER_NAME);
   dlg.m_nTabLength        = GetProfileInt(SECTION_KEY, TAB_LENGTH, 6);

   int nResponse = dlg.DoModal();
   if (nResponse == IDOK)
   {
      int i,c, nLastChar = 0;
      if (m_lpCmdLine[0] != '\0')
      {  
         char *name;
         name = dlg.GetPortName();

         FILE *fpout, *fp;
         fpout = fopen(name, "wt");
         if (!fpout) return false;

         for (i=1; __argv[i] != NULL; i++)
         {
            fp = fopen(__argv[i], "rt");
            if (!fp) break;

            while( (c = fgetc( fp )) != EOF )
            {
               switch(c)
               {
                  case 228: c = 132; break; // ä
                  case 246: c = 148; break; // ö
                  case 252: c = 129; break; // ü
                  case 196: c = 142; break; // Ä
                  case 214: c = 153; break; // Ö
                  case 220: c = 154; break; // Ü
                  case 223: c = 225; break; // ß
                  case (int)'\\':
                  if (dlg.m_bEtsHelpFormat)
                  {
                     c = fgetc(fp);
                     switch(c)
                     {
                        case 'n': 
                        fputc(13, fpout);
                        c = 10;
                        break;
                        case 't':
                        continue;
                     }
                  } break;
               }
               if (c == 9)
               {
                  for (int k=0; k<dlg.m_nTabLength; k++)
                     fputc(' ', fpout);
               }
               else fputc( c, fpout);
               nLastChar = c;
            }

            fputc(0x0c, fpout);
            fclose( fp );
         }
         fclose( fpout );
      }
      else if (dlg.m_bPrintClipboard && ::IsClipboardFormatAvailable(CF_TEXT) && ::OpenClipboard(NULL))
      {
         char * text= (char*)::GetClipboardData(CF_TEXT);
         if (text)
         {
            int j = 0;
            char *name;
            name = dlg.GetPortName();

            FILE *fpout;
            fpout = fopen(name, "wt");
            if (!fpout) return false;

            while( (c = text[j++]) != 0 )
            {
               switch(c)
               {
                  case -28: c = 132; break; // ä
                  case -10: c = 148; break; // ö
                  case  -4: c = 129; break; // ü
                  case -60: c = 142; break; // Ä
                  case -42: c = 153; break; // Ö
                  case -36: c = 154; break; // Ü
                  case -33: c = 225; break; // ß
                  case (int)'\\':
                  if (dlg.m_bEtsHelpFormat)
                  {
                     switch((int)text[j])
                     {
                        case 'n': 
                        fputc(13, fpout);
                        c = 10;
                        j++;
                        break;
                        case 't':
                        j++;
                        continue;
                     }
                  } break;
               }
               if (c == 9)
               {
                  for (int k=0; k<dlg.m_nTabLength; k++)
                     fputc(' ', fpout);
               }
               else fputc( c, fpout);
               nLastChar = c;
            }

            fputc(0x0c, fpout);
            fclose( fpout );
         }
         ::CloseClipboard();
      }
      WriteProfileString(SECTION_KEY, PRINTER_NAME, dlg.m_SelectPrinterName);
      WriteProfileInt(SECTION_KEY, TAB_LENGTH, dlg.m_nTabLength);
   }
   else if (nResponse == IDCANCEL)
   {
   }
   return FALSE;
}
