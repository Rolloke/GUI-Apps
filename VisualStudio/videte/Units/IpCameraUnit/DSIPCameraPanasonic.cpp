// DsIpCameraPanasonic.cpp : implementation file
//

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "DsIpCameraPanasonic.h"


/////////////////////////////////////////////////////////////////////////////
// CDsIpCameraPanasonic
IMPLEMENT_DYNAMIC(CDsIpCameraPanasonic, CDsIpCamera)
/////////////////////////////////////////////////////////////////////////////
CDsIpCameraPanasonic::CDsIpCameraPanasonic(const CString&sName, DWORD dwFlags/*=0*/) :
	CDsIpCamera(sName, dwFlags)
{
	m_sLengthID = "Content-length:";
	m_sStartID  = "\n";
}
/////////////////////////////////////////////////////////////////////////////
CDsIpCameraPanasonic::~CDsIpCameraPanasonic()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsIpCameraPanasonic::InitInstance()
{
	BOOL bReturn = CDsIpCamera::InitInstance();
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
int CDsIpCameraPanasonic::ExitInstance()
{
	return CDsIpCamera::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsIpCameraPanasonic::InitFilters()
{
	HRESULT hr = S_OK;
	switch (m_CompressionType)
	{
		case COMPRESSION_JPEG: break;	// jpg is ok
		default: m_CompressionType = COMPRESSION_JPEG; break;
	}
	if (m_CompressionType == COMPRESSION_JPEG)
	{
		try
		{
			CString sRequest;
			CSize szPict = SizeOfEnum(m_Resolution);
//			int nCompression = 10;
			
			sRequest.Format(_T("cgi-bin/jpeg?framerate=%d&resolution=%d&connect=start&vmdinfo=none&UID=0"),
				m_iFPS, szPict.cx);

			if (m_pStream == NULL)
			{
				m_pStream = GetHTTPConnection()->OpenRequest(CHttpConnection::HTTP_VERB_GET, 
					sRequest,
					NULL, // pstrReferer
					CONTEXTID_STREAM,
					NULL, // ppstrAcceptTypes
					NULL, // pstrVersion
					INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);
			}
			else
			{
				ASSERT(FALSE);
				return TYPE_E_INVALIDSTATE;
			}
			// Make the request
			if (m_pStream->SendRequest())
			{
				TRACE(_T("SENT: %s/%s\n"), m_sURL, sRequest);
				hr = CDsIpCamera::InitFilters();
			}
		}
		catch (CInternetException* pEx)
		{		
			hr = HRESULT_FROM_NT(pEx->m_dwError);
			theApp.ReportError(pEx);
		}
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCameraPanasonic::ReleaseFilters()
{
	if (m_CompressionType == COMPRESSION_JPEG)
	{
		CloseStream();
	}
	// release Filters
	CDsIpCamera::ReleaseFilters();
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraPanasonic::GetLocation()
{
	return CDsIpCamera::GetLocation();
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraPanasonic::GetType()
{
	CString sTxt;
	sTxt.LoadString(IDS_TYPE_PANASONIC);
	return sTxt;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsIpCameraPanasonic::OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData)
{
	if (CDsIpCamera::OnRequestGetValue(pOutput, id, sKey, dwUserData))
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsIpCameraPanasonic::OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (CDsIpCamera::OnRequestSetValue(pOutput, id, sKey, sValue, dwUserData))
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDsIpCameraPanasonic, CDsIpCamera)
	//{{AFX_MSG_MAP(CDsIpCameraPanasonic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDsIpCameraPanasonic message handlers
