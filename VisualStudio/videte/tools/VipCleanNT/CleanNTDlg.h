#if !defined(AFX_CLEANNTDLG_H__0D4B44F6_C5C4_11D4_8C97_004005A1D890__INCLUDED_)
#define AFX_CLEANNTDLG_H__0D4B44F6_C5C4_11D4_8C97_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CleanNTDlg.h : header file
//
#ifdef _IDIP

#include "VipCleanReg.h"
#include "IdipCleanDlg.h"
#include "Psapi.h."

/////////////////////////////////////////////////////////////////////////////
// CCleanNTDlg dialog

class CCleanNTDlg : public CDialog
{
// Construction
public:
	CCleanNTDlg(CIdipCleanDlg* pIdip, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCleanNTDlg)
	enum { IDD = IDD_CLEANNT };
	CEdit	m_ctrlSetHDDID;
	CEdit	m_ctrlSysOverview;
	CEdit	m_ctrlFileList;
	CButton	m_ctrlCreate;
	CButton	m_ctrlSaveHDDID;
	CStatic	m_ctrlStaticList;
	CStatic	m_ctrlStaticSys;
	CStatic	m_ctrlStaticOrdner;
	CStatic	m_ctrlHDDLine;
	CEdit	m_ctrlHDD2;
	CEdit	m_ctrlHDD;
	CStatic	m_ctrlStaticHDD;
	CStatic	m_ctrlSeconds;
	CEdit	m_ctrlLoaderTimeout;
	CStatic	m_ctrlLoader;
	CButton	m_ctrlUebernehmen;
	CStatic	m_ctrlStaticNero;
	CEdit	m_ctrlNero6;
	CEdit	m_ctrlNero5;
	CEdit	m_ctrlNero4;
	CEdit	m_ctrlNero3;
	CEdit	m_ctrlNero2;
	CEdit	m_ctrlNero1;
	CEdit	m_ctrlEditXPE;
	CStatic	m_ctrlStaticXPE;
	CStatic	m_ctrlStaticOEMMiddle;
	CStatic	m_ctrlStaticOEM;
	CStatic	m_ctrlOSInUse;
	CEdit	m_cResult;
	CEdit	m_cStatus;
	CEdit	m_cInfo_OrdnerDatei;
	CEdit	m_cInfo_KeineOrdnerDatei;
	CEdit	m_cInfo_KeineAenderungen;
	CEdit	m_cInfo_Aenderungen;
	CEdit	m_ControlOrga;
	CEdit	m_ControlCompName;
	CEdit	m_ControlOEM3;
	CEdit	m_ControlOEM2;
	CEdit	m_ControlOEM1;
	CButton	m_CheckSystemOverview;
	CButton	m_CheckFileList;
	CButton	m_CheckDeskFolder;
	CEdit	m_ControlDeskFolder;
	CString	m_sOEM1;
	CString	m_sOEM2;
	CString	m_sOEM3;
	CString	m_sCompName;
	CString	m_sOrga;
	CString	m_sLoaderTimeout;
	CString	m_sDeskFolder;
	CString	m_sFileList;
	CString	m_sSysOverview;
	CString	m_sInfo_Aenderungen;
	CString	m_sInfo_KeineAenderungen;
	CString	m_sInfo_KeineOrdnerDatei;
	CString	m_sInfo_OrdnerDatei;
	CString	m_sStatus;
	CString	m_sResult;
	CString	m_sOSInUse;
	CString	m_sNero1;
	CString	m_sNero2;
	CString	m_sNero3;
	CString	m_sNero4;
	CString	m_sNero5;
	CString	m_sNero6;
	CString	m_sEditXP;
	CString	m_sHDD;
	CString	m_sHDD2;
	CString	m_sSetHDDID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCleanNTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void	OnOK();
	virtual void	OnCancel();
	//}}AFX_VIRTUAL


// Implementation
public:
	void Start();
	void InitCleanNTDlg();
	inline CString GetStringDiagnoseDlg();
	inline void SetHandleDiagnoseDlg(HWND hWnd);
	inline CString GetStringBerichtDlg();
	inline CString GetStringBerichtSpeichernDlg();
	inline void SetHandleBerichtSpeichernMsg(HWND hWnd);
	inline HWND GetHandleDiagnoseDlg();
	inline void SetHandleErrorMsgBox(HWND hWnd);
	inline CString GetStringBerichtErstellenDlg();
	inline void SetHandleBerichtErstellenDlg(HWND hWnd);
	inline HWND GetHandleBerichtErstellenDlg();
	inline void SetBoolFoundDlg(BOOL bValue);
	inline BOOL GetBoolFoundDlg();
	inline void SetBoolBerichtEnde(BOOL bValue);
	void GetXPDataFromReg();


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCleanNTDlg)
	afx_msg void OnUebernehmen();
	afx_msg void OnChangeOem1();
	afx_msg void OnChangeOem2();
	afx_msg void OnChangeOem3();
	afx_msg void OnChangeCompname();
	afx_msg void OnChangeOrga();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusOem2();
	afx_msg void OnSetfocusOem3();
	afx_msg void OnSetfocusOrga();
	afx_msg void OnSetfocusCompname();
	afx_msg void OnCheckSystemOverview();
	afx_msg void OnCheckFileList();
	afx_msg void OnKillfocusOem1();
	afx_msg void OnKillfocusOem2();
	afx_msg void OnKillfocusOem3();
	afx_msg void OnChangeEditNero1();
	afx_msg void OnChangeEditNero2();
	afx_msg void OnChangeEditNero3();
	afx_msg void OnChangeEditNero4();
	afx_msg void OnChangeEditNero5();
	afx_msg void OnChangeEditNero6();
	afx_msg void OnKillfocusEditNero1();
	afx_msg void OnKillfocusEditNero2();
	afx_msg void OnKillfocusEditNero3();
	afx_msg void OnKillfocusEditNero4();
	afx_msg void OnKillfocusEditNero5();
	afx_msg void OnKillfocusEditNero6();
	afx_msg void OnChangeEditXp();
	afx_msg void OnChangeLoadertimeout();
	afx_msg void OnCheckDeskFolder();
	afx_msg void OnChangeEditHdd();
	afx_msg void OnChangeEditHdd2();
	afx_msg void OnCheckHdd();
	afx_msg void OnBnClickedBtnCfInit();
	afx_msg void OnBnClickedBtnInitDisks();
	afx_msg void OnBnClickedBtnCfUninit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



// Implementation
private:
	BOOL StartWinDiagnose(CString sWinDiagnose);
	BOOL StartWinDiagnoseXP(CString sWinDiagnose);
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	void ClickDruckenButton();
	void SaveWinDiagnose();
	void EndWinDiagnose();
	CString GetNTLoaderTimeout();
	BOOL Check_AllEntries(int &iResult);
	BOOL Check_SameNames(int &iResult);
	BOOL Check_NTCheckBoxes(int &iResult);
	void GetNewEntries();
	void SaveAllEntries();
	void SaveSameNames();
	void SetRadioOnWinDiagnoseDlg();
	BOOL PrintTreeRecursiv(CString sStartDir, 
						   CString sResultFile,
						   BOOL bDelete);
	void ShowStatus();
	void GetAndSaveVolumeID();
	void InitStrings();
	void MoveTempOverviewToDesktopFolder();
	void CryptNewHDDID();
	BOOL SaveHDDID();

//Members
private:

typedef struct NT_ENTRIES
{
	CString sOEM1;
	CString sOEM2;
	CString sOEM3;
	CString sOEMMid;
	CString sCompName;
	CString sOrga;
	CString sLoaderTimeout;
	CString sDeskFolder;
	CString sSysOverview;
	CString sFileList;
	CString sXPeLicence;
	CString sNero1;
	CString sNero2;
	CString sNero3;
	CString sNero4;
	CString sNero5;
	CString sNero6;
};

typedef struct STATUS
{
	BOOL bAenderungen;
	BOOL bOrdnerDatei;
};

	CIdipCleanDlg*	m_pIdipCleanDlg;
	CVipCleanReg	m_Reg;
	CString			m_sDiagnoseDlg;
	CString			m_sBerichtDlg;
	CString			m_sBerichtSpeichern;
	CString			m_sBerichtErstellenDlg;
	CString			m_sOEMMid;
	CString			m_sPathToDesktop;	
	CString			m_sPathToMSInfo;
	CString			m_sMSInfoCommandLineParam;
	CString			m_sTempOverview;
	HWND			m_hDiagnose;
	HWND			m_hBerichtSpeichern;
	HWND			m_hBerichtSpeichernMsg;
	HWND			m_hErrorMsgBox;
	HWND			m_hBerichtErstellenDlg;
	BOOL			m_bFoundDlg;
	BOOL			m_bBerichtEnde;
	DWORD			m_dwDruckenButton;
	DWORD			m_dwDatei;
	DWORD			m_dwAlleRegs;
	DWORD			m_dwZusammenfassung;
	DWORD			m_dwDruckButton;
	DWORD			m_dwEditDateiName;
	DWORD			m_dwSpeicherButton;
	NT_ENTRIES		m_NTDefaults;
	NT_ENTRIES		m_NTNewValues;
	STATUS			m_STATUS;
	CString			m_sSystem32Path;
	CString			m_sNeroLicense;
	int				m_iNeroVersion;
	CString			m_sNewHDDID;
	BOOL			m_bSaveHDD;
	BOOL			m_bSaveAsSerialNumberKey;
public:
};



/////////////////////////////////////////////////////////////////////////////
inline CString CCleanNTDlg::GetStringDiagnoseDlg()
{
	return m_sDiagnoseDlg;
}

/////////////////////////////////////////////////////////////////////////////
inline CString CCleanNTDlg::GetStringBerichtDlg()
{
	return m_sBerichtDlg;
}

/////////////////////////////////////////////////////////////////////////////
inline void CCleanNTDlg::SetHandleDiagnoseDlg(HWND hWnd)
{
	m_hDiagnose = hWnd;
}

/////////////////////////////////////////////////////////////////////////////
inline CString CCleanNTDlg::GetStringBerichtSpeichernDlg()
{
	return m_sBerichtSpeichern;
}

/////////////////////////////////////////////////////////////////////////////
inline void CCleanNTDlg::SetHandleBerichtSpeichernMsg(HWND hWnd)
{
	m_hBerichtSpeichernMsg = hWnd;
}

/////////////////////////////////////////////////////////////////////////////
inline HWND CCleanNTDlg::GetHandleDiagnoseDlg()
{
	return m_hDiagnose;
}

/////////////////////////////////////////////////////////////////////////////
inline void CCleanNTDlg::SetHandleErrorMsgBox(HWND hWnd)
{
	m_hErrorMsgBox = hWnd;
}

/////////////////////////////////////////////////////////////////////////////
inline CString CCleanNTDlg::GetStringBerichtErstellenDlg()
{
	return m_sBerichtErstellenDlg;
}

/////////////////////////////////////////////////////////////////////////////
inline void CCleanNTDlg::SetHandleBerichtErstellenDlg(HWND hWnd)
{
	m_hBerichtErstellenDlg = hWnd;
}

/////////////////////////////////////////////////////////////////////////////
inline void CCleanNTDlg::SetBoolFoundDlg(BOOL bValue)
{
	m_bFoundDlg = bValue;
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL CCleanNTDlg::GetBoolFoundDlg()
{
	return m_bFoundDlg;
}

/////////////////////////////////////////////////////////////////////////////
inline void CCleanNTDlg::SetBoolBerichtEnde(BOOL bValue)
{
	m_bBerichtEnde = bValue;
}	

/////////////////////////////////////////////////////////////////////////////
inline HWND	CCleanNTDlg::GetHandleBerichtErstellenDlg()
{
	return m_hBerichtErstellenDlg;
}
#endif // _IDIP

//{{AFX_INSERT_LOCATION}}
	inline void SetBoolFoundDlg(BOOL bValue);// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLEANNTDLG_H__0D4B44F6_C5C4_11D4_8C97_004005A1D890__INCLUDED_)
