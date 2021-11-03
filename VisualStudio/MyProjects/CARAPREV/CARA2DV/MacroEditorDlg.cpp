// MacroEditorDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "MacroEditorDlg.h"
#include "..\stdafx.h"
#include "..\Resource.h"
#include "..\TreeDlg.h"
#include "..\TextEdit.h"
#include ".\macroeditordlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FIRST_LEVEL   0
#define SEPARATOR_KEY 1
#define READ_KEY      2
#define LOOP_KEY      3
#define SWITCH_KEY    4
#define OTHER_KEY     5

#define FIRST_SEPARATOR_KEY   2
#define LAST_SEPARATOR_KEY   22
#define FIRST_READ_KEY       24
#define LAST_READ_KEY        33
#define FIRST_LOOP_KEY       34
#define LAST_LOOP_KEY        38
#define FIRST_SWITCH_KEY     39
#define LAST_SWITCH_KEY      51
#define FIRST_OTHER_KEY      52
#define LAST_OTHER_KEY       56

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld MacroEditorDlg 
const MacroEditorDlg::KeyWords MacroEditorDlg::gm_Keys[] =
{
   { " ",             SPACE},   // Trennzeichen (0)
   { "//",            COMMENT}, // Alles danach ist Kommentar (cpp konform)
   { ",",             2  },
   { ";",             3  },
   { ":",             4  },
   { "-",             5  },
   { "_",             6  },
   { "/",             7  },
   { "(",             8  },
   { ")",             9  },
   { "!",             10 },
   { "§",             11 },
   { "$",             12 },
   { "%",             13 },
   { "&",             14 },
   { "{",             15 },
   { "}",             16 },
   { "[",             17 },
   { "]",             18 },
   { "?",             19 },
   { "#",             20 },
   { "\"",            TUEDELCHEN   }, // 21
   { "\n",            NEWMAKROLINE }, // Zeilenvorschub
   { "NO_OF_CURVES",  NO_OF_CURVES }, // Anzahl der Kurven folgt als Zahl
   { "DESCRIPTION",   DESCRIPTION  }, // Kurvenbeschreibung, Index der Kurve folgt als Zahl
   { "UNITX",         UNITX        }, // X-Kurveneinheit, Index der Kurve folgt als Zahl
   { "UNITY",         UNITY        }, // Y-Kurveneinheit, Index der Kurve folgt als Zahl
   { "UNITLOCUS",     UNITLOCUS    }, // Kurveneinheit für Winkel der Ortskurve
   { "VALUEX",        VALUEX       }, // X-Kurvenwert, Index der Kurve folgt als Zahl
   { "VALUEY",        VALUEY       }, // Y-Kurvenwert, Index der Kurve folgt als Zahl
   { "VALUELOCUS",    VALUELOCUS   }, // Winkel der Ortskurve, Index der Kurve folgt als Zahl
   { "START_VALUE",   START_VALUE  }, // Startwert für X-Achse bzw. Winkel der Ortskurve, Index der Kurve folgt als Zahl
   { "STEP_VALUE",    STEP_VALUE   }, // Steigung für X-Achse bzw. Winkel der Ortskurve, Index der Kurve folgt als Zahl
   { "OFFSETY",       OFFSETY      }, // Offset für alle Y-Werte, Index der Kurve folgt als Zahl
   { "REPEAT",        REPEAT       }, // Schleifenanfang
   { "UNTIL",         UNTIL        }, // Schleifenende
   { "NO_OF_VALUES",  NO_OF_VALUES }, // Anzahl der Werte, Index der Kurve folgt als Zahl
   { "NO_OF_LOOPS",   NO_OF_LOOPS  }, // Schleifenende
   { "END",           ENDLOOP      }, // Makroende
   { "DECIMAL_POINT", DECIMAL_POINT}, // Dezimalpunkt Auswertung
   { "DECIMAL_COMMA", DECIMAL_COMMA}, // Dezimalkomma Auswertung
   { "XLOGARITHMIC",  XLOGARITHMIC }, // Logarithmische Kurve
   { "XPOLAR",        XPOLAR       }, // X-Achse als Polarachse
   { "YLOGARITHMIC",  YLOGARITHMIC }, // Logarithmische Kurve
   { "ANGLE_GON",     ANGLE_GON,   }, // Winkel in Gon
   { "ANGLE_RAD",     ANGLE_RAD,   }, // Winkel in RAD
   { "LOCUS_LINEAR",  LOCUS_LINEAR }, // Ortskurve linear
   { "LOCUS_LOG",     LOCUS_LOG    }, // Ortskurve logarithmisch
   { "FORMAT_DOUBLE", FORMAT_DOUBLE}, // Speicherformat double
   { "FORMAT_FLOAT",  FORMAT_FLOAT }, // Speicherformat float
   { "FORMAT_SHORT",  FORMAT_SHORT }, // Speicherformat short
   { "SINGLE_FORMAT", SINGLE_FORMAT}, // Speicherformat nur Y-Werte
   { "INDEX",         INDEX        }, // Increment-Zeichen
   { "NEWLINE",       NEWLINE      }, // Leerzeile lesen
   { "FIND",          FIND_STRING  }, // Suchen eines Ausdrucks
   { "STEP_BY",       STEP_BY      }, // Springt eine Anzahl von Zeichen in der Zeile weiter
   { "=",             EQUAL        }, // Gleichheitszeichen
   { "\t",            TABULATOR    }, // Tabulator
   {NULL,             0            }
};

long MacroEditorDlg::gm_lEditCallBack = 0;

MacroEditorDlg::MacroEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MacroEditorDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(MacroEditorDlg)
   //}}AFX_DATA_INIT
 
   m_nRun = DEBUG_TM_BREAK;
   m_pTexEdit = NULL;
   
   LoadFromRegistry();

   if (m_Descriptions.GetSize() != m_Makros.GetSize())
   {
      m_Makros.RemoveAll();
      m_Descriptions.RemoveAll();
   }
}

MacroEditorDlg::~MacroEditorDlg()
{
   CWinApp* pApp  = AfxGetApp();
   HKEY hAppKey = theApp.GetAppRegistryKey();
   if (hAppKey)
   {
      RegDeleteKey(hAppKey, REGKEY_CURVE_TEXT_MACROS);
      RegCloseKey(hAppKey);
   }

   SaveToRegistry();

   m_Makros.RemoveAll();
   m_Descriptions.RemoveAll();
   if (m_pTexEdit)
   {
      if (m_pTexEdit->m_hWnd)
      {
         m_pTexEdit->DestroyWindow();
      }
      delete m_pTexEdit;
      m_pTexEdit = NULL;
   }
   m_nRun = 0;
}

void MacroEditorDlg::SaveToRegistry()
{
   CString  strDescription, strMakro;
   CString  strKey;
   int i, nNumMakros = min(m_Makros.GetSize(),m_Descriptions.GetSize());
   for (i=0; i<nNumMakros; i++)
   {
      strKey.Format("DESCRIPTION%d", i);
      strDescription = m_Descriptions[i];
      if (!strDescription.IsEmpty())
         theApp.WriteProfileString(REGKEY_CURVE_TEXT_MACROS, strKey, strDescription);

      strKey.Format("MACRO%d", i);
      strMakro = m_Makros[i];
      if (!strMakro.IsEmpty())
      {
         if (theApp.m_pszRegistryKey == NULL)
         {
            strMakro.Replace("\r\n", "&crnl;");
         }
         theApp.WriteProfileString(REGKEY_CURVE_TEXT_MACROS, strKey, strMakro);
      }
   }
}

void MacroEditorDlg::LoadFromRegistry()
{
   CWinApp* pApp  = AfxGetApp();
   CString  strDescription, strMakro;
   CString  strKey;
   for (int i=0; ; i++)
   {
      strKey.Format("DESCRIPTION%d", i);
      strDescription = theApp.GetProfileString(REGKEY_CURVE_TEXT_MACROS, strKey);
      if (!strDescription.IsEmpty())
      {
         m_Descriptions.Add(LPCTSTR(strDescription));
      }
      else break;

      strKey.Format("MACRO%d", i);
      strMakro = theApp.GetProfileString(REGKEY_CURVE_TEXT_MACROS, strKey);
      if (!strMakro.IsEmpty())
      {
         if (theApp.m_pszRegistryKey == NULL)
         {
            strMakro.Replace("&crnl;", "\r\n");
         }
         m_Makros.Add(LPCTSTR(strMakro));
      }
      else break;
   }
}

void MacroEditorDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MacroEditorDlg)
   DDX_Control(pDX, IDC_MACRO_LIST, m_MakroList);
   DDX_Control(pDX, IDC_MACRO_EDIT, m_MakroEdit);
   DDX_Radio(pDX, IDC_RD_RUN, (int&) m_nRun);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MacroEditorDlg, CDialog)
   //{{AFX_MSG_MAP(MacroEditorDlg)
   ON_BN_CLICKED(IDC_MACRO_DELETE, OnDeleteMacro)
   ON_BN_CLICKED(IDC_MACRO_NEW, OnNewMacro)
   ON_CBN_SELCHANGE(IDC_MACRO_LIST, OnSelchangeMakroList)
   ON_EN_CHANGE(IDC_MACRO_EDIT, OnChangeMakroEdit)
   ON_CBN_EDITCHANGE(IDC_MACRO_LIST, OnEditchangeMakroList)
   ON_BN_CLICKED(IDC_MACRO_SAVE, OnSaveMakro)
   ON_MESSAGE(WM_HELP, OnHelp)
   ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpButton)
   ON_BN_CLICKED(IDC_STOP, OnCancel)
   ON_BN_CLICKED(IDC_MACRO_DEBUG, OnBnClickedMacroDebug)
   ON_MESSAGE(WM_USER, OnUser)
   ON_BN_CLICKED(IDC_RD_RUN, OnBnClickedRdStep)
   ON_BN_CLICKED(IDC_RD_STEP, OnBnClickedRdStep)
   ON_BN_CLICKED(IDC_RD_BREAK, OnBnClickedRdStep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten MacroEditorDlg 
void MacroEditorDlg::OnDeleteMacro() 
{
   if (AfxMessageBox(IDS_DELETE_MACRO, MB_YESNO|MB_ICONQUESTION)==IDYES)
   {
      int nNum = m_MakroList.GetCount();
      if (nNum != CB_ERR)
      {
         if (m_MakroList.DeleteString(m_nEditNo) != CB_ERR)
         {
            m_Makros.RemoveAt(m_nEditNo);
            if (nNum > 1)
            {
               m_MakroList.SetCurSel(0);
               CString string;
               m_MakroList.GetLBText(0, string);
               m_MakroList.SetWindowText(string);
               LoadMakroText(0);
            }
         }
      }
   }
}

void MacroEditorDlg::OnNewMacro() 
{
   int nNumMakros = m_MakroList.GetCount();
   SaveMakroText();
   CString strDescription;
   CString strFormat;
   strFormat.LoadString(IDS_FORMAT_NEWMAKRONAME);
   strDescription.Format(strFormat, nNumMakros);
   m_MakroList.AddString(strDescription);
   m_MakroList.SetWindowText(strDescription);
   m_nEditNo = nNumMakros;
   m_Makros.Add("NO_OF_CURVES 1");
   m_MakroEdit.SetWindowText(m_Makros[m_nEditNo]);
}

void MacroEditorDlg::OnSelchangeMakroList()
{
   int      nEditNo;
   nEditNo = m_MakroList.GetCurSel();                          // Index ermitteln
   if (nEditNo != CB_ERR)                                      // ist der Index gültig ?
   {
      if (nEditNo != m_nEditNo)                                // wurde ein anderer Index gewählt ?
      {
         if (SaveMakroText())
         {
            LoadMakroText(nEditNo);
         }
         else if (m_nEditNo  != CB_ERR)
         {
            CString string;
            m_MakroList.SetCurSel(m_nEditNo);
            m_MakroList.GetLBText(m_nEditNo, string);
            m_MakroList.SetWindowText(string);
         }
      }
   }
}

void MacroEditorDlg::OnSaveMakro()
{
   SaveMakroText(false);
}

bool MacroEditorDlg::SaveMakroText(bool bRequest)
{
   UINT rValue = IDYES;
   if (m_bChanged && bRequest) rValue = AfxMessageBox(IDS_SAVE_CHANGES, MB_YESNO|MB_ICONQUESTION);
   if (m_bChanged && (rValue==IDYES))                          // Text speichern ?
   {
      if ((m_nEditNo>=0) && (m_nEditNo < m_Makros.GetSize()))  // Stimmt der Index-Bereich ?
      {
         CString string;
         SControl pControl[MAX_CONTROL];
         m_MakroEdit.GetWindowText(string);                    // Textobjekt aus Editfeld holen
         long pos = CheckSyntax(string, pControl, true);
         for (int i=0; i<MAX_CONTROL; i++)
         {
            if (pControl[i].ptr) delete pControl[i].ptr;
         }
         if (pos)                                              // Ist die Syntax falsch
         {
            m_MakroEdit.SetSel(LOWORD(pos), HIWORD(pos), true);
            m_MakroEdit.SetFocus();
            return false;
         }
         else
         {
            m_Makros.SetAt(m_nEditNo, LPCTSTR(string));        // Text im Array speichern
            m_MakroList.GetWindowText(string);                 // Listentext aus dem Window holen
            if (m_MakroList.DeleteString(m_nEditNo) != CB_ERR)
            {
               m_MakroList.InsertString(m_nEditNo, LPCTSTR(string));
               m_MakroList.SetWindowText(string);
            }
         }
         m_bChanged = false;
      }
   }
   return true;
}

long MacroEditorDlg::CheckSyntax(CString& string, SControl *pControl, bool bTest)
{
   char *buffer;
   int   i, j, nlength, nControl, nKeyLength, nNoOfCurves, nMaxCurves, nRepeatDepth, nNoOfCurvesPos, nNoOfLoops;
   e_MacroCommand nKey;
   DWORD dwDebugPos;

   if (pControl == NULL) return 0;

   memset(pControl, 0, sizeof(SControl)*MAX_CONTROL);

   nlength        = string.GetLength()-1;
   buffer         = string.GetBuffer(nlength);
   nControl       = 1;                                         // Position 0 ist reserviert für NoOfCurves
   nRepeatDepth   = MAX_CONTROL;                               // Schleifentiefe für verschachtelte Repeatschleifen
   nNoOfCurves    = -1;                                        // nNoOfCurves initialisieren
   nNoOfLoops     = -1;
   nNoOfCurvesPos = -1;
   nMaxCurves     =  0;
   for (i=0; i<nlength;)
   {
      nKey = (e_MacroCommand) Keyword(&buffer[i], nKeyLength);
      dwDebugPos = MAKELONG(i, i+nKeyLength);
      if (nControl<MAX_CONTROL)
      {
         switch(nKey)
         {
            case NO_OF_CURVES:                                 // Anzahl der Kurven
               i+=nKeyLength;
               nNoOfCurves = GetnValue(&buffer[i], nKeyLength);// des Makros
               i+=nKeyLength;
               if (nNoOfCurves>0)
               {
                  if (pControl[0].nCommand == 0)               // Control 0 noch nicht belegt
                  {
                     pControl[0].nCommand = nKey;
                     pControl[0].nNumber  = nNoOfCurves;
                     pControl[0].dwDebugPos = dwDebugPos;
                     nNoOfCurvesPos       = 0;
                  }
                  else                                         // später erst weitere Änderungen der Kurvenanzahl
                  {
                     pControl[nControl].nCommand = nKey;
                     pControl[nControl].nNumber  = nNoOfCurves;
                     pControl[nControl].dwDebugPos = dwDebugPos;
                     nNoOfCurvesPos              = nControl;
                     nControl++;                               // Kontrollfeldzeiger erhöhen
                  }
               }
               else                                            // Fehler kein Index vorhanden
               {
                  if (bTest) AfxMessageBox(IDS_ERROR_NO_NO_OF_CURVES, MB_OK|MB_ICONEXCLAMATION);
                  return MAKELONG(i-nKeyLength, i);            // Fehler: Index fehlt
               }
               break;
            case NO_OF_LOOPS:                                  // Anzahl der Schleifen
            {
               nNoOfLoops = nControl;                          // Index merken
               i+=nKeyLength;
               CString num;
               char *ptrNewLine;
               char find[2] = {'\n', 0};
               ptrNewLine = strstr(&buffer[i], find);
               if (ptrNewLine) ptrNewLine[0] = 0;              // wenn innerhalb dieser Zeile
               char*pstr = strstr(&buffer[i], "=");            // ein Leerzeichen steht
               if (pstr)
               {
                  i += (pstr - &buffer[i])+1;                  // entfernen
               }
               if (ptrNewLine) ptrNewLine[0] = '\n';
               nKeyLength=ExtractString(&buffer[i], NUMERIC, num, NULL, " ");
               if (nKeyLength)                                 // steht noch eine Zahl dahinter
               {
                  i+=nKeyLength;                               // Zahl ermitteln
                  pControl[nControl].nNumber = atoi(num);      //
               }
               pControl[nControl].nCommand = nKey;             // Key zuweisen
               pControl[nControl].dwDebugPos = dwDebugPos;
               if (pControl[nControl].nNumber > 0);            // ist diese Zahl OK
               else                                            // sonst
               {
                  pControl[nControl].nNumber = 0;              // wird sie aus der Datei gelesen
                  nControl++;                                  // Kontrollfeldzähler erhöhen
                  pControl[nControl].nCommand = NEWLINE;       //
                  pControl[nControl].nNumber  = 1;
               }
               nControl++;                                     // Kontrollfeldzähler erhöhen
            }   break;
            case ENDLOOP:                                      // Ende des Makros
            case DECIMAL_POINT:                                // Schlüsselwort für Umschaltung auf Dezimalpunkt
            case DECIMAL_COMMA:                                // Schlüsselwort für Umschaltung auf Kommazahlen
            {
               i+=nKeyLength;
               pControl[nControl  ].nCommand = nKey;
               pControl[nControl  ].dwDebugPos = dwDebugPos;
               pControl[nControl++].nNumber  = 0;
            } break;
            case XLOGARITHMIC: case YLOGARITHMIC:              // logarithmische Kurvenwerte
            case XPOLAR:                                       // Polar Achse
            case ANGLE_GON: case ANGLE_RAD:                    // Winkelformate Gon, Rad
            case FORMAT_DOUBLE: case FORMAT_FLOAT:case FORMAT_SHORT:// Speicherformat
            case SINGLE_FORMAT:                                // nur Y-Werte
            case LOCUS_LINEAR:                                 // Ortskurve linear
            case LOCUS_LOG:                                    // Ortskurve logarithmisch
            case DESCRIPTION:                                  // Beschreibung [n] lesen
            case UNITX: case UNITY: case UNITLOCUS:            // Einheiten der Kurve [n] lesen
            case NO_OF_VALUES:                                 // Anzahl Kurvenwerte für Kurve [n] lesen
            case VALUEX: case VALUEY: case VALUELOCUS:         // Werte der Kurve [n] lesen
            case START_VALUE: case STEP_VALUE:                 // Start und Step-Werte der Kurve [n] lesen
            case OFFSETY:                                      // Offset der Kurve [n] lesen
            case INDEX:                                        // X-Achse als Index setzen
            {
               int nIndex = 0, nLen;
               i+=nKeyLength;
               CString num;                                    // Index [n] der jeweiligen Kurve lesen
               nLen=ExtractString(&buffer[i], NUMERIC, num, NULL, " ");
               if (nLen)
               {
                  i+=nLen;
                  nIndex = atoi(num);
               }
               else
               {
                  if (bTest) AfxMessageBox(IDS_ERROR_NO_INDEX, MB_OK|MB_ICONEXCLAMATION);
                  return MAKELONG(i-nKeyLength, i);            // Fehler: Index fehlt
               }
               if (nIndex > 0)
               {
                  if (nNoOfCurves == -1)                       // Anzahl der Kurven noch nicht festgelegt
                  {
                     if (nIndex > nMaxCurves) nMaxCurves = nIndex;// Anzahl der Kurven wird noch ermittelt
                  }
                  else if (nIndex > nNoOfCurves)               // 
                  {
                     if (bTest) AfxMessageBox(IDS_ERROR_INDEX, MB_OK|MB_ICONEXCLAMATION);
                     return MAKELONG(i-nKeyLength, i);         // Fehler: Index zu groß
                  }
                  pControl[nControl].nCommand = nKey;
                  pControl[nControl].dwDebugPos = dwDebugPos;
                  pControl[nControl].nNumber  = nIndex;
                  if (pControl[nControl-1].nCommand > SET_VALUE)// Zuweisung überprüfen
                  {  // ist das Zuweisungsformat gleich:
                     int nLastCommand = pControl[nControl-1].nCommand - SET_VALUE,
                         nCommand     = pControl[nControl  ].nCommand;
                     // Anzahl kann nur einer Anzahl zugewiesen werden
                     if ( ((nLastCommand==NO_OF_VALUES) &&
                           (nCommand    ==NO_OF_VALUES) ) ||
                     // Texte können nur Texten zugewiesen werden
                         (((nLastCommand==DESCRIPTION)||(nLastCommand== UNITX)||(nLastCommand==UNITY)||(nLastCommand==UNITLOCUS))&&
                          ((nCommand    ==DESCRIPTION)||(nCommand    == UNITX)||(nCommand    ==UNITY)||(nCommand    ==UNITLOCUS))  ) ||
                     // Werte können nur Werten zugewiesen werden
                         (((nLastCommand==VALUEX)||(nLastCommand==VALUEY)||(nLastCommand==VALUELOCUS)||
                           (nLastCommand==START_VALUE)||(nLastCommand==STEP_VALUE)||(nLastCommand==OFFSETY))&&
                          ((nCommand    ==VALUEX)||(nCommand    ==VALUEY)||(nCommand    ==VALUELOCUS)||
                           (nCommand    ==START_VALUE)||(nCommand    ==STEP_VALUE)||(nCommand    ==OFFSETY))  ))
                     {
                        if ((nLastCommand==nCommand) && (pControl[nControl-1].nNumber==pControl[nControl].nNumber))
                        {                                      // Fehler: Zuweisung an sich selbst
                           CString msg, cmd1, cmd2;
                           GetCommand(&pControl[nControl-1], cmd1);
                           GetCommand(&pControl[nControl  ], cmd2);
                           msg.Format(IDS_ERROR_INIT_SAME, cmd1, cmd2);
                           if (bTest) AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                           return MAKELONG(i-nKeyLength-cmd1.GetLength()-2, i+1);
                        }
                        else
                        {
                           for (j=nControl-2; j>0; j--)
                              if (    (   nCommand == pControl[j].nCommand
                                       || nCommand == pControl[j].nCommand-SET_VALUE)
                                   && pControl[nControl].nNumber == pControl[j].nNumber)
                                 break;
                           if (j==0)                           // Fehler: Der Initialisierer ist selbst noch nicht belegt worden
                           {
                              CString msg, cmd1;
                              GetCommand(&pControl[nControl], cmd1);
                              msg.Format(IDS_ERROR_NOVALUE, cmd1);
                              if (bTest) AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                              return MAKELONG(i-nKeyLength-cmd1.GetLength()-3, i+1);
                           }
                        }
                     }
                     else                                      // Fehler: Der Initialisierer hat ein anderes Format
                     {
                        CString msg, cmd1, cmd2;
                        GetCommand(&pControl[nControl-1], cmd1);
                        GetCommand(&pControl[nControl  ], cmd2);
                        msg.Format(IDS_ERROR_INIT_WRONG, cmd1, cmd2);
                        if (bTest) AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                        return MAKELONG(i-nKeyLength-cmd1.GetLength()-2, i+1);
                     }
                  }
                  nControl++;
               }
               else
               {
                  if (bTest) AfxMessageBox(IDS_ERROR_INDEX, MB_OK|MB_ICONEXCLAMATION);
                  return MAKELONG(i-nKeyLength, i);            // Fehler: Index fehlt
               }

            }  break;
            case EQUAL:                                        // Gleichheitszeichen
            case NEWMAKROLINE:                                 // neue Zeile einlesen
            {
               i+=nKeyLength;
               int nLastKey = pControl[nControl-1].nCommand;   // Nur, wenn vorher ein Wert gelesen wurde
               if (((nLastKey == DESCRIPTION  ) ||
                    (nLastKey == SOMECHARACTER) ||
                    (nLastKey == NO_OF_VALUES ) ||
                    (nLastKey == UNITX      )||(nLastKey == UNITY     )||(nLastKey == UNITLOCUS )||
                    (nLastKey == VALUEX     )||(nLastKey == VALUEY    )||(nLastKey == VALUELOCUS)||
                    (nLastKey == START_VALUE)||(nLastKey == STEP_VALUE)||(nLastKey == OFFSETY   )) &&
                    (nControl > 1) && (pControl[nControl-2].nCommand < SET_VALUE))// und keine Zuweisung erfolgte
               {
                  if (nKey==EQUAL)
                  {
                     int nCommand = pControl[nControl-1].nCommand;
                     long nError = 0;
                     pControl[nControl-1].nCommand = (e_MacroCommand)(pControl[nControl-1].nCommand + SET_VALUE);
                     char *ptrNewLine;
                     char find[2] = {'\n', 0};
                     ptrNewLine = strstr(&buffer[i], find);
                     if (ptrNewLine) ptrNewLine[0] = 0;        // wenn innerhalb dieser Zeile
                        
                     find[0] = '"';                            // eine Zuweisung zwischen zwei
                     char * ptr = strstr(&buffer[i], find);    // Anführungszeichen steht
                     if (ptr)                                  // erstes "
                     {
                        i +=  ptr - &buffer[i]+1;
                        ptr = strstr(&buffer[i], find);
                        if (ptr)                               // zweites "
                        {
                           int  nlen = ptr - &buffer[i];       // Länge des Textes
                           pControl[nControl].nCommand = nKey;
                           pControl[nControl].dwDebugPos = dwDebugPos;
                           pControl[nControl].nNumber  = nlen;
                           switch (nCommand)
                           {
                              case DESCRIPTION: case UNITX: case UNITY: case UNITLOCUS:
                                 pControl[nControl].ptr = new char[nlen+1];
                                 strncpy((char*)pControl[nControl].ptr, &buffer[i], nlen);
                                 ((char*)pControl[nControl].ptr)[nlen] = 0;
                                 nControl++;
                                 break;
                              default:
                              {
                                 CString msg, cmd1;
                                 GetCommand(&pControl[nControl-1], cmd1);
                                 buffer[i+nlen+1]=0;
                                 msg.Format(IDS_ERROR_INIT_WRONG, cmd1, &buffer[i-1]);
                                 if (bTest) AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                                 nError = MAKELONG(i-cmd1.GetLength(), i+nlen+1);
                              } break;
                           }
                           i+=nlen+1;
                        }
                        else
                        {
                           CString msg, cmd1;
                           msg.Format(IDS_ERROR_STRINGEND, &buffer[i]);
                           if (bTest) AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                           nError = MAKELONG(i, i+strlen(&buffer[i]));
                        }
                     }
                     else
                     {
                        CString num;
                        nKeyLength=0;
                        switch (nCommand)
                        {
                           case NO_OF_VALUES:
                              nKeyLength=ExtractString(&buffer[i], NUMERIC, num, NULL, " ");
                              if (nKeyLength)
                              {
                                 pControl[nControl].ptr = new int;
                                 *((int*)pControl[nControl].ptr) = atoi(num);
                                 nControl++;
                              }
                              break;
                           case VALUEX:      case VALUEY:     case VALUELOCUS:
                           case START_VALUE: case STEP_VALUE: case OFFSETY:
                              nKeyLength=ExtractString(&buffer[i], NUMERIC, num, NULL, " .+-e");
                              if (nKeyLength)
                              {
                                 pControl[nControl].ptr = new double;
                                 *((double*)pControl[nControl].ptr) = atof(num);
                                 nControl++;
                              }
                              break;
                           default: break;
                        }
                        i+=nKeyLength;
                     }
                     if (ptrNewLine) ptrNewLine[0] = '\n';        // Zeilenumbruch wieder setzen !
                     if (nError) return nError;
                  }
                  else
                  {
                     pControl[nControl].nCommand = nKey;
                     pControl[nControl].dwDebugPos = dwDebugPos;
                     pControl[nControl].nNumber  = 1;
                     nControl++;
                  }
               }
            }
            break;
            case STEP_BY:
               pControl[nControl].nCommand = nKey;
               pControl[nControl].dwDebugPos = dwDebugPos;
               i+=nKeyLength;
               pControl[nControl].nNumber = GetnValue(&buffer[i], nKeyLength);// des Makros
               i+=nKeyLength;
               if (pControl[nControl].nNumber == 0)
               {
                  pControl[nControl].nNumber = 1;
               }
               nControl++;
               break;
            case FIND_STRING:
            {
               i+=nKeyLength;
               char find[2] = {'"', 0};
               char * ptr = strstr(&buffer[i], find);
               if (ptr)
               {
                  i +=  ptr - &buffer[i]+1;
                  ptr = strstr(&buffer[i], find);
                  if (ptr)
                  {
                     int  nlen = ptr - &buffer[i];
                     pControl[nControl].nCommand = nKey;
                     pControl[nControl].dwDebugPos = dwDebugPos;
                     pControl[nControl].nNumber  = 1;
                     pControl[nControl].ptr      = new char[nlen+1];
                     strncpy((char*)pControl[nControl].ptr, &buffer[i], nlen);
                     ((char*)pControl[nControl].ptr)[nlen] = 0;
                     nControl++;
                     i+=nlen+1;
                  }
               }
            } break;
            case NEWLINE:                                      // Zeile überlesen
               pControl[nControl].nNumber = atoi(&buffer[i+nKeyLength]);// evtl. mehrere
               if (pControl[nControl].nNumber <= 0) pControl[nControl].nNumber = 1;
               i+=nKeyLength;
               pControl[nControl  ].nCommand = nKey;
               pControl[nControl++].dwDebugPos = dwDebugPos;
               break;
            case REPEAT:                                       // Anfang einer Schleife
               i+=nKeyLength;
               pControl[nControl  ].nCommand = nKey;           // Schlüsselwort REPEAT
               pControl[nControl  ].dwDebugPos = dwDebugPos;
               pControl[nControl++].nNumber  = nRepeatDepth++; // Schleifentiefe                    --*
               break;                                          //                                     |
            case UNTIL:                                        // Ende einer Schleife                 |
            {                                                  //                                     |
               int nDepth = nRepeatDepth-1,                    // Schleifentiefe merken               |
                   k, nCurves = 0;                             //                                     |
               i+=nKeyLength;                                  //                                     |
               for (j=nControl; j>=0; j--)                     // nach dem Schleifenanfang suchen     |
               {                                               //                                     |
                  switch(pControl[j].nCommand)                 //                                     |
                  {                                            //                                     |
                     case REPEAT:                              //                                     |
                        if (pControl[j].nNumber==(nRepeatDepth-1))// Schleifenanfang gefunden       --*
                        {
                           nRepeatDepth--;                     // Schleifentiefe verringern
                           pControl[nControl  ].nCommand = nKey;// Schlüsselwort UNTIL
                           pControl[nControl  ].dwDebugPos = dwDebugPos;
                           pControl[j].nNumber = nControl;     // Repeat kennt Until
                           pControl[nControl++].nNumber  = j;  // Until kennt Repeat
                           pControl[nControl  ].nCommand = AUTOSIZE;// Schlüsselwort AUTOSIZE
                           pControl[nControl++].nNumber  = nCurves;// Anzahl der registrierten Kurven merken
                           j = -1;                             // Repeat gefunden
                        }
                        else nDepth--;                         // lokale Schleifentiefe verringern
                        break;
                     case VALUEX:                              // X-Werte der Kurve [n] lesen
                     case VALUEY:                              // Y-Werte der Kurve [n] lesen
                     case VALUELOCUS:                          // Ortskurven-Werte der Kurve [n] lesen
                     case INDEX:                               // X-Werte als Index erhöhen
                     case VALUEX+SET_VALUE:                    // X-Werte der Kurve [n] setzen bzw. lesen
                     case VALUEY+SET_VALUE:                    // Y-Werte der Kurve [n] setzen bzw. lesen
                     case VALUELOCUS+SET_VALUE:                // Ortskurven-Werte der Kurve [n] setzen bzw. lesen
                        if (nDepth == (nRepeatDepth-1))        // ist in dieser Schleife
                        {
                           for (k=0; k<nCurves; k++)           // nach schon registrierten Kurven suchen
                              if (pControl[nControl+2+k].nCommand == (e_MacroCommand)pControl[j].nNumber)
                                 break;
                           if (k == nCurves)                   // noch nicht registriert
                           {                                   // Index der Kurve registrieren
                              pControl[nControl+2+nCurves].nCommand = (e_MacroCommand)pControl[j].nNumber;
                              nCurves++;                       // Kurvenanzahl erhöhen
                           }
                           for (k=0; k<nCurves; k++)           // registrierte Kurven prüfen
                           {
                              if (pControl[nControl+2+k].nCommand == pControl[j].nNumber)
                              {
                                 if ((pControl[j-1].nCommand==(VALUEX+SET_VALUE))||(pControl[j-1].nCommand==(VALUEY+SET_VALUE)))
                                    break;
                                 bool bError = false;
                                 if ((pControl[j].nCommand==VALUEY)||(pControl[j].nCommand==(VALUEY+SET_VALUE)))
                                 {
                                    if (pControl[nControl+2+k].nNumber & 2) bError = true;
                                    else pControl[nControl+2+k].nNumber |= 2;
                                 }
                                 else
                                 {
                                    if (pControl[nControl+2+k].nNumber & 1) bError = true;
                                    else pControl[nControl+2+k].nNumber |= 1;
                                 }
                                 if (bError)                   // Fehler: Kurvenwert wurde schon Initialisiert
                                 {
                                    CString msg, cmd1;
                                    GetCommand(&pControl[j], cmd1);
                                    msg.Format(IDS_ERROR_CURVE_INIT_TWICE, cmd1);
                                    if (bTest) AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                                    return MAKELONG(i-nKeyLength, i);
                                 }
                              }
                           }
                        }
                        break;
                     case UNTIL:                               // Ende einer Schleife
                           nDepth--;                           // lokale Schleifentiefe vergrößern
                        break;
                  }
               }
               if (j==-2)                                      // Wenn er den Anfang der Schleife gefunden hat
               {
                  if (bTest)                                   // 
                  {
                     for (k=0; k<nCurves; k++)                 // Kurvenbelegung ermitteln
                     {
                        if (pControl[nControl+k].nNumber!=3)   // Fehler, wenn die Werte einer Kurve innerhalb 
                        {                                      // einer Schleife nicht belegt werden
                           CString msg, cmd1;
                           SControl sc;
                           bool     bShowError = true;
                           sc.nCommand = ZERO_COMMAND;
                           sc.nNumber  = pControl[nControl+k].nCommand;
                           if      (pControl[nControl+k].nNumber==1) sc.nCommand = VALUEY;
                           else if (pControl[nControl+k].nNumber==2)
                           {
                              int nC, nCurve = k+1;
                              for (nC=pControl[nControl-2].nNumber-1; (nC >= 0) && (pControl[nC].nCommand != NO_OF_CURVES); nC--)
                              {
                                 if ((pControl[nC].nCommand == SINGLE_FORMAT) && (nCurve == pControl[nC].nNumber))
                                 {
                                    bShowError = false;
                                    break;
                                 }
                              }
                              sc.nCommand = VALUEX;
                           }
                           else cmd1 = _T("?");
                           if (bShowError)
                           {
                              if (sc.nCommand) GetCommand(&sc, cmd1);
                              msg.Format(IDS_ERROR_CURVE_INIT, cmd1, nRepeatDepth - MAX_CONTROL);
                              AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                           }
                        }
                     }
                  }
                  for (; i<nlength; i++)                       // in dieser Zeile weiterlesen
                  {
                     nKey = (e_MacroCommand)Keyword(&buffer[i], nKeyLength);
                     if (nKey == NO_OF_VALUES)                 // Steht das Schlüsselwort NO_OF_VALUES hinter UNTIL
                     {                                         // ist die Werteanzahl die Abbruchbedingung der Schleife
                        i+=nKeyLength;
                        pControl[nControl-1].nCommand = nKey;
                        for (k=0; k<nCurves; k++)
                        {
                           for (j=pControl[nControl-2].nNumber; j>0; j--)
                              if (((pControl[j].nCommand==  NO_OF_VALUES          )   ||
                                   (pControl[j].nCommand== (NO_OF_VALUES+SET_VALUE))) &&
                                  ( pControl[j].nNumber == pControl[nControl+k].nCommand))
                                 break;
                           if (j==0)                           // Fehler Kurven Größe nicht gesetzt
                           {
                              CString msg;
                              msg.Format(IDS_ERROR_NO_OF_VALUES, pControl[nControl+k].nCommand);
                              if (bTest) AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
                              return MAKELONG(i-nKeyLength, i);
                           }
                        }
                        break;
                     }
                     else if (nKey == NO_OF_LOOPS)             // Steht das Schlüsselwort NO_OF_LOOPS hinter UNTIL
                     {
                        i+=nKeyLength;
                        pControl[nControl-1].nCommand = nKey;  // Schlüssewort NO_OF_LOOPS eintragen
                        pControl[nControl-1].dwDebugPos = dwDebugPos;
                        pControl[nControl-1].nNumber  = 2;     // Anzahl der Schleifendurchläufe, wenn 0 : beliebig
                        pControl[nControl  ].nCommand = ZERO_COMMAND;// Schleifenzäler mit 0 initialisieren
                        pControl[nControl  ].nNumber  = nNoOfLoops;// Zeiger auf NoOfLoops
                        nNoOfLoops  = -1;                      // wieder initialisieren
                        break;
                     }
                     else if (nKey == NEWMAKROLINE)            // Steht nichts hinter UNTIL
                        break;                                 // so ist die Kurve AUTOSIZEd
                  }
                  nControl += pControl[nControl-1].nNumber;    // Kontrollfeldzeiger weiterstellen
               }
               else                                            // Fehler: Schleifenanfang nicht gefunden
               {
                  if (bTest) AfxMessageBox(IDS_ERROR_REPEAT, MB_OK|MB_ICONEXCLAMATION);
                  return MAKELONG(i-nKeyLength, i);
               }
               if (nNoOfCurvesPos != -1)
               {
                  if (nNoOfCurves == -1)
                  {
                     pControl[nNoOfCurvesPos].nNumber = nMaxCurves;
                     if (bTest) AfxMessageBox(IDS_ERROR_NO_NO_OF_CURVES, MB_OK|MB_ICONEXCLAMATION);
                  }
                  nNoOfCurvesPos = -1;
               }
            }  break;
            case SPACE:
               i+=nKeyLength;
               break;
            case COMMENT:
               for (; i<nlength; i++)
                  if (buffer[i] == '\n')
                  {
                     i++;
                     break;
                  }
               break;
            case TUEDELCHEN:                                   // " -Zeichen
               pControl[nControl  ].nCommand = SOMECHARACTER;  // Schlüsselwort für Trennzeichen
               pControl[nControl  ].dwDebugPos = dwDebugPos;
               pControl[nControl++].nNumber  = '\"';           // Trennzeichen
               i++;                                            // Schleifenzähler erhöhen
               break;
            case TABULATOR:
               pControl[nControl  ].nCommand = SOMECHARACTER;  // Schlüsselwort für Trennzeichen
               pControl[nControl  ].dwDebugPos = dwDebugPos;
               pControl[nControl++].nNumber  = '\t';           // Trennzeichen
               i++;                                            // Schleifenzähler erhöhen
               break;
            default:
               if ((nKey >= 2) && (nKey <= 20))
               {
				  int nK;
                  pControl[nControl  ].nCommand = SOMECHARACTER;// Schlüsselwort für Trennzeichen
                  pControl[nControl  ].dwDebugPos = dwDebugPos;
                  for (nK=0; gm_Keys[nK].szKey!=NULL; nK++)
                     if (gm_Keys[nK].nCode == nKey) break;
                  pControl[nControl++].nNumber  = gm_Keys[nK].szKey[0];// Trennzeichen
                  i++;                                            // Schleifenzähler erhöhen
               }
               else if (iswalpha(buffer[i]))                   // Fehler: Unbekannter Bezeichner
               {
                  CString string;
                  CString msg;
                  nKeyLength=ExtractString(&buffer[i], 0, string, " ");
                  i+=nKeyLength;
                  msg.LoadString(IDS_ERROR_UNKNOWN);
                  msg += string;
                  if (bTest) AfxMessageBox(LPCTSTR(msg), MB_OK|MB_ICONEXCLAMATION);
                  return MAKELONG(i-nKeyLength, i);
               }
               else i++;
         }
      }
   }
   pControl[nControl].nCommand = ENDLOOP;
   pControl[nControl].nNumber  = 0;

#ifdef _DEBUG
   CString strCmd;
   for (i=0; i<=nControl; i++)
   {
      GetCommand(&pControl[i], strCmd);
      TRACE("%s\n", strCmd);
   }
#endif

   return 0;
}
/************************************************************************
* Name  : Keyword                                                       *
* Zweck : Finden der Schlüsselwörter und Rückgabe des Wortindex         *
* Aufruf: Keyword(str);                                        	      *
* Parameter   :                                                         *
* str     (E) : Zeichenkette                                   (char*)  *
* Funktionswert: Index des Schlüsselwortes + 1                 (int)    *
*                0 wenn nicht gefunden                                  *
************************************************************************/
int MacroEditorDlg::Keyword(LPCTSTR str, int &nKeyLength)
{
   int i, j;
   char *key;
   for (i=0; (key = (char*)gm_Keys[i].szKey)!=NULL; i++)
   {
      for (j=0; key[j] != 0; j++)
      {
         if (str[j] != key[j]) break;
      }
      if (key[j] == 0)
      {
         nKeyLength = j;
         return gm_Keys[i].nCode;
      }
   }
   return 0;
}

/************************************************************************
* Name  : ExtractString                                                 *
* Zweck : Extrahieren eines Teilstrings aus einem anderen nach Trenn-   *
*         zeichen oder nicht dazugehörigen.                             *
* Aufruf: ExtractString(strInput, nWhat, strOutput, strCut);            *
* Parameter   :                                                         *
* strInput (E): Eingabestring                                  (char*)  *
* nWhat    (E): dazugehörige Zeichengruppe (ALPHA, NUMERIC)    (int)    *
* strOutput(A): Ausgabestring                               (CString&)  *
* strCut   (E): Trennzeichen                                   (char*)  *
* strInclude(E): Eingeschlossene Zeichen                       (char*)  *
* Funktionswert: Länge des Extrahierten Strings                (int)    *
************************************************************************/
int MacroEditorDlg::ExtractString(char *strInput, int nWhat, CString& strOutput, char *strCut, char *strInclude)
{
   int i, j;
   char strInsert[2] = " ";
   for (i=0; (strInput[i] != '\n') && (strInput[i] != 0); i++)
   {
//      if (strInput[i] == '\t') continue;
      if (((nWhat&ALPHA  ) &&                               // nur Alpha zeichen erlaubt
           (iswalpha(strInput[i])==0))||
          ((nWhat&NUMERIC) &&                               // oder nur numerische
           (iswdigit(strInput[i])==0)))
      {
         if (strInclude)                                    // weitere Zeichen eingeschlossen
         {
            for (j=0; strInclude[j] != 0; j++)              // bis zum Ende der Include Zeichen suchen
            {
               if (strInclude[j] == strInput[i]) break;
            }
            if (strInclude[j] == 0) break;                  // Include Zeichen nicht gefunden, einfügen Ende
         }
         else break;                                        // keine Include Zeichen, einfügen Ende
      }
      if (strCut)                                           // Trennzeichen vorhanden
      {
         for (j=0; strCut[j] != 0; j++)                     // bis zum Ende der Trennzeichen suchen
         {
            if (strCut[j]== strInput[i]) break;
         }
         if (strCut[j] != 0) {i++; break;}                  // Tennzeichen gefunden, einfügen Ende
      }
      strInsert[0] = strInput[i];
      strOutput += _T(strInsert);
   }
   return i;
}

/************************************************************************
* Name  : ExtractdValue                                                 *
* Zweck : Extrahieren eine double Zahl aus einem strings mit Überprüfung*
*         der Zahl auf NAN.                                             *
* Aufruf: ExtractdValue(strInput, dValue);                              *
* Parameter   :                                                         *
* strInput (E): Eingabestring                                  (char*)  *
* dValue   (A): gewandelter Wert                               (double&)*
* Funktionswert: Index des Schlüsselwortes + 1                 (int)    *
************************************************************************/
bool MacroEditorDlg::ExtractdValue(char *strInput, bool bDecComma, double &dValue)
{
   char szDecimal[4]=".";
   GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 4);
   char *ptr = strInput;                                    
   if (bDecComma)                                           // Dezimalkomma vorhanden
   {                                                        // Gruppierung mit Punkten mögl.
      while ((ptr = strstr(ptr, ".")) != NULL)              // nach Punkten suchen
      {
         int i, nLen = strlen(ptr);                         // Punkt überschreiben
         for (i=0; i<nLen; i++)                             // 0 am Ende auch 
            ptr[i] = ptr[i+1];                              // nach links verschieben
      }
      ptr = strstr(strInput, ",");                          // Komma suchen
      if (ptr) ptr[0] = szDecimal[0];                       // Trennzeichen einsetzen
   }
   else                                                     // Dezimalpunkt konvertieren
   {
      ptr = strstr(strInput, ".");                          // Punkt suchen
      if (ptr) ptr[0] = szDecimal[0];                       // Trennzeichen einsetzen
   }
   dValue = atof(strInput);                                 // auswerten
   return true; //(_isnan(dValue) == TRUE) ? false: true;
}
int MacroEditorDlg::GetnValue(char *pszNumber, int &nLen)
{
   int i;
   for (i=0; (pszNumber[i] != 0) && (pszNumber[i] != '\n'); i++)
   {
      if (!isspace(pszNumber[i]) && !isdigit(pszNumber[i]))
         break;
   }
   nLen = i;
   if (nLen)
      return atoi(pszNumber);
   else return 0;
}
int MacroEditorDlg::GetIndex(SControl *pControl, int nCurrentUntil, int nIndex)
{
   if (pControl[nCurrentUntil].nCommand == UNTIL)
   {
	  int i;
      for (i=0; i<pControl[nCurrentUntil+1].nNumber; i++) // nach registrierter Kurven suchen
         if (pControl[nCurrentUntil+2+i].nCommand == nIndex) break;
      if (i == pControl[nCurrentUntil+1].nNumber) return 0;   // nicht registriert
      return pControl[nCurrentUntil+2+i].nNumber;
   }
   return 0;
}
void MacroEditorDlg::LoadMakroText(int nEditNo)
{
   if ((nEditNo >= 0) && (nEditNo < m_Makros.GetSize())) // Stimmt der Index-Bereich ?
   {
      CString string = m_Makros.GetAt(nEditNo);          // Textobjekt holen
      m_MakroEdit.SetWindowText(string);                 // Text in das Editfenster setzen
      m_nEditNo = nEditNo;                               // EditNo merken.
   }
}

void MacroEditorDlg::OnChangeMakroEdit()     {m_bChanged = true;}
void MacroEditorDlg::OnEditchangeMakroList() {m_bChanged = true;}

void MacroEditorDlg::ReloadMacroLists()
{
   m_MakroList.ResetContent();
   int i, nNumMakros = m_Descriptions.GetSize();
   for (i=0; i<nNumMakros; i++)
   {
      m_MakroList.AddString(m_Descriptions[i]);
   }

   if (nNumMakros > 0)
   {
      m_MakroList.SetWindowText(m_Descriptions[0]);
      LoadMakroText(0);
   }
}

BOOL MacroEditorDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   ReloadMacroLists();
   gm_lEditCallBack = SetWindowLong(m_MakroEdit.m_hWnd, GWL_WNDPROC, (long)EditCallback);
   SetWindowLong(m_MakroEdit.m_hWnd, GWL_USERDATA, (long) this);
   m_bChanged = false;
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void MacroEditorDlg::OnCancel() 
{
   SaveMakroText();
   int i, nNumMakros = m_MakroList.GetCount();

   m_Descriptions.RemoveAll();
   if (nNumMakros != CB_ERR)
   {
      CString strDescription;
      for (i=0; i<nNumMakros; i++)
      {
         m_MakroList.GetLBText(i, strDescription);
         m_Descriptions.Add(LPCTSTR(strDescription));
      }
   }
   CDialog::OnCancel();
}

LRESULT MacroEditorDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
   if (wParam == DEBUG_TMSET_SELECT)
   {
      SControl *pC = (SControl*)lParam;
      if (pC && pC->dwDebugPos)
      {
         m_MakroEdit.SetSel(pC->dwDebugPos);
         m_MakroEdit.SetFocus();
      }
   }
   if (wParam == DEBUG_TMSET_RUN)
   {
      CDataExchange dx(this, FALSE);
      DDX_Radio(&dx, IDC_RD_RUN, (int&)m_nRun);
   }
   if (wParam == DEBUG_TMSET_SET_TXT)
   {
      if (m_pTexEdit == NULL)
      {
         m_pTexEdit = new CTextEdit();
      }
      if (m_pTexEdit->m_hWnd == NULL)
      {
         if (m_pTexEdit->Create(CTextEdit::IDD, this))
         {
            m_pTexEdit->ShowWindow(SW_SHOW);
         }
         CFile *pFile = (CFile*)lParam;
         int nLen = (int) pFile->GetLength();
         BYTE *pBuffer = new BYTE[nLen+1];
         pFile->Read(pBuffer, nLen);
         m_pTexEdit->m_cTextEdit.SetWindowText((LPCTSTR)pBuffer);
         delete pBuffer;
         pFile->SeekToBegin();
      }
   }
   return 0;
}

LRESULT MacroEditorDlg::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
   if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
   {
      if (pHI->iCtrlId == IDC_MACRO_EDIT)                      // der Editor für die Makros
      {
         int nStart, nEnd;
         m_MakroEdit.GetSel(nStart, nEnd);                     // Stellt eine Hilfe für die
         CString str;                                          // Syntax der Makrosprache bereit
         m_MakroEdit.GetWindowText(str);                       // Text ermitteln
         LPCTSTR psz = LPCTSTR(str);
         for (; nStart > 0; nStart--)
            if ((psz[nStart-1]!='_') && !isalpha(psz[nStart-1]))break;
         int nKey = Keyword(&psz[nStart], nEnd);
         if (nKey && (nEnd>1)) pHI->dwContextId = nKey+HID_BASE_DISPATCH;
      }
      if (pHI->dwContextId == 0) return true;                  // wenn keine Kontext ID vorhanden ist 
      CDialog::WinHelp(pHI->dwContextId, HELP_CONTEXTPOPUP);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      if (pHI->dwContextId == 0)                               // wenn keine Kontext ID vorhanden ist
      {
         pHI->dwContextId = HID_BASE_COMMAND+ID_FUNC_MAKROEDIT;// eine Command ID erzeugen diese stehen in "CAR2D_EXT.HM"
      }
      CDialog::WinHelp(pHI->dwContextId, HELP_FINDER);
   }
   return true;
}

void MacroEditorDlg::OnHelpButton()
{
   CDialog::WinHelp(HID_BASE_COMMAND+ID_FUNC_MAKROEDIT, HELP_CONTEXT);
}

void MacroEditorDlg::GetCommand(SControl *pControl, CString& str)
{
   if (pControl->nCommand == SOMECHARACTER)
   {
      str.Format(IDS_SEPARATOR_SIGN, pControl->nNumber);
   }
   else if (pControl->nCommand == (short)'\n');
   else if (pControl->nCommand == NEWMAKROLINE)
      str.LoadString(IDS_NEWLINE);
   else
   {
      int nCommand = pControl->nCommand;
      if (pControl->nCommand > SET_VALUE)
         nCommand -= SET_VALUE;
      bool bFound = false;
      int i;
      for (i=0; gm_Keys[i].szKey!=NULL; i++)
      {
         if (gm_Keys[i].nCode == nCommand)
         {
            str.Format("%s %d", gm_Keys[i].szKey, pControl->nNumber);
            if (nCommand != pControl->nCommand) str += _T(" = ");
            bFound = true;
            break;
         }
         if (!bFound)
         {
            str.Format(IDS_FORMATCHARACTER, pControl->nCommand, pControl->nNumber);
         }
      }
   }
}

LRESULT CALLBACK MacroEditorDlg::EditCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   ASSERT(gm_lEditCallBack!=0);
   if (uMsg == WM_KEYDOWN)
   {
      if (wParam == VK_TAB)
      {
         char str[4] = "   ";                                  // kein TAB
         ::SendMessage(hWnd, EM_REPLACESEL, 1, (LPARAM)str);   // TAB ist ein Trennzeichen
         return 0;
      }
      else if (wParam == VK_F2)
      {
         ::SendMessage(hWnd, WM_LBUTTONDBLCLK, MK_LBUTTON, 0);
         return 0;
      }
   }
   if (uMsg == WM_LBUTTONDBLCLK)
   {
      MacroEditorDlg *pThis = (MacroEditorDlg*)GetWindowLong(hWnd, GWL_USERDATA);
      if (pThis) return pThis->OnLButtonDblClkEdit(wParam, lParam);
   }
   return CallWindowProc((WNDPROC) gm_lEditCallBack, hWnd, uMsg, wParam, lParam);
}

LRESULT MacroEditorDlg::OnLButtonDblClkEdit(WPARAM wParam, LPARAM lParam)
{
   CTreeDlg dlg;
   char text[64];
   dlg.m_strWndTitle.LoadString(IDS_MACRO_KEYS);
   dlg.m_pParam      = this;
   dlg.m_pCallBack   = InitTree;
   dlg.m_pHelp       = TreeHelp;
   dlg.m_pParamHelp  =  this;
   dlg.m_TvItem.mask = TVIF_PARAM|TVIF_TEXT;
   dlg.m_TvItem.pszText    = text;
   dlg.m_TvItem.cchTextMax = 63;

   if ((dlg.DoModal()==IDOK) && (dlg.m_TvItem.lParam > FIRST_LEVEL))
   {
      if (dlg.m_TvItem.pszText[0] == '\t')
         dlg.m_TvItem.pszText[1] = 0;
      m_MakroEdit.SendMessage(EM_REPLACESEL, 1, (LPARAM)dlg.m_TvItem.pszText);
   }
   return 0;
}

int MacroEditorDlg::InitTree(CTreeDlg *pDlg, int nReason)
{
   if (nReason == ONOK)
   {
      return (pDlg->m_TvItem.hItem != NULL) ? 1 : 0;
   }
   if (nReason != ONINITDIALOG) return 0;
   MacroEditorDlg *pThis = (MacroEditorDlg*) pDlg->m_pParam;
   TV_INSERTSTRUCT tv_is;
   int i;
   HTREEITEM hSeparators;
   HTREEITEM hReadKeys;
   HTREEITEM hLoopKeys;
   HTREEITEM hSwitchKeys;
   HTREEITEM hOthers;
   CString   str;

   if (!pDlg->m_TreeCtrl.DeleteAllItems()) return 0;

   memset(&tv_is, 0, sizeof(TV_INSERTSTRUCT));
   tv_is.hParent      = TVI_ROOT;
   tv_is.hInsertAfter = TVI_LAST;
   tv_is.item.mask    = TVIF_PARAM|TVIF_TEXT;
   tv_is.item.lParam  = FIRST_LEVEL;
   
   str.LoadString(IDS_SEPARATOR_KEYS);
   tv_is.item.pszText = (char*)LPCTSTR(str);
   hSeparators  = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   str.LoadString(IDS_READ_KEYS);
   tv_is.item.pszText = (char*)LPCTSTR(str);
   hReadKeys = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   str.LoadString(IDS_LOOP_KEYS);
   tv_is.item.pszText = (char*)LPCTSTR(str);
   hLoopKeys  = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   str.LoadString(IDS_SWITCH_KEYS);
   tv_is.item.pszText = (char*)LPCTSTR(str);
   hSwitchKeys         = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   str.LoadString(IDS_OTHER_KEYS);
   tv_is.item.pszText = (char*)LPCTSTR(str);
   hOthers            = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   tv_is.hParent      = hSeparators;
   tv_is.item.lParam  = SEPARATOR_KEY;
   for (i=FIRST_SEPARATOR_KEY; (i<LAST_SEPARATOR_KEY) && (gm_Keys[i].szKey != NULL); i++)
   {
      tv_is.item.pszText = (char*)gm_Keys[i].szKey;
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }
   tv_is.item.pszText = "\t TAB";
   pDlg->m_TreeCtrl.InsertItem(&tv_is);

   tv_is.hParent      = hReadKeys;
   tv_is.item.lParam  = READ_KEY;
   for (i=FIRST_READ_KEY; (i<=LAST_READ_KEY) && (gm_Keys[i].szKey != NULL); i++)
   {
      tv_is.item.pszText = (char*)gm_Keys[i].szKey;
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.hParent      = hLoopKeys;
   tv_is.item.lParam  = LOOP_KEY;
   for (i=FIRST_LOOP_KEY; (i<=LAST_LOOP_KEY) && (gm_Keys[i].szKey != NULL); i++)
   {
      tv_is.item.pszText = (char*)gm_Keys[i].szKey;
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.hParent      = hSwitchKeys;
   tv_is.item.lParam  = SWITCH_KEY;
   for (i=FIRST_SWITCH_KEY; (i<=LAST_SWITCH_KEY) && (gm_Keys[i].szKey != NULL); i++)
   {
      tv_is.item.pszText = (char*)gm_Keys[i].szKey;
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.hParent      = hOthers;
   tv_is.item.lParam  = OTHER_KEY;
   for (i=FIRST_OTHER_KEY; (i<=LAST_OTHER_KEY) && (gm_Keys[i].szKey != NULL); i++)
   {
      tv_is.item.pszText = (char*)gm_Keys[i].szKey;
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }
   tv_is.item.pszText = (char*)gm_Keys[23].szKey;
   pDlg->m_TreeCtrl.InsertItem(&tv_is);
   tv_is.item.pszText = (char*)gm_Keys[1].szKey;
   pDlg->m_TreeCtrl.InsertItem(&tv_is);
   return 1;
}

void MacroEditorDlg::TreeHelp(CTreeDlg *pDlg, HELPINFO *pHI)
{
   MacroEditorDlg *pThis = (MacroEditorDlg*) pDlg->m_pParamHelp;
   char text[64];
   TVITEM tvItem;
   POINT ptHit = pHI->MousePos;
   pDlg->m_TreeCtrl.ScreenToClient(&ptHit);
   ZeroMemory(&tvItem, sizeof(TVITEM));
   tvItem.mask = TVIF_PARAM|TVIF_TEXT;
   tvItem.pszText    = text;
   tvItem.cchTextMax = 63;
   tvItem.hItem = pDlg->m_TreeCtrl.HitTest(ptHit);
   if (tvItem.hItem)
   {
      pDlg->m_TreeCtrl.GetItem(&tvItem);
      int nLen, nKey;
      nKey = pThis->Keyword(tvItem.pszText, nLen);
      if (nKey && (nLen>1)) pHI->dwContextId = nKey+HID_BASE_DISPATCH;
   }
}

void MacroEditorDlg::OnBnClickedMacroDebug()
{
   if (SaveMakroText(false))
   {
      AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FUNC_LOAD_CURVE, NULL);
   }
}

void MacroEditorDlg::OnBnClickedRdStep()
{
   CDataExchange dx(this, TRUE);
   DDX_Radio(&dx, IDC_RD_RUN, (int&)m_nRun);
}
