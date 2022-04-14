#pragma once

#include "SVPage.h"
#include "resource.h"
#include "afxwin.h"
#include "EdtSubItemListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMiscellaneousEntry
class CMiscellaneousEntry
{
public:
// Construction
	CMiscellaneousEntry();
	CMiscellaneousEntry(CString sName, CString sSection, CString sKey,
		DWORD dwDefault, WORD wRegType=REG_DWORD,
		WORD wValueType = 0, WK_ApplicationId wai=WAI_INVALID);
	CMiscellaneousEntry(UINT nIDName, CString sSection, CString sKey,
		DWORD dwDefault, WORD wRegType=REG_DWORD,
		WORD wValueType = 0, WK_ApplicationId wai=WAI_INVALID);
	~CMiscellaneousEntry();

// Implementation
	BOOL Load(CWK_Profile &wkp);
	BOOL Save(CWK_Profile &wkp);

// Dialog Data
	CString	m_sSection;
	CString	m_sKey;
	CString	m_sValue;
	CString	m_sName;
	DWORD	m_dwValue;
	DWORD	m_dwDefaultValue;
	WORD	m_wRegType;
	WORD	m_wValueType;
	BOOL	m_bChanged;
	WK_ApplicationId m_Wai;
};

/////////////////////////////////////////////////////////////////////////////
// CMiscellaneousList
class CMiscellaneousList : public CEdtSubItemListCtrl
{
public:
	CMiscellaneousList();
	virtual ~CMiscellaneousList();

	int InsertColumn(LPCTSTR pszName, int nWidth);
	int InsertEntry(CMiscellaneousEntry*pEntry);
	void Load(CWK_Profile &wkp);
	void Save(CWK_Profile &wkp);
	const CStringArray& GetInitApplications() const;

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiscellaneousList)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL CanEditSubItem(int, int nSubItem);
	virtual eEditType GetEditType(int nItem, int nSubItem);
	virtual void InitComboBox(CSubItemCombo*pCombo, const CString& sText);
	virtual DWORD GetItemStyle(int nItem, int nSubItem, DWORD dwStyle);
	virtual void DrawSubItem(CDC*pDC, int nItem, int nSubItem, CRect&rcSubItem, CString sText, LPARAM lParam);
	//}}AFX_VIRTUAL
public:

	// Generated message map functions
	//{{AFX_MSG(CMiscellaneousList)
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int*	m_pnWidth;
	CStringArray m_saInitApps;
};

class CMiscellaneousPage : public CSVPage
{
	DECLARE_DYNAMIC(CMiscellaneousPage)

// Construction
public:
	CMiscellaneousPage(CSVView* pParent = NULL);   // standard constructor
	virtual ~CMiscellaneousPage();

// Dialog Data
	//{{AFX_DATA(CSMSPage)
	enum { IDD = IDD_MISCELLANEOUS_PAGE };
	CMiscellaneousList m_List;
	//}}AFX_DATA



// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiscellaneousPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void Initialize();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL StretchElements();
	//}}AFX_VIRTUAL


protected:
	// Generated message map functions
	//{{AFX_MSG(CMiscellaneousPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
