#if !defined(AFX_GUARDTOURPAGE_H__743DB0BB_431D_414C_95A0_4F2DD2526B78__INCLUDED_)
#define AFX_GUARDTOURPAGE_H__743DB0BB_431D_414C_95A0_4F2DD2526B78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GuardTourPage.h : header file
//

#include "StatusList.h"
#include "SVPage.h"
/////////////////////////////////////////////////////////////////////////////
// CGuardTourPage dialog

class CGuardTourPage : public CSVPage
{
private:
	struct LB_DATA
	{
		CString sItem;
		DWORD   dwData;
		BOOL    bCheck;
	};

// Construction
public:
	CGuardTourPage(CSVView* pParent);   // standard constructor

// Overrides
public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL StretchElements();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGuardTourPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableExceptNew();
	void InitProcessList();
	void FillPermissions();
	void FillHosts();
	void FillProcesses();
	int	 InsertProcess(CProcess* pProcess);
	void SelectProcessItem(int i);
	void SelectHostItem(int i);
	void SelectHostItemBySubID(WORD wSubID);
	void CheckHostItemStatus();
	void AddHost();
	void RemoveHost();
	int  InsertHostItem(int, LB_DATA&);
	void RemoveHostItem(int, LB_DATA&);
	void ProcessToControl();
	void ControlToProcess();

	// Generated message map functions
	//{{AFX_MSG(CGuardTourPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckFullScreen();
	afx_msg void OnCheckSave();
	afx_msg void OnSelchangePermission();
	afx_msg void OnChangeSwitchTime();
	afx_msg void OnItemchangedGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeName();
	afx_msg void OnChangeCameras();
	afx_msg void OnCheckExclusive();
	afx_msg void OnCheckDisconnect();
	afx_msg void OnRadioHold();
	afx_msg void OnChangeTries();
	afx_msg void OnSelchangeHosts();
	afx_msg void OnClickGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkGuardtourProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposGtpHostSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
// Dialog Data
	//{{AFX_DATA(CGuardTourPage)
	enum { IDD = IDD_PROZ_GUARD_TOUR };
	CEdit	m_editSwitchTime;
	CButton	m_btnSave;
	CButton	m_btnFullscreen;
	CButton	m_btnForever;
	CButton m_btnHold;
	CButton	m_btnExclusive;
	CButton	m_btnDisconnect;
	CEdit	m_editName;
	CListCtrl	m_listProcesses;
	CEdit	m_editNrOfTries;
	CEdit	m_editCameras;
	CCheckListBox	m_listHosts;
	CComboBox	m_cbPermission;
	BOOL	m_bFullScreen;
	BOOL	m_bSave;
	DWORD	m_dwSwitchTime;
	CString	m_sCameras;
	DWORD	m_dwNrOfTries;
	CString	m_sName;
	BOOL	m_bExclusive;
	BOOL	m_bDisconnect;
	int		m_iHold;
	//}}AFX_DATA
private:
	CProcessList*		m_pProcessList;
	CInputList*			m_pInputList;
	CPermissionArray*	m_pPermissionArray;
	CHostArray*			m_pHostArray;

	// temporary data for current selected process
	CProcess*			m_pSelectedProcess;
	int					m_iSelectedProcessItem;
	int					m_iSelectedHostItem;
	int					m_iSelectedCamerasItem;
	CWordArray			m_waHosts;
	CStringArray		m_saCameras;
	CString           m_sSeconds;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUARDTOURPAGE_H__743DB0BB_431D_414C_95A0_4F2DD2526B78__INCLUDED_)
