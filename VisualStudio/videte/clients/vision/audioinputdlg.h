#if !defined(AFX_AUDIOINPUTDLG_H__72E67E7D_3674_4085_B798_0A1147FD44D2__INCLUDED_)
#define AFX_AUDIOINPUTDLG_H__72E67E7D_3674_4085_B798_0A1147FD44D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioInputDlg.h : header file
//

class CIPCAudioVision;
/////////////////////////////////////////////////////////////////////////////
// CAudioInputDlg dialog


class CAudioInputDlg : public CDialog
{
// Construction
public:
	CAudioInputDlg(CIPCAudioVision *pCIPC, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAudioInputDlg)
	enum { IDD = IDD_AUDIO_INPUT_PROP };
	CSliderCtrl	m_cSensitivity;
	CComboBox	m_cNamedInputs;
	//}}AFX_DATA
	void SetSecID(CSecID secID) { m_SecID = secID; };


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitCtrls();
	void SelectNamedInput();

	
	// Generated message map functions
	//{{AFX_MSG(CAudioInputDlg)
	afx_msg void OnReleasedcaptureSliderAuInputSensitivity(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboNamedInputs();
	//}}AFX_MSG
	afx_msg LRESULT OnNotifyWindow(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	CIPCAudioVision *m_pCIPC;
	CSecID                m_SecID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOINPUTDLG_H__72E67E7D_3674_4085_B798_0A1147FD44D2__INCLUDED_)
