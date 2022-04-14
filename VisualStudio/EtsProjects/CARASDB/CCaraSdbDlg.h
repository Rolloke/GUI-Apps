#if !defined __SELECT_BOX_DLG_H_INCLUDED_
#define __SELECT_BOX_DLG_H_INCLUDED_

#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#define INCLUDE_ETS_HELP
#include "CEtsDlg.h"
#include "CTreeView.h"
#include "CCaraWalk.h"
#include "LsBoxDb_cfg.h"
#include "CCaraSdb.h"
#include "CEtsList.h"                            // Headerdatei für CEtsList einbinden

#define NO_OF_BUTTON_ICONS         6            // Anzahl der Buttons mit Icon
#define NO_OF_SPEAKER_TYPES        7            // Anzahl der Lautsprechertypen

                                                // Lautsprechertypen
#define  STAND_ALONE       1                    // Bit 1, allein spielfähiger LS
#define  MASTER            2                    // Bit 2, Subwoofer in einer Sat-Sub-Anlage
#define  PSEUDO_MASTER     3                    // Bit 3, virtueller Teil eines Subw.  "
#define  SLAVE             4                    // Bit 4, Satellit einer Sat-Sub-Anlage
#define  ADD_ON1           5                    // Bit 5, Subwoofer
#define  ADD_ON2           6                    // Bit 6, Mitteltoneinheit
#define  ADD_ON3           7                    // Bit 7, Hochtoneinheit

// LOWORD: Bit-Feld für die Lautsprechertypen   
// im HIWORD steht die Position für die Felder gm_nBoxtype und gm_nBoxDescr
#define  MAIN_SPEAKER      (DWORD)0x00010001    // STAND_ALONE,               Hauptlautsprecher
#define  CENTER_SPEAKER_F  (DWORD)0x00020001    // STAND_ALONE,               Centerlautsprecher Front
#define  CENTER_SPEAKER_R  (DWORD)0x00040001    // STAND_ALONE,               Centerlautsprecher Rear
#define  EFFECT_SPEAKER    (DWORD)0x00080001    // STAND_ALONE,               Effektlautsprecher
#define  ALL_UNITS         (DWORD)0x00100003    // Alle sind wählbar,         Beschallungslautsprecher außer Zusatzlautsprechern
#define  SAT_SUB_SYSTEM    (DWORD)0x00200002    // MASTER,                    Sateliten / Subwoofersysteme
#define  ADD_ON_SPEAKER    (DWORD)0x00400070    // ADD_ON1, ADD_ON2, ADD_ON3, Zusatzlautsprecher
#define  SLAVE_SPEAKER     (DWORD)0x00000008    // nicht wählbarer Typ
// im LOWORD stehen die Bits für die Lautsprechertypen
#define  BOX_TYPE(S)       (1 << (S-1))         // Bitmaske für Lautsprechertypen

#define  NO_NAMEBOX        " NoName"            // Lautsprecher der Marke NoName

#define  IMAGE_NONAME          0                // ImageID für NoName-Hersteller
#define  IMAGE_MANUFACTURER    1                // ImageID für Namenhafte Hersteller
#define  IMAGE_USER_DEFINED    2                // ImageID für User-Defined-Hersteller
#define  IMAGE_BOXES           3                // ImageID für Boxen
#define  IMAGE_SLAVES          4                // ImageID für Slaves
#define  IMAGE_SLAVES_MAIN     5                // ImageID für Slaves als Hauptlautsprecher
#define  IMAGE_SLAVES_CENTER_F 6                // ImageID für Slaves als Centerlautsprecher
#define  IMAGE_SLAVES_CENTER_R 7                // ImageID für Slaves als Centerlautsprecher
#define  IMAGE_SLAVES_EFFECT   8                // ImageID für Slaves als Effektlautsprecher
#define  IMAGE_USER_BOXES      9                // ImageID für User-Defined-Boxen

#define  MAIN_SPEAKER_BITS     0x0000000f       // SourceConfig Bits für die Hauptlautsprecher
#define  CENTER_SPEAKER_F_BITS 0x00000030       // SourceConfig Bits für die Centerlautsprecher vorne
#define  CENTER_SPEAKER_R_BITS 0x000000c0       // SourceConfig Bits für die Centerlautsprecher hinten
#define  EFFECT_SPEAKER_BITS   0x00001f00       // SourceConfig Bits für die Effektlautsprecher
#define  ADD_ON_SPEAKER_BITS   0x00002000       // SourceConfig Bits für die AddOnlautsprecher
#define  ALL_UNITS_BITS        0x00004000       // SourceConfig Bits für die Beschallungslautsprecher
#define  NO_OF_SPEAKER_BITS    15               // Anzahl der Bits

#define  GET_CROSS_REFERENCE  1                 // Belegen der zugeordneten Lautsprecher
#define  GET_ALL_UNITS        2                 // Belegen der Beschallungs Lautsprecher

#define  BUTTON_TIMER_EVENT 0x0ade              // TimereventID für die Button- bzw. Updatesteuerung
#define  BUTTON_TIMER_DELAY    60               // Wiederholungszeit des Timers

union SourceConfig
{
   SourceConfiguration sc;
   UINT                sf;
};


class CCaraSdbDlg: public CEtsDialog
{
public:
   CCaraSdbDlg();
   CCaraSdbDlg(HINSTANCE, int, HWND);
   ~CCaraSdbDlg();

   void UpdateDataBase(char*);
	void SetBoxData(char*);

private:
   void Constructor();
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnCancel();
   virtual int    OnOk(WORD);
   virtual int    OnCommand(WORD, WORD, HWND);
   virtual int    OnMessage(UINT, WPARAM, LPARAM);
   virtual void   OnEndDialog(int);

   void     OnSelectChangeBoxType(HWND);
   void     OnSelectChangedBox(NM_TREEVIEW *pNmTv = NULL);
   void     OnContextMenu(HWND, POINT&);
   LRESULT  OnTimer(WPARAM nIDEvent);
   void     OnDropFiles(HDROP hDrop);

   void     DeleteSdBData();
   bool     MasterError(BoxData *);
   void     SaveDataBase();
   void     ReadDataBase();
   void     BuildNewTree();
   void     ReportFileError(HANDLE&, char *);
   bool     CheckMasterConfig(HTREEITEM, int nMode = 0, int *pnCount = NULL);
   void     DeleteBoxFileByItem(HWND , HTREEITEM);
	void     SelectMaterial(HWND, TVITEM&);
   void     DeleteBoxByName(char *);
   void     LoadBoxFiles();
   bool     UpdateDataBase();
   unsigned UpdateDataBaseThread();
   void     UpdateManufacturer(BoxData*);
   Manufacturer *FindManufacturer(char *);

   static unsigned __stdcall UpdateDataBaseThread(void *);
   static unsigned __stdcall WatchBoxFolderThread(void *);

   static void DeleteBoxFiles(BoxData *, bool bMaster = true, bool bSlaves = true);
   static void GetLsBoxPath(char*);
   static void CopyFileData(HANDLE, HANDLE, long);
   static int  CompareBoxes(const void *elem1, const void *elem2);
	static int  DeleteItemByParam(HWND, HTREEITEM, WPARAM, LPARAM);
	static int  InsertBox(HWND, HTREEITEM, WPARAM, LPARAM);
   static void InsertManufacturer(HWND, TVINSERTSTRUCT&, TVINSERTSTRUCT&, Manufacturer*, bool);
   static void InsertBox(HWND, TVINSERTSTRUCT &, TVINSERTSTRUCT &, BoxData *);
   static BOOL CloseHandle(HANDLE&);

public:
	void SetDownloadPath(char*);
	void GetDownloadPath();
	int CopyForStaticCtrl(LPTSTR, LPCTSTR);
   CARASDB_PARAMS *m_pCaraSdbP;
   int             m_nError;

private:
   CTreeView       m_BoxTree;
   CEtsList        m_Manufacturer;
   CEtsList        m_BoxData;
   CCaraWalk      *m_pBoxView;
   bool            m_bViewDataOk;
   int             m_nButtonTimer;
   CARAWALK_BoxPos m_BoxPos;
   HTREEITEM       m_hSelectedBox;
   DWORD           m_nBoxTypes;
   int             m_nMaxSelectableBoxes;
   SourceConfig    m_MasterConfig;
   volatile HANDLE m_hThread;
   volatile DWORD  m_nThreadID;
   volatile bool   m_bWatchBoxInvalid;
   HANDLE          m_hWatchBoxFolder;
   HANDLE          m_hWatchBoxFolderThread;
   bool            m_bCanUpdateBoxView;
   HWND            m_hwndGLView;
   char           *m_pszOldDownloadPath;
private:
   static int   gm_nImageID[ NO_OF_BUTTON_ICONS];
   static int   gm_nButtonID[NO_OF_BUTTON_ICONS];
   static DWORD gm_nBoxtype[ NO_OF_SPEAKER_TYPES];
   static int   gm_nBoxDescr[NO_OF_SPEAKER_TYPES];
   static const char gm_szIniSection[];
protected:
	void UpdateAngle();
	static void DeleteManufacturerData(void*);
	static void DeleteBoxData(void*);
};

void * __cdecl operator new(unsigned int);

void __cdecl operator delete( void *);

void Se_TranslatorFnc(unsigned int , _EXCEPTION_POINTERS*);

#endif