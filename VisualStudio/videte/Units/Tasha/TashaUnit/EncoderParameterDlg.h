#if !defined(AFX_ENCODERPARAMETERDLG_H__82F9CBCD_2A9D_4111_9F9B_FFCB5AD99E59__INCLUDED_)
#define AFX_ENCODERPARAMETERDLG_H__82F9CBCD_2A9D_4111_9F9B_FFCB5AD99E59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncoderParameterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEncoderParameterDlg dialog
class CCodec;
class CEncoderParameterDlg : public CDialog
{
// Construction
public:
	CEncoderParameterDlg(CWnd* pParent = NULL, CCodec* pCodec = NULL);   // standard constructor
	BOOL IsVisible(){return m_bVisible;}
	BOOL OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	BOOL OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	void ShowConfigDlg(HWND hWndParent, WORD wSource);
	void HideConfigDlg();

// Dialog Data
	//{{AFX_DATA(CEncoderParameterDlg)
	enum { IDD = IDD_ENCODER_PARAM };
	int		m_nBitrate;
	int		m_nCamera;
	int		m_nFPS;
	int		m_nIFrameIntervall;
	int		m_nProzessID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncoderParameterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEncoderParameterDlg)
	afx_msg void OnSendEncoderParameter();
	virtual void OnOK();
	afx_msg void OnResolutionHigh();
	afx_msg void OnResolutionMid();
	afx_msg void OnResolutionLow();
	afx_msg void OnMpeg4();
	afx_msg void OnYuv422();
	afx_msg long OnUpdate(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateDlg();

private:
	BOOL			m_bVisible;
	CCodec*			m_pCodec;
	EncoderParam	m_EncParam;
protected:
public:
	afx_msg void OnReadEncoderParameter();
	afx_msg void OnBnClickedReqSingleField();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCODERPARAMETERDLG_H__82F9CBCD_2A9D_4111_9F9B_FFCB5AD99E59__INCLUDED_)
