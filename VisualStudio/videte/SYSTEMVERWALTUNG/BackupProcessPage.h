#if !defined(AFX_BACKUPPROCESSPAGE_H__3A5250F3_CD62_11D2_B596_004005A19028__INCLUDED_)
#define AFX_BACKUPPROCESSPAGE_H__3A5250F3_CD62_11D2_B596_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackupProcessPage.h : header file
//

#include "SVPage.h"
/////////////////////////////////////////////////////////////////////////////
// CBackupProcessPage dialog
class CBackupProcessPage : public CSVPage
{
// Construction
public:
	CBackupProcessPage(CSVView* pParent);   // standard constructor

// Overrides
public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackupProcessPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitProcessList();
	void InsertProcesses();
	int  InsertProcess(CProcess* pProcess);

	void InsertArchives();
	void InsertDrives();

	void ControlToProcess();

	void EnableExceptNew();
	void SelectItem(int i);

	// Generated message map functions
	//{{AFX_MSG(CBackupProcessPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckDelete();
	afx_msg void OnSelchangeCombo();
	afx_msg void OnChangeEditName();
	afx_msg void OnItemchangedListBackupProcess(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


// Dialog Data
protected:
	//{{AFX_DATA(CBackupProcessPage)
	enum { IDD = IDD_PROZ_BACKUP };
	CEdit	m_editName;
	CComboBox	m_comboArchive;
	CComboBox	m_comboDrive;
	CComboBox	m_comboTimeSpan;
	CListCtrl	m_listProcesses;
	CString	m_sName;
	int		m_iArchive;
	int		m_iDrive;
	BOOL	m_bDelete;
	//}}AFX_DATA
private:
	CInputList*			m_pInputList;
	CProcessList*		m_pProcessList;
	CArchivInfoArray*	m_pArchivList;
	CIPCDrives*		  m_pDrives;

	CProcess*	m_pSelectedProcess;
	int			m_iSelectedItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKUPPROCESSPAGE_H__3A5250F3_CD62_11D2_B596_004005A19028__INCLUDED_)
