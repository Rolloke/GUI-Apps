#pragma once


#include "DsIpCamera.h"

/////////////////////////////////////////////////////////////////////////////
// CDsIpCameraPanasonic
class CDsIpCameraPanasonic : public CDsIpCamera
{
	DECLARE_DYNAMIC(CDsIpCameraPanasonic)

	CDsIpCameraPanasonic(const CString&sName, DWORD dwFlags=0);           // protected constructor used by dynamic creation
	virtual ~CDsIpCameraPanasonic();

protected:

public:
	//{{AFX_VIRTUAL()
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual HRESULT InitFilters();
	virtual void ReleaseFilters();
	virtual CString GetLocation();
	virtual CString GetType();
	virtual BOOL OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData);
	virtual BOOL OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//{{AFX_DATA()
	//}}AFX_DATA
};


