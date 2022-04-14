#pragma once

#include "DSCamera.h"


// CDsCaptureCamera

class CDsCaptureCamera : public CDsCamera
{
	DECLARE_DYNAMIC(CDsCaptureCamera)

public:
	CDsCaptureCamera(const CString&sName, DWORD dwFlags=0);           // protected constructor used by dynamic creation
	virtual ~CDsCaptureCamera();

	// Implementation
protected:
	HRESULT CameraProperties(VideoProcAmpProperty CameraProperty, bool bSet, bool &bAuto, DWORD &dwValue);
	HRESULT CameraControl(CameraControlProperty CameraProperty, bool bSet, bool &bAuto, DWORD &dwValue);
	HRESULT CameraControlPTZ(UINT nTimerEvent, CameraControlCmd Cmd, DWORD *pdwReturn);
	void	InitParameters();

private:
	//{{AFX_VIRTUAL()
	virtual HRESULT InitFilters();
	virtual void ReleaseFilters();
	virtual HRESULT OnEvent(LONG lCode, LONG lParam1, LONG lParam2);
	virtual CString GetLocation();
	virtual CString GetType();
	virtual BOOL OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData);
	virtual BOOL OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG()
	afx_msg void OnTimer(WPARAM, LPARAM);
	afx_msg void OnSetFilters(WPARAM wParam, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	//{{AFX_DATA()
	IBaseFilter*	m_pCapture;
	int				m_nCapturePin;
	UINT			m_nPTZCmd;
	UINT			m_nPTZSpeed;
	CString			m_sResolutions;
	CString			m_sRotations;
	CString			m_sFramerates;
	CString			m_sCompressionTypes;
	CString			m_sInputs;
	CString			m_sEncoder;
	//}}AFX_DATA
};
