// Globale Makros, Variablen, Strukturen und Prototypen für DLL CARABOX
//*********************************************************************
// 25.11.99, Tho

#ifndef __STRUCTURES_H_INCLUDED
#define __STRUCTURES_H_INCLUDED


#define  STRICT
#include <WINDOWS.H>

#include "CVector.H"
#define   M_PI            (double)  3.14159265358979  // Zahl Pi
#include "CALCMPLX.H"
#include "CETSHELP.H"
#include "CCARASDB.H"
#include "FILEHEADER.H"
#include "CCARABOXVIEW.H"

// Makros
#define  CARABOX_HEAPERROR         1             // Fehlercode für Anlegen Datenfelder im Heap
#define  CARABOX_PROGRAMERROR      2             // Programmfehler

#define  CARABOX_BOXTYP_CLOS       1             // Typ = Closed Box
#define  CARABOX_BOXTYP_BR         2             // Typ = Vented Box
#define  CARABOX_BOXTYP_TL         3             // Typ = TL-Box
#define  CARABOX_BOXTYP_PM         4             // Typ = Passivmembran Box

#define  CARABOX_CHASSISTYP_PM     0             // Chassistyp = Passivmembran
#define  CARABOX_CHASSISTYP_BR     1             // Chassistyp = BR, Baßreflexrohr
#define  CARABOX_CHASSISTYP_TL     2             // Chassistyp = TL, Transmission-Line
#define  CARABOX_CHASSISTYP_TT1    3             // Chassistyp = TT1
#define  CARABOX_CHASSISTYP_TT2    4             // Chassistyp = TT2, TT früher ausgekoppelt
#define  CARABOX_CHASSISTYP_MT1    5             // Chassistyp = MT1, unterer Mitteltöner
#define  CARABOX_CHASSISTYP_MT2    6             // Chassistyp = MT2, mittlerer Mitteltöner
#define  CARABOX_CHASSISTYP_MT3    7             // Chassistyp = MT3, oberer Mitteltöner
#define  CARABOX_CHASSISTYP_HT     8             // Chassistyp = HT, Hochtöner

#define  CARABOX_MEMBRTYP_FLAT1    1             // Flachmembran rund
#define  CARABOX_MEMBRTYP_FLAT2    2             // Flachmembran rechteckig
#define  CARABOX_MEMBRTYP_CON      3             // Konus
#define  CARABOX_MEMBRTYP_DOME     4             // Kalotte
#define  CARABOX_MEMBRTYP_DIPOLE   5             // rechteckige Dipol- Flächenstrahler
#define  CARABOX_MEMBRTYP_HORN1    6             // Hornlautsprecher rund
#define  CARABOX_MEMBRTYP_HORN2    7             // Hornlautsprecher rechteckig
#define  CARABOX_MEMBRTYP_SPHERE   8             // Kugelstrahler

#define  CARABOX_CABWALL_TOP       1             // Einbauwand: Gehäusedeckel
#define  CARABOX_CABWALL_FRONT     2             // Einbauwand: Vorne
#define  CARABOX_CABWALL_BACK      3             // Einbauwand: Hinten
#define  CARABOX_CABWALL_LEFT      4             // Einbauwand: Links
#define  CARABOX_CABWALL_RIGHT     5             // Einbauwand: Rechts
#define  CARABOX_CABWALL_BOTTOM    6             // Einbauwand: Unten

#define  CARABOX_TTRESPONSETYPMINU     1         // TT-Frequenzgangtyp für tiefe Frequenzen: überdämpft (Q=0.5)
#define  CARABOX_TTRESPONSETYPNORM     2         // TT-Frequenzgangtyp für tiefe Frequenzen: normal (Q=0.9..1)
#define  CARABOX_TTRESPONSETYPPLUS     3         // TT-Frequenzgangtyp für tiefe Frequenzen: unterdämpft (Q=2)

#define  CARABOX_CO_1W             7             // NW-Typ7: normale 1-Wege-Box, 1-2 Grenzfrequenzen
#define  CARABOX_CO_2W             1             // NW-Typ1: normale 2-Wege-Box, 1 Überg.Freq
#define  CARABOX_CO_2_3W           2             // NW-Typ2: 2(3)-Wege-Box, 1 Überg.Freq + 2.TT früher abgekoppelt
#define  CARABOX_CO_3W             3             // NW-Typ3: normale 3-Wege-Box, 2 Überg.Freq
#define  CARABOX_CO_3_4W           4             // NW-Typ4: 3(4)-Wege-Box, 2 Überg.Freq + 2.TT früher abgekoppelt
#define  CARABOX_CO_4W             5             // NW-Typ5: normale 4-Wege-Box, 3 Überg.Freq
#define  CARABOX_CO_5W             6             // NW-Typ6: normale 5-Wege-Box, 4 Überg.Freq

#define  CARABOX_CHASSISTYPES      9             // verschiedene Chassistypen
#define  CARABOX_CABSIZES          7             // Definition der Gehäusemaße
#define  CARABOX_ANZMAXCHASSIS    32             // Max. Anzahl aller Chassis incl. BR,TL
#define  CALCNOOFFREQ            118             // Anzahl CALE-Standardfrequenzsatz
#define  FRQ_INDEX_CA_100_HZ      39             // Index für   100.79368399159 Hz
#define  FRQ_INDEX_CA_16000_HZ   104             // Index für 15050.081200902   Hz
#define  FRQ_INDEX_20480_HZ      108             // Index für 20480 Hz

#define  P0             (double)   2E-5           // Normschalldruck in Pa
#define  CSCHALL        (double)  344.0           // Schallgeschwindigkeit in m/sec
#define  MIN_ACC_DOUBLE (double)  1.0e-13
#define  MIN_ACC_FLOAT  (float)   1.0e-6
#define  M_PI_D_180     (double)  0.01745329251994329
#define  M180_D_PI      (double) 57.29577951308232087

#define  STAND_ALONE    1                        // allein spielfähiger LS
#define  MASTER         2                        // Subwoofer in einem Sateliten-Subwoofersystem
#define  PSEUDO_MASTER  3                        // virtueller Teil eines Subw.  "
#define  SLAVE          4                        // Satellit einer Sat-Sub-Anlage
#define  ADD_ON1        5                        // Subwoofer
#define  ADD_ON2        6                        // Mitteltoneinheit
#define  ADD_ON3        7                        // Hochtoneinheit
                                                 // HIWORD() gilt nur für BOX-Files !

#define  ACTIVE_LS           0x00010000          // Aktiver Lautsprecher
#define  DIPOLE_FLAT         0x00020000          // Dipol Flächenstrahler
#define  WIDTH_ON_BACK       0x00040000          // Breite wird hinten gändert statt oben
#define  DIPOLE_FLAT_H       0x00080000          // Dipol Flächenstrahler Hybrid

#define  NM_CUSTOM_CHASSIS_UPDATE  0x00000100

class CEtsList;
class CBoxPropertySheet;
class CCabinet;

struct NMHDRCUSTOM
{
   NMHDR hdr;
   WPARAM wParam;
   LPARAM lParam;
};

// Strukuren
struct FilterBox
{
   int          nHPOrder;                        // Hochpaß-Filterordnung: 1..4, 0->kein HP, -1->ext.X-over
   int          nHPType;                         // 0->Butterworth,
                                                 // 1->'entdämpft' (d.h. Q variabel, nur für 2./3. Ordnung)
                                                 // 2->Bessel
                                                 // 3->Tschebyscheff mit 0.5 dB Welligkeit
                                                 // 4->Linkwitz-Riley (nur f. 4. Ordnung)
   double       dHPQFaktor;                      // gilt nur für nHPType=1 'entdämpft', sonst dHPQFaktor=0
   double       dHPFreq;                         // HP-Kennfrequenz in Hz (untere Grenzfrequenz)

   int          nTPOrder;                        // Tiefpaß-Filterordnung: 1..4, 0->kein TP, -1->ext.X-over
   int          nTPType;                         // 0->Butterworth,
                                                 // 1->'entdämpft' (d.h. Q variabel, nur für 2./3. Ordnung)
                                                 // 2->Bessel
                                                 // 3->Tschebyscheff mit 0.5 dB Welligkeit
                                                 // 4->Linkwitz-Riley (nur f. 4. Ordnung)
   double       dTPQFaktor;                      // gilt nur für nTPType=1 'entdämpft', sonst dTPQFaktor=0
   double       dTPFreq;                         // TP-Kennfrequenz in Hz (obere Grenzfrequenz)

   Complex      cFilterBox[CALCNOOFFREQ];        // komplexe Filterkoeffizienten, frequenzabhängig
};

#define BOXTEXT_LEN 32
struct BoxText
{
   char     szDescrpt[BOXTEXT_LEN];              // Beschreibung der Box
   char     szTradeMark[BOXTEXT_LEN];            // Markenname
   char     szFilename[256];                     // Dateiname
   char     szComment[256];                      // Kommentar
};


#define NO_OF_FILTER_FRQ 16
struct BasicData
{
   int      nBoxTyp;                             // Closed, BR oder TL
   int      nXoverTyp;                           // 2-Wege etc.
   int      nTypeTTResp;                         // überdämpft, normal, unterdämpft
   int      nNennImped;                          // 4,6,8 Ohm
   double   dImpedLowFreqBR;                     // Res.Freq des unteren Imped-Peaks bei BR/TL
   double   dImpedMidFreqBR;                     // Res.Freq des Imped-Minimums bei BR/TL
   double   dImpedHighFreqBR;                    // Res.Freq des oberen Imped-Peaks bei BR/TL
   double   dMainResFreq;                        // Res.Freq (Peak) einer ClosedBox
   double   dEffectivDBWm;                       // z.B. 87 dB/W/m
   double   dLowLimitFreq;                       // Übertragungsbereich in Hz (UG)
   double   dHighLimitFreq;                      // Übertragungsbereich in Hz (OG)
   // Filterdaten der Chassisgruppen (TT1, TT2, MT1, ...)
   FilterBox   sTT1;                             // Filter Tieftöner 1 (nur TP-Freq, d.h. keine UG)
   FilterBox   sTT2;                             // Filter Tieftöner 2 (nur TP-Freq, d.h. keine UG)
   FilterBox   sMT1;                             // Filter Mitteltöner 1
   FilterBox   sMT2;                             // Filter Mitteltöner 2
   FilterBox   sMT3;                             // Filter Mitteltöner 3
   FilterBox   sHT;                              // Filter Hochtöner   (nur HP-Freq, d.h. keine OG)
   // Anpassungsfaktoren
   double   dCorrectAmplTT12;                    // Anpassungsfaktor für die Chassisgruppe TT1/TT2
   double   dCorrectAmplMT1;                     // Anpassungsfaktor für die Chassisgruppe MT1
   double   dCorrectAmplMT2;                     // Anpassungsfaktor für die Chassisgruppe MT2
   double   dCorrectAmplMT3;                     // Anpassungsfaktor für die Chassisgruppe MT3
   double   dCorrectAmplHT;                      // Anpassungsfaktor für die Chassisgruppe HT
   Complex  cTotalCorrect[CALCNOOFFREQ];         // über alle Abstrahlrichtungen geltende Ampl-Korr
   Complex  cZeroDegCorrect[CALCNOOFFREQ];       // im Bereich um 0° geltende Ampl-Korrektur (vom Benutzer)
   Complex  cSPTotalZeroStandard[CALCNOOFFREQ];  // Schalldruck der gesamten Box in 0°-Standardrichtung
   long     nNoOfLowerHPFrequ;                   // Anzahl der Grenzfrequenzen
   long     nSelLowerHPFrequ;                    // Selektierter Index für die Grenzfrequenz
   float    fLowerHPFrequ[NO_OF_FILTER_FRQ];     // Grenzfrequenzen für aktive bandbegrenzte Lautsprecher
   long     nNoOfUpperTPFrequ;                   // Anzahl der Grenzfrequenzen
   long     nSelUpperTPFrequ;                    // Selektierter Index für die Grenzfrequenz
   float    fUpperTPFrequ[NO_OF_FILTER_FRQ];     // Grenzfrequenzen für aktive bandbegrenzte Lautsprecher

	void WriteData(CBoxPropertySheet*);
	void ReadData(CBoxPropertySheet*);
	void CountFilterFrequencies();
	void CheckFilterArrayRange();
	void SortFilterArrays(bool bUG);
	void InitData();
public:
	void ResetFilterData();
};

#define  CHASSIS_CABWALL_FLAGS_CHANGED 0x0100   // Mögliche Wände geändert
#define  CHASSIS_2D_POS_INVALID        0x0200   // 2D-Position ungültig
#define  CHASSIS_3D_POS_INVALID        0x0400   // 3D-Position ungültig
#define  CHASSIS_POS_CHANGED           0x0800   // Position geändert
#define  CHASSIS_VAS_VALID             0x1000   // dödelkram
#define  CHASSIS_POS_VALID             0x2000   // Position gültig
#define  CHASSIS_POS_ON_2_WALLS        0x4000   // Chassis schneidet mehrere Wände

class ChassisData
{
public:
	bool IsPositionValid();
   ChassisData();
   ~ChassisData();

	void Draw(HDC, bool, CVector2D*);
	RECT GetRect(HDC, bool, CVector2D*);

	int  CheckPositionParam();
	int  CheckChassisParam();
	double& SetDistFromCenter();
	double& SetHeightOnCabWall();

	ChassisData * GetCoupled(int);
	void SetNoOfCoupledChassis(int, CEtsList*);
	int  GetNoOfCoupled()  {return m_nCoupled;};
	WORD* GetCabWallFlags(int);
	const void** GetPtrCoupled(){return (const void**)m_ppCoupled;};

	bool IsRound();
	bool IsSphere();
	bool IsRect();
	bool IsHidden();
	bool IsDummy();
	bool AdoptChassisEffic();
	bool Calculate();
   bool ShowZeroSpl();
   bool CalcTransferFunc();
   bool CalcFilter();
   int  GetFirstDummy();
   int  GetMembrTyp() {return LOWORD(nMembrTyp);}
   int  GetWall()     {return LOWORD(nCabWall);};
	int  GetCabinet()  {return HIWORD(nCabWall);};

	void AdaptCoupledSize();
	void SetHidden(bool);
	void SetDummy(bool);
	void SetAdoptChassisEffic(bool);
	void SetCalculation(bool);
	void SetShowZeroSpl(bool);
   void SetCalcTransferFunc(bool);
   void SetCalcFilter(bool);
   void SetFirstDummy(int);
	void SetMembrTyp(int);
	void SetCorrAmplToSlave();
   void SetWall(int);
	void SetCabinet(int);

   void WriteData(CBoxPropertySheet*);
	void ReadData(CBoxPropertySheet*);

   static int CountValidListChassis(void *, WPARAM, LPARAM);
	static int CountSpheres(void*, WPARAM,LPARAM);
	static int CountDipoles(void*, WPARAM,LPARAM);
   static int FindChassisOfType(void *, WPARAM, LPARAM);
   static int CheckChassisVersion(void *, WPARAM, LPARAM);
   static int CountChassisOnWall(void *, WPARAM, LPARAM);
   static int ChangeCabinetTo(void *, WPARAM, LPARAM);

   char     szDescrpt[BOXTEXT_LEN];
   int      nChassisTyp;                         // LS-Chassistyp (TT, MT, HT, BR, ...)
   int      nMembrTyp;                           // Art der Membran (Konus, Kalotte, Horn, Flach, ...)
   int      nCabWall;                            // Einbauwand (vorne, hinten, rechts, ...)
   int      nPolung;                             // +1 -> normal, -1 -> umgepolt
   double   dEffDiameter;                        // effektiver Membrandurchmesser (Sickenmitte) in mm
   double   dEffBreite;                          // effektive Breite bei rechteckigen Membranen in mm
   double   dEffHoehe;                           // effektive Hoehe bei rechteckigen Membranen in mm
   CVector  vPosChassis;                         // Die Positionskoordinaten in mm, bezogen auf den
                                                 // Gehäusemittelpunkt, Koordinatensystem = CALE
   char     szDirectFileName[32];                // Directivity-File-Name aus CALE (ohne Pfad und ohne Ext)
   double   dResFreqMounted;                     // ResFreq des eingebauten Chassis in Hz
   double   dResFreqFrei;                        // ResFreq des Chassis im Freifeld in Hz
   double   dQFact;                              // zu ResFreqClosed gehöriger Q-Faktor
   double   dInduReal;                           // in mH, Induktivitätskoeffizienten auf 4 Ohm bezogen
   double   dInduImag;                           // in mH, Induktivitätskoeffizienten auf 4 Ohm bezogen
   double   dInduExpReal;                        // Induktivitätskoeffizient (Exponent)
   double   dInduExpImag;                        // Induktivitätskoeffizient (Exponent)
   double   dCorrectAmpl;                        // Anpassungsfaktor für Membranbeschlg. durch Flächenunter-
                                                 // schiede der Chassis innerhalb einer Gruppe (TT1/2, MT1..)
   Complex  cAmplAccel[CALCNOOFFREQ];            // Membranbeschleunigung in m/s² (Mechanik + Filter)
   Complex  cSPZeroStandard[CALCNOOFFREQ];       // Schalldruck des Chassis in 0°-Standardrichtung

   DWORD    dwFlagsCH;                           // Flags für mögliche Boxenwände
   double   dDistFromCenter;                     // Abstand vom Mittelpunkt
   double   dHeightOnCabWall;                    // Höhe auf der LS-Wand
   bool     bDataOk;                             // 
   float    fVAS;                                // Äquivalentes Volumen

   CCabinet **m_ppCabinets;                      // 
private:
   int      m_nCoupled;                          // Anzahl der Chassis
   ChassisData **m_ppCoupled;                    // zugehörige Chassis
};


//*******************************************************************
//*** Globale Struktur für Float-Vektoren ***************************
//*******************************************************************

struct PointFloat
{
   float   x;
   float   y;
   float   z;
};

class CAutoDisable
{
public:
   CAutoDisable(HWND hwnd)
   {
      if ((hwnd != NULL) && ::IsWindowEnabled(hwnd))
      {
         m_hwnd = hwnd;
         ::EnableWindow(m_hwnd, false);
      }
      else m_hwnd = NULL;
   }
   ~CAutoDisable()
   {
      if (m_hwnd) ::EnableWindow(m_hwnd, true);
   }
private:
      HWND m_hwnd;
};

#endif  // __STRUCTURES_H_INCLUDED
