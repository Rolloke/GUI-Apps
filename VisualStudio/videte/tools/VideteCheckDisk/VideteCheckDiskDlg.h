// VideteCheckDiskDlg.h : Headerdatei
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "InfoDlg.h"

#define DISABLE 2


class CStringMap
{
public:
    typedef std::map<int, CString> indexed_map;
    typedef indexed_map::iterator iterator;
    typedef indexed_map::const_iterator const_iterator;

    CStringMap();

    const CString& get_value(const int pos) const;
    const CString& operator[](const int pos);

    CString& set_value(int pos);
    void     set_value(int  pos, const CString& value);

    void     clear();
    iterator get_pos(int pos);
    iterator begin();
    iterator end();

private:

    indexed_map mMap;
};

// CVideteCheckDiskDlg Dialogfeld
class CVideteCheckDiskDlg : public CDialog
{
	friend class CInfoDlg;
// Konstruktion
public:
	CVideteCheckDiskDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	virtual ~CVideteCheckDiskDlg();	

// Dialogfelddaten
public:
	enum { IDD = IDD_VIDETECHECKDISK_DIALOG };
protected:
	// common
	CEdit			m_edtOutput;
	CProgressCtrl	m_cProgress;
	CTabCtrl		m_TabCtrl;
	BOOL			m_bCompactFlash;
	CString			m_sDontCheck;
	int				m_nPageFileSize;
	int				m_nCFDisk;
	int				m_nCFPartition;
	int				m_nProductType;
	BOOL			m_bCreateAutoPageFile;
	// chkdsk
	BOOL		m_bCheckAfterReboot;
	BOOL		m_bFixErrors;
	BOOL		m_bShowAddInfo;
	BOOL		m_bLocateAndRecoverBadSectors;
	BOOL		m_bForceDismounting;
	BOOL		m_bCheckIndexEntries;
	BOOL		m_bCheckCyclicFolderStruct;
    CStringMap  m_Pathes;

	//{{AFX_VIRTUAL(CVideteCheckDiskDlg)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
public:
	void CheckHidden();
private:
	BOOL SetOutSourcePath(CWK_Profile&wkp, LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue=NULL, int nType=0);
	void DoExitWindows(UINT uFlags);
	void SetInfoText(LPCTSTR sInfo, int nTimeout=350);
	void SetInfoText(UINT nIDinfo, int nTimeout=350);
	void PartitionAndFormatFinished();
	void CheckDiskFinished();
	void SwitchToTab(int nTab);
	void CreateDirectory(LPCTSTR sDir);
	void TraceOutputString(LPCTSTR sOut);
	BOOL SetFirstFixedDriveToCheckEdt();
	void AddToFormatList(CString sDrive, const CString&sVolume);
	void StartFormat();
    void InitPathes();

// Implementierung
protected:

	// general
	CWKControlledProcess*	m_pProcess;
	CFont					m_FixedFont;
	HICON					m_hIcon;
	CString					m_strTipText;
	CInfoDlg*				m_pInfoDlg;
	DWORD					m_dwSelfCheck;
	// Diskpart and format
	CStringArray m_saFreeDrives,
				 m_saAssigned,
				 m_saDisks,
				 m_saPartitions;

	CString		 m_sCurrentDisk,
				 m_sFormating;
	int			m_nDisks;
	int			m_nLinesCounted;
	UINT		m_nTimer;
	BOOL		m_bCloseDialog;;
public:
	BOOL m_bAutoMode;
	BOOL m_bAutoCheckDisk;
	BOOL m_bAutoEnableEwf;
	BOOL m_bReboot;
	// Checkdisk
public:
	// Paths
	CString m_sCheckDrives;
	CString m_sLogFilesSource;
	CString m_sLogFilesDestination;
	BOOL	m_bCheckRegKeys;
	CWK_Profile*m_pDelete;
	// identifier strings
protected:
	CString		m_sPartition; 
	CString		m_sExtended;
	CString		m_sFinnished;
	CString		m_sPercentDone;
	CString		m_sYesNo;
	CString		m_sYes;
	CString		m_sNo;
	BOOL		m_bTraceAllOutputStrings;

	// Generierte Funktionen für die Meldungstabellen
	//{{AFX_MSG(CVideteCheckDiskDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCreateDisks();
	afx_msg void OnBnClickedCheckDisks();
	afx_msg void OnBnClickedCreatePaths();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSaveSettings();
	afx_msg BOOL OnToolTipNotify(UINT /*id*/, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCkLocateAndRecoverBadSectors();
	afx_msg void OnBnClickedCkForceDismount();
	afx_msg void OnBnClickedBtnCheckCfReg();
	afx_msg void OnBnClickedBtnClearOutputWindow();
	afx_msg void OnBnClickedCkCompactFlash();
	afx_msg void OnChanged();
	afx_msg LRESULT OnUser(WPARAM,LPARAM);
	afx_msg LRESULT OnUserProcess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelfCheck(WPARAM,LPARAM);
	afx_msg void OnEnChangeEdtCheckDrives();
	afx_msg void OnBnClickedBtnCheckPathes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnRestorePathes();
};

