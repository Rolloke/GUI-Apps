// ControlRecordBaxall.cpp: implementation of the CControlRecordBaxall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommUnit.h"
#include "ControlRecordBaxall.h"

//////////////////////////////////////////////////////////////////////
#define DEBUG_TRACE	WK_TRACE

//////////////////////////////////////////////////////////////////////
#define BAXALL_MODIFIER_CODE	0x08
#define BAXALL_ADD_CODE			BAXALL_MODIFIER_CODE + 0x01
#define BAXALL_SEND_ERROR_MAX	1

#define BAXALL_ACK				(BYTE)0x41
#define BAXALL_NAK				(BYTE)0x42
#define BAXALL_STATUS_RESPONSE	(BYTE)0x43
#define BAXALL_STATUS_REQUEST	(BYTE)0x81

//////////////////////////////////////////////////////////////////////
CControlRecordBaxall::CControlRecordBaxall(int iCom, BOOL bTraceHex)
	: CControlRecord(CCT_BAXALL, iCom, FALSE, bTraceHex, FALSE)
{
	m_eStatus = READY;
	m_bDecreaseBaxallAddCode = FALSE;
	m_bySourceID = 0x00;
	m_bySendErrorCounter = 0;
	RefreshTimeOut(300);
#ifdef _DEBUG
	m_bTelemetrySet = FALSE;
#endif
}
//////////////////////////////////////////////////////////////////////
CControlRecordBaxall::~CControlRecordBaxall()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordBaxall::Create()
{
	DWORD	dwBaudRate	= CBR_9600;
	BYTE	byDataBits	= 8;
	BYTE	byParity	= NOPARITY;
	BYTE	byStopBits	= ONESTOPBIT;
	ReadPortValues(dwBaudRate, byDataBits, byParity, byStopBits);
	WK_TRACE(_T("ControlRecordBaxall Create COM=%d\n"), GetCOMNumber());
	return Open(dwBaudRate, byDataBits, byParity, byStopBits);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordBaxall::Add(CSecID secID, DWORD dwCamID)
{
	if (CheckForValidBaxallIDs(dwCamID))
	{
		CControlRecord::Add(secID, dwCamID);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::OnNewCommand(CCameraCommandRecord* pCCommandR)
{
	if (theApp.m_bTraceCommand)
	{
		WK_TRACE(_T("OnNewCommand %s, %s, %lx, %d\n"),
				 NameOfEnum(GetType()),
				 NameOfEnum(pCCommandR->GetCommand()),
				 pCCommandR->GetCamID().GetID(),
				 pCCommandR->GetValue());
	}

	CameraControlCmd cmd = pCCommandR->GetCommand();
//	DWORD	dwValue = pCCommandR->GetValue();
	WORD	wSpeedX = min(10, LOWORD(pCCommandR->GetValue()));
	WORD	wSpeedY = min(10, HIWORD(pCCommandR->GetValue()));
	if (!wSpeedY) wSpeedY = wSpeedX;
	BYTE	bySpeedX = (BYTE)((0xFF * wSpeedX) / 10);
	BYTE	bySpeedY = (BYTE)((0xFF * wSpeedY) / 10);

//	BYTE bySourceID	= 0x00; // initialise with invalid source id
	BYTE byDestID	= 0x00; // initialise with invalid dest id
	BYTE byCamID	= 0x00; // initialise with invalid camera id
	if (GetBaxallIDs(pCCommandR->GetCamID(), byDestID, byCamID))
	{
		if (   (cmd != CCC_PAN_STOP)
			&& (cmd != CCC_TILT_STOP)
			&& (cmd != CCC_ZOOM_STOP)
			&& (cmd != CCC_FOCUS_STOP)
			&& (cmd != CCC_IRIS_STOP)
			)
		{
			// First select camera command
			FillCameraCmd(byDestID, byCamID);
		}

		// Second the telemetry command
		if ( (CCC_PAN_LEFT <= cmd) && (cmd <= CCC_ZOOM_STOP) )
		{
			if (   (cmd != CCC_PAN_STOP)
				&& (cmd != CCC_TILT_STOP)
				&& (cmd != CCC_ZOOM_STOP)
				&& (cmd != CCC_FOCUS_STOP)
				&& (cmd != CCC_IRIS_STOP)
				)
			{
				// Switch to telemetry enabled
				FillTelemetrySetCmd(byDestID);
			}
			
			// Iris has no speed
			if ( (CCC_IRIS_OPEN <= cmd) && (cmd <= CCC_IRIS_STOP) )
			{
				bySpeedX = 0;
				bySpeedY = 0;
			}
			FillTelemetryCmd(byDestID, cmd, bySpeedX, bySpeedY);
		}
		else if ( (CCC_POS_1 <= cmd) && (cmd <= CCC_POS_9) )
		{
			FillPresetCmd(byDestID, cmd);
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
BOOL CControlRecordBaxall::CheckForValidBaxallIDs(DWORD dwCamID)
{
	BOOL bValid = FALSE;
	BYTE bySourceID = (BYTE)(dwCamID / 100000); // Source ID as remote keyboard 1 - 8 as decimal Sddccc
	if ( (1 <= bySourceID) && (bySourceID <= 8))
	{
		dwCamID -= (bySourceID * 100000);
		bySourceID += 0x80;
		// check for consistent source ID (only the same ID allowed
		if (m_bySourceID == 0x00)
		{
			m_bySourceID = bySourceID;
		}
		if (m_bySourceID == bySourceID)
		{
			BYTE byDestID = (BYTE)(dwCamID / 1000); // Dest ID as ZTX6 or ZMX 9 - 25 as decimal sDDccc
//			if ( (1 <= byDestID) && (byDestID <= 25)) // for CRC test only
			if ( (9 <= byDestID) && (byDestID <= 25))
			{
				dwCamID -= (byDestID * 1000);
				byDestID += 0x80;
				BYTE byCamID = (BYTE)dwCamID; // CameraID 1 - 255 as decimal sddCCC
				if ( (1 <= byCamID) && (byCamID <= 255))
				{
					TRACE(_T("Baxall IDs Source %02x Dest %02x Cam %u\n"), bySourceID, byDestID, byCamID);
					bValid = TRUE;
				}
				else
				{
					WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid Cam ID %d\n"),
										GetCOMNumber(), byCamID);
				}
			}
			else
			{
				WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid Dest ID %d\n"),
									GetCOMNumber(), byDestID);
			}
		}
		else
		{
			WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Inconsistent Source ID %02x<>%02x\n"),
								GetCOMNumber(), m_bySourceID, bySourceID);
		}
	}
	else
	{
		WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid Source ID %d\n"),
								GetCOMNumber(), bySourceID);
	}
	return bValid;
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordBaxall::GetBaxallIDs(CSecID idCamSec, BYTE& byDestID, BYTE& byCamID)
{
	BOOL bFound = FALSE;
	BOOL bValid = FALSE;
	for (int i=0 ; i<m_secIDs.GetSize() ; i++)
	{
		if (idCamSec == m_secIDs[i])
		{
			bFound = TRUE;
			// found Camera SecID, now check for valid system IDs
			DWORD dwID = m_camIDs[i];
			BYTE bySourceID = (BYTE)(dwID / 100000); // Source ID as remote keyboard 1 - 8 as decimal Sddccc
			if ( (1 <= bySourceID) && (bySourceID <= 8))
			{
				dwID -= (bySourceID * 100000);
				bySourceID += 0x80;
				// check for consistent source ID (only the same ID allowed
				if (m_bySourceID == 0x00)
				{
					m_bySourceID = bySourceID;
				}
				if (m_bySourceID == bySourceID)
				{
					byDestID = (BYTE)(dwID / 1000); // Dest ID as ZTX6 or ZMX 9 - 25 as decimal sDDccc
//					if ( (1 <= byDestID) && (byDestID <= 25)) // for CRC test only
					if ( (9 <= byDestID) && (byDestID <= 25))
					{
						dwID -= (byDestID * 1000);
						byDestID += 0x80;
						byCamID = (BYTE)dwID; // CameraID 1 - 255 as decimal sddCCC
						if ( (1 <= byCamID) && (byCamID <= 255))
						{
							TRACE(_T("Baxall IDs Source %02x Dest %02x Cam %u\n"), bySourceID, byDestID, byCamID);
							bValid = TRUE;
						}
						else
						{
							WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid Cam ID %d\n"),
												GetCOMNumber(), byCamID);
						}
					}
					else
					{
						WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid Dest ID %d\n"),
											GetCOMNumber(), byDestID);
					}
				}
				else
				{
					WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Inconsistent Source ID %02x<>%02x\n"),
										GetCOMNumber(), m_bySourceID, bySourceID);
				}
			}
			else
			{
				WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid Source ID %d\n"),
										GetCOMNumber(), byDestID);
			}
			break;
		}
	}
	if (!bFound)
	{
		WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid SecID %08x\n"),
								GetCOMNumber(), idCamSec);
	}
	return bValid;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillCameraCmd(BYTE byDestID, BYTE byCamID)
{
	CCommandRecord* pNewCommand = FillHeader(byDestID);
	pNewCommand->Add(0x0c);	// camera cmd
	// modifier code because no. of original data bytes = 1 <= BAXALL_MODIFIER_CODE
	pNewCommand->Add(BAXALL_MODIFIER_CODE);
	pNewCommand->Add(BAXALL_ADD_CODE + 1);
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byCamID <= BAXALL_MODIFIER_CODE)
	{
		pNewCommand->Add(BAXALL_MODIFIER_CODE);
		pNewCommand->Add((BYTE)(byCamID + BAXALL_ADD_CODE));
	}
	else
	{
		pNewCommand->Add(byCamID);
	}
	FillCRCandFooterPAD(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillTelemetrySetCmd(BYTE byDestID)
{
	CCommandRecord* pNewCommand = FillHeader(byDestID);
	pNewCommand->Add(0x1A);	// Telemetry set
	// modifier code because no. of original data bytes = 0 <= BAXALL_MODIFIER_CODE
	pNewCommand->Add(BAXALL_MODIFIER_CODE);
	pNewCommand->Add(BAXALL_ADD_CODE);
	FillCRCandFooterPAD(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillPresetCmd(BYTE byDestID, CameraControlCmd cmd)
{
	// calculate the data value from the cmd
	BYTE byPreset = 0xFF; // our define as home position
	switch (cmd)
	{
		case CCC_POS_1:
		case CCC_POS_2:
		case CCC_POS_3:
		case CCC_POS_4:
		case CCC_POS_5:
		case CCC_POS_6:
			byPreset = (BYTE)(cmd - CCC_POS_1 + 1);
			break;
		case CCC_POS_HOME:
			byPreset = 0xFF;
			break;
		case CCC_POS_7:
		case CCC_POS_8:
		case CCC_POS_9:
			byPreset = (BYTE)(cmd - CCC_POS_7 + 7);
			break;
	}
	
	DEBUG_TRACE(_T("ControlRecordBaxall COM=%d FillPresetCmd %02x\n"), GetCOMNumber(), byPreset);

	CCommandRecord* pNewCommand = FillHeader(byDestID);
	pNewCommand->Add(0x0E);	// preset select
	// modifier code because no. of original data bytes = 1 <= BAXALL_MODIFIER_CODE
	pNewCommand->Add(BAXALL_MODIFIER_CODE);
	pNewCommand->Add(BAXALL_ADD_CODE + 1);
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byPreset <= BAXALL_MODIFIER_CODE)
	{
		pNewCommand->Add(BAXALL_MODIFIER_CODE);
		pNewCommand->Add((BYTE)(byPreset + BAXALL_ADD_CODE));
	}
	else
	{
		pNewCommand->Add(byPreset);
	}
	FillCRCandFooterPAD(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillTelemetryCmd(BYTE byDestID, CameraControlCmd cmd, BYTE bySpeedX, BYTE bySpeedY)
{
	// check if command can be repeated, all accept STOP
	BOOL bRepeat = TRUE;
	switch (cmd)
	{
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_MOVE_STOP:
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_STOP:
		case CCC_IRIS_STOP:
			bRepeat = FALSE;
			break;
	}

	BYTE byCmd = 0x00; // default inactive (Stop)
	switch (cmd)
	{
		case CCC_PAN_LEFT:			byCmd = 0x21;	break;
		case CCC_PAN_RIGHT:			byCmd = 0x23;	break;
		case CCC_TILT_UP:			byCmd = 0x24;	break;
		case CCC_TILT_DOWN:			byCmd = 0x2c;	break;
		case CCC_MOVE_LEFT_UP:		byCmd = 0x25;	break;
		case CCC_MOVE_LEFT_DOWN:	byCmd = 0x2d;	break;
		case CCC_MOVE_RIGHT_UP:		byCmd = 0x27;	break;
		case CCC_MOVE_RIGHT_DOWN:	byCmd = 0x2f;	break;
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_MOVE_STOP:			byCmd = 0x20;	break;
		case CCC_ZOOM_IN:			byCmd = 0x03;	break;
		case CCC_ZOOM_OUT:			byCmd = 0x01;	break;
		case CCC_ZOOM_STOP:			byCmd = 0x00;	break;
		case CCC_FOCUS_NEAR:		byCmd = 0x04;	break;
		case CCC_FOCUS_FAR:			byCmd = 0x0c;	break;
		case CCC_FOCUS_STOP:		byCmd = 0x00;	break;
		case CCC_IRIS_OPEN:			byCmd = 0x11;	break;
		case CCC_IRIS_CLOSE:		byCmd = 0x13;	break;
		case CCC_IRIS_STOP:			byCmd = 0x10;	break;
			break;
	}
	
	DEBUG_TRACE(_T("ControlRecordBaxall COM=%d FillTelemetryCmd %02x Speed %02x, %02x\n"),
									GetCOMNumber(), byCmd, bySpeedX, bySpeedY);
	CCommandRecord* pNewCommand = FillHeader(byDestID);
	pNewCommand->m_bRepeatAllowed = bRepeat;
	pNewCommand->Add(0x10);	// telemetry cmd
	// modifier code because no. of original data bytes = 3 <= BAXALL_MODIFIER_CODE
	pNewCommand->Add(BAXALL_MODIFIER_CODE);
	pNewCommand->Add(BAXALL_ADD_CODE + 3);
	// 2nd byte, telemetry type and action
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byCmd <= BAXALL_MODIFIER_CODE)
	{
		pNewCommand->Add(BAXALL_MODIFIER_CODE);
		pNewCommand->Add((BYTE)(byCmd + BAXALL_ADD_CODE));
	}
	else
	{
		pNewCommand->Add(byCmd);
	}
	// 3rd byte, joystick speed for PAN or magnitude for ZOOM
	BYTE byZoomSpeed = 0x00; // default no joystick, no speed cmd
	switch (cmd)
	{
		case CCC_PAN_LEFT:
		case CCC_PAN_RIGHT:
		case CCC_PAN_STOP:
		case CCC_ZOOM_IN:
		case CCC_ZOOM_OUT:
		case CCC_ZOOM_STOP:
			byZoomSpeed = bySpeedX;
			break;
	}
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	TRACE(_T("Baxall Telemetry Zoom Speed %u\n"), byZoomSpeed);
	if (byZoomSpeed <= BAXALL_MODIFIER_CODE)
	{
		pNewCommand->Add(BAXALL_MODIFIER_CODE);
		pNewCommand->Add((BYTE)(byZoomSpeed + BAXALL_ADD_CODE));
	}
	else
	{
		pNewCommand->Add(byZoomSpeed);
	}
	// 4th byte, joystick speed for TILT or magnitude for FOCUS
	BYTE byFocusSpeed = 0x00; // default no joystick, no speed cmd
	switch (cmd)
	{
		case CCC_TILT_UP:
		case CCC_TILT_DOWN:
		case CCC_TILT_STOP:
		case CCC_FOCUS_NEAR:
		case CCC_FOCUS_FAR:
		case CCC_FOCUS_STOP:
			byFocusSpeed = bySpeedY;
			break;
	}
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byFocusSpeed <= BAXALL_MODIFIER_CODE)
	{
		pNewCommand->Add(BAXALL_MODIFIER_CODE);
		pNewCommand->Add((BYTE)(byFocusSpeed + BAXALL_ADD_CODE));
	}
	else
	{
		pNewCommand->Add(byFocusSpeed);
	}
	FillCRCandFooterPAD(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillStatusResponse(BYTE byDestID)
{
	CCommandRecord* pNewCommand = FillHeader(byDestID);
	pNewCommand->Add(0x0a);					// status command
	// modifier code because no. of original data bytes = 4 <= BAXALL_MODIFIER_CODE
	pNewCommand->Add(BAXALL_MODIFIER_CODE);
	pNewCommand->Add(BAXALL_ADD_CODE + 4);
	pNewCommand->Add(0x80);					// device type
	pNewCommand->Add(BAXALL_STATUS_RESPONSE);	// status response
	// next are 2 LED bytes, set to zero
	pNewCommand->Add(BAXALL_MODIFIER_CODE);
	pNewCommand->Add(BAXALL_ADD_CODE);
	pNewCommand->Add(BAXALL_MODIFIER_CODE);
	pNewCommand->Add(BAXALL_ADD_CODE);
	FillCRCandFooterPAD(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
CCommandRecord* CControlRecordBaxall::FillHeader(BYTE byDestID)
{
	CCommandRecord* pNewCommand = new CCommandRecord;
	pNewCommand->Add(0xAA);			// Header PAD
	pNewCommand->Add(0xAA);			// Header PAD
	pNewCommand->Add(_STX_);		// Start sysmbol
	pNewCommand->Add(byDestID);		// Destination ID
	pNewCommand->Add(m_bySourceID);	// Source ID
	return pNewCommand;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillCRCandFooterPAD(CCommandRecord* pNewCommand)
{
	// checksum, sum up from source id (index 4) to end of data
	WORD wCRC = 0;
	BYTE byte = 0;
	for (int i=4 ; i<pNewCommand->GetSize() ; i++)
	{
		byte = pNewCommand->GetAtFast(i);
		if (byte == BAXALL_MODIFIER_CODE)
		{
			// ignore and take next reduced
			i++;
			byte  = pNewCommand->GetAtFast(i);
			byte -= BAXALL_ADD_CODE;
			wCRC = (WORD) (wCRC + byte);
		}
		else
		{
			wCRC = (WORD) (wCRC + byte);
		}
	}
	// now set the CRC bytes appropriate
	BYTE byCRC = LOBYTE(wCRC);
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byCRC <= BAXALL_MODIFIER_CODE)
	{
		pNewCommand->Add(BAXALL_MODIFIER_CODE);
		pNewCommand->Add((BYTE)(byCRC + BAXALL_ADD_CODE));
	}
	else
	{
		pNewCommand->Add(byCRC);
	}
	byCRC = HIBYTE(wCRC);
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byCRC <= BAXALL_MODIFIER_CODE)
	{
		pNewCommand->Add(BAXALL_MODIFIER_CODE);
		pNewCommand->Add((BYTE)(byCRC + BAXALL_ADD_CODE));
	}
	else
	{
		pNewCommand->Add(byCRC);
	}
	// end of data and footer PAD
	pNewCommand->Add(_ETX_);
	pNewCommand->Add(0xAA);
	pNewCommand->Add(0x55);
	pNewCommand->Add(0x00);

	m_arrayCommand.SafeAdd(pNewCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::OnReceivedData(LPBYTE pBuffer,DWORD dwLen)
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
			WK_TRACE_ERROR(_T("ControlRecordBaxall COM%i Daten Len %i Buffer-Overrun, no ETX\n"),
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
			m_bDecreaseBaxallAddCode = FALSE;
			m_byaReceived.Add(byte); // add for easier handling
		}
		// Wenn ein Endezeichen kommt, ist ein Datensatz komplett
		else if (byte == _ETX_)
		{
			m_byaReceived.Add(byte); // add for easier handling
			CheckData();
			m_bDecreaseBaxallAddCode = FALSE;
		}
		else
		{
			// Sonst das Zeichen in den Buffer sichern,
			if (byte == BAXALL_MODIFIER_CODE)
			{
				m_bDecreaseBaxallAddCode = TRUE;
			}
			else
			{
				if (m_bDecreaseBaxallAddCode)
				{
					byte -= BAXALL_ADD_CODE;
					m_bDecreaseBaxallAddCode = FALSE;
				}
				m_byaReceived.Add(byte); 
			}
		}
		dw++;
	} // while
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::OnTimeOutTransparent()
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
			m_bySendErrorCounter++;
			if (m_bySendErrorCounter >= BAXALL_SEND_ERROR_MAX)
			{
				WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; OnTimeOutTransparent OVERFLOW\n"),
									GetCOMNumber());
				// remove current command and try next one
				CurrentCommandFinished(TRUE);
			}
			else
			{
				TryToSendCommand(TRUE);
			}
		}
	}
	else
	{ // valid state if nothing to do
	}
	m_csSendCommand.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::CheckData()
{
	int iLen = m_byaReceived.GetSize();
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
							if (byte == BAXALL_ACK)	// ACK
							{
								OnACK();
							}
							else if (byte == BAXALL_NAK)	// NAK
							{
								OnNAK();
							}
							else if (byte == BAXALL_STATUS_REQUEST)	// Status request
							{
								OnStatusRequest(m_byaReceived.GetAtFast(2));
							}
							else
							{
								WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d CheckData UNKNOWN\n"), GetCOMNumber());
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
			WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Data not for us %02x<>%02x\n"),
									GetCOMNumber(), byte, m_bySourceID);
#endif
		}
	}
	else
	{
		DataCorrupt(1);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordBaxall::CheckLengthAndCRC()
{
	BOOL bReturn = FALSE;
	int iLen = m_byaReceived.GetSize();
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
				wCRC_T = (WORD)(wCRC_T << 8);
				wCRC_T = wCRC_T + (WORD)m_byaReceived.GetAtFast(iIndexLastCrcByte-1);
				WORD wCRC_R = 0;
				int iLastDataIndex = iIndexLastCrcByte - 2;
				for (int i=2 ; i<=iLastDataIndex ; i++)
				{
					wCRC_R = wCRC_R + (WORD)m_byaReceived.GetAtFast(i);
				}
				if (wCRC_T == wCRC_R)
				{
					bReturn = TRUE;
				}
				else
				{
					WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Invalid CRC %04x<>%04x\n"),
											GetCOMNumber(), wCRC_T, wCRC_R);
				}
			}
			else
			{
				DataCorrupt(byLenR);
			}
		}
		else
		{
			WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Length invalid %02x<>%02x\n"),
									GetCOMNumber(), byLenT, byLenR);
		}
	}
	else
	{
		DataCorrupt(4);
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::DataCorrupt(int iIndex)
{
	WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; Data corrupt for index %i\n"),
							GetCOMNumber(), iIndex);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::CurrentCommandFinished(BOOL bForceNextOne)
{
	DEBUG_TRACE(_T("ControlRecordBaxall COM=%d CurrentCommandFinished\n"), GetCOMNumber());
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
void CControlRecordBaxall::OnACK()
{
	DEBUG_TRACE(_T("ControlRecordBaxall COM=%d OnACK\n"), GetCOMNumber());
	CurrentCommandFinished(FALSE);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::OnNAK()
{
	DEBUG_TRACE(_T("ControlRecordBaxall COM=%d OnNAK\n"), GetCOMNumber());
	m_csSendCommand.Lock();
	m_bySendErrorCounter++;
	if (m_bySendErrorCounter >= BAXALL_SEND_ERROR_MAX)
	{
		WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; OnNAK OVERFLOW\n"),
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
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::OnStatusRequest(BYTE byDestID)
{
	DEBUG_TRACE(_T("ControlRecordBaxall COM=%d OnStatusRequest\n"), GetCOMNumber());
	m_csSendCommand.Lock();
	FillStatusResponse(byDestID);
	TryToSendCommand(FALSE);
	m_eStatus = READY;	// we do not await a response
	m_csSendCommand.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::TryToSendCommand(BOOL bForceRepeat)
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
			Write(m_pCurrentCommand->GetData(), m_pCurrentCommand->GetSize());
			m_eStatus = WAITING_FOR_ACK;
		}
		else
		{
			WK_TRACE_WARNING(_T("ControlRecordBaxall COM=%d ; SendCommand without data\n"),
								GetCOMNumber());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CControlRecordBaxall::DecodeData()
{
	if (CheckLengthAndCRC())
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
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::SendACK(BYTE byDestID, BYTE bySource)
{
	CCommandRecord* pACK = new CCommandRecord;
	pACK->Add(0xAA);		// Header PAD
	pACK->Add(0xAA);		// Header PAD
	pACK->Add(_STX_);		// Start sysmbol
	pACK->Add(byDestID);	// Destination ID
	pACK->Add(bySource);	// Source ID

	pACK->Add(0x0A);		// ACK/NAK/Status
	pACK->Add(BAXALL_MODIFIER_CODE);
	pACK->Add(BAXALL_ADD_CODE + 6); // 6 Bytes data

	pACK->Add(0xA1);		// we simulate a ZTX6
	pACK->Add(0x41);		// the ACK
	if (m_bTelemetrySet)
	{
		pACK->Add(0x80);		// LED 1
	}
	else
	{
		pACK->Add(BAXALL_MODIFIER_CODE);
		pACK->Add(BAXALL_ADD_CODE);
	}
	pACK->Add(BAXALL_MODIFIER_CODE);
	pACK->Add(BAXALL_ADD_CODE + 1);	// LED 2, very short beep
	pACK->Add(BAXALL_MODIFIER_CODE);
	pACK->Add(BAXALL_ADD_CODE + 2);	// LED 3, Pause
	pACK->Add(BAXALL_MODIFIER_CODE);
	pACK->Add(BAXALL_ADD_CODE );	// LED 4, always zero

	FillCRCandFooterPAD(pACK);

	m_csSendCommand.Lock();
	TryToSendCommand(FALSE);
	m_csSendCommand.Unlock();
	CurrentCommandFinished(TRUE);
}
#endif