/* GlobalReplace: CDVRDrive --> CIPCDrive */
#if !defined(AFX_DRIVEPAGE_H__65D46D53_779C_11D2_B4EA_00C095EC9EFA__INCLUDED_)
#define AFX_DRIVEPAGE_H__65D46D53_779C_11D2_B4EA_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrivePage.h : header file
//
#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CDrivePage dialog

class CDrivePage : public CSVPage
{
// Construction
public:
	CDrivePage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrivePage)
	enum { IDD = IDD_DRIVE };
	CButton	m_checkAll;
	CEdit	m_editUse;
	CButton	m_radioSystem;
	CButton	m_radioNormal;
	CButton	m_radioDatabase;
	CButton	m_radioBackupWrite;
	CButton	m_radioBackupRead;
	CListCtrl	m_Drives;
	int		m_iUsedAs;
	UINT	m_dwUse;
	CString	m_sRoot;
	CString	m_sDrivesMB;
	CString	m_sArchivsMB;
	BOOL	m_bAll;
	//}}AFX_DATA


// Overrides
public:
	virtual BOOL StretchElements();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrivePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// overrides
protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();

	// Implementation
protected:
	CString GetTypString(UINT dt);
	CString GetUsedString(CIPCDrive* pD);

	void EnableControls();
	void FillDriveListBox();
	int	 InsertItem(CIPCDrive* pD, BOOL bSelectIt = FALSE);
	void SelectItem(int i);
	int	 GetTypImage(UINT dt);
	BOOL SetNewTyp(DvrDriveUse dt);
	int  GetUsedRadio(CIPCDrive* pD);

	void SelectedItemToControl();

	// Generated message map functions
	//{{AFX_MSG(CDrivePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedDrives(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditUse();
	afx_msg void OnRadioBackupRead();
	afx_msg void OnRadioBackupWriteRead();
	afx_msg void OnRadioDatabase();
	afx_msg void OnRadioNormal();
	afx_msg void OnRadioSystem();
	afx_msg void OnAll();
	afx_msg void OnClickDrives(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDrives(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickDrives(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkDrives(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CArchivInfoArray* m_pArchivs;
	CIPCDrives*		  m_pDrives;
	CImageList		  m_Images;
	int				  m_iSelectedItem;
	CIPCDrive*		  m_pSelectedDrive;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEPAGE_H__65D46D53_779C_11D2_B4EA_00C095EC9EFA__INCLUDED_)
