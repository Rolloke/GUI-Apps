#if !defined(AFX_AUDIOOUTPUTDLG_H__CCACE9DD_F664_4BEE_B1C4_DFB341BF9453__INCLUDED_)
#define AFX_AUDIOOUTPUTDLG_H__CCACE9DD_F664_4BEE_B1C4_DFB341BF9453__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioOutputDlg.h : header file
//

class CIPCAudioVision;
/////////////////////////////////////////////////////////////////////////////
// CAudioOutputDlg dialog

class CAudioOutputDlg : public CDialog
{
// Construction
public:
	CAudioOutputDlg(CIPCAudioVision*, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAudioOutputDlg)
	enum { IDD = IDD_AUDIO_OUPUT_PROP };
	CSliderCtrl	m_cVolume;
	//}}AFX_DATA

	void SetSecID(CSecID secID) { m_SecID = secID; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioOutputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAudioOutputDlg)
	afx_msg void OnReleasedcaptureSliderAuOutputVolume(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnNotifyWindow(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	CIPCAudioVision *m_pCIPC;
	CSecID                m_SecID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOOUTPUTDLG_H__CCACE9DD_F664_4BEE_B1C4_DFB341BF9453__INCLUDED_)
