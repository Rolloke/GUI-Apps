#pragma once

#include "DsIpCamera.h"

/////////////////////////////////////////////////////////////////////////////
// CDsIpCameraAxis
class CDsIpCameraAxis : public CDsIpCamera
{
	DECLARE_DYNAMIC(CDsIpCameraAxis)

public:
	CDsIpCameraAxis(const CString&sName, DWORD dwFlags=0);           // protected constructor used by dynamic creation
	virtual ~CDsIpCameraAxis();

	// Attribute
protected:
	CString GetName();
	CString GetRotations();
	CString GetResolutions();
	CString GetCompressionTypes();
	CString	GetNoOfInputs();
	CString	GetNoOfOutputs();
	CString	GetNoOfRelais();
	CString	GetNoOfCOMPorts();
	CString	GetMD();
	CString	GetPTZ();

	// Implementation
public:
	void SetCameraNo(int nNo);

protected:
	int		GetPropertyInt(LPCTSTR sProperty);
	CString GetPropertyString(LPCTSTR sProperty);

protected:
	//{{AFX_VIRTUAL()
	virtual BOOL InitInstance();
	virtual HRESULT InitFilters();
	virtual void ReleaseFilters();
	virtual CString GetLocation();
	virtual CString GetType();
	virtual BOOL OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData);
	virtual BOOL OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	virtual DWORD GetVideoStatus();

	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA()
	CString m_sCapabilities;
	int		m_nCameraNo;
	//}}AFX_DATA
//	IBaseFilter *m_pHTTPStreamSource;
//	IBaseFilter *m_pMPEG2Demux;
};



