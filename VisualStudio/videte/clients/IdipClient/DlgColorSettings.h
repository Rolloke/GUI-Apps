// DlgColorSettings.h : header file
//
#ifndef __DlgColorSettings_H_
#define __DlgColorSettings_H_

class CWndLive;
/////////////////////////////////////////////////////////////////////////////
// CDlgColorSettings dialog

class CDlgColorSettings : public CSkinDialog
{
// Construction
public:
	CDlgColorSettings(CWnd* pParent, int iColor,  int iBrightness,  int iContrast);

// Dialog Data
	//{{AFX_DATA(CDlgColorSettings)
	enum { IDD = IDD_CSB };
	CSkinSlider m_Contrast;
	CSkinSlider	m_Color;
	CSkinSlider	m_Brightness;
	CString		m_sTxtCamServer;
	//}}AFX_DATA

public:
	void ActualizeSettings(int iColor,int iBrightness, int iContrast);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColorSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWndLive*	m_pWndLive;
	int			m_iBrightness;
	int			m_iColor;
	int			m_iContrast;
	BOOL		m_bTracking;
};
/////////////////////////////////////////////////////////////////////////////
#endif
