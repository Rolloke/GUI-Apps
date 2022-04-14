// SVPropertySheet.h: interface for the CSVPropertySheet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVPROPERTYSHEET_H__C74C79B0_6D44_4C1C_ACA0_1FED69739BA5__INCLUDED_)
#define AFX_SVPROPERTYSHEET_H__C74C79B0_6D44_4C1C_ACA0_1FED69739BA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CSVPropertySheet
class CPropPageVariable;
class CSVView;

#define ALLOW_NEW    1
#define ALLOW_DELETE 2
#define ALLOW_SAVE	 3
#define ALLOW_CANCEL 4
#define ALLOW_INIT   5

class CSVPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSVPropertySheet)

// Construction
public:
	CSVPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSVPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
	BOOL Create(CWnd*);

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVPropertySheet)
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool	AllowDelete();
	bool	AllowNew();
	bool	AllowSave();
	bool	AllowCancel();
	bool	AllowInit();
	bool	IsDeleting();
	CPropPageVariable* InsertPage(CPropPageVariable*);
	void	PressLastButton();
	void	SetLastButton(int nID);

	void     DeletePages();
	CSVView* GetSVView();
	virtual ~CSVPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSVPropertySheet)
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

private:
	CPtrList	m_PageList;
	int			m_nLastBtnClicked;
	bool		m_bDeleting;
	bool		m_bInitialized;
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_SVPROPERTYSHEET_H__C74C79B0_6D44_4C1C_ACA0_1FED69739BA5__INCLUDED_)
