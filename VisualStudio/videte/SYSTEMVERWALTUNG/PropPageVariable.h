#if !defined(AFX_PROPPAGEVARIABLE_H__4C2A2E33_8C5D_49AE_B850_434CC6B6F1D0__INCLUDED_)
#define AFX_PROPPAGEVARIABLE_H__4C2A2E33_8C5D_49AE_B850_434CC6B6F1D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPageVariable.h : header file
//
/*
class CMyHtmlView: public CHtmlView
{
	DECLARE_DYNCREATE(CMyHtmlView)
      CMyHtmlView(){};
   
	//{{AFX_MSG(CMyHtmlView)
   afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

int CMyHtmlView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
   return nResult;
}

IMPLEMENT_DYNCREATE(CMyHtmlView, CHtmlView)

BEGIN_MESSAGE_MAP(CMyHtmlView, CHtmlView)
	//{{AFX_MSG_MAP(CMyHtmlView)
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/
/////////////////////////////////////////////////////////////////////////////
// CPropPageVariable dialog
#define CREATE_NEW_DATA      0x00000001
#define ALLOW_NEW_DATA       0x00000002
#define DELETE_EXISTING_DATA 0x00000004
#define ALLOW_DELETE_DATA    0x00000008
#define INIT_ON_NEXT         0x00000010
#define INIT_ON_FINISH       0x00000020
#define ENABLE_PAGE          0x00000040
#define ENABLE_TREE          0x00000080
#define DONT_VISIT_TWICE     0x00000100

#define PAGEID_IS_SEC_ID     0x00010000

class CSVPropertySheet;
class CPropPageVariable;

typedef BOOL (*VARIABLE_PP_FC)(CPropPageVariable*, LPARAM) ;

class CPropPageVariable : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPageVariable)

// Construction
public:
	CPropPageVariable();	// Construction
	~CPropPageVariable();	// destruction
// Initialisation
	void Init(int nPageID, int nTextID, DWORD dwFlags=0, LPARAM lParam=0, VARIABLE_PP_FC pfnI=NULL, VARIABLE_PP_FC pfnSA=NULL, VARIABLE_PP_FC pfnN=NULL);
	void SetPageTextID(LPCTSTR sPageTextID);
// Attributes
public:
	int		GetPageID();
	void	SetPageID(int);
	int		GetTextID();

protected:
	CSVPropertySheet* GetSVPropertySheet();

protected:
// Dialog Data
	//{{AFX_DATA(CPropPageVariable)
	enum { IDD = IDD_PROPPAGE_VARIABLE };
	CString	m_strWizardText;
	CString	m_strWizardHeading;
	//}}AFX_DATA
   int  m_nWizardPageID;
   int  m_nWizardTextID;
   CString m_sPageTextID;

   bool m_bCreateNew;
   bool m_bCreated;
   bool m_bAllowNew;

   bool m_bDeleteExisting;
   bool m_bDeleted;
   bool m_bAllowDelete;

   bool m_bInitializeOnNext;
   bool m_bInitializeOnFinish;

   bool m_bDisableTree;
   bool m_bDisablePage;
   
   bool m_bVisited;
   bool m_bDontVisitTwice;
   bool m_bPageIDisSecID;
   bool m_bDummy;

   LPARAM m_lParam;
   VARIABLE_PP_FC m_pfnInit;
   VARIABLE_PP_FC m_pfnSetActive;
   VARIABLE_PP_FC m_pfnNext;
   CFont  m_HeadingFont;
// Overrides
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageVariable)
	public:
	virtual BOOL OnSetActive();
	virtual void OnCancel();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropPageVariable)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnGetPageParam(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SVView.cpp
inline CSVPropertySheet* CPropPageVariable::GetSVPropertySheet()
{
	return (CSVPropertySheet*)GetParent();
}
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_PROPPAGEVARIABLE_H__4C2A2E33_8C5D_49AE_B850_434CC6B6F1D0__INCLUDED_)
