#if !defined(AFX_MSDNCOLLECTION_H__0B897751_9AFA_423C_BF38_BF203D01E6D3__INCLUDED_)
#define AFX_MSDNCOLLECTION_H__0B897751_9AFA_423C_BF38_BF203D01E6D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MSDNCollection.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMSDNCollection dialog
struct MSDNDocCol;
class CMSDNCollection : public CDialog
{
// Construction
public:
	CMSDNCollection(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMSDNCollection)
	enum { IDD = IDD_COLLECTIONS };
	CSpinButtonCtrl	m_cSpin;
	int		m_nNum;
	//}}AFX_DATA
   
   CPtrArray    *m_pPtrList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSDNCollection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMSDNCollection)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeColEdtNum();
	afx_msg void OnChange();
	afx_msg void OnBtnColDelete();
	afx_msg void OnBtnColNew();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SaveChanged();
   MSDNDocCol *m_pDocCol;
   bool        m_bChanged;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSDNCOLLECTION_H__0B897751_9AFA_423C_BF38_BF203D01E6D3__INCLUDED_)
