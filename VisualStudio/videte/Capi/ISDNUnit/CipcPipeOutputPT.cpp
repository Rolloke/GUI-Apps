/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcPipeOutputPT.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CipcPipeOutputPT.cpp $
// CHECKIN:		$Date: 27.07.05 10:29 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 27.07.05 10:23 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SecID.h"
#include "CIPCPipeOutputPT.h"

#include "CipcOutputRecord.h"
#include "CipcStringDefs.h"
#include "IsdnConnection.h"
#include "PTBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CIPCPipeOutputPT::CIPCPipeOutputPT(	CIPCType t,
						    CISDNConnection* pConnection,
							LPCTSTR pSMName,
							BOOL bAsMaster,
							DWORD dwOptions,
							WORD wID,
							DWORD dwRequestCounter,
							BOOL bIsAlarm
							)
			: CIPCPipeISDN(		// call super class constructor
			t, pConnection, pSMName,
			bAsMaster, dwOptions,wID,dwRequestCounter, bIsAlarm
			), CIPCOutput (pSMName,bAsMaster)
{

}

CIPCPipeOutputPT::~CIPCPipeOutputPT()
{
	StopThread();
}

/////////////////////////////////////////////////////////////////////////////
void CIPCPipeOutputPT::OnReadChannelFound()
{
	TRACE(_T("ReadChannelFound '%s'\n"), GetShmName());	// OBSOLETE
}

/////////////////////////////////////////////////////////////////////////////
void CIPCPipeOutputPT::OnRequestDisconnect()
{
	DelayedDelete();
	m_pConnection->RemoveMe(this);
}

CIPC *CIPCPipeOutputPT::GetCIPC()	// non const!
{
	return this;
}

void CIPCPipeOutputPT::OnRequestInfoOutputs(WORD wGroupID)
{
	int iRetry=100;
	while (iRetry && m_pConnection->GetPTBox()->GetPTConnectionState()!=PCS_CONNECTED) {
		WK_TRACE(_T("Output waiting for PCS_CONNECTED...\n"));
		Sleep(50);
		iRetry--;
	}
	if (m_pConnection->GetPTBox()->GetPTConnectionState()!=PCS_CONNECTED) {
		WK_TRACE_ERROR(_T("RequestInfoOutputs: Not connected ?\n"));
	}

	const CPTCameras & cams = m_pConnection->GetPTBox()->GetCameras();
	CIPCOutputRecord records[3+3];	// OOPS maximum, but not all are filled
	int c=0;
	for (c=0;c<cams.GetSize();c++) {
		WORD dwFlags = 
				OREC_IS_ENABLED|OREC_IS_CAMERA
				|OREC_CAMERA_COLOR|OREC_CAMERA_IS_FIXED
				|OREC_CAMERA_PRESENCE;

		if (cams[c]->HasVideoSignal()) {	// OOPS right condition
			dwFlags |= OREC_IS_OKAY;
		}
		records[c].Set(
		cams[c]->GetName(),
		cams[c]->GetID(),
		dwFlags
		); 
	}
	// NOT YET OREC_RELAY_CLOSED initial state
	DWORD dwRelayBits = m_pConnection->GetPTBox()->GetRelayBits();

	WORD wRelayFlags ;
	
	wRelayFlags = OREC_IS_OKAY|OREC_IS_ENABLED|OREC_IS_RELAY;
	if (dwRelayBits & 1 ) {
		wRelayFlags |= OREC_RELAY_CLOSED;
	}
	records[c].Set("B3Relay1",CSecID(SECID_GROUP_OUTPUT,3),wRelayFlags);

	wRelayFlags = OREC_IS_OKAY|OREC_IS_ENABLED|OREC_IS_RELAY;
	if (dwRelayBits & 2 ) {
		wRelayFlags |= OREC_RELAY_CLOSED;
	}
	records[c+1].Set("B3Relay2",CSecID(SECID_GROUP_OUTPUT,4),wRelayFlags);
	wRelayFlags = OREC_IS_OKAY|OREC_IS_ENABLED|OREC_IS_RELAY;
	if (dwRelayBits & 4 ) {
		wRelayFlags |= OREC_RELAY_CLOSED;
	}
	records[c+2].Set("B3Relay3",CSecID(SECID_GROUP_OUTPUT,5),wRelayFlags);

	int iNumRecords;
	if (m_pConnection->GetPTBox()->GetVersion()<200) {
		iNumRecords = c+1;
	} else {
		iNumRecords = c+3;
	}
	DoConfirmInfoOutputs(wGroupID, 1, iNumRecords,records);

}

//////////////////////////////////////////////////////////////////////////////
void CIPCPipeOutputPT::OnRequestBitrate()
{
	DoConfirmBitrate(64*1024);	// OOPS
}

void CIPCPipeOutputPT::OnRequestEncodedVideo(
								CSecID camID, 
								Resolution  res,
								Compression comp,
								WORD	wNumPictures,
								DWORD	dwBitrate,
								DWORD	dwUserData
							   )
{

	BOOL bActive;
	if (res==RESOLUTION_NONE || comp==COMPRESSION_NONE) {
		bActive = FALSE;
	} else {
		bActive = TRUE;
	}
	WK_TRACE(_T("Select camera %x, active %d\n"),camID.GetID(),bActive);
	m_pConnection->GetPTBox()->DoSelectCamera(camID, bActive,res);
}

void CIPCPipeOutputPT::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
	WK_TRACE(_T("Set Relay %x, active %d\n"),relayID.GetID(),bClosed);
	m_pConnection->GetPTBox()->DoSetRelay(relayID, bClosed);
}

#define CSD_RESOLUTION _T("Resolution")
#define CSD_BOX_NAME _T("BoxName")
#define CSD_BOX_NUMBER _T("BoxNumber")

void CIPCPipeOutputPT::OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					)
{
//	WK_TRACE(_T("GetValue %s\n"),(const char *)sKey);
	BOOL bSupported = FALSE;
	BOOL bIsNumeric =TRUE;

	const CPTCameras & cams = m_pConnection->GetPTBox()->GetCameras();
	const CPTCamera *pCam = cams.GetCameraByID(id);

	int iValue = 0;
	CString sValue;

	if (pCam)
	{
		if (   (sKey == CSD_CONTRAST)
			|| (sKey == CSD_BRIGHTNESS)
			|| (sKey == CSD_SATURATION)
			)
		{
			m_pConnection->GetPTBox()->DoRequestCameraValue(id,sKey);
			return;
		} 
		else if (sKey == CSD_RESOLUTION) 
		{
			iValue = pCam->GetResolution();
			bSupported = TRUE;
		}
	}
	else
	{
		if (sKey == CSD_BOX_NAME) 
		{		
			sValue = m_pConnection->GetPTBox()->GetBoxName();
			bSupported = TRUE;
			bIsNumeric = FALSE;
		} 
		else if (sKey == CSD_BOX_NUMBER) 
		{		
			sValue = m_pConnection->GetPTBox()->GetBoxNumber();
			bSupported = TRUE;
			bIsNumeric = FALSE;
		} 
		else 
		{
			WK_TRACE(_T("Unsupported key %s in GetValue\n"), LPCTSTR(sKey));
			bSupported=FALSE;
		}
	}

	// numeric values
	if (bSupported && bIsNumeric) 
	{
		CString sFormatValue;
		sFormatValue.Format(_T("%d"),iValue);

		DoConfirmGetValue(
			id, // might be used as group ID only
			sKey,
			sFormatValue,
			dwServerData
			);
	}
	
	// string values
	if (bSupported && bIsNumeric==FALSE) 
	{
		DoConfirmGetValue(
			id, // might be used as group ID only
			sKey,
			sValue,
			dwServerData
			);
	}
}


void CIPCPipeOutputPT::OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	WK_TRACE(_T("SetValue %s %s\n"), LPCTSTR(sKey), LPCTSTR(sValue));
	if (sKey==_T("CameraName")) 
	{
		// UNUSED int ix = id.GetSubID();
		// NOT YET holes
		m_pConnection->GetPTBox()->DoSetCameraName(id.GetSubID(),sValue);
	} 
	else if (sKey==_T("WriteEEP")) 
	{
		m_pConnection->GetPTBox()->DoWriteEEP();
	} 
	else if (sKey == CSD_CONTRAST) 
	{
		// range is 0..511
		m_pConnection->GetPTBox()->DoSetCameraValue(id,sKey,sValue);
	} 
	else if (sKey == CSD_BRIGHTNESS) 
	{
		// range is 0..254
		m_pConnection->GetPTBox()->DoSetCameraValue(id,sKey,sValue);
	} 
	else if (sKey == CSD_SATURATION) 
	{
		// range is 0..511
		m_pConnection->GetPTBox()->DoSetCameraValue(id,sKey,sValue);
	} 
	else if (sKey == CSD_BOX_NAME) 
	{
		// range is 0..511
		m_pConnection->GetPTBox()->DoSetHostName(sValue);
	} 
	else 
	{
		WK_TRACE(_T("Unsupported key %s in SetValue\n"), LPCTSTR(sKey));
	}

}

#if 0
// test for compiler options
extern int Foo();
extern int Goo();
extern int Hoo();

int Foo()
{
	int foo;
	return 1;
	if (foo) {
		return 0;
	}
}

int Goo()
{

	int goo =1;
	int unsed=0;
	if (goo) {
		goo = goo + 8 << 4;
	}
	return 1;

	goo++;
}

int Hoo()
{
}

#endif