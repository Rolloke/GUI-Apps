/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CipcOutputAkuUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CipcOutputAkuUnit.cpp $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 27.02.04 9:02 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mmsystem.h"
#include "CAku.h"
#include "AkuUnitDlg.h"
#include "CIPCOutputAkuUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"

CIPCOutputAkuUnit::CIPCOutputAkuUnit(CAkuUnitDlg* pMainWnd, CAku *pAku,
							 LPCTSTR shmName)
	: CIPCOutput(shmName, FALSE)
{
//	TRACE("CIPCOutputAkuUnit::CIPCOutputAkuUnit\n");
	m_pAku				= pAku;				// Pointer auf Aku-Objekt
	m_wGroupID			= SECID_NO_GROUPID;		// Noch keine GroupID
	m_bOK				= TRUE;					// TRUE->Objekt ok
	m_dwHardwareState	= m_pAku->GetHardwareState();	// Hardwarekonfiguration
	m_pMainWnd			= pMainWnd;

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputAkuUnit::~CIPCOutputAkuUnit()
{
	// TRACE("CIPCOutputAkuUnit::~CIPCOutputAkuUnit\n");
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputAkuUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputAkuUnit::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID, 1);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputAkuUnit::OnRequestReset(WORD wGroupID)	
{
//	TRACE("CIPCOutputAkuUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCOutputAkuUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputAkuUnit::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
	 WK_TRACE( _T("CIPCOutputAkuUnit::OnRequestSetRelay\tGroupID=%i\tNr=%i\t%i\n"),
		   relayID.GetGroupID(), relayID.GetSubID(), (int)bClosed);

 	if (relayID.GetGroupID() != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCOutputAkuUnit::OnRequestSetRelay\tWrong wGroupID\n"));

		DoIndicateError(0, relayID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DWORD	dwOldState	= 0L;
	DWORD	dwNewState	= 0L;;
	int		iNr			= relayID.GetSubID();
	
	// Bisherigen Relaisstatus holen.
	dwOldState = GetAllRelaisState();

	if (bClosed)
		dwNewState = SETBIT(dwOldState, iNr); // Relais ein
	else
		dwNewState = CLRBIT(dwOldState, iNr); // Relais aus
	
	// Relais setzen
	SetAllRelaisState(dwNewState);

	// Und Server mitteilen.
	DoConfirmSetRelay(relayID, bClosed);
}


//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputAkuUnit::OnRequestCurrentState(WORD wGroupID)
{
//	TRACE(_T("CIPCOutputAkuUnit::OnRequestCurrentState\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCOutputAkuUnit::OnRequestCurrentState\tWrong wGroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmCurrentState(m_wGroupID, GetAllRelaisState());
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputAkuUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputAkuUnit::OnRequestHardware(WORD wGroupID)
{
//	TRACE(_T("CIPCOutputAkuUnit::OnRequestHardware\tGroupID=%i\n"), wGroupID);

	int iErrorCode = 0;

	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCOutputAkuUnit::OnRequestHardware\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pAku && m_pAku->CheckHardware())
		iErrorCode = 0;
	else
	{
		 WK_TRACE( _T("ERROR:\tCIPCOutputAkuUnit::OnRequestHardware\tCheckHardware failed\n"));
		iErrorCode = 1;
	}

	DoConfirmHardware(m_wGroupID, iErrorCode,
									FALSE, /* No SW-Compress */
									FALSE, /* No SW-DeCompress */
									FALSE, /* No Co-Compress */
									FALSE, /* No Co-DeCompress */
									FALSE);/* No Overlay */
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCOutputAkuUnit::GetAllRelaisState()
{
	DWORD dwBit			= 0L;
	DWORD dwStateMask	= 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwStateMask |= (DWORD)m_pAku->GetRelais(AKU_CARD0);
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwStateMask |= (DWORD)m_pAku->GetRelais(AKU_CARD1)<<8;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwStateMask |= (DWORD)m_pAku->GetRelais(AKU_CARD2)<<16;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwStateMask |= (DWORD)m_pAku->GetRelais(AKU_CARD3)<<24;
	}

	return dwStateMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputAkuUnit::SetAllRelaisState(DWORD dwStateMask)
{
	DWORD dwBit = 0;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetRelais(AKU_CARD0, (BYTE)(dwStateMask>>0  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetRelais(AKU_CARD1, (BYTE)(dwStateMask>>8  & 0x000000ff));
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetRelais(AKU_CARD2, (BYTE)(dwStateMask>>16 & 0x000000ff));
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetRelais(AKU_CARD3, (BYTE)(dwStateMask>>24 & 0x000000ff));
	}
}

// OOPS HEDU only performs StopMpegEncoding
void CIPCOutputAkuUnit::OnRequestDisconnect()
{
}

