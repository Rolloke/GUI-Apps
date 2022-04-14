// ControlRecordTOA.cpp: implementation of the CControlRecordTOA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommUnit.h"
#include "ControlRecordTOA.h"

//////////////////////////////////////////////////////////////////////
#define DEBUG_TRACE	WK_TRACE

//////////////////////////////////////////////////////////////////////
#define TOA_MODIFIER_CODE	0x08
#define TOA_ADD_CODE			TOA_MODIFIER_CODE + 0x01
#define TOA_SEND_ERROR_MAX	2

#define TOA_ACK				(BYTE)0x41
#define TOA_NAK				(BYTE)0x42
#define TOA_STATUS_RESPONSE	(BYTE)0x43
#define TOA_STATUS_REQUEST	(BYTE)0x81

//////////////////////////////////////////////////////////////////////
CControlRecordTOA::CControlRecordTOA(int iCom, BOOL bTraceHex)
	: CControlRecord(CCT_TOA, iCom, FALSE, bTraceHex, FALSE)
{
	m_eStatus = READY;
	m_bySendErrorCounter = 0;
	RefreshTimeOut(200);
#ifdef _DEBUG
	m_bTelemetrySet = FALSE;
#endif
}
//////////////////////////////////////////////////////////////////////
CControlRecordTOA::~CControlRecordTOA()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordTOA::Create()
{
	WK_TRACE(_T("ControlRecordTOA Create COM=%d\n"), GetCOMNumber());
	DWORD	dwBaudRate	= CBR_38400;
	BYTE	byDataBits	= 8;
	BYTE	byParity	= EVENPARITY;
	BYTE	byStopBits	= ONESTOPBIT;
	ReadPortValues(dwBaudRate, byDataBits, byParity, byStopBits);
	return Open(dwBaudRate, byDataBits, byParity, byStopBits);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordTOA::Add(CSecID secID, DWORD dwCamID)
{
	if (CheckForValidTOAIDs(dwCamID))
	{
		CControlRecord::Add(secID, dwCamID);
		m_Flags.Add(0);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::OnNewCommand(CCameraCommandRecord* pCCommandR)
{
	if (theApp.m_bTraceCommand)
	{
		WK_TRACE(_T("OnNewCommand %s, %s, %lx, %d\n"),
				 NameOfEnum(GetType()),
				 NameOfEnum(pCCommandR->GetCommand()),
				 pCCommandR->GetCamID().GetID(),
				 pCCommandR->GetValue());
	}

	BYTE byHeaderOnes=0, byHundredsTens=0;
	int iCam = 0;
	if (GetTOAIDs(pCCommandR->GetCamID(), byHeaderOnes, byHundredsTens, iCam))
	{
		CameraControlCmd cmd = pCCommandR->GetCommand();
		DWORD dwValue = pCCommandR->GetValue();

		if (cmd == CCC_CONFIG)
		{
			FillMenuCmd(byHeaderOnes, byHundredsTens, dwValue);
		}
		else
		{
			FillCameraCmd(byHeaderOnes, byHundredsTens, cmd, dwValue, iCam);
		}

		m_csSendCommand.Lock();
		if (m_eStatus == READY)
		{
			TryToSendCommand(FALSE);
		}
		m_csSendCommand.Unlock();
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordTOA::GetTOAIDs(CSecID idCamSec, BYTE& byHeaderOnes, BYTE& byHundredsTens, int& iCam)
{
	BOOL bFound = FALSE;
	BOOL bValid = FALSE;
	for (int i=0 ; i<m_secIDs.GetSize() ; i++)
	{
		if (idCamSec == m_secIDs[i])
		{
			bFound = TRUE;
			iCam = i;
			// found Camera SecID, now check for valid system IDs
			DWORD dwCamID = m_camIDs[i];
			bValid = CheckForValidTOAIDs(dwCamID);
			if (bValid)
			{
				BYTE byHundreds = (BYTE)(dwCamID / 100); // Camera ID 100's place
				dwCamID -= (byHundreds * 100);
				BYTE byTens = (BYTE)(dwCamID / 10); // Camera ID 10's place
				dwCamID -= (byTens * 10);
				BYTE byOnes = (BYTE)(dwCamID); // Camera ID 1's place

				byHeaderOnes = (BYTE)(0xf0 + byOnes);
				byHundredsTens = (BYTE)((byHundreds<<4) + byTens);
			}
			break;
		}
	}
	if (!bFound)
	{
		WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; Invalid SecID %08x\n"),
			GetCOMNumber(), idCamSec);
	}
	return bValid;
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordTOA::CheckForValidTOAIDs(DWORD dwCamID)
{
	BOOL bValid = FALSE;
	if ( (1 <= dwCamID) && (dwCamID <= 999))
	{
		bValid = TRUE;
	}
	else
	{
		WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; Invalid Camera ID %d\n"),
			GetCOMNumber(), dwCamID);
	}
	return bValid;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::FillMenuCmd(BYTE byHeaderOnes, BYTE byHundredsTens, DWORD dwValue)
{
	// First command always only once
	CCommandRecord* pNewCommand = FillHeader(byHeaderOnes, byHundredsTens);
	pNewCommand->m_bRepeatAllowed = FALSE;

	// Byte 3 Menu command
	BYTE byte = GetMenuCommand(dwValue, FALSE);
	pNewCommand->Add(byte);	// Menu command
	// Byte 4 - 6 Control command and data, here 0 for no control data
	pNewCommand->Add(0x00);	// Control command and data
	pNewCommand->Add(0x00);	// Control command and data
	pNewCommand->Add(0x00);	// Control command and data
	FillParity(pNewCommand);

	// Now check for repeatable parts
	byte =GetMenuCommand(dwValue, TRUE);
	if (byte != 0x00)
	{
		CCommandRecord* pNewCommand = FillHeader(byHeaderOnes, byHundredsTens);
		pNewCommand->m_bRepeatAllowed = TRUE;

		// Byte 3 Menu command
		pNewCommand->Add(byte);	// Menu command
		// Byte 4 - 6 Control command and data, here 0 for no control data
		pNewCommand->Add(0x00);	// Control command and data
		pNewCommand->Add(0x00);	// Control command and data
		pNewCommand->Add(0x00);	// Control command and data
		FillParity(pNewCommand);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BYTE CControlRecordTOA::GetMenuCommand(DWORD dwValue, BOOL bRepeat)
{
	// TODO gf not yet implemented
//	m_byLastMenuCmd
	BYTE byte = 0x00; // signals also 'no repeat state'
	switch (dwValue)
	{
	case PTZF_CONFIG_PANASONIC_ON:		// Menu on
		byte = 0x08;
		break;
	case PTZF_CONFIG_PANASONIC_OFF:		// Menu off
		byte = 0x00;
		break;
	case PTZF_CONFIG_PANASONIC_SET1:	// Enter
		byte = bRepeat ? 0x08 : 0x18;
		break;
	case PTZF_CONFIG_PANASONIC_SET2:	// Clear
		byte = bRepeat ? 0x08 : 0x28;
		break;
	case PTZF_CONFIG_PANASONIC_UP:		// Up
	case PTZF_CONFIG_PANASONIC_RIGHT:
		byte = bRepeat ? 0x08 : 0x48;
		break;
	case PTZF_CONFIG_PANASONIC_DOWN:	// Up
	case PTZF_CONFIG_PANASONIC_LEFT:
		byte = bRepeat ? 0x08 : 0x88;
		break;
	default:
		break;
	}
	return byte;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::FillCameraCmd(BYTE byHeaderOnes, BYTE byHundredsTens, CameraControlCmd cmd, DWORD dwValue, int iCam)
{
	CCommandRecord* pNewCommand = FillHeader(byHeaderOnes, byHundredsTens);
	BYTE byMenu = 0; // GetMenuCommand(dwValue, FALSE);
	pNewCommand->Add(byMenu);
	
	switch (cmd)
	{
		case CCC_PAN_LEFT:
		case CCC_PAN_RIGHT:
		case CCC_PAN_STOP:
		case CCC_TILT_UP:
		case CCC_TILT_DOWN:
		case CCC_TILT_STOP:
		case CCC_ZOOM_IN:
		case CCC_ZOOM_OUT:
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_NEAR:
		case CCC_FOCUS_FAR:
		case CCC_FOCUS_STOP:
		case CCC_FOCUS_AUTO:
		case CCC_IRIS_OPEN:
		case CCC_IRIS_CLOSE:
		case CCC_IRIS_STOP:
		case CCC_IRIS_AUTO:
		case CCC_MOVE_LEFT_UP:
		case CCC_MOVE_RIGHT_UP:
		case CCC_MOVE_LEFT_DOWN:
		case CCC_MOVE_RIGHT_DOWN:
		case CCC_MOVE_STOP:
			FillTelemetryCmd(pNewCommand, cmd, dwValue);
			break;
		case CCC_POS_1:
		case CCC_POS_2:
		case CCC_POS_3:
		case CCC_POS_4:
		case CCC_POS_5:
		case CCC_POS_6:
		case CCC_POS_7:
		case CCC_POS_8:
		case CCC_POS_9:
		case CCC_POS_HOME:
		case CCC_AUXOUT:
			FillPresetCmd(pNewCommand, cmd, dwValue, iCam);
			break;
		default:
			WK_TRACE_ERROR(_T("ControlRecordTOA COM=%d ; Camera Cmd UNHANDLED %d\n"),
				GetCOMNumber(), cmd);
			break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
CCommandRecord* CControlRecordTOA::FillHeader(BYTE byHeaderOnes, BYTE byHundredsTens)
{
	CCommandRecord* pNewCommand = new CCommandRecord;
	// Byte 1 Header and Camera number unit's place
	pNewCommand->Add(byHeaderOnes);
	// Byte 2 Camera number hundred's and ten's place
	pNewCommand->Add(byHundredsTens);
	return pNewCommand;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::FillTelemetryCmd(CCommandRecord* pNewCommand, CameraControlCmd cmd, DWORD dwValue)
{
	BYTE byte4 = 0x02; // Mode 2 Manual control
	BYTE byte5 = 0x00; // Camera control command
	BYTE byte6 = 0x00; // Speed command

	// Speed has four different 'relativ' values 0 - 3
	DWORD wSpeedX = min(10, LOWORD(dwValue));	// 1 - 10
	DWORD wSpeedY = min(10, HIWORD(dwValue));	// 1 - 10
	if (!wSpeedY) wSpeedY = wSpeedX;
	BYTE bySpeedX = (BYTE)(((3.0 * (float)wSpeedX) / 10.0) + 0.3);	// 0 - 3
	BYTE bySpeedY = (BYTE)(((3.0 * (float)wSpeedY) / 10.0) + 0.3);	// 0 - 3;; 

	switch (cmd)
	{
		// Iris: not implemented
		case CCC_IRIS_OPEN:
		case CCC_IRIS_CLOSE:
			break;
		// Focus
		case CCC_FOCUS_FAR:		byte5 = 0x40;	byte6 = bySpeedX<<6;	break;
		case CCC_FOCUS_NEAR:	byte5 = 0x80;	byte6 = bySpeedX<<6;	break;
		case CCC_FOCUS_AUTO:	byte5 = 0xc0;	byte6 = bySpeedX<<6;	break;
		// Zoom
		case CCC_ZOOM_IN:		byte5 = 0x10;	byte6 = bySpeedX<<4;	break;
		case CCC_ZOOM_OUT:		byte5 = 0x20;	byte6 = bySpeedX<<4;	break;
		// Pan
		case CCC_PAN_RIGHT:		byte5 = 0x04;	byte6 = bySpeedX<<2;	break;
		case CCC_PAN_LEFT:		byte5 = 0x08;	byte6 = bySpeedX<<2;	break;
		// Tilt
		case CCC_TILT_UP:		byte5 = 0x01;	byte6 = bySpeedY;	break;
		case CCC_TILT_DOWN:		byte5 = 0x02;	byte6 = bySpeedY;	break;
		// Move
		case CCC_MOVE_LEFT_UP:		byte5 = 0x08 | 0x01; break;
		case CCC_MOVE_RIGHT_UP:		byte5 = 0x04 | 0x01; break;
		case CCC_MOVE_LEFT_DOWN:	byte5 = 0x08 | 0x02; break;
		case CCC_MOVE_RIGHT_DOWN:	byte5 = 0x04 | 0x02; break;

		case CCC_PAN_STOP: case CCC_TILT_STOP: case CCC_ZOOM_STOP: case CCC_FOCUS_STOP: case CCC_MOVE_STOP: case CCC_IRIS_STOP:
			break;	// stop cmds arrive, but do not do anything here
		default:
			byte5 = 0x00;	byte6 = 0x00;
			WK_TRACE_ERROR(_T("ControlRecordTOA COM=%d ; Camera Cmd UNHANDLED %d\n"),
				GetCOMNumber(), cmd);
			break;
	}

	// STOP commands can not be repeated and allways set speed to lowest value then
	BOOL bRepeat = TRUE;
	switch (cmd)
	{
	case CCC_PAN_STOP:
	case CCC_TILT_STOP:
	case CCC_ZOOM_STOP:
	case CCC_FOCUS_STOP:
	case CCC_MOVE_STOP:
	case CCC_IRIS_STOP:
		bRepeat = FALSE;
		byte5 = 0x00;	byte6 = 0x00;
		break;
	case CCC_MOVE_LEFT_UP:
	case CCC_MOVE_RIGHT_UP:
	case CCC_MOVE_LEFT_DOWN:
	case CCC_MOVE_RIGHT_DOWN:
		byte6 = bySpeedX<<2 | bySpeedY;
		break;
	}
	pNewCommand->m_bRepeatAllowed = bRepeat;

	DEBUG_TRACE(_T("ControlRecordTOA COM=%d FillCameraCmd %02x %02x %02x Speed %04x, %04x\n"),
		GetCOMNumber(), byte4, byte5, byte6, wSpeedX, wSpeedY);

	pNewCommand->Add(byte4);
	pNewCommand->Add(byte5);
	pNewCommand->Add(byte6);
	FillParity(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::FillPresetCmd(CCommandRecord* pNewCommand, CameraControlCmd cmd, DWORD dwValue, int iCam)
{
	// No command can be repeated
	pNewCommand->m_bRepeatAllowed = FALSE;

	BYTE byte4 = 0x01; // Mode 1 Preset / Reproduction Position
	BYTE byte5 = 0x00; // default no position
	BYTE byte6 = 0x00; // default no position

	if (cmd == CCC_AUXOUT)
	{
		byte4 = 0x04;	// Mode 4 auxiliary out
		byte5 = 0x0a;	// both outputs off
		DWORD dwAuxOutflags = 1 << (LOWORD(dwValue) -1);
		if (HIWORD(dwValue))
		{
			m_Flags[iCam] |= dwAuxOutflags;
		}
		else
		{
			m_Flags[iCam] &= ~dwAuxOutflags;
		}
		if (m_Flags[iCam] & 0x00000001)	// aux 1
		{
			byte5 &= ~0x00000008;		// on
			byte5 |=  0x00000004;
		}

		if (m_Flags[iCam] & 0x00000002)	// aux 2
		{
			byte5 &= ~0x00000002;		// on
			byte5 |=  0x00000001;
		}
		DEBUG_TRACE(_T("ControlRecordTOA COM=%d FillAuxOutCmd %02x %02x %02x Preset %i\n"),
			GetCOMNumber(), byte4, byte5, byte6, dwValue);
	}
	else
	{
		if (dwValue)
		{
			byte5 = 0x40;
		}

		BYTE byPosition = 0;

		switch (cmd)
		{
		case CCC_POS_1:
		case CCC_POS_2:
		case CCC_POS_3:
		case CCC_POS_4:
		case CCC_POS_5:
		case CCC_POS_6:
			byPosition = (BYTE)(cmd - CCC_POS_1 + 1);
			break;
		case CCC_POS_7:
		case CCC_POS_8:
		case CCC_POS_9:
			byPosition = (BYTE)(cmd - CCC_POS_7 + 7);
			break;
		case CCC_POS_HOME:
			byPosition = 0xFF;
			break;
		}

		byte5 |= (byPosition & 0x3F);
		byte6 |= ((byPosition & 0xC0) >> 6);

		DEBUG_TRACE(_T("ControlRecordTOA COM=%d FillPresetCmd %02x %02x %02x Preset %i\n"),
			GetCOMNumber(), byte4, byte5, byte6, dwValue);
	}
	pNewCommand->Add(byte4);
	pNewCommand->Add(byte5);
	pNewCommand->Add(byte6);
	FillParity(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::FillParity(CCommandRecord* pNewCommand)
{
	// check for parity and add to odd
	BYTE byte = 0;
	BYTE bParity = 0xff;
	for (int i=0 ; i<pNewCommand->GetSize() ; i++)
	{
		byte = pNewCommand->GetAtFast(i);
		bParity ^= byte;				// toggle bits from odd to even
										// zeor bits do not toggle
	}
	pNewCommand->Add(bParity);

	m_arrayCommand.SafeAdd(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::OnReceivedData(LPBYTE pBuffer,DWORD dwLen)
{
	DWORD	dw = 0;
	BYTE	byte;
	while (pBuffer && dw<dwLen)
	{
		if (m_byaReceived.GetSize() >= 60)
		{
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			// Fehler, zu viele Zeichen
			CString sMsg;
			WK_TRACE_ERROR(_T("ControlRecordTOA COM%i Daten Len %i Buffer-Overrun, no ETX\n"),
							GetCOMNumber(), m_byaReceived.GetSize());
			sMsg.Format( _T("Buffer-Overrun|COM%i"), GetCOMNumber() );
			WK_STAT_PEAK(_T("BufferOverrun"), 1, sMsg);
			m_byaReceived.RemoveAll();
		}

		byte = pBuffer[dw];
		// Wenn ein Startzeichen kommt, fangen die Daten erst richtig an
		if (byte == _STX_)
		{
			ClearAllData();
			m_byaReceived.Add(byte); // add for easier handling
		}
		// Wenn ein Endezeichen kommt, ist ein Datensatz komplett
		else if (byte == _ETX_)
		{
			m_byaReceived.Add(byte); // add for easier handling
			CheckData();
		}
		else
		{
			m_byaReceived.Add(byte); 
		}
		dw++;
	} // while
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::OnTimeOutTransparent()
{
	m_csSendCommand.Lock();
	if (m_eStatus == WAITING_FOR_ACK)
	{
		if (theApp.m_iTestResponse == 0)
		{
			OnNAK();
		}
		else if (theApp.m_iTestResponse == 1)
		{
			OnACK();
		}
		else
		{
			OnACK();

//			m_bySendErrorCounter++;
//			if (m_bySendErrorCounter >= TOA_SEND_ERROR_MAX)
//			{
//				WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; OnTimeOutTransparent OVERFLOW\n"),
//									GetCOMNumber());
//				// remove current command and try next one
//				CurrentCommandFinished(TRUE);
//			}
//			else
//			{
//				TryToSendCommand(TRUE);
//			}
		}
	}
	else
	{ // valid state if nothing to do
	}
	m_csSendCommand.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::CheckData()
{
/*	int iLen = m_byaReceived.GetSize();
	BYTE byte;
	// Check for valid destination (=our source) address
	if (iLen > 1)
	{
		byte = m_byaReceived.GetAtFast(1);
		if (byte == m_bySourceID)
		{
			if (CheckLengthAndCRC())
			{
				// Check for command
				if (iLen > 3)
				{
					byte = m_byaReceived.GetAtFast(3);
					if (byte == 0x0a)	// ACK/Status
					{
						// Check for ACK/NAK/Status
						if (iLen > 6)
						{
							byte = m_byaReceived.GetAtFast(6);
							if (byte == TOA_ACK)	// ACK
							{
								OnACK();
							}
							else if (byte == TOA_NAK)	// NAK
							{
								OnNAK();
							}
							else if (byte == TOA_STATUS_REQUEST)	// Status request
							{
								OnStatusRequest(m_byaReceived.GetAtFast(2));
							}
							else
							{
								WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d CheckData UNKNOWN\n"), GetCOMNumber());
							}
						}
						else
						{
							DataCorrupt(6);
						}
					}
				}
				else
				{
					DataCorrupt(3);
				}
			}
		}
		else
		{
#ifdef _DEBUG
			// wir fühlen uns immer angesprochen
			DecodeData();
#else
			WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; Data not for us %02x<>%02x\n"),
									GetCOMNumber(), byte, m_bySourceID);
#endif
		}
	}
	else
	{
		DataCorrupt(1);
	}
*/
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordTOA::CheckLengthAndParity()
{
	BOOL bReturn = FALSE;
/*	int iLen = m_byaReceived.GetSize();
	// Check for length
	if (iLen > 4)
	{
		BYTE byLenT = m_byaReceived.GetAtFast(4);
		BYTE byLenR = (BYTE)(iLen - 8);
		if (byLenT == byLenR)
		{
			// Check for CRC
			int iIndexLastCrcByte = byLenT + 6;
			if (iLen > iIndexLastCrcByte)
			{
				WORD wCRC_T = m_byaReceived.GetAtFast(iIndexLastCrcByte);
				wCRC_T = (BYTE)(wCRC_T << 8);
				wCRC_T = (WORD)(wCRC_T + m_byaReceived.GetAtFast(iIndexLastCrcByte-1));
				WORD wCRC_R = 0;
				int iLastDataIndex = iIndexLastCrcByte - 2;
				for (int i=2 ; i<=iLastDataIndex ; i++)
				{
					wCRC_R = (WORD)(wCRC_R + m_byaReceived.GetAtFast(i));
				}
				if (wCRC_R == wCRC_R)
				{
					bReturn = TRUE;
				}
				else
				{
					WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; Invalid CRC %04x<>%04x\n"),
											GetCOMNumber(), wCRC_R, wCRC_R);
				}
			}
			else
			{
				DataCorrupt(byLenR);
			}
		}
		else
		{
			WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; Length invalid %02x<>%02x\n"),
									GetCOMNumber(), byLenT, byLenR);
		}
	}
	else
	{
		DataCorrupt(4);
	}
*/	return bReturn;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::DataCorrupt(int iIndex)
{
	WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; Data corrupt for index %i\n"),
							GetCOMNumber(), iIndex);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::CurrentCommandFinished(BOOL bForceNextOne)
{
	DEBUG_TRACE(_T("ControlRecordTOA COM=%d CurrentCommandFinished\n"), GetCOMNumber());
	m_bySendErrorCounter = 0;
	m_csSendCommand.Lock();
	m_eStatus = READY;
	if (   bForceNextOne
		|| (   m_pCurrentCommand
			&& (m_pCurrentCommand->m_bRepeatAllowed == FALSE)
			)
		)
	{
		WK_DELETE(m_pCurrentCommand);
	}
	TryToSendCommand(FALSE);
	m_csSendCommand.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::OnACK()
{
	DEBUG_TRACE(_T("ControlRecordTOA COM=%d OnACK\n"), GetCOMNumber());
	CurrentCommandFinished(FALSE);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::OnNAK()
{
	DEBUG_TRACE(_T("ControlRecordTOA COM=%d OnNAK\n"), GetCOMNumber());
	m_csSendCommand.Lock();
	m_bySendErrorCounter++;
	if (m_bySendErrorCounter >= TOA_SEND_ERROR_MAX)
	{
		WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; OnNAK OVERFLOW\n"),
							GetCOMNumber());
		// remove current command and try next one
		CurrentCommandFinished(TRUE);
	}
	else
	{
		TryToSendCommand(TRUE);
	}
	m_csSendCommand.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordTOA::TryToSendCommand(BOOL bForceRepeat)
{
	if (bForceRepeat)
	{ // we MUST repeat, so try it
	}
	else
	{  
		// do we have a new command
		if (m_arrayCommand.GetSize())
		{
			WK_DELETE(m_pCurrentCommand);
			m_arrayCommand.Lock();
			m_pCurrentCommand = m_arrayCommand.GetAtFast(0);
			m_arrayCommand.RemoveAt(0);
			m_arrayCommand.Unlock();
		}
	}

	if (m_pCurrentCommand)
	{
		if (m_pCurrentCommand->GetSize())
		{
			int nSize = m_pCurrentCommand->GetSize();
			const BYTE *pData = m_pCurrentCommand->GetData();
#ifdef _DEBUG
			TRACE(_T("Write(:"));
			for (int i=0; i<nSize; i++)
			{
				TRACE(_T("%02x:"), pData[i]);
			}
			TRACE(_T(")\n"));
#endif
			Write((void*)pData, nSize);
			m_eStatus = WAITING_FOR_ACK;
		}
		else
		{
			WK_TRACE_WARNING(_T("ControlRecordTOA COM=%d ; SendCommand without data\n"),
								GetCOMNumber());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CControlRecordTOA::DecodeData()
{
/*	if (CheckLengthAndCRC())
	{
		int iLen = m_byaReceived.GetSize();
			WK_TRACE(_T("DATA Lenght  : %i\n"), iLen);
		BYTE byte;
		if (iLen > 3)
		{
			byte = m_byaReceived.GetAtFast(3);
			WK_TRACE(_T("     Command : %02x %3d\n"), byte, byte);
			if (byte == 0x1A)
			{
				m_bTelemetrySet = !m_bTelemetrySet;
			}
		}
		if (iLen > 4)
		{
			byte = m_byaReceived.GetAtFast(4);
			WK_TRACE(_T("     Data len: %i\n"), byte);
		}
		if (iLen > 5)
		{
			byte = m_byaReceived.GetAtFast(5);
			WK_TRACE(_T("     Data 01 : %02x %3d\n"), byte, byte);
		}
		if (iLen > 6)
		{
			byte = m_byaReceived.GetAtFast(6);
			WK_TRACE(_T("     Data 02 : %02x %3d\n"), byte, byte);
		}
		if (iLen > 7)
		{
			byte = m_byaReceived.GetAtFast(7);
			WK_TRACE(_T("     Data 03 : %02x %3d\n"), byte, byte);
		}
		if (iLen > 8)
		{
			byte = m_byaReceived.GetAtFast(8);
			WK_TRACE(_T("     Data 04 : %02x %3d\n"), byte, byte);
		}
		if (iLen > 3)
		{
			SendACK(m_byaReceived.GetAtFast(2), m_byaReceived.GetAt(1));
		}
	}
*/
}
#endif
