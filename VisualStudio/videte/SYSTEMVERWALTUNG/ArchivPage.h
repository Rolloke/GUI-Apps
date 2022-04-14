/* GlobalReplace: CDVRDrive --> CIPCDrive */
#if !defined(AFX_ARCHIVPAGE_H__4505D623_7575_11D2_B4EA_00C095EC9EFA__INCLUDED_)
#define AFX_ARCHIVPAGE_H__4505D623_7575_11D2_B4EA_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchivPage.h : header file
//

#include "SVPage.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CArchivPage dialog

class CArchivPage : public CSVPage
{
// Construction
public:
	CArchivPage(CSVView* pParent);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchivPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();
	static int GetTypImage(ArchivType at);
protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CArchivPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeName();
	afx_msg void OnChangeEditSize();
	afx_msg void OnSelchangeComboAlarm();
	afx_msg void OnRadioAlarm();
	afx_msg void OnRadioRing();
	afx_msg void OnRadioSafering();
	afx_msg void OnRadioAlarmList();
	afx_msg void OnSelchangeComboFixedDrive();
	afx_msg void OnClickArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditMaxTime();
	afx_msg void OnBnClickedAutoReduceArchiveSize();
	afx_msg void OnBnClickedCkDynamicExpandArchives();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void    EnableExceptNew();
	void	SelectItem(int i);
	void	FillArchivListBox();
	int		InsertItem(CArchivInfo* pAI);
	void	ClearArchivListBox();
	BOOL	CheckActivationsWithArchiv(CArchivInfo* pArchiveInfo);
	void	FillAlarmArchivCombo();
	void	FillDriveCombo();
	CString	GetTypString(ArchivType at);
	void	SetNewTyp(ArchivType at);
	void	SelectedItemToControl();
	void	OnChange();
	void	CopyActualSettingsToNew(CArchivInfo* pAI);
	void	UpdateSizeInfo();


// Dialog Data
private:
	//{{AFX_DATA(CArchivPage)
	enum { IDD = IDD_ARCHIV };
	CStatic	m_txtMaxTime;
	CEdit	m_editMaxTime;
	CComboBox	m_comboFixedDrive;
	CComboBox	m_comboAlarm;
	CButton	m_radioRing;
	CButton	m_radioAlarm;
	CButton	m_radioVorring;
	CButton m_radioAlarmList;
	CEdit	m_editSize;
	CEdit	m_editName;
	CListCtrl	m_Archivs;
	CString	m_sName;
	int		m_iType;
	UINT	m_dwSize;
	CString	m_sMax;
	CString	m_sUsed;
	CString	m_sWhole;
	DWORD	m_dwMaxTime;
	CButton m_ctrlDynamicExpandArchive;
	//}}AFX_DATA
	CArchivInfoArray* m_pArchivs;
	CIPCDrives*		  m_pDrives;
	CInputList*		  m_pInputList;	
	CHostArray*		  m_pHostArray;
	CDWordArray		  m_ProcessesToDelete;

	int				  m_iSelectedItem;
	CArchivInfo*	  m_pSelectedArchivInfo;
	CImageList		  m_Images;

	BOOL	m_bActivationWereDeleted;
	BOOL	m_bHostArchivesWereDeleted;
	BOOL	m_bAutoReduceArchiveSize;
	BOOL	m_bDynamicExpandArchiveSize;	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVPAGE_H__4505D623_7575_11D2_B4EA_00C095EC9EFA__INCLUDED_)
