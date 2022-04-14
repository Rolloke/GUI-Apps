#if !defined(AFX_DATABASEFIELDPAGE_H__73B25980_45D9_4FA4_A4BF_EE64B660307A__INCLUDED_)
#define AFX_DATABASEFIELDPAGE_H__73B25980_45D9_4FA4_A4BF_EE64B660307A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DatabaseFieldPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CDatabaseFieldPage dialog
class CDatabaseFieldPage : public CSVPage
{
// Construction
public:
	CDatabaseFieldPage(CSVView* pParent);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatabaseFieldPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();
protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDatabaseFieldPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckUse();
	afx_msg void OnBtnDefaultValues();
	afx_msg void OnChangeEditName();
	afx_msg void OnChangeEditLength();
	afx_msg void OnSelchangeComboType();
	afx_msg void OnItemchangedListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditID();
	afx_msg void OnBnClickedBtnDbfDefaultValuesAll();
	afx_msg void OnBnClickedBtnDbfCreateDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void    EnableExceptNew();
	void	FillDatabaseFieldListBox();
	void	ClearDatabaseFieldListBox();
	int		InsertItem(CIPCField* pDatabaseField);
	void	SelectItem(int i);
	void	SelectedItemToControl();
	void	OnChange();
private:
	void		InitDatabaseListControl();
	void		FillTypeCombo();
	CIPCField*	AddNew();
	BOOL		IsDefaultField(CIPCField* pField);
	CString		GetDefaultValue(CString sName);

// Dialog Data
protected:
	//{{AFX_DATA(CDatabaseFieldPage)
	enum { IDD = IDD_DATABASE_FIELDS };
	CEdit	m_editID;
	CStatic	m_txtID;
	CStatic	m_txtType;
	CStatic	m_txtName;
	CStatic	m_txtLength;
	CStatic	m_txtDefault;
	CButton	m_btnDefaultValues;
	CComboBox	m_cbType;
	CEdit	m_editLength;
	CEdit	m_editName;
	CButton	m_btnUse;
	CListCtrl	m_DatabaseFields;
	BOOL	m_bUse;
	CString	m_sName;
	DWORD	m_dwLength;
	CString	m_sDefaultValue;
	int		m_iType;
	CString	m_sID;
	//}}AFX_DATA
	CIPCFields*	m_pDatabaseFields;
	int			m_iSelectedItem;
	CIPCField*	m_pSelectedDatabaseField;
	CMapStringToString	m_mapTypes;

	CDWordArray* m_pSDIProtocols;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATABASEFIELDPAGE_H__73B25980_45D9_4FA4_A4BF_EE64B660307A__INCLUDED_)
