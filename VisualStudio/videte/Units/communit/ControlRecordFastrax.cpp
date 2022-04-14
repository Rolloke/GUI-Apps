// ControlRecordFastrax.cpp: implementation of the CControlRecordFastrax class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommUnit.h"
#include "ControlRecordFastrax.h"

//////////////////////////////////////////////////////////////////////
#define DEBUG_TRACE	TRACE
#define LEN_ANSWER_NONE		1
#define LEN_ANSWER_STATUS	4
#define LEN_ANSWER_POSITION	8

#define REPEAT_INFINITE		MAXLONG

//////////////////////////////////////////////////////////////////////
CControlRecordFastrax::CControlRecordFastrax(int iCom, BOOL bTraceHex)
	: CControlRecord(CCT_FASTRAX, iCom, FALSE, bTraceHex, FALSE)
{
	m_bBidirectional  = FALSE;
	m_dwRepeatTime    = 100;
	m_bOpenCloseRS232 = FALSE;

	m_eStatusSend     = READY;
	m_eStatusReceive  = ANSWER_NONE;
	m_iBytesToReceive = LEN_ANSWER_NONE;

	CWK_Profile wkp;
	m_bBidirectional  = wkp.GetInt(_T("CommUnit\\Debug"), _T("FastraxBidirectional"), m_bBidirectional);
	m_dwRepeatTime    = wkp.GetInt(_T("CommUnit\\Debug"), _T("RepeatTimeFastrax"), m_dwRepeatTime);
	m_bOpenCloseRS232 = wkp.GetInt(_T("CommUnit\\Debug"), _T("FastraxOpenCloseRS232"), m_bOpenCloseRS232);

	wkp.WriteInt(_T("CommUnit\\Debug"), _T("FastraxBidirectional"), m_bBidirectional);
	wkp.WriteInt(_T("CommUnit\\Debug"), _T("RepeatTimeFastrax"), m_dwRepeatTime);
	wkp.WriteInt(_T("CommUnit\\Debug"), _T("FastraxOpenCloseRS232"), m_bOpenCloseRS232);

	if (m_bBidirectional)
	{
		m_dwRepeatTime    = 60000;
		m_bOpenCloseRS232 = FALSE;
	}

	WK_TRACE(_T("FastraxBidirectional  = %i\n"), m_bBidirectional);
	WK_TRACE(_T("RepeatTimeFastrax     = %i\n"), m_dwRepeatTime);
	WK_TRACE(_T("FastraxOpenCloseRS232 = %i\n"), m_bOpenCloseRS232);

	RefreshTimeOut(m_dwRepeatTime);
}
/////////////////////////////////////////////////////////////////////////////
CControlRecordFastrax::~CControlRecordFastrax()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordFastrax::Create()
{
	WK_TRACE(_T("ControlRecordFastrax Create COM=%d\n"), GetCOMNumber());
	BOOL bOpenOK = OpenCom();
	if (   m_bOpenCloseRS232
		&& bOpenOK
		)
	{
		DEBUG_TRACE(_T("ControlRecordFastrax COM=%d Close Com at Create\n"), GetCOMNumber());
		Close();
	}
	return bOpenOK;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordFastrax::Add(CSecID secID, DWORD camID)
{
	if (IsBetween(camID, 1, 100))
	{
		CControlRecord::Add(secID, camID);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordFastrax::OnNewCommand(CCameraCommandRecord* pCCommandR)
{
	if (theApp.m_bTraceCommand)
	{
		WK_TRACE(_T("OnNewCommand %s, %s, %lx, %d\n"),
				 NameOfEnum(GetType()),
				 NameOfEnum(pCCommandR->GetCommand()),
				 pCCommandR->GetCamID().GetID(),
				 pCCommandR->GetValue());
	}

	BOOL bFound = FALSE;
	BYTE byID = 0x00;

	for (int i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			byID = (BYTE)(m_camIDs[i]); // CameraID 1 - 100
			break;
		}
	}
	if (bFound)
	{
		CameraControlCmd cmd = pCCommandR->GetCommand();
		DWORD dwValue = pCCommandR->GetValue();

		CCommandRecord* pNewCommand = CreateCommandRecord(byID);
		CameraControlCmd nReturn = FillCameraCmd(pNewCommand, cmd, dwValue);
		FillCRC(pNewCommand);
		m_arrayCommand.SafeAdd(pNewCommand);
		DEBUG_TRACE(_T("ControlRecordFastrax COM=%d Add new %08x Repeat:%d\n"), GetCOMNumber(), pNewCommand, pNewCommand->m_bRepeatAllowed);
		TryToSendCommand(FALSE);
		if (nReturn != CCC_INVALID)
		{
			Sleep(100);
			CCameraCommandRecord *pccr = new CCameraCommandRecord(pCCommandR->GetCommID(), pCCommandR->GetCamID(), nReturn, pCCommandR->GetValue());
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CAMERA_COMMAND, (LPARAM)pccr);
		}

		// Workaround for second preset command
/*		if (   (CCC_POS_1 <= cmd)
			&& (cmd <= CCC_POS_9)
			&& (cmd != CCC_POS_HOME)
			)
		{
			BOOL bPreset = (BOOL)dwValue;
			if (bPreset)
			{
				CCommandRecord* pNewCommand2 = CreateCommandRecord(byID);
				FillSecondPresetCmd(pNewCommand2, cmd);
				FillCRC(pNewCommand2);
				m_arrayCommand.SafeAdd(pNewCommand2);
				DEBUG_TRACE(_T("ControlRecordFastrax COM=%d Add new %08x Repeat:%d\n"), GetCOMNumber(), pNewCommand2, pNewCommand2->m_bRepeatAllowed);
				TryToSendCommand(FALSE);
			}
		}
*/
	}
}
/////////////////////////////////////////////////////////////////////////////
CCommandRecord* CControlRecordFastrax::CreateCommandRecord(BYTE byID)
{
	CCommandRecord* pCommand = new CCommandRecord();
	pCommand->Add(0xA5);	// Header
	pCommand->Add(byID);	// id	
	pCommand->Add(0x10);	// for lens control with Pan/ Tilt commands, functions are special values
	pCommand->Add(0x0D);	// stop zoom as default value for pan/tilt commands
	pCommand->Add(0x88);	// stop pan & tilt as default value for focus/iris/zoom commands
	pCommand->Add(0x55);	// End
	pCommand->Add(0x00);	// CRC
	return pCommand;
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd CControlRecordFastrax::FillCameraCmd(CCommandRecord* pCommand, CameraControlCmd cmd, DWORD dwValue)
{
	// some special values from dwValue
//	TRACE(_T("FastraxII dwValue %u\n"), dwValue);
	BOOL bPreset = (BOOL)dwValue;
	WORD wSpeedX = min(10, LOWORD(dwValue));	// 1 - 10
	WORD wSpeedY = min(10, HIWORD(dwValue));	// 1 - 10
	if (!wSpeedY) wSpeedY = wSpeedX;
	BYTE bySpeedX = (BYTE)(((7.0 * (float)wSpeedX) / 10.0) + 0.5);	// 1 - 7
	BYTE bySpeedY = (BYTE)(((7.0 * (float)wSpeedY) / 10.0) + 0.5);	// 1 - 7
	BOOL bRepeat = REPEAT_INFINITE;
	CameraControlCmd nReturn = CCC_INVALID;
	
	TRACE(_T("FillCameraCmd:%s, (%d, %d)\n"), NameOfEnum(cmd), bySpeedX, bySpeedY);

	if (cmd == CCC_CONFIG)
	{
		switch (dwValue)
		{
			case PTZF_CONFIG_PANASONIC_ON:
				pCommand->SetAt(2, 0x19);
				pCommand->SetAt(3, 0x80);
				pCommand->SetAt(4, 0x80);
				break;
			case PTZF_CONFIG_PANASONIC_OFF:
				pCommand->SetAt(2, 0x1f);
				pCommand->SetAt(3, 0x80);
				pCommand->SetAt(4, 0x80);
				pCommand->m_bRepeatAllowed = 3;
				break;
			case PTZF_CONFIG_PANASONIC_UP:
				cmd = CCC_TILT_UP;
				bySpeedX = 7;
				bRepeat = FALSE;
				nReturn = CCC_TILT_STOP;
				break;
			case PTZF_CONFIG_PANASONIC_DOWN:
				cmd = CCC_TILT_DOWN;
				bySpeedX = 7;
				bRepeat = FALSE;
				nReturn = CCC_TILT_STOP;
				break;
			case PTZF_CONFIG_PANASONIC_LEFT:
				cmd = CCC_PAN_LEFT;
				bySpeedX = 7;
				bRepeat = FALSE;
				nReturn = CCC_PAN_STOP;
				break;
			case PTZF_CONFIG_PANASONIC_RIGHT:
				cmd = CCC_PAN_RIGHT;
				bySpeedX = 7;
				bRepeat = FALSE;
				nReturn = CCC_PAN_STOP;
				break;
			case PTZF_CONFIG_PANASONIC_SET1:	// Start Tour 1
				pCommand->SetAt(2, 0x13);
				pCommand->SetAt(3, 0x80);
				pCommand->SetAt(4, 0x81);
				break;
			case PTZF_CONFIG_PANASONIC_SET2:	// Escape
				pCommand->SetAt(2, 0x1f);
				pCommand->SetAt(3, 0x80);
				pCommand->SetAt(4, 0x80);
				break;
			case PTZF_CONFIG_PANASONIC_RESET:
				break;
			case PTZF_CONFIG_PANASONIC_ALLRESET:
				break;
			case PTZF_CONFIG_SET3:
				cmd = CCC_ZOOM_IN;
				bRepeat = FALSE;
				nReturn = CCC_ZOOM_STOP;
				break;
			case PTZF_CONFIG_SET4:
				cmd = CCC_ZOOM_OUT;
				bRepeat = FALSE;
				nReturn = CCC_ZOOM_STOP;
				break;
		}
	}
//	TRACE(_T("FastraxII bySpeed %u\n"), bySpeed);
	switch (cmd)
	{
		case CCC_CONFIG: break;
		case CCC_PAN_LEFT:
			pCommand->SetAt(4, (BYTE)(0x88 - (bySpeedX<<4)));
			pCommand->m_bRepeatAllowed = bRepeat;
			break;
		case CCC_PAN_RIGHT:	
			pCommand->SetAt(4, (BYTE)(0x88 + (bySpeedX<<4)));
			pCommand->m_bRepeatAllowed = bRepeat;
			break;
		case CCC_TILT_UP:	
			pCommand->SetAt(4, (BYTE)(0x88 + (bySpeedX)));
			pCommand->m_bRepeatAllowed = bRepeat;
			break;
		case CCC_TILT_DOWN:	
			pCommand->SetAt(4, (BYTE)(0x88 - (bySpeedX)));
			pCommand->m_bRepeatAllowed = bRepeat;
			break;
		case CCC_MOVE_LEFT_UP:
			pCommand->SetAt(4, 0x80 - (bySpeedX<<4) | 0x08 + (bySpeedY));
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
		case CCC_MOVE_LEFT_DOWN:
			pCommand->SetAt(4, 0x80 - (bySpeedX<<4) | 0x08 - (bySpeedY));
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
		case CCC_MOVE_RIGHT_UP:
			pCommand->SetAt(4, 0x80 + (bySpeedX<<4) | 0x08 + (bySpeedY));
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
		case CCC_MOVE_RIGHT_DOWN:
			pCommand->SetAt(4, 0x80 + (bySpeedX<<4) | 0x08 - (bySpeedY));
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
	
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:	
		case CCC_MOVE_STOP:
			// use default
			break;

		case CCC_FOCUS_NEAR:
			pCommand->SetAt(3, 0x03);
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
		case CCC_FOCUS_FAR:
			pCommand->SetAt(3, 0x04);
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
		case CCC_FOCUS_STOP:
			// use default
			break;
		case CCC_IRIS_OPEN:
			pCommand->SetAt(3, 0x06);
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
		case CCC_IRIS_CLOSE:
			pCommand->SetAt(3, 0x05);
			pCommand->m_bRepeatAllowed = REPEAT_INFINITE;
			break;
		case CCC_IRIS_STOP:
			// use default
			break;
		case CCC_ZOOM_IN:
			pCommand->SetAt(3, 0x0E);
			pCommand->m_bRepeatAllowed = bRepeat;
			break;
		case CCC_ZOOM_OUT:
			pCommand->SetAt(3, 0x0C);
			pCommand->m_bRepeatAllowed = bRepeat; // only some commands have to be repeated
			break;
		case CCC_ZOOM_STOP:
			// use default
			break;
		case CCC_POS_1:
		case CCC_POS_2:
		case CCC_POS_3:
		case CCC_POS_4:
		case CCC_POS_5:
		case CCC_POS_6:
			if (bPreset == 2)
			{
				pCommand->SetAt(2, 0x26); // 0x26 clears the positions, so we can use the normal write_preset command
			}
			else if (bPreset == 1)
			{
//				pCommand->SetAt(2, 0x26); // 0x26 clears the positions, so we can use the normal write_preset command
//				pCommand->SetAt(2, 0x27); // 0x27 normal write_preset commands, works only on 'empty' positions (cleared)
				pCommand->SetAt(2, 0x25); // 0x25 clear_write_preset command, works only on already set positions (second or more set)
//				pCommand->m_bRepeatAllowed = 1; // workaround for the second preset command following
			}
			else
			{
				pCommand->SetAt(2, 0x11);
			}
			pCommand->SetAt(3, 0x80);
			pCommand->SetAt(4, (BYTE)(0x80 + (cmd - CCC_POS_1) +1));
			break;
		case CCC_POS_HOME:
			if (bPreset)
			{
				// FastraxII has no programable home position, but a home function, programable via keyboard controller
				// This can be one of the 240 preset positions - or none, a tour, a pattern, autoscan
				//	- 
			}
			else
			{
				pCommand->SetAt(2, 0x1D);
				pCommand->SetAt(3, 0x80);
				pCommand->SetAt(4, 0x80);
			}
			break;
		case CCC_POS_7:
		case CCC_POS_8:
		case CCC_POS_9:
			if (bPreset == 2)
			{
				pCommand->SetAt(2, 0x26); // 0x26 clears the positions, so we can use the normal write_preset command
			}
			else if (bPreset == 1)
			{
				// for comments look above!
				pCommand->SetAt(2, 0x25); // 0x25 clear_write_preset command, works only on already set positions (second or more set)
			}
			else
			{
				pCommand->SetAt(2, 0x11);
			}
			pCommand->SetAt(3, 0x80);
			pCommand->SetAt(4, (BYTE)(0x80 + (cmd - CCC_POS_1)));
			break;
	}
	return nReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordFastrax::FillSecondPresetCmd(CCommandRecord* pCommand, CameraControlCmd cmd)
{
	// Always preset
	switch (cmd)
	{
		case CCC_POS_1:
		case CCC_POS_2:
		case CCC_POS_3:
		case CCC_POS_4:
		case CCC_POS_5:
		case CCC_POS_6:
			pCommand->SetAt(2, 0x27); // 0x27 normal write_preset commands, works only on 'empty' positions (cleared)
//			pCommand->SetAt(2, 0x25); // 0x25 clear_write_preset command, works only on already set positions (second or more set)
			pCommand->SetAt(3, 0x80);
			pCommand->SetAt(4, (BYTE)(0x80 + (cmd - CCC_POS_1) +1));
			break;
		case CCC_POS_7:
		case CCC_POS_8:
		case CCC_POS_9:
			pCommand->SetAt(2, 0x27); // 0x27 normal write_preset commands, works only on 'empty' positions (cleared)
//			pCommand->SetAt(2, 0x25); // 0x25 clear_write_preset command, works only on already set positions (second or more set)
			pCommand->SetAt(3, 0x80);
			pCommand->SetAt(4, (BYTE)(0x80 + (cmd - CCC_POS_1)));
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordFastrax::FillCRC(CCommandRecord* pCommand)
{
	// calculate checksum
	BYTE byCRC = 0;
	for (int i=0 ; i<pCommand->GetSize() ; i++)
	{
		byCRC = (BYTE)(byCRC + pCommand->GetAtFast(i));		// checksum
	}
	pCommand->SetAt(6, byCRC);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordFastrax::OnReceivedData(LPBYTE pBuffer,DWORD dwLen)
{
	DWORD	dw = 0;
	BYTE	byte;
	while (pBuffer && dw<dwLen)
	{
		if (m_byaReceived.GetSize() >= m_iBytesToReceive)
		{
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			// Fehler, zu viele Zeichen
			CString sMsg;
			WK_TRACE_ERROR(_T("ControlRecordFastrax COM%i Daten Len %i Buffer-Overrun, no ETX\n"),
							GetCOMNumber(), m_byaReceived.GetSize());
			sMsg.Format( _T("Buffer-Overrun|COM%i"), GetCOMNumber() );
			WK_STAT_PEAK(_T("BufferOverrun"), 1, sMsg);
			m_byaReceived.RemoveAll();
		}

		byte = pBuffer[dw];
		if (theApp.m_bTraceData)
		{
			TRACE(_T("CControlRecordFastrax %i Received: %02x\n"), GetCOMNumber(), byte);
		}

		if (m_bBidirectional)
		{
			// Wenn ein Startzeichen kommt, fangen die Daten erst richtig an
			// Header status answer
			if (byte == 0xcc)
			{
				if (m_byaReceived.GetSize() == 0)
				{
					// OK, new answer starts
					// Camera sends all the time, so ignore the most of them 
/*					if (m_eStatusSend != WAITING_FOR_ANSWER_STATUS)
					{
						WK_TRACE_ERROR(_T("ControlRecordFastrax COM%i Wrong Answer start byte %02x for STATUS\n"),
										GetCOMNumber(), byte);
					}
*/
					m_eStatusReceive  = ANSWER_STATUS;
					m_byaReceived.Add(byte); // add for easier handling
					m_iBytesToReceive = LEN_ANSWER_STATUS;
				}
				else if (m_byaReceived.GetSize() < m_iBytesToReceive)
				{
					// OK, may be data or CRC
					m_byaReceived.Add(byte);
				}
				else
				{
					//Too many data bytes, start new record
					WK_TRACE_ERROR(_T("ControlRecordFastrax COM%i Daten Len %i Buffer-Overrun\n"),
									GetCOMNumber(), m_byaReceived.GetSize());
					CString sMsg;
					sMsg.Format( _T("Buffer-Overrun|COM%i"), GetCOMNumber() );
					WK_STAT_PEAK(_T("BufferOverrun"), 1, sMsg);
					ClearAllData();

					m_eStatusReceive  = ANSWER_STATUS;
					m_byaReceived.Add(byte); // add for easier handling
					m_iBytesToReceive = LEN_ANSWER_STATUS;
				}
			}
			// Header position data
			else if (byte == 0xc5)
			{
				if (m_byaReceived.GetSize() == 0)
				{
					// OK, new answer starts
					// Camera sends all the time, so ignore the most of them 
/*					if (m_eStatusSend != WAITING_FOR_ANSWER_POSITION)
					{
						WK_TRACE_ERROR(_T("ControlRecordFastrax COM%i Wrong Answer start byte %02x for STATUS\n"),
											GetCOMNumber(), byte);
					}
*/			
					m_eStatusReceive  = ANSWER_POSITION;
					m_byaReceived.Add(byte); // add for easier handling
					m_iBytesToReceive = LEN_ANSWER_POSITION;
				}
				else if (m_byaReceived.GetSize() < m_iBytesToReceive)
				{
					// OK, may be data or CRC
					m_byaReceived.Add(byte); // add for easier handling
				}
				else
				{
					//Too many data bytes, start new record
					WK_TRACE_ERROR(_T("ControlRecordFastrax COM%i Daten Len %i Buffer-Overrun\n"),
									GetCOMNumber(), m_byaReceived.GetSize());
					CString sMsg;
					sMsg.Format( _T("Buffer-Overrun|COM%i"), GetCOMNumber() );
					WK_STAT_PEAK(_T("BufferOverrun"), 1, sMsg);
					ClearAllData();

					m_eStatusReceive  = ANSWER_POSITION;
					m_byaReceived.Add(byte); // add for easier handling
					m_iBytesToReceive = LEN_ANSWER_POSITION;
				}
			}
			else if (m_eStatusReceive != ANSWER_NONE)
			{
				// Zeichen in den Buffer sichern,
				m_byaReceived.Add(byte); 
			}
			else
			{
				// ignorieren, wir warten auf ein Startzeichen
			}

			// Check for answer ok
			CheckData();
		}
		dw++;
	} // while
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordFastrax::OnTimeOutTransparent()
{
	if (m_bBidirectional == FALSE)
	{
		TryToSendCommand(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordFastrax::TryToSendCommand(BOOL bForceRepeat)
{
	m_csSendCommand.Lock();
	if (m_eStatusSend == READY)
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
				DEBUG_TRACE(_T("ControlRecordFastrax COM=%d TryToSendCommand get new %08x Repeat:%d\n"), GetCOMNumber(), m_pCurrentCommand, m_pCurrentCommand->m_bRepeatAllowed);
			}
		}

		if (m_pCurrentCommand)
		{
			if (m_pCurrentCommand->GetSize())
			{
				if (m_bBidirectional)
				{
					Write(m_pCurrentCommand->GetData(), m_pCurrentCommand->GetSize());
					if (m_pCurrentCommand->GetAtFast(2) == 0x40)
					{
						m_eStatusSend = WAITING_FOR_ANSWER_POSITION;
					}
					else
					{
						m_eStatusSend = WAITING_FOR_ANSWER_STATUS;
					}
				}
				else
				{
					if (m_bOpenCloseRS232)
					{
						if (OpenCom())
						{
							Write(m_pCurrentCommand->GetData(), m_pCurrentCommand->GetSize());
							if (CurrentCommandFinished(TRUE))
							{
								DEBUG_TRACE(_T("ControlRecordFastrax COM=%d TryToSendCommand no more data ; CLOSE\n"), GetCOMNumber());
								// Let the data flow ... before cutting the line
								Sleep(100);
								Close();
							}
						}
						else
						{ // Already traced
						}
					}
					else
					{
						Write(m_pCurrentCommand->GetData(), m_pCurrentCommand->GetSize());
						CurrentCommandFinished(TRUE);
					}
				}
			}
			else
			{
				WK_TRACE_WARNING(_T("CControlRecordFastrax COM=%d ; SendCommand without data\n"),
									GetCOMNumber());
			}
		}
		else
		{	// no new command, ok
		}
	}
	m_csSendCommand.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordFastrax::CurrentCommandFinished(BOOL bAcknowledged)
{
	DEBUG_TRACE(_T("ControlRecordFastrax COM=%d CurrentCommandFinished\n"), GetCOMNumber());
	BOOL bReturn = FALSE;
	m_csSendCommand.Lock();
	m_eStatusReceive = ANSWER_NONE;
	m_iBytesToReceive = LEN_ANSWER_NONE;
	m_eStatusSend = READY;
	if (bAcknowledged)
	{
		if (m_pCurrentCommand)
		{
			if (m_pCurrentCommand->m_bRepeatAllowed-- == FALSE)
			{
				WK_DELETE(m_pCurrentCommand);
				bReturn = TRUE;
			}
		}
	}
	if (m_bBidirectional)
	{
		TryToSendCommand(!bAcknowledged);
	}
	m_csSendCommand.Unlock();
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordFastrax::OpenCom()
{
	BOOL bReturn = FALSE;
	if (IsOpen())
	{
		bReturn = TRUE;
	}
	else
	{
		DWORD	dwBaudRate	= CBR_9600;
		BYTE	byDataBits	= 8;
		BYTE	byParity	= NOPARITY;
		BYTE	byStopBits	= ONESTOPBIT;
		ReadPortValues(dwBaudRate, byDataBits, byParity, byStopBits);
		DEBUG_TRACE(_T("ControlRecordFastrax COM=%d OpenCom\n"), GetCOMNumber());
		bReturn = Open(dwBaudRate, byDataBits, byParity, byStopBits);
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordFastrax::CheckData()
{
	int iLen = m_byaReceived.GetSize();
	// Check for valid length
	if (iLen == m_iBytesToReceive)
	{
		BOOL bOK = CheckCRC();
		ClearAllData();
		CurrentCommandFinished(bOK);
	}
	else
	{
		// ok, wait for end
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordFastrax::CheckCRC()
{
	BOOL bReturn = FALSE;
	BYTE byte = 0;
	for (int i=0 ; i<m_byaReceived.GetSize()-1 ; i++)
	{
		byte = (BYTE)(byte + (BYTE)m_byaReceived.GetAtFast(i));
	}

	if (m_byaReceived.GetAtFast(i) == byte)
	{
		bReturn = TRUE;
	}

	return bReturn;
}
