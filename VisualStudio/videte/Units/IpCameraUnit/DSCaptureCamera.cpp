// DSCaptureCamera.cpp : implementation file
//

#include "stdafx.h"
#include "DSCaptureCamera.h"

#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"

#include "CIPCOutputIPcamUnit.h"

#define PTZ_TIMER 1

#define PTZ_FUNCTIONS		1
#define PTZ_FUNCTIONS_EX	2
#define PTZ_TYPE			3

#define FLIP_VERTICAL	_T("FlipVertical")
#define FLIP_HORIZONTAL	_T("FlipHorizontal")

/////////////////////////////////////////////////////////////////////////////
// CDsCaptureCamera
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDsCaptureCamera, CDsCamera)
/////////////////////////////////////////////////////////////////////////////
CDsCaptureCamera::CDsCaptureCamera(const CString&sName, DWORD dwFlags/*=0*/) :
	CDsCamera(sName, dwFlags)
{
	m_pCapture = NULL;
	m_nPTZCmd  = 0;
	m_nCapturePin = 0;
	m_sEncoder = _T("MJPEG Compressor");
}
/////////////////////////////////////////////////////////////////////////////
CDsCaptureCamera::~CDsCaptureCamera()
{
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDsCaptureCamera, CDsCamera)
	//{{AFX_MSG_MAP(CDsCaptureCamera)
	ON_THREAD_MESSAGE(WM_TIMER, OnTimer)
	ON_THREAD_MESSAGE(WM_SET_FILTERS, OnSetFilters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
HRESULT CDsCaptureCamera::InitFilters()
{
	ASSERT(m_pCapture == NULL);

	const int nTypes = 2;
	AM_MEDIA_TYPE mt[nTypes];
	ZERO_INIT(mt);
	VIDEOINFOHEADER vih;
	ZERO_INIT(vih);
	int i, nMediatypes = 1;
	BOOL bInitEncoder = FALSE;
	BOOL bInitMotionDetection = FALSE;

	for (i=0; i<nTypes; i++)
	{
		mt[i].majortype = MEDIATYPE_Video;
	}

	if (m_CompressionType == COMPRESSION_YUV_422 || bInitMotionDetection)
	{
		mt[0].subtype = MEDIASUBTYPE_YUY2;
		mt[1].subtype = MEDIASUBTYPE_YUYV;
		nMediatypes = 2;
	}
	else if (m_CompressionType == COMPRESSION_RGB_24)
	{
		mt[0].subtype = MEDIASUBTYPE_RGB24;
	}
	else if (   m_CompressionType == COMPRESSION_JPEG 
		     && !m_sEncoder.IsEmpty())
	{
		mt[0].subtype = MEDIASUBTYPE_YUY2;
		mt[1].subtype = MEDIASUBTYPE_YUYV;
		nMediatypes = 2;
//		bInitEncoder = TRUE;
	}
	else if (   m_CompressionType == COMPRESSION_MPEG4
		     && !m_sEncoder.IsEmpty())
	{
		mt[0].subtype = MEDIASUBTYPE_YUY2;
		mt[1].subtype = MEDIASUBTYPE_YUYV;
		nMediatypes = 2;
//		bInitEncoder = TRUE;
	}

	HRESULT hr;
	hr = CDsCamera::InitFilters();

	SIZE szPicture = SizeOfEnum(m_Resolution);
	if (szPicture.cx != 0 && szPicture.cy)
	{
		vih.bmiHeader.biWidth  = szPicture.cx;
		vih.bmiHeader.biHeight = szPicture.cy;
		for (i=0; i<nTypes; i++)
		{
			mt[i].formattype = FORMAT_VideoInfo;
			mt[i].pbFormat = (BYTE*)&vih;
		}
	}

	IBaseFilter *pFilter = NULL;
	IPin *pPinOut = NULL;
	try
	{
		CString sName = GetName();
		hr = AddFilterToGraph(m_pGraph, &m_pCapture, CLSID_CVidCapClassManager, sName, AFTG_MATCH_EXACT, L"VideoCapture");
		HRESULT_EXCEPTION(hr);

		if (bInitEncoder)
		{
			hr = AddFilterToGraph(m_pGraph, &pFilter, CLSID_CIcmCoClassManager, m_sEncoder, AFTG_MATCH_EXACT, L"VideoEncoder");
			HRESULT_EXCEPTION(hr);
		}

		hr = InitDumpFilterReceiveFnc(mt, nMediatypes, TRUE);
		HRESULT_EXCEPTION(hr);
		hr = m_pGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pCapture, pFilter, m_pDump);
		HRESULT_EXCEPTION(hr);
		InitParameters();

		if (m_dwFlags & DSTF_PREVIEW)
		{
			hr = m_pGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pCapture, NULL, NULL);
			HRESULT_EXCEPTION(hr);
		}
	}
	catch (ErrorStruct *ps)
	{
		ReleaseFilters();
		ReportError(ps, FALSE);
//		PostToMainWnd(WM_GRAPHNOTIFY, MAKELONG(DSTF_NOTIFICATION, EC_DEVICE_LOST), (LPARAM)this);
	}
	RELEASE_OBJECT(pFilter);
	RELEASE_OBJECT(pPinOut);
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCaptureCamera::ReleaseFilters()
{
	RemoveAndReleaseFilter(m_pCapture);
	CDsCamera::ReleaseFilters();
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsCaptureCamera::OnEvent(LONG lCode, LONG lParam1, LONG lParam2)
{
	if (lCode == EC_DEVICE_LOST)
	{
		TRACE(_T("DEVICE_LOST, stopping\n"));
		OnSetMediaState(State_Stopped, 0);
		PostToMainWnd(WM_GRAPHNOTIFY, MAKELONG(DST_NOTIFICATION, lCode), (LPARAM)this);
	}
	else
	{
		return CDsCamera::OnEvent(lCode, lParam1, lParam2);
	}
	// Free event parameters to prevent memory leaks
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
CString CDsCaptureCamera::GetLocation()
{
	CString sName = GetName();
	int nFind = sName.Find(_T("@"));
	if (nFind != -1)
	{
		return sName.Mid(nFind + 1);
	}
	return _T("USB");
}
/////////////////////////////////////////////////////////////////////////////
CString CDsCaptureCamera::GetType()
{
	CString sTxt;
	sTxt.LoadString(IDS_TYPE_CAPTURE);
	return sTxt;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsCaptureCamera::OnRequestGetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, DWORD dwUserData)
{
	BOOL  bFound = CDsCamera::OnRequestGetValue(pOutput, id, sKey, dwUserData);
	if (bFound)
	{
		return bFound;
	}
	bFound = TRUE;
	VideoProcAmpProperty CamProperty = (VideoProcAmpProperty)0;
	CameraControlProperty CamCtrlProp = (CameraControlProperty)0;
	DWORD dwValue = 0;
	CString sValue;
	HRESULT  hr = S_OK;

	if (0==sKey.Find(CSD_CAM_CONTROL_FKT))
	{
		dwValue = PTZ_FUNCTIONS;
		CameraControlPTZ(0, CCC_INVALID, &dwValue);
	}
	else if (0==sKey.Find(CSD_CAM_CONTROL_FKTEX))
	{
		dwValue = PTZ_FUNCTIONS_EX;
		CameraControlPTZ(0, CCC_INVALID, &dwValue);
	}
	else if (0==sKey.Find(CSD_CAM_CONTROL_TYPE))
	{
		dwValue = PTZ_TYPE;
		CameraControlPTZ(0, CCC_INVALID, &dwValue);
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
//		if (m_pDlg->GetMD())
//		{
//			sValue = m_pDlg->GetMD()->GetMDMaskSensitivity(wSource);
//		}
//		else
//		{
//			bFound = FALSE;
//		}
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
//		if (m_pDlg->GetMD())
//		{
//			sValue = m_pDlg->GetMD()->GetMDAlarmSensitivity(wSource);
//		}
//		else
//		{
//			bFound = FALSE;
//		}
	}
	if (sKey == CSD_RESOLUTIONS)
	{
		sValue = m_sResolutions;
	}
	else if (sKey == CSD_ROTATIONS)
	{
		sValue = m_sRotations;
	}
	else if (sKey == CSD_FRAMERATES)
	{
		sValue = m_sFramerates;
	}
	else if (sKey == CSD_COMPRESSIONS)
	{
		sValue = m_sCompressionTypes;
	}
	else if (sKey == CSD_NAME)
	{
		sValue = GetName();
	}
	else if (sKey == CSD_OUTPUTS)
	{
		sValue = _T("0");
	}
	else if (sKey == CSD_RELAIS)
	{
		sValue  = _T("0");
	}
	else if (sKey == CSD_INPUTS)
	{
		sValue  = m_sInputs;
	}
	else if (sKey == CSD_CAM_CAPTURE)
	{
		CStringArray sa;
		EnumFilterCategory(CLSID_CVidCapClassManager, FALSE, sa);
		ConcatenateStrings(sa, sValue, _T(','));
	}
	else
	{
		bFound = FALSE;
		if (sKey == CSD_BRIGHTNESS)
		{
			CamProperty = VideoProcAmp_Brightness;
			bFound = TRUE;
		}
		else if (sKey == CSD_CONTRAST)
		{
			CamProperty = VideoProcAmp_Contrast;
			bFound = TRUE;
		}
		else if (sKey == CSD_SATURATION)
		{
			CamProperty = VideoProcAmp_Saturation;
			bFound = TRUE;
		}
		else if (sKey == CSD_HUE_PERCENT)
		{
			CamProperty = VideoProcAmp_Hue;
			bFound = TRUE;
		}
		else if (sKey == CSD_SHARPNESS_PERCENT)
		{
			CamProperty = VideoProcAmp_Sharpness;
			bFound = TRUE;
		}
		else if (sKey == CSD_GAMMA_PERCENT)
		{
			CamProperty = VideoProcAmp_Gamma;
			bFound = TRUE;
		}
		else if (sKey == CSD_VIDEOGAIN_PERCENT)
		{
			CamProperty = VideoProcAmp_Gain;
			bFound = TRUE;
		}
		else if (sKey == CSD_COLORENABLE)
		{
			CamProperty = VideoProcAmp_ColorEnable;
			bFound = TRUE;
		}
		else if (sKey == CSD_WHITEBALANCE_PERCENT)
		{
			CamProperty = VideoProcAmp_WhiteBalance;
			bFound = TRUE;
		}
		else if (sKey == CSD_BACKLIGHTCOMPENSATION_PERCENT)
		{
			CamProperty = VideoProcAmp_BacklightCompensation;
			bFound = TRUE;
		}

		if (bFound)
		{
			bool bAuto;
			hr = CameraProperties(CamProperty, false, bAuto, dwValue);
			sValue.Format(_T("%d"), dwValue);
		}
		else
		{
			if (sKey == CSD_PAN_PERCENT)
			{
				CamCtrlProp = CameraControl_Pan;
				bFound = TRUE;
			}
			else if (sKey == CSD_TILT_PERCENT)
			{
				CamCtrlProp = CameraControl_Tilt;
				bFound = TRUE;
			}
			else if (sKey == CSD_ROLL_PERCENT)
			{
				CamCtrlProp = CameraControl_Roll;
				bFound = TRUE;
			}
			else if (sKey == CSD_ZOOM_PERCENT)
			{
				CamCtrlProp = CameraControl_Zoom;
				bFound = TRUE;
			}
			else if (sKey == CSD_EXPOSURE_PERCENT)
			{
				CamCtrlProp = CameraControl_Exposure;
				bFound = TRUE;
			}
			else if (sKey == CSD_IRIS_PERCENT)
			{
				CamCtrlProp = CameraControl_Iris;
				bFound = TRUE;
			}
			else if (sKey == CSD_FOCUS_PERCENT)
			{
				CamCtrlProp = CameraControl_Focus;
				bFound = TRUE;
			}
			if (bFound)
			{
				bool bAuto;
				DWORD dwValue;
				hr = CameraControl(CamCtrlProp, false, bAuto, dwValue);
				sValue.Format(_T("%d"), dwValue);
			}
		}
	}

	if (bFound)
	{
		if (hr == E_POINTER)
		{
			pOutput->DoIndicateError(0, id, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
		}
		else if (FAILED(hr))
		{
			CString str, sError, sDescription;
			GetErrorStrings(hr, sError, sDescription);
			sValue.Format(_T("Error:%x: %s: %s"), hr, sError, sDescription);
			pOutput->DoConfirmGetValue(id, sKey, sValue, dwUserData);
		}
		else if (sValue.IsEmpty())
		{
			sValue.Format(_T("%d"), dwValue);
			pOutput->DoConfirmGetValue(id, sKey, sValue, dwUserData);
		}
		else
		{
			pOutput->DoConfirmGetValue(id, sKey, sValue, dwUserData);
		}
	}
	return bFound;
}
//////////////////////////////////////////////////////////////////////////
BOOL CDsCaptureCamera::OnRequestSetValue(CIPCOutputIPcamUnit*pOutput, CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	BOOL  bFound = CDsCamera::OnRequestSetValue(pOutput, id, sKey, sValue, dwUserData);
	if (bFound)
	{
		return bFound;
	}
	bFound = TRUE;

	VideoProcAmpProperty CamProperty = (VideoProcAmpProperty)0;
	CameraControlProperty CamCtrlProp = (CameraControlProperty)0;
	DWORD dwValue = 0;
	HRESULT  hr = S_OK;

	if (sKey == CSD_CAMERA_CONTROL)
	{
		dwValue = _ttoi(sValue);
		CameraControlPTZ(0, (CameraControlCmd)dwValue, NULL);
	}
	else if (sKey == CSD_MD)
	{
//		WK_TRACE(_T("SetMotionDetection\n"));
//		m_pDlg->SetMotionDetection(wSource, (sValue == CSD_ON) ? true : false);
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
//		if (m_pDlg->GetMD())
//		{
//			m_pDlg->GetMD()->SetMDMaskSensitivity(wSource, sValue);
//		}
//		else
//		{
//			bFound = FALSE;
//		}
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
//		if (m_pDlg->GetMD())
//		{
//			m_pDlg->GetMD()->SetMDAlarmSensitivity(wSource, sValue);
//		}
//		else
//		{
//			bFound = FALSE;
//		}
	}
	else if (sKey == CSD_CAM_CAPTURE)
	{
		RELEASE_OBJECT(m_pCapture);
		CString sName = sValue;
		hr = AddFilterToGraph(m_pGraph, &m_pCapture, CLSID_CVidCapClassManager, sName, AFTG_MATCH_EXACT, L"VideoCapture");
		InitParameters();
	}
	else if (sKey == CSD_NAME)
	{
		HRESULT hr;
		IMoniker*pMoniker = NULL;
		LPOLESTR bstr = NULL;
		CStringArray sa;
		SplitString(sValue, sa, _T(","));

		try
		{
			hr = FindMonikerByName(CLSID_CVidCapClassManager, sa[0], AFTG_MATCH_EXACT, &pMoniker);
			HRESULT_EXCEPTION(hr);

			hr = pMoniker->GetDisplayName(NULL, NULL, &bstr);
			HRESULT_EXCEPTION(hr);
			// set the name for device notifications
			CString sDisplayName(bstr);
			sDisplayName.Replace(_T("#"), _T("\\"));
			int nFind1 = sDisplayName.Find(_T("\\\\?\\"));
			if (nFind1 != -1)
			{
				int nFind2 = sDisplayName.Find(_T("\\{"), nFind1);
				if (nFind2 != -1)	// and change it also
				{
					sDisplayName = _T("SYSTEM\\CurrentControlSet\\Enum") + sDisplayName.Mid(nFind1+3, (nFind2-nFind1)-3);
					CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
					BOOL bRet = wkp.WriteString(sDisplayName, _T("FriendlyName"), sa[1]);
					hr = bRet ? S_OK : REGDB_E_WRITEREGDB;
					HRESULT_EXCEPTION(hr);
				}
			}

			IPropertyBag*pBag;
			hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void**)&pBag);
			HRESULT_EXCEPTION(hr);

	        VARIANT varName;
			varName.vt = VT_BSTR;
			varName.bstrVal = sa[1].AllocSysString();
			hr = pBag->Write(L"FriendlyName", &varName);
			SysFreeString(varName.bstrVal);
			HRESULT_EXCEPTION(hr);
			pOutput->DoConfirmSetValue(id, sKey, sa[1], dwUserData);
			return TRUE;
		}
		catch (ErrorStruct *ps)
		{
			ReportError(ps, FALSE);
		}
		RELEASE_OBJECT(pMoniker);
		SAFE_COTASKMEMFREE(bstr);
	}
	else
	{
		bFound = FALSE;
		if (sKey == CSD_BRIGHTNESS)
		{
			CamProperty = VideoProcAmp_Brightness;
			bFound = TRUE;
		}
		else if (sKey == CSD_CONTRAST)
		{
			CamProperty = VideoProcAmp_Contrast;
			bFound = TRUE;
		}
		else if (sKey == CSD_SATURATION)
		{
			CamProperty = VideoProcAmp_Saturation;
			bFound = TRUE;
		}
		else if (sKey == CSD_HUE_PERCENT)
		{
			CamProperty = VideoProcAmp_Hue;
			bFound = TRUE;
		}
		else if (sKey == CSD_SHARPNESS_PERCENT)
		{
			CamProperty = VideoProcAmp_Sharpness;
			bFound = TRUE;
		}
		else if (sKey == CSD_GAMMA_PERCENT)
		{
			CamProperty = VideoProcAmp_Gamma;
			bFound = TRUE;
		}
		else if (sKey == CSD_VIDEOGAIN_PERCENT)
		{
			CamProperty = VideoProcAmp_Gain;
			bFound = TRUE;
		}
		else if (sKey == CSD_COLORENABLE)
		{
			CamProperty = VideoProcAmp_ColorEnable;
			bFound = TRUE;
		}
		else if (sKey == CSD_WHITEBALANCE_PERCENT)
		{
			CamProperty = VideoProcAmp_WhiteBalance;
			bFound = TRUE;
		}
		else if (sKey == CSD_BACKLIGHTCOMPENSATION_PERCENT)
		{
			CamProperty = VideoProcAmp_BacklightCompensation;
			bFound = TRUE;
		}

		if (bFound)
		{
			bool bAuto;
			hr = CameraProperties(CamProperty, true, bAuto, dwValue);
		}
		else
		{
			if (sKey == CSD_PAN_PERCENT)
			{
				CamCtrlProp = CameraControl_Pan;
				bFound = TRUE;
			}
			else if (sKey == CSD_TILT_PERCENT)
			{
				CamCtrlProp = CameraControl_Tilt;
				bFound = TRUE;
			}
			else if (sKey == CSD_ROLL_PERCENT)
			{
				CamCtrlProp = CameraControl_Roll;
				bFound = TRUE;
			}
			else if (sKey == CSD_ZOOM_PERCENT)
			{
				CamCtrlProp = CameraControl_Zoom;
				bFound = TRUE;
			}
			else if (sKey == CSD_EXPOSURE_PERCENT)
			{
				CamCtrlProp = CameraControl_Exposure;
				bFound = TRUE;
			}
			else if (sKey == CSD_IRIS_PERCENT)
			{
				CamCtrlProp = CameraControl_Iris;
				bFound = TRUE;
			}
			else if (sKey == CSD_FOCUS_PERCENT)
			{
				CamCtrlProp = CameraControl_Focus;
				bFound = TRUE;
			}
			if (bFound)
			{
				bool bAuto;
				DWORD dwValue;
				hr = CameraControl(CamCtrlProp, true, bAuto, dwValue);
			}
		}
	}
	if (bFound)
	{
		if (hr == E_POINTER)
		{
			pOutput->DoIndicateError(0, id, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
		}
		else if (FAILED(hr))
		{
			CString str, sError, sDescription;
			GetErrorStrings(hr, sError, sDescription);
			str.Format(_T("Error: %s: %s"), sError, sDescription);
			pOutput->DoConfirmSetValue(id, sKey, str, (DWORD)hr);
		}
		else
		{
			pOutput->DoConfirmSetValue(id, sKey, sValue, dwUserData);
		}
	}
	return bFound;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CDsCaptureCamera::CameraControlPTZ(UINT nTimerEvent, CameraControlCmd Cmd, DWORD *pdwReturn)
{
	if (m_pCapture == NULL)
	{
		return E_POINTER;
	}
	IAMCameraControl *pVPA = NULL;
	HRESULT hr = m_pCapture->QueryInterface(IID_IAMCameraControl, (void**)&pVPA);
	if (SUCCEEDED(hr))
	{
		bool bTimer = false;
		CString sPosition;
		switch (Cmd)
		{
			case CCC_INVALID:
			if (pdwReturn)
			{
				if (*pdwReturn == PTZ_FUNCTIONS)
				{
					*pdwReturn = 0;
					long lValue, lFlags;
					if (SUCCEEDED(pVPA->Get(CameraControl_Pan     , &lValue, &lFlags))) *pdwReturn |= PTZF_PAN|PTZF_TURN_180;
					if (SUCCEEDED(pVPA->Get(CameraControl_Tilt    , &lValue, &lFlags))) *pdwReturn |= PTZF_TILT;
					if (SUCCEEDED(pVPA->Get(CameraControl_Zoom    , &lValue, &lFlags))) *pdwReturn |= PTZF_ZOOM;
					if (SUCCEEDED(pVPA->Get(CameraControl_Iris    , &lValue, &lFlags))) *pdwReturn |= PTZF_IRIS;
					if (SUCCEEDED(pVPA->Get(CameraControl_Focus   , &lValue, &lFlags))) *pdwReturn |= PTZF_FOCUS;
					if (SUCCEEDED(pVPA->Get(CameraControl_Roll    , &lValue, &lFlags))) *pdwReturn |= PTZF_ROLL;
					if (SUCCEEDED(pVPA->Get(CameraControl_Exposure, &lValue, &lFlags))) *pdwReturn |= PTZF_EXPOSURE;
					*pdwReturn |= PTZF_SPEED|PTZF_POSALL|PTZF_EXTENDED;
				}
				else if (*pdwReturn == PTZ_FUNCTIONS_EX)
				{
					// TODO! RKE: implement and test ptz and extended features
					*pdwReturn = PTZF_EX_ERASE_PRESET|PTZF_EX_SPEED_READJUST;
				}
				else if (*pdwReturn == PTZ_TYPE)
				{
					*pdwReturn = CCT_DIRECT_SHOW;
				}
			}break;
			case CCC_PAN_STOP:
			case CCC_TILT_STOP:
			case CCC_FOCUS_STOP:
			case CCC_IRIS_STOP:
			case CCC_ZOOM_STOP:
				m_nPTZCmd = 0;
				if (nTimerEvent)
				{
					KillThreadTimer(nTimerEvent);
				}
				break;
			case CCC_PAN_LEFT:   case CCC_PAN_RIGHT: 
			case CCC_TILT_UP:    case CCC_TILT_DOWN:
			case CCC_FOCUS_NEAR: case CCC_FOCUS_FAR:
			case CCC_IRIS_OPEN:  case CCC_IRIS_CLOSE:
			case CCC_ZOOM_IN:	 case CCC_ZOOM_OUT:
			if (nTimerEvent == 0)
			{
				int nTime = 500; // ms
				m_nPTZCmd = Cmd;
				if (pdwReturn)
				{
					m_nPTZSpeed = *pdwReturn;
					if (*pdwReturn < 6)
					{
						*pdwReturn = 6 - *pdwReturn;
						nTime *= *pdwReturn;
					}
				}
				bTimer = TRUE;
				SetThreadTimer(PTZ_TIMER, nTime);
				nTimerEvent = PTZ_TIMER;
			}	break;
			case CCC_FOCUS_AUTO:
			case CCC_IRIS_AUTO:
			case CCC_TURN_180:
				bTimer = TRUE;
				break;
			case CCC_POS_HOME: sPosition = _T("Home"); break;
			case CCC_POS_1:    sPosition = _T("Pos1"); break;
			case CCC_POS_2:    sPosition = _T("Pos2"); break;
			case CCC_POS_3:    sPosition = _T("Pos3"); break;
			case CCC_POS_4:    sPosition = _T("Pos4"); break;
			case CCC_POS_5:    sPosition = _T("Pos5"); break;
			case CCC_POS_6:    sPosition = _T("Pos6"); break;
			case CCC_POS_7:    sPosition = _T("Pos7"); break;
			case CCC_POS_8:    sPosition = _T("Pos8"); break;
			case CCC_POS_9:    sPosition = _T("Pos9"); break;
			case CCC_CONFIG:	break;
			default:
				bTimer = TRUE;
				break;
		}
		if (bTimer)
		{
			CameraControlProperty ccp = (CameraControlProperty)-1;
			long lMin, lMax, lDelta, lDefault, lValue,
				  lFlags = CameraControl_Flags_Manual, 
				  lSteps = m_nPTZSpeed;
			if (lSteps < 6) lSteps  = 1; // Timer langsamer
			else            lSteps -= 4; // Schrittweite hoeher
			bool bUp   = false,
				  bSet  = true,
			     bAuto = false;

			switch (m_nPTZCmd)
			{
				case CCC_PAN_RIGHT: bUp = true;
				case CCC_PAN_LEFT:  ccp = CameraControl_Pan;
					break;
				case CCC_TILT_UP:   bUp = true;
				case CCC_TILT_DOWN: ccp = CameraControl_Tilt;
					break;
				case CCC_FOCUS_FAR: bUp = true;
				case CCC_FOCUS_NEAR:ccp = CameraControl_Focus;
					break;
				case CCC_IRIS_OPEN: bUp = true;
				case CCC_IRIS_CLOSE:ccp = CameraControl_Iris;
					break;
				case CCC_ZOOM_IN:   bUp = true;
				case CCC_ZOOM_OUT:  ccp = CameraControl_Zoom;
					break;
			}

			if (ccp == -1)
			{
				switch (Cmd)
				{
					case CCC_FOCUS_AUTO:
						bAuto = true;
						ccp   = CameraControl_Focus;
						break;
					case CCC_IRIS_AUTO:
						bAuto = true;
						ccp   = CameraControl_Iris;
						break;
					case CCC_TURN_180:
						ccp = CameraControl_Pan;
						lSteps = -1;
						break;
				}
			}

			hr = pVPA->GetRange(ccp, &lMin, &lMax, &lDelta, &lDefault, &lFlags);
			if (SUCCEEDED(hr))
			{
				hr = pVPA->Get(ccp, (long*)&lValue, &lFlags);
				if (bAuto)					// Set Auto
				{
					lFlags = CameraControl_Flags_Auto;
				}
				else if (lSteps == -1)	// Turn 180
				{
					if (lValue < 0) lValue += 180;
					else            lValue -= 180;
				}
				else if (bUp)				// Manual control
				{
					lValue += lDelta * lSteps;
					if (lValue >= lMax) bSet = false;
				}
				else
				{
					lValue -= lDelta * lSteps;
					if (lValue <= lMin) bSet = false;
				}
				if (bSet)
				{
					hr = pVPA->Set(ccp, lValue, lFlags);
				}
				else if (nTimerEvent)
				{
					KillThreadTimer(nTimerEvent);
				}
			}
		}
		else if (!sPosition.IsEmpty())
		{
			CString strSection;
			CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.GetDlg()->GetKey(), _T(""));
			strSection.Format(CAMERA_N, GetCameraSubID()+1);
			strSection += _T("\\") + sPosition;
			long lValue,
				  lFlags = CameraControl_Flags_Manual;
			if (pdwReturn) // Preset
			{
				hr = pVPA->Get(CameraControl_Pan, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_PAN, lValue);
				}
				hr = pVPA->Get(CameraControl_Tilt, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_TILT, lValue);
				}
				hr = pVPA->Get(CameraControl_Zoom, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_ZOOM, lValue);
				}
				hr = pVPA->Get(CameraControl_Focus, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_FOCUS, lValue);
				}
				hr = pVPA->Get(CameraControl_Roll, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_ROLL, lValue);
				}
			}
			else // Set
			{
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_PAN, 360);
				if (lValue != 360)
				{
					hr = pVPA->Set(CameraControl_Pan, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_TILT, 360);
				if (lValue != 360)
				{
					hr = pVPA->Set(CameraControl_Tilt, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_ZOOM, 0);
				if (lValue != 0)
				{
					hr = pVPA->Set(CameraControl_Zoom, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_FOCUS, -1);
				if (lValue != -1)
				{
					hr = pVPA->Set(CameraControl_Focus, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_ROLL, 360);
				if (lValue != 360)
				{
					hr = pVPA->Set(CameraControl_Roll, (long)lValue, lFlags);
				}
			}
		}

		pVPA->Release();
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsCaptureCamera::CameraProperties(VideoProcAmpProperty CameraProperty, bool bSet, bool &bAuto, DWORD &dwValue)
{
	if (m_pCapture == NULL)
	{
		return E_POINTER;
	}
	IAMVideoProcAmp *pVPA = NULL;
	HRESULT hr = m_pCapture->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVPA);
	if (SUCCEEDED(hr))
	{
		long lMin, lMax, lDelta, lDefault, lFlags = 0,
			  lMinA, lMaxA;
		double y, m;
		hr = pVPA->GetRange(CameraProperty, &lMin, &lMax, &lDelta, &lDefault, &lFlags);
		switch (CameraProperty)
		{
			case VideoProcAmp_Brightness:
				lMinA = MIN_BRIGHTNESS;
				lMaxA = MAX_BRIGHTNESS;
				break;
			case VideoProcAmp_Contrast:
				lMinA = MIN_CONTRAST;
				lMaxA = MAX_CONTRAST;
				break;
			case VideoProcAmp_Saturation:
				lMinA = MIN_SATURATION;
				lMaxA = MAX_SATURATION;
				break;
			default:
				lMinA = 0;
				lMaxA = 1000;
				break;
		}

		if (SUCCEEDED(hr))
		{
			if (bSet)
			{
				lFlags = (bAuto) ? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual;
				m = (lMax-lMin) / (double)(lMaxA-lMinA);
				y = m * dwValue + lMin;
				dwValue = (DWORD)y;
				hr = pVPA->Set(CameraProperty, dwValue, lFlags);
			}
			else
			{
				hr = pVPA->Get(CameraProperty, (long*)&dwValue, &lFlags);
				m = (lMaxA-lMinA) / (double)(lMax-lMin);
				y = m * dwValue + lMinA;
				dwValue = (DWORD)y;
				if (lFlags & VideoProcAmp_Flags_Auto)   bAuto = true;
				if (lFlags & VideoProcAmp_Flags_Manual) bAuto = false;
			}
		}
		pVPA->Release();
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsCaptureCamera::CameraControl(CameraControlProperty CameraProperty, bool bSet, bool &bAuto, DWORD &dwValue)
{
	if (m_pCapture == NULL)
	{
		return E_POINTER;
	}
	IAMCameraControl *pVPA = NULL;
	HRESULT hr = m_pCapture->QueryInterface(IID_IAMCameraControl, (void**)&pVPA);
	if (SUCCEEDED(hr))
	{
		long lMin, lMax, lDelta, lDefault, lFlags = 0,
			  lMinA = 0, lMaxA = 1000;
		double y, m;

		hr = pVPA->GetRange(CameraProperty, &lMin, &lMax, &lDelta, &lDefault, &lFlags);
		if (SUCCEEDED(hr))
		{
			if (bSet)
			{
				lFlags = (bAuto) ? CameraControl_Flags_Auto : CameraControl_Flags_Manual;
				m = (lMax-lMin) / (double)(lMaxA-lMinA);
				y = m * dwValue + lMin;
				dwValue = (DWORD)y;
				hr = pVPA->Set(CameraProperty, dwValue, lFlags);
			}
			else
			{
				hr = pVPA->Get(CameraProperty, (long*)&dwValue, &lFlags);
				m = (lMaxA-lMinA) / (double)(lMax-lMin);
				y = m * dwValue + lMinA;
				dwValue = (DWORD)y;
				if (lFlags & VideoProcAmp_Flags_Auto)   bAuto = true;
				if (lFlags & VideoProcAmp_Flags_Manual) bAuto = false;
			}
		}
		pVPA->Release();
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
void	CDsCaptureCamera::InitParameters()
{
	HRESULT				hr;
	IPin*				pPinOut	= NULL;
	IAMVideoControl*	pVC		= NULL;
	IAMStreamConfig*	pCfg		= NULL;
	AM_MEDIA_TYPE*		pmt=NULL;
	CString				sText;
	CStringArray		saResoultions, saCompressionTypes;
	long i, lCount = 0;
	long nSelOPF = -1;

	m_sResolutions.Empty();
	m_sRotations.Empty();
	m_sCompressionTypes.Empty();
	m_sFramerates.Empty();
	m_sInputs = _T("0");
	m_nCapturePin = 0;

	try
	{
		for (i=0; ; i++)
		{
			hr = GetPin(m_pCapture, PINDIR_OUTPUT, i, &pPinOut);
			if (FAILED(hr)) break;
			GUID guid;
			hr = GetPinCategory(pPinOut, &guid);
			RELEASE_OBJECT(pPinOut);
			if (SUCCEEDED(hr))
			{
				if (guid == PIN_CATEGORY_CAPTURE)
				{
					m_nCapturePin = i;
					break;
				}
			}
		}

		hr = GetPin(m_pCapture, PINDIR_OUTPUT, m_nCapturePin, &pPinOut);
		HRESULT_EXCEPTION(hr);
		SIZE szOutput = {0, 0};
		hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
		if (SUCCEEDED(hr))
		{
			VIDEO_STREAM_CONFIG_CAPS *pVSCC = NULL;
			int  iCount, iSize;
	
			hr = pCfg->GetNumberOfCapabilities(&iCount, &iSize);
			if (SUCCEEDED(hr))
			{
				pVSCC = (VIDEO_STREAM_CONFIG_CAPS*) CoTaskMemAlloc(iSize);

				for (i=0; i<iCount; i++)
				{
					pCfg->GetStreamCaps(i, &pmt, (BYTE*)pVSCC);
					if (nSelOPF == -1)
					{
						nSelOPF = i;
						szOutput = pVSCC->InputSize;
					}
					GetGUIDString(sText, &pmt->subtype);
					int nFind = sText.Find(_T("_"));
					if (nFind != -1) sText = sText.Mid(nFind+1);
					SAFE_DELETEMEDIATYPE(pmt);
					if (FindString(saCompressionTypes, sText, TRUE, TRUE) == -1)
					{
						saCompressionTypes.Add(sText);
					}

					Resolution r = EnumOfSize(pVSCC->InputSize);
					if (r == m_Resolution)
					{
						nSelOPF = i;
						szOutput = pVSCC->InputSize;
					}
					if (r == RESOLUTION_NONE)
					{
						sText.Format(_T("%dx%d"), pVSCC->InputSize.cx, pVSCC->InputSize.cy);
					}
					else
					{
						sText = ShortNameOfEnum(r);
					}
					if (FindString(saResoultions, sText, TRUE, TRUE) == -1)
					{
						saResoultions.Add(sText);
					}
				}
			}
			SAFE_COTASKMEMFREE(pVSCC);
		}
		
		hr = m_pCapture->QueryInterface(IID_IAMVideoControl, (void**)&pVC);
		if (SUCCEEDED(hr))
		{
			pVC->GetCaps(pPinOut, &lCount);
			m_sRotations += _T("0");
			if (lCount & VideoControlFlag_FlipVertical)
			{
				m_sRotations += _T(",") FLIP_VERTICAL;
			}
			if (lCount & VideoControlFlag_FlipHorizontal)
			{
				m_sRotations += _T(",") FLIP_HORIZONTAL;
				if (lCount & VideoControlFlag_FlipVertical)
				{
					m_sRotations += _T(",180");
				}
			}
			if (lCount & VideoControlFlag_Trigger)
			{
				m_sInputs = _T("1");
			}

			pVC->GetMode(pPinOut, &lCount);
			lCount &= ~(VideoControlFlag_FlipVertical|VideoControlFlag_FlipHorizontal);
			if (m_sRotation == _T("180"))
			{
				lCount |= VideoControlFlag_FlipVertical|VideoControlFlag_FlipHorizontal;
			}
			else if (m_sRotation == FLIP_VERTICAL)
			{
				lCount |= VideoControlFlag_FlipVertical;
			}
			else if (m_sRotation == FLIP_HORIZONTAL)
			{
				lCount |= VideoControlFlag_FlipHorizontal;
			}
			pVC->SetMode(pPinOut, lCount);

			LONGLONG *pllFramRate = NULL;
			hr = pVC->GetFrameRateList(pPinOut, nSelOPF, szOutput, &lCount, &pllFramRate);
			if (SUCCEEDED(hr))
			{
				double dRateMS, dFps;
				int nFps;
				int nMinDiff = 1000, nSel = -1;
				for (i=0; i<lCount; i++)
				{
					dRateMS = pllFramRate[i]*100e-6;
					dFps    = 1/dRateMS*1000;
					nFps = (int)(dFps+0.5);
					if (nFps)
					{
						int nDiff = abs(nFps - m_iFPS);
						if (nDiff < nMinDiff)
						{
							nSel = i;
							nMinDiff = nDiff;
						}
						sText.Format(_T("%d"), nFps); // fps
					}
					else
					{
						sText.Format(_T("%d ms"), (int)dRateMS); // ms
					}
					m_sFramerates += sText;
					if (i<lCount-1)
					{
						m_sFramerates += _T(",");
					}
				}
				if (nSel != -1)
				{
					AM_MEDIA_TYPE*pmt;
					hr = pCfg->GetFormat(&pmt);
					if (SUCCEEDED(hr) && pmt->formattype == FORMAT_VideoInfo)
					{
						VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
						pVIH->AvgTimePerFrame = pllFramRate[nSel];
						hr = pCfg->SetFormat(pmt);
					}
				}

			}
			SAFE_COTASKMEMFREE(pllFramRate);
		}
	}
	catch (ErrorStruct *ps)
	{
		ReportError(ps, FALSE);
	}

	RELEASE_OBJECT(pPinOut);
	RELEASE_OBJECT(pVC);
	RELEASE_OBJECT(pCfg);

	saCompressionTypes.Add(_T("----------"));
	EnumFilterCategory(CLSID_CIcmCoClassManager, FALSE, saCompressionTypes);
	ConcatenateStrings(saCompressionTypes, m_sCompressionTypes, _T(','));
	ConcatenateStrings(saResoultions, m_sResolutions, _T(','));
}
/////////////////////////////////////////////////////////////////////////////
// CDsCaptureCamera message handlers
void CDsCaptureCamera::OnTimer(WPARAM dwID, LPARAM lTime)
{
	static LONG_PTR lOldTime = 0;
	if (dwID == (WPARAM)PTZ_TIMER)
	{
		CameraControlPTZ((UINT)dwID, (CameraControlCmd)-1, NULL);
	}
	TRACE(_T("CDsCaptureCamera::OnTimer(%d, %d)\n"), dwID, lTime-lOldTime);
	lOldTime = lTime;
}
/////////////////////////////////////////////////////////////////////////////
void CDsCaptureCamera::OnSetFilters(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case PREVIEWED:
			if (m_pRenderer == NULL)
			{
				OnSetMediaState(State_Stopped, 0);
				HRESULT  hr = m_pGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pCapture, NULL, NULL);
				if (SUCCEEDED(hr))
				{
					m_dwFlags |= DSTF_PREVIEW;
				}
				CWnd *pWnd = theApp.GetDlg()->GetDlgItem(IDC_STAT_PICTURE);
				if (pWnd)
				{
					IVideoWindow *pVW = NULL;
					hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVW);
					if (SUCCEEDED(hr))
					{

						pVW->put_Owner((OAHWND)pWnd->m_hWnd);
						pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
						RECT grc;
						pWnd->GetClientRect(&grc);
						pVW->SetWindowPosition(0, 0, grc.right, grc.bottom);
					}
				}
				OnSetMediaState(State_Running, 0);
			}
			break;
		default:
			CDsCamera::OnSetFilters(wParam, lParam);
			break;
	}
}