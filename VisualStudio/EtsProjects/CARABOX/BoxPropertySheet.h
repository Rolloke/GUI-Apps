#if !defined __CBOX_PROPERTY_SHEET_H_INCLUDED_
#define __CBOX_PROPERTY_SHEET_H_INCLUDED_

#include "CEtsPropertySheet.h"
#include "BoxStructures.h"
#include "CCabinet.h"
#include "CEtsList.h"

class CCaraBoxView;
#define FILE_BUFFERSIZE 512
// Flags für Gültigkeit und Änderung
#define CARABOX_NEW_DATA       0x00000000 // keine PropSheetPage Ok (2)
#define CARABOX_DESCRIPTION    0x00000040 // CBoxDescriptionPage   : Ok
#define CARABOX_BASE_DATA      0x00000001 // CBoxBaseDataPage      : Ok                  / 1. PropSheetPage Ok (1)
#define CARABOX_CROSS_OVER     0x00000002 // CBoxCrossOverPage     : Ok                  / 2. PropSheetPage Ok (2)
#define CARABOX_CHASSIS_NO     0x00000004 // CBoxChassisDataPage   : Chassis Anzahl Ok   / 3. PropSheetPage Chassisanzahl OK (4)
#define CARABOX_CHASSIS_POS    0x00000008 // CBoxCabinetDesignPage : Chassis Position Ok / 3. PropSheetPage OK (8)
#define CARABOX_CHASSIS_DATA   0x00000080 // CBoxChassisDataPage   : Chassis Daten Ok
#define CARABOX_CABINET        0x00000100 // CBoxCabinetDesignPage : Cabinet Ok
#define CARABOX_ALL_DATA       (CARABOX_BASE_DATA|CARABOX_CROSS_OVER|CARABOX_CHASSIS_NO|CARABOX_CHASSIS_DATA|CARABOX_CABINET|CARABOX_CHASSIS_POS)

// Flag für berechnete Daten
#define CARABOX_SPL_ZERO       0x00000010 // SP(L)-Daten in 0°-Standardrichtung berechnet (16)
#define CARABOX_BDD_DATA       0x00000020 // BDD-Daten berechnet (32)

#define CARABOX_SAVE_DATA      0x00000040 // Dateien müssen gespeichert werden
// Flag für die automatische Berechnung (editiert heißt nicht mehr berechnen)
#define CARABOX_IMPED_EDITED   0x00010000 // Flag für Impedanzwerte editiert
#define CARABOX_FILTER2_EDITED 0x00020000 // Filter für TT2 wurde eingestellt
#define CARABOX_FILTER_EDITED  0x00040000 // irgendein Filter wurde eingestellt
// Flag für Abbruch der Berechnung
#define CARABOX_BDD_DELETED    0x00080000 // schon vorhandene BDD-datei wurde gelöscht
// Flag für Dipole mit gekoppelten Chassis
#define CARABOX_DIPOL_RE_INIT  0x00100000 // Die gekoppelten Chassis müssen neu initilisiert werden
#define CARABOX_INIT_STD_FRQ   0x00200000 // Weichenfrequenzen mit Standardwerten initialisieren
#define CARABOX_EXT_FRQ_RNG    0x00400000 // Weichenfrequenzen können sich überschneiden

// Schaltflags für die Berechnung der Daten
#define CARABOX_ADOPT_EFFIC    0x01000000 // Empfindlichkeit anpassen
#define CARABOX_DONT_DEL_BT    0x02000000 // Boxtext nicht Löschen
#define CARABOX_PASS_CHECK     0x04000000 // Check überspringen
#define CARABOX_SAVE_AS        0x08000000 // Speichern der Daten der Box
#define CARABOX_FILTER_SPL     0x10000000 // TransferFkt für SPL-Diagramme
#define CARABOX_FILTER_BDD     0x20000000 // TransferFkt für BDD-Datei
// interne Flags für Änderung
#define CARABOX_CHASSIS_LIST   0x00001000 // Chassis Liste geändert
#define UPDATE_CHASSIS_LIST    0x00002000 // Chassis Liste updaten
#define UPDATE_CABINET_LIST    0x00004000 // Quader Liste updaten
 
#define USERMODE_WALLS         0x00000001 // Enable/disable Walls
#define USERMODE_VOLUME        0x00000002 // Volumeparameter einstellbar
#define USERMODE_THETA         0x00000004 // Drehen in Thetarichtung
#define USERMODE_SURVEY        0x00000008 // Überwachen der Optimierung AdaptEffic
#define USERMODE_FILTER_FRQ    0x00000010 // Filterfrequenzen getrennt einstellbar
#define USERMODE_USER_REF      0x00000020 // Userreferenzpunkt anzeigen
#define USERMODE_ALPHA_CONTEXT 0x00000040 // Weitere Einstellungen für durchsichtige Darstellung

class CBoxPropertySheet: public CEtsPropertySheet
{
   struct SDirectivData
   {
      char      *pszDirectivFileName;
      Complex ***pDirectivData;
   };

   friend class CBoxDescriptionPage;
   friend class CBoxBaseDataPage;
   friend class CBoxCrossOverPage;
   friend class CBoxChassisDataPage;
   friend class CBoxCabinetDesignPage;
   friend class COpenGLView;
   friend class C3DViewDlg;

public:
   CBoxPropertySheet();
   ~CBoxPropertySheet();

   void Create3DView();
	bool ShowThisBox(char *);
	void Close3DView();

	int       GetFileVersion();

   void      WriteFileFH(void*, DWORD);
	void      ReadFileFH(void*, DWORD);
	void      WriteFileBuffered(void*, DWORD);
	void      CloseFile();
	void      SetChanged(int);
	void      SetVersionText(HWND, long, long);

   void      SetAdoptDistance();

	void      TerminateThreadSave();
	void      CloseCalcThreadHandle();

   HWND      GetHWND() {return m_hWnd;};
   HANDLE    GetCalcThreadHandle() {return m_hCalcThread;};
   CEtsList *GetChassisList()      {return &m_ChassisData;};
   FilterBox*GetOneWayFilter();
	DWORD     GetCARADirectory(DWORD, LPTSTR);
	int       GetChassisTypeString(ChassisData*, char*, int);
	CCabinet* GetCurrentCab();
	CCabinet* GetCabinet(int);
   void      InitSphereFoot(ChassisData*);
	LRESULT   SendMsgTo3DDlg(UINT, WPARAM, LPARAM);
	bool      IsSectionChangeable();

   static void SinCos(double dAngleIn, double&dSinOut, double&dCosOut)
   {
      double dSin, dCos;
      __asm
      {
   	   PUSHA                                                 ; Register retten
         FLD      dAngleIn                                     ; Winkel laden
         FSINCOS                                               ; cos und sinus berechnen
         FSTSW  ax                                             ; BugFix für einige 487 Copros
         FSTP     dCos                                         ; cosinuswert ausgeben
         FSTP     dSin                                         ; sinuswert ausgeben
         POPA                                                  ; Register wiederherstellen
      }
      dSinOut = dSin;
      dCosOut = dCos;
   }

	static int    SetPaintCtrlWnd(HWND, int, int);
   static void   CalcFilterBox(FilterBox *, double *);
   static int    CheckChassisPos(void *, WPARAM , LPARAM);

   static       double gm_dFreq[CALCNOOFFREQ];// CALE Standard-Frequenzen
   const static double gm_dDist[4];
   const static double gm_dDefaultTransmisionRanges[5][2];

protected:
	virtual bool OnInitSheet(HWND hWnd);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);

   void  OnBtnClckSPLView();
	bool  OnBtnClckOpen();
	void  OnBtnClckSaveAs();
	void  OnBtnClck3Dview();
	void  OnBtnClckNew();
	void  OnBtnClckUpLoad();
	void  OnBtnClckDownLoad();
   void  OnChangeLSType();
	bool  OnWizardFinish(bool bRequest = true);
	bool  SaveBoxData(char*);
	bool  SaveBDDFile(char*);
	bool  LoadBoxData(const char*);

   void  CheckCabinetDesign(CCabinet*pC=NULL);
	void  SetWizardButtons(bool bBack, bool bNext);

	void  InitData();
//	void  ShowBoxIn3D();
   void  CreateCaraBDD(int nFlag);

private:
   void  WriteBoxFile();                                       // Lesen und schreiben von BOX Daten
   bool  ReadBoxFile(const char *);

	void  StackProcessWindows(int nHow=0);                      // Kontrolle von Kindprozessen
	static int EnumProcesses(void *, WPARAM, LPARAM);

	bool  RequestSaveData();
   bool  CheckAllPages();                                      
	void  ResetAllPages();
	bool  CheckCurrentPage();

	bool   UpdateZeroSPL();                                      // Berechnungs Fkt.
   bool   Allocate3D_SPL();
   int    CalcTransferFunc();
	void   CalcReferencePoints();
   int    CalcBoxChassisData();
   int    CalcZeroSPL();
   void   AdaptEffic();
   double GetDeltaEffic(double *, double);
   double GetCorrAmpl(int, double*);
   int    CalcCaraBDD();
   Complex *** ReadSP_Directiv(char *);
	void    GetThetaPhiIndex(double, double, int&, int&, double&, int&, int&, double&);
   Complex GetSPLDirectiv(Complex *** , int, int, double, int, int, double, int);
	void    GetDirectivityName(double, ChassisData*);

   void  ShowZeroSPL();                                        // Anzeigefkt
   void  ShowSPLPolar2D();
   void  ShowSPLPolar3D(int);

	bool CalcInThread(int);                                     // Threadfkt für Berechnung
	static unsigned int __stdcall CalcThread(void*);

	static int CountChassisPolyPoints(void*, WPARAM,LPARAM);
	static int GetChassisPolygons(void *, WPARAM, LPARAM);
	static int WriteChassisData(void *, WPARAM, LPARAM);
	static int WriteChassisForm(void *, WPARAM, LPARAM);
 	static int DeterminChassisPositions(void *, WPARAM, LPARAM);

// static int GetBoxPolyPointsOfChassis(void *, WPARAM, LPARAM);
// static int CountBoxPolyPointsOfChassis(void*, WPARAM,LPARAM);
// static int FindCoupledChassis(void *, WPARAM, LPARAM);
                                                               // Subclassparameter für Zeichnen von GrpCtrl
   static LRESULT CALLBACK PaintCtrlSubClassProc(HWND, UINT, WPARAM, LPARAM);
   static LONG  gm_lOldGrpCtrlSCProc;
   static LONG  gm_lOldGrpCtrlCount;

	static void DeleteChassisDataFnc(void*);                    // Löschfunktionen für die Listen
	static void DeleteDirectivDataFnc(void*);
   static void DeleteProcessHandlesFnc(void*);

   static int __cdecl FindProcessID(const void *, const void *);// Such/Sortierfunktionen für die Listen
   static int __cdecl SortDirectivData(const void *, const void *);

   static BOOL CALLBACK EnumThreadWndProc(HWND, LPARAM);       // Enum Window Fkt
   static BOOL CALLBACK EnumWindows(HWND, LPARAM);
   static BOOL CALLBACK NotifyChildPages(HWND, LPARAM);

public:
	C3DViewDlg     *m_p3DViewDlg;
   volatile HWND   m_hwndChildDlg;
   int             m_nCurrentCab;
   int             m_nNoMovements;

private:
   BoxText       m_BoxText;
   int           m_nLSType;
   BasicData     m_BasicData;
   CEtsList      m_ChassisData;
   CCabinet      m_Cabinet;

   int           m_nFlagOk;
   int           m_nFlagChanged;
   CCaraBoxView *m_pCaraBoxV;

   CEtsList      m_DirectivData;
   Complex   ****m_ppppcSP_3D;
   double        m_dVolume;

   CFileHeader  *m_pFH;
   HANDLE        m_hFile;
   BYTE          m_Buffer[FILE_BUFFERSIZE];
   int           m_nBufferPointer;
   CEtsList      m_ProcessHandles;
   int           m_nByteCount;
   int           m_nFileVersion;
   double        m_dDistance;

   static char   gm_szCARADir[MAX_PATH];

   volatile HANDLE m_hCalcThread;
   volatile int    m_nCalcWhat;
};

#endif //__CBOX_PROPERTY_SHEET_H_INCLUDED_
