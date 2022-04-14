// CIPCOutputMiniClient.cpp: implementation of the CIPCOutputMiniClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCStringDefs.h"
#include "MiniClient.h"
#include "CIPCOutputMiniClient.h"
#include "MainFrm.h"
#include "MiniClientDef.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMiniClientApp theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CIPCOutputMiniClient::CIPCOutputMiniClient(CMainFrame* pMainFrame, LPCTSTR shmName)
					 :CIPCOutputClient(shmName, FALSE)

{
	m_pMainFrame = pMainFrame;
	m_nNumberOfOutputs = 0;
	StartThread();
}

//////////////////////////////////////////////////////////////////////
CIPCOutputMiniClient::~CIPCOutputMiniClient()
{

}

//////////////////////////////////////////////////////////////////////
void CIPCOutputMiniClient::OnAddRecord(const CIPCOutputRecord &pRec)
{
	if (pRec.IsCamera() && pRec.CameraDoesJpeg())
	{
		WK_TRACE(_T("%s 0x%x\n"), pRec.GetName(), pRec.GetID());
	}	
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputMiniClient::OnUpdateRecord(const CIPCOutputRecord &pRec)
{
	TRACE(_T("OnUpdateRecord\n"));
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputMiniClient::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[])
{
	WK_TRACE(_T("GroupID=%x NumGroups=%d NumRecords=%d\n"), wGroupID, iNumGroups, numRecords);
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID, iNumGroups, numRecords, records);
	m_nNumberOfOutputs = GetNumberOfOutputs();
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputMiniClient::OnDeleteRecord(const CIPCOutputRecord &pRec)
{
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputMiniClient::RequestPictures(WORD wSource)
{
	WORD wI		= 0;
	wSource--;	// Die MiCoUnit zählt die Kameras ab 0

	WK_TRACE(_T("RequestPictures from Camera %u\n"), wSource);
	// Wait until CIPC-Connected
	while ((GetIPCState() != CIPC_STATE_CONNECTED) && (wI++ < 50))
	{
		Sleep(100);
	}
									 
	// Wait until OnConfirmInfoOutputs received
	wI = 0;
	while (m_nNumberOfOutputs == 0 && (wI++ < 50))
	{
		Sleep(100);
	}											 

	if (GetIPCState() != CIPC_STATE_CONNECTED)
	{
		WK_TRACE_ERROR(_T("CIPCOutputMiniClient::RequestPictures CIPCState != CIPC_STATE_CONNECTED\n"));
		return FALSE;
	}
						
	for (int nI = 0; nI < m_nNumberOfOutputs; nI++)
	{
		const CIPCOutputRecord& PictRecord = GetOutputRecordFromIndex(nI);
		
		if (PictRecord.IsCamera() && PictRecord.CameraDoesJpeg())   
		{
			if (PictRecord.GetID().GetSubID() == wSource)
			{
				DoRequestNewJpegEncoding(PictRecord.GetID(),RESOLUTION_HIGH,COMPRESSION_1, 1,0); 
				return TRUE;
			}
		}
		Sleep(10);
	}								   

	WK_TRACE_ERROR(_T("CIPCOutputMiniClient::RequestPictures Can't find Output\n"));
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputMiniClient::OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserID, CSecID idArchive)
{
	CIPCPictureRecord *pTmpPict = new CIPCPictureRecord(pict);
	
	if (m_pMainFrame)
		m_pMainFrame->PostMessage(WM_SHOW_IMAGE, 0, (LPARAM)pTmpPict);

	// Wenn sich der Client NICHT im Testmodus befindet, die Bildanforderung nach erhalt stoppen
	if (!theApp.m_bMiniClientTest)
		DoRequestNewJpegEncoding(pict.GetCamID(),RESOLUTION_HIGH,COMPRESSION_1, 0,0); //Stop
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputMiniClient::OnRequestDisconnect()
{
	if (m_pMainFrame)
		m_pMainFrame->PostMessage(WM_QUIT, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputMiniClient::SetCameraParameter(WORD wSource, int nBrigtness, int nContrast, int nSaturation)
{
	WORD wI			= 0;
	CString sTemp	= _T("");
	wSource--;		// Die MiCoUnit zählt die Kameras ab 0

	while ((GetIPCState() != CIPC_STATE_CONNECTED) && (wI++ < 50))
	{
		Sleep(100);
	}
	
	if (GetIPCState() != CIPC_STATE_CONNECTED)
		return FALSE;

	CIPCOutputRecord PictRecord;
	for (int nI = 0; nI < GetNumberOfOutputs(); nI++)
	{
		PictRecord = GetOutputRecordFromIndex(nI);
		if (PictRecord.IsCamera() && PictRecord.CameraDoesJpeg())
		{
			if (PictRecord.GetID().GetSubID() == wSource)
			{
				sTemp.Format(_T("%d"), nBrigtness);
				DoRequestSetValue(PictRecord.GetID(), CSD_BRIGHTNESS, sTemp);
				sTemp.Format(_T("%d"), nContrast);
				DoRequestSetValue(PictRecord.GetID(), CSD_CONTRAST,   sTemp);
				sTemp.Format(_T("%d"), nSaturation);
				DoRequestSetValue(PictRecord.GetID(), CSD_SATURATION, sTemp);
				return TRUE;
			}
		}
		Sleep(10);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputMiniClient::RequestCameraParameter(WORD wSource)
{
	WORD wI	= 0;

	wSource--; // Die MiCoUnit zählt Kameras ab 0

	while ((GetIPCState() != CIPC_STATE_CONNECTED) && (wI++ < 50))
	{
		Sleep(100);
	}
	
	if (GetIPCState() != CIPC_STATE_CONNECTED)
		return FALSE;

	CIPCOutputRecord PictRecord;
	for (int nI = 0; nI < GetNumberOfOutputs(); nI++)
	{
		PictRecord = GetOutputRecordFromIndex(nI);
		if (PictRecord.IsCamera() && PictRecord.CameraDoesJpeg())
		{
			if (PictRecord.GetID().GetSubID() == wSource)
			{
				DoRequestGetValue(PictRecord.GetID(), CSD_BRIGHTNESS);
				DoRequestGetValue(PictRecord.GetID(), CSD_CONTRAST);
				DoRequestGetValue(PictRecord.GetID(), CSD_SATURATION);
				return TRUE;
			}
		}
		Sleep(10);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputMiniClient::OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	static int nBrightness  = -1;
	static int nContrast	= -1;
	static int nSaturation	= -1;

	int nValue  = _ttoi((LPCTSTR)sValue);
	int nSource = id.GetSubID()+1; // Der HHLA-Server zählt Kameras ab 1

	if (sKey == CSD_BRIGHTNESS)
		nBrightness = (nValue * 100 + 50) / 256;
	else if (sKey == CSD_CONTRAST)
		nContrast	= (nValue * 100 + 50) / 512;
	else if (sKey == CSD_SATURATION)
		nSaturation = (nValue * 100 + 50) / 512;
														   
	// Alle drei Parameter beisammen?
	if ((nBrightness != -1) && (nContrast != -1) && (nSaturation != -1))
	{
		if (m_pMainFrame)
		{
			::PostMessage(m_pMainFrame->GetRemoteControlWnd(),WM_CONF_CAMERA_PARAMETER,
				nSource, MAKELONG(MAKEWORD(0, nBrightness), MAKEWORD(nContrast, nSaturation)));
		}
		nBrightness = -1;						  
		nContrast	= -1;
		nSaturation	= -1;
	}
}

void CIPCOutputMiniClient::OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	WK_TRACE(_T("CIPCOutputMiniClient::OnConfirmSetValue Key=%s Value=%s\n"), sKey, sValue);

	static int nBrightness  = -1;
	static int nContrast	= -1;
	static int nSaturation	= -1;

	int nValue  = _ttoi((LPCTSTR)sValue);
	int nSource = id.GetSubID()+1; // Der HHLA-Server zählt Kameras ab 1

	if (sKey == CSD_BRIGHTNESS)
		nBrightness = (nValue * 100 + 50) / 256;
	else if (sKey == CSD_CONTRAST)
		nContrast	= (nValue * 100 + 50) / 512;
	else if (sKey == CSD_SATURATION)
		nSaturation = (nValue * 100 + 50) / 512;
														   
	// Alle drei Parameter beisammen?
	if ((nBrightness != -1) && (nContrast != -1) && (nSaturation != -1))
	{
		// Ein Bild anfordern.
		RequestPictures((WORD)nSource);
		nBrightness = -1;						  
		nContrast	= -1;
		nSaturation	= -1;
	}
}
