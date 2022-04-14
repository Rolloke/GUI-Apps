#ifndef AFX_CARAWINAPP_H__B46219E2_0C0A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CARAWINAPP_H__B46219E2_0C0A_11D2_9DB9_0000B458D891__INCLUDED_

// CaraWinApp.h : Header-Datei
//
#include "stdafx.h"
#include "CEtsDiv.h"
#include "Label.h"
#include "CMarginDialogParams.h"
#include "CEtsPrtD.h"


// Registry
#define  REGKEY_DEFAULT_BKCOLOR   "BkColor"
#define  REGKEY_DEFAULT_CUST_COL  "CustomColors"

#define  LAST_CMD_MOVE           5
#define  LAST_CMD_CURVE_EDIT     6
#define  MAX_UNDO                50

class CUndoParams
{
public:
   CUndoParams();
   ~CUndoParams();
   void CreateUndoFileName();
   int     nCmd;
   CLabel *pLabel;
   int     nLabel;
   TCHAR   *pszFileName;
   CLabel *pFrom;
   int     nFrom;
   int     nPos;
   CLabel *pTo;
   int     nTo;
};

class AFX_EXT_CLASS CCaraSharedFile : public CSharedFile
{
	DECLARE_DYNAMIC(CCaraSharedFile)

public:
// Constructors
	CCaraSharedFile(UINT nAllocFlags = GMEM_DDESHARE|GMEM_MOVEABLE,
		UINT nGrowBytes = 4096);

// Implementation
public:
	bool FreeBuffer();
	bool UnlockHandle();
	void* LockHandle();
	bool TestMemory(DWORD dwBytes=0xffffffff);
protected:
	virtual BYTE* Alloc(DWORD dwBytes);
	virtual BYTE* Realloc(BYTE* lpMem, DWORD dwBytes);
};

class  CEtsPrtD;
struct ETSPRTD_PARAMETER;

/////////////////////////////////////////////////////////////////////////////
// Thread CCaraWinApp 

class AFX_EXT_CLASS CCaraWinApp : public CWinApp
{
   DECLARE_DYNAMIC(CCaraWinApp)
private:
// Konstruktion, Destruktion
public:
   CCaraWinApp();           // Dynamische Erstellung verwendet geschützten Konstruktor
   virtual ~CCaraWinApp();
// Attribute
public:

// Operationen
public:
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCaraWinApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCaraWinApp)
   afx_msg void OnFilePrintSetup();
   afx_msg void OnEditUndo();
   afx_msg void OnAppAbout();
   afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
   afx_msg void OnFileOpen();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:

   void      SetBackgroundColor(COLORREF c) {m_Backgroundcolor = c;};
   COLORREF  GetBackgroundColor()           {return m_Backgroundcolor;}
	void      PostMsgToCallingWnd();
	static bool DoDebugReport(TCHAR*);
   static void Se_TranslatorFnc(unsigned int, _EXCEPTION_POINTERS*);
   // Clipboard-Funktionen
   void        InsertIntoClipboard(CLabel *, CDC *, bool, bool);
   // Undo-Funktionen
   void        SetUndoCmd(int, CLabel *pl, CLabel *pFrom, int, CLabel *pTo);
   void        RemoveUndoListElement(bool, bool);
   void        DeleteUndoList();
   void        UpdateUndoList(CUndoParams *pUps, CLabel *pl);
	int         GetUndoCount();
   // Printer-Funktionen   
   int         DoCaraPrintDialog(bool bSetup = false);
   bool        GetCaraPrinterDefaults(CPrintInfo* pInfo);
   HDC         CreateCaraPrinterDC(bool bPreview = false);
   bool        IsAPrinterAvailabel();
	void        EndPrinting(bool bSetup = false);
	bool        ReadPrinterData10(CArchive &);
	bool        SetPrinterData(BYTE*, DWORD);
	BYTE*       GetPrinterData(DWORD& dwSize);
	void        GetPageSettings();
	bool        GetPrinter();
	void        DeletePrinterParams();
	bool        AllocatePrinterParams();
	void        SetPageParams(ETSPRTD_PARAMETER*);
   // Registry-Funktionen
   void SavePrinterSettings();
   void LoadPrinterSettings();

   virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName); // open named file

   CEtsPrtD          *m_pPrtD;
   ETSPRTD_PARAMETER *m_pPrtParams;
   SMarginDlgParam    m_Margins;       // sizeof(SMarginDlgParam)
   CCommandLineInfo   m_CmdInfo;
   CString            m_strPreviewHelpPath;
   LPARAM             m_CallingWindowlParam;
   WPARAM             m_CallingWindowwParam;
   HWND               m_hCallingWindowHandle;
   UINT               m_nCallingWindowMessage;
   bool               m_bLightVersion;
   bool               m_bNoUndo;
   bool               m_bToolTipInit;
   static HWND        gm_hCurrentTmeWnd;
private:
   static MEMORYSTATUS  gm_MemStatus;
/* typedef struct _MEMORYSTATUS { // mst 
    DWORD dwLength;        // sizeof(MEMORYSTATUS) 
    DWORD dwMemoryLoad;    // percent of memory in use 
    DWORD dwTotalPhys;     // bytes of physical memory 
    DWORD dwAvailPhys;     // free physical memory bytes 
    DWORD dwTotalPageFile; // bytes of paging file 
    DWORD dwAvailPageFile; // free bytes of paging file 
    DWORD dwTotalVirtual;  // user bytes of address space 
    DWORD dwAvailVirtual;  // free user bytes 
} MEMORYSTATUS, *LPMEMORYSTATUS; */

   static OSVERSIONINFO gm_OsVersionInfo;
/* struct {
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    TCHAR szCSDVersion[ 128 ]; 
} OSVERSIONINFO; */

public:
	void DeletePrinterDLL();
	bool IsLightVersion();
	void CheckPageScaling();
	void SetCurrentProgramDir();

   static bool  IsOnWin95();
   static bool  IsOnWin98();     
   static bool  IsOnWinNT();
   static bool  IsOnWin2000();
   static DWORD GetTotalPhysicalMem()
   {
      return gm_MemStatus.dwTotalPhys;
   };
   static DWORD GetTotalVirtualMem()      
   {
      if (gm_MemStatus.dwTotalVirtual == 0) ::GlobalMemoryStatus(&gm_MemStatus);
      return gm_MemStatus.dwTotalVirtual;
   };
   static DWORD GetTotalPageFileMem()
   {
      if (gm_MemStatus.dwTotalPageFile == 0) ::GlobalMemoryStatus(&gm_MemStatus);
      return gm_MemStatus.dwTotalPageFile;
   };
   static DWORD GetAvailablePhysicalMem() 
   {
      ::GlobalMemoryStatus(&gm_MemStatus);
      return gm_MemStatus.dwAvailPhys;
   };
   static DWORD GetAvailableVirtualMem()
   {
      ::GlobalMemoryStatus(&gm_MemStatus);
      return gm_MemStatus.dwAvailVirtual;
   };
   static DWORD GetAvailablePageFileMem()
   {
      ::GlobalMemoryStatus(&gm_MemStatus);
      return gm_MemStatus.dwAvailPageFile;
   };

   bool              m_bMailAvailable;
protected:
   COLORREF          m_Backgroundcolor;
   CString           m_strDefaultDir;
private:
   int               m_nDeleteCount;
   bool              m_bUndoListOverflow;
   CPtrList          m_UndoList;
//   CEtsDiv           m_PrevDiv;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CARAWINAPP_H__B46219E2_0C0A_11D2_9DB9_0000B458D891__INCLUDED_
