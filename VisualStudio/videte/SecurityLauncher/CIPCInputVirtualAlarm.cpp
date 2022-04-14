// CIPCInputVirtualAlarm.cpp: implementation of the CIPCInputVirtualAlarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCInputVirtualAlarm.h"

#ifndef SETBIT 
#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#endif

#include "SecurityLauncher.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CIPCInputVirtualAlarm::CIPCInputVirtualAlarm() 
: CIPCInput(SM_LAUNCHER_INPUT,FALSE)
{
	m_wGroupID		= SECID_NO_GROUPID;
	m_dwAlarmState	= 0;
	m_dwEdgeMask	= 0;
	m_nErrorCounter	= 0;
	StartThread();
}

//////////////////////////////////////////////////////////////////////
CIPCInputVirtualAlarm::~CIPCInputVirtualAlarm()
{
	StopThread();
}

//////////////////////////////////////////////////////////////////////
BOOL CIPCInputVirtualAlarm::OnErrorArrived(CWK_RunTimeError *pError)
{
	CString sMsg;	  
	
	if (!pError) 
	{
		return FALSE;
	}

   // CWK_RunTimeError
	DWORD	dwNewState		= m_dwAlarmState;
	BOOL	bShowMsgBox		= FALSE;	// TRUE -> Zeige RTE in Messagebox
	BOOL	bVirtualAlarm	= TRUE;		// TRUE -> RTE ist ein virtueller Alarm
	int		nBit			= 0;		// Bitmaske der Alarme
	DWORD	dwParam1		= pError->GetParam1();
	DWORD	dwParam2		= pError->GetParam2();
	BOOL    bTextToFields	= FALSE;
	CString sErrorText = pError->GetErrorText();
	CIPCFields fields;

	switch (pError->GetError())
	{
		case RTE_ARCHIVE_60PERCENT:
			nBit = BIT_VALARM_ARCHIVE_60PERCENT_FULL;
#ifdef _DEBUG
			bTextToFields = TRUE;
#endif
			break;
		case RTE_ARCHIVE_FULL:
			nBit = BIT_VALARM_ARCHIVE_FULL;
#ifdef _DEBUG
			bTextToFields = TRUE;
#endif
			break;
		case RTE_CAMERA_MISSING:
			// Kameraausfaelle muessen auf jeden Fall angezeigt werden
			bShowMsgBox = TRUE;
			nBit = BIT_VALARM_KAMERAMISSING;
			m_nErrorCounter = (dwParam2 ? m_nErrorCounter + 1: m_nErrorCounter-1);
#ifdef _DEBUG
			bTextToFields = TRUE;
#endif
			break;
		case RTE_ISDN_BCHANNEL:
			// Kein echter Alarm, nur Info
			bVirtualAlarm = FALSE;
			break;
		case RTE_ARCHIVE_FUEL:
			// Kein echter Alarm, nur Info
			bVirtualAlarm = FALSE;
			break;
		case RTE_TIME_CHANGED:
			// Kein echter Alarm, nur Info
			bVirtualAlarm = FALSE;
			break;
		case RTE_OUTGOING_CALL:
			switch (pError->GetApplicationId())
			{
				case WAI_ISDN_UNIT:
					nBit = BIT_VALARM_OUTGOING_CALL_ISDN1;
					break;
				case WAI_ISDN_UNIT_2:
					nBit = BIT_VALARM_OUTGOING_CALL_ISDN2;
					break;
				case WAI_PRESENCE_UNIT:
					nBit = BIT_VALARM_OUTGOING_CALL_PT1;
					break;
				case WAI_PRESENCE_UNIT_2:
					nBit = BIT_VALARM_OUTGOING_CALL_PT2;
					break;
				case WAI_SOCKET_UNIT:
					nBit = BIT_VALARM_OUTGOING_CALL_TCP;
					break;
				default:
					WK_TRACE_ERROR(_T("Unknown ApplicationID\n"));
			}
			bTextToFields = FALSE;
			break;
		case RTE_INCOMING_CALL:
			switch (pError->GetApplicationId())
			{
				case WAI_ISDN_UNIT:
					nBit = BIT_VALARM_INCOMING_CALL_ISDN1;
					break;
				case WAI_ISDN_UNIT_2:
					nBit = BIT_VALARM_INCOMING_CALL_ISDN2;
					break;
				case WAI_PRESENCE_UNIT:
					nBit = BIT_VALARM_INCOMING_CALL_PT1;
					break;
				case WAI_PRESENCE_UNIT_2:
					nBit = BIT_VALARM_INCOMING_CALL_PT2;
					break;
				case WAI_SOCKET_UNIT:
					nBit = BIT_VALARM_INCOMING_CALL_TCP;
					break;
				default:
					WK_TRACE_ERROR(_T("Unknown ApplicationID\n"));
			}
			bTextToFields = TRUE;
			break;
		case RTE_REJECT:
			switch (pError->GetApplicationId())
			{
				case WAI_ISDN_UNIT:
					nBit = BIT_VALARM_REJECT_ISDN1;
					break;
				case WAI_ISDN_UNIT_2:
					nBit = BIT_VALARM_REJECT_ISDN2;
					break;
				case WAI_PRESENCE_UNIT:
					nBit = BIT_VALARM_REJECT_PT1;
					break;
				case WAI_PRESENCE_UNIT_2:
					nBit = BIT_VALARM_REJECT_PT2;
					break;
				case WAI_SOCKET_UNIT:
					nBit = BIT_VALARM_REJECT_TCP;
					break;
				default:
					WK_TRACE_ERROR(_T("Unknown ApplicationID\n"));
			}
			bTextToFields = TRUE;
			break;
		case RTE_CHECK_CALL_ERROR:
			nBit = BIT_VALARM_CHECK_CALL_ERROR;
			bTextToFields = TRUE;
			break;
		case RTE_ALWAYS_ALARM:
			nBit = BIT_VALARM_ALWAYS_ALARM;
			break;
		case RTE_CHECK_CALL_OK:
			nBit = BIT_VALARM_CHECK_CALL_OK;
			{
				CStringArray sa;
				SplitString(sErrorText,sa,_T(','));
				for (int i=0;i<sa.GetSize();i++)
				{
					fields.Add(new CIPCField(CIPCField::m_sfStNm,sa.GetAt(i),_T('C')));
				}
			}
			break;
		case RTE_ALARM_OFF_SPAN:
			nBit = BIT_VALARM_ALARM_OFF_SPAN;
			bTextToFields = TRUE;
			break;
		case RTE_EXTERNAL_DEVICE_FAILURE:
			nBit          = BIT_VALARM_EXTERNAL_DEVICE_FAILURE;
			bShowMsgBox   = dwParam2 & 2 ? TRUE : FALSE;
			bTextToFields = dwParam2 & 4 ? TRUE : FALSE;
			bVirtualAlarm = dwParam2 & 8 ? FALSE : TRUE;
			break;
		case RTE_TEMPERATURE:
			nBit          = BIT_VALARM_TEMPERATURE;
			if (dwParam2 && dwParam1)
			{
				CString sDevice;
				signed short nDevice = HIWORD(dwParam1);
				switch (nDevice)
				{
					case DEVICE_HARDDISK:		sDevice.LoadString(IDS_DEVICE_HARDDISK); break;
					case DEVICE_CPU:			sDevice.LoadString(IDS_DEVICE_CPU); break;
					case DEVICE_POWER_SUPPLY:	sDevice.LoadString(IDS_DEVICE_POWER_SUPPLY); break;
					case DEVICE_HW_BOARD:		sDevice.LoadString(IDS_DEVICE_HW_BOARD); break;
					case DEVICE_CASE:			sDevice.LoadString(IDS_DEVICE_CASE); break;
					case DEVICE_SYSTEM:			sDevice.LoadString(IDS_DEVICE_SYSTEM); break;
					case DEVICE_STRING:
						sDevice = sErrorText; 
						sErrorText.Empty();
						break;
					case DEVICE_UNKNOWN:
					default:
						sDevice.LoadString(IDS_DEVICE_UNKNOWN); 
						break;
				}
				if (!sErrorText.IsEmpty())
				{
					sDevice += sErrorText;
				}
				sErrorText.Format(IDS_TEMPERATUR_ALARM, LOWORD(dwParam1), HIWORD(dwParam2), sDevice);
				pError->SetErrorText(sErrorText);
				bShowMsgBox   = dwParam2 & 2 ? TRUE : FALSE;
				bTextToFields = TRUE;//dwParam2 & 4 ? TRUE : FALSE;
				bVirtualAlarm = dwParam2 & 8 ? FALSE : TRUE;

				DWORD dwTemperatureBit = 1 << nDevice;
				theApp.m_dwRTE_TemperatureBits |= dwTemperatureBit;
			}
			break;
		case RTE_FANCONTROL:
			nBit          = BIT_VALARM_FAN_SPEED;
			if (dwParam2 && dwParam1)
			{
				CString sDevice;
				signed short nDevice = HIWORD(dwParam1);
				switch (nDevice)
				{
					case DEVICE_HARDDISK:		sDevice.LoadString(IDS_DEVICE_HARDDISK); break;
					case DEVICE_CPU:			sDevice.LoadString(IDS_DEVICE_CPU); break;
					case DEVICE_POWER_SUPPLY:	sDevice.LoadString(IDS_DEVICE_POWER_SUPPLY); break;
					case DEVICE_HW_BOARD:		sDevice.LoadString(IDS_DEVICE_HW_BOARD); break;
					case DEVICE_STRING:
						sDevice = sErrorText; 
						sErrorText.Empty();
						break;
					case DEVICE_UNKNOWN:
					default:
						sDevice.LoadString(IDS_DEVICE_UNKNOWN); 
						break;
				}
				if (!sErrorText.IsEmpty())
				{
					sDevice += sErrorText;
				}
				sErrorText.Format(IDS_FAN_SPEED_ALARM, LOWORD(dwParam1), HIWORD(dwParam2), sDevice);
				pError->SetErrorText(sErrorText);
				bShowMsgBox   = dwParam2 & 2 ? TRUE : FALSE;
				bTextToFields = TRUE;//dwParam2 & 4 ? TRUE : FALSE;
				bVirtualAlarm = dwParam2 & 8 ? FALSE : TRUE;

				DWORD dwFanBit = 1 << nDevice;
				theApp.m_dwRTE_FanBits |= dwFanBit;
			}	
			break;
		case RTE_SMART:
			nBit          = BIT_VALARM_SMART_NOT_OK;
			bShowMsgBox   = dwParam2 & 2 ? TRUE : FALSE;
			bTextToFields = TRUE;//dwParam2 & 4 ? TRUE : FALSE;
			bVirtualAlarm = dwParam2 & 8 ? FALSE : TRUE;
			break;
		case RTE_HARDDISK_FALURE:
			nBit          = BIT_VALARM_HARDDISK_FAILURE;
			bShowMsgBox   = dwParam2 & 2 ? TRUE : FALSE;
			bTextToFields = TRUE;//dwParam2 & 4 ? TRUE : FALSE;
			bVirtualAlarm = dwParam2 & 8 ? FALSE : TRUE;
			break;
		default:
			// Kein virtueller Alarm, muss auf jeden Fall angezeigt werden
			bVirtualAlarm = FALSE;
			bShowMsgBox = TRUE;
			break;
	}	
	
	// Virtueller Alarm?
	if (bVirtualAlarm)
	{
		WK_TRACE(_T("VirtualAlarm arriving: %s, %d, %d\n"), 
			(LPCTSTR)sErrorText, dwParam1, dwParam2);

		if (dwParam2 & 1)
		{
			dwNewState = SETBIT(dwNewState, nBit);		// Alarm
		}
		else
		{
			// Bei Kamerausfall-Fehlern, soll der virtuelle Alarm erst wieder weg,
			// wenn alle Kamerasignale wieder zur Verfügung stehen.
			switch (pError->GetError())
			{
				case RTE_CAMERA_MISSING:
				if (m_nErrorCounter == 0)
				{
					dwNewState = CLRBIT(dwNewState, nBit);	// Alarm aus
				}	break;
				case RTE_TEMPERATURE:
				{
					signed short nDevice = HIWORD(dwParam1);
					DWORD dwTemperatureBit = 1 << nDevice;
					if (nDevice == DEVICE_HARDDISK)			// HDD Temperature
					{
						BOOL bHarddiskTemp = FALSE;			// is check 
						for (int i=0; i<theApp.m_nMaxCheckedDrives; i++)
						{
							if (theApp.m_pnLastTemperatures[i] == VIRTUAL_ALARM_SET)
							{
								bHarddiskTemp = TRUE;
								break;
							}
						}
						if (bHarddiskTemp == FALSE)
						{
							theApp.m_dwRTE_TemperatureBits &= ~dwTemperatureBit;
						}
					}
					else
					{
						theApp.m_dwRTE_TemperatureBits &= ~dwTemperatureBit;
					}
					if (theApp.m_dwRTE_TemperatureBits == 0)
					{
						dwNewState = CLRBIT(dwNewState, nBit);	// Alarm aus
					}
				}break;
				case RTE_FANCONTROL:
				{
					signed short nDevice = HIWORD(dwParam1);
					DWORD dwFanBit = 1 << nDevice;
					theApp.m_dwRTE_FanBits &= ~dwFanBit;
					if (theApp.m_dwRTE_FanBits == 0)
					{
						dwNewState = CLRBIT(dwNewState, nBit);	// Alarm aus
					}
				}break;

				default:
					dwNewState = CLRBIT(dwNewState, nBit);	// Alarm aus
					break;
			}
		}

		if (bTextToFields)
		{
			CStringArray sa;
			SplitString(pError->GetErrorText(),sa,_T(','));
			for (int i=0;i<sa.GetSize();i++)
			{
				fields.Add(new CIPCField(CIPCField::m_sfStNm,sa.GetAt(i),_T('C')));
			}
		}

		if (GetIPCState() == CIPC_STATE_CONNECTED)
		{
			if (fields.GetSize()>0)
			{
				DoIndicateAlarmFieldsState(m_wGroupID, 
					dwNewState^m_dwEdgeMask, 
					(dwNewState^m_dwEdgeMask)^(m_dwAlarmState^m_dwEdgeMask),
					fields);
			}
			else
			{
				DoIndicateAlarmState(m_wGroupID, dwNewState^m_dwEdgeMask, (dwNewState^m_dwEdgeMask)^(m_dwAlarmState^m_dwEdgeMask));
			}
		}

		m_dwAlarmState = dwNewState;
	}


	m_nErrorCounter = max(m_nErrorCounter, 0);

	if (m_nErrorCounter > 0) 
	{
		theApp.GetWatchDog().PiezoSetFlag(PF_NO_VIDEOSIGNAL);
	}
	else 
	{
		theApp.GetWatchDog().PiezoClearFlag(PF_NO_VIDEOSIGNAL);
	}
	
	return bShowMsgBox;
}

//////////////////////////////////////////////////////////////////////
void CIPCInputVirtualAlarm::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputVirtualAlarm::OnRequestHardware(WORD wGroupID)
{
	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputVirtualAlarm::OnRequestReset(WORD wGroupID)	
{
	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputVirtualAlarm::OnRequestAlarmState(WORD wGroupID)
{
	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState^m_dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputVirtualAlarm::OnRequestSetEdge(WORD wGroupID,DWORD dwEdgeMask)	// 1 positive, 0 negative
{
	m_dwEdgeMask = dwEdgeMask;
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputVirtualAlarm::OnRequestSetFree(WORD wGroupID,DWORD dwOpenMask)	// 1 open, 0 closed
{
	DoConfirmFree(m_wGroupID, dwOpenMask);
}
//////////////////////////////////////////////////////////////////////
void CIPCInputVirtualAlarm::OnIndicateError(DWORD dwCmd, 
											 CSecID id, 
											 CIPCError error, 
											 int iErrorCode,
											 const CString &sErrorMessage)
{
	if (dwCmd == CIPC_BASE_CONFIRM_ALIVE)
	{
		WK_TRACE(_T("deleting virtual alarms by no confirm alive\n"));
		DelayedDelete();
	}
}

