#if !defined(AFX_AUDIOPROPERTIESDLG_H__F4530721_2B05_4A1E_8661_6D7E22AB8E80__INCLUDED_)
#define AFX_AUDIOPROPERTIESDLG_H__F4530721_2B05_4A1E_8661_6D7E22AB8E80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAudio dialog
class CIPCAudioIdipClient;

class CDlgAudio : public CSkinDialog
{
// Construction
public:
	CDlgAudio(CIPCAudioIdipClient*pInput,CIPCAudioIdipClient*pOutput, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAudio)
	enum  { IDD = IDD_AUDIO_PROPERTIES };
	CSkinSlider		m_cThreshold;
	CSkinSlider		m_cOutputVolume;
	CSkinSlider		m_cInputSensitivity;
	CSkinComboBox	m_cNamedInputs;
	CSkinComboBox	m_cAudioInput;
	int		m_nStereoChannel;
	//}}AFX_DATA
	bool        m_bReduced;
	CString     m_sInput;

	void SetSecID(CSecID secID) { m_SecIDinput = secID; };


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAudio)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAudio)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureSliderAuInputSensitivity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderAuOutputVolume(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboNamedInputs();
	afx_msg void OnSelchangeComboAuInputChannels();
	afx_msg void OnDestroy();
	afx_msg void OnReleasedcaptureSliderAuThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRdStereoChannel();
	//}}AFX_MSG
	afx_msg LRESULT OnNotifyWindowI(WPARAM, LPARAM);
	afx_msg LRESULT OnNotifyWindowO(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	LRESULT OnNotifyWindow(CIPCAudioIdipClient*, WPARAM, LPARAM);
	void InitCtrls();
	CIPCAudioIdipClient *m_pControlInput;
	CIPCAudioIdipClient	*m_pControlOutput;
	CSecID			m_SecIDinput;
	CSecID			m_SecIDoutput;
	bool			m_bStopEncoding;
	bool			m_bSensitivityInitialized;
	bool			m_bVolumeInitialized;		 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOPROPERTIESDLG_H__F4530721_2B05_4A1E_8661_6D7E22AB8E80__INCLUDED_)
