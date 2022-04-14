#if !defined(AFX_SELECTBACKUPDRIVEDIALOG_H__30D1DCA3_CCA4_11D2_B590_004005A19028__INCLUDED_)
#define AFX_SELECTBACKUPDRIVEDIALOG_H__30D1DCA3_CCA4_11D2_B590_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectBackupDriveDialog.h : header file
//
class CBackupDialog;
/////////////////////////////////////////////////////////////////////////////
// CSelectBackupDriveDialog dialog

class CSelectBackupDriveDialog : public CDialog
{
// Construction
public:
	CSelectBackupDriveDialog(CBackupDialog* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectBackupDriveDialog)
	enum { IDD = IDD_SELECT_BACKUP_DRIVE };
	CListCtrl	m_ctrlDrives;
	//}}AFX_DATA

public:
	CString GetRootString();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectBackupDriveDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitDrives();
	void InsertDrives();
	int  InsertDrive(CIPCDrive* pDrive);
	int	 GetTypImage(UINT dt);
	CString GetTypString(UINT dt);

	// Generated message map functions
	//{{AFX_MSG(CSelectBackupDriveDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageList		  m_Images;
	CBackupDialog*	  m_pParent;
	CString			  m_sRoot;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTBACKUPDRIVEDIALOG_H__30D1DCA3_CCA4_11D2_B590_004005A19028__INCLUDED_)
