/* GlobalReplace: pApp\-\> --> theApp. */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputServer.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcOutputServer.cpp $
// CHECKIN:		$Date: 25.08.06 22:06 $
// VERSION:		$Revision: 166 $
// LAST CHANGE:	$Modtime: 25.08.06 21:55 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "CipcOutputServer.h"
#include "sec3.h"

#include "CipcServerControlServerSide.h"
#include "CipcOutputServerClient.h"
#include "ProcessScheduler.h"
#include "ProcessSchedulerVideo.h"
#include "ProcessSchedulerTasha.h"
#include "Quad.h"

#include "InputList.h"	// to restart processes
#include "CipcInputServer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
// FPS options, new 14.21.99, 0 as default
// not 0 is for testing, else the registry is read
int CIPCOutputServer::m_iMicoFPS = 16;	// also set in CSec3App::Reset!
int CIPCOutputServer::m_iSaVicFPS = 12;	// also set in CSec3App::Reset!
int CIPCOutputServer::m_iCocoFPS = 12;	// also set in CSec3App::Reset!

/////////////////////////////////////////////////////////////////////////////
// CIPCOutputServer
/////////////////////////////////////////////////////////////////////////////
/*@TOPIC{CIPCOutputServer Overview|CIPCOutputServer,Overview}
@RELATED @LINK{members|CIPCOutputServer},
*/
/*
@MLIST @RELATED @LINK{CIPCOutputServerClient}
*/

/*@MHEAD{constructor:}*/
/*@MD NYD */
CIPCOutputServer::CIPCOutputServer(LPCTSTR pName, int iSize, LPCTSTR pSMName)
	: CIPCOutput(pSMName, TRUE)
	, CIOGroup(pName)

{
	m_bIsMiCo  = FALSE;
	m_bIsJaCob = FALSE;
	m_bIsSaVic = FALSE;
	m_bIsTasha = FALSE;
	m_bIsQ = FALSE;
	m_bIsIP = FALSE;

	m_bIsDummy = TRUE;
	m_iNrOfMDDetectors = 0;

	m_bHasCameras = FALSE;
	m_dwBoardNr = 0;

	// OLD the following values are set in OnConfirmHardware
	// NEW 0603997 calc from ShmName 
	m_bCanSWCompress = FALSE;
	m_bCanBWDecompress = FALSE;
	m_bCanColorCompress = FALSE;
	m_bCanColorDecompress = FALSE;
	m_bCanOverlay = FALSE;


	// picture data
	m_compressionType = COMPRESSION_UNKNOWN;
	m_iFramesPerSecond = 0;
	
	// hardcoded via shared memory names
	CString sShm(pSMName);
	{
		if (sShm==SM_MICO_OUTPUT_CAMERAS 
					|| sShm==SM_MICO_OUTPUT_CAMERAS2
					|| sShm==SM_MICO_OUTPUT_CAMERAS3
					|| sShm==SM_MICO_OUTPUT_CAMERAS4
					) 
		{
			CWK_Dongle dongle;
			if (dongle.RunJaCobUnit1())
			{
				m_bIsJaCob = TRUE;
			}
			else
			{
				m_bIsMiCo = TRUE;
			}
			m_compressionType = COMPRESSION_JPEG;
			m_bHasCameras = TRUE;
			
			if (sShm==SM_MICO_OUTPUT_CAMERAS)
			{
				m_dwBoardNr = 0;
				m_sAppname = WK_APP_NAME_MICOUNIT;
			}
			else if (sShm==SM_MICO_OUTPUT_CAMERAS2)
			{
				m_dwBoardNr = 1;
				m_sAppname = WK_APP_NAME_MICOUNIT2;
			}
			else if (sShm==SM_MICO_OUTPUT_CAMERAS3)
			{
				m_dwBoardNr = 2;
				m_sAppname = WK_APP_NAME_MICOUNIT3;
			}
			else if (sShm==SM_MICO_OUTPUT_CAMERAS4)
			{
				m_dwBoardNr = 3;
				m_sAppname = WK_APP_NAME_MICOUNIT4;
			}

			if (m_iMicoFPS==0) 
			{	// read value from registry
				CWK_Profile wkp;
				CString sFPS(_T("MiCoFPS1"));
				if (sShm==SM_MICO_OUTPUT_CAMERAS) {
					sFPS=_T("MiCoFPS1");
				} else if (sShm==SM_MICO_OUTPUT_CAMERAS2) {
					sFPS=_T("MiCoFPS2");
				} else if (sShm==SM_MICO_OUTPUT_CAMERAS3) {
					sFPS=_T("MiCoFPS3");
				} else if (sShm==SM_MICO_OUTPUT_CAMERAS4) {
					sFPS=_T("MiCoFPS4");
				} else {
					// internal error
				}
				m_iFramesPerSecond	= wkp.GetInt(_T("SecurityServer"),sFPS,25);
			} 
			else 
			{
				// use explicit value (for testing)
				m_iFramesPerSecond = m_iMicoFPS;	// static member set via ServerDebug.cfg
			}
			m_bCanColorCompress = TRUE;
		} 
		else if (   sShm==SM_SAVIC_OUTPUT_CAMERAS 
				 || sShm==SM_SAVIC_OUTPUT_CAMERAS2
				 || sShm==SM_SAVIC_OUTPUT_CAMERAS3
				 || sShm==SM_SAVIC_OUTPUT_CAMERAS4
				) 
		{
			m_bIsSaVic = TRUE;
			m_compressionType = COMPRESSION_YUV_422;
			m_bHasCameras = TRUE;
			
			if (sShm==SM_SAVIC_OUTPUT_CAMERAS)
			{
				m_dwBoardNr = 0;
				m_sAppname = WK_APP_NAME_SAVICUNIT1;
			}
			else if (sShm==SM_SAVIC_OUTPUT_CAMERAS2)
			{
				m_dwBoardNr = 1;
				m_sAppname = WK_APP_NAME_SAVICUNIT2;
			}
			else if (sShm==SM_SAVIC_OUTPUT_CAMERAS3)
			{
				m_dwBoardNr = 2;
				m_sAppname = WK_APP_NAME_SAVICUNIT3;
			}
			else if (sShm==SM_SAVIC_OUTPUT_CAMERAS4)
			{
				m_dwBoardNr = 3;
				m_sAppname = WK_APP_NAME_SAVICUNIT4;
			}

			if (m_iSaVicFPS==0) 
			{	// read value from registry
				CWK_Profile wkp;
				CString sFPS(_T("SaVicFPS1"));
				if (sShm==SM_SAVIC_OUTPUT_CAMERAS) {
					sFPS=_T("SaVicFPS1");
				} else if (sShm==SM_SAVIC_OUTPUT_CAMERAS2) {
					sFPS=_T("SaVicFPS2");
				} else if (sShm==SM_SAVIC_OUTPUT_CAMERAS3) {
					sFPS=_T("SaVicFPS3");
				} else if (sShm==SM_SAVIC_OUTPUT_CAMERAS4) {
					sFPS=_T("SaVicFPS4");
				} else {
					// internal error
				}
				m_iFramesPerSecond	= wkp.GetInt(_T("SecurityServer"),sFPS,25);
			} 
			else 
			{
				// use explicit value (for testing)
				m_iFramesPerSecond = m_iSaVicFPS;	// static member set via ServerDebug.cfg
			}
			m_bCanColorCompress = TRUE;
		}
		else if (   sShm==SM_TASHA_OUTPUT_CAMERAS 
				|| sShm==SM_TASHA_OUTPUT_CAMERAS2
				|| sShm==SM_TASHA_OUTPUT_CAMERAS3
				|| sShm==SM_TASHA_OUTPUT_CAMERAS4
			) 
		{
			m_bIsTasha = TRUE;
			m_compressionType = COMPRESSION_MPEG4;
			m_bHasCameras = TRUE;

			if (sShm==SM_TASHA_OUTPUT_CAMERAS)
			{
				m_dwBoardNr = 0;
				m_sAppname = WK_APP_NAME_TASHAUNIT1;
			}
			else if (sShm==SM_TASHA_OUTPUT_CAMERAS2)
			{
				m_dwBoardNr = 1;
				m_sAppname = WK_APP_NAME_TASHAUNIT2;
			}
			else if (sShm==SM_TASHA_OUTPUT_CAMERAS3)
			{
				m_dwBoardNr = 2;
				m_sAppname = WK_APP_NAME_TASHAUNIT3;
			}
			else if (sShm==SM_TASHA_OUTPUT_CAMERAS4)
			{
				m_dwBoardNr = 3;
				m_sAppname = WK_APP_NAME_TASHAUNIT4;
			}

			m_bCanColorCompress = TRUE;
		}
		else if (sShm==SM_Q_OUTPUT_CAMERA)
		{
			m_dwBoardNr = 0;
			m_bIsQ = TRUE;
			m_compressionType = COMPRESSION_YUV_422;
			m_bHasCameras = TRUE;
			m_sAppname = WK_APP_NAME_QUNIT;
			m_bCanColorCompress = TRUE;
		}
		else if (sShm==SM_IP_CAMERA_OUTPUT_CAMERA)
		{
			m_dwBoardNr = 0;
			m_bIsIP = TRUE;
			m_compressionType = COMPRESSION_JPEG;
			m_bHasCameras = TRUE;
			m_sAppname = WK_APP_NAME_IP_CAMERA_UNIT;
			m_bCanColorCompress = TRUE;
		}

		// tasha relais
		if (sShm==SM_TASHA_OUTPUT_RELAYS)
		{
			m_dwBoardNr = 0;
			m_bIsTasha = TRUE;
			m_sAppname = WK_APP_NAME_TASHAUNIT1;
		}
		else if (sShm==SM_TASHA_OUTPUT_RELAYS2)
		{
			m_dwBoardNr = 1;
			m_bIsTasha = TRUE;
			m_sAppname = WK_APP_NAME_TASHAUNIT2;
		}
		else if (sShm==SM_TASHA_OUTPUT_RELAYS3)
		{
			m_dwBoardNr = 2;
			m_bIsTasha = TRUE;
			m_sAppname = WK_APP_NAME_TASHAUNIT3;
		}
		else if (sShm==SM_TASHA_OUTPUT_RELAYS4)
		{
			m_dwBoardNr = 3;
			m_bIsTasha = TRUE;
			m_sAppname = WK_APP_NAME_TASHAUNIT4;
		}
		// SAVIC
		else if (sShm==SM_SAVIC_OUTPUT_RELAYS)
		{
			m_dwBoardNr = 0;
			m_bIsSaVic = TRUE;
			m_sAppname = WK_APP_NAME_SAVICUNIT1;
		}
		else if (sShm==SM_SAVIC_OUTPUT_RELAYS2)
		{
			m_dwBoardNr = 1;
			m_bIsSaVic = TRUE;
			m_sAppname = WK_APP_NAME_SAVICUNIT2;
		}
		else if (sShm==SM_SAVIC_OUTPUT_RELAYS3)
		{
			m_dwBoardNr = 2;
			m_bIsSaVic = TRUE;
			m_sAppname = WK_APP_NAME_SAVICUNIT3;
		}
		else if (sShm==SM_SAVIC_OUTPUT_RELAYS4)
		{
			m_dwBoardNr = 3;
			m_bIsSaVic = TRUE;
			m_sAppname = WK_APP_NAME_SAVICUNIT4;
		}
		// JACOB
		else if (sShm==SM_MICO_OUTPUT_RELAYS)
		{
			m_dwBoardNr = 0;
			m_bIsJaCob = TRUE;
			m_sAppname = WK_APP_NAME_MICOUNIT;
		}
		else if (sShm==SM_MICO_OUTPUT_RELAYS2)
		{
			m_dwBoardNr = 1;
			m_bIsJaCob = TRUE;
			m_sAppname = WK_APP_NAME_MICOUNIT2;
		}
		else if (sShm==SM_MICO_OUTPUT_RELAYS3)
		{
			m_dwBoardNr = 2;
			m_bIsJaCob = TRUE;
			m_sAppname = WK_APP_NAME_MICOUNIT3;
		}
		else if (sShm==SM_MICO_OUTPUT_RELAYS4)
		{
			m_dwBoardNr = 3;
			m_bIsJaCob = TRUE;
			m_sAppname = WK_APP_NAME_MICOUNIT4;
		}
		else if (sShm == SM_Q_OUTPUT_RELAY)
		{
			m_dwBoardNr = 0;
			m_bIsQ = TRUE;
			m_sAppname = WK_APP_NAME_QUNIT;
		}
		else if (sShm == SM_IP_CAMERA_OUTPUT_RELAY)
		{
			m_dwBoardNr = 0;
			m_bIsQ = TRUE;
			m_sAppname = WK_APP_NAME_IP_CAMERA_UNIT;
		}
	}

	m_iHardware = -1;
	m_pQuad = NULL;
	m_bIsDummy = sShm==_T("OutputDummyShm");
	m_pProcessScheduler = NULL;

	if (   m_bHasCameras 
		&& (IsTasha() || IsQ() || IsIP())
		)
	{
		m_pProcessScheduler = new CProcessScheduler_Tasha_Q_IP(this,iSize);
	}
	else
	{
		m_pProcessScheduler = new CProcessSchedulerVideo(this);
	}
	Sleep(0);
}

/*@MHEAD{destructor:}*/
/*@MD NYD */
void CIPCOutputServer::SetShutdownInProgress()
{
	CIOGroup::SetShutdownInProgress();	// call super class!
	m_pProcessScheduler->SetShutdownInProgress();
}

/*@MD NYD */
CIPCOutputServer::~CIPCOutputServer()
{
	WK_DELETE(m_pQuad);
	StopThread();
	WK_DELETE(m_pProcessScheduler);
	DeleteAllOutputs();
	WK_TRACE(_T("CIPCOutputServer::~CIPCOutputServer %s\n"),GetShmName());
}
/////////////////////////////////////////////////////
BOOL CIPCOutputServer::Start()
{
	if (m_bIsDummy)
	{
		if (m_pQuad)
		{
			m_pQuad->Create();
		}
	}
	else
	{
		StartThread();
	}
	return TRUE;
}
/////////////////////////////////////////////////////
void CIPCOutputServer::Load(CWK_Profile& wkp, int iNr)
{
	if (m_bIsDummy)
	{
		// any quad to control ?
		QuadControlType qct = (QuadControlType)wkp.GetInt(_T("Quad"),_T("Type"),QCT_UNKNOWN);
		if (qct!= QCT_UNKNOWN)
		{
			int iCom = wkp.GetInt(_T("Quad"),_T("COM"),0);
			m_pQuad = new CQuad(iCom,qct);
			m_pQuad->Load(wkp);
		}
	}
	else
	{
		CString sOutputGroupFolder;
		sOutputGroupFolder.Format(_T("%s\\Group%x"),_T("OutputGroups"),GetGroupID());
		COutput* pOutput = NULL;
		// loop over all outputs for this group
		for (int i=0;i<iNr;i++) 
		{
			pOutput = NULL;
			CString sSubString = wkp.GetString(sOutputGroupFolder, i+1, _T(""));
			if (sSubString.GetLength()) 
			{	
				CString sType;
				sType = CWK_Profile::GetStringFromString(sSubString, _T("\\TY"),_T("") );
				pOutput = new COutput(this,(WORD)i,TypeName2TypeNr(sType));
				if (pOutput->GetOutputType()==OUTPUT_CAMERA)
				{
					BOOL bSN = (BYTE)CWK_Profile::GetNrFromString(sSubString, _T("\\SN"), FALSE);
					BOOL bRef = (BYTE)CWK_Profile::GetNrFromString(sSubString, INI_REFERENCE, FALSE);
					BOOL bTerm = (BYTE)CWK_Profile::GetNrFromString(sSubString, INI_TERMINATION, TRUE);
					CSecID idArchive = (DWORD)CWK_Profile::GetHexFromString(sSubString,"\\AR",SECID_NO_ID);

					pOutput->SetFixed(!bSN);
					pOutput->SetReference(bRef);
					pOutput->SetTermination(bTerm);
					if (idArchive!=SECID_NO_ID)
					{
						pOutput->SetCurrentArchiveID(idArchive);
					}

					if (bSN)
					{
						CameraControlCmd ccc = (CameraControlCmd)CWK_Profile::GetNrFromString(sSubString,_T("\\SNCmd"),CCC_INVALID);
						int iPTZInterface = CWK_Profile::GetNrFromString(sSubString,_T("\\I"),0);
						pOutput->SetPTZ(ccc,iPTZInterface);
					}
				} 
				else if (pOutput->GetOutputType()==OUTPUT_RELAY) 
				{
					pOutput->ReadRelayAttributesFromRegistryString(sSubString);
				}
				CString sTmp = CWK_Profile::GetStringFromString(sSubString, _T("\\CO"), _T(""));
				if (sTmp.GetLength()) 
				{
					pOutput->m_sName=sTmp;
				}
				AddOutput(pOutput);
			} 
			else 
			{
				WK_TRACE_ERROR(_T("%s:no output record for %d\n"),(LPCTSTR)GetName(),i);
			}
		}	// end of loop over each output
	}
}
/////////////////////////////////////////////////////
void CIPCOutputServer::Save(CWK_Profile& wkp)
{
	if (HasCameras())
	{
		CString sSection,sKey,sValue,sNewValue;

		sSection.Format(_T("OutputGroups\\Group%04hx"),GetGroupID());
		for (int i=0;i<GetSize();i++)
		{
			COutput* pOutput = GetOutputAtWritable(i);
			sKey.Format(_T("%d"),i+1);

			sValue = wkp.GetString(sSection,sKey,_T(""));
			
			// alten AR wert löschen
			int p = sValue.Find(_T("\\AR"));
			if (p!=-1)
			{
				CString l,r;
				l = sValue.Left(p);
				r = sValue.Mid(p+11);
				sValue = l+r;
			}


			sNewValue.Format(_T("%s\\AR%08lx"),sValue,pOutput->GetCurrentArchiveID());
			wkp.WriteString(sSection,sKey,sNewValue);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
CProcessScheduler* CIPCOutputServer::GetProcessScheduler() const
{
	return m_pProcessScheduler;
}
/////////////////////////////////////////////////////////////////////////////////////////
CProcessSchedulerVideoBase*	CIPCOutputServer::GetProcessSchedulerVideo() const
{
	return m_pProcessScheduler;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::IsMiCo() const
{
	return m_bIsMiCo;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::IsJaCob() const
{
	return m_bIsJaCob;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::IsSaVic() const
{
	return m_bIsSaVic;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::IsTasha() const
{
	return m_bIsTasha;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::IsQ() const
{
	return m_bIsQ;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::IsIP() const
{
	return m_bIsIP;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::HasCameras() const
{
	return m_bHasCameras;
}
/////////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputServer::GetAppname() const
{
	return m_sAppname;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServer::IsMDActive(CSecID id)
{
	if (IsJaCob())
	{
		CString sSection;
		sSection.Format(_T("JaCobUnit\\Device%d\\MotionDetection"),m_dwBoardNr+1);
		CWK_Profile wkp;
		return wkp.GetInt(sSection,_T("Activate"),FALSE);
	}
	else if (IsSaVic())
	{
		CString sSection;
		sSection.Format(_T("SaVicUnit\\Device%d\\MotionDetection"),m_dwBoardNr+1);
		CWK_Profile wkp;
		return wkp.GetInt(sSection,_T("Activate"),FALSE);
	}
	else if (IsTasha())
	{
		return TRUE; // ML 07.06.04 // Tasha macht immer MD
/*		CString sSection;
		sSection.Format(_T("TashaUnit\\Device%d\\MotionDetection"),m_dwBoardNr+1);
		CWK_Profile wkp;
		return wkp.GetInt(sSection,_T("Activate"),FALSE);
*/	}
	else if (IsQ())
	{
		return TRUE; // ML 12.07.06 // Q macht immer MD
	}
	else
	{
		return FALSE;
	}
}

// s:		Output-type als string [camera, isdn, relay]
// RETURN:	Output-type.
SecOutputType CIPCOutputServer::TypeName2TypeNr(LPCTSTR sz)
{
	if (_tcsicmp(sz, _T("relay"))==0) {
		return OUTPUT_RELAY;
	} else if (_tcsicmp(sz, _T("audio"))==0) {
	 	return OUTPUT_AUDIO;
	} else if (_tcsicmp(sz, _T("camera"))==0) {
		return OUTPUT_CAMERA;
	} else if (_tcsicmp(sz, _T("off"))==0) {
		return OUTPUT_OFF;
	} else {
		TRACE(_T("TypeName2TypeNr:Invalid OutputType:%s!\n"), sz);
		return OUTPUT_OFF;
	}
}

// iTypeNr:	TypeNummer [P_CAMERA usw ]
LPCTSTR CIPCOutputServer::TypeNr2TypeName(int iTypeNr)
{
	if (iTypeNr==OUTPUT_CAMERA)	
		return _T("camera");
	else if (iTypeNr==OUTPUT_RELAY)	
		return _T("relay");
	else if (iTypeNr==OUTPUT_AUDIO)	
		return _T("audio");
	else if (iTypeNr==OUTPUT_OFF)	
		return _T("off");
	else 
		return _T("invalid output");
}
////////////////////////////////////////////////////////////////
void CIPCOutputServer::DoActivityVideoOut(CSecID id, int iPort)
{
	if (m_bIsDummy)
	{
		if (m_pQuad)
		{
			int iVOUT = m_pQuad->GetVideoOut(id,iPort);
			GetProcessSchedulerVideo()->DoActivityVideoOut(CSecID(SECID_GROUP_OUTPUT,(WORD)iVOUT));
		}
	}
	else
	{
		GetProcessSchedulerVideo()->DoActivityVideoOut(id);
	}
}
////////////////////////////////////////////////////////////////
void CIPCOutputServer::SwitchVideoOut(CSecID id, int iPort)
{
	if (CProcessScheduler::m_bTraceVideoout)
	{
		WK_TRACE(_T("SwitchVideoOut %s %d->%d\n"),GetShmName(),id.GetSubID(),iPort);
	}
	if (m_bIsDummy)
	{
		if (m_pQuad)
		{
			if (id == SECID_NO_ID)
			{
				m_pQuad->SwitchQuad();
			}
			else
			{
				m_pQuad->SwitchCamSingle(id.GetSubID());
			}
		}
	}
	else
	{
		CString sValue;
		switch (iPort)
		{
		case 1:
			sValue = _T("PORT0");
			break;
		case 0:
			sValue = _T("PORT1");
			break;
		case 2:
			sValue = _T("PORT2");
			break;
		case 3:
			sValue = _T("PORT3");
			break;
		}
		DoRequestSetValue(id,CSD_V_OUT,sValue,0 /*server data*/);
	}
}

/*@MHEAD{connection:}*/
/*@MLIST The initial connection is made in the following order:*/
/*@MD NYD */
void CIPCOutputServer::OnReadChannelFound()
{
	Sleep(0);
	DoRequestConnection();
}


/*@MD NYD */
void CIPCOutputServer::OnConfirmConnection()
{
	WK_TRACE(_T("CIPCOutputServer::OnConfirmConnection %s\n"),GetShmName());
	CString sMsg;
	sMsg.Format(_T("OutputUnit|%s"),GetShmName());
	WK_STAT_LOG(_T("Units"),1,sMsg);
	CIPC::OnConfirmConnection();

	DoRequestVersionInfo(GetGroupID());
	DoRequestSetGroupID(m_wGroupID);
	theApp.UpdateStateInfo();
}

/*@MDT{optional} NYD */
void CIPCOutputServer::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	Sleep(0);
	m_dwVersion = dwVersion;
}

/*@MD NYD */
void CIPCOutputServer::OnConfirmSetGroupID(WORD wGroupID)
{
	Sleep(0);
	for (int i=0;i<GetSize();i++) 
	{
		GetOutputAtWritable(i)->m_bRelayClosed = FALSE;
	}
	DoRequestHardware(m_wGroupID);
}
/*@MD NYD */
void CIPCOutputServer::OnConfirmHardware(WORD wGroupID,
								int iErrorCode,
								BOOL bCanSWCompress,
								BOOL bCanBWDecompress,
								BOOL bCanColorCompress,
								BOOL bCanColorDecompress,
								BOOL bCanOverlay)
{
	WK_TRACE(_T("OnConfirmHardware %s %d\n"),GetShmName(),iErrorCode);
	ASSERT(wGroupID==m_wGroupID);
	m_CS.Lock();
	m_iHardware = iErrorCode;
	// picture data
	m_bCanSWCompress = bCanSWCompress;
	m_bCanBWDecompress = bCanBWDecompress;
	m_bCanColorCompress = bCanColorCompress;
	m_bCanColorDecompress = bCanColorDecompress;
	m_bCanOverlay = bCanOverlay;
	m_CS.Unlock();

	if (m_iHardware==0) 
	{
		DoRequestReset(m_wGroupID);
	}
	
	// CLIENT LOOP
	// client is informed but the following OnRequestInfoOutputs
	// might be denied during server reset
	// BUT the server forces a DoConfirmInfoOutputs for all clients
	// once the reset is done
	CIPCOutputClientsArray &clients=theApp.GetOutputClients();
	clients.Lock(_T(__FUNCTION__));
	for (int ci=0;ci<clients.GetSize();ci++) {
			clients[ci]->DoConfirmHardware(
									wGroupID, 
									iErrorCode,
									bCanSWCompress,
									bCanBWDecompress,
									bCanColorCompress,
									bCanColorDecompress,
									bCanOverlay
									);
	}
	clients.Unlock();
}

/*@MD NYD */
void CIPCOutputServer::OnConfirmReset(WORD wGroupID)
{
	WK_TRACE(_T("CIPCOutputServer::OnConfirmReset %s\n"),GetShmName());
	m_CS.Lock();
	m_dwState = 0L;
	m_CS.Unlock();

	CString s = GetShmName();
	s.Replace(_T("OutputCamera"),_T("MDInput"));
	CIPCInputServer* pMDGroup = theApp.GetInputGroups().GetGroupByShmName(s);

	if (   pMDGroup
		&& HasCameras()
		&& (   IsSaVic() 
		    || IsJaCob()
			|| IsTasha())
		)
	{
		m_iNrOfMDDetectors = 0;
		for (int i=0;i<GetSize();i++)
		{
			const COutput *pOutput = GetOutputAt(i);
			if (pOutput->GetOutputType() == OUTPUT_CAMERA)
			{
				// camera ist an, ist der zugehörige MD Melder auch an?
				const CInput* pInput = pMDGroup->GetInputAt(i);
				if (pInput && pInput->IsMD())
				{
					if (pInput->GetIDActivate() == SECID_ACTIVE_ON)
					{
						WK_TRACE(_T("setting MD ON for %s %08lx\n"),pOutput->GetName(),pOutput->GetID().GetID());
						DoRequestSetValue(pOutput->GetID(),CSD_MD,CSD_ON);
						m_iNrOfMDDetectors++;
					}
					else
					{
						WK_TRACE(_T("setting MD OFF for %s %08lx\n"),pOutput->GetName(),pOutput->GetID().GetID());
						DoRequestSetValue(pOutput->GetID(),CSD_MD,CSD_OFF);
					}
				}

				DoRequestSetValue(pOutput->GetID(),CSD_TERM,pOutput->IsTerminated() ? CSD_ON : CSD_OFF);
			}
		}
	}

	m_pProcessScheduler->OnUnitConfirmReset();

	DoRequestCurrentState(m_wGroupID);

	// restart possible lost processes in case unit came back
	for (int i=0;i<theApp.GetInputGroups().GetSize();i++) 
	{
		CIPCInputServer *pInputGroup = theApp.GetInputGroups().GetGroupAt(i);
		if (pInputGroup->IsInitDone()) 
		{
			WK_TRACE(_T("StartMissingProcesses %s %s\n"),GetShmName(),pInputGroup->GetShmName());
			pInputGroup->StartMissingProcesses(m_wGroupID, NULL);	// no timer
		}
	}
}
/*@MHEAD{disconnect:}*/
/*@MD NYD */
void CIPCOutputServer::OnRequestDisconnect()
{
	CString sMsg;
	sMsg.Format(_T("OutputUnit|%s"),GetShmName());
	WK_STAT_LOG(_T("Units"),0,sMsg);
	m_iHardware = -1;

	// CLIENT LOOP
	CIPCOutputClientsArray &clients=theApp.GetOutputClients();
	clients.Lock(_T(__FUNCTION__));
	for (int ci=0;ci<clients.GetSize();ci++) 
	{
		clients[ci]->DoConfirmHardware(m_wGroupID, -1,FALSE, FALSE, FALSE, FALSE, FALSE);
	}
	clients.Unlock();
	theApp.UpdateStateInfo();
	m_pProcessScheduler->OnUnitDisconnect();
}

/*@MHEAD{relay:}*/
/*@MD NYD */
void CIPCOutputServer::OnConfirmSetRelay(CSecID relayID, BOOL bClosed)
{
	if (m_bShutdownInProgress) {
		return;	// EXIT!
	}

	WORD ix = relayID.GetSubID();
	if (ix < 0 || ix >= GetSize()) {
		WK_TRACE_ERROR(_T("SetRelay invalid id %x\n"),relayID.GetID());
		return;	// EXIT
	}

	m_CS.Lock();
	// OOPS m_dwState ?
	if (bClosed) {
		m_dwState |= (1L<<relayID.GetSubID());
	} else {
		m_dwState &= ~(1L<<relayID.GetSubID());
	}
	COutput *pOutput = GetOutputAtWritable(ix);
	pOutput->m_bRelayClosed = bClosed;
	m_CS.Unlock();

	// CLIENT LOOP
	CIPCOutputClientsArray &clients=theApp.GetOutputClients();
	clients.Lock(_T(__FUNCTION__));
	for (int ci=0;ci<clients.GetSize();ci++) {
		if (clients[ci]->GetOptions() & SCO_WANT_RELAYS) {
			clients[ci]->DoConfirmSetRelay(relayID, bClosed);
		}
	}
	clients.Unlock();
		
}

/*@MHEAD{state:}*/
/*@MD NYD */
void CIPCOutputServer::OnConfirmCurrentState(WORD wGroupID, DWORD stateMask)
{
	m_CS.Lock();
	m_dwState = stateMask;	// Bitmask-Status der Ausgänge
	m_CS.Unlock();
			
}

/*--------------------------@MHEAD{jpeg:}*/
/*@MD NYD */
void CIPCOutputServer::	OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserData,CSecID)
{
	if (!m_bShutdownInProgress) 
	{
		m_pProcessScheduler->OnIndicateVideo(pict,0,0,dwUserData);
	}
}
/*@MD NYD */
void CIPCOutputServer::OnConfirmJpegDecoding(WORD wGroupID,
									WORD wXSize, WORD wYSize, 
									DWORD dwUserData,
									const CIPCPictureRecord &pict
								)
{
	if (!m_bShutdownInProgress) 
	{
		// CLIENT LOOP
		CIPCOutputClientsArray &clients=theApp.GetOutputClients();
		clients.Lock(_T(__FUNCTION__));
		for (int i=0;i<clients.GetSize();i++) {
			if (clients[i]->GetOptions() & SCO_WANT_DECOMPRESS_BW) {
				clients[i]->DoConfirmJpegDecoding(
										wGroupID, 
										wXSize, wYSize,
										dwUserData,
										pict
										);
			}
		}
		clients.Unlock();
	}
}

/*--------------------------@MHEAD{local video (bitmap):}*/
/*@MD NYD */
void CIPCOutputServer::OnIndicateLocalVideoData(
										CSecID camID,
										WORD wXSize, WORD wYSize, 
										const CIPCPictureRecord &pict)
{
	if (!m_bShutdownInProgress) 
	{
		// NOT YET as process
		// CLIENT LOOP
		CIPCOutputClientsArray &clients=theApp.GetOutputClients();
		clients.Lock(_T(__FUNCTION__));
		for (int ci=0;ci<clients.GetSize();ci++) {
			if (clients[ci]->GetOptions() & SCO_WANT_CAMERAS_BW) {
					clients[ci]->DoIndicateLocalVideoData(
											camID, 
											wXSize, wYSize,
											pict
										);
			}
		}
		clients.Unlock();
	}
}

/*--------------------------@MHEAD{mpeg:}*/
/*@MD NYD */
void CIPCOutputServer::OnIndicateH263Data(const CIPCPictureRecord &pict, DWORD dwJobData,CSecID)
{
	if (!m_bShutdownInProgress) 
	{
		m_pProcessScheduler->OnIndicateVideo(pict, 0, 0, dwJobData);	// OOPS always ?
	}
}

/*@MD NYD */
void CIPCOutputServer::OnConfirmStopH263Encoding(CSecID camID)
{
	// NOT YET clients
}
/*@MD NYD */
void CIPCOutputServer::OnConfirmH263Decoding(WORD wGroupID, DWORD dwUserData)
{
	if (theApp.GetServerControl()==NULL) {
		return;	// EXIT
	}
	if (m_bShutdownInProgress) {
		return;	// EXIT!
	}
	CClientLinkArray & usage = theApp.GetServerControl()->m_usageH263Decoding;
	usage.Lock(_T(__FUNCTION__));
	for (int i=0;i<usage.GetSize();i++) {
		if (usage[i]->m_dwID==dwUserData) {
			usage[i]->m_pOutputClient->DoConfirmH263Decoding(
				wGroupID, usage[i]->m_dwUserData
				);
			usage.DropAndSwap(i);
			usage.Unlock();
			return;		// EXIT
		}
	}
	usage.Unlock();
	// none found, client already has disconnected
}
/*--------------------------@MHEAD{bitmap from overlay:}*/
/*@MD NYD */
void CIPCOutputServer::OnConfirmDumpOutputWindow(
				WORD wGroupID,
				WORD wUserID,
				int iFormat,
				const CIPCExtraMemory &bitmapData
				)
{
	if (theApp.GetServerControl()==NULL) {
		return;	// EXIT
	}

	CClientLinkArray & usage = theApp.GetServerControl()->m_usageH263Decoding;
	usage.Lock(_T(__FUNCTION__));
	for (int i=0;i<usage.GetSize();i++) {
		if (usage[i]->m_dwID==wUserID) {
			usage[i]->m_pOutputClient->DoConfirmDumpOutputWindow(
				wGroupID,
				(WORD) usage[i]->m_dwUserData,
				iFormat,
				bitmapData
				);
			
			usage.DropAndSwap(i);
			usage.Unlock();
			return;		// EXIT
		}
	}
	usage.Unlock();
	// none found, client already has disconnected
}


/*--------------------------@MHEAD{CIPC value:}*/
/*@MD NYD */
void CIPCOutputServer::OnConfirmGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	CSecID clientID(dwServerData);
	CIPCOutputClientsArray &clients=theApp.GetOutputClients();
	clients.Lock(_T(__FUNCTION__));
	CIPCOutputServerClient *pClient=NULL;
	pClient=clients.GetClientByID(clientID);
	if (pClient) {
		pClient->DoConfirmGetValue(id,sKey,sValue,0);
	} else {
		// client already disappeared
		WK_TRACE_ERROR(_T("OnConfirmGetValue(%s,%s) client %x not found\n"),
			sKey,sValue,clientID.GetID());
	}
	clients.Unlock();
}
////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputServer::OnConfirmSetValue(CSecID id, // might be used as group ID only
										const CString &sKey,
										const CString &sValue,
										DWORD dwServerData
										)
{
	if (dwServerData!=0)
	{
		CSecID clientID(dwServerData);
		CIPCOutputClientsArray &clients=theApp.GetOutputClients();
		clients.Lock(_T(__FUNCTION__));
		CIPCOutputServerClient *pClient=NULL;
		pClient=clients.GetClientByID(clientID);
		if (pClient) 
		{
			pClient->DoConfirmSetValue(id,sKey,sValue,0);
		} 
		else 
		{
			// client already disappeared
			WK_TRACE(_T("OnConfirmSetValue(%s,%s) client %x not found\n"),
				sKey,sValue,clientID.GetID());
		}
		clients.Unlock();
	}
}
/*@MD NYD */
void CIPCOutputServer::OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	CSecID clientID(dwUserID);
	CIPCOutputClientsArray &clients=theApp.GetOutputClients();
	clients.Lock(_T(__FUNCTION__));
	CIPCOutputServerClient *pClient=NULL;
	pClient=clients.GetClientByID(clientID);
	if (pClient) 
	{
		pClient->DoConfirmGetMask(camID,0,mask);
	} 
	else 
	{
		// client already disappeared
		WK_TRACE_ERROR(_T("OnConfirmGetMask %08lx Client no longer connected\n"),camID.GetID());
	}
	clients.Unlock();
}
/*@MD NYD */
void CIPCOutputServer::OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	CSecID clientID(dwUserID);
	CIPCOutputClientsArray &clients=theApp.GetOutputClients();
	clients.Lock(_T(__FUNCTION__));
	CIPCOutputServerClient *pClient=NULL;
	pClient=clients.GetClientByID(clientID);
	if (pClient) 
	{
		pClient->DoConfirmSetMask(camID,type,0);
	} 
	else 
	{
		// client already disappeared
		WK_TRACE_ERROR(_T("OnConfirmSetMask %08lx Client no longer connected\n"),camID.GetID());
	}
	clients.Unlock();
}

/*@MD NYD */
void CIPCOutputServer::OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend, 
				   DWORD dwType,DWORD dwUserID)
{
	// NOT YET DROPPPED
}

////////////////////////////////////
/*--------------------------@MHEAD{unsorted:}*/
/*@MD NYD */
void CIPCOutputServer::OnConfirmStartH263Encoding(CSecID camID)
{
	// NOT YET
}

/*@MD NYD */
void CIPCOutputServer::OnConfirmSetOutputWindow (WORD wGroupID, int iPictureResult)
{
	// NOT YET
}

/*@MD NYD */
void CIPCOutputServer::OnConfirmSetDisplayWindow(WORD wGroupID)
{
	// NOT YET
}

/*@MD NYD */
void CIPCOutputServer::OnConfirmSetUp(WORD wGroupID)
{
	// OOPS
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputServer::OnIndicateError(DWORD dwCmd, 
									 CSecID id, 
									 CIPCError error, 
									 int iErrorCode,
									 const CString &sErrorMessage)
{
	if (dwCmd == CIPC_BASE_CONFIRM_ALIVE)
	{
		WK_TRACE(_T("resetting by unconfirmed alive (CIPCOutputServer %s)\n") ,GetShmName());
		theApp.EmergencyReset();
	}
	else if (error == CIPC_ERROR_UNHANDLED_CMD)
	{
		WK_TRACE(_T("UNIT %s:unhandled cmd %s\n"),GetShmName(),NameOfCmd(dwCmd));
	}
	else if (error == CIPC_ERROR_CAMERA_NOT_PRESENT)
	{
		m_pProcessScheduler->OnVideoSignal(id,iErrorCode);
		// id has camera id
		if (iErrorCode == 0)
		{
			// camera signal loss
			// pause processes
		}
		else if (iErrorCode == 1)
		{
			// camera signal present
			// restart processes
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputServer::OnIndicateVideo(const CIPCPictureRecord& pict,
										DWORD dwMDX,
										DWORD dwMDY,
										DWORD dwUserData,
										CSecID)
{
	if (   !m_bShutdownInProgress
		&& theApp.IsUpAndRunning())
	{
		/*
		TRACE(_T("OnIndicateVideo %08lx %d %s,%03d %d, %d\n"),
			pict.GetCamID().GetID(),pict.GetBlockNr(),
			pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().GetMilliseconds(),
			SPT2TCHAR(pict.GetPictureType()),
			dwUserData);*/
		m_pProcessScheduler->OnIndicateVideo(pict,dwMDX,dwMDY,dwUserData);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputServer::OnRequestSetValue(CSecID id,
										const CString &sKey,
										const CString &sValue,
										DWORD dwServerData)
{
	WK_TRACE(_T("reset by PC-Reset ->Tasha/SaVic/JaCob\n"));
	theApp.DoReset();
}
