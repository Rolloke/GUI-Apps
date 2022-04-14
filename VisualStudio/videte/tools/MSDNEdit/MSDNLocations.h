#if !defined(AFX_MSDNLOCATIONS_H__E45309CE_9F52_4877_8564_036CE69053F6__INCLUDED_)
#define AFX_MSDNLOCATIONS_H__E45309CE_9F52_4877_8564_036CE69053F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MSDNLocations.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMSDNLocations dialog
struct MSDNDocLoc;

class CMSDNLocations : public CDialog
{
// Construction
public:
	CMSDNLocations(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMSDNLocations)
	enum { IDD = IDD_LOCATIONS };
	CSpinButtonCtrl	m_cSpin;
	int		m_nNum;
	int		m_nLocNum;
	CString	m_strLocPath;
	CString	m_strName;
	CString	m_strTitle;
	CString	m_strVolume;
	//}}AFX_DATA

   CPtrArray    *m_pPtrList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSDNLocations)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMSDNLocations)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeLocEdtNum();
	afx_msg void OnChange();
	afx_msg void OnBtnLocNew();
	afx_msg void OnBtnLocDelete();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SaveChanged();
   MSDNDocLoc *m_pDocLoc;
   bool        m_bChanged;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSDNLOCATIONS_H__E45309CE_9F52_4877_8564_036CE69053F6__INCLUDED_)
