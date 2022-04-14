/********************************************************************/
/*                                                                  */
/*                  CARAMAT.CPP für CARAMAT.DLL                     */
/*    für die CARA-Programmodule CARACAD u. CARACALC CARAWALK.DLL   */
/*                                                                  */
/*          Materialeditor: Anlegen, Editieren und Abfragen         */
/*                      von Absorbermaterialien                     */
/*          (Name, Mat-Gruppe, Refl-Faktoren, Bitmaps etc.)         */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*     edited by Rolf Kary-Ehlers                                   */
/*                                                                  */
/*     Version 2.0            14.09.2000                            */
/*                                                                  */
/*     edited by O. Wesnigk   08.06.2000                            */
/*                                                                  */
/********************************************************************/

#define  STRICT

#include <WINDOWS.H>
#include <COMMCTRL.H>
#include <MATH.H>

#include "FILEHEADER.H"
#include "RESOURCE.H"
#include "CEtsHelp.H"

class Complex;                                   // für die zukünftige Erweiterung

//***********************************************
// Diese Makros und Strukturen müssen identisch
// zu denen in CCaraMat.H definierten sein !!
//***********************************************
#define  CARAMAT_HEAPERROR             1         // Fehlercode für Anlegen Datenfelder im Heap
#define  CARAMAT_PROGRAMERROR          2         // Programmfehler
#define  CARAMAT_CARAHELPERROR         3         // Fehler beim Laden des Help- oder Error-Strings
#define  CARAMAT_FILEREADERROR         4         // Fehler beim File-Lesen
#define  CARAMAT_FILESAVEERROR         5         // Fehler beim File-Schreiben
#define  CARAMAT_DLLERROR              0xFFFF    // DLL-Datei konnte nicht geladen werden
#define  CARAMAT_MATINDEXERROR         7         // falscher Materialindex (z.B. > Anzahl Materialien)
#define  CARAMAT_LOADBMERROR           8         // Fehler beim Laden einer Bitmap
#define  CARAMAT_USERHEAPERROR         9         // neu 31.08.99 Heapfehler vom Übergebenen Heaphandle

#define  CARAMAT_MATDESCRLENGTH		  96   		 // Länge des szString für Materialbeschreibung
#define  CARAMAT_MATNAMELENGTH    	  32 			 // Länge des szString für Materialname
#define  CARAMAT_GROUPNAMELENGTH 	  32 		    // Länge des szString für den Namen der Materialgruppe
#define  CARAMAT_MAXNOOFFREQ     	 118 	   	 // maximale Anzahl der Frequenzstützpunkte (5 - 40960 Hz)

#define  CARAMAT_BITMAPTYPE8X8         8         // Typ der BitmapInfo-Daten (8x8 Bitmap)
#define  CARAMAT_BITMAPTYPE32X32      32         // Typ der BitmapInfo-Daten (32x32 Bitmap)

#define  STANDARDLOWFREQ             200.0       // Untere Standard Frequenz 200 Hz

struct CARAMAT_MATDSCRS                          // Struktur für Materialbeschreibung für CARACAD
{
   int    nMatIndex;                             // Index/Nummer des Absorbermaterials
   int    nGroupIndex;                           // Index/Nummer der Materialgruppe, zu der Mat gehört
   char   pszMatName[CARAMAT_MATNAMELENGTH];     // Text für Materialname
   char   pszDescription[CARAMAT_MATDESCRLENGTH];// Text für Materialbeschreibung
   char   pszGroupName[CARAMAT_GROUPNAMELENGTH]; // Text für Materialgruppenname
};

struct CARAMAT_MATREFLS                          // Struktur für Materialdaten für CARACALC
{
   int      nMatIndex;                           // Index/Nummer des Absorbermaterials
   int      nNoOfFreq;                           // Anzahl der benutzten Frequenzstützpunkte
   double   dReflGradLowFreq;                    // Reflektionsgrad (Schallintensität !!) bei tiefen Freq.
   double   dFrequency[CARAMAT_MAXNOOFFREQ];     // Feld mit nNoOfFreq Frequenzwerten zwischen 5-40960 Hz
   double   dReflGradFreq[CARAMAT_MAXNOOFFREQ];  // Feld mit nNoOfFreq Reflektionsgraden zu den Frequenzen
};
//***********************************************
//***********************************************


#define  CARAMAT_NOOFBITMAPS          86         // aktuelle Anzahl der Material-Bitmaps


struct CARAMAT_MATERIALS                         // Struktur für technische Material-Detailbeschreibung
{
   int    nGroupIndex;                           // Index/Nummer der Materialgruppe, zu der Mat gehört
   int    nNoOfFreq;                             // Anzahl der benutzten Frequenzstützpunkte
   char   pszDescription[CARAMAT_MATDESCRLENGTH];// Text für Materialbeschreibung
   char   pszMatName[CARAMAT_MATNAMELENGTH];     // Text für Materialname
   int    nBM_IndexOffset;                       // Offset zur ersten Bitmap der Oberflächenstruktur
                                                 // Die Numerierung der 32x32-Bitmaps muß fortlaufend
                                                 // ohne Lücken sein !!  (die 8x8-Bitmaps werden im
                                                 // aufrufenden Programm aus den 32x32-Bitmaps erzeugt)
   int      nDummyInt;                           // Dummy, nur zum Ausrichten auf 64-Bit-Struktur
   double   dReflGradLowFreq;                    // Reflektionsgrad (Schallintensität !!) bei tiefen Freq.
   double   dFrequency[CARAMAT_MAXNOOFFREQ];     // Feld mit nNoOfFreq Frequenzwerten zwischen 5 - 40960 Hz
   double   dReflGradFreq[CARAMAT_MAXNOOFFREQ];  // Feld mit nNoOfFreq Reflektionsgraden zu den Frequenzen
   int      nSizeImpedance;                      // AnzBytes des Datenfeldes der komplexe Schallimpedanzen
   Complex  *pImpedance;                         // Pointer auf ein Komplex-Feld (118 Werte)
   int      nSizeDetails;                        // AnzBytes des Datenfeldes der Material-Detaildaten
   void     *pMatDetails;                        // Pointer auf eine Struktur der Material-Detaildaten
                                                 // (Texturen etc.)
};

struct CARAMAT_GRPMATRIXS                        // Struktur für die Korrelation Material<->Materialgruppe
{
   int    nNoOfMaterials;                        // aktuelle Anzahl der definierten Absorbermaterialien
   int    nNoOfGroups;                           // aktuelle Anzahl der definierten Materialgruppen
   char   **ppszGroupName;                       // Pointer auf die Liste der Gruppennamen-Texte
   int    *pnNoOfMatInGroup;                     // Pointer auf den Array mit der Anzahl
                                                 // der Absorbermaterialien pro Materialgruppe[i]
   int    **ppnMatIndex_Group;                   // Pointer auf die Matrix[Gruppen-#][Index in Gruppe] =
                                                 // = MaterialIndex; 'Index in Gruppe' < NoOfMatInGroup
};



CEtsHelp       g_CEtsHelp;                       // globale Instanz der Klasse CEtsHelp für die ETSHELP.DLL
int            g_nErrorCode;                     // Fehlercode für CARAMAT
HINSTANCE      g_hInstance;                      // globaler Instance-Handle

CARAMAT_GRPMATRIXS    g_MatOverview;             // Matrix für die Gruppierung der Absorbermaterialien
CARAMAT_MATERIALS     *g_pMaterial;              // Pointer auf Absorbermaterial-Array (dynamisch)
int            g_nNoOfMaterialsEx;               // erweiterte Mat-Anzahl für Neuanlage von Materialien
int            g_nNoOfGroupsEx;                  // erweiterte Gruppen-Anzahl für Neuanlage von Mat-Gruppen
HANDLE         g_hMyHeap;                        // Handle auf privaten Heap
WNDPROC        g_lpOldEditProc;                  // Pointer der org. EditControl-Prozedur (für SubClassing)

int            g_nMatIndex, g_nGroupIndex;       // aktuelle Indizes von Material bzw. Gruppe (von 1 an)
char           g_pszMatName[CARAMAT_MATNAMELENGTH], // aktuelle Namen und Mat-Beschreibung
               g_pszMatDescr[CARAMAT_MATDESCRLENGTH],
               g_pszGroupName[CARAMAT_GROUPNAMELENGTH];
int            g_nBM_IndexOffset;                // aktueller IndexOffset zur 1. Bitmap (0 = 1. Bitmap)
double         g_dReflGradLowFreq;               // aktueller Reflektionsgrad (z.B. 0.87 = 13% Abs.Grad)
int            g_nNoOfFreq;                      // aktuelle Anzahl der def. Freq.- und (!!) ReflGrad-Werte
double         g_dFrequency[CARAMAT_MAXNOOFFREQ],// aktuelle Frequenzwert-Liste
               g_dReflGradFreq[CARAMAT_MAXNOOFFREQ]; // aktuelle Reflektionsgrad-Liste
double         g_dCalcValues[CARAMAT_MAXNOOFFREQ];// aktuelle berechnete Reflektionsgradwerte
char          *g_pszCaption;                     // Pointer auf übergebene Überschrift 

int            g_nFlagChanged;                   // Flag für Materialdaten im DialogCreate geändert
                                                 // Flag = 1  -> Daten geändert
                                                 // Flag = 0  -> Daten unverändert
int            g_nFlagRadioChanged;              // Flag = 1  -> Radiobutton Mat_Edit wurde gedrückt, keine Datenänderung !!

int            g_nSelListItem = -1;

extern "C"
{
   void Destructor(void *);                      // neu 27.08.99 der DLL-Destructor !

   int  GetLastErrorCode();                      // Wenn Return-Wert der 'Haupt'-Schnittstellenfunktionen
                                                 // = false, dann wird hiermit der ErrorCode abgefragt.
      
   bool EditMaterial( HWND );                    // HWND: Handle des aufrufenden Fensters
      
   bool SelectMaterial( HWND, int *, char * );   // HWND: Handle des aufrufenden Fensters
                                                 // int *: Pointer auf nMatIndex, Aufruf-/Rückgabeparameter
                                                 // char *: Pointer auf szString (Überschrift), Aufrufpar.

   bool GetBitmapInfo( int, int, HANDLE, BITMAPINFO ** );
                                                 // int: nMatIndex, Index-Nummer des gewünschten Materials
                                                 // int: nType, TypMakro für gewünschte Bitmap (8x8, 32x32)
                                                 // HANDLE: hMyHeap, Handle auf privaten Heap des Aufrufers
                                                 // BITMAPINFO **: ppBitmapInfo, Pointer auf Pointer auf
                                                 //                BITMAPINFO-Struktur, die in hMyHeap von
                                                 //                CARAMAT angelegt und beschrieben wird.

   bool GetMatDscrDaten( int, CARAMAT_MATDSCRS * );
                                                 // int: nMatIndex, gewünschter MaterialIndex
                                                 // MatDscrDaten *: Pointer auf Struktur für Texte,
                                                 //                 Rückgabeparameter

   bool GetMatReflDaten( int, CARAMAT_MATREFLS * );
                                                 // int: nMatIndex, gewünschter MaterialIndex
                                                 // MatReflDaten *: Pointer auf Struktur für Refl-Grade,
                                                 //                 Rückgabeparameter

   void Calc_ReflCoeff1(double, double *);
   void Calc_ReflCoeff2(int, double *, double *, double*);
}

/********************************************************************/
/*                                                                  */
/*               Freigeben der CEtsHelp.DLL im                      */
/*                         DLL - Destructor                         */
/*                                                                  */
/********************************************************************/
bool FreeDaten();

LRESULT OnDrawFrqResponse(DRAWITEMSTRUCT *);
void    GetMatReflDataFromList(HWND hDlg);
void    CalcFrequResp();

void Destructor(void * pParam)
{
   g_CEtsHelp.Destructor();                      // neu 27.08.99 Wichtig neue Schnittstellenklasse !
/*
   if( g_hMyHeap )
   {
      if( g_pMaterial != NULL ) FreeDaten();     // dynamische Datenfelder freigeben
                                                 // später: die evtl. vorhandenen Zusatzdatenfelder
                                                 // für Impedanz und Materialdetaildaten vorher löschen !!

      HeapDestroy(g_hMyHeap);                    // privaten Heap löschen
      g_hMyHeap = NULL;
   }
*/
}





/********************************************************************/
/*                                                                  */
/*               Freigeben der globalen Datenfelder                 */
/* GroupName, NoOfMatInGroup, MatIndex_Group innerhalb g_MatOverview*/
/*                         und g_pMaterial                          */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false          */
/*                                                                  */
/********************************************************************/
bool FreeDaten()
{
   int      i;
   bool     nerror = true;


   for( i = 0; i <= g_nNoOfGroupsEx; i++ )
   {
      if( (g_MatOverview.ppszGroupName != NULL) && (g_MatOverview.ppszGroupName[i] != NULL) )
      {
         if( HeapFree( g_hMyHeap, 0, g_MatOverview.ppszGroupName[i] ) == NULL )   nerror = false;
      }
      if( (g_MatOverview.ppnMatIndex_Group != NULL) && (g_MatOverview.ppnMatIndex_Group[i] != NULL) )
      {
         if( HeapFree( g_hMyHeap, 0, g_MatOverview.ppnMatIndex_Group[i] ) == NULL )   nerror = false;
      }
   }
      
   if( g_MatOverview.ppszGroupName != NULL )
   {
      if( HeapFree( g_hMyHeap, 0, g_MatOverview.ppszGroupName ) == NULL )   nerror = false;
   }
   if( g_MatOverview.pnNoOfMatInGroup != NULL )
   {
      if( HeapFree( g_hMyHeap, 0, g_MatOverview.pnNoOfMatInGroup ) == NULL )   nerror = false;
   }
   if( g_MatOverview.ppnMatIndex_Group != NULL )
   {
      if( HeapFree( g_hMyHeap, 0, g_MatOverview.ppnMatIndex_Group ) == NULL )   nerror = false;
   }
   if( g_pMaterial != NULL )                     // die evtl. vorhandenen Zusatzdaten getrennt freigeben
   {
      if( HeapFree( g_hMyHeap, 0, g_pMaterial ) == NULL )   nerror = false;
   }

   if( nerror == false )   g_nErrorCode = CARAMAT_HEAPERROR;

   g_MatOverview.ppszGroupName     = NULL;
   g_MatOverview.pnNoOfMatInGroup  = NULL;
   g_MatOverview.ppnMatIndex_Group = NULL;
   g_pMaterial                     = NULL;

   return (nerror);
}


/********************************************************************/
/*                                                                  */
/*            Anlegen/Erweitern der globalen Datenfelder            */
/* GroupName, NoOfMatInGroup, MatIndex_Group innerhalb g_MatOverview*/
/*                         und g_pMaterial                          */
/*                                                                  */
/*   Parameter:                                                     */
/*     - Flag: 0 -> erstes Anlegen, 1 -> Erweitern um 100 Sätze     */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false          */
/*                                                                  */
/********************************************************************/
bool AllocDaten( int flag )
{
   int            i, j;
   int            nNoOfMaterialsEx, nNoOfGroupsEx;
   bool           nerror = true;
   CARAMAT_GRPMATRIXS    MatOverview;            // Matrix für die Gruppierung der Absorbermaterialien
   CARAMAT_MATERIALS     *pMaterial;             // Pointer auf Absorbermaterial-Array (dynamisch)


                                                 //  erweiterte Datenfelder lokal anlegen
   nNoOfMaterialsEx = g_MatOverview.nNoOfMaterials + 100; // erweiterte Materialanzahl
   nNoOfGroupsEx    = g_MatOverview.nNoOfGroups + 100;    // erweiterte Anzahl für Materialgruppen

                                                 // Datenfeld GroupName anlegen (StartIndex = 1 !!)
   MatOverview.ppszGroupName = (char **) HeapAlloc( g_hMyHeap,
                                                    HEAP_ZERO_MEMORY,
                                                    (nNoOfGroupsEx+1)*sizeof(char *) );
   if( MatOverview.ppszGroupName == NULL )   nerror = false;

   for( i = 0; (i <= nNoOfGroupsEx) && (nerror == true); i++ )
   {
      MatOverview.ppszGroupName[i] = (char *) HeapAlloc( g_hMyHeap,
                                                         HEAP_ZERO_MEMORY,
                                                         CARAMAT_GROUPNAMELENGTH*sizeof(char) );
      if( MatOverview.ppszGroupName[i] == NULL )   nerror = false;
   }
                                                 // Datenfeld NoOfMatInGroup anlegen (StartIndex = 1 !!)
   MatOverview.pnNoOfMatInGroup = (int *) HeapAlloc( g_hMyHeap,
                                                     HEAP_ZERO_MEMORY,
                                                     (nNoOfGroupsEx+1)*sizeof(int) );
   if( MatOverview.pnNoOfMatInGroup == NULL )   nerror = false;

                                                 // Datenfeld MatIndex_Group anlegen (StartIndex = 1 !!)
   MatOverview.ppnMatIndex_Group = (int **) HeapAlloc( g_hMyHeap,
                                                       HEAP_ZERO_MEMORY,
                                                       (nNoOfGroupsEx+1)*sizeof(int *) );
   if( MatOverview.ppnMatIndex_Group == NULL )   nerror = false;

   for( i = 0; (i <= nNoOfGroupsEx) && (nerror == true); i++ )
   {
                                                 // hier in der Zeile StartIndex = 0 !!
      MatOverview.ppnMatIndex_Group[i] = (int *) HeapAlloc( g_hMyHeap,
                                                            HEAP_ZERO_MEMORY,
                                                            nNoOfMaterialsEx*sizeof(int) );
      if( MatOverview.ppnMatIndex_Group[i] == NULL )   nerror = false;
   }
                                                 // Datenfeld Material anlegen (StartIndex = 1 !!)
   pMaterial = (CARAMAT_MATERIALS *) HeapAlloc( g_hMyHeap, HEAP_ZERO_MEMORY,
                                                (nNoOfMaterialsEx+1)*sizeof(CARAMAT_MATERIALS) );
   if( pMaterial == NULL )   nerror = false;

                                                 // alte Daten retten und entspr. Datenfelder löschen
   if( (flag == 1) && (nerror == true) )
   {
      for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
      {
                                                 // GroupName umspeichern
         wsprintf( MatOverview.ppszGroupName[i], "%s", g_MatOverview.ppszGroupName[i] );
                                                 // NoOfMatInGroup umspeichern
         MatOverview.pnNoOfMatInGroup[i] = g_MatOverview.pnNoOfMatInGroup[i];
                                                 // MatIndex_Group umspeichern
         for( j = 0; j < g_MatOverview.pnNoOfMatInGroup[i]; j++ )
            MatOverview.ppnMatIndex_Group[i][j] = g_MatOverview.ppnMatIndex_Group[i][j];
      }
                                                 // Material-Strukturen umspeichern
      for( i = 1; i <= g_MatOverview.nNoOfMaterials; i++ )
         pMaterial[i] = g_pMaterial[i];          // die dynamisch evtl. angelegten 'erweiterten' Zusatzdaten
                                                 // für Impedanz und Material-Detaildaten bleiben erhalten!!

                                                 // alte globale Datenfelder freigeben (ohne Zusatzdaten!!)
      nerror = FreeDaten();
   }
                                                 // lokale Heap-Pointer auf globale umsetzen
   g_nNoOfMaterialsEx = nNoOfMaterialsEx;
   g_nNoOfGroupsEx    = nNoOfGroupsEx;
   g_MatOverview.ppszGroupName     = MatOverview.ppszGroupName;
   g_MatOverview.pnNoOfMatInGroup  = MatOverview.pnNoOfMatInGroup;
   g_MatOverview.ppnMatIndex_Group = MatOverview.ppnMatIndex_Group;
   g_pMaterial                     = pMaterial;

   if( nerror == false )
   {
      FreeDaten();                               // Fehler beim Allocieren -> Datenfelder löschen
                                                 // später: die evtl. vorhandenen Zusatzdatenfelder
                                                 // für Impedanz und Materialdetaildaten vorher löschen !!
      g_nErrorCode = CARAMAT_HEAPERROR;
   }

   return (nerror);
}


/********************************************************************/
/*                                                                  */
/*       Öffnen und Lesen Material-Datei (im aktuellen Ordner)      */
/*                                                                  */
/*   Parameter:                                                     */
/*     - Handle auf des Window des aufrufenden Prozesses            */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false          */
/*                                                                  */
/********************************************************************/
bool LoadMatDataFile( HWND hwnd )
{
	int	         i, nanz_read;
   char           szbuffer[256];
   HANDLE         hFile;
   CFileHeader    FileHeader;                    // Materialdaten
   unsigned int   nCRC_Load;                     // Original-CRC-Code im Header für Materialdaten


   hFile = CreateFile( "CARAMAT.RFL", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

   if( hFile != INVALID_HANDLE_VALUE )
   {
	   ReadFile( hFile, &FileHeader, sizeof(CFileHeader), (LPDWORD) &nanz_read, NULL );
      nCRC_Load = FileHeader.GetChecksum();
      FileHeader.CalcChecksum();                 // Prüfsumme zurücksetzen

      if( FileHeader.IsType( "MAT" ) == 0 )
      {
         wsprintf( szbuffer, "CARAMAT.RFL-File has a wrong format !" );
         MessageBox( (HWND) hwnd, szbuffer,
                     "Fatal Error: FILEREAD", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
         
         CloseHandle( hFile );
         g_nErrorCode = CARAMAT_FILEREADERROR;
         return (false);
      }
                                                 // Daten der GruppenMatrix lesen      
      ReadFile( hFile, &g_MatOverview.nNoOfMaterials, sizeof(int), (LPDWORD) &nanz_read, NULL );
      FileHeader.CalcChecksum( &g_MatOverview.nNoOfMaterials, sizeof(int) ); // 1. Teil der neuen Prüfsumme

      ReadFile( hFile, &g_MatOverview.nNoOfGroups, sizeof(int), (LPDWORD) &nanz_read, NULL );
      FileHeader.CalcChecksum( &g_MatOverview.nNoOfGroups, sizeof(int) );    // 2. Teil der neuen Prüfsumme

      if( !(AllocDaten( 0 )) )                   // dynamische Datenfelder erstmals anlegen
      {
         CloseHandle( hFile );
         return (false);                         // Heap-Error
      }

      for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
      {
                                                 // Gruppennamen einlesen + Prüfsumme bilden
         ReadFile( hFile, g_MatOverview.ppszGroupName[i],
                   CARAMAT_GROUPNAMELENGTH*sizeof(char), (LPDWORD) &nanz_read, NULL );
         FileHeader.CalcChecksum( g_MatOverview.ppszGroupName[i], CARAMAT_GROUPNAMELENGTH*sizeof(char) );
      }
                                                 // NoOfMatInGroup[i] einlesen + Prüfsumme bilden
      ReadFile( hFile, &g_MatOverview.pnNoOfMatInGroup[1],
                g_MatOverview.nNoOfGroups*sizeof(int), (LPDWORD) &nanz_read, NULL );
      FileHeader.CalcChecksum( &g_MatOverview.pnNoOfMatInGroup[1],
                               g_MatOverview.nNoOfGroups*sizeof(int) );

      for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
      {
                                                 // MaterialIndizes pro Gruppe einlesen + Prüfsumme bilden
         ReadFile( hFile, g_MatOverview.ppnMatIndex_Group[i],
                   g_MatOverview.pnNoOfMatInGroup[i]*sizeof(int), (LPDWORD) &nanz_read, NULL );
         FileHeader.CalcChecksum( g_MatOverview.ppnMatIndex_Group[i],
                                  g_MatOverview.pnNoOfMatInGroup[i]*sizeof(int) );
      }
                                                 // Materialdaten einlesen + Prüfsumme bilden
      for( i = 1; i <= g_MatOverview.nNoOfMaterials; i++ )
      {
         ReadFile( hFile, &g_pMaterial[i],
                   sizeof(CARAMAT_MATERIALS), (LPDWORD) &nanz_read, NULL );
         FileHeader.CalcChecksum( &g_pMaterial[i], sizeof(CARAMAT_MATERIALS) );

         g_pMaterial[i].pImpedance  = NULL;
         g_pMaterial[i].pMatDetails = NULL;

         if( g_pMaterial[i].nSizeImpedance != 0 )
         {
            // Datenfeld für komplexe Schallimpedanzwerte anlegen und einlesen !!
         }
         if( g_pMaterial[i].nSizeDetails != 0 )
         {
            // Datenfeld für Material-Detaildaten anlegen und einlesen !!
         }
      }

      CloseHandle( hFile );
   
      if( FileHeader.IsValid( nCRC_Load ) == 0 )
      {
         wsprintf( szbuffer, "CARAMAT.RFL-File is not valid !" );
         MessageBox( (HWND) hwnd, szbuffer,
                     "Fatal Error: FILEREAD", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
         
         g_nErrorCode = CARAMAT_FILEREADERROR;
         return (false);
      }
   }
   else
   {
      wsprintf( szbuffer, "Cannot find CARAMAT.RFL-File in actual directory !" );
      MessageBox( (HWND) hwnd, szbuffer,
                  "Fatal Error: FILEREAD", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );

      g_nErrorCode = CARAMAT_FILEREADERROR;
      return (false);
   }
   
   return (true);
}


/********************************************************************/
/*                                                                  */
/*   Abspeichern/Sichern der Material-Datei (im aktuellen Ordner)   */
/*                                                                  */
/*   Parameter:                                                     */
/*     - Handle auf das Window des aufrufenden Prozesses            */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false          */
/*                                                                  */
/********************************************************************/
bool SaveMatDataFile( HWND hwnd )
{
	int	         i, j, nanz_read;
   char           szbuffer[256];
   HANDLE         hFile;
   CFileHeader    FileHeader( "MAT", "1998", 100 ); // Materialdaten

   
   hFile = CreateFile( "CARAMAT.RFL", GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

   if( hFile != INVALID_HANDLE_VALUE )
   {
      FileHeader.SetComment( "Material Description Data" );
      FileHeader.CalcChecksum();                 // Prüfsumme zurücksetzen

      WriteFile( hFile, (LPCVOID) &FileHeader, sizeof(CFileHeader), (LPDWORD) &nanz_read, NULL );
                                                 // Daten der GruppenMatrix schreiben      
      WriteFile( hFile, (LPCVOID) &g_MatOverview.nNoOfMaterials, sizeof(int), (LPDWORD) &nanz_read, NULL );
      FileHeader.CalcChecksum( &g_MatOverview.nNoOfMaterials, sizeof(int) ); // 1. Teil der neuen Prüfsumme

      WriteFile( hFile, (LPCVOID) &g_MatOverview.nNoOfGroups, sizeof(int), (LPDWORD) &nanz_read, NULL );
      FileHeader.CalcChecksum( &g_MatOverview.nNoOfGroups, sizeof(int) );    // 2. Teil der neuen Prüfsumme

      for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
      {
                                                 // Gruppennamen schreiben + Prüfsumme bilden
         WriteFile( hFile, (LPCVOID) g_MatOverview.ppszGroupName[i],
                    CARAMAT_GROUPNAMELENGTH*sizeof(char), (LPDWORD) &nanz_read, NULL );
         FileHeader.CalcChecksum( g_MatOverview.ppszGroupName[i], CARAMAT_GROUPNAMELENGTH*sizeof(char) );
      }
                                                 // NoOfMatInGroup[i] schreiben + Prüfsumme bilden
      WriteFile( hFile, (LPCVOID) &g_MatOverview.pnNoOfMatInGroup[1],
                 g_MatOverview.nNoOfGroups*sizeof(int), (LPDWORD) &nanz_read, NULL );
      FileHeader.CalcChecksum( &g_MatOverview.pnNoOfMatInGroup[1], g_MatOverview.nNoOfGroups*sizeof(int) );

      for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
      {
                                                 // MaterialIndizes pro Gruppe schreiben + Prüfsumme bilden
         WriteFile( hFile, (LPCVOID) g_MatOverview.ppnMatIndex_Group[i],
                    g_MatOverview.pnNoOfMatInGroup[i]*sizeof(int), (LPDWORD) &nanz_read, NULL );
         FileHeader.CalcChecksum( g_MatOverview.ppnMatIndex_Group[i],
                                  g_MatOverview.pnNoOfMatInGroup[i]*sizeof(int) );
      }

                                                 // die unbenutzten frequenzabhängigen Daten auf 0 setzen
      for( i = 1; i <= g_MatOverview.nNoOfMaterials; i++ )
      {
         for( j = g_pMaterial[i].nNoOfFreq; j < CARAMAT_MAXNOOFFREQ; j++ )
         {
            g_pMaterial[i].dFrequency[j]    = 0.0;
            g_pMaterial[i].dReflGradFreq[j] = 0.0;
         }

                                                 // Materialdaten schreiben + Prüfsumme bilden
         WriteFile( hFile, (LPCVOID) &g_pMaterial[i],
                    sizeof(CARAMAT_MATERIALS), (LPDWORD) &nanz_read, NULL );
         FileHeader.CalcChecksum( &g_pMaterial[i], sizeof(CARAMAT_MATERIALS) );

         if( g_pMaterial[i].nSizeImpedance != 0 )
         {
            // Datenfeld für komplexe Schallimpedanzwerte abspeichern !!
         }
         if( g_pMaterial[i].nSizeDetails != 0 )
         {
            // Datenfeld für Material-Detaildaten abspeichern !!
         }
      }

      SetFilePointer( hFile, 0, NULL, FILE_BEGIN ); // File-Pointer auf 0 setzen und Fileheader schreiben
      WriteFile( hFile, (LPCVOID) &FileHeader, sizeof(CFileHeader), (LPDWORD) &nanz_read, NULL );

      CloseHandle( hFile );
   }
   else
   {
      wsprintf( szbuffer, "Cannot open CARAMAT.RFL-File !" );
      MessageBox( (HWND) hwnd, szbuffer,
                  "Fatal Error: FILEOPEN", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );

      g_nErrorCode = CARAMAT_FILESAVEERROR;
      return (false);
   }

   return (true);
}


/********************************************************************/
/*                                                                  */
/* Call-Back-SubClass-Funktion für EditControls von List-/TreeViews */
/*                                                                  */
/********************************************************************/
LRESULT CALLBACK EditSubclass( HWND hWnd, UINT wMsg, WPARAM wp, LPARAM lp )
{
                                                 // alle Tastencodes umleiten
   if( wMsg == WM_GETDLGCODE )   return (DLGC_WANTALLKEYS);
                                                 // org. Editor-Control-Funktion aufrufen
   return (CallWindowProc( g_lpOldEditProc, hWnd, wMsg, wp, lp ));
}


/********************************************************************/
/*                                                                  */
/* Funktion: Suche nach dem i-ten Material im TreeView,             */
/*           Rückgabe des betr. TV-Item-Handles                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/
HTREEITEM GetHandle_TVItem_Mat( HWND hDlg, int nMatIndex )
{
   TVITEM              tvItem;                   // Struktur für TreeView-Items
   HTREEITEM           hTV_ItemGroup,            // Handle auf ein TV-Item einer MaterialGruppe
                       hTV_ItemMat;              // Handle auf ein TV-Item eines Materials


   tvItem.mask = TVIF_PARAM;                     // von den einzelnen Items nur den Index abfragen
                                                 // Handle des obersten Eintrags an der Wurzel holen
   hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );

   while ( hTV_ItemGroup != NULL )               // Schleife über Materialgruppen
   {
                                                 // Handle des ersten Kind-Eintrags (=Material) der Gruppe
      hTV_ItemMat = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                    TVGN_CHILD, (LPARAM) (HTREEITEM) hTV_ItemGroup );

      while( hTV_ItemMat != NULL )               // Schleife über alle Materialien der betr. Gruppe
      {
         tvItem.hItem = hTV_ItemMat;             // MatHandle setzen, Abfrage des lParam
         SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETITEM,
                             0, (LPARAM) (const LPTVITEM) &tvItem );
         if( tvItem.lParam == nMatIndex )   return (hTV_ItemMat);

                                                 // Handle des nächsten Kind-Eintrags (=Material) der Gruppe
         hTV_ItemMat = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                       TVGN_NEXT, (LPARAM) (HTREEITEM) hTV_ItemMat );
      }
                                                 // Handle des nächsten Root-Eintrags (=MatGruppe)
      hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                      TVGN_NEXT, (LPARAM) (HTREEITEM) hTV_ItemGroup );
   }

   return (NULL);
}


/********************************************************************/
/*                                                                  */
/* Funktion: Suche nach dem Materialindex des nächsten Eintrags     */
/*           im Material-TreeView                                   */
/*                                                                  */
/*                                                                  */
/********************************************************************/
int GetMatIndex_for_NextTVItem( HWND hDlg, int nMatIndex )
{
   TVITEM              tvItem;                   // Struktur für TreeView-Items
   HTREEITEM           hTV_ItemGroup,            // Handle auf ein TV-Item einer MaterialGruppe
                       hTV_ItemMat;              // Handle auf ein TV-Item eines Materials
   int                 fFlagFoundActMatIndex = 0;// Flag, daß akt. MatIndex im TreeView gefunden


   tvItem.mask = TVIF_PARAM;                     // von den einzelnen Items nur den Index abfragen
                                                 // Handle des obersten Eintrags an der Wurzel holen
   hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );

   while ( hTV_ItemGroup != NULL )               // Schleife über Materialgruppen
   {
                                                 // Handle des ersten Kind-Eintrags (=Material) der Gruppe
      hTV_ItemMat = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                    TVGN_CHILD, (LPARAM) (HTREEITEM) hTV_ItemGroup );

      while( hTV_ItemMat != NULL )               // Schleife über alle Materialien der betr. Gruppe
      {
         tvItem.hItem = hTV_ItemMat;             // MatHandle setzen, Abfrage des lParam
         SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETITEM,
                             0, (LPARAM) (const LPTVITEM) &tvItem );
         if( tvItem.lParam == nMatIndex )        fFlagFoundActMatIndex = 1;
         else if( fFlagFoundActMatIndex == 1 )   return (tvItem.lParam);

                                                 // Handle des nächsten Kind-Eintrags (=Material) der Gruppe
         hTV_ItemMat = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                       TVGN_NEXT, (LPARAM) (HTREEITEM) hTV_ItemMat );
      }
                                                 // Handle des nächsten Root-Eintrags (=MatGruppe)
      hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                      TVGN_NEXT, (LPARAM) (HTREEITEM) hTV_ItemGroup );
      if( hTV_ItemGroup == NULL )                // TreeView-Ende
      {
         fFlagFoundActMatIndex = 1;              // auf den ersten TreeVieweintrag setzen
                                                 // Handle des obersten Eintrags an der Wurzel nochmals holen
         hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );
      }
   }
   return (1);                                   // wird nie angesprungen, aber sonst Compilerwarnung
}


/********************************************************************/
/*                                                                  */
/* Funktion: Suche nach der i-ten Materialgruppe im TreeView,       */
/*           Rückgabe des betr. TV-Item-Handles                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/
HTREEITEM GetHandle_TVItem_Group( HWND hDlg, int nGroupIndex )
{
   TVITEM              tvItem;                   // Struktur für TreeView-Items
   HTREEITEM           hTV_ItemGroup;            // Handle auf ein TV-Item einer MaterialGruppe


   tvItem.mask = TVIF_PARAM;                     // von den einzelnen Items nur den Index abfragen
                                                 // Handle des obersten Eintrags an der Wurzel holen
   hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );

   while ( hTV_ItemGroup != NULL )               // Schleife über Materialgruppen
   {
      tvItem.hItem = hTV_ItemGroup;              // MatHandle setzen, Abfrage des lParam
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETITEM,
                          0, (LPARAM) (const LPTVITEM) &tvItem );
      if( tvItem.lParam == nGroupIndex )   return (hTV_ItemGroup);
                                                 // Handle des nächsten Root-Eintrags (=MatGruppe)
      hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                      TVGN_NEXT, (LPARAM) (HTREEITEM) hTV_ItemGroup );
   }
   return (NULL);
}


/********************************************************************/
/*                                                                  */
/* Funktion: Suche nach dem Materialgruppenindex des nächsten       */
/*           Eintrags im Material-TreeView                          */
/*                                                                  */
/*                                                                  */
/********************************************************************/
int GetMatGroupIndex_for_NextTVItem( HWND hDlg, int nGroupIndex )
{
   TVITEM              tvItem;                   // Struktur für TreeView-Items
   HTREEITEM           hTV_ItemGroup;            // Handle auf ein TV-Item einer MaterialGruppe
   int                 fFlagFoundActMatGroupIndex = 0; // Flag, daß akt. MatGroupIndex im TreeView gefunden


   tvItem.mask = TVIF_PARAM;                     // von den einzelnen Items nur den Index abfragen
                                                 // Handle des obersten Eintrags an der Wurzel holen
   hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );

   while ( hTV_ItemGroup != NULL )               // Schleife über Materialgruppen
   {
      tvItem.hItem = hTV_ItemGroup;              // MatHandle setzen, Abfrage des lParam
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETITEM,
                          0, (LPARAM) (const LPTVITEM) &tvItem );
      if( tvItem.lParam == nGroupIndex )           fFlagFoundActMatGroupIndex = 1;
      else if( fFlagFoundActMatGroupIndex == 1 )   return (tvItem.lParam);
                                                 // Handle des nächsten Root-Eintrags (=MatGruppe)
      hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                      TVGN_NEXT, (LPARAM) (HTREEITEM) hTV_ItemGroup );
      if( hTV_ItemGroup == NULL )                // TreeView-Ende
      {
         fFlagFoundActMatGroupIndex = 1;         // auf den ersten TreeVieweintrag setzen
                                                 // Handle des obersten Eintrags an der Wurzel nochmals holen
         hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );
      }
   }
   return (1);                                   // wird nie angesprungen, aber sonst Compilerwarnung
}


/********************************************************************/
/*                                                                  */
/* Funktion: Alle Vater-Eintäge zuklappen,                          */
/*                                                                  */
/* nGroupIndex = 0: alles deselektieren                             */
/*             = i: i-te MatGruppe selektieren                      */
/*                                                                  */
/********************************************************************/
void CollapseTVParents( HWND hDlg, int nGroupIndex )
{
   HTREEITEM           hTV_ItemGroup;            // Handle auf ein TV-Item einer MaterialGruppe


                                                 // alle selektierten Items deselektieren
   SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_CARET, NULL );
                                                 // Handle des obersten Eintrags an der Wurzel holen
   hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );
   
   while ( hTV_ItemGroup != NULL )               // Schleife über Materialgruppen (Parents schließen)
   {
                                                 // erst sichtbar machen und dann schließen
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_FIRSTVISIBLE,
                          (LPARAM) (HTREEITEM) hTV_ItemGroup );
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_EXPAND,
                          TVE_COLLAPSE, (LPARAM) (HTREEITEM) hTV_ItemGroup );
                                                 // Handle des nächsten Root-Eintrags (=MatGruppe)
      hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM,
                                                      TVGN_NEXT, (LPARAM) (HTREEITEM) hTV_ItemGroup );
   }

   if( nGroupIndex > 0 )                         // MatGruppe selektieren
   {
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_CARET,
                          (LPARAM) (HTREEITEM) GetHandle_TVItem_Group( hDlg, nGroupIndex ) );
                                                 // die betr. MatGruppe sichtbar machen
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_FIRSTVISIBLE,
                          (LPARAM) (HTREEITEM) GetHandle_TVItem_Group( hDlg, nGroupIndex ) );
   }
   else
   {
                                                 // obersten Eintrags an der Wurzel sichtbar machen
      hTV_ItemGroup = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_GETNEXTITEM, TVGN_ROOT, 0 );
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_FIRSTVISIBLE,
                          (LPARAM) (HTREEITEM) hTV_ItemGroup );
   }
}


/********************************************************************/
/*                                                                  */
/* Funktion: Sichern der geänderten Daten einer                     */
/*           Absorber-Materialgruppe in die globalen Datenfelder    */
/*                                                                  */
/********************************************************************/
BOOL SaveChangedGroupData( HWND hDlg )
{
   int       i, flagEqual = 0;
   char      szString[256],
             szString1[256],
             szString2[256],
             szString3[256];                     // Textbuffer
   TVITEM    tvItem;                             // Struktur für TreeView-Items


                                                 // Materialgruppenname gelöscht ?, nicht speichern
   if( strlen(g_pszGroupName) == 0 )   return (false);

   for( i = 0; i < (int) strlen(g_pszGroupName); i++ )
      if( g_pszGroupName[i] > 32 )   break;
                                                 // ist mindestens ein sinnvolles Zeichen eingegeben ?
   if( i == (int) strlen(g_pszGroupName) )   return (false);

                                                 // Überprüfung, ob GruppenName schon vorhanden
   for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
   {
      if( (lstrcmpi( g_MatOverview.ppszGroupName[i], g_pszGroupName ) == 0) && (i != g_nGroupIndex) )
      {
         flagEqual = 1;
         break;
      }
   }
   if( flagEqual == 1 )
   {
      LoadString( g_hInstance, IDS_TXT_GRPEQUAL1, szString1, 256 );
      LoadString( g_hInstance, IDS_TXT_GRPEQUAL2, szString2, 256 );
      LoadString( g_hInstance, IDS_TXT_GRPEQUAL3, szString3, 256 );
      wsprintf( szString, "%s%s%s\n%s", szString1, g_pszGroupName, szString2, szString3 );
      LoadString( g_hInstance, IDS_TXT_CAPTION_GRPEQUAL, szString2, 256 );
      if( MessageBox( hDlg, szString, szString2, MB_YESNO ) == IDNO )   return (false);
   }
                                                 // hat sich eventuell der Gruppenname geändert ?
   if( lstrcmpi( g_MatOverview.ppszGroupName[g_nGroupIndex], g_pszGroupName ) != 0 )
   {
      tvItem.hItem   = (HTREEITEM) GetHandle_TVItem_Group( hDlg, g_nGroupIndex );
      tvItem.mask    = TVIF_TEXT;
      tvItem.pszText = &szString[0];
      wsprintf( szString, "%s", g_pszGroupName );// Materialgruppenbezeichnung
                                                 // Materialgruppenbezeichnung im TreeView entspr. ändern
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SETITEM, 0, (LPARAM) (const LPTVITEM) &tvItem );
                                                 // Materialgruppenname übernehmen
      wsprintf( g_MatOverview.ppszGroupName[g_nGroupIndex], "%s", g_pszGroupName );

      return (true);
   }
   else   return (false);                        // Gruppenname unverändert -> nicht sichern
}


/********************************************************************/
/*                                                                  */
/* Funktion: Sichern der Daten einer neue definierten               */
/*           Absorber-Materialgruppe in die globalen Datenfelder    */
/*                                                                  */
/********************************************************************/
BOOL SaveNewGroupData( HWND hDlg )
{
   int                 i, flagEqual = 0;
   char                szString[256],
                       szString1[256],
                       szString2[256],
                       szString3[256];           // Textbuffer
   TVINSERTSTRUCT      tvInsert;                 // Struktur zum Anlegen eines TV-Items


                                                 // Materialgruppenname gelöscht ?, nicht speichern
   if( strlen(g_pszGroupName) == 0 )   return (false);

   for( i = 0; i < (int) strlen(g_pszGroupName); i++ )
      if( g_pszGroupName[i] > 32 )   break;
                                                 // ist mindestens ein sinnvolles Zeichen eingegeben ?
   if( i == (int) strlen(g_pszGroupName) )   return (false);

                                                 // Überprüfung, ob GroupName schon vorhanden
   for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
   {
      if( lstrcmpi( g_MatOverview.ppszGroupName[i], g_pszGroupName ) == 0 )
      {
         flagEqual = 1;
         break;
      }
   }
   if( flagEqual == 1 )
   {
      LoadString( g_hInstance, IDS_TXT_GRPEQUAL1, szString1, 256 );
      LoadString( g_hInstance, IDS_TXT_GRPEQUAL2, szString2, 256 );
      LoadString( g_hInstance, IDS_TXT_GRPEQUAL3, szString3, 256 );
      wsprintf( szString, "%s%s%s\n%s", szString1, g_pszGroupName, szString2, szString3 );
      LoadString( g_hInstance, IDS_TXT_CAPTION_GRPEQUAL, szString2, 256 );
      if( MessageBox( hDlg, szString, szString2, MB_YESNO ) == IDYES )
         flagEqual = 0;                          // Es soll trotzdem gepeichert werden
   }
   
   
   if( flagEqual == 0 )                          // GroupName noch nicht vorhanden oder trotzdem speichern
   {
      g_MatOverview.nNoOfGroups++;               // Anzahl der Materialgruppen um 1 erhöhen
                                                 // bezeichnung der Materialgruppe
      wsprintf( g_MatOverview.ppszGroupName[g_nGroupIndex], "%s", g_pszGroupName );

                                                 // TreeView updaten
      tvInsert.hInsertAfter   = TVI_SORT;        // den Eintrag alphabetisch einsortieren
      tvInsert.hParent        = TVI_ROOT;        // Materialgruppen
      tvInsert.item.mask      = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
      tvInsert.item.stateMask = TVIS_BOLD;
      tvInsert.item.state     = TVIS_BOLD;       // fette Schrift ist Kennzeichen für Materialgruppe (!!!)
      tvInsert.item.pszText   = &szString[0];
      tvInsert.item.lParam    = g_nGroupIndex;   // Materialgruppennummer
      wsprintf( szString, "%s", g_pszGroupName );// Materialgruppenbezeichnung
   
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_INSERTITEM,
                          0, (LPARAM) (const LPTVINSERTSTRUCT) &tvInsert );
      return (true);
   }
   else
      return (false);
}


/********************************************************************/
/*                                                                  */
/* Funktion: Sichern der geänderten Daten eines                     */
/*           Absorber-Materials in die globalen Datenfelder         */
/*                                                                  */
/********************************************************************/
BOOL SaveChangedMatData( HWND hDlg )
{
   int       i, flagFound = 0, flagEqual = 0;
   char      szString[256],
             szString1[256],
             szString2[256],
             szString3[256];                     // Textbuffer
   TVITEM    tvItem;                             // Struktur für TreeView-Items

   int nLen = strlen(g_pszMatName);              // Materialname gelöscht ?, kann nicht gesaved werden
   if(nLen > 0)
   {
      for(i=0; i<nLen; i++)
         if(g_pszMatName[i] > 32) break;
                                                 // ist mindestens ein sinnvolles Zeichen eingegeben ?
      if( i == nLen)  nLen = 0;
   }
   if (nLen == 0)
   {
      char str[64];
      POINT pt = {0,0};
      HWND hwnd = GetDlgItem(hDlg, IDC_EDITCREATE2);
      ::SetFocus(hwnd);
      ::ClientToScreen(hwnd, &pt);
      ::LoadString(g_hInstance, IDS_TXT_ERROR_INPUT, str, 64);
      CEtsHelp::CreateErrorWnd(str, pt.x, pt.y);
      return false;
   }
                                                 // Überprüfung, ob MatName schon vorhanden
   for( i = 1; i <= g_MatOverview.nNoOfMaterials; i++ )
   {
      if( (lstrcmpi( g_pMaterial[i].pszMatName, g_pszMatName ) == 0) && (i != g_nMatIndex) )
      {
         flagEqual = 1;
         break;
      }
   }
   if( flagEqual == 1 )
   {
      LoadString( g_hInstance, IDS_TXT_MATEQUAL1, szString1, 256 );
      LoadString( g_hInstance, IDS_TXT_MATEQUAL2, szString2, 256 );
      LoadString( g_hInstance, IDS_TXT_MATEQUAL3, szString3, 256 );
      wsprintf( szString, "%s%s%s\n%s", szString1, g_pszMatName, szString2, szString3 );
      LoadString( g_hInstance, IDS_TXT_CAPTION_MATEQUAL, szString2, 256 );
      if( MessageBox( hDlg, szString, szString2, MB_YESNO ) == IDNO )   return (false);
   }
                                                 // Hat sich die Materialgruppe geändert ?
   if( g_pMaterial[g_nMatIndex].nGroupIndex != g_nGroupIndex )
   {
      int                 nGrIndAlt, nGrIndNeu;
      TVINSERTSTRUCT      tvInsert;              // Struktur zum Anlegen eines TV-Items
      HTREEITEM           hTV_Item;              // Handle auf ein TV-Item einer MatGruppe

      nGrIndAlt = g_pMaterial[g_nMatIndex].nGroupIndex;
      nGrIndNeu = g_nGroupIndex;
                                                 // alten Eintrag im TreeView löschen
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_DELETEITEM,
                          0, (LPARAM) (HTREEITEM) GetHandle_TVItem_Mat( hDlg, g_nMatIndex ) );
                                                 // Material in neue MatGruppe des TreeViews eintragen
      hTV_Item = (HTREEITEM) GetHandle_TVItem_Group( hDlg, g_nGroupIndex );

      tvInsert.hInsertAfter   = TVI_SORT;        // den neuen Eintrag alphabetisch einsortieren
      tvInsert.hParent        = hTV_Item;        // AbsorberMaterial: Kind einer MatGruppe
      tvInsert.item.mask      = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
      tvInsert.item.stateMask = (UINT) -1;
      tvInsert.item.state     = 0;
      tvInsert.item.pszText   = &szString[0];    // Pointer auf Textfeld für Mat-Bezeichnung
      tvInsert.item.lParam    = g_nMatIndex;     // Materialnummer
      wsprintf( szString, "%s", g_pszMatName );  // Materialbezeichnung
   
                                                 // in Baumstruktur einfügen
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_INSERTITEM,
                          0, (LPARAM) (const LPTVINSERTSTRUCT) &tvInsert );

                                                 // alte Zuordnung in der GruppenMatrix löschen
      for( i = 0; i < g_MatOverview.pnNoOfMatInGroup[nGrIndAlt]; i++ )
      {
         if( g_MatOverview.ppnMatIndex_Group[nGrIndAlt][i] == g_nMatIndex )   flagFound = 1;

         if( flagFound == 1 )
         {
            g_MatOverview.ppnMatIndex_Group[nGrIndAlt][i] = 0; // alte Materialnummer löschen
                                                 // das in der Matrix entstandene 'Loch' wieder auffüllen
            if( i < (g_MatOverview.pnNoOfMatInGroup[nGrIndAlt] - 1) )
            {
               g_MatOverview.ppnMatIndex_Group[nGrIndAlt][i] =
               g_MatOverview.ppnMatIndex_Group[nGrIndAlt][i+1];
            }
         }
      }
      g_MatOverview.pnNoOfMatInGroup[nGrIndAlt]--; // Anzahl der Materialien der betr. Gruppe erniedrigen

                                                 // neue Zuordnung in die GruppenMatrix eintragen
      g_MatOverview.ppnMatIndex_Group[nGrIndNeu][g_MatOverview.pnNoOfMatInGroup[nGrIndNeu]] = g_nMatIndex;
      g_MatOverview.pnNoOfMatInGroup[nGrIndNeu]++; // Anzahl der Materialien der betr. Gruppe erhöhen
                                                 // Neue GruppenIndex in Materialdaten eintragen
      g_pMaterial[g_nMatIndex].nGroupIndex = nGrIndNeu;
   }
                                                 // hat sich eventuell der Materialname geändert ?
   else if( lstrcmpi( g_pMaterial[g_nMatIndex].pszMatName, g_pszMatName ) != 0 )
   {
      tvItem.hItem   = (HTREEITEM) GetHandle_TVItem_Mat( hDlg, g_nMatIndex );
      tvItem.mask    = TVIF_TEXT;
      tvItem.pszText = &szString[0];
      wsprintf( szString, "%s", g_pszMatName );  // Materialbezeichnung
                                                 // Materialbezeichnung im TreeView entspr. ändern
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SETITEM, 0, (LPARAM) (const LPTVITEM) &tvItem );
                                                 // neu sortieren
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SORTCHILDREN,
                          0, (LPARAM) (HTREEITEM) GetHandle_TVItem_Group( hDlg, g_nGroupIndex ) );
   }

   GetMatReflDataFromList(hDlg);
                                                 // Materialbeschreibung und Name
   wsprintf( g_pMaterial[g_nMatIndex].pszDescription, "%s", g_pszMatDescr );
   wsprintf( g_pMaterial[g_nMatIndex].pszMatName, "%s", g_pszMatName );
                                                 // Offset für BitMap-Numerierung   
   g_pMaterial[g_nMatIndex].nBM_IndexOffset  = g_nBM_IndexOffset;

   return (true);
}


/********************************************************************/
/*                                                                  */
/* Funktion: Sichern der Daten eines neue definierten               */
/*           Absorber-Materials in die globalen Datenfelder         */
/*                                                                  */
/********************************************************************/
BOOL SaveNewMatData( HWND hDlg )
{
   int                 i, flagEqual = 0;
   char                szString[256],
                       szString1[256],
                       szString2[256],
                       szString3[256];           // Textbuffer
   TVINSERTSTRUCT      tvInsert;                 // Struktur zum Anlegen eines TV-Items
   HTREEITEM           hTV_Item;                 // Handle auf ein TV-Item einer MatGruppe


   int nLen = strlen(g_pszMatName);              // Materialname gelöscht ?, kann nicht gesaved werden
   if(nLen > 0)
   {
      for(i=0; i<nLen; i++)
         if(g_pszMatName[i] > 32) break;
                                                 // ist mindestens ein sinnvolles Zeichen eingegeben ?
      if( i == nLen)  nLen = 0;
   }
   if (nLen == 0)
   {
      char str[64];
      POINT pt = {0,0};
      HWND hwnd = GetDlgItem(hDlg, IDC_EDITCREATE2);
      ::SetFocus(hwnd);
      ::ClientToScreen(hwnd, &pt);
      ::LoadString(g_hInstance, IDS_TXT_ERROR_INPUT, str, 64);
      CEtsHelp::CreateErrorWnd(str, pt.x, pt.y);
      return false;
   }
                                                 // Überprüfung, ob MatName schon vorhanden
   for( i = 1; i <= g_MatOverview.nNoOfMaterials; i++ )
   {
      if( lstrcmpi( g_pMaterial[i].pszMatName, g_pszMatName ) == 0 )
      {
         flagEqual = 1;
         break;
      }
   }
   if( flagEqual == 1 )
   {
      LoadString( g_hInstance, IDS_TXT_MATEQUAL1, szString1, 256 );
      LoadString( g_hInstance, IDS_TXT_MATEQUAL2, szString2, 256 );
      LoadString( g_hInstance, IDS_TXT_MATEQUAL3, szString3, 256 );
      wsprintf( szString, "%s%s%s\n%s", szString1, g_pszMatName, szString2, szString3 );
      LoadString( g_hInstance, IDS_TXT_CAPTION_MATEQUAL, szString2, 256 );
      if( MessageBox( hDlg, szString, szString2, MB_YESNO ) == IDYES )
         flagEqual = 0;                          // Es soll trotzdem gepeichert werden
   }
   
   
   if( flagEqual == 0 )                          // MatName noch nicht vorhanden oder trotzdem speichern
   {
      g_MatOverview.nNoOfMaterials++;            // Anzahl der Materialien um 1 erhöhen
                                                 // MatIndex in GroupMatrix eintragen (ZeilenIndex ab 0 !!)
      g_MatOverview.ppnMatIndex_Group[g_nGroupIndex][g_MatOverview.pnNoOfMatInGroup[g_nGroupIndex]] =
                                                                                                g_nMatIndex;
      g_MatOverview.pnNoOfMatInGroup[g_nGroupIndex]++; // Anzahl der Materialien der betr. Gruppe erhöhen

                                                 // Neue Materialdaten eintragen
      g_pMaterial[g_nMatIndex].nGroupIndex = g_nGroupIndex; // zugehöriger GruppenIndex

      GetMatReflDataFromList(hDlg);
                                                 // Materialbeschreibung und Name
      wsprintf( g_pMaterial[g_nMatIndex].pszDescription, "%s", g_pszMatDescr );
      wsprintf( g_pMaterial[g_nMatIndex].pszMatName, "%s", g_pszMatName );
                                                 // Offset für BitMap-Numerierung   
      g_pMaterial[g_nMatIndex].nBM_IndexOffset  = g_nBM_IndexOffset;

                                                 // TreeView updaten
      hTV_Item = (HTREEITEM) GetHandle_TVItem_Group( hDlg, g_nGroupIndex );

      tvInsert.hInsertAfter   = TVI_SORT;        // den neuen Eintrag alphabetisch einsortieren
      tvInsert.hParent        = hTV_Item;        // AbsorberMaterial: Kind einer MatGruppe
      tvInsert.item.mask      = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
      tvInsert.item.stateMask = (UINT) -1;
      tvInsert.item.state     = 0;
      tvInsert.item.pszText   = &szString[0];    // Pointer auf Textfeld für Mat-Bezeichnung
      tvInsert.item.lParam    = g_nMatIndex;     // Materialnummer
      wsprintf( szString, "%s", g_pszMatName );  // Materialbezeichnung
   
                                                 // in Baumstruktur einfügen
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_INSERTITEM,
                          0, (LPARAM) (const LPTVINSERTSTRUCT) &tvInsert );

      return (true);
   }
   else
      return (false);
}


/********************************************************************/
/*                                                                  */
/*    Funktion: Vorbelegung der Eingabedaten für DIALOGCREATE       */
/*                                                                  */
/*    flag = 1: WM_INITDIALOG                                       */
/*           2: Eingabe neues Material (RadioButton1)               */
/*           3: Ändern vorhandener Materialdaten (RadioButton2)     */
/*                                                                  */
/*    ( Wenn nMatIndex == 0 dann erfolgte der Aufruf durch WM_INIT  */
/*      oder Auslösen der Radiobuttons; bei != 0 durch eine Auswahl */
/*      im TreeView der Materialien/Materialgruppen.)               */
/*                                                                  */
/********************************************************************/
BOOL InitMatData1( HWND hDlg, int flag, int nMatIndex )
{
   int                 i, j;
   unsigned int        nStyle;
   const  int          nwidth = 32, nheight = 32;
   HBITMAP             hBitMapLoad, hBitMapCtrl; // Handles auf 32x32 Bitmap (für Load und STM_SETIMAGE)
   LVITEM              lvItem_BM, lvItem_REP;    // Strukturen für ListView-Items
   LVCOLUMN            lvColumn_REP;             // Struktur für Spaltenüberschrift
   TVINSERTSTRUCT      tvInsert;                 // Struktur zum Anlegen eines TV-Items
   HTREEITEM           hTV_Item;                 // Handle auf ein TV-Item einer MatGruppe/eines Materials
   char                szString[256];            // Textbuffer


   if( flag < 3 )                                // Eingabe: neues Material
   {
      g_nMatIndex = g_MatOverview.nNoOfMaterials+1; // neuer Mat-Index
      if( g_nMatIndex > g_nNoOfMaterialsEx )     // evtl. Datenfelder erweitern
      {
         if( !(AllocDaten( 1 )) )   return (false); // dynamische Datenfelder erweitern
      }

      g_nGroupIndex = 1;                         // die 1. Gruppe ("Allgemein") ist die Default-Gruppe
      wsprintf( g_pszMatName, "" );              // noch kein Materialname eingegeben
      wsprintf( g_pszMatDescr, "" );             // noch keine Materialbeschreibung eingegeben
                                                 // aktueller Gruppenname ("Allgemein")
      wsprintf( g_pszGroupName, "%s", g_MatOverview.ppszGroupName[g_nGroupIndex] );
      g_nBM_IndexOffset  = 0;                    // 1. BitMap
      g_dReflGradLowFreq = 0.9;                  // Vorbelegung mit 10% Absoptionsgrad
      g_pMaterial[g_nMatIndex].dReflGradLowFreq = g_dReflGradLowFreq;
      g_nNoOfFreq        = 0;                    // noch keine Frequenzabhängigkeit eingegeben
      g_pMaterial[g_nMatIndex].nNoOfFreq = 0;
   }
   else                                          // Ändern Materialdaten
   {
      if( nMatIndex == 0 )                       // keine direkte Auswahl
      {
                                                 // Vorlegen: den nächsten (zyklisch) Eintrags im TreeView
         g_nMatIndex = GetMatIndex_for_NextTVItem( hDlg, g_nMatIndex );
      }
      else   g_nMatIndex = nMatIndex;            // direkte Auswahl über TreeView
                                                 // die zugehörige Material-Gruppe, Name und Beschreibung
      g_nGroupIndex = g_pMaterial[g_nMatIndex].nGroupIndex;
      wsprintf( g_pszMatName, "%s", g_pMaterial[g_nMatIndex].pszMatName );
      wsprintf( g_pszMatDescr, "%s", g_pMaterial[g_nMatIndex].pszDescription );
                                                 // aktueller Gruppenname
      wsprintf( g_pszGroupName, "%s", g_MatOverview.ppszGroupName[g_nGroupIndex] );
                                                 // zugehöriger Reflektionsgrad, Frequenzabhängigkeit
      g_nBM_IndexOffset  = g_pMaterial[g_nMatIndex].nBM_IndexOffset;
      g_dReflGradLowFreq = g_pMaterial[g_nMatIndex].dReflGradLowFreq;
      g_nNoOfFreq = g_pMaterial[g_nMatIndex].nNoOfFreq;
      for( i = 0; i < CARAMAT_MAXNOOFFREQ; i++ ) // ReflGrade entspr. der Standardfrequenzwerte zuweisen
      {
         g_dReflGradFreq[i] = -10;               // Vorbelegen, falls der entspr. Freq.Wert nicht vorhanden
         for( j = 0; j < g_nNoOfFreq; j++ )
         {
            if( fabs(g_dFrequency[i] - g_pMaterial[g_nMatIndex].dFrequency[j]) < 1E-3 )
            {
               g_dReflGradFreq[i] = g_pMaterial[g_nMatIndex].dReflGradFreq[j];
               break;                            // Abbruch der j-Schleife
            }
         }
      }
   }
   CalcFrequResp();
   InvalidateRect(GetDlgItem(hDlg, IDC_MAT_ABSPT_FRQ_RESP9), NULL, true);

                                                 // die zugehörige Material-Gruppe, Name und Beschreibung
                                                 // MaterialIndex anzeigen
   wsprintf( szString, "%d", g_nMatIndex );
   SetDlgItemText( hDlg, IDC_TEXTCREATE2_1, szString );
                                                 // Materialname und Materialbeschreibung setzen ("")
   SetDlgItemText( hDlg, IDC_EDITCREATE2, g_pszMatName );
   SetDlgItemText( hDlg, IDC_EDITCREATE3, g_pszMatDescr );
                                                 // Absorptionsgrad bei tiefen Frequenzen vorbelegen (10%)
   wsprintf( szString, "%d", ((int) (200.0*(1 - g_dReflGradLowFreq)+1))>>1 );
   SetDlgItemText( hDlg, IDC_EDITCREATE5, szString );
                                                 // Index der zugehörigen Material-Gruppe anzeigen
   wsprintf( szString, "%d", g_nGroupIndex );
   SetDlgItemText( hDlg, IDC_TEXTCREATE2_7, szString );
                                                 // Gruppenname setzen ("Allgemein" oder ...)
   SetDlgItemText( hDlg, IDC_EDITCREATE8, g_pszGroupName );
   
   if( flag == 1 )                               // bei Erstinitialisierung
   {
                                                 // RadioButtons für Material: 'Check' setzen bei "Neu"
	   SendDlgItemMessage( hDlg, IDC_RADIOCREATE1, BM_SETCHECK, 1, 0 );
	   SendDlgItemMessage( hDlg, IDC_RADIOCREATE2, BM_SETCHECK, 0, 0 );
                                                 // RadioButtons für Mat-Gruppe: keiner ist gecheckt
	   SendDlgItemMessage( hDlg, IDC_RADIOCREATE3, BM_SETCHECK, 0, 0 );
	   SendDlgItemMessage( hDlg, IDC_RADIOCREATE4, BM_SETCHECK, 0, 0 );
                                                 // Längenbegrenzung der Textfelder
      SendDlgItemMessage( hDlg, IDC_EDITCREATE2, EM_LIMITTEXT, CARAMAT_MATNAMELENGTH-1, 0 );
		SendDlgItemMessage( hDlg, IDC_EDITCREATE3, EM_LIMITTEXT, CARAMAT_MATDESCRLENGTH-1, 0 );
		SendDlgItemMessage( hDlg, IDC_EDITCREATE5, EM_LIMITTEXT, 6, 0 );
		SendDlgItemMessage( hDlg, IDC_EDITCREATE8, EM_LIMITTEXT, CARAMAT_GROUPNAMELENGTH-1, 0 );
   }
   else                                          // über RadioButton ausgewählt
   {
                                                 // eigene Controls öffnen
      EnableWindow( GetDlgItem( hDlg, IDC_EDITCREATE2 ), true );
      EnableWindow( GetDlgItem( hDlg, IDC_EDITCREATE3 ), true );
      EnableWindow( GetDlgItem( hDlg, IDC_EDITCREATE5 ), true );
      EnableWindow( GetDlgItem( hDlg, IDC_LISTCREATEREP ), true );
      EnableWindow( GetDlgItem( hDlg, IDC_MAT_ABSPT_FRQ_RESP9 ), true );
      EnableWindow( GetDlgItem( hDlg, IDC_LISTCREATEBM ), true );
   }

                                                 // andere Controls (Materialgruppen-Bezeichnung) sperren
                                                 // (mit READONLY, damit Text besser lesbar)
   SendDlgItemMessage( hDlg, IDC_EDITCREATE8, EM_SETREADONLY, true, 0 );
                                                 // WindowStyle von IDC_EDITCREATE8: kein TABSTOP
   nStyle = GetWindowLong( GetDlgItem( hDlg, IDC_EDITCREATE8 ), GWL_STYLE );
   if( nStyle & WS_TABSTOP )   nStyle ^= WS_TABSTOP;
   SetWindowLong( GetDlgItem( hDlg, IDC_EDITCREATE8 ), GWL_STYLE, nStyle );

   hBitMapLoad = NULL;

   if( flag == 1 )                               // bei Erstinitialisierung ListView-BitMap initialisieren
   {
      HIMAGELIST hImageList;

      hImageList = ImageList_Create( nwidth, nheight, ILC_COLORDDB, CARAMAT_NOOFBITMAPS, 0 );

      for( i = 0; i < CARAMAT_NOOFBITMAPS; i++ )
      {
                                                 // BitMap-Objekte für ImageList löschen
         if( hBitMapLoad != NULL )   DeleteObject( hBitMapLoad );
                                                 // BitMaps laden, IDB_BITMAP1/2/3.. müssen fortlaufend sein
         hBitMapLoad = (HBITMAP) LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1+i),
                                            IMAGE_BITMAP, nwidth, nheight, LR_DEFAULTCOLOR );
                                                 // BitMap in Image-Liste einfügen
         ImageList_Add( hImageList, hBitMapLoad, NULL );
      }
		SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_SETIMAGELIST,
                          (WPARAM) LVSIL_NORMAL, (LPARAM) (HIMAGELIST) hImageList );

      DWORD dwI = SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_SETICONSPACING, 0, MAKELONG(48, 48));

/*
      SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_SETIMAGELIST,
                          (WPARAM) LVSIL_SMALL, (LPARAM) (HIMAGELIST) hImageList );
*/
                                                 // ItemListe für Oberflächenstruktur (BitMaps) erstellen
      SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_SETITEMCOUNT,
                          (WPARAM) CARAMAT_NOOFBITMAPS, (LPARAM) 0 );

      lvItem_BM.mask      = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
      lvItem_BM.iSubItem  = 0;
      lvItem_BM.pszText   = &szString[0];
      lvItem_BM.stateMask = (UINT) -1;           // alle Zustände frei setzbar
      for( i = 0; i < CARAMAT_NOOFBITMAPS; i++ )
      {
         wsprintf( szString, "%d", i+1 );
            
         lvItem_BM.iItem     = i;
         lvItem_BM.iImage    = i;
         lvItem_BM.lParam    = i;                // fester eigener Index (iItem kann sich ändern)
         if( i == g_nBM_IndexOffset )
            lvItem_BM.state = LVIS_SELECTED | LVIS_FOCUSED;
         else
            lvItem_BM.state = 0;

			SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_INSERTITEM,
                             0, (LPARAM) (const LPLVITEM) &lvItem_BM );
      }
   }
   else                                          // über RadioButton ausgewählt
   {
                                                 // Markierung in ImageList setzen: entspr. BM_IndexOffset
      lvItem_BM.mask      = LVIF_STATE | LVIF_PARAM;
      lvItem_BM.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
      lvItem_BM.iSubItem = 0;
      for( i = 0; i < CARAMAT_NOOFBITMAPS; i++ )
      {
         lvItem_BM.iItem = i;
			SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_GETITEM,
                             0, (LPARAM) (const LPLVITEM) &lvItem_BM );
         if( lvItem_BM.lParam == g_nBM_IndexOffset )
         {
            lvItem_BM.state = LVIS_SELECTED | LVIS_FOCUSED;
            SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_ENSUREVISIBLE, (WPARAM) i, (LPARAM) true );
         }
         else
            lvItem_BM.state = 0;

			SendDlgItemMessage( hDlg, IDC_LISTCREATEBM, LVM_SETITEM,
                             0, (LPARAM) (const LPLVITEM) &lvItem_BM );
      }
   }

                                                 // letztes BitMap-Objekt für ImageList löschen
   if( hBitMapLoad != NULL )   DeleteObject( hBitMapLoad );
                                                 // entspr. BM als Oberflächenstruktur
   hBitMapLoad = (HBITMAP) LoadImage( g_hInstance,
                                      MAKEINTRESOURCE(IDB_BITMAP1+g_nBM_IndexOffset),
                                      IMAGE_BITMAP, nwidth, nheight, LR_DEFAULTCOLOR );
	hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDlg, IDC_BILDCREATE4, STM_SETIMAGE,
                                               (WPARAM) IMAGE_BITMAP, (LPARAM) (HANDLE) hBitMapLoad );
   if( hBitMapCtrl != NULL )   DeleteObject( hBitMapCtrl ); // vorherige Bitmap des Controls löschen


   if( flag == 1 )                               // bei Erstinitialisierung ListView-Report initialisieren
   {
                                                 // Spalten des ListView-Reports: Überschriften etc.
      wsprintf( szString, "Freq.[Hz]" );         // 1. Spalte = Hauptspalte (??)
      lvColumn_REP.mask     = LVCF_FMT | LVCF_ORDER | LVCF_TEXT | LVCF_WIDTH;
      lvColumn_REP.fmt      = LVCFMT_LEFT;
      lvColumn_REP.cx       = 57;
      lvColumn_REP.pszText  = &szString[0];
      lvColumn_REP.iOrder   = 0;                 // StartColumn = Hauptitem, SubItem = 0
		SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_INSERTCOLUMN,
                          0, (LPARAM) (const LPLVCOLUMN) &lvColumn_REP );

      wsprintf( szString, "alpha [%%]" );        // 2. Spalte = SubItem
      lvColumn_REP.mask    |= LVCF_SUBITEM; 
      lvColumn_REP.iSubItem = 1;
      lvColumn_REP.iOrder   = 1;
		SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_INSERTCOLUMN,
                          0, (LPARAM) (const LPLVCOLUMN) &lvColumn_REP );

                                                 // ItemListe für Freq.Abhängkt. der Absorp.Grade erstellen
      SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_SETITEMCOUNT,
                          (WPARAM) CARAMAT_MAXNOOFFREQ, (LPARAM) 0 );

      wsprintf( szString, "---" );
      lvItem_REP.pszText   = &szString[0];
      lvItem_REP.mask      = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
      lvItem_REP.stateMask = (UINT) -1;          // alle Zustände frei setzbar
      lvItem_REP.iSubItem  = 0;
      for( i = 0; i < CARAMAT_MAXNOOFFREQ; i++ ) // Items (nur diese sind editierbar)
      {
//         g_dFrequency[i]    = 5*pow(2, 1.0*i/9.0 ); // 118 Frequenzwerte von 5-40960 Hz, 1/3 Terz
         g_dReflGradFreq[i] = -10;
            
         lvItem_REP.iItem   = i;
         lvItem_REP.lParam  = i;                 // fester eigener Index (iItem kann sich ändern)
         if( i == 0 )
            lvItem_REP.state = LVIS_SELECTED | LVIS_FOCUSED;
         else
            lvItem_REP.state = 0;

			SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_INSERTITEM,
                             0, (LPARAM) (const LPLVITEM) &lvItem_REP );
      }
      
      lvItem_REP.mask      = LVIF_TEXT;          // für SubItems nur Text setzbar
      lvItem_REP.stateMask = 0;
      lvItem_REP.iSubItem  = 1;
      for( i = 0; i < CARAMAT_MAXNOOFFREQ; i++ ) // SubItems (nicht editierbar)
      {
         if( g_dFrequency[i] < 100.0 )
            wsprintf( szString, "%d.%d", (int) g_dFrequency[i],
                                         (int) ((g_dFrequency[i] - (int) g_dFrequency[i])*10.0) );
         else
            wsprintf( szString, "%d", (int) g_dFrequency[i] );

         lvItem_REP.iItem = i;
            
			SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_SETITEM,
                             0, (LPARAM) (const LPLVITEM) &lvItem_REP );
      }
   }
   else                                          // über RadioButton ausgewählt
   {
                                                 // Freq.Tabelle mit alpha (%)
      lvItem_REP.pszText = &szString[0];
      for( i = 0; i < CARAMAT_MAXNOOFFREQ; i++ ) // Items (nur die editierbaren)
      {
         if( flag == 2 )   g_dReflGradFreq[i] = -10; // Eingabe: neues Material
         if( g_dReflGradFreq[i] == -10 )
            wsprintf( szString, "---" );
         else                                    // Ändern Materialdaten
            wsprintf( szString, "%d", ((int) (200.0*(1 - g_dReflGradFreq[i])+1))>>1 );
            
         lvItem_REP.mask      = LVIF_TEXT | LVIF_STATE;
         lvItem_REP.stateMask = (UINT) -1;       // alle Zustände frei setzbar
         lvItem_REP.iItem     = i;
         lvItem_REP.iSubItem  = 0;
         if( i == 0 )
         {
            lvItem_REP.state = LVIS_SELECTED | LVIS_FOCUSED;
            SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_ENSUREVISIBLE, (WPARAM) i, (LPARAM) true );
         }
         else
            lvItem_REP.state = 0;

			SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_SETITEM,
                             0, (LPARAM) (const LPLVITEM) &lvItem_REP );
      }
      g_nSelListItem = 0;
   }

   if( flag == 1 )                               // bei Erstinitialisierung TreeView initialisieren
   {
      tvInsert.hInsertAfter   = TVI_SORT;        // alle Einträge alphabetisch einsortieren
      tvInsert.item.mask      = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
      tvInsert.item.stateMask = TVIS_BOLD;
      tvInsert.item.pszText   = &szString[0];
   
      for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
      {
         tvInsert.hParent     = TVI_ROOT;        // Materialgruppen
         wsprintf( szString, "%s", g_MatOverview.ppszGroupName[i] ); // Gruppenname
         tvInsert.item.lParam = i;               // Materialgruppennummer
         tvInsert.item.state  = TVIS_BOLD;       // fette Schrift ist Kennzeichen für Materialgruppe (!!!)
 
         hTV_Item = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_INSERTITEM,
                                                    0, (LPARAM) (const LPTVINSERTSTRUCT) &tvInsert );

         for( j = 0; j < g_MatOverview.pnNoOfMatInGroup[i]; j++ )
         {
            tvInsert.hParent     = hTV_Item;     // AbsorberMaterial
                                                 // Kurzbezeichnung
            wsprintf( szString, "%s", g_pMaterial[g_MatOverview.ppnMatIndex_Group[i][j]].pszMatName );
            tvInsert.item.lParam = g_MatOverview.ppnMatIndex_Group[i][j]; // Materialnummer
            tvInsert.item.state  = 0;

            SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_INSERTITEM,
                                0, (LPARAM) (const LPTVINSERTSTRUCT) &tvInsert );
         }
      }
                                                 // 1. MatGruppe selektieren und sichtbar machen
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_CARET,
                          (LPARAM) (HTREEITEM) GetHandle_TVItem_Group( hDlg, g_nGroupIndex ) );
      SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_FIRSTVISIBLE,
                          (LPARAM) (HTREEITEM) GetHandle_TVItem_Group( hDlg, g_nGroupIndex ) );
   }
   else                                          // Neues Material definieren oder Material ändern
   {
      if( flag == 2 )                            // Neues Material definieren
         CollapseTVParents( hDlg, 1 );           // alle Parents schließen, erste MatGruppe selektieren
      else                                       // Material ändern
      {
                                                 // TV-Item-Handle zum betr. Material holen
         hTV_Item = GetHandle_TVItem_Mat( hDlg, g_nMatIndex );

                                                 // aktuellen TV-Handle selektieren, nur wenn über
                                                 // RadioButtons ausgelöst (nMatIndex = 0), bei Direktwahl
                                                 // aus dem TreeView ist die Selektierung schon erfolgt !!
                                                 // Ist der TreeView schon richtig selektiert, erfolgt
                                                 // keine erneute Selektion mehr.
         if( nMatIndex == 0 )
         {
            if(hTV_Item != (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREECREATE,
                                                           TVM_GETNEXTITEM, TVGN_CARET, 0 ) )
            {
               SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_CARET,
                                   (LPARAM) (HTREEITEM) hTV_Item );
               SendDlgItemMessage( hDlg, IDC_TREECREATE, TVM_SELECTITEM, TVGN_FIRSTVISIBLE,
                                   (LPARAM) (HTREEITEM) hTV_Item );
            }
         }
      }
   }

   return (true);
}


/********************************************************************/
/*                                                                  */
/*    Funktion: Vorbelegung der Eingabedaten für DIALOGCREATE       */
/*                                                                  */
/*    flag = 2: Eingabe neue MaterialGruppe (RadioButton3)          */
/*           3: Ändern von Materialgruppendaten (RadioButton4)      */
/*                                                                  */
/********************************************************************/
BOOL InitGroupData1( HWND hDlg, int flag )
{
   char                szString[256];
   unsigned int        nStyle;
   
   
   if( flag == 2 )                               // Eingabe einer neuen Materialgruppe
   {
      g_nGroupIndex = g_MatOverview.nNoOfGroups + 1; // neuer Gruppenindex
      if( g_nGroupIndex > g_nNoOfGroupsEx )      // evtl. Datenfelder erweitern
      {
         if( !(AllocDaten( 1 )) )   return (false); // dynamische Datenfelder erweitern
      }
                                                 // neuer Gruppenname (Vorschlag)
      wsprintf( g_pszGroupName, "" );
   }
   else                                          // Ändern einer vorhandenen Materialgruppe
   {
                                                 // Vorlegen: den nächsten (zyklisch) Eintrags im TreeView
      g_nGroupIndex = GetMatGroupIndex_for_NextTVItem( hDlg, g_nGroupIndex );
                                                 // zugehöriger Gruppenname
      wsprintf( g_pszGroupName, "%s", g_MatOverview.ppszGroupName[g_nGroupIndex] );
   }
                                                 // Index der zugehörigen Material-Gruppe anzeigen
   wsprintf( szString, "%d", g_nGroupIndex );
   SetDlgItemText( hDlg, IDC_TEXTCREATE2_7, szString );
                                                 // Gruppenname setzen
   SetDlgItemText( hDlg, IDC_EDITCREATE8, g_pszGroupName );

                                                 // eigenen Control öffnen
   SendDlgItemMessage( hDlg, IDC_EDITCREATE8, EM_SETREADONLY, false, 0 );
                                                 // andere Controls sperren
   EnableWindow( GetDlgItem( hDlg, IDC_EDITCREATE2 ), false );
   EnableWindow( GetDlgItem( hDlg, IDC_EDITCREATE3 ), false );
   EnableWindow( GetDlgItem( hDlg, IDC_EDITCREATE5 ), false);
   EnableWindow( GetDlgItem( hDlg, IDC_LISTCREATEREP ), false );
   EnableWindow( GetDlgItem( hDlg, IDC_MAT_ABSPT_FRQ_RESP9 ), false );
   EnableWindow( GetDlgItem( hDlg, IDC_LISTCREATEBM ), false );
                                                 // WindowStyle von IDC_EDITCREATE8: mit TABSTOP
   nStyle = GetWindowLong( GetDlgItem( hDlg, IDC_EDITCREATE8 ), GWL_STYLE );
   SetWindowLong( GetDlgItem( hDlg, IDC_EDITCREATE8 ),
                              GWL_STYLE, (nStyle | WS_TABSTOP) );

   if( flag == 2 ) 
      CollapseTVParents( hDlg, 0 );              // alle Parents schließen, nichts selektieren
   else
      CollapseTVParents( hDlg, g_nGroupIndex );  // alle Parents schließen, MatGruppe markieren

   return (true);
}


/********************************************************************/
/*                                                                  */
/*      Funktion zur Prüfung auf '&': '&' wird verdoppelt           */
/* (Achtung: szNewString muß doppelt so lang wie szOrgString sein)  */
/*                                                                  */
/********************************************************************/
void CheckStringForAND( char* szOrgString, char* szNewString )
{
   unsigned int i, j = 0;
   for( i = 0; i < strlen(szOrgString); i++ )
   {
      szNewString[j] = szOrgString[i];
      if( szOrgString[i] == '&' )
      {
         j++;
         szNewString[j] = '&';
      }
      j++;
   }
   szNewString[j] = 0;
}


/********************************************************************/
/*                                                                  */
/*                Call-Back-Funktion DIALOGCREATE                   */
/*Erstellung/Neudefinition eines Datensatzes für Absorbermaterialien*/
/*                                                                  */
/********************************************************************/
BOOL CALLBACK DialogFunc1( HWND hDialog1,
						         UINT message,
						         WPARAM wParam,
						         LPARAM lParam )
{
   const  int          nwidth = 32, nheight = 32;
   HBITMAP             hBitMapLoad, hBitMapCtrl; // Handles auf 32x32 Bitmap (für Load und STM_SETIMAGE)
   LPNMLISTVIEW        pNM_ListView;             // Pointer auf NMLISTVIEW-Struktur
   LPNMTREEVIEW        pNM_TreeView;             // Pointer auf NMTREEVIEW-Struktur
   NMLVDISPINFO        *pNM_DispInfo;            // Struktur mit LVITEM-Struktur
   int	              helpId;
   HICON               hIcon = NULL;
	LPHELPINFO          lphi;
	char	              szString[768];            // Hilfszeichenfolge
   static int          nFlagState;               // Zustandsflag: 1 = Mat_New,   2 = Mat_Change,
                                                 //               3 = Group_New, 4 = Group_Change

	switch( message )
	{
      case WM_DRAWITEM:
      {
         DRAWITEMSTRUCT *pDI = (DRAWITEMSTRUCT*) lParam;
         if ((pDI->CtlType == ODT_STATIC) && (pDI->CtlID == IDC_MAT_ABSPT_FRQ_RESP9))
         {
            return OnDrawFrqResponse(pDI);
         }
      } break;

      case WM_HELP:
			lphi = (LPHELPINFO) lParam;
         if( lphi->iCtrlId < 100 )   return (0);
         helpId = lphi->iCtrlId - 100*( (int) (lphi->iCtrlId/100) );
         if( LoadString( g_hInstance, helpId, szString, sizeof(szString) ) )
         {
            RECT   rect;

            GetWindowRect(GetDlgItem(hDialog1, lphi->iCtrlId), &rect);
            if( PtInRect(&rect, lphi->MousePos) )
               CEtsHelp::CreateContextWnd( szString, lphi->MousePos.x, lphi->MousePos.y );
            else
               CEtsHelp::CreateContextWnd( szString,
                                            rect.left + (rect.right-rect.left)/2,
                                            rect.top  + (rect.bottom-rect.top)/2);
         }
         return (1);

      case WM_TIMER:                             // Bereichsfehler bei alpha-Eingabe in ListView_REP oder Editfeld
         if( (wParam == 0x1234567) || (wParam == 0x1234568) ) // zusätzliche kennung
         {
            RECT  rc;

            MessageBeep( MB_ICONEXCLAMATION );
            GetWindowRect( GetDlgItem( hDialog1, IDC_LISTCREATEREP ), &rc );
            if( wParam == 0x1234567 )            // falsche Eingabe im ListView (>500%)
            {
               if( LoadString( g_hInstance, IDS_ERROR_ALPHA, szString, sizeof(szString) ) )
                  CEtsHelp::CreateErrorWnd( szString, rc.left+8, rc.top-30 );
               KillTimer(hDialog1,0x1234567);
            }
            else                                 // falsche Eingabe im Editfeld (> 30%)
            {
               if( LoadString( g_hInstance, IDS_ERROR_ALPHA0, szString, sizeof(szString) ) )
                  CEtsHelp::CreateErrorWnd( szString, rc.left+8, rc.top-30 );
               KillTimer(hDialog1,0x1234568);
            }
         }
         break;
      case WM_NOTIFY:
         switch( LOWORD(wParam) )
         {
            case IDC_LISTCREATEBM:               // ListView meldet sich
               switch( ((NMHDR *) lParam)->code )
               {
                  case NM_SETFOCUS:              // den selektierten Eintrag sichtbar machen
                     SendDlgItemMessage( hDialog1, IDC_LISTCREATEBM, LVM_ENSUREVISIBLE,
                                         (WPARAM) SendDlgItemMessage( hDialog1, IDC_LISTCREATEBM,
                                                                      LVM_GETNEXTITEM,
                                                                      (WPARAM) (UINT) -1,
                                                                      MAKELPARAM((UINT) LVNI_SELECTED, 0) ),
                                         (LPARAM) true );
                     break;

                  case LVN_ITEMCHANGED:
                     if( nFlagState < 3 )        // Zustand: Material Neu/Ändern, sonst keine Bild-Auswahl
                     {
                        pNM_ListView = (LPNMLISTVIEW) lParam;
                                                 // hat sich der Zustand eines ListView-Items geändert ?
                        if( pNM_ListView->uChanged & LVIF_STATE )
                        {
                           if( (pNM_ListView->uNewState & LVIS_FOCUSED) &&
                               (pNM_ListView->uNewState & LVIS_SELECTED) )
                           {
                              g_nBM_IndexOffset = pNM_ListView->lParam; // iItem mit Index lParam sel.+foc.
                              hBitMapLoad = (HBITMAP) LoadImage( g_hInstance,
                                                             MAKEINTRESOURCE(IDB_BITMAP1+g_nBM_IndexOffset),
                                                             IMAGE_BITMAP, nwidth, nheight,
                                                             LR_DEFAULTCOLOR );
	                           hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDialog1, IDC_BILDCREATE4,
                                                                          STM_SETIMAGE,
                                                                          (WPARAM) IMAGE_BITMAP,
                                                                          (LPARAM) (HANDLE) hBitMapLoad );
                                                 // vorherige Bitmap des Controls löschen
                              if( hBitMapCtrl != NULL )   DeleteObject( hBitMapCtrl );

                              if( g_nFlagRadioChanged ) // keine wirkliche Änderung, nur auf Radios etc. gedrückt
                                 g_nFlagRadioChanged = 0;
                              else
                                 EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), true ); // Übernehmen einschalten
                           }
                        }
                     }
                     break;
               }
               break;

            case IDC_LISTCREATEREP:              // ListView-Report meldet sich (Freq.Abhängkt. der alpha)
               switch( ((NMHDR *) lParam)->code )
               {
                  case NM_SETFOCUS:              // den selektierten Eintrag sichtbar machen
                     SendDlgItemMessage( hDialog1, IDC_LISTCREATEREP, LVM_ENSUREVISIBLE,
                                         (WPARAM) SendDlgItemMessage( hDialog1, IDC_LISTCREATEREP,LVM_GETNEXTITEM, (WPARAM) (UINT) -1, MAKELPARAM((UINT) LVNI_SELECTED, 0) ),
                                         (LPARAM) true );
                     break;

                  case LVN_BEGINLABELEDIT:
                     if( nFlagState < 3 )        // Zustand: Material Neu/Ändern, sonst keine Editierung
                     {
                        unsigned int   nStyle;
                        HWND           hEdit;
                                                 // Handle des (dynamischen) EditControls, nur Zahlen
                        hEdit   = ListView_GetEditControl( GetDlgItem( hDialog1, IDC_LISTCREATEREP ) );
		                  SendMessage( hEdit, EM_LIMITTEXT, 6, 0 );  // Längenbegrenzung
                        nStyle  = GetWindowLong( hEdit, GWL_STYLE );
                        nStyle |= ES_NUMBER;
                        SetWindowLong( hEdit, GWL_STYLE, nStyle ); // nur Zifferneingabe möglich
                                                 // alle Tastendrücke an EditControl
                        g_lpOldEditProc = (WNDPROC) SetWindowLong( hEdit, GWL_WNDPROC,
                                                                   (LONG) EditSubclass );

                        return (0);              // Editieren erlaubt
                     }
                     else   return (1);          // Editieren nicht erlaubt

                  case LVN_ITEMCHANGED:
                  {
                     NMLISTVIEW *plv = (NMLISTVIEW*)lParam;
                     g_nSelListItem =  plv->iItem;
                     InvalidateRect(GetDlgItem(hDialog1, IDC_MAT_ABSPT_FRQ_RESP9), NULL, true);
                  } break;
                  case LVN_ENDLABELEDIT:
                     if( nFlagState < 3 )        // Zustand: Material Neu/Ändern, sonst keine Editierung
                     {
                        int   dummyint, nFlagOK = 1;

                        pNM_DispInfo = (NMLVDISPINFO *) lParam;

                        if( (pNM_DispInfo->item).pszText != NULL )
                        {
                                                 // Textstring verwertbar ?
                           if( strlen((pNM_DispInfo->item).pszText) == 0 )   nFlagOK = 0;
                           for( dummyint = 0;
                                (dummyint < (int) strlen((pNM_DispInfo->item).pszText)) && (nFlagOK == 1);
                                dummyint++ )
                           {
                              if( (pNM_DispInfo->item).pszText[dummyint] == '-' )   nFlagOK = 0;
                           }
                           
                           if( nFlagOK == 1 )
                           {
                                                 // falscher Wertebereich ?
                              dummyint = atoi( (pNM_DispInfo->item).pszText );
                              if( (dummyint < 0) || (dummyint > 500) ) // jetzt auch zugelassen (6.8.99)
                              {
                                 SetTimer(hDialog1,0x1234567,300,NULL);
                                 return (0);
                              }
                           }
                           else
                              wsprintf( (pNM_DispInfo->item).pszText, "---" ); // Text löschen (---)

                           (pNM_DispInfo->item).mask = LVIF_TEXT;
			                  SendDlgItemMessage( hDialog1, IDC_LISTCREATEREP, LVM_SETITEM,
                                               0, (LPARAM) (const LPLVITEM) &pNM_DispInfo->item );

                           EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), true ); // Übernehmen einschalten
                           GetMatReflDataFromList(hDialog1);
                           CalcFrequResp();
                           InvalidateRect(GetDlgItem(hDialog1, IDC_MAT_ABSPT_FRQ_RESP9), NULL, true);
                           return (1);           // Text wird übernommen
                        }
                        else   return (0);       // Text wird nicht übernommen
                     }
                     break;
               }
               break;

            case IDC_TREECREATE:                 // TreeView meldet sich
               switch( ((NMHDR *) lParam)->code )
               {
                  case TVN_SELCHANGED:
                     pNM_TreeView = (LPNMTREEVIEW) lParam;

                     if( (pNM_TreeView->action == TVC_BYKEYBOARD) ||
                         (pNM_TreeView->action == TVC_BYMOUSE) ) // Aktionen, nur wenn vom Benutzer gewollt
                     {
                        if( nFlagState < 3 )     // aktueller Programmzustand: Material Neu/Ändern
                        {
                                                 // Auswahl einer Materialgruppe im TreeView
                           if( pNM_TreeView->itemNew.state & TVIS_BOLD )
                           {
                                                 // neuer GruppenIndex
                              g_nGroupIndex = pNM_TreeView->itemNew.lParam;
                                                 // aktueller Gruppenname
                              wsprintf( g_pszGroupName, "%s", g_MatOverview.ppszGroupName[g_nGroupIndex] );
                                                 // Index der zugehörigen Material-Gruppe anzeigen
                              wsprintf( szString, "%d", g_nGroupIndex );
                              SetDlgItemText( hDialog1, IDC_TEXTCREATE2_7, szString );
                                                 // Gruppenname setzen ("Allgemein" oder ...)
                              SetDlgItemText( hDialog1, IDC_EDITCREATE8, g_pszGroupName );
                              EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), true ); // Übernehmen einschalten
                           }
                           else                  // Auswahl eines neuen Materials
                           {
                              nFlagState = 2;
                              g_nFlagRadioChanged = 1;
                                                 // RadioButtons für Material: 'Check' setzen bei "Ändern"
                        	   SendDlgItemMessage( hDialog1, IDC_RADIOCREATE1, BM_SETCHECK, 0, 0 );
	                           SendDlgItemMessage( hDialog1, IDC_RADIOCREATE2, BM_SETCHECK, 1, 0 );
                              InitMatData1( hDialog1, 3, pNM_TreeView->itemNew.lParam );
                           }
                        }
                        else                     // akt. Programmzustand: MatGruppe Neu/Ändern
                        {
                           if( pNM_TreeView->itemNew.state & TVIS_BOLD )
                           {
                              if( nFlagState == 3 )
                              {
                                 nFlagState = 4; // auf Gruppe ändern springen
                        	      SendDlgItemMessage( hDialog1, IDC_RADIOCREATE3, BM_SETCHECK, 0, 0 );
	                              SendDlgItemMessage( hDialog1, IDC_RADIOCREATE4, BM_SETCHECK, 1, 0 );
                              }
                                                 // neuer GruppenIndex
                              g_nGroupIndex = pNM_TreeView->itemNew.lParam;
                                                 // aktueller Gruppenname
                              wsprintf( g_pszGroupName, "%s", g_MatOverview.ppszGroupName[g_nGroupIndex] );
                                                 // Index der zugehörigen Material-Gruppe anzeigen
                              wsprintf( szString, "%d", g_nGroupIndex );
                              SetDlgItemText( hDialog1, IDC_TEXTCREATE2_7, szString );
                                                 // Gruppenname setzen ("Allgemein" oder ...)
                              SetDlgItemText( hDialog1, IDC_EDITCREATE8, g_pszGroupName );
                           }
                        }
                     }
                     break;
               }
               break;
         }
         break;

		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
            case IDC_EDITCREATE2:                // Materialbezeichnung (max. 31 char)
               if( (HIWORD(wParam) == EN_KILLFOCUS) &&
                   (SendDlgItemMessage( hDialog1, IDC_EDITCREATE2, EM_GETMODIFY, 0, 0 )) )
               {
                  SendDlgItemMessage( hDialog1, IDC_EDITCREATE2, EM_SETMODIFY, 0, 0 );
   					GetDlgItemText( hDialog1, IDC_EDITCREATE2, g_pszMatName, CARAMAT_MATNAMELENGTH-1 );
                  EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), true ); // Übernehmen einschalten
               }
               return (1);

            case IDC_EDITCREATE3:                // Materialbeschreibung (max. 95 char)
               if( (HIWORD(wParam) == EN_KILLFOCUS) &&
                   (SendDlgItemMessage( hDialog1, IDC_EDITCREATE3, EM_GETMODIFY, 0, 0 )) )
               {
                  SendDlgItemMessage( hDialog1, IDC_EDITCREATE3, EM_SETMODIFY, 0, 0 );
   					GetDlgItemText( hDialog1, IDC_EDITCREATE3, g_pszMatDescr, CARAMAT_MATDESCRLENGTH-1 );
                  EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), true ); // Übernehmen einschalten
               }
               return (1);

            case IDC_EDITCREATE5:                // alpha bei 100-300 Hz (0 - 30%), seit 6.8.99 nur noch bis max 30%
               if( (HIWORD(wParam) == EN_KILLFOCUS) &&
                   (SendDlgItemMessage( hDialog1, IDC_EDITCREATE5, EM_GETMODIFY, 0, 0 )) )
               {
                  SendDlgItemMessage( hDialog1, IDC_EDITCREATE5, EM_SETMODIFY, 0, 0 );
   					GetDlgItemText( hDialog1, IDC_EDITCREATE5, szString, 10 );

                  int    dummyint;
                  dummyint = atoi( szString );
                  if( dummyint > 30 )
                  {
                     dummyint = 30;
                     wsprintf( szString, "%d", 30 );
	                  SetDlgItemText( hDialog1, IDC_EDITCREATE5, szString );
                     SetFocus( (HWND) lParam );  // Focus erneut auf dieses Control setzen
                     SetTimer(hDialog1,0x1234568,300,NULL);
                  }
                  else
                  {
                     g_dReflGradLowFreq = 0.01*(100.0 - dummyint);
                     wsprintf( szString, "%d", ((int) (200.0*(1 - g_dReflGradLowFreq)+1))>>1 );
                     SetDlgItemText( hDialog1, IDC_EDITCREATE5, szString );
                  }
                  if ((g_nMatIndex >= 0) && (g_nMatIndex < g_nNoOfMaterialsEx))
                  {
                     g_pMaterial[g_nMatIndex].dReflGradLowFreq = g_dReflGradLowFreq;
                     CalcFrequResp();
                     InvalidateRect(GetDlgItem(hDialog1, IDC_MAT_ABSPT_FRQ_RESP9), NULL, true);
                  }
                  EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), true ); // Übernehmen einschalten
               }
               return (1);

            case IDC_EDITCREATE8:                // Materialgruppenbezeichnung (max. 31 char)
               if( (HIWORD(wParam) == EN_KILLFOCUS) &&
                   (SendDlgItemMessage( hDialog1, IDC_EDITCREATE8, EM_GETMODIFY, 0, 0 )) )
               {
                  SendDlgItemMessage( hDialog1, IDC_EDITCREATE8, EM_SETMODIFY, 0, 0 );
   					GetDlgItemText( hDialog1, IDC_EDITCREATE8, g_pszGroupName, CARAMAT_GROUPNAMELENGTH-1 );
                  EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), true ); // Übernehmen einschalten
               }
               return (1);
            case IDC_MAT_ABSPT_FRQ_RESP9:
               {
                  HWND hwnd = (HWND)lParam;
                  POINT pt;
                  RECT  rc;
                  LVITEM lvi;
                  lvi.mask      = LVIF_STATE;
                  lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
                  lvi.state     = LVIS_SELECTED | LVIS_FOCUSED;
                  lvi.iSubItem = 0;
                  ::GetClientRect(hwnd, &rc);
                  ::GetCursorPos(&pt);
                  ::ScreenToClient(hwnd, &pt);
                  lvi.iItem = MulDiv(pt.x, CARAMAT_MAXNOOFFREQ, rc.right);
                  long lResult = SendDlgItemMessage( hDialog1, IDC_LISTCREATEREP, LVM_SETSELECTIONMARK, 0, lvi.iItem);
           			SendDlgItemMessage(hDialog1, IDC_LISTCREATEREP, LVM_SETITEM,0, (LPARAM) &lvi);
                  SendDlgItemMessage(hDialog1, IDC_LISTCREATEREP, LVM_ENSUREVISIBLE, lvi.iItem, 0);
                  ListView_SetItemState(hwnd, lvi.iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
                  if (lResult != -1)
                  {
                     lvi.iItem = lResult;
                     lvi.state = 0;
              			SendDlgItemMessage(hDialog1, IDC_LISTCREATEREP, LVM_SETITEM,0, (LPARAM) &lvi);
                  }
               }
               return (1);
            case IDC_UEBERNEHMEN:
               if( nFlagState == 1 )       // Material neu
               {
                  if( SaveNewMatData( hDialog1 ) )
                  {
	                  SendDlgItemMessage( hDialog1, IDC_RADIOCREATE1, BM_CLICK, 0, 0 );
                     g_nFlagChanged = 1;
                  }
               }
               else if( nFlagState == 2 )  // Material ändern
               {
                  if( SaveChangedMatData( hDialog1 ) )
                  {
	                  SendDlgItemMessage( hDialog1, IDC_RADIOCREATE2, BM_CLICK, 0, 0 );
                     g_nFlagChanged = 1;
                  }
               }
               else if( nFlagState == 3 )  // MatGruppe neu
               {
                  if( SaveNewGroupData( hDialog1 ) )
                  {
	                  SendDlgItemMessage( hDialog1, IDC_RADIOCREATE3, BM_CLICK, 0, 0 );
                     g_nFlagChanged = 1;
                  }
               }
               else if( nFlagState == 4 )  // MatGruppe ändern
               {
                  if( SaveChangedGroupData( hDialog1 ) )
                  {
	                  SendDlgItemMessage( hDialog1, IDC_RADIOCREATE4, BM_CLICK, 0, 0 );
                     g_nFlagChanged = 1;
                  }
               }
               EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), false ); // Übernehmen ausschalten
					
               return (1);

            case IDC_OK2:                        // ist der DEFPUSHBUTTON
               {
                  HWND  hfrom;

                  hfrom = GetFocus();            // wer hat den Focus, nur wenn "IDOK" dann ok
/*
                  BITMAPINFO *pBmi = NULL;
                  GetBitmapInfo(g_nMatIndex, CARAMAT_BITMAPTYPE32X32, g_hMyHeap, &pBmi);
                  if (pBmi) HeapFree(g_hMyHeap, 0, pBmi);
*/
                  HWND  hList = GetDlgItem(hDialog1,IDC_LISTCREATEREP);

                  if(hfrom == hList)             // ListView-Report (Freq.Abhängkt. der alpha)
                  {
                     int item =  ListView_GetSelectionMark(hList);
                     if(item != -1)              // Selectierten Eintrag ändern
                     {
                        ListView_EditLabel(hList,item);
                        return 0;
                     }
                  }
                  else if( hfrom == GetDlgItem( hDialog1, IDC_OK2 ) )
                  {
                     bool bEndDialog = true;
                     if( nFlagState == 1 )       // Material neu
                     {
                        if( SaveNewMatData( hDialog1 ) )   g_nFlagChanged = 1;
                        else bEndDialog = false;
                     }
                     else if( nFlagState == 2 )  // Material ändern
                     {
                        if( SaveChangedMatData( hDialog1 ) )   g_nFlagChanged = 1;
                     }
                     else if( nFlagState == 3 )  // MatGruppe neu
                     {
                        if( SaveNewGroupData( hDialog1 ) )   g_nFlagChanged = 1;
                     }
                     else if( nFlagState == 4 )  // MatGruppe ändern
                     {
                        if( SaveChangedGroupData( hDialog1 ) )   g_nFlagChanged = 1;
                     }

                     hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDialog1, IDC_BILDCREATE4,
                                                                 STM_SETIMAGE,
                                                                 (WPARAM) IMAGE_BITMAP, NULL );
                                                 // vorherige Bitmap des Controls löschen
                     if( hBitMapCtrl != NULL )   DeleteObject( hBitMapCtrl );

                     if (bEndDialog) EndDialog( hDialog1, 0 );
                  }
                  else   PostMessage( hDialog1, WM_NEXTDLGCTL, 0, 0 );
               }
					
               return (1);

				case IDCANCEL:
            case IDC_CANCEL2:
               hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDialog1, IDC_BILDCREATE4,
                                                           STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, NULL );
                                                 // vorherige Bitmap des Controls löschen
               if( hBitMapCtrl != NULL )   DeleteObject( hBitMapCtrl );

               EndDialog( hDialog1, 0 );
					return (1);

            case IDC_RADIOCREATE1:
            case IDC_RADIOCREATE2:
            case IDC_RADIOCREATE3:
            case IDC_RADIOCREATE4:
                                                 // Zustand: Material Neu
               if( SendDlgItemMessage( hDialog1, IDC_RADIOCREATE1, BM_GETCHECK, 0, 0 ) == 1)
               {
                  g_nFlagRadioChanged = 1;
                  nFlagState  = 1;
                  if( !(InitMatData1( hDialog1, 2, 0 )) )
                     EndDialog( hDialog1, 0 );   // Heap-Error, Dialog beenden
               }
                                                 // Zustand: Material Ändern
               else if( SendDlgItemMessage( hDialog1, IDC_RADIOCREATE2, BM_GETCHECK, 0, 0 ) == 1)
               {
                  if( g_MatOverview.nNoOfMaterials == 0 )
	                  SendDlgItemMessage( hDialog1, IDC_RADIOCREATE1, BM_CLICK, 0, 0 );
                  else
                  {
                     g_nFlagRadioChanged = 1;
                     nFlagState = 2;
                     InitMatData1( hDialog1, 3, 0 );
                  }
               }
                                                 // Zustand: Mat-Gruppe Neu
               else if( SendDlgItemMessage( hDialog1, IDC_RADIOCREATE3, BM_GETCHECK, 0, 0 ) == 1)
               {
                  nFlagState = 3;
                  if( !(InitGroupData1( hDialog1, 2 )) )
                  {
                     hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDialog1, IDC_BILDCREATE4,
                                                                 STM_SETIMAGE,
                                                                 (WPARAM) IMAGE_BITMAP, NULL );
                                                 // vorherige Bitmap des Controls löschen
                     if( hBitMapCtrl != NULL )   DeleteObject( hBitMapCtrl );

                     EndDialog( hDialog1, 0 );   // Heap-Error, Dialog beenden
                  }
               }
                                                 // Zustand: Mat-Gruppe Ändern
               else if (SendDlgItemMessage( hDialog1, IDC_RADIOCREATE4, BM_GETCHECK, 0, 0 ) == 1)
               {
                  nFlagState = 4;
                  InitGroupData1( hDialog1, 3 );
               }

               return (1);

			}
			break;

		case WM_INITDIALOG:
         nFlagState = 1;                         // Material neu definieren
         InitMatData1( hDialog1, 1, 0 );

         if( (hIcon = (HICON) SendMessage( hDialog1, WM_GETICON, ICON_SMALL, 0 )) == NULL )
         {
            hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1)); // Icon-Symbol
            if( hIcon )   SendMessage( hDialog1, WM_SETICON, ICON_SMALL, (LPARAM) hIcon );
         }

         g_nFlagRadioChanged = 0;
         EnableWindow( GetDlgItem( hDialog1, IDC_UEBERNEHMEN ), false ); // Übernehmen ausschalten
         return (1);

      case WM_DESTROY:
         {
            hIcon = (HICON) SendMessage( hDialog1, WM_GETICON, ICON_SMALL, 0 );
            if( hIcon )   DestroyIcon( hIcon );
         }
         return (0);
	}
	
   return (0);
}


/********************************************************************/
/*                                                                  */
/*     Funktion: Vorbelegung der Eingabedaten für DIALOGSELECT      */
/*                                                                  */
/*    Wenn nMatIndex == 0 dann erfolgte der Aufruf durch WM_INIT,   */
/*    bei != 0 durch eine Auswahl im TreeView der Materialien       */
/*    oder durch Direktwahl des aufrufenden Programms (dann auch    */
/*    hier über WM_INIT !!)                                         */
/*                                                                  */
/********************************************************************/
void InitMatData2( HWND hDlg, bool bFlagInit, int nMatIndex )
{
   int                 i, j;
   const  int          nwidth = 32, nheight = 32;
   HBITMAP             hBitMapLoad, hBitMapCtrl; // Handles auf 32x32 Bitmap (für Load und STM_SETIMAGE)
   TVINSERTSTRUCT      tvInsert;                 // Struktur zum Anlegen eines TV-Items
   HTREEITEM           hTV_Item;                 // Handle auf ein TV-Item einer MatGruppe/eines Materials
   HTREEITEM           hTV_ItemCARET;            // Handle auf das zu selektierende TV-Item
   char                szString[256];            // Textbuffer
   char                szNewString[256];         // Hilfsstring für &-Verdopplung, OrgString max 96 Bytes lang

   g_nSelListItem = -1;
   if( nMatIndex == 0 )                          // keine direkte Auswahl
   {
                                                 // Vorlegen: den nächsten (zyklisch) Eintrag
      if( g_nMatIndex < g_MatOverview.nNoOfMaterials)
         g_nMatIndex++;
      else
         g_nMatIndex = 1;
   }
   else   g_nMatIndex = nMatIndex;               // direkte Auswahl über TreeView oder aufrufendes Programm

   CalcFrequResp();
   InvalidateRect(GetDlgItem(hDlg, IDC_MAT_ABSPT_FRQ_RESP9), NULL, true);
                                                 // die zugehörige Material-Gruppe, Name und Beschreibung

   g_nGroupIndex = g_pMaterial[g_nMatIndex].nGroupIndex;
   wsprintf( g_pszMatName, "%s", g_pMaterial[g_nMatIndex].pszMatName );
   wsprintf( g_pszMatDescr, "%s", g_pMaterial[g_nMatIndex].pszDescription );
                                                 // aktueller Gruppenname
   wsprintf( g_pszGroupName, "%s", g_MatOverview.ppszGroupName[g_nGroupIndex] );
                                                 // zugehöriger Reflektionsgrad, Frequenzabhängigkeit
   g_nBM_IndexOffset = g_pMaterial[g_nMatIndex].nBM_IndexOffset;
                                                 // Materialname und Materialbeschreibung setzen ("")
   CheckStringForAND( g_pszMatName, szNewString );
   SetDlgItemText( hDlg, IDC_TEXTSELECT2_2, szNewString );
   CheckStringForAND( g_pszMatDescr, szNewString );
   SetDlgItemText( hDlg, IDC_TEXTSELECT2_3, szNewString );
                                                 // Gruppenname setzen ("Allgemein" oder ...)
   CheckStringForAND( g_pszGroupName, szNewString );
   SetDlgItemText( hDlg, IDC_TEXTSELECT2_8, szNewString );
   
                                                 // entspr. BM als Oberflächenstruktur
   hBitMapLoad = (HBITMAP) LoadImage( g_hInstance,
                                      MAKEINTRESOURCE(IDB_BITMAP1+g_nBM_IndexOffset),
                                      IMAGE_BITMAP, nwidth, nheight, LR_DEFAULTCOLOR );
	hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDlg, IDC_BILDSELECT4, STM_SETIMAGE,
                                               (WPARAM) IMAGE_BITMAP, (LPARAM) (HANDLE) hBitMapLoad );
   if( hBitMapCtrl != NULL )   DeleteObject( hBitMapCtrl ); // vorherige Bitmap des Controls löschen

   if( bFlagInit == true )                       // bei Erstinitialisierung TreeView initialisieren
   {
      tvInsert.hInsertAfter   = TVI_SORT;        // alle Einträge alphabetisch einsortieren
      tvInsert.item.mask      = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
      tvInsert.item.stateMask = TVIS_BOLD;
      tvInsert.item.pszText   = &szString[0];
   
      for( i = 1; i <= g_MatOverview.nNoOfGroups; i++ )
      {
         tvInsert.hParent     = TVI_ROOT;        // Materialgruppen
         wsprintf( szString, "%s", g_MatOverview.ppszGroupName[i] ); // Gruppenname
         tvInsert.item.lParam = i;               // Materialgruppennummer
         tvInsert.item.state  = TVIS_BOLD;       // fette Schrift ist Kennzeichen für Materialgruppe (!!!)
 
         hTV_Item = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREESELECT, TVM_INSERTITEM,
                                                    0, (LPARAM) (const LPTVINSERTSTRUCT) &tvInsert );

         for( j = 0; j < g_MatOverview.pnNoOfMatInGroup[i]; j++ )
         {
            tvInsert.hParent     = hTV_Item;     // AbsorberMaterial
                                                 // Kurzbezeichnung
            wsprintf( szString, "%s", g_pMaterial[g_MatOverview.ppnMatIndex_Group[i][j]].pszMatName );
            tvInsert.item.lParam = g_MatOverview.ppnMatIndex_Group[i][j]; // Materialnummer
            tvInsert.item.state  = 0;

            if( g_MatOverview.ppnMatIndex_Group[i][j] == g_nMatIndex )
            {
                                                 // Handle auf den zu selektierenden Eintrag merken
               hTV_ItemCARET = (HTREEITEM) SendDlgItemMessage( hDlg, IDC_TREESELECT, TVM_INSERTITEM, 0,
                                                               (LPARAM)(const LPTVINSERTSTRUCT) &tvInsert );
            }
            else
            {
               SendDlgItemMessage( hDlg, IDC_TREESELECT, TVM_INSERTITEM,
                                   0, (LPARAM) (const LPTVINSERTSTRUCT) &tvInsert );
            }
         }
      }
                                                 // CARET-Eintrag selektieren und sichtbar machen
      SendDlgItemMessage( hDlg, IDC_TREESELECT, TVM_SELECTITEM, TVGN_CARET,
                          (LPARAM) (HTREEITEM) hTV_ItemCARET );
      SendDlgItemMessage( hDlg, IDC_TREESELECT, TVM_SELECTITEM, TVGN_FIRSTVISIBLE,
                          (LPARAM) (HTREEITEM) hTV_ItemCARET );
   }
}


/********************************************************************/
/*                                                                  */
/*                Call-Back-Funktion DIALOGSELECT                   */
/*                   Auswahl Absorbermaterials                      */
/*                                                                  */
/********************************************************************/
BOOL CALLBACK DialogFunc2( HWND hDialog2,
						         UINT message,
						         WPARAM wParam,
						         LPARAM lParam )
{
   const  int          nwidth = 32, nheight = 32;
   HBITMAP             hBitMapCtrl;
   LPNMTREEVIEW        pNM_TreeView;             // Pointer auf NMTREEVIEW-Struktur
   int	              helpId;
   HICON               hIcon = NULL;
	LPHELPINFO          lphi;
	char	              szString[768];            // Hilfszeichenfolge

   
	switch( message )
	{
      case WM_DRAWITEM:
      {
         DRAWITEMSTRUCT *pDI = (DRAWITEMSTRUCT*) lParam;
         if ((pDI->CtlType == ODT_STATIC) && (pDI->CtlID == IDC_MAT_ABSPT_FRQ_RESP9))
         {
            return OnDrawFrqResponse(pDI);
         }
      } break;
      case WM_DESTROY:
         {
            hIcon = (HICON) SendMessage( hDialog2, WM_GETICON, ICON_SMALL, 0 );
            if( hIcon )   DestroyIcon( hIcon );
         }
         return (0);

		case WM_HELP:
			lphi = (LPHELPINFO) lParam;
         if( lphi->iCtrlId < 100 )   return (0);
         helpId = lphi->iCtrlId - 100*( (int) (lphi->iCtrlId/100) );
         if( LoadString( g_hInstance, helpId, szString, sizeof(szString) ) )
         {
            RECT   rect;

            GetWindowRect(GetDlgItem(hDialog2, lphi->iCtrlId), &rect);
            if( PtInRect(&rect, lphi->MousePos) )
               CEtsHelp::CreateContextWnd( szString, lphi->MousePos.x, lphi->MousePos.y );
            else
               CEtsHelp::CreateContextWnd( szString,
                                            rect.left + (rect.right-rect.left)/2,
                                            rect.top  + (rect.bottom-rect.top)/2);
         }
         
         return (1);

      case WM_NOTIFY:
         switch( LOWORD(wParam) )
         {
            case IDC_TREESELECT:                 // TreeView meldet sich
               switch( ((NMHDR *) lParam)->code )
               {
                  case TVN_SELCHANGED:
                     pNM_TreeView = (LPNMTREEVIEW) lParam;

                                                 // Auswahl eines AbsorberMaterials im TreeView
                     if(pNM_TreeView->itemNew.state & TVIS_BOLD)
                     {
                        g_dCalcValues[0] = -100.0;
                        InvalidateRect(GetDlgItem(hDialog2, IDC_MAT_ABSPT_FRQ_RESP9), NULL, true);
                     }
                     else
                     {
                        InitMatData2( hDialog2, false, pNM_TreeView->itemNew.lParam );
                     }
                     break;
               }
               break;
         }
         break;

		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
            case IDC_OK2:
               {
                  HWND  hfrom;

                  hfrom = GetFocus();            // wer hat den Focus, nur wenn "IDOK" dann ok
                  if( hfrom == GetDlgItem( hDialog2, IDC_OK2 ) )
                  {
	                  hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDialog2, IDC_BILDSELECT4, STM_SETIMAGE,
                                                                 (WPARAM) IMAGE_BITMAP, NULL );
                     if( hBitMapCtrl != NULL )
                        DeleteObject( hBitMapCtrl ); // vorherige Bitmap des Controls löschen

                     EndDialog( hDialog2, 0 );
                  }
                  else   PostMessage( hDialog2, WM_NEXTDLGCTL, 0, 0 );
               }
					
               return (1);

				case IDCANCEL:
            case IDC_CANCEL2:
               hBitMapCtrl = (HBITMAP) SendDlgItemMessage( hDialog2, IDC_BILDSELECT4, STM_SETIMAGE,
                                                           (WPARAM) IMAGE_BITMAP, NULL );
               if( hBitMapCtrl != NULL )
                  DeleteObject( hBitMapCtrl );   // vorherige Bitmap des Controls löschen

               g_nMatIndex = 0;                  // Kennung, daß keine Auswahl getroffen wurde
               EndDialog( hDialog2, 0 );
					
               return (1);
			}
			break;

		case WM_INITDIALOG:
         {
            char   szString1[1024], szString2[1024];
            char  *pszString;

            InitMatData2( hDialog2, true, g_nMatIndex );

            if( (hIcon = (HICON) SendMessage( hDialog2, WM_GETICON, ICON_SMALL, 0 )) == NULL )
            {
               hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1)); // Icon-Symbol
               if( hIcon )   SendMessage( hDialog2, WM_SETICON, ICON_SMALL, (LPARAM) hIcon );
            }

            if( g_pszCaption )
            {
               pszString = (char *) HeapAlloc( g_hMyHeap, HEAP_ZERO_MEMORY,
                                               (strlen(g_pszCaption)+10)*sizeof(char) );
               if( pszString )
               {
                  wsprintf( pszString, "%s", g_pszCaption );
                  if( strlen(pszString) >= 256 )   pszString[256] = '\x0';

                  if( pszString[0] == '!' )
                  {
                     SetWindowText( hDialog2, &pszString[1] ); // ! ausblenden, alleinige Überschrift
                  }
                  else
                  {
                     GetWindowText( hDialog2, szString1, 256 );
                     wsprintf( szString2, "%s: %s", szString1, pszString );
                     SetWindowText( hDialog2, szString2 );     // 1. Teil = fest, 2. Teil wurde übergeben
                  }
                  HeapFree( g_hMyHeap, 0, pszString );
               }
            }
            return (1);
         }
	}
	
   return (0);
}




//***********************************************
//***********************************************
// Schnittstellenfunktionen für CARAMAT.DLL
//***********************************************
//***********************************************

int   GetLastErrorCode()                             // Funktion für Rückgabe des letzten Fehlercodes
{
   return (g_nErrorCode);
}


/********************************************************************/
/*                                                                  */
/*   Schnittstellenfunktion zur Editierung der Absorbermaterialien  */
/*                                                                  */
/*   Parameter:                                                     */
/*     - Handle auf des Window des aufrufenden Prozesses            */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false          */
/*                                                                  */
/********************************************************************/
bool   EditMaterial( HWND hwnd )
{
   char   szbuffer[256], szbuffer2[256];


                                                 // privater Heap konnte nicht angelegt bzw. benutzt werden
   if( (g_hMyHeap == NULL) || (g_nErrorCode == CARAMAT_HEAPERROR) )
   {
      g_nErrorCode = CARAMAT_HEAPERROR;
      return (false);
   }
                                                 // Materialdaten aus CARAMAT.RFL-File nicht geladen
   if( g_nErrorCode == CARAMAT_FILEREADERROR )
   {
      LoadString( g_hInstance, IDS_TXT_CREATENEW, szbuffer, 256 );
      LoadString( g_hInstance, IDS_TXT_CAPTION_CREATENEW, szbuffer2, 256 );
                                                 // Messagebox nur, wenn Editierschnittstelle aufgerufen
      if( MessageBox( hwnd, szbuffer, szbuffer2, MB_YESNO ) == IDYES )
      {
         g_nErrorCode = 0;                       // ErrorCode zurücksetzen
         g_MatOverview.nNoOfMaterials = 0;
         g_MatOverview.nNoOfGroups    = 1;
      
         if( g_pMaterial == NULL )               // globale Heap-Datenfelder noch nicht angelegt
         {
            if( !(AllocDaten( 0 )) )             // dynamische Datenfelder erstmals anlegen
               return (false);                   // Heap-Error
         }
                                                 // 1. GroupName definieren
         LoadString( g_hInstance, IDS_TXT_FIRSTGROUPNAME, szbuffer, (CARAMAT_GROUPNAMELENGTH-1) );
         wsprintf( g_MatOverview.ppszGroupName[1], "%s", szbuffer );
      }
      else   return (false);
   }
   
   g_nErrorCode   = 0;                           // ErrorCode zurücksetzen
   g_nFlagChanged = 0;                           // Voreinstellung: noch keine Daten geändert
   DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_DIALOGCREATE), hwnd, (DLGPROC) DialogFunc1 );

   if( (g_nErrorCode == 0) && (g_nFlagChanged == 1) )
      SaveMatDataFile( hwnd );                   // Daten sichern auf Platte, wenn sie geändert wurden

   if( g_nErrorCode == 0 )   return (true);
   else                      return (false);
}


/********************************************************************/
/*                                                                  */
/*    Schnittstellenfunktion zur Auswahl von Absorbermaterialien    */
/*                                                                  */
/*   Parameter:                                                     */
/*      Handle auf des Window des aufrufenden Prozesses,            */
/*      Pointer auf int-Variable, in die der selektierte MatIndex   */
/*      eingetragen wird und die bei Aufruf einen vorzulegenden     */
/*      MatIndex enthalten kann                                     */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false;         */
/*             Wenn kein Fehler (= true) aber nMatIndex = 0, dann   */
/*             wurde der Dialog zur Auswahl abgebrochen.            */
/*                                                                  */
/********************************************************************/
bool   SelectMaterial( HWND hwnd, int *pnMatIndex, char *pszCaption )
{
                                                 // privater Heap konnte nicht angelegt bzw. benutzt werden
   if( (g_hMyHeap == NULL) ||
       (g_nErrorCode == CARAMAT_HEAPERROR) ||
       (g_nErrorCode == CARAMAT_FILEREADERROR) )
   {
      if( g_nErrorCode != CARAMAT_FILEREADERROR )   g_nErrorCode = CARAMAT_HEAPERROR;
      *pnMatIndex  = 0;
      return (false);
   }
   
   if( g_MatOverview.nNoOfMaterials == 0 )       // CARAMAT.RFL fehlte, neue Daten wurden nicht angelegt
   {
      g_nErrorCode = CARAMAT_FILEREADERROR;
      *pnMatIndex  = 0;
      return (false);
   }

   g_nErrorCode = 0;                             // ErrorCode zurücksetzen

   if( (*pnMatIndex < 0) || (*pnMatIndex > g_MatOverview.nNoOfMaterials) )
      g_nMatIndex = 0;                           // bei keiner spez. Auswahl: das erste Element vorgelegen
   else
      g_nMatIndex = *pnMatIndex;

   g_pszCaption = pszCaption;                    // Pointer auf Überschriftstring global setzen

   DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_DIALOGSELECT), hwnd, (DLGPROC) DialogFunc2 );

   if( (g_nMatIndex < 0) || (g_nMatIndex > g_MatOverview.nNoOfMaterials) )
   {
      g_nErrorCode = CARAMAT_PROGRAMERROR;
      *pnMatIndex  = 0;
      return (false);
   }
   else
   {
      *pnMatIndex = g_nMatIndex;                 // bei Dialog-Abbruch: g_nMatIndex = 0
      return (true);
   }
}


/********************************************************************/
/*                                                                  */
/*         Schnittstellenfunktion zur Abfrage der DI-Bitmap         */
/*         der Oberflächenstruktur eines Absorbermaterials          */
/*                                                                  */
/*   Parameter:                                                     */
/*      MaterialIndex, Typ der DI-Bitmap (8x8 oder 32x32),          */
/*      Handle auf privaten Heap des Aufrufers,                     */
/*      Pointer auf Pointer auf eine BITMAPINFO-Struktur            */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false,         */
/*             Eintrag der BITMAPINFO-Struktur-Adresse              */
/*                                                                  */
/********************************************************************/
bool GetBitmapInfo( int nMatIndex, int nType, HANDLE hMyHeap, BITMAPINFO **ppBitmapInfo )
{
   bool                 bReturn   = true;
   int                  nSize     = 0;
   HBITMAP              hBitmap   = NULL;
   BITMAPINFO           *pBi      = NULL;
   BITMAPINFOHEADER     Bih;
   HDC                  hdc;

                                                 // privater Heap konnte nicht angelegt bzw. benutzt werden
   if( (g_hMyHeap == NULL) ||
       (g_nErrorCode == CARAMAT_HEAPERROR) ||
       (g_nErrorCode == CARAMAT_FILEREADERROR) )
   {
      if( g_nErrorCode != CARAMAT_FILEREADERROR )   g_nErrorCode = CARAMAT_HEAPERROR;
      bReturn = false;
   }
   else if( g_MatOverview.nNoOfMaterials == 0 )  // CARAMAT.RFL fehlte, neue Daten wurden nicht angelegt
   {
      g_nErrorCode = CARAMAT_FILEREADERROR;
      bReturn = false;
   }
   else if( (nMatIndex < 1) || (nMatIndex > g_MatOverview.nNoOfMaterials) ) // falscher Index
   {
      g_nErrorCode = CARAMAT_MATINDEXERROR;
      bReturn = false;
   }

   if( nType == CARAMAT_BITMAPTYPE8X8          ) nSize = 8;  // 8x8 Bitmap
   else if ( nType == CARAMAT_BITMAPTYPE32X32  ) nSize = 32; // 32x32 Bitmap
   else if ( nType == CARAMAT_BITMAPTYPE32X32+1) nSize = 32; // 32x32 Bitmap
   else
   {
      g_nErrorCode = CARAMAT_PROGRAMERROR;       // auch wenn vorher schon ein Fehler war, dieser ist noch schlimmer
      bReturn = false;
   }

   if( nSize > 0 )
   {
      if( bReturn == false )
      {
                                                 // Fehlerbitmap laden
         if( nSize == 8 )
         {
            hBitmap = (HBITMAP) LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_NOBMP08),
                                           IMAGE_BITMAP, nSize, nSize, LR_DEFAULTCOLOR );
         }
         else if( nSize == 32 )
         {
            hBitmap = (HBITMAP) LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_NOBMP32),
                                           IMAGE_BITMAP, nSize, nSize, LR_DEFAULTCOLOR );
         }
      }
      else
      {
         if (nType == CARAMAT_BITMAPTYPE32X32+1) // Neues Flag für OpenGL auch größere Bmp´s
         {
            switch (g_pMaterial[nMatIndex].nBM_IndexOffset)
            {
               case 82: nSize = 128; break;
               case 83: nSize = 128; break;
               case 84: nSize = 256; break;
               case 85: nSize = 128; break;
            }
         }                                       // Speicher für BITMAPINFO pBi allozieren
                                                 // Bitmap-Resource zu nMatIndex laden
         hBitmap = (HBITMAP) LoadImage( g_hInstance,
                                        MAKEINTRESOURCE(IDB_BITMAP1+g_pMaterial[nMatIndex].nBM_IndexOffset),
                                        IMAGE_BITMAP, nSize, nSize, LR_DEFAULTCOLOR );
      }
   }

   if( hBitmap )
   {
      hdc = GetDC(NULL);                      // ScreenDC belegen
      
      Bih.biSize          = sizeof(BITMAPINFOHEADER);
      Bih.biWidth         = nSize;            // Größe der Bitmap
      Bih.biHeight        = nSize;
      Bih.biPlanes        = 1;                // nur 1 Bitplane
      Bih.biBitCount      = 32;               // 32 Bit Farben
      Bih.biCompression   = BI_RGB;           // als RGB Farbwerte ablegen (d.h. keine Palette)
      Bih.biSizeImage     = nSize*nSize*4;    // Speichergröße für die Pixel (nur 2er Potenzen !!)
      Bih.biXPelsPerMeter = nSize;            // Dummy
      Bih.biYPelsPerMeter = nSize;            // Dummy
      Bih.biClrUsed       = 0;                // alle Farben wichtig
      Bih.biClrImportant  = 0;

      pBi = (BITMAPINFO *) HeapAlloc( hMyHeap, HEAP_ZERO_MEMORY,
                                      (sizeof(BITMAPINFOHEADER)+Bih.biSizeImage+sizeof(COLORREF)*Bih.biClrUsed) );
      if( pBi )
      {
         *( (BITMAPINFOHEADER *) pBi) = Bih;  // Kopieren des obigen Headers in den Heap

         if( GetDIBits( hdc, hBitmap, 0, nSize,
                        &(pBi->bmiColors[0]), pBi, DIB_RGB_COLORS ) == NULL )
         {
            HeapFree( hMyHeap, 0, pBi );      // belegten Speicherblock im User-Heap wieder freigeben
            g_nErrorCode = CARAMAT_PROGRAMERROR;
            bReturn = false;                  // DIB-Format war nicht möglich
         }
         else
         {
            if( bReturn )   g_nErrorCode = 0; // ErrorCode zurücksetzen
            *ppBitmapInfo = pBi;              // Adresse des BITMAPINFO-Blocks zurückgeben
         }
      }
      else                                    // Speicher im User-Heap nicht belegbar
      {
         g_nErrorCode = CARAMAT_USERHEAPERROR; // neu Wichtig !
         bReturn = false;
      }

      ReleaseDC(NULL, hdc);                   // ScreenDC freigeben
      DeleteObject(hBitmap);                  // Bitmap löschen
   }
   else if( nSize > 0 )                       // Bitmap-Resource nicht ladbar, sonst falscher nType -> Programmfehler
   {
      g_nErrorCode = CARAMAT_LOADBMERROR;
      bReturn = false;
   }

   return (bReturn);
}


/********************************************************************/
/*                                                                  */
/*   Schnittstellenfunktion zur Abfrage der Materialbeschreibungen  */
/*                                                                  */
/*   Parameter:                                                     */
/*     - MaterialIndex, Pointer auf Struktur CARAMAT_MATDSCRS       */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false,         */
/*             Eintrag der MatDscrDaten in die übergebene Adresse   */
/*                                                                  */
/********************************************************************/
bool GetMatDscrDaten( int nMatIndex, CARAMAT_MATDSCRS *pMatDscrDaten )
{
   bool bReturn = true;
                                                 // privater Heap konnte nicht angelegt bzw. benutzt werden
   if( (g_hMyHeap == NULL) ||
       (g_nErrorCode == CARAMAT_HEAPERROR) ||
       (g_nErrorCode == CARAMAT_FILEREADERROR) )
   {
      if( g_nErrorCode != CARAMAT_FILEREADERROR )   g_nErrorCode = CARAMAT_HEAPERROR;
      bReturn = false;
   }
   else if( g_MatOverview.nNoOfMaterials == 0 )  // CARAMAT.RFL fehlte, neue Daten wurden nicht angelegt
   {
      g_nErrorCode = CARAMAT_FILEREADERROR;
      bReturn = false;
   }
   else if( (nMatIndex < 1) || (nMatIndex > g_MatOverview.nNoOfMaterials) ) // falscher Index
   {
      g_nErrorCode = CARAMAT_MATINDEXERROR;
      bReturn = false;
   }

   if( bReturn == false )
   {
                                                 // Rückgabestruktur belegen
      pMatDscrDaten->nMatIndex   = -1;
      pMatDscrDaten->nGroupIndex = -1;
      
      wsprintf( pMatDscrDaten->pszMatName, "Unknown" );
      wsprintf( pMatDscrDaten->pszDescription, "Unknown" );
      wsprintf( pMatDscrDaten->pszGroupName, "Unknown" );
   }
   else
   {
      g_nErrorCode = 0;                          // ErrorCode zurücksetzen
                                                 // Rückgabestruktur belegen
      pMatDscrDaten->nMatIndex   = nMatIndex;
      pMatDscrDaten->nGroupIndex = g_pMaterial[nMatIndex].nGroupIndex;
      
      wsprintf( pMatDscrDaten->pszMatName, "%s", g_pMaterial[nMatIndex].pszMatName );
      wsprintf( pMatDscrDaten->pszDescription, "%s", g_pMaterial[nMatIndex].pszDescription );
      wsprintf( pMatDscrDaten->pszGroupName, "%s",
                g_MatOverview.ppszGroupName[g_pMaterial[nMatIndex].nGroupIndex] );
   }
   
   return (bReturn);
}

/********************************************************************/
/*                                                                  */
/*      Berechnung der Frequenzabhängigkeit der Reflektonsgrade     */
/*    aus einem übergebenen Reflektionsgrad bei tiefen Frequenzen   */
/*                                                                  */
/********************************************************************/
void Calc_ReflCoeff1(double reflcoefflowfreq, double *pdOutFrequ)
{
   int      i;
   double   dreflcoeffmin, dreflcoeffmax;        // Empirische Formel: Die Schallabsorption soll mit der
   double   dmaxfactabsorption;                  // Frequenz auf das maximal Dreifache des Wertes bei tiefen
   double   drelativefreq;                       // Frequenzen (z.B. 100 Hz) steigen; dieser Faktor (max. 3)
                                                 // hängt von der Größe des Absorptionsgrades bei tiefen
                                                 // Frequenzen ab, da die Absorption nie größer als 1 sein
                                                 // kann. Daraus ergibt sich: MaxFaktorAbsorption = 0.27 +
                                                 // 1/(Abs.Faktor(100 Hz) + 0.37); Der maximale Reflexions-
                                                 // grad ist der bei tiefen Frequenzen (1-Abs.Grad(100 Hz)),
                                                 // der minimale Reflexionsgrad ist der bei hohen Frequenzen
                                                 // und bestimmt sich aus der Größe von MaxFaktorAbsorption
                                                 // ( = 1 - MaxFaktorAbsorption*Abs.Grad(100 Hz) ).
                                                 // Mit der verwendeten (empirischen) Gesamtformel ergibt
                                                 // sich bei ca. 600 Hz ca. 50% der Absorptionszunahme.

   dmaxfactabsorption = 0.27 + 1.0/((1-reflcoefflowfreq) + 0.37);

   dreflcoeffmax = reflcoefflowfreq;

   dreflcoeffmin = 1 - dmaxfactabsorption*(1 - reflcoefflowfreq);

   for( i = 0; i < CARAMAT_MAXNOOFFREQ; i++ )
   {
      drelativefreq          = g_dFrequency[i]/STANDARDLOWFREQ;

      pdOutFrequ[i] = dreflcoeffmin + (dreflcoeffmax - dreflcoeffmin)/
                                               (1 + drelativefreq*(dreflcoeffmax - dreflcoeffmin));
   }
}


/********************************************************************/
/*                                                                  */
/*     Berechnung der Frequenzabhängigkeit der Reflektonsgrade      */
/* durch Interpolation innerhalb eines übergebenen ReflGrad-Arrays  */
/*                                                                  */
/********************************************************************/
void Calc_ReflCoeff2( int nooffreq, double *frequency, double *reflcoeff, double *pdOutFrequ)
{
   int      i, j = 1, ffound;
   double   dlambda;


   for( i = 0; i < CARAMAT_MAXNOOFFREQ; i ++ )
   {
      do
      {
         ffound = 1;

         if( g_dFrequency[i] <= frequency[0] )
         {
            pdOutFrequ[i] = reflcoeff[0];
         }
          else if( g_dFrequency[i] > frequency[nooffreq-1] )
         {
            pdOutFrequ[i] = reflcoeff[nooffreq-1];
         }
         else if( (g_dFrequency[i] > frequency[j-1]) && (g_dFrequency[i] <= frequency[j]) )
         {
            dlambda = log(g_dFrequency[i]/frequency[j-1])/log(frequency[j]/frequency[j-1]);
                                                 // logarithmisch interpoliert
            pdOutFrequ[i] = reflcoeff[j-1] + dlambda*(reflcoeff[j]-reflcoeff[j-1]);
         }
         else
         {
            ffound = 0;
            j++;
         }
      }while( ffound == 0 );
   }
}

LRESULT OnDrawFrqResponse(DRAWITEMSTRUCT *pDI)
{
   if (g_dCalcValues[0] != -100.0)
   {
      char szText[64] = "88888";
      double dmY, dbY, dmX, dbX, dVal;
      double dMinVal = 0, dMaxVal, dStepVal = 20;        // 20 % Schritte;
      double *pdX = g_dFrequency;
      int    nWidth  = pDI->rcItem.right  - pDI->rcItem.left;
      int    nHeight = pDI->rcItem.top - pDI->rcItem.bottom;
      int    i, nTextW, nTextH, nHeadingBottom;
      bool   bText;
      POINT  ptLine;
      RECT   rcText = {0,0,0,0};
      COLORREF ColMagn  = RGB(255,0,0);
      HPEN hMagnPen  = ::CreatePen(PS_SOLID, 1, ColMagn);
      HPEN hGridPen  = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_GRAYTEXT));

      for (i=0; i<CARAMAT_MAXNOOFFREQ; i++)
      {
         dVal = (1.0 - g_dCalcValues[i]) * 100.0;
         if (i==0) dMaxVal = dVal;
         else
         {
            if (dMaxVal < dVal) dMaxVal = dVal;
         }
      }
      if (dMaxVal < 40) dMaxVal = 40;

      dStepVal = (double)(10 * (int)((dMaxVal / 45) + 0.5));
      if (dStepVal < 5) dStepVal = 5;
      dMinVal -= dStepVal;
      dVal     = dMaxVal;
      dMaxVal  = (double)(dStepVal * (int)((dMaxVal / dStepVal)+1));
      if ((dMaxVal - dVal) < 0.5 * (dStepVal)) dMaxVal += dStepVal;

      dmX = (double)nWidth / (log10(pdX[CARAMAT_MAXNOOFFREQ-1])-log10(pdX[0]));// Transformation X-Achse
      dbX = (double)pDI->rcItem.left - dmX * log10(pdX[0]);
      dmY = (double)nHeight / (dMaxVal - dMinVal);             // Transformation Amplituden-Achse
      dbY = (double)pDI->rcItem.bottom - dmY * dMinVal;

      ::SaveDC(pDI->hDC);
      ::SetBkColor(pDI->hDC, ::GetSysColor(COLOR_WINDOW));
      ::SetTextColor(pDI->hDC, ::GetSysColor(COLOR_GRAYTEXT)); // für den Gittertext

      ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_LEFT|DT_TOP|DT_CALCRECT);
      nTextW = rcText.right  - rcText.left;                    // Textgröße ermitteln
      nTextH = rcText.bottom - rcText.top;                     // Hintergrund löschen
      ::FillRect(pDI->hDC, &pDI->rcItem, ::GetSysColorBrush(COLOR_WINDOW));

      ::SelectObject(pDI->hDC, hGridPen);                      // für die Gitterlinien

      rcText.left    = pDI->rcItem.left + nTextW;              // Amplitudenachsenbeschriftung
      rcText.right   = pDI->rcItem.right;
      rcText.top     = pDI->rcItem.top;
      rcText.bottom  = rcText.top + nTextH;
      nHeadingBottom = rcText.bottom;
      ::GetWindowText(pDI->hwndItem, szText, 64);              // Überschrift
      ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_LEFT|DT_BOTTOM|DT_SINGLELINE);
      rcText.left  = pDI->rcItem.left;
      rcText.right = rcText.left + MulDiv(nTextW, 3, 4);
      for (dVal=dMinVal+dStepVal; dVal<dMaxVal; dVal+=dStepVal)
      {
         ptLine.x = rcText.right;
         ptLine.y = (int)(dmY * dVal + dbY);
         if (ptLine.y < nHeadingBottom) break;
         ::MoveToEx(pDI->hDC, ptLine.x, ptLine.y, NULL);
         ptLine.x = pDI->rcItem.right;
         ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
         rcText.bottom = ptLine.y + (nTextH>>1);
         rcText.top    = rcText.bottom - nTextH;
         wsprintf(szText, "%d", (int)dVal);
         ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_LEFT|DT_BOTTOM|DT_SINGLELINE);
      }


      rcText.bottom = pDI->rcItem.bottom;                      // Frequenzachsenbeschriftung
      rcText.top    = rcText.bottom - nTextH;
      rcText.left   = pDI->rcItem.right - nTextW;
      rcText.right  = pDI->rcItem.right;
      strcpy(szText, "Hz ");
      ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);
      bText = false;
      // nHeadingBottom = pDI->rcItem.top + nTextH;
      nHeadingBottom = ptLine.y-1;
      //   20 Hz
      for (i=18; i<CARAMAT_MAXNOOFFREQ-9; i+=9)            // Oktaven bei CALE-Frequenzen
      {
         ptLine.x = (int)(dmX * log10(pdX[i]) + dbX);
         ptLine.y = nHeadingBottom;
         ::MoveToEx(pDI->hDC, ptLine.x, ptLine.y, NULL);
         if (bText)
         {
            ptLine.y = rcText.top;
            ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
            rcText.left   = ptLine.x - (nTextW>>1);
            rcText.right  = ptLine.x + (nTextW>>1);
            wsprintf(szText, "%d", (int)pdX[i]);
            ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_CENTER|DT_BOTTOM|DT_SINGLELINE);
         }
         else
         {
            if (i == 108) ptLine.y = rcText.top;
            else          ptLine.y = pDI->rcItem.bottom;
            ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
         }
         bText = !bText;
      }

      ::SelectObject(pDI->hDC, hMagnPen);                      // Ausgabe der Amplitude
      for (i=0; i<CARAMAT_MAXNOOFFREQ; i++)
      {
         dVal = (1.0 - g_dCalcValues[i]) * 100.0;

         ptLine.x = (int)(dmX * log10(pdX[i]) + dbX);
         ptLine.y = (int)(dmY * dVal + dbY);
         if (i==0) ::MoveToEx(pDI->hDC, ptLine.x, ptLine.y, NULL);
         else      ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
         if (i==g_nSelListItem)
         {
            ::Rectangle(pDI->hDC, ptLine.x-2, ptLine.y-2, ptLine.x+3, ptLine.y+3);
         }
      }

      ::RestoreDC(pDI->hDC, -1);
      ::DeleteObject(hMagnPen);
      ::DeleteObject(hGridPen);
   }
   else
   {
      ::FillRect(pDI->hDC, &pDI->rcItem, ::GetSysColorBrush(COLOR_MENU));
   }
   return 1;
}

/********************************************************************/
/*                                                                  */
/*        Schnittstellenfunktion zur Abfrage der von CARACALC       */
/*                     benötigten Materialdaten                     */
/*                                                                  */
/*   Parameter:                                                     */
/*     - MaterialIndex, Pointer auf Struktur CARAMAT_MATREFLS       */
/*                                                                  */
/*   Rückgabe: true, wenn kein Fehler auftrat; sonst false,         */
/*             Eintrag der MatReflDaten in die übergebene Adresse   */
/*                                                                  */
/********************************************************************/
bool   GetMatReflDaten( int nMatIndex, CARAMAT_MATREFLS *pMatReflDaten )
{
                                                 // privater Heap konnte nicht angelegt bzw. benutzt werden
   if( (g_hMyHeap == NULL) ||
       (g_nErrorCode == CARAMAT_HEAPERROR) ||
       (g_nErrorCode == CARAMAT_FILEREADERROR) )
   {
      if( g_nErrorCode != CARAMAT_FILEREADERROR )   g_nErrorCode = CARAMAT_HEAPERROR;
      return (false);
   }

   if( g_MatOverview.nNoOfMaterials == 0 )       // CARAMAT.RFL fehlte, neue Daten wurden nicht angelegt
   {
      g_nErrorCode = CARAMAT_FILEREADERROR;
      return (false);
   }

   g_nErrorCode = 0;                             // ErrorCode zurücksetzen

   if( (nMatIndex < 1) || (nMatIndex > g_MatOverview.nNoOfMaterials) )
   {
      g_nErrorCode = CARAMAT_MATINDEXERROR;
      return (false);
   }
   else
   {
                                                 // Rückgabestruktur belegen
      pMatReflDaten->nMatIndex        = nMatIndex;
      pMatReflDaten->nNoOfFreq        = g_pMaterial[nMatIndex].nNoOfFreq;
      pMatReflDaten->dReflGradLowFreq = g_pMaterial[nMatIndex].dReflGradLowFreq;

      int   i;

      for( i = 0; i < CARAMAT_MAXNOOFFREQ; i++ )
      {
         pMatReflDaten->dFrequency[i]    = g_pMaterial[nMatIndex].dFrequency[i];
         pMatReflDaten->dReflGradFreq[i] = g_pMaterial[nMatIndex].dReflGradFreq[i];
      }
   
      return (true);
   }
}

void GetMatReflDataFromList(HWND hDlg)
{
   LVITEM    lvItem_REP;               // Struktur für ListView-Items
   char      szString[256];
   int       i, dummyint;

   // Neue Materialdaten eintragen
   g_nNoOfFreq = 0;                              // Bestimmung der Anz. definierter alpha(Freq.)

   lvItem_REP.mask       = LVIF_TEXT | LVIF_PARAM;
   lvItem_REP.pszText    = &szString[0];
   lvItem_REP.cchTextMax = 10;
   lvItem_REP.iSubItem   = 0;
   for( i = 0; i < CARAMAT_MAXNOOFFREQ; i++ )
   {
      lvItem_REP.iItem = i;
   	SendDlgItemMessage( hDlg, IDC_LISTCREATEREP, LVM_GETITEM,
                          0, (LPARAM) (const LPLVITEM) &lvItem_REP );

      if( lvItem_REP.pszText[0] != '-' )
      {
         dummyint = atoi( lvItem_REP.pszText );
         g_pMaterial[g_nMatIndex].dFrequency[g_nNoOfFreq]    = g_dFrequency[lvItem_REP.lParam];
         g_pMaterial[g_nMatIndex].dReflGradFreq[g_nNoOfFreq] = 0.01*(100.0 - dummyint);
         g_nNoOfFreq++;
      }
   }

   g_pMaterial[g_nMatIndex].nNoOfFreq = g_nNoOfFreq;
                                                 // Reflexionsgrad für niedrige Frequenzen
   g_pMaterial[g_nMatIndex].dReflGradLowFreq = g_dReflGradLowFreq;
}

void CalcFrequResp()
{
   if (g_pMaterial[g_nMatIndex].nNoOfFreq < 3)
   {
      Calc_ReflCoeff1(g_pMaterial[g_nMatIndex].dReflGradLowFreq, g_dCalcValues);
   }
   else
   {
      Calc_ReflCoeff2(g_pMaterial[g_nMatIndex].nNoOfFreq, g_pMaterial[g_nMatIndex].dFrequency, g_pMaterial[g_nMatIndex].dReflGradFreq, g_dCalcValues);
   }
}

/********************************************************************/
/*                       DLL Main Funktion                          */
/********************************************************************/
bool WINAPI DllMain(HINSTANCE hInstance,DWORD fReason,LPVOID pvReseved)
{
   switch (fReason)
   {
      case DLL_PROCESS_ATTACH:                   // Einbinden der DLL in den Adressraum des Prozesses
      {
         DisableThreadLibraryCalls( hInstance );

         g_hInstance    = hInstance;             // Instance-Handle global speichern
         g_nErrorCode   = 0;                     // ErrorCode auf 0 setzen
         g_pMaterial    = NULL;                  // Voreinstellung: noch keine Heap-Daten angelegt

         INITCOMMONCONTROLSEX icce;
         icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
         icce.dwICC = ICC_WIN95_CLASSES;

         for(int i=0; i<CARAMAT_MAXNOOFFREQ; i++)
         {
            g_dFrequency[i] = 5.0*pow(2, 1.0*i/9.0); // 118 Frequenzwerte von 5-40960 Hz, 1/3 Terz
         }
         g_dCalcValues[0] = -100.0;
         InitCommonControlsEx(&icce);
	                                              // privaten Heap anlegen
         g_hMyHeap = HeapCreate(HEAP_NO_SERIALIZE, 1024000, 0);

         if( g_hMyHeap )                                       // privater Heap konnte angelegt werden
            LoadMatDataFile( NULL );                           // Materialdaten aus CARAMAT.RFL-File laden

      } break;
      case DLL_PROCESS_DETACH:                                 // Die DLL wird freigegeben
         if( g_hMyHeap )
         {
            if( g_pMaterial != NULL ) FreeDaten();             // dynamische Datenfelder freigeben
                                                               // später: die evtl. vorhandenen Zusatzdatenfelder
                                                               // für Impedanz und Materialdetaildaten vorher löschen !!
            HeapDestroy(g_hMyHeap);                            // privaten Heap löschen
            g_hMyHeap = NULL;
         }
         break;
   }

   return true;
}


