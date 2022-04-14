#if !defined(AFX_OPTIONSDIALOG_H__4E8431E3_0905_4746_9A95_13501370CC16__INCLUDED_)
#define AFX_OPTIONSDIALOG_H__4E8431E3_0905_4746_9A95_13501370CC16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog

class COptionsDialog : public CDialog
{
// Construction
public:
	COptionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDialog)
	enum { IDD = IDD_OPTIONS };
	CComboBox	m_comboFPS;
	CEdit	m_editWidth;
	CEdit	m_editHeight;
	BOOL	m_bDecodeToHDD;
	BOOL	m_bDecodeToScreen;
	BOOL	m_bBrightness;
	BOOL	m_bNoiseReduction;
	BOOL	m_bH263;
	int		m_iScale;
	UINT	m_iHeight;
	UINT	m_iWidth;
	//}}AFX_DATA
	UINT	m_iFPS;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDialog)
	afx_msg void OnCheckDecodeScreen();
	afx_msg void OnCheckH263();
	afx_msg void OnCheckDecodeHdd();
	afx_msg void OnRadioNoScale();
	afx_msg void OnRadioScaleCif();
	afx_msg void OnRadioScaleFree();
	afx_msg void OnRadioScaleQcif();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDIALOG_H__4E8431E3_0905_4746_9A95_13501370CC16__INCLUDED_)
