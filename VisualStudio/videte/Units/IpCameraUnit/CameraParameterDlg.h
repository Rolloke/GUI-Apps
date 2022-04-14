#pragma once
#include "afxwin.h"


// CCameraParameterDlg dialog

#ifdef _TEST_SV_DLG
class CIPCOutputIPcamUnit;
class CCameraParameterDlg : public CDialog
{
	DECLARE_DYNAMIC(CCameraParameterDlg)

public:
	CCameraParameterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCameraParameterDlg();


// Implementierung
public:
	void OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	void OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
protected:
	void InitCaptureDeviceList();
	void InitStrings(const CString&saCsv, CComboBox&combo);
	void RequestParameterSet(CSecID id);

// Dialog Data
	//{{AFX_DATA(CCameraParameterDlg)
	enum { IDD = IDD_CAMERA_PARAMETER };
	CComboBox	m_cTypes;
	CStringArray m_saTypes;
	CStringArray m_saUrls;
	CComboBox	m_cCaptureDevices;
	CComboBox	m_cResolutions;
	CComboBox	m_cRotations;
	CComboBox	m_cCompression;
	CComboBox	m_cURL;
public:
	CString		m_sUrl;
	UINT		m_uPort;
	CString		m_sDeviceType;
	CString		m_sName;
	CString		m_sCameras;
	CString		m_sInputs;
	CString		m_sRelais;
	CString		m_sResolution;
	CString		m_sCompresion;
	CString		m_sRotation;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(CCameraParameterDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCameraParameterDlg)
	afx_msg void OnCbnSelchangeComboCaptureDevices();
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnSetName();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	afx_msg void OnCbnEditchangeComboCaptureDevices();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdtPortOrUrl();
	afx_msg void OnCbnSelchangeComboUrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CIPCOutputIPcamUnit*m_pOutputControl;
	int m_nCurrentCaptureDevice;
public:
};
#endif
