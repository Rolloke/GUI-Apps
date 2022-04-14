#if !defined(AFX_AUDOPAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_)
#define AFX_AUDOPAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SoftwarePage.h : header file
//

#include "SVPage.h"

#define AUDIO_INPUT_DEVICE_GUID  _T("{33d9a762-90c8-11d0-bd43-00a0c911ce86}")
#define AUDIO_OUTPUT_DEVICE_GUID _T("{e0f158e1-cb04-11d0-bd4e-00a0c911ce86}")
#define AUDIO_COMPRESSOR_GUID    _T("{33d9a761-90c8-11d0-bd43-00a0c911ce86}")
#define AUDIO_DEVICE_GUID        _T("{fbf6f530-07b9-11d2-a71e-0000f8004788}")

/////////////////////////////////////////////////////////////////////////////
// CAudioPage dialog
class CIPCControlAudioUnit;

class CAudioPage : public CSVPage
{
	friend class CIPCControlAudioUnit;
	DECLARE_DYNAMIC(CAudioPage)

// Construction
public:
	CAudioPage(CSVView*  pParent, int);   // standard constructor
	virtual ~CAudioPage();   // standard constructor

private:
// Dialog Data
	//{{AFX_DATA(CAudioPage)
	enum { IDD = IDD_AUDIO };
	CSliderCtrl	m_cOutputVolume;
	CProgressCtrl	m_cInputLevelPeak;
	CStatic	m_cActivating;
	CProgressCtrl	m_cInputLevel;
	CComboBox	m_cQuality;
	CSliderCtrl	m_cInputGain;
	CComboBox	m_cInputPins;
	CComboBox	m_cSampFrq;
	CComboBox	m_cRenderer;
	CComboBox	m_cCompressor;
	CComboBox	m_cCapture;
	BOOL			m_b2BytesPerSample;
	int			m_nSequence;
	int			m_nThreshold;
	int			m_nDwellTime;
	BOOL			m_bDetectorActive;
	BOOL			m_bUnitActive;
	int			m_nChannel;
	BOOL	m_bListen;
	BOOL	m_bDefaultRecDev;
	BOOL	m_bDefaultPlayDev;
	BOOL	m_bInputLevel;
	//}}AFX_DATA

public:
	void SaveLauncherSettings();
	virtual void Initialize();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	
	static BOOL IsUnitActive(CWK_Profile&, int);
	static BOOL CanActivateUnit(CWK_Profile&, int);
	static LPCTSTR GetUnitName(int);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnConfirmHardware(int errorCode, DWORD dwFlags, DWORD dwRequestID, const CIPCExtraMemory *pEM);
	void OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData);
	void ControlUnit(BOOL);
	void OnRequestDisconnect();
	void Create();

	// Generated message map functions
	//{{AFX_MSG(CAudioPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeComboAudioCapture();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnReleasedcaptureSliderInputGain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboAudioInputpins();
	afx_msg void OnSelchangeComboAudioCompressor();
	afx_msg void OnSelchangeComboAudioLengthOfSequence();
	afx_msg void OnSelchangeComboAudioRenderer();
	afx_msg void OnSelchangeComboAudioSampFrequency();
	afx_msg void OnBtnTestCompressionMethod();
	afx_msg void OnKillfocusEdtAudioThreshold();
	afx_msg void OnSelchangeComboAudioQuality();
	afx_msg void OnCkAudioUnitActive();
	afx_msg void OnCkAudioInputLevel();
	afx_msg void OnCustomdrawSliderInputGain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCkAudioInputListen();
	afx_msg void OnBtnAudioVolumeCtrl();
	afx_msg void OnCkAudioDetector();
	afx_msg void OnCustomdrawSliderOutputVolume(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderOutputVolume(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
private:

	void RemoveUsedDevices(CComboBox&, LPCTSTR);
	BOOL CheckInputActivations(int bCheckOptions = SVP_CO_CHECK);
	BOOL CheckMediaActivations(int bCheckOptions);

private:
	int m_nPageNo;
	CIPCControlAudioUnit *m_pCIPCcontrolUnit;
	CString m_sRenderer;
	CString m_sCompressor;
	CString m_sCapture;
	int     m_nInputPin;
	int     m_nAudioCapture;
	int     m_nQuality;
	CString m_sSampleFrq;
	UINT    m_nRequestUnitTimer;
	BOOL    m_bOldUnitActive;
	BOOL    m_bEnableDefaultRecDev;
	BOOL    m_bEnableDefaultPlayDev;

	CString m_sKey;
	CString m_sBase;
	CString m_sUnitName;
	bool    m_bIsLocal;
	bool    m_bRecordinLevelInitialized;
	bool    m_bOutputVolumeInitialized;
	bool    m_bUpdateHardwareBranch;
	bool    m_bActivateUnit;
	bool    m_bDeactivateUnit;
	bool    m_bDeleteActivations;
	bool    m_bDeleteInputActivations;
	long    m_lWndProcLevel;
	long    m_lWndProcLevelPeak;

	
	CIPCMediaRecord::MediaRecordFlags m_Flags;
	static LRESULT CALLBACK WndProcLevel(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDOPAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_)
