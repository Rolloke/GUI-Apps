// AnalyzerBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnalyzerBar dialog
#include "InitDialogBar.h"

class CAnalyzerBar : public CInitDialogBar
{
// Construction
public:
	CAnalyzerBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnalyzerBar)
	enum { IDD = CG_IDD_SECANALYZERBAR };
	CCheckListBox	m_ctlKnownGroups;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalyzerBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	public:
		void UpdateGroups();
		void UpdateUsedGroups();
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnalyzerBar)
	afx_msg void OnLastReset();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeKnownGroups();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
