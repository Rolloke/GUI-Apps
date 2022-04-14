// CSBDialog.h : header file
//
#ifndef __CSBDialog_H_
#define __CSBDialog_H_

class CDisplayWindow;
/////////////////////////////////////////////////////////////////////////////
// CCSBDialog dialog

class CCSBDialog : public CDialog
{
// Construction
public:
	CCSBDialog(CWnd* pParent, int iColor,  int iBrightness,  int iContrast);

// Dialog Data
	//{{AFX_DATA(CCSBDialog)
	enum { IDD = IDD_CSB };
	CSliderCtrl	m_Contrast;
	CSliderCtrl	m_Color;
	CSliderCtrl	m_Brightness;
	CString		m_sTxtCamServer;
	//}}AFX_DATA

public:
	void ActualizeSettings(int iColor,int iBrightness, int iContrast);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSBDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCSBDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CDisplayWindow* m_pDisplayWindow;
	int			m_iBrightness;
	int			m_iColor;
	int			m_iContrast;
};
/////////////////////////////////////////////////////////////////////////////
#endif
