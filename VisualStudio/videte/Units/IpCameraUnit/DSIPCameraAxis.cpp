// DsIpCameraAxis.cpp : implementation file
//

#include "stdafx.h"
#include "DsIpCameraAxis.h"

#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"

#include "CIPCOutputIPcamUnit.h"

#include "rtccore.h"

//#include "BDATYPES.H"
//#include "bdamedia.h"
//#include "Bdaiface.h"

//#include "PSIParser\iMpeg2PsiParser.h"

/////////////////////////////////////////////////////////////////////////////
// CDsIpCameraAxis
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDsIpCameraAxis, CDsIpCamera)
/////////////////////////////////////////////////////////////////////////////
CDsIpCameraAxis::CDsIpCameraAxis(const CString&sName, DWORD dwFlags/*=0*/) :
	CDsIpCamera(sName, dwFlags)
{
	m_nCameraNo = -1;
	m_sRotation = _T("0");
	m_sLengthID = "Content-Length:";
	m_sStartID  = "\r\n\r\n";

//	m_pHTTPStreamSource = NULL;
//	m_pMPEG2Demux = NULL;
}
/////////////////////////////////////////////////////////////////////////////
CDsIpCameraAxis::~CDsIpCameraAxis()
{
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDsIpCameraAxis, CDsIpCamera)
	//{{AFX_MSG_MAP(CDsIpCameraAxis)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CDsIpCameraAxis::InitInstance()
{
	BOOL bReturn = CDsIpCamera::InitInstance();

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsIpCameraAxis::InitFilters()
{
	HRESULT hr = S_OK;
	TRACE(_T("InitFilters%d(%d): %s\n"), GetCameraSubID(), m_CompressionType, GetTickCountEx());

	switch (m_CompressionType)
	{
		case COMPRESSION_JPEG: break;	// jpg is ok
#ifdef _DEBUG
//		case COMPRESSION_MPEG4: break;	// mpg4 is ok
#endif
		default: m_CompressionType = COMPRESSION_JPEG; break;
	}
	if (m_Resolution == RESOLUTION_NONE)
	{
		m_Resolution = RESOLUTION_CIF;
	}

	if (m_CompressionType == COMPRESSION_JPEG)
	{
		try
		{
			CString sRequest, sCmd;
			CSize szPict = SizeOfEnum(m_Resolution);
			int nCompression = 10;
			// TODO! RKE: calculate compression value depending on resolution
			//							 2048KBit/s   ,	64Bit/s
			if (IsBetween(m_Compression, COMPRESSION_1, COMPRESSION_26))
			{
				nCompression = 5 + MulDiv(m_Compression, 95, 25);
			}
			if (m_sRotation.IsEmpty())
			{
				m_sRotation = _T("0");
			}

			sRequest.Format(_T("axis-cgi/mjpg/video.cgi?camera=%d&resolution=%dx%d&compression=%d&rotation=%s&des_fps=%d"), // &duration=0&brofframes=0"),
				m_nCameraNo, szPict.cx, szPict.cy, nCompression, m_sRotation, m_iFPS );

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
			theApp.ReportError(pEx);
			hr = HRESULT_FROM_NT(pEx->m_dwError);
		}
	}
	else
	{
//		CStdioFile*pFile = m_pSession->OpenURL(_T("axrtpu://192.168.0.233/mpeg4/media.amp"));

		try 
		{
			hr = CDsIpCamera::InitFilters();
			HRESULT_EXCEPTION(hr);
			BSTR bstr = m_sURL.AllocSysString();
			hr = m_pClient->CreateSession(RTCST_PC_TO_PC, bstr, NULL, 0, &m_pRTCSession);
			SysFreeString(bstr);
			HRESULT_EXCEPTION(hr);
			CString sRequest = _T("mpeg4/media.amp");
			DWORD dwResult;
			HTTPRequest(sRequest, dwResult);
			if (dwResult == HTTP_STATUS_OK)
			{
				hr = S_OK;
			}
		}
		catch (ErrorStruct *ps)
		{
			ReleaseFilters();
			ReportError(ps, FALSE);
		}
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCameraAxis::ReleaseFilters()
{
	CloseStream();
//	RemoveAndReleaseFilter(m_pMPEG2Demux);
//	RemoveAndReleaseFilter(m_pHTTPStreamSource);
	CDsIpCamera::ReleaseFilters();
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCameraAxis::SetCameraNo(int nNo)
{
	if (m_nCameraNo != nNo)
	{
		SetDirty(TRUE);
	}
	m_nCameraNo = nNo;
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraAxis::GetLocation()
{
	CString sTxt;
	if (m_nCameraNo != -1)
	{
		sTxt.Format(_T(":C=%d"), m_nCameraNo);
	}
	return m_sURL + sTxt;
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraAxis::GetType()
{
	CString sTxt;
	sTxt.LoadString(IDS_TYPE_AXIS);
	return sTxt;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CDsIpCameraAxis::GetVideoStatus()
{
	CString sAnswer, sRequest;
	DWORD dwResult = CAMS_UNKNOWN;
	sRequest.Format(_T("axis-cgi/view/videostatus.cgi?status=%d"), m_nCameraNo);
	sAnswer = HTTPRequest(sRequest, dwResult);
	if (dwResult == HTTP_STATUS_OK)
	{
		sRequest.Format(_T("Video %d = video"), m_nCameraNo);
		if (sAnswer.Find(sRequest) != -1)
		{
			return CAMS_VIDEO_OK;
		}
		else
		{
			return CAMS_NO_VIDEO;
		}
	}
	else
	{
		return dwResult;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraAxis::GetPropertyString(LPCTSTR sProperty)
{
	if (m_sCapabilities.IsEmpty())
	{
		CString sRequest = _T("axis-cgi/view/param.cgi?action=list");
		DWORD dwResult;
		m_sCapabilities = HTTPRequest(sRequest, dwResult);
		if (dwResult != HTTP_STATUS_OK)
		{
			WK_TRACE(_T("%s\n"), m_sCapabilities);
		}
	}
	int nFind = m_sCapabilities.Find(sProperty);
	if (nFind != -1)
	{
		nFind += ((int)_tcslen(sProperty)+1);
		int nEnd = m_sCapabilities.Find(_T("\n"), nFind);
		return m_sCapabilities.Mid(nFind, nEnd-nFind);
	}
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
int	CDsIpCameraAxis::GetPropertyInt(LPCTSTR sProperty)
{
	CString s = GetPropertyString(sProperty);
	if (!s.IsEmpty())
	{
		return _ttoi(s);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraAxis::GetName()
{
	return GetPropertyString(_T("ProdShortName")) + _T(" ") + GetPropertyString(_T("SerialNumber"));
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraAxis::GetRotations()
{
	return GetPropertyString(_T("Image.Rotation"));
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraAxis::GetResolutions()
{
	return GetPropertyString(_T("Image.Resolution"));
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCameraAxis::GetCompressionTypes()
{
	return GetPropertyString(_T("Image.Format"));
}
/////////////////////////////////////////////////////////////////////////////
CString	CDsIpCameraAxis::GetNoOfInputs()
{
	return GetPropertyString(_T("Input.NbrOfInputs"));
}
/////////////////////////////////////////////////////////////////////////////
CString	CDsIpCameraAxis::GetNoOfOutputs()
{
	return GetPropertyString(_T("ImageSource.NbrOfSources"));
}
/////////////////////////////////////////////////////////////////////////////
CString	CDsIpCameraAxis::GetNoOfRelais()
{
	return GetPropertyString(_T("Output.NbrOfOutputs"));
}
/////////////////////////////////////////////////////////////////////////////
CString	CDsIpCameraAxis::GetNoOfCOMPorts()
{
	return GetPropertyString(_T("Serial.NbrOfPorts"));
}
/////////////////////////////////////////////////////////////////////////////
CString	CDsIpCameraAxis::GetMD()
{
	return GetPropertyString(_T("Motion.Motion"));
}
/////////////////////////////////////////////////////////////////////////////
CString	CDsIpCameraAxis::GetPTZ()
{
	return GetPropertyString(_T("PTZ.PTZ"));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsIpCameraAxis::OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData)
{
	if (CDsIpCamera::OnRequestGetValue(pOutput, id, sKey, dwUserData))
	{
		return TRUE;
	}

	CString sValue;
	if (sKey == CSD_RESOLUTIONS)
	{
		sValue = GetResolutions();
	}
	else if (sKey == CSD_ROTATIONS)
	{
		sValue = GetRotations();
	}
	else if (sKey == CSD_FRAMERATES)
	{
//		sValue = GetResolutions();
	}
	else if (sKey == CSD_OUTPUTS)
	{
		sValue = GetNoOfOutputs();
	}
	else if (sKey == CSD_RELAIS)
	{
		sValue = GetNoOfRelais();
	}
	else if (sKey == CSD_INPUTS)
	{
		sValue = GetNoOfInputs();
	}
	else if (sKey == CSD_COMPRESSIONS)
	{
		sValue = GetCompressionTypes();
	}
	else if (sKey == CSD_NAME)
	{
		sValue = GetName();
	}
	if (sValue.IsEmpty())
	{
		return FALSE;
	}
	else
	{
		pOutput->DoConfirmGetValue(id, sKey, sValue, dwUserData);
		return TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsIpCameraAxis::OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (sKey == CSD_MD)
	{
		CString sAnswer, sRequest;
		CStringArray ar;
		DWORD dwResult;
		sRequest = _T("axis-cgi/operator/param.cgi?action=list&group=Motion");
		sAnswer = HTTPRequest(sRequest, dwResult);
		if (dwResult == HTTP_STATUS_OK)
		{
			int iStart = 0, nFind;
			sRequest.Format(_T("ImageSource=%d"), m_nCameraNo-1);
			for (;;)
			{
				nFind = sAnswer.Find(sRequest, iStart);
				if (nFind == -1) break;
				ar.Add(sAnswer.Mid(nFind - 3, 2));
				iStart = nFind + sRequest.GetLength();
			}
		}
		// Motion detection parameters
		// 
//		_T("axis-cgi/operator/param.cgi?action=add&group=Motion&template=motion")
	}
	if (CDsIpCamera::OnRequestSetValue(pOutput, id, sKey, sValue, dwUserData))
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CDsIpCameraAxis message handlers
/////////////////////////////////////////////////////////////////////////////

/*
	HRESULT hr;
	IBaseFilter *pFilter = NULL;
	IBaseFilter *pBaseDemux = NULL;
	IPin *pPinOut = NULL;
	IEnumFilters *pEnum = NULL;
	ULONG cFetched;

	IFileSourceFilter *pFSF = NULL;
	IPersistStream *pPS = NULL;
	IMoniker*pUrlMon = NULL;
	IStream*pStream = NULL;

	int nCompression = 25;
	SIZE szPict = SizeOfEnum(m_Resolution);
	if (szPict.cx == 0 && szPict.cy == 0)
	{
		szPict = SizeOfEnum(RESOLUTION_CIF);
	}

	CString sRequest;
	if (m_CompressionType == COMPRESSION_JPEG)
	{
		sRequest.Format(_T("axis-cgi/mjpg/video.cgi?camera=%d&resolution=%dx%d&compression=%d&fps=%d"),
			m_nCameraNo, szPict.cx, szPict.cy, nCompression, m_iFPS);
	}

	try
	{
		LoadGraph(_T("camMJPG.GRF"));

		if (m_pGraph)
		{
			hr = m_pGraph->EnumFilters(&pEnum);
			if (SUCCEEDED(hr))
			{
				while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
				{
					FILTER_INFO FilterInfo;
					hr = pFilter->QueryFilterInfo(&FilterInfo);
					if (SUCCEEDED(hr))
					{
						CString s(FilterInfo.achName);
						if (s == _T("Dump"))
						{
							m_pDump = pFilter;
							pFilter = NULL;
						}
						else if (s.Find(_T("HTTP")) != -1)
						{
							m_pHTTPStreamSource = pFilter;
							pFilter = NULL;
						}
					}
					RELEASE_OBJECT(pFilter);
					RELEASE_OBJECT(FilterInfo.pGraph);
				}
				RELEASE_OBJECT(pEnum);
			}
		}
		if (m_pHTTPStreamSource && m_pDump)
		{
			InitDumpFilterReceiveFnc();

			hr = ConnectFilters(m_pGraph, m_pHTTPStreamSource, 0, m_pDump, 0);
			HRESULT_EXCEPTION(hr);
		}
	}
	catch (ErrorStruct *ps)
	{
		ReportError(ps);
	}
	RELEASE_OBJECT(pStream);
	RELEASE_OBJECT(pUrlMon);
	RELEASE_OBJECT(pPS);
	RELEASE_OBJECT(pFSF);
	RELEASE_OBJECT(pFilter);
	RELEASE_OBJECT(pPinOut);
	RELEASE_OBJECT(pEnum);
*/
/*
IUnknown
IPersistStream
IPersist
IBaseFilter
IMediaFilter
IFileSourceFilter
IAMOpenProgress
IAMovieSetup
ISpecifyPropertyPages
*/
/*
	IFileSourceFilter *pFSF = NULL;
	IPersistStream *pPS = NULL;
		CString sName = _T("Axis MPEG HTTP Stream Source");
		hr = AddFilterToGraph(m_pGraph, &m_pHTTPStreamSource, CLSID_CQzFilterClassManager, sName, AFTG_MATCH_EXACT, L"StreamSource");
		HRESULT_EXCEPTION(hr);

		hr = m_pHTTPStreamSource->QueryInterface(IID_IFileSourceFilter, (void**)&pFSF);
		HRESULT_EXCEPTION(hr);
		AM_MEDIA_TYPE mt;
		LPOLESTR pszFile = NULL;
		hr = pFSF->GetCurFile(&pszFile, &mt);
		HRESULT_EXCEPTION(hr);
		CString sFile(pszFile);
		SAFE_COTASKMEMFREE(pszFile);

			hr = m_pHTTPStreamSource->QueryInterface(IID_IPersistStream, (void**)&pPS);
			HRESULT_EXCEPTION(hr);
			CLSID clsid;
			hr = pPS->GetClassID(&clsid);
			ULARGE_INTEGER uliSize;
			hr = pPS->GetSizeMax(&uliSize);

			CString sUrl = _T("http://192.168.0.233/axis-cgi/mjpg/video.cgi");
			hr = CreateURLMoniker(NULL, sUrl, &pUrlMon);
			HRESULT_EXCEPTION(hr);
			CBSCallbackImpl bsc(m_hThread);
			IBindStatusCallback *pbsc = NULL;
			bsc.QueryInterface(IID_IBindStatusCallback, (void**)&pbsc);
			IEnumFORMATETC *penumfmtetc = NULL;
			IBindCtx *pBctx = NULL;
			hr = CreateAsyncBindCtx(0, pbsc, penumfmtetc, &pBctx);

			hr = pUrlMon->BindToObject(pBctx, NULL, IID_IStream, (void**)&pStream);
//			hr = pUrlMon->BindToStorage(pBctx, NULL, IID_IStream, (void**)&pStream);
			HRESULT_EXCEPTION(hr);
			hr = pPS->Load(pStream);
			HRESULT_EXCEPTION(hr);



	determine content of stream
			CString sName = _T("MPEG-2 Demultiplexer");
			hr = AddFilterToGraph(m_pGraph, &m_pMPEG2Demux, CLSID_CQzFilterClassManager, sName, AFTG_MATCH_EXACT, L"StreamSource");
			HRESULT_EXCEPTION(hr);
			hr = m_pMPEG2Demux->QueryInterface(IID_IMpeg2Demultiplexer, (void**)&pIDeMux);
			HRESULT_EXCEPTION(hr);

			hr = ConnectFilters(m_pGraph, m_pHTTPStreamSource, 0, m_pMPEG2Demux, 0);
			HRESULT_EXCEPTION(hr);

			AM_MEDIA_TYPE mt;
			ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
			mt.majortype = KSDATAFORMAT_TYPE_MPEG2_SECTIONS;
			hr = pIDeMux->CreateOutputPin(&mt, L"PSI", &pPinOut);
			HRESULT_EXCEPTION(hr);

			hr = pPinOut->QueryInterface(IID_IMPEG2StreamIdMap, (void**)&pIStreamIdMapper);
			HRESULT_EXCEPTION(hr);
			IEnumStreamIdMap *pEnum;
			hr = pIStreamIdMapper->EnumStreamIdMap(&pEnum);
			while (SUCCEEDED(hr))
			{
				STREAM_ID_MAP sidm;
				ULONG uR;
				pEnum->Next(1, &sidm, &uR);
				if (uR == 0)
				{
					break;
				}
			}


			// Map to PID 0.
//			ULONG Pid = 0x00;
//			hr = pIStreamIdMapper->MapStreamId(1, &Pid, MEDIA_MPEG2_PSI);
//			HRESULT_EXCEPTION(hr);

			sName = _T("PSI Parser");
			hr = AddFilterToGraph(m_pGraph, &pFilter, CLSID_CQzFilterClassManager, sName, AFTG_MATCH_EXACT, L"StreamSource");
			HRESULT_EXCEPTION(hr);

			hr = ConnectFilters(m_pGraph, m_pMPEG2Demux, 0, pFilter, 0);
			HRESULT_EXCEPTION(hr);
/*
			AM_MEDIA_TYPE mt;
			mt.majortype = 
			pIDeMux->CreateOutputPin(&mt, L("JpegVideo"), &pPinOut);

			hr = m_pMPEG2Demux->QueryInterface(IID_IMPEG2StreamIdMap, (void**)&pIStreamIdMapper);
			HRESULT_EXCEPTION(hr);
*/

/*
HRESULT CDsIpCameraAxis::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo)
{
	const char sContentLength[] = "Content-Length: ";
	char*pBuffer;
	int nSampleLength = pSample->GetActualDataLength();
	int nLen;
	pSample->GetPointer((BYTE**)&pBuffer);

	if (m_nCurPos < m_nBufferLen)
	{
		nLen = m_nBufferLen - m_nCurPos;
		if (nLen < nSampleLength)
		{
		}
		nLen = min(nSampleLength, nLen);
		memcpy(&m_pBuffer[m_nCurPos], pBuffer, nLen);
	}
	char *ptr = strstr(pBuffer, sContentLength);
	if (ptr)
	{
		ptr = &ptr[sizeof(sContentLength)-1];
		int nLen = atoi(ptr);
		if (nLen > m_nMaxBufferLen)
		{
			m_nMaxBufferLen = nLen;
			m_pBuffer = (BYTE*)realloc(m_pBuffer, m_nMaxBufferLen);
		}
		m_nBufferLen = nLen;
		m_nCurPos = 0;
		ptr = strstr(ptr, "\r\n\r\n");
		if (ptr)
		{
			ptr = &ptr[4];
			int nHead = ptr - pBuffer;
			nSampleLength -= nHead;
			nLen = min(nSampleLength, nLen);
			memcpy(m_pBuffer, ptr,  nLen);
			m_nCurPos = nLen;
		}
	}
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
*/
