#ifndef _CCARACADV_
#define _CCARACADV_

/**************************************+****************************************

                                   CCaraCadV.h
                                   ----------

            Declaration der Schnittstellenklasse für die CARACADV.DLL

                     (C) 2000 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 23.08.2000


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#if !defined(__CALCVECT_H)                       // daimt kein calcvect.h benötigt wird
class Vector 
{
private:
   double xcomp,ycomp,zcomp;
};

#endif

// diese Meldungen werden von WM_LOADCADFILE zurückgegeben (0 == kein "Fehler")

#define RTE_ENDOFFILE            1               // Dateiende erreicht
#define RTE_CRCINVALID           2               // CRC Code der Datei ist inkorrekt
#define RTE_DATAINVALID          3               // gelesene Daten können nicht stimmen
#define RTE_FILE_NOT_FOUND       4               // CAD oder BDD Datendatei nicht gefunden
#define RTE_INVALID_HEADER       7               // keine CAD,BGP,BDD,BSD,BGD
#define RTE_OUTOFMEMORY          10              // kein Speicher
#define RTE_CREATEMETAFILE       11              // EnhMetafile konnte nicht erstellt werden
#define RTE_OLDVERSION           333             // CAD ist eine alte Version (kein Fehler !)

// Nachrichten das das CARACADV-Fenster

#define WM_LOADCADFILE (WM_APP+100)              // Lade oder Schließe CAD File (WPARAM = char * szPath oder NULL "Close")
#define WM_OPTIMIZE    (WM_APP+101)              // Mitteile, das Optimierung startet oder endet (WPARAM == bool (true==optimierung läuft))
#define WM_SETNEWPOS   (WM_APP+102)              // neue Position einstellen    (WPARAM = CARACADV_NEWPOSITIONS * )

// Strukturen für die Nachricht WM_SETNEWPOS

struct CARACADV_POS
{
   Vector vPosition;                             // Position in CALE Koordinaten
   int    nPhi;                                  // Winkel Phi
   int    nTheta;                                // nur für Lautsprecherboxen auch Winkel Theta
   int    nPsi;                                  // und Psi
   bool   bChanged;                              // diese Position hat sich geändert (true)
};

struct CARACADV_NEWPOSITIONS
{
   CARACADV_POS sTarget;                         // Position für den ersten Hörer
   DWORD        nSources;                        // Anzahl der folgenden Boxenpositionen
   int          ndummy;                          // 4 structur alignment
   CARACADV_POS sSources[1];                     // embedded Array mit den Boxenpositionen
};


class CCaraCadV
{
public:
   ~CCaraCadV();

   static void Destructor();
   static HWND Create(HWND);
};

#endif