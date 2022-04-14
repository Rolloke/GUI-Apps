/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: CipcOutputIPcamUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/IPCameraUnit/CipcOutputIPcamUnit.cpp $
// CHECKIN:		$Date: 17.08.06 9:15 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 17.08.06 9:01 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "IPCameraUnitDlg.h"
#include "CameraParameterDlg.h"

#include "CIPCOutputIPcamUnit.h"
#include "CIPCExtraMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
	#define DBG_TRACE(S) WK_TRACE(_T("%s\n%s(%d):\n"), S, _T(__FILE__), __LINE__);
#else
	#define DBG_TRACE // 
#endif

#ifndef CCT_DIRECT_SHOW
	#define CCT_DIRECT_SHOW 20
#endif

CIPCOutputIPcamUnit::CIPCOutputIPcamUnit(CIpCameraUnitDlg* pMainWnd, 
							 LPCTSTR shmName,
							 BOOL basMaster)
	: CIPCOutput(shmName, basMaster)
{
	m_wGroupID			= SECID_NO_GROUPID;				// Noch keine GroupID
	m_pDlg				= pMainWnd;
	m_bReset		    = FALSE;
	m_dwRelaisState		= 0;

#ifdef _DEBUG
	m_pParamDlg = NULL;
#endif

/*
	m_lpSmallTestbild	= NULL;							// Pointer auf kleines Störungsbild
	m_dwSmallTestbildLen= 0;							// Länge des kleinen Störungsbildes
	m_lpLargeTestbild	= NULL;							// Pointer auf großes Störungsbild
	m_dwLargeTestbildLen= 0;							// Länge des großen Störungsbildes

	// Kleines Testbild aus der Resource laden
	HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_SMALL_TESTBILD), _T("BINRES"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwSmallTestbildLen	= SizeofResource(NULL, hRc);
			m_lpSmallTestbild		= (BYTE*)LockResource(hJpegResource);
		}
		else
			WK_TRACE_ERROR(_T("Kleines Testbild konnte nicht geladen werden\n"));
	}
	else
		WK_TRACE_ERROR(_T("Testbild konnte nicht geladen werden\n"));


	// Großes Testbild aus der Resource laden
	hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_LARGE_TESTBILD), _T("BINRES"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwLargeTestbildLen	= SizeofResource(NULL, hRc);
			m_lpLargeTestbild		= (BYTE*)LockResource(hJpegResource);
		}
		else
			WK_TRACE_ERROR(_T("Großes Testbild konnte nicht geladen werden\n"));
	}
	else
		WK_TRACE_ERROR(_T("Testbild konnte nicht geladen werden\n"));
*/

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputIPcamUnit::~CIPCOutputIPcamUnit()
{
	// TRACE(_T("CIPCOutputIPcamUnit::~CIPCOutputIPcamUnit\n"));
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputIPcamUnit::CheckGroupID(WORD wGid, LPCTSTR sFunction)
{
	if (wGid != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("%s: Wrong GroupID\n"), sFunction);
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestGetValue(CSecID id, const CString &sKey, DWORD dwUserData)
{
//	TRACE(_T("OnConfirmSetValue(%s, %x)\n"), sKey, dwUserData);
	CString sValue = _T("");
	if (!CheckGroupID(id.GetGroupID(), _T(__FUNCTION__)))
	{
		return;
	}

	if (m_pDlg)
	{
		m_pDlg->OnRequestGetValue(id, sKey, dwUserData);
	}
	else
	{
		WK_TRACE_ERROR(_T("No Main Window\n"));
		DoIndicateError(0, id, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
//	TRACE(_T("OnRequestSetValue(%s, %s, %x)\n"), sKey, sValue, dwUserData);
	if (!CheckGroupID(id.GetGroupID(), _T(__FUNCTION__)))
	{
		return;
	}
	if (m_pDlg)
	{
		m_pDlg->OnRequestSetValue(id, sKey, sValue, dwUserData);
	}
	else
	{
		WK_TRACE_ERROR(_T("No Main Window\n"));
		DoIndicateError(0, id, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestVersionInfo(WORD wGroupID)
{
	TRACE(_T("OnRequestVersionInfo\n"));
	DoConfirmVersionInfo(wGroupID, 1);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestReset(WORD wGroupID)	
{
	TRACE(_T("OnRequestReset(%s)\n"), GetShmName());
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}
	m_bReset = TRUE;
	m_pDlg->InitResetTimer();
	InitRelaisStrings();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestCurrentState(WORD wGroupID)
{
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}
	TRACE(_T("OnRequestCurrentState\n"));

	DoConfirmCurrentState(m_wGroupID, m_dwRelaisState);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::InitRelaisStrings()
{
	DWORD	dwMask;
	m_dwRelaisState = 0;

	m_saRelais.RemoveAll();

	CWK_Profile wkp;
	CString sSection, sRelais;
	sSection.Format(_T("%s\\") SEC_NAME_GROUP, SEC_NAME_OUTPUTGROUPS, m_wGroupID);
	WORD i;
	for (i=1, dwMask=1; i<=32; i++, dwMask<<=1)
	{
		sRelais   = wkp.GetString(sSection, i, NULL);
		if (sRelais.IsEmpty())
		{
			break;
		}
		if (DoRequestRelais(sRelais, TRUE, FALSE))
		{
			DoConfirmSetRelay(CSecID(m_wGroupID, i-1), TRUE);
			m_dwRelaisState |= dwMask;
		}
		m_saRelais.Add(sRelais);
	}
	m_pDlg->NotifyIOList(CSecID(m_wGroupID, SECID_NO_SUBID), TRUE);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
	if (!CheckGroupID(relayID.GetGroupID(), _T(__FUNCTION__)))
	{
		return;
	}

	BOOL bOK = FALSE;
	int nRelais = relayID.GetSubID();
	if (nRelais < m_saRelais.GetCount())
	{
		bOK = DoRequestRelais(m_saRelais.GetAt(nRelais), FALSE, bClosed);
	}
	if (bOK)
	{
		m_pDlg->NotifyIOList(relayID, bClosed);
		DoConfirmSetRelay(relayID, bClosed);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputIPcamUnit::DoRequestRelais(const CString& sRelais, BOOL bRequestValue, BOOL bClosed)
{
	BOOL bOK = FALSE;
	CString sType, sUrl, sHTTPRequest;
	int nPort, nNr;
	BOOL    bPost = FALSE;
	sType = CWK_Profile::GetStringFromString(sRelais, INI_RELAIS_TYPE, NULL);
	sUrl  = CWK_Profile::GetStringFromString(sRelais, INI_URL, NULL);
	nPort = CWK_Profile::GetNrFromString(sRelais, INI_PORT, 0);
	nNr   = CWK_Profile::GetNrFromString(sRelais, INI_RELAIS_NR, 1);
	if (sType == CSD_CAM_AXIS)
	{
		CString sLocation = _T("axis-cgi/io/output.cgi?");
		if (bRequestValue)
		{
			sHTTPRequest.Format(_T("check=%d"), nNr);
		}
		else
		{
			sHTTPRequest.Format(_T("action=%d:"), nNr);
			sHTTPRequest += bClosed ? _T("/") : _T("\\");
		}
		sHTTPRequest = sLocation + sHTTPRequest;
	}
	else if (sType == CSD_CAM_PANASONIC)
	{
		// TODO! RKE: implement Panasonic
	}
	// TODO! RKE: implement other relais protocolls
	if (!sHTTPRequest.IsEmpty())
	{
		DWORD dwResult;
		CString sAnswer = m_pDlg->HTTPRequest(sUrl, nPort, sHTTPRequest, dwResult, bPost);
		if (bRequestValue)
		{
			if (sType == CSD_CAM_AXIS)
			{
				bOK = sAnswer.Find(_T("=1")) != -1 ? TRUE : FALSE;
			}
			else if (sType == CSD_CAM_PANASONIC)
			{
				// TODO! RKE: implement Panasonic
			}
		}
		else
		{
			bOK = dwResult == HTTP_STATUS_OK;
		}
	}
	return bOK;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestSetGroupID(WORD wGroupID)
{
	TRACE(_T("OnRequestSetGroupID(%x)\n"), wGroupID);
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestHardware(WORD wGroupID)
{
	int iErrorCode = 0;
	TRACE(_T("OnRequestHardware\n"));
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	DoConfirmHardware(m_wGroupID, iErrorCode,
									TRUE,  // Can I BW-Compress
									FALSE, // Can I BW-DeCompress
									TRUE,  // Can I Color-Compress
									FALSE, // Can I Color-DeCompress
									FALSE);// Can I Overlay: Das waere das IVideo Renderer Window
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestDisconnect()
{
 	if (m_pDlg)
	{
		m_pDlg->InitResetTimer();
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestSetDisplayWindow(WORD wGroupID, const CRect &Rect)
{
//	DBG_TRACE(_T("OnRequestSetDisplayWindow"));
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	DoConfirmSetDisplayWindow(wGroupID);
}
		 
//////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestSetOutputWindow(WORD wGroupID, const CRect& /*Rect*/, OverlayDisplayMode /*odm*/)
{
//	DBG_TRACE(_T("OnRequestSetOutputWindow"));
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	DoConfirmSetOutputWindow(wGroupID, 0);
}

void CIPCOutputIPcamUnit::OnRequestStartVideo(CSecID id,			// camera nr.
									 Resolution res, Compression comp, CompressionType ct,
									 int iFPS, int iIFrameInterval, DWORD dwUserData)
{
	if (m_pDlg->IsShuttingDown())
	{
		return;
	}

	if (id == SECID_NO_ID)
	{
		WK_TRACE_WARNING(_T("CIPCOutputIPcamUnit::OnRequestStartVideo: camID=SECID_NOID\n"));
		return;
	}

	if (!CheckGroupID(id.GetGroupID(), _T(__FUNCTION__)))
	{
		return;
	}
	WORD wSubid = id.GetSubID();

	if (m_pDlg)
	{

		CameraParameters *pCP = new CameraParameters(dwUserData, res, comp, ct, iFPS, iIFrameInterval);
		m_pDlg->PostMessage(WM_SET_CAMERA_ACTIVE, (WPARAM)pCP, MAKELONG(wSubid, TRUE));
		Sleep(100);	// give the next camera a little time
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestStopVideo(CSecID camID, DWORD dwUserData)
{
	if (!CheckGroupID(camID.GetGroupID(), _T(__FUNCTION__)))
	{
		return;
	}
	if (m_pDlg)
	{
		WORD wSubid = camID.GetSubID();
		if (m_pDlg->IsCameraPresent(wSubid))
		{
			m_pDlg->PostMessage(WM_SET_CAMERA_ACTIVE, (WPARAM)0, MAKELONG(wSubid, FALSE));
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::IndicationVideoState(WORD wCamSubID, BOOL bVideoState)
{					  
	CString sError, sMsg;
	// TODO! RKE: indicate device lost message or something else
	   									 
	if (!bVideoState)		    
	{
		sMsg.LoadString(IDS_RTE_NO_VIDEO);
		sError.Format(sMsg, wCamSubID+1);
		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, wCamSubID+1, TRUE);
		RTerr.Send();
	}
	else
	{
		sMsg.LoadString(IDS_RTE_VIDEO);
		sError.Format(sMsg, wCamSubID+1);

		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, wCamSubID+1, FALSE);
		RTerr.Send();
	}

}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	if (!CheckGroupID(camID.GetGroupID(), _T(__FUNCTION__)))
	{
		return;
	}

	// TODO! RKE: implement MD filter access
	// DoConfirmGetMask(camID, dwUserID, mask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	if (!CheckGroupID(camID.GetGroupID(), _T(__FUNCTION__)))
	{
		return;
	}
	// TODO! RKE: implement MD filter access
	//	DoConfirmSetMask(camID, mask.GetType(), dwUserID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnResetButtonPressed()
{
	DoRequestSetValue(SECID_NO_ID, CSD_RESET, _T(""), 0);
}
//////////////////////////////////////////////////////////////////////////////////////

BOOL CIPCOutputIPcamUnit::IsConnected()
{
	return (GetIPCState() == CIPC_STATE_CONNECTED) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::ConfirmReset()
{
	if (m_bReset)
	{
		m_bReset = FALSE;
		DoConfirmReset(m_wGroupID);
	}
}
//////////////////////////////////////////////////////////////////////
#ifdef _TEST_SV_DLG
void CIPCOutputIPcamUnit::SetParamDlg(CCameraParameterDlg*pDlg)
{
	m_pParamDlg = pDlg;
	m_wGroupID = CSV_CONTROL_ID;
}
void CIPCOutputIPcamUnit::OnReadChannelFound()
{
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmConnection()
{
	if (GetIsMaster())
	{
		DoRequestSetGroupID(m_wGroupID);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmSetGroupID(WORD wGroupID)
{
	if (m_pParamDlg)
	{
		m_pParamDlg->PostMessage(WM_USER, WM_INITDIALOG);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmSetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData)
{
	TRACE(_T("OnConfirmSetValue(%s, %s, %x)\n"), sKey, sValue, dwUserData);
	if (m_pParamDlg)
	{
		m_pParamDlg->OnConfirmSetValue(id, sKey, sValue, dwUserData);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmGetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData)
{
	TRACE(_T("OnConfirmGetValue(%s, %s, %x)\n"), sKey, sValue, dwUserData);
	if (m_pParamDlg)
	{
		m_pParamDlg->OnConfirmGetValue(id, sKey, sValue, dwUserData);
	}
}
#endif
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmReset(WORD wGroupID)
{
	TRACE(_T("OnConfirmReset\n"));
}
