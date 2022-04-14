#if !defined(AFX_DlgSelectBackupDrive_H__30D1DCA3_CCA4_11D2_B590_004005A19028__INCLUDED_)
#define AFX_DlgSelectBackupDrive_H__30D1DCA3_CCA4_11D2_B590_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectBackupDrive.h : header file
//
class CDlgBackup;
/////////////////////////////////////////////////////////////////////////////
// CDlgSelectBackupDrive dialog

class CDlgSelectBackupDrive : public CSkinDialog
{
// Construction
public:
	CDlgSelectBackupDrive(CDlgBackup* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectBackupDrive)
	enum  { IDD = IDD_SELECT_BACKUP_DRIVE };
	CSkinListCtrlX	m_ctrlDrives;
	//}}AFX_DATA

public:
	CString GetRootString();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectBackupDrive)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
public:
	void InsertDrives();
protected:
	void InitDrives();
	int  InsertDrive(CIPCDrive* pDrive);
	int	 GetTypImage(UINT dt);
	CString GetTypString(UINT dt);

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectBackupDrive)
	afx_msg void OnNMClickListe(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageList	m_Images;
	CDlgBackup*	m_pParent;
	CString		m_sRoot;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgSelectBackupDrive_H__30D1DCA3_CCA4_11D2_B590_004005A19028__INCLUDED_)
