// DSCamera.cpp : implementation file
//

#include "stdafx.h"
#include "DSCamera.h"

#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"
#include "CIPCOutputIPcamUnit.h"


#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include "Dump\idump.h"

/////////////////////////////////////////////////////////////////////////////
// CDsCamera

IMPLEMENT_DYNAMIC(CDsCamera, CDirectShowThread)
/////////////////////////////////////////////////////////////////////////////
CDsCamera::CDsCamera(const CString&sName, DWORD dwFlags) : 
	CDirectShowThread(sName, dwFlags)
{
	m_pDump = NULL;
	m_pDecoder = NULL;
	m_wCamSubID = 0;
	m_wPackageNr = 0;
	m_Resolution = RESOLUTION_NONE; 
	m_Compression = COMPRESSION_NONE;
	m_CompressionType = COMPRESSION_UNKNOWN;
	m_iFPS = 0;
	m_iIFrameInterval = 0;
	m_bAutoDelete = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDsCamera::~CDsCamera()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDsCamera, CDirectShowThread)
	//{{AFX_MSG_MAP(CDsCamera)
	ON_THREAD_MESSAGE(WM_REQUEST_GET_VALUE, OnRequestGetValue)
	ON_THREAD_MESSAGE(WM_REQUEST_SET_VALUE, OnRequestSetValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::ReleaseFilters()
{
	RemoveAndReleaseFilter(m_pDump);
	RemoveAndReleaseFilter(m_pDecoder);
	CDirectShowThread::ReleaseFilters();
}
/////////////////////////////////////////////////////////////////////////////
int CDsCamera::Run()
{
	// notify next thread can be started
	theApp.GetDlg()->PostMessage(WM_THREAD_STARTED);
	return CDirectShowThread::Run();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsCamera::IsThreadRunning() const
{
	if (m_hThread)
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CString CDsCamera::GetLocation()
{
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
CString CDsCamera::GetType()
{
	return _T("DircetShow Camera");
}
/////////////////////////////////////////////////////////////////////////////
DWORD CDsCamera::GetVideoStatus()
{
	return CAMS_UNKNOWN;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::DrawPicture(const BYTE*pData, DWORD dwLenght, CompressionType ct)
{
	if (m_dwFlags & DSTF_PREVIEW)
	{
		CRect rc;
		CWnd *pWnd = theApp.GetDlg()->GetDlgItem(IDC_STAT_PICTURE);
		if (pWnd)
		{
			CClientDC dc(pWnd);
			pWnd->GetClientRect(&rc);
			if (ct == COMPRESSION_JPEG)
			{
				m_JpgDecoder.DecodeJpegFromMemory((BYTE*)pData, dwLenght);
				m_JpgDecoder.OnDraw(&dc, rc);
				if (theApp.GetDlg()->m_bUpdateImageSize)
				{
					theApp.GetDlg()->UpdateImageSize(m_JpgDecoder.GetImageDims());
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::DrawPicture(CIPCPictureRecord*pPict)
{
	if (m_dwFlags & DSTF_PREVIEW)
	{
		CRect rc;
		CWnd *pWnd = theApp.GetDlg()->GetDlgItem(IDC_STAT_PICTURE);
		if (pWnd)
		{
			CClientDC dc(pWnd);
			pWnd->GetClientRect(&rc);
			m_JpgDecoder.SetPictureRecord(pPict, TRUE);
			dc.m_bPrinting = FALSE;
			m_JpgDecoder.OnDraw(&dc, rc);
			if (theApp.GetDlg()->m_bUpdateImageSize)
			{
				theApp.GetDlg()->UpdateImageSize(m_JpgDecoder.GetImageDims());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::SetResolution(Resolution res)
{
	if (m_Resolution != res) SetDirty(TRUE);
	m_Resolution = res;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::SetCompression(Compression comp)
{
	if (m_Compression != comp) SetDirty(TRUE);
	m_Compression = comp;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::SetCompressionType(CompressionType ct)
{
	if (m_CompressionType != ct) SetDirty(TRUE);
	m_CompressionType = ct;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::SetFPS(int iFPS)
{
	if (m_iFPS != iFPS) SetDirty(TRUE);
	m_iFPS = iFPS;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::SetIFrameInterval(int iIFrameInterval)
{
	if (m_iIFrameInterval != iIFrameInterval) SetDirty(TRUE);
	m_iIFrameInterval = iIFrameInterval;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::SetRotation(const CString&sRot)
{
	if (m_sRotation != sRot)  SetDirty(TRUE);
	m_sRotation = sRot;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsCamera::InitDumpFilterReceiveFnc(AM_MEDIA_TYPE*pmt, int nCount, BOOL bEnum)
{
	HRESULT hr = E_POINTER;
	if (m_pDump == NULL)
	{
		hr = AddFilterToGraph(m_pGraph, &m_pDump, CLSID_Dump, L"Sample Dump");
		if (FAILED(hr))
		{
			return hr;
		}
	}
	if (m_pDump)
	{
		IPin* pPinIn = NULL;
		IDump *pIDump = NULL;
		hr = GetPin(m_pDump  , PINDIR_INPUT , 0, &pPinIn);
		if (SUCCEEDED(hr))
		{
			hr = pPinIn->QueryInterface(IID_IDump, (void **)&pIDump);
			if (SUCCEEDED(hr))
			{
				hr = pIDump->SetReceiveFunction(ReceiveMediaSample, (long)(LONG_PTR)this);
				if (pmt)
				{
					pIDump->SetAcceptedMediaTypes(pmt, nCount, bEnum);
				}
			}
		}
		RELEASE_OBJECT(pPinIn);
		RELEASE_OBJECT(pIDump);
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CDsCamera::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo, long lParam)
{
	CDsCamera *pThis = (CDsCamera*)(LONG_PTR)lParam;
	return pThis->ReceiveMediaSample(pSample, nPackageNo);
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsCamera::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo)
{
	CIPCOutputIPcamUnit*pOutput = theApp.GetDlg()->GetOutput(m_wCamSubID);
	if (pOutput && pOutput->IsConnected())
	{
		ASSERT(m_pDump != NULL);
		AM_MEDIA_TYPE*pMT;
		BYTE*		pImageData;
		int			nSampleLength = 0;
		DWORD		dwMDx = 0,
					dwMDy = 0;

		CSecID camID(pOutput->GetGroupID(), m_wCamSubID);
		CSecID idArchive; // TODO! RKE: Archive ID ermitteln

		nSampleLength = pSample->GetActualDataLength();
		pSample->GetPointer(&pImageData);
		pSample->GetMediaType(&pMT);
		if (   pImageData == NULL 
			|| nSampleLength == 0
			|| pMT == NULL)
		{
			return S_FALSE;
		}
		VIDEOINFOHEADER *pVIH  = (VIDEOINFOHEADER*)pMT->pbFormat;

		CSystemTime	TimeStamp;
		// TODO! RKE: retrieve mediatime
		TimeStamp.GetLocalTime();

		if (  (MEDIASUBTYPE_YVYU   == pMT->subtype) 
			 ||(MEDIASUBTYPE_YUY2   == pMT->subtype))
		{
			pVIH->bmiHeader.biCompression = mmioFOURCC('Y', '4', '2', '2');
			CIPCPictureRecord PictRec(pOutput,
									(const unsigned char*)pImageData,
									pVIH->bmiHeader.biSizeImage,
									camID,
									m_Resolution,
									COMPRESSION_NONE,
									COMPRESSION_YUV_422,
									TimeStamp,
									TRUE,
									NULL,
									0,
									SPT_FULL_PICTURE,
									0,
									&pVIH->bmiHeader, pVIH->bmiHeader.biSize);
			pOutput->DoIndicateVideo(PictRec, dwMDx, dwMDy, m_dwUserData, idArchive);
		}
		else if (pMT->subtype == MEDIASUBTYPE_RGB32)
		{
			CIPCPictureRecord PictRec(pOutput,
									(const unsigned char*)pImageData,
									pVIH->bmiHeader.biSizeImage,
									camID,
									m_Resolution,
									COMPRESSION_NONE,
									COMPRESSION_RGB_24,
									TimeStamp,
									TRUE,
									NULL,
									0,
									SPT_FULL_PICTURE,
									(WORD)nPackageNo,
									&pVIH->bmiHeader, pVIH->bmiHeader.biSize);
			pOutput->DoIndicateVideo(PictRec, dwMDx, dwMDy, m_dwUserData, idArchive);
		}
		else
		{
			CString str;
			GetGUIDString(str, &pMT->subtype, TRUE);
			str.MakeUpper();
			if (   str.Find(_T("JPG")) != -1
				|| str.Find(_T("MJPG")) != -1
				|| str.Find(_T("JPEG")) != -1)
			{
				CIPCPictureRecord PictRec(pOutput,
										(const unsigned char*)pImageData,
										pVIH->bmiHeader.biSizeImage,
										camID,
										m_Resolution,
										m_Compression,
										COMPRESSION_JPEG,
										TimeStamp,
										TRUE,
										NULL,
										0,
										SPT_FULL_PICTURE,
										(WORD)nPackageNo);
		   
				pOutput->DoIndicateVideo(PictRec, dwMDx, dwMDy, m_dwUserData, idArchive);
			}
			else if (   str.Find(_T("MPG4")) != -1
				     || str.Find(_T("XVID")) != -1
					 || str.Find(_T("DIVX")) != -1)
			{
				// TODO! RKE: determine full picture or GOP
				CIPCPictureRecord PictRec(pOutput,
										(const unsigned char*)pImageData,
										pVIH->bmiHeader.biSizeImage,
										camID,
										m_Resolution,
										m_Compression,
										COMPRESSION_MPEG4,
										TimeStamp,
										TRUE,
										NULL,
										0,
										SPT_FULL_PICTURE,
										(WORD)nPackageNo);
		   
				pOutput->DoIndicateVideo(PictRec, dwMDx, dwMDy, m_dwUserData, idArchive);
			}
		}
		

		
// TODO! RKE: implement MD
//		CPoint ptMD(0,0);
//		m_csMDpoints.Lock();
//		if (m_mdPoints[wSource].GetCount())
//		{
//			CMDPoints*pPoints = (CMDPoints*)m_mdPoints[wSource].RemoveHead();
//			ptMD = pPoints->GetPointAt(0);
//			WK_DELETE(pPoints);
//		}
//		m_csMDpoints.Unlock();
//		m_pOutput->IndicationDataReceived(&Image, TRUE, ptMD.x, ptMD.y);


	}
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::OnRequestGetValue(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR pszKey = (LPCTSTR)wParam;
	CIPCOutputIPcamUnit*pOutput = theApp.GetDlg()->GetOutput(m_wCamSubID);
	if (pOutput && pOutput->IsConnected())
	{
		CSecID id(pOutput->GetGroupID(), m_wCamSubID);
		if (!OnRequestGetValue(pOutput, id, pszKey, (DWORD)lParam))
		{
			pOutput->DoIndicateError(0, id, CIPC_ERROR_INVALID_VALUE, 0);	
		}
	}
	free((void*)pszKey);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsCamera::OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData)
{
	if (sKey == CSD_SCAN_FOR_CAMERAS)
	{
		// TODO! RKE: get cameras with video signal
		DWORD dwCameras = theApp.GetDlg()->GetActiveCameraMask(FALSE);
		dwCameras  &= dwUserData;
		CString sValue;
		sValue.Format(_T("%lu"), dwCameras);
		pOutput->DoConfirmGetValue(id, sKey, sValue, dwUserData);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCamera::OnRequestSetValue(WPARAM wParam, LPARAM lParam)
{
	CStringArray*psa = (CStringArray*)wParam;
	CIPCOutputIPcamUnit*pOutput = theApp.GetDlg()->GetOutput(m_wCamSubID);
	if (psa && pOutput && pOutput->IsConnected())
	{
		CSecID id(pOutput->GetGroupID(), m_wCamSubID);
		if (!OnRequestSetValue(pOutput, id, psa->GetAt(0), psa->GetAt(1), (DWORD)lParam))
		{
			pOutput->DoIndicateError(0, id, CIPC_ERROR_INVALID_VALUE, 0);	
		}
	}
	WK_DELETE(psa);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsCamera::OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (sKey == CSD_ACTIVE_CAMERA_MASK)
	{
		DWORD i, dwMask, dwValue = _ttoi(sValue);
		ASSERT(FALSE);
		for (i=0, dwMask=1; i<MAX_CAMERAS; i++, dwMask<<=1)
		{
//			theApp.GetDlg()->SetCameraActive(pOutput, (WORD)i, dwValue&dwMask ? true : false, dwUserData);
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CDsCamera message handlers
