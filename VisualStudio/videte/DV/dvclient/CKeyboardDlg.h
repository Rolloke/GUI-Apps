#include "afxwin.h"
#if !defined(AFX_CKEYBOARDDLG_H__58540BF3_E834_11D3_8E33_004005A11E32__INCLUDED_)
#define AFX_CKEYBOARDDLG_H__58540BF3_E834_11D3_8E33_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CKeyboardDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKeyboardDlg dialog
class CDisplay;

class CKeyboardDlg : public CTransparentDialog
{
	DECLARE_DYNAMIC(CKeyboardDlg)
	friend class	CDVUIThread;
// Construction
public:
	CKeyboardDlg(CDisplay* pDisplay, const CString &sFormat, const CString &sInput, CWnd* pParent =NULL);  
	virtual ~CKeyboardDlg();

// Attributes
public:
	CString GetInputString();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyboardDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnButtonPressed(const CString &sChar);
	void MoveButtons();
	void ChangeKeyBoard();

	// Generated message map functions
	//{{AFX_MSG(CKeyboardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBackSpace();
	afx_msg void OnButtonShift();
	afx_msg void OnBnClickedButtonKblang();
	afx_msg void OnButtonSpace();
	afx_msg void OnButtonPlus();
	afx_msg void OnButtonMinus();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnButtonRange(UINT);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Dialog Data
	//{{AFX_DATA(CKeyboardDlg)
	enum { IDD = IDD_KEYBOARD };
	CSkinButton	m_ctrlButtonUE;
	CSkinButton	m_ctrlButtonOE;
	CSkinButton	m_ctrlButtonAE;
	CSkinButton	m_ctrlButtonSC1;
	CSkinButton	m_ctrlButtonSC2;
	CSkinButton	m_ctrlButtonSC3;
	CSkinButton	m_ctrlButtonSpace;
	CSkinButton	m_ctrlButtonPlus;
	CSkinButton	m_ctrlButtonMinus;
	CSkinButton	m_ctrlButtonShift;
	CSkinButton	m_ctrlButtonBackSpace;
	CSkinButton	m_ctrlButtonOK;
	CSkinButton	m_ctrlButton0;
	CSkinButton	m_ctrlButton1;
	CSkinButton	m_ctrlButton2;
	CSkinButton	m_ctrlButton3;
	CSkinButton	m_ctrlButton4;
	CSkinButton	m_ctrlButton5;
	CSkinButton	m_ctrlButton6;
	CSkinButton	m_ctrlButton7;
	CSkinButton	m_ctrlButton8;
	CSkinButton	m_ctrlButton9;
	CSkinButton	m_ctrlButtonQ;
	CSkinButton	m_ctrlButtonW;
	CSkinButton	m_ctrlButtonE;
	CSkinButton	m_ctrlButtonR;
	CSkinButton	m_ctrlButtonT;
	CSkinButton	m_ctrlButtonZ;
	CSkinButton	m_ctrlButtonU;
	CSkinButton	m_ctrlButtonI;
	CSkinButton	m_ctrlButtonO;
	CSkinButton	m_ctrlButtonP;
	CSkinButton	m_ctrlButtonA;
	CSkinButton	m_ctrlButtonS;
	CSkinButton	m_ctrlButtonD;
	CSkinButton	m_ctrlButtonF;
	CSkinButton	m_ctrlButtonG;
	CSkinButton	m_ctrlButtonH;
	CSkinButton	m_ctrlButtonJ;
	CSkinButton	m_ctrlButtonK;
	CSkinButton	m_ctrlButtonL;
	CSkinButton	m_ctrlButtonY;
	CSkinButton	m_ctrlButtonX;
	CSkinButton	m_ctrlButtonC;
	CSkinButton	m_ctrlButtonV;
	CSkinButton	m_ctrlButtonB;
	CSkinButton	m_ctrlButtonN;
	CSkinButton	m_ctrlButtonM;
	//}}AFX_DATA
private:
	CString		m_sFormat;
	CString		m_sInput;
	COLORREF	m_BaseColor;
	CDisplay*	m_pDisplay;
	CWnd*		m_pPanel;
	CImageList  m_ilBackSpace;
	CImageList  m_ilShift;
	CImageList	m_ilEnter;
	BOOL		m_bShift;
	BOOL		m_bKBLang;
	CSkinButtons	m_DVButtons;
	HCURSOR		m_hCursor;
	HACCEL		m_hAccelTable;
	bool        m_bInitializedBtn;
	CSkinButton *m_pCirconFlexe;
	CSkinButton *m_pAigu;
	CSkinButton *m_pGrave;
	CSkinButton *m_pEnje;
	CSkinButton *m_pDiaerese;
	CSkinButton m_btnKbLang;
public:
	CString m_sNotAllowedChars;
	BOOL	m_bOnlyAsscii;
	BOOL	m_bIPallowed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CKEYBOARDDLG_H__58540BF3_E834_11D3_8E33_004005A11E32__INCLUDED_)
