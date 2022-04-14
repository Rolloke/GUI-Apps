#include "stdafx.h"
#include "CommUnit.h"
#include "ControlRecordUni1.h"

#define DEBUG_TRACE	TRACE
#define FLAGS_MENU_ENABLED	1

CControlRecordUni1::CControlRecordUni1(int iCom, BOOL bTraceHex, CameraControlType camType)
	: CControlRecord(camType, iCom, FALSE, bTraceHex, FALSE)
{
	m_nReceiveCount = 0;

	CWK_Profile wkp;
	CString sReg			= GetRegPath();
	m_eStatus				= READY;
	m_bySourceID			= 0;
	m_bySendErrorCounter	= 0;
	DWORD dwReg, dwRefresh	= 250;
	BOOL  bRepeat			= FALSE;

	dwReg     = wkp.GetInt(sReg, _T("RefreshRate"), -1);
	m_bRepeat = wkp.GetInt(sReg, _T("Repeat"), -1);

	switch (GetType())
	{
		case CCT_PELCO_D:
//			dwRefresh = 200;
//			bRepeat = FALSE; 
			break;
		case CCT_PELCO_P:
//			dwRefresh = 200;
//			bRepeat = FALSE; 
			break;
		case CCT_BBV_RS422:
//			dwRefresh = 200;
//			bRepeat = FALSE; 
			break;
		case CCT_SENSORMATIC:
//			dwRefresh = 200;
//			bRepeat = FALSE; 
			break;
		case CCT_CBC_ZCNAF27:
//			dwRefresh = 200;
//			bRepeat = FALSE; 
			break;
		case CCT_VIDEOTEC:
//			dwRefresh = 200;
//			bRepeat = FALSE; 
			break;
		case CCT_BEWATOR:
//			dwRefresh = 200;
//			bRepeat = FALSE;
			break;
		case CCT_JVC_TK_C:
//			dwRefresh = 200;
//			bRepeat = FALSE;
			break;
		case CCT_MERIDIAN:
//			dwRefresh = 200;
//			bRepeat = FALSE;
			break;
		case CCT_RS232_TRANSPARENT:
			return;
	}

	if (dwReg == -1)
	{
		wkp.WriteInt(sReg, _T("RefreshRate"), dwRefresh);
	}
	else
	{
		dwRefresh = dwReg;
	}

	if (m_bRepeat == -1)
	{
		m_bRepeat = bRepeat;
		wkp.WriteInt(sReg, _T("Repeat"), m_bRepeat);
	}

	RefreshTimeOut(dwRefresh);
}
/////////////////////////////////////////////////////////////////////////////
CControlRecordUni1::~CControlRecordUni1()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordUni1::Create()
{
	DWORD dwBaudRate = CBR_9600;
	BYTE byDataBits = 8, byParity = NOPARITY, byStopBits = ONESTOPBIT;
	switch (GetType())
	{
		case CCT_PELCO_D:
			dwBaudRate = CBR_4800;
			break;
		case CCT_PELCO_P:
			dwBaudRate = CBR_4800;
			break;
		case CCT_BBV_RS422:
			break;
		case CCT_SENSORMATIC:
			dwBaudRate = CBR_4800;
			break;
		case CCT_CBC_ZCNAF27:
			break;
		case CCT_VIDEOTEC:
			break;
		case CCT_BEWATOR:
			byParity = EVENPARITY;
			break;
		case CCT_JVC_TK_C:
			byParity = EVENPARITY;
			break;
		case CCT_MERIDIAN:
			break;
	}

	ReadPortValues(dwBaudRate, byDataBits, byParity, byStopBits);
	WK_TRACE(_T("ControlRecord %s Create COM=%d\n"), NameOfEnum(GetType()), GetCOMNumber());
	return Open(dwBaudRate, byDataBits, byParity, byStopBits);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordUni1::Add(CSecID secID, DWORD camID)
{
	BOOL bOK = FALSE;
	DWORD dwFlags = 0;
	switch (GetType())
	{
		case CCT_PELCO_D:
			if (IsBetween(camID, 1, 255))
			{
				bOK = TRUE;
			} break;
		case CCT_PELCO_P:
			if (IsBetween(camID, 1, 32))
			{
				bOK = TRUE;
			} break;
		case CCT_BBV_RS422:
			if (IsBetween(camID, 1, 96))
			{
				bOK = TRUE;
			} break;
		case CCT_SENSORMATIC:
			if (IsBetween(camID, 1, 99))
			{
				bOK = TRUE;
			} break;
		case CCT_CBC_ZCNAF27:
			if (IsBetween(camID, 0, 255))
			{
				bOK = TRUE;
			} break;
		case CCT_VIDEOTEC:
			if (IsBetween(camID, 1, 999))
			{	// 254 is the universal identification code for messages to all connected receivers
				bOK = camID != 254 ? TRUE : FALSE;
			} break;
		case CCT_BEWATOR:
			if (IsBetween(camID, 1, 512))
			{	
				bOK = TRUE;
			}
			dwFlags = 0x00000001;	// low BYTE contains the latched ouput flags
									// camera on is bit zero
			break;
		case CCT_JVC_TK_C:
			if (IsBetween(camID, 1, 32))
			{
				bOK = TRUE;
			} break;
		case CCT_MERIDIAN:
			if (IsBetween(camID, 1, 999))
			{
				bOK = TRUE;
			} break;
		default: break;
	}
	if (bOK)
	{
		CControlRecord::Add(secID, camID);
		m_Flags.Add(dwFlags);
	}
	else
	{
		WK_TRACE(_T("CamID for %s Protokoll out of Range\n"), NameOfEnum(GetType()));
	}
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::OnNewCommand(CCameraCommandRecord* pCCommandR)
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
	DWORD dwID = 0x00;

	DWORD *pdwsecIDs = m_secIDs.GetData();
	for (int i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==pdwsecIDs[i])
		{
			// found
			bFound = TRUE;
			dwID = (DWORD)(m_camIDs[i]); // CameraID
			break;
		}
	}
	if (bFound)
	{
		CameraControlCmd nReturn = CCC_INVALID;
		switch (GetType())
		{
			case CCT_PELCO_D:
				CreateNewCmdPelcoD((BYTE)dwID, pCCommandR);
				break;
			case CCT_PELCO_P:
				CreateNewCmdPelcoP((BYTE)dwID, pCCommandR);
				break;
			case CCT_BBV_RS422:
				CreateNewCmdBBVRS422((BYTE)dwID, pCCommandR);
				break;
			case CCT_SENSORMATIC:
				CreateNewCmdSensormatic(i, pCCommandR);
				break;
			case CCT_CBC_ZCNAF27:
				nReturn = CreateNewCmdCbcNaf27(i, pCCommandR);
				break;
			case CCT_VIDEOTEC:
				nReturn = CreateNewCmdVideotec((WORD)dwID, pCCommandR);
				break;
			case CCT_BEWATOR:
				nReturn = CreateNewCmdBewator(i, pCCommandR);
				break;
			case CCT_JVC_TK_C:
				nReturn = CreateNewCmdJvcTKC(i, pCCommandR);
				break;
			case CCT_MERIDIAN:
				nReturn = CreateNewCmdMeridian(i, pCCommandR);
				break;
		}
		if (nReturn != CCC_INVALID)
		{
			Sleep(200);
			CCameraCommandRecord *pccr = new CCameraCommandRecord(pCCommandR->GetCommID(), pCCommandR->GetCamID(), nReturn, pCCommandR->GetValue());
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CAMERA_COMMAND, (LPARAM)pccr);
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::CreateNewCmdPelcoD(BYTE byID, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	DWORD	wSpeedX = min(10, LOWORD(dwValue)); // 1 - 10
	DWORD	wSpeedY = min(10, HIWORD(dwValue)); // 1 - 10
	BYTE	command1 = 0, command2 = 0, data1 = 0, data2 = 0;
	BOOL    bTelemetryCommand = TRUE;
	if (!wSpeedY) wSpeedY = wSpeedX;
	data1 = (BYTE)(wSpeedX * 7);
	data2 = (BYTE)(wSpeedY * 7);
	if (wSpeedX == 10)
	{
		data1 = 0xff;
	}
	if (wSpeedY == 10)
	{
		data2 = 0x3f;
	}

	BOOL bRepeat = m_bRepeat;	// repeat 
	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_AUTO:							break;
		case CCC_FOCUS_FAR:			command2 = 0x80;	break;
		case CCC_FOCUS_NEAR:		command1 = 0x01;	break;
		// Iris Commands
		case CCC_IRIS_AUTO:								break;
		case CCC_IRIS_OPEN:			command1 = 0x02;	break;
		case CCC_IRIS_CLOSE:		command1 = 0x04;	break;
		// Zoom Commands
		case CCC_ZOOM_IN:			command2 = 0x20;	break;
		case CCC_ZOOM_OUT:			command2 = 0x40;	break;
		// Pan Commands
		case CCC_PAN_LEFT:			command2 = 0x04; data2 = 0;	break;
		case CCC_PAN_RIGHT:			command2 = 0x02; data2 = 0; break;
		// Tilt Commands
		case CCC_TILT_UP:			command2 = 0x08; data1 = 0; break;
		case CCC_TILT_DOWN:			command2 = 0x10; data1 = 0; break;
		// Move Commands
		case CCC_MOVE_LEFT_DOWN:	command2 = 0x14; data2 >>= 1; break;
		case CCC_MOVE_LEFT_UP:		command2 = 0x0c; data2 >>= 1; break;
		case CCC_MOVE_RIGHT_DOWN:	command2 = 0x12; data2 >>= 1; break;
		case CCC_MOVE_RIGHT_UP:		command2 = 0x0a; data2 >>= 1; break;
		// Turn 180 degree
		case CCC_TURN_180:			command2 = 0x07; data2 = 0x21; data1 = 0; break;
		// Stop Commands
		case CCC_MOVE_STOP:
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_STOP:
		case CCC_IRIS_STOP:
			bRepeat = FALSE;
			data1 = 0;	// pan speed
			data2 = 0;	// tilt speed
			break;
		default:
			bTelemetryCommand = FALSE;
			bRepeat = FALSE;
			data1 = 0;	// pan speed
			data2 = 0;	// tilt speed
			break;
	}
	if (!bTelemetryCommand)
	{
		BOOL bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_HOME:	data2 = 0x20; break;
			case CCC_POS_1:		data2 = 1; break;
			case CCC_POS_2:		data2 = 2; break;
			case CCC_POS_3:		data2 = 3; break;
			case CCC_POS_4:		data2 = 4; break;
			case CCC_POS_5:		data2 = 5; break;
			case CCC_POS_6:		data2 = 6; break;
			case CCC_POS_7:		data2 = 7; break;
			case CCC_POS_8:		data2 = 8; break;
			case CCC_POS_9:		data2 = 9; break;
			case CCC_POS_X:		data2 = (BYTE)HIWORD(dwValue);
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			switch (LOWORD(dwValue))
			{
				case 0: command2 = 0x07; break;	// go to preset
				case 1: command2 = 0x03; break;	// set preset
				case 2: command2 = 0x05; break;	// clear preset
			}
		}
		else
		{
			if (cmd == CCC_AUXOUT)
			{
				data2 = (BYTE)LOWORD(dwValue);				// Output Number (1,...,8)
				command2 = HIWORD(dwValue) ? 0x09 : 0x0b;	// On, Off
			}
			else
			{
				WK_TRACE(_T("Unknown Command for %s Protokoll\n"), NameOfEnum(GetType()));
				// Zero Pan Position
				// Auto Scan
				// Stop Auto Scan
				// Remote Reset
				// Zone Start
				// Zone End
				// Write char to screen
				// Alarm Ack
				// Zone Scan On
				// Pattern Start
				// Pattern Stop
				// Run Pattern
				// Zoom Lens Speed
				// Focus Lens Speed
			}
		}
	}

	CCommandRecord* pNewCommand = new CCommandRecord;
	pNewCommand->Add(0xff);			// 1: Synchronisation command

	pNewCommand->Add(byID);			// 2: camera ID
	pNewCommand->Add(command1);		// 3: command 1
	pNewCommand->Add(command2);		// 4: command 2
	pNewCommand->Add(data1);		// 5: pan speed
	pNewCommand->Add(data2);		// 6: tilt speed
//		BYTE bCheckSum = (BYTE)(((WORD)byID + (WORD)command1 + (WORD)command2 + (WORD)data1 + (WORD)data2));
	BYTE bCheckSum = byID + command1 + command2 + data1 + data2;
	pNewCommand->Add(bCheckSum);	// 7: checksum
	pNewCommand->m_bRepeatAllowed = bRepeat;

	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::CreateNewCmdPelcoP(BYTE byID, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	DWORD	wSpeedX = min(10, LOWORD(dwValue)); // 1 - 10
	DWORD	wSpeedY = min(10, HIWORD(dwValue)); // 1 - 10
	BYTE	command1 = 0, command2 = 0, data1 = 0, data2 = 0, byCheckSum;
	BOOL    bTelemetryCommand = TRUE;
	if (!wSpeedY) wSpeedY = wSpeedX;
	data1 = (BYTE)MulDiv(wSpeedX, 0x40, 10);
	data2 = (BYTE)MulDiv(wSpeedY, 0x3f, 10);

	BOOL bRepeat = m_bRepeat;
	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_AUTO:							break;
		case CCC_FOCUS_FAR:			command1 = 0x01;	break;
		case CCC_FOCUS_NEAR:		command1 = 0x02;	break;
		// Iris Commands
		case CCC_IRIS_AUTO:								break;
		case CCC_IRIS_OPEN:			command1 = 0x04;	break;
		case CCC_IRIS_CLOSE:		command1 = 0x08;	break;
		// Zoom Commands
		case CCC_ZOOM_IN:			command2 = 0x20;	break;
		case CCC_ZOOM_OUT:			command2 = 0x40;	break;
		// Pan Commands
		case CCC_PAN_LEFT:			command2 = 0x04; data2 = 0;	break;
		case CCC_PAN_RIGHT:			command2 = 0x02; data2 = 0; break;
		// Tilt Commands
		case CCC_TILT_UP:			command2 = 0x08; data1 = 0; break;
		case CCC_TILT_DOWN:			command2 = 0x10; data1 = 0; break;
		// Move Commands
		case CCC_MOVE_LEFT_DOWN:	command2 = 0x14; data2 >>= 1; break;
		case CCC_MOVE_LEFT_UP:		command2 = 0x0c; data2 >>= 1; break;
		case CCC_MOVE_RIGHT_DOWN:	command2 = 0x12; data2 >>= 1; break;
		case CCC_MOVE_RIGHT_UP:		command2 = 0x0a; data2 >>= 1; break;
		// Turn 180 degree
		case CCC_TURN_180:			command2 = 0x07; data1 = 0; data2 = 0x21; break;
		// Stop Commands
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_MOVE_STOP:
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_STOP:
		case CCC_IRIS_STOP:
			bRepeat = FALSE;
			data1 = 0;	// pan speed
			data2 = 0;	// tilt speed
			break;
		default:
			bTelemetryCommand = FALSE;
			bRepeat = FALSE;
			data1 = 0;	// pan speed
			data2 = 0;	// tilt speed
			break;
	}
	if (!bTelemetryCommand)
	{
		BOOL bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_HOME:	data2 = 0xff; break;
			case CCC_POS_1:		data2 = 1; break;
			case CCC_POS_2:		data2 = 2; break;
			case CCC_POS_3:		data2 = 3; break;
			case CCC_POS_4:		data2 = 4; break;
			case CCC_POS_5:		data2 = 5; break;
			case CCC_POS_6:		data2 = 6; break;
			case CCC_POS_7:		data2 = 7; break;
			case CCC_POS_8:		data2 = 8; break;
			case CCC_POS_9:		data2 = 9; break;
			case CCC_POS_X:		data2 = (BYTE)HIWORD(dwValue);
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			switch (LOWORD(dwValue))
			{
				case 0: command2 = 0x07; break;	// go to preset
				case 1: command2 = 0x03; break;	// set preset
				case 2: command2 = 0x05; break;	// clear preset
			}
		}
		else
		{
			// TODO! RKE: implement extended commands
			// Zero Pan Position
			// Auto Scan
			// Stop Auto Scan
			// Remote Reset
			// Zone Start
			// Zone End
			// Write char to screen
			// Alarm Ack
			// Zone Scan On
			// Pattern Start
			// Pattern Stop
			// Run Pattern
			// Zoom Lens Speed
			// Focus Lens Speed
			WK_TRACE(_T("Unknown Command for %s Protokoll\n"), NameOfEnum(GetType()));
		}
	}

	byID--;							// zero based index for camera ID
	byCheckSum	 = 0xA0;
	byCheckSum  ^= byID;			// XOR sum of byte 1 to byte 7
	byCheckSum	^= command1;
	byCheckSum	^= command2;
	byCheckSum	^= data1;
	byCheckSum	^= data2;
	byCheckSum	^= 0xAF;

	CCommandRecord* pNewCommand = new CCommandRecord;
	pNewCommand->Add(0xA0);			// 1:STX Start Transmission
	pNewCommand->Add(byID);			// 2:camera ID
	pNewCommand->Add(command1);		// 3:command 1
	pNewCommand->Add(command2);		// 4:command 2
	pNewCommand->Add(data1);		// 5:pan speed
	pNewCommand->Add(data2);		// 6:tilt speed
	pNewCommand->Add(0xAF);			// 7:ETX End Transmision
	pNewCommand->Add(byCheckSum);	// 8:checksum

	pNewCommand->m_bRepeatAllowed = bRepeat;

	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::CreateNewCmdBBVRS422(BYTE byID, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	DWORD	wSpeedX = min(10, LOWORD(dwValue)); // 1 - 10
	DWORD	wSpeedY = min(10, HIWORD(dwValue)); // 1 - 10
	BYTE	byte1 = 0xB0, byte3 = 0, byte4 = 0, byte5 = 0, byte6 = 0, byte7 = 0xBF, byCheckSum;
	BOOL    bTelemetryCommand = TRUE;
	if (!wSpeedY) wSpeedY = wSpeedX;
	byte5 = (BYTE)MulDiv(wSpeedX, 0x40, 10);
	byte6 = (BYTE)MulDiv(wSpeedY, 0x3f, 10);

	BOOL bRepeat = m_bRepeat;	// no repeat 
	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_AUTO:						break;
		case CCC_FOCUS_FAR:			byte4 = 0x02;	break;
		case CCC_FOCUS_NEAR:		byte3 = 0x01;	break;
		// Iris Commands
		case CCC_IRIS_AUTO:							break;
		case CCC_IRIS_OPEN:			byte3 = 0x04;	break;
		case CCC_IRIS_CLOSE:		byte3 = 0x08;	break;
		// Zoom Commands
		case CCC_ZOOM_IN:			byte4 = 0x20;	break;
		case CCC_ZOOM_OUT:			byte4 = 0x40;	break;
		// Pan Commands
		case CCC_PAN_LEFT:			byte4 = 0x04; byte6 = 0; break;
		case CCC_PAN_RIGHT:			byte4 = 0x02; byte6 = 0; break;
		// Tilt Commands
		case CCC_TILT_UP:			byte4 = 0x08; byte5 = 0; break;
		case CCC_TILT_DOWN:			byte4 = 0x10; byte5 = 0; break;
		// Move Commands
		case CCC_MOVE_LEFT_DOWN:	byte4 = 0x14; byte6 >>= 1; break;
		case CCC_MOVE_LEFT_UP:		byte4 = 0x0c; byte6 >>= 1; break;
		case CCC_MOVE_RIGHT_DOWN:	byte4 = 0x12; byte6 >>= 1; break;
		case CCC_MOVE_RIGHT_UP:		byte4 = 0x0a; byte6 >>= 1; break;
		// Turn 180 degree
		case CCC_TURN_180:			byte4 = 0x07; byte6 = 0x21; byte5 = 0; break;
		// Stop Commands
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_MOVE_STOP:
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_STOP:
		case CCC_IRIS_STOP:
			bRepeat = FALSE;
			byte5 = 0;	// pan speed
			byte6 = 0;	// tilt speed
			break;
		default:
			bTelemetryCommand = FALSE;
			bRepeat = FALSE;
			byte5 = 0;	// pan speed
			byte6 = 0;	// tilt speed
			break;
	}
	if (!bTelemetryCommand)
	{
		BOOL bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_HOME:	byte6 = 0x20; break;
			case CCC_POS_1:		byte6 = 1; break;
			case CCC_POS_2:		byte6 = 2; break;
			case CCC_POS_3:		byte6 = 3; break;
			case CCC_POS_4:		byte6 = 4; break;
			case CCC_POS_5:		byte6 = 5; break;
			case CCC_POS_6:		byte6 = 6; break;
			case CCC_POS_7:		byte6 = 7; break;
			case CCC_POS_8:		byte6 = 8; break;
			case CCC_POS_9:		byte6 = 9; break;
			case CCC_POS_X:		byte6 = (BYTE)HIWORD(dwValue);
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			switch (LOWORD(dwValue))
			{
				case 0: byte4 = 0x07; break;	// go to preset
				case 1: byte4 = 0x03; break;	// set preset
				case 2: byte4 = 0x05; break;	// clear preset
			}
		}
		else
		{
			if (cmd == CCC_AUXOUT)
			{
				byte6 = (BYTE)LOWORD(dwValue);				// Output Number (1,...,8)
				byte4 = (BYTE)HIWORD(dwValue) ? 0x09 : 0x0b;	// On, Off
			}
			else
			{
				WK_TRACE(_T("Unknown Command for %s Protokoll\n"), NameOfEnum(GetType()));
				// Zero Pan Position
				// Auto Scan
				// Stop Auto Scan
				// Remote Reset
				// Zone Start
				// Zone End
				// Write char to screen
				// Alarm Ack
				// Zone Scan On
				// Pattern Start
				// Pattern Stop
				// Run Pattern
				// Zoom Lens Speed
				// Focus Lens Speed
			}
		}
	}

	byID--;							// zero based index for camera ID
	byCheckSum  = byte1;			// XOR checksum of byte 1 to byte 7
	byCheckSum ^= byID;
	byCheckSum ^= byte3;
	byCheckSum ^= byte4;
	byCheckSum ^= byte5;
	byCheckSum ^= byte6;
	byCheckSum ^= byte7;

	CCommandRecord* pNewCommand = new CCommandRecord;
	pNewCommand->Add(byte1);		// 1:STX Synchronisation command
	pNewCommand->Add(byID);			// 2:camera ID
	pNewCommand->Add(byte3);		// 3:command 1
	pNewCommand->Add(byte4);		// 4:command 2
	pNewCommand->Add(byte5);		// 5:pan speed
	pNewCommand->Add(byte6);		// 6:tilt speed
	pNewCommand->Add(byte7);		// 7:ETX
	pNewCommand->Add(byCheckSum);	// 8:checksum
	pNewCommand->m_bRepeatAllowed = bRepeat;

	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::CreateNewCmdSensormatic(int iCam, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	DWORD	dwSpeed = min(10, dwValue);	// 1 - 10
	BYTE	bCamID = (BYTE)m_camIDs[iCam];
	BYTE	byte1 = bCamID, byte2=0xc0, byte3 = 0, byte4 = 0;
	BYTE    wCheckSum;
	BOOL    bTelemetryCommand = TRUE;

	byte4 = (BYTE) MulDiv(dwSpeed, 110, 10)-10;

	if (CCommandRecord::IsTelemetryCommand(m_cccLast) && CCommandRecord::IsTelemetryCommand(cmd))
	{
		CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_MOVE_STOP, 0);
		CreateNewCmdSensormatic(iCam, &ccr1);
	}

	BOOL bRepeat = m_bRepeat;	// no repeat 
	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_FAR:			byte2 = 0x88;	break;
		case CCC_FOCUS_NEAR:		byte2 = 0x87;	break;
		// Iris Commands
		case CCC_IRIS_OPEN:			byte2 = 0x90;	break;
		case CCC_IRIS_CLOSE:		byte2 = 0x91;	break;
		// Zoom Commands
		case CCC_ZOOM_IN:			byte2 = 0x8a;	break;
		case CCC_ZOOM_OUT:			byte2 = 0x8b;	break;
		// Pan Commands
		case CCC_PAN_LEFT:			byte3 = 0x81;	break;
		case CCC_PAN_RIGHT:			byte3 = 0x82;	break;
		// Tilt Commands
		case CCC_TILT_UP:			byte3 = 0x84;	break;
		case CCC_TILT_DOWN:			byte3 = 0x85;	break;
		// Stop Commands
		case CCC_PAN_STOP:			byte2 = 0x83; bRepeat = FALSE;	break;
		case CCC_TILT_STOP:			byte2 = 0x86; bRepeat = FALSE;	break;
		case CCC_ZOOM_STOP:			byte2 = 0x8c; bRepeat = FALSE;	break;
		case CCC_FOCUS_STOP:		byte2 = 0x89; bRepeat = FALSE;	break;
		case CCC_IRIS_STOP:			byte2 = 0x92; bRepeat = FALSE;	break;
		case CCC_MOVE_STOP:			byte2 = 0x93; bRepeat = FALSE;	break;
		default:
			bTelemetryCommand = FALSE;
			bRepeat = FALSE;
			break;
	}

	if (!bTelemetryCommand)
	{
		BOOL bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_1:	byte2 = LOWORD(dwValue) == 0 ? 0xb4 : 0xa8; break;
			case CCC_POS_2:	byte2 = LOWORD(dwValue) == 0 ? 0xb5 : 0xa9; break;
			case CCC_POS_3:	byte2 = LOWORD(dwValue) == 0 ? 0xb6 : 0xaa; break;
			case CCC_POS_4:	byte2 = LOWORD(dwValue) == 0 ? 0xb7 : 0xab; break;
			case CCC_POS_5:	byte2 = LOWORD(dwValue) == 0 ? 0xbc : 0xb9; break;
			case CCC_POS_6:	byte2 = LOWORD(dwValue) == 0 ? 0xbd : 0xba; break;
			case CCC_POS_7:	byte2 = LOWORD(dwValue) == 0 ? 0xbe : 0xbb; break;
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			switch (LOWORD(dwValue))
			{
				case 0: byte4 = 0x07; break;	// go to preset
				case 1: byte4 = 0x03; break;	// set preset
			}
		}
		else
		{
			if (cmd == CCC_AUXOUT)
			{
				DWORD dwOutputFlag = 1 << (LOWORD(dwValue)-1);
				if (HIWORD(dwValue))
				{
					m_Flags[iCam] |= dwOutputFlag;
				}
				else
				{
					m_Flags[iCam] &= ~dwOutputFlag;
				}
				byte2 = (BYTE)(0xe0|(m_Flags[iCam]& 0x0000000f));// Output Bits (0 - 3)
			}
			else
			{
				BOOL bCombinedCmds = TRUE;
				switch (cmd)
				{
					case CCC_TURN_180:
					{
						CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), (CameraControlCmd)0x108d, 0);	// Fast
						CCameraCommandRecord ccr2(pCCommandR->GetCommID(), pCCommandR->GetCamID(), (CameraControlCmd)0x108e, 0);	// Fastest
						CreateNewCmdSensormatic(iCam, &ccr1);
						m_cccLast = CCC_INVALID;
						CreateNewCmdSensormatic(iCam, &ccr2);
						byte2 = 0x8f;	// fast stop
					}	break;
					case CCC_FOCUS_AUTO: case CCC_IRIS_AUTO:
					{
						CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_IRIS_OPEN, 0);
						CCameraCommandRecord ccr2(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_IRIS_CLOSE, 0);
						CreateNewCmdSensormatic(iCam, &ccr1);
						m_cccLast = CCC_INVALID;
						CreateNewCmdSensormatic(iCam, &ccr2);
						byte2 = 0x92;	// Iris Stop
					}	break;
					case CCC_MOVE_LEFT_DOWN:
					{
						CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_LEFT, dwValue);
						CreateNewCmdSensormatic(iCam, &ccr1);
						byte3 = 0x85;	// Tilt down
						byte4 >>= 1;	// half speed
					}	break;
					case CCC_MOVE_LEFT_UP:
					{
						CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_LEFT, dwValue);
						CreateNewCmdSensormatic(iCam, &ccr1);
						byte3 = 0x84;	// Tilt up
						byte4 >>= 1;	// half speed
					}	break;
					case CCC_MOVE_RIGHT_DOWN:
					{
						CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_RIGHT, dwValue);
						CreateNewCmdSensormatic(iCam, &ccr1);
						byte3 = 0x85;	// Tilt down
						byte4 >>= 1;	// half speed
					}	break;
					case CCC_MOVE_RIGHT_UP:
					{
						CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_RIGHT, dwValue);
						CreateNewCmdSensormatic(iCam, &ccr1);
						byte3 = 0x84;	// tilt up
						byte4 >>= 1;	// half speed
					}	break;
					case CCC_CONFIG:
					{
						switch (dwValue)
						{
							case PTZF_CONFIG_PANASONIC_ON:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_IRIS_OPEN, 0);
								CCameraCommandRecord ccr2(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_FOCUS_FAR, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								m_cccLast = CCC_INVALID;
								CreateNewCmdSensormatic(iCam, &ccr2);
								byte2 = 0x8b;	// ZoomOut
							}	break;
							case PTZF_CONFIG_PANASONIC_OFF:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_IRIS_CLOSE, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x87;	// Focus Near
							}	break;
							case PTZF_CONFIG_PANASONIC_UP:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_TILT_UP, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x93;	// Stop All
							}	break;
							case PTZF_CONFIG_PANASONIC_DOWN:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_TILT_DOWN, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x93;	// Stop All
							}	break;
							case PTZF_CONFIG_PANASONIC_LEFT:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_LEFT, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x93;	// Stop All
							}	break;
							case PTZF_CONFIG_PANASONIC_RIGHT:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_RIGHT, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x93;	// Stop All
							}	break;
							case PTZF_CONFIG_PANASONIC_SET1:	// Start Tour 1
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_FOCUS_FAR, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x93;	// Stop All
							}	break;
							case PTZF_CONFIG_PANASONIC_SET2:
								// not used
								break;
							case PTZF_CONFIG_PANASONIC_RESET:
								// not used
								break;
							case PTZF_CONFIG_PANASONIC_ALLRESET:
								// not used
								break;
							case PTZF_CONFIG_SET3:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_ZOOM_IN, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x93;	// Stop All
							}	break;
							case PTZF_CONFIG_SET4:
							{
								CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_ZOOM_OUT, 0);
								CreateNewCmdSensormatic(iCam, &ccr1);
								byte2 = 0x93;	// Stop All
							}	break;
						}
					}
					default: bCombinedCmds = FALSE; break;
				}
				if (bCombinedCmds)
				{
				}
				else if (cmd > (CameraControlCmd)0x1000)
				{
					byte2 = (BYTE)cmd;
				}
				else
				{
					WK_TRACE(_T("Unknown Command for %s Protokoll\n"), NameOfEnum(GetType()));
				}
			}
		}
	}

	CCommandRecord* pNewCommand = new CCommandRecord;
	wCheckSum = 0;
	pNewCommand->Add(byte1);		// 1:camID
	if (byte2 == 0xc0)				// 5 byte command
	{
		if (byte4 <= 0) byte4 = 1;	// speed shall not be 0
		pNewCommand->Add(byte2);	// 2: id for 5 byte command
		pNewCommand->Add(byte3);	// 3: command
		pNewCommand->Add(byte4);	// 4: speed
	}
	else							// 3 byte command
	{
		pNewCommand->Add(byte2);	// 2: command
	}

	for (int i=0; i<pNewCommand->GetCount(); i++)
	{
		wCheckSum = wCheckSum + pNewCommand->GetAtFast(i);
	}
	wCheckSum = 0x00 - wCheckSum;
	pNewCommand->Add((BYTE)wCheckSum);	// checksum
	pNewCommand->m_bRepeatAllowed = bRepeat;

	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);

	m_cccLast = cmd;
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd CControlRecordUni1::CreateNewCmdCbcNaf27(int nCam, CCameraCommandRecord* pCCommandR)
{
	static const BYTE    bFocusFar[10]	= {0x72,0x74,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x70};
	static const BYTE    bFocusNear[10]	= {0x73,0x75,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x71};
	static const BYTE    bZoomIn[3]		= {0x68,0x7a,0x69};
	static const BYTE    bZoomOut[3]	= {0x6a,0x7e,0x6b};

	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	WORD    wCamID	= (WORD)m_camIDs[nCam];
	int		nFocusSpeed = min(10, dwValue);		// 1 - 10
	BYTE	byte1=0xc5,							// command comes from PC
			byCode1=0x5f,						// key action
			byCode2= 0,							// key code
			byCode3= 0,							// data/command
			byCamid = (BYTE)wCamID;				// camerea ID zero based
	WORD    wCheckSum;							// CHECKSUM
	BOOL    bTelemetryCommand = TRUE;
	CameraControlCmd nReturn = CCC_INVALID;

	if (nFocusSpeed > 0) nFocusSpeed--;
	int nZoomSpeed = nFocusSpeed / 3;
	if (nZoomSpeed > 0) nZoomSpeed--;
//	byte4 = (BYTE) MulDiv(dwSpeed, 110, 10)-10;
	BOOL bRepeat = m_bRepeat;	// no repeat 
	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_FAR:			byCode2 = bFocusFar[nFocusSpeed];	break;
		case CCC_FOCUS_NEAR:		byCode2 = bFocusNear[nFocusSpeed];	break;
		// Iris Commands
		case CCC_IRIS_OPEN:			byCode2 = 0x42;	break;
		case CCC_IRIS_CLOSE:		byCode2 = 0x43;	break;
		// Zoom Commands
		case CCC_ZOOM_IN:			byCode2 = bZoomIn[nZoomSpeed];	break;
		case CCC_ZOOM_OUT:			byCode2 = bZoomOut[nZoomSpeed];	break;
		// Stop Commands
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_STOP:
		case CCC_IRIS_STOP:			byCode2 = 0x0c;	break;
		default:
			bTelemetryCommand = FALSE;
			bRepeat = FALSE;
			break;
	}

	if (!bTelemetryCommand)
	{
		switch (cmd)
		{
			case CCC_FOCUS_AUTO:
				byCode2 = 0x05; break;
				break;
			case CCC_IRIS_AUTO:
				byCode1 = 0xAA;
				byCode2 = 0x60;
				if (m_Flags[nCam] & 0x01)
				{
					m_Flags[nCam] &= ~0x01;
					byCode3 = 0x00;
				}
				else
				{
					m_Flags[nCam] |= 0x01;
					byCode3 = 0x04;
				}
				break;

			// Config Menu
			case CCC_CONFIG:
			{
				switch (dwValue)
				{
					case PTZF_CONFIG_PANASONIC_ON:		byCode2 = 0x25;	break;
					case PTZF_CONFIG_PANASONIC_OFF:		byCode2 = 0x25;	break;
					case PTZF_CONFIG_PANASONIC_UP:		byCode2 = 0x4f;	break;
					case PTZF_CONFIG_PANASONIC_DOWN:	byCode2 = 0x50;	break;
					case PTZF_CONFIG_PANASONIC_LEFT:	byCode2 = 0x27;	break;
					case PTZF_CONFIG_PANASONIC_RIGHT:	byCode2 = 0x28;	break;
					case PTZF_CONFIG_SET3:				byCode2 = 0x27;	break;
					case PTZF_CONFIG_SET4:				byCode2 = 0x28;	break;
					case PTZF_CONFIG_PANASONIC_SET1:	// unused
					case PTZF_CONFIG_PANASONIC_SET2:
					case PTZF_CONFIG_PANASONIC_RESET:
					case PTZF_CONFIG_PANASONIC_ALLRESET:
						break;
				}

			}break;
			default:
				byCode2 = 0;
				break;
		}
		if (byCode2 && byCode1 == 0x5f) // Key Action needs stop command afterwards
		{
			nReturn = CCC_ZOOM_STOP;
		}
	}

	if (byCode2 == 0)
	{
		byCode2 = 0x0c; // KN_STOP
	}

	if (cmd > (CameraControlCmd)0x1000)
	{
		byCode2 = (BYTE)cmd;
	}


	CCommandRecord* pNewCommand = new CCommandRecord;
	wCheckSum = 0;
	pNewCommand->Add(byte1);
	pNewCommand->Add(byCode1);
	pNewCommand->Add(byCode2);
	pNewCommand->Add(byCode3);
	pNewCommand->Add(byCamid);

	for (int i=0; i<pNewCommand->GetCount(); i++)
	{
		wCheckSum = wCheckSum + pNewCommand->GetAtFast(i);
	}

	pNewCommand->Add((BYTE)wCheckSum);	// checksum
	pNewCommand->m_bRepeatAllowed = bRepeat;

	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);
	return nReturn;
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd CControlRecordUni1::CreateNewCmdVideotec(WORD wID, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd nReturn = CCC_INVALID;
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	WORD wSpeedX = min(10, LOWORD(dwValue));	// 1 - 10
	WORD wSpeedY = min(10, HIWORD(dwValue));	// 1 - 10
	if (!wSpeedY) wSpeedY = wSpeedX;
	BYTE	bySpeedTilt, bySpeedPan, byCamIdL=0, byCamIdH=0, byCommand=0, byParam=0xff;
	BOOL    bTelemetryCommand = TRUE;
	if (wID > 100)							// 100 - 999
	{
		byCamIdH = (BYTE)(230 + wID/100);	// 100´er
		byCamIdL = (BYTE)(wID%100);			// rest
	}
	else									// 1-99
	{
		byCamIdL = (BYTE)(128 + wID);
	}

	bySpeedPan   = (BYTE)(MulDiv(wSpeedX, 8, 10)-1);
	bySpeedTilt  = (BYTE)(MulDiv(wSpeedY, 8, 10)-1);
	bySpeedPan   = bySpeedPan << 4;

	BOOL bRepeat = m_bRepeat;	// no repeat
	BOOL bLastCmd = CCommandRecord::IsTelemetryCommand(m_cccLast);
	if (bLastCmd && CCommandRecord::IsTelemetryCommand(cmd) && m_cccLast != cmd)
	{
		CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), bLastCmd == TRUE ? CCC_MOVE_STOP : CCC_ZOOM_STOP, 0);
		CreateNewCmdVideotec(wID, &ccr1);
		Sleep(10);
	}

	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_FAR:			byCommand = 12; byParam = 1; break;
		case CCC_FOCUS_NEAR:		byCommand = 11;	byParam = 1; break;
		// Iris Commands
		case CCC_IRIS_OPEN:			byCommand =  7;	byParam = 1; break;
		case CCC_IRIS_CLOSE:		byCommand =  8;	byParam = 1; break;
		// Zoom Commands
		case CCC_ZOOM_IN:			byCommand =  9;	break;
		case CCC_ZOOM_OUT:			byCommand = 10;	break;
		// Pan Commands
		case CCC_PAN_LEFT:			byCommand = 23; byParam = bySpeedPan; break;
		case CCC_PAN_RIGHT:			byCommand = 19; byParam = bySpeedPan; break;
		// Tilt Commands
		case CCC_TILT_UP:			byCommand = 17; byParam = bySpeedTilt; break;
		case CCC_TILT_DOWN:			byCommand = 21; byParam = bySpeedTilt; break;
		// Move Commands
		case CCC_MOVE_LEFT_DOWN:	byCommand = 22; byParam = bySpeedPan|bySpeedTilt; break;
		case CCC_MOVE_LEFT_UP:		byCommand = 24; byParam = bySpeedPan|bySpeedTilt; break;
		case CCC_MOVE_RIGHT_DOWN:	byCommand = 20; byParam = bySpeedPan|bySpeedTilt; break;
		case CCC_MOVE_RIGHT_UP:		byCommand = 18; byParam = bySpeedPan|bySpeedTilt; break;
		// Stop Commands
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_MOVE_STOP:
			byCommand = 25;				// Stop Command für pan, tilt
			byParam   = 0;	
			break;
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_STOP:
		case CCC_IRIS_STOP:
			bRepeat = FALSE;
			byCommand = 48;				// Stop Command für zoom, focus, iris
			break;
		case CCC_IRIS_AUTO:
			byCommand = 7;
			nReturn = (CameraControlCmd)(CCC_IRIS_AUTO+1000);
			break;
		case CCC_FOCUS_AUTO:						
			byCommand = 11;
			nReturn = (CameraControlCmd)(CCC_FOCUS_AUTO+1000);
			break;
		default:
			if ((int)cmd > 1000)
			{
				*((int*)&cmd) -= 1000;
				switch (cmd)
				{
					case CCC_IRIS_AUTO:  byCommand = 8; break;
					case CCC_FOCUS_AUTO: byCommand = 12; break;
				}
			}
			else
			{
				bTelemetryCommand = FALSE;
				bRepeat = FALSE;
			}
			break;
	}
	if (!bTelemetryCommand)
	{
		BOOL bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_1:		byParam = 1; break;
			case CCC_POS_2:		byParam = 2; break;
			case CCC_POS_3:		byParam = 3; break;
			case CCC_POS_4:		byParam = 4; break;
			case CCC_POS_5:		byParam = 5; break;
			case CCC_POS_6:		byParam = 6; break;
			case CCC_POS_7:		byParam = 7; break;
			case CCC_POS_8:		byParam = 8; break;
			case CCC_POS_9:		byParam = 9; break;
			case CCC_POS_X:		byParam = (BYTE)HIWORD(dwValue);
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			switch (LOWORD(dwValue))
			{
				case 0: byCommand = 15; break;	// go to preset
				case 1: byCommand = 16; break;	// set preset
//				case 2: byCommand = 0x05; break;// clear preset
			}
		}
		else
		{
			if (cmd == CCC_AUXOUT)
			{
				if (IsBetween(LOWORD(dwValue), 1, 4))
				{
					byCommand = (BYTE)LOWORD(dwValue); // Output Number (1,...,6)
				}
				else if (IsBetween(LOWORD(dwValue), 5, 6))
				{
					if (HIWORD(dwValue))
					{
						byCommand = (BYTE)LOWORD(dwValue); // Output Number (1,...,6)
					}
					else
					{
						byCommand = 48; // Stop
					}
				}
				else
				{
					WK_TRACE(_T("Invalid Output Number%d\n"), LOWORD(dwValue));
				}
			}
			else
			{
				WK_TRACE(_T("Unknown Command for %s Protokoll\n"), NameOfEnum(GetType()));
				// Zero Pan Position
				// Auto Scan
				// Stop Auto Scan
				// Remote Reset
				// Zone Start
				// Zone End
				// Write char to screen
				// Alarm Ack
				// Zone Scan On
				// Pattern Start
				// Pattern Stop
				// Run Pattern
				// Zoom Lens Speed
				// Focus Lens Speed
			}
		}
	}

	if (byCommand)
	{
		CCommandRecord* pNewCommand = new CCommandRecord;
		if (byCamIdH)					// Address
		{
			pNewCommand->Add(byCamIdH);
			pNewCommand->Add(byCamIdL);
		}
		else
		{
			pNewCommand->Add(byCamIdL);
		}
		pNewCommand->Add(byCommand);	// Command
		if (byParam != 0xff)			// optional
		{
			pNewCommand->Add(byParam);	// Parameter
		}

		pNewCommand->m_bRepeatAllowed = bRepeat;

		m_arrayCommand.SafeAdd(pNewCommand);
		TryToSendCommand(FALSE);
	}
	
	m_cccLast = cmd;

	return nReturn;
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd CControlRecordUni1::CreateNewCmdBewator(int nCam, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd nReturn = CCC_INVALID;
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	WORD    wID	= (WORD)m_camIDs.GetAt(nCam);
	WORD	wSpeedX = min(10, LOWORD(dwValue));	// 1 - 10
	WORD	wSpeedY = min(10, HIWORD(dwValue));	// 1 - 10
	BOOL    bTelemetryCommand = TRUE, bPresetCmd = FALSE, bOutputs = FALSE;;
	BYTE	byIDlo = 0, byIDhiAndCmd = 0, bySpeedPan = 0, bySpeedTilt = 0, byFunction1 = 0, byFunction2 = 0, byFunction3 = 0x00;

	bySpeedPan  = (BYTE)(MulDiv(wSpeedX, 127, 10));
	if (wSpeedY)
	{
		bySpeedTilt = (BYTE)(MulDiv(wSpeedY, 127, 10));
	}
	else
	{
		bySpeedTilt = bySpeedPan >> 1;	// half tilt speed because its half angle range
	}

	BOOL bRepeat = m_bRepeat;	// no repeat
	BOOL bLastCmd = CCommandRecord::IsTelemetryCommand(m_cccLast);
	BOOL bCurrent = CCommandRecord::IsTelemetryCommand(cmd);
	if (bLastCmd && bCurrent && m_cccLast != cmd)
	{
		CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_MOVE_STOP, 0);
		CreateNewCmdBewator(nCam, &ccr1);
		Sleep(10);
	}

	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_FAR:			byFunction1 = 0x80; bySpeedPan = 0; break;
		case CCC_FOCUS_NEAR:		byFunction2|= 0x01; bySpeedPan = 0; break;
		// Iris Commands
		case CCC_IRIS_OPEN:			byFunction2|= 0x04; bySpeedPan = 0; break;
		case CCC_IRIS_CLOSE:		byFunction2|= 0x02; bySpeedPan = 0; break;
		// Zoom Commands
		case CCC_ZOOM_IN:			byFunction1 = 0x20; bySpeedPan = 0; break;
		case CCC_ZOOM_OUT:			byFunction1 = 0x40; bySpeedPan = 0; break;
		// Pan Commands
		case CCC_PAN_LEFT:			byFunction1 = 0x04; break;
		case CCC_PAN_RIGHT:			byFunction1 = 0x02; break;
		// Tilt Commands
		case CCC_TILT_UP:			byFunction1 = 0x08; break;
		case CCC_TILT_DOWN:			byFunction1 = 0x10; break;
		// Move Commands
		case CCC_MOVE_LEFT_DOWN:	byFunction1 = 0x14; break;
		case CCC_MOVE_LEFT_UP:		byFunction1 = 0x0c; break;
		case CCC_MOVE_RIGHT_DOWN:	byFunction1 = 0x12; break;
		case CCC_MOVE_RIGHT_UP:		byFunction1 = 0x0a; break;
		// Stop Commands
		case CCC_PAN_STOP:
		case CCC_TILT_STOP:
		case CCC_MOVE_STOP:
		case CCC_ZOOM_STOP:
		case CCC_FOCUS_STOP:
		case CCC_IRIS_STOP:
			bRepeat = FALSE;
			bySpeedPan = 0;				// Stop Command für zoom, focus, iris
			break;
		case CCC_IRIS_AUTO:
			bySpeedPan = 0;
			if (m_Flags[nCam] & 0x08)		// defined as latched output
			{
				m_Flags[nCam] &= ~0x08;
			}
			else
			{
				m_Flags[nCam] |= 0x08;
			}
			break;
		case CCC_FOCUS_AUTO:			// defined as latched output		
			bySpeedPan = 0;
			if (m_Flags[nCam] & 0x40)
			{
				m_Flags[nCam] &= ~0x40;
			}
			else
			{
				m_Flags[nCam] |= 0x40;
			}
			break;
		case CCC_AUXOUT:
		{
			bySpeedPan = 0;
			switch (LOWORD(dwValue))
			{
				case 1: byFunction2 = 0x08; break;	// wash
				case 2: byFunction3 = 0x04; break;	// wipe
				case 3: byFunction3 = 0x10; break;	// light
				case 4: byFunction3 = 0x02; break;	// aux 1
				case 5: byFunction2 = 0x10;	break;	// aux 2
				case 6: byFunction3 = 0x80; break;	// aux 3
				default:
					bTelemetryCommand = FALSE;
					WK_TRACE(_T("Invalid Output Number%d\n"), LOWORD(dwValue));
					return nReturn;
			}
			if (bTelemetryCommand)
			{
				if (HIWORD(dwValue))	// on
				{
					if (byFunction3)
					{
						m_Flags[nCam] |= ((WORD)byFunction3);
					}
				}
				else					// off
				{
					byFunction2 = 0;
					if (byFunction3)
					{
						m_Flags[nCam] &= ~((WORD)byFunction3);
					}
				}
			}
		} break;
		default:
			bTelemetryCommand = FALSE;
			break;
	}

	if (bTelemetryCommand)
	{
		if (bySpeedPan)
		{
			byIDhiAndCmd = 0x0b; // functions and speed update
		}
		else
		{
			byIDhiAndCmd = 0x0b; // functions update
		}
		byFunction3 = (BYTE)m_Flags[nCam];	// latched output flags
	}
	else
	{
		bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_1:		byFunction2 = 1; break;
			case CCC_POS_2:		byFunction2 = 2; break;
			case CCC_POS_3:		byFunction2 = 3; break;
			case CCC_POS_4:		byFunction2 = 4; break;
			case CCC_POS_5:		byFunction2 = 5; break;
			case CCC_POS_6:		byFunction2 = 6; break;
			case CCC_POS_7:		byFunction2 = 7; break;
			case CCC_POS_8:		byFunction2 = 8; break;
			case CCC_POS_9:		byFunction2 = 9; break;
			case CCC_POS_X:		byFunction2 = (BYTE)HIWORD(dwValue);
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			byIDhiAndCmd = 0x00;
			switch (LOWORD(dwValue))
			{
				case 0: byFunction1 = 0x12; break;	// go to preset
				case 1: byFunction1 = 0x13; break;	// set preset
//				case 2: byCommand = 0x05; break;// clear preset
			}
		}
		else
		{
			WK_TRACE(_T("Unknown Command for %s Protokoll\n"), NameOfEnum(GetType()));
			// Zero Pan Position
			// Auto Scan
			// Stop Auto Scan
			// Remote Reset
			// Zone Start
			// Zone End
			// Write char to screen
			// Alarm Ack
			// Zone Scan On
			// Pattern Start
			// Pattern Stop
			// Run Pattern
			// Zoom Lens Speed
			// Focus Lens Speed
			return nReturn;
		}
	}

	// set the camera ID
	wID--;	// is sent zero based
	byIDlo = (BYTE)wID;
	if (wID > 0xff)
	{
		byIDhiAndCmd |= 0xa0;
	}
	else
	{
		byIDhiAndCmd |= 0x10;
	}

	CCommandRecord* pNewCommand = new CCommandRecord;
	pNewCommand->Add(byIDlo);		// ID
	pNewCommand->Add(byIDhiAndCmd);	// ID and cmd

	if (bTelemetryCommand)
	{
		pNewCommand->Add(byFunction1);	// home, PTZ, ff
		pNewCommand->Add(byFunction2);	// fn, ic, io, [w, a2]
		pNewCommand->Add(byFunction3);	// latched outputs
		if (bySpeedPan)
		{
			pNewCommand->Add(bySpeedPan);	// pan speed
			pNewCommand->Add(bySpeedTilt);	// tilt speed
		}
	}
	else if (bPresetCmd)
	{
		pNewCommand->Add(byFunction1);		// recall, store
		pNewCommand->Add(byFunction2);		// preset number
	}
	else if (bOutputs)
	{
		pNewCommand->Add(byFunction1);		// momentary
		pNewCommand->Add(byFunction2);		// latched
	}
	
	pNewCommand->m_bRepeatAllowed = bRepeat;
	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);

	m_cccLast = cmd;

	return nReturn;
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd	CControlRecordUni1::CreateNewCmdJvcTKC(int nCam, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd nReturn = CCC_INVALID;
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	WORD	wSpeedX = min(10, LOWORD(dwValue));	// 1 - 10
	WORD	wSpeedY = min(10, HIWORD(dwValue));	// 1 - 10
	BYTE	byEnq = 0x83, byCmd = 0x45, byItem = 0, byValue1 = 0, byValue2 = 0;
	BOOL    bTelemetryCommand = TRUE, bPresetCmd = FALSE, bOutputs = FALSE;;

	byValue1 = (BYTE)MulDiv(wSpeedX, 0x0f, 0x0a);
	if (wSpeedY)
	{
		byValue2 = (BYTE)MulDiv(wSpeedY, 0x0f, 0x0a);
	}
	else
	{
		byValue2 = byValue1 + 2;	// adjust tilt speed according to pan speed
	}

	if (byValue2 > 0x0f) byValue2 = 0x0f;
	byValue1 |= 0x10;
	byValue2 |= 0x10;

	BOOL bRepeat = m_bRepeat;	// no repeat
	BOOL bLastCmd = CCommandRecord::IsTelemetryCommand(m_cccLast);
	BOOL bCurrent = CCommandRecord::IsTelemetryCommand(cmd);
	if (bLastCmd && bCurrent && m_cccLast != cmd)
	{
		CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_MOVE_STOP, 0);
		CreateNewCmdJvcTKC(nCam, &ccr1);
		Sleep(10);
	}
 
	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_FAR:			byItem = 0x09; byValue1 = (BYTE)MulDiv(wSpeedX, 0x03, 0x0a); break;
		case CCC_FOCUS_NEAR:		byItem = 0x0a; byValue1 = (BYTE)MulDiv(wSpeedX, 0x03, 0x0a); break;
		// Iris Commands
		case CCC_IRIS_OPEN:			byEnq = 0x82; byItem = 0x06; break;
		case CCC_IRIS_CLOSE:		byEnq = 0x82; byItem = 0x07; break;
		// Zoom Commands
		case CCC_ZOOM_IN:			byItem = 0x0c; byValue1 = (BYTE)MulDiv(wSpeedX, 0x03, 0x0a); break;
		case CCC_ZOOM_OUT:			byItem = 0x0d; byValue1 = (BYTE)MulDiv(wSpeedX, 0x03, 0x0a); break;
		// Pan Commands
		case CCC_PAN_RIGHT:			byItem = 0x0;  break;
		case CCC_PAN_LEFT:			byItem = 0x01; break;
		// Tilt Commands
		case CCC_TILT_UP:			byItem = 0x03; break;
		case CCC_TILT_DOWN:			byItem = 0x04; break;
		// Move Commands
		case CCC_MOVE_LEFT_DOWN:
		{
			CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_LEFT, dwValue);
			CreateNewCmdJvcTKC(nCam, &ccr1);
			Sleep(10);
			byItem = 0x04;
			byValue1 = byValue2;
		}	break;
		case CCC_MOVE_LEFT_UP:		
		{
			CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_LEFT, dwValue);
			CreateNewCmdJvcTKC(nCam, &ccr1);
			Sleep(10);
			byItem = 0x03;
			byValue1 = byValue2;
		}	break;
		case CCC_MOVE_RIGHT_DOWN:
		{
            CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_RIGHT, dwValue);
			CreateNewCmdJvcTKC(nCam, &ccr1);
			Sleep(10);
			byItem = 0x04;
			byValue1 = byValue2;
		}	break;
		case CCC_MOVE_RIGHT_UP:
		{
			CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_RIGHT, dwValue);
			CreateNewCmdJvcTKC(nCam, &ccr1);
			Sleep(10);
			byItem = 0x03;
			byValue1 = byValue2;
		}	break;
		// Stop Commands
		case CCC_PAN_STOP:		byEnq = 0x82; byItem = 0x02; break;
		case CCC_TILT_STOP:		byEnq = 0x82; byItem = 0x05; break;
		case CCC_IRIS_STOP:		byEnq = 0x82; byItem = 0x08; break;
		case CCC_FOCUS_STOP:	byEnq = 0x82; byItem = 0x0b; break;
		case CCC_ZOOM_STOP:		byEnq = 0x82; byItem = 0x0e; break;
		case CCC_MOVE_STOP:		byEnq = 0x82; byItem = 0x0f; break;	// stop all
		case CCC_IRIS_AUTO:		byCmd = 0x42; byItem = dwValue ? 1 : 0; break;
		case CCC_FOCUS_AUTO:	byEnq = 0x82; byItem = 0x1f; break;
		default:
			bTelemetryCommand = FALSE;
			break;
	}
	if (!bTelemetryCommand)
	{
		bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_1:		byValue1 = 1; break;
			case CCC_POS_2:		byValue1 = 2; break;
			case CCC_POS_3:		byValue1 = 3; break;
			case CCC_POS_4:		byValue1 = 4; break;
			case CCC_POS_5:		byValue1 = 5; break;
			case CCC_POS_6:		byValue1 = 6; break;
			case CCC_POS_7:		byValue1 = 7; break;
			case CCC_POS_8:		byValue1 = 8; break;
			case CCC_POS_9:		byValue1 = 9; break;
			case CCC_POS_X:		byValue1 = (BYTE)HIWORD(dwValue); break;
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			switch (LOWORD(dwValue))
			{
				case 0: byCmd = 0x42; byItem = 0x0; break;	// go to preset
				case 1: byItem = 0x28; break;	// set preset
				case 2: byItem = 0x31; break;	// clear preset
			}
		}
		else if (cmd == CCC_AUXOUT)
		{
			bOutputs = TRUE;
			switch (LOWORD(dwValue))
			{
				case 1: byItem = 0x19; break;	// aux 1
				case 2: byItem = 0x1a;	break;	// aux 2
				case 3: byItem = 0x1b; break;	// aux 3
				default:
					bOutputs = FALSE;
					WK_TRACE(_T("Invalid Output Number%d\n"), LOWORD(dwValue));
					return nReturn;
			}
			byValue1 = HIWORD(dwValue) ? 1 : 0;
		}
		else if (cmd == CCC_CONFIG)
		{
			BOOL bRepeat = FALSE;
			switch (dwValue)
			{
				case PTZF_CONFIG_PANASONIC_ON:		byEnq = 0x82; byItem = 0x29; break;
				case PTZF_CONFIG_PANASONIC_OFF:		byEnq = 0x82; byItem = 0x29;
					if (m_Flags[nCam]-- > 0)
					{
						bRepeat = TRUE;
					}
					else
					{
						m_Flags[nCam] = 0;
					} break;
				case PTZF_CONFIG_PANASONIC_UP:		nReturn = CCC_TILT_UP;	 break;
				case PTZF_CONFIG_PANASONIC_DOWN:	nReturn = CCC_TILT_DOWN; break;
				case PTZF_CONFIG_PANASONIC_LEFT:	nReturn = CCC_PAN_LEFT;	 break;
				case PTZF_CONFIG_PANASONIC_RIGHT:	nReturn = CCC_PAN_RIGHT; break;
				case PTZF_CONFIG_PANASONIC_SET1:	// Return
					byEnq = 0x82; byItem = 0x2A; 
					m_Flags[nCam]++; break;
				case PTZF_CONFIG_PANASONIC_SET2:	// Esc
					if (m_Flags[nCam])
					{
						byEnq = 0x82; byItem = 0x29;
						m_Flags[nCam]--; 
					}break;
				case PTZF_CONFIG_SET3:				nReturn = CCC_ZOOM_IN; break;
				case PTZF_CONFIG_SET4:				nReturn = CCC_ZOOM_OUT;	break;
				case PTZF_CONFIG_PANASONIC_ALLRESET:byItem = 0x1d; byValue1 = 0x42; break;
				case PTZF_CONFIG_PANASONIC_RESET:	// invalid commands
					return nReturn;
			}
			if (nReturn != CCC_INVALID)
			{
				CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), nReturn, dwValue);
				CreateNewCmdJvcTKC(nCam, &ccr1);
				return CCC_MOVE_STOP;
			}
			if (bRepeat)
			{
				nReturn = cmd;
			}
		}
	}

	CCommandRecord* pNewCommand = new CCommandRecord;
// TODO! RKE: init start value
//	pNewCommand->Add(byIDlo);		// init

// TODO! RKE: select cam id, 
//	pNewCommand->Add(byIDlo);		// ID

	pNewCommand->Add(byEnq);
	pNewCommand->Add(byCmd);
	pNewCommand->Add(byItem);
	if (byEnq == 0x83)
	{
		pNewCommand->Add(byValue1);	// speed, preset position or cmd specific
	}

	pNewCommand->m_bRepeatAllowed = bRepeat;
	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);

	m_cccLast = cmd;

	return nReturn;
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd	CControlRecordUni1::CreateNewCmdMeridian(int nCam, CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd nReturn = CCC_INVALID;
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD	dwValue = pCCommandR->GetValue();
	WORD	wSpeedX = min(10, LOWORD(dwValue));	// 1 - 10
	WORD	wSpeedY = min(10, HIWORD(dwValue));	// 1 - 10
	BYTE	bStart	= 0x5b, // [
		    bStop	= 0x5d, // ]
			bAt		= 0x40, // @
			bFunction = 0,
			bFunctionEx = 0, 
			bAuxiliary  = 0;
	int		i;
	char	szText[16] = "";
	BOOL bRepeat = m_bRepeat;	// no repeat
/*
	// simultaneous operations are possible!?
	// this is not nessessary
	BOOL bLastCmd = CCommandRecord::IsTelemetryCommand(m_cccLast);
	BOOL bCurrent = CCommandRecord::IsTelemetryCommand(cmd);
	if (bLastCmd && bCurrent && m_cccLast != cmd)
	{
		CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_MOVE_STOP, 0);
		CreateNewCmdMeridian(nCam, &ccr1);
		Sleep(10);
	}
*/
	CCommandRecord* pNewCommand = new CCommandRecord;
	pNewCommand->Add(bStart);
	pNewCommand->Add(bAt);
	sprintf(szText, "%02d", m_camIDs.GetAt(nCam)); // 01-99, 100-999
	for (i=0; szText[i] != 0; i++)	pNewCommand->Add(szText[i]); // CamID

	BOOL    bTelemetryCommand = TRUE;
	switch (cmd)
	{
		// Focus Commands
		case CCC_FOCUS_FAR:		bAuxiliary = 'T'; bFunction = 'F'; break;
		case CCC_FOCUS_NEAR:	bAuxiliary = 'T'; bFunction = 'N'; break;
		// Iris Commands
		case CCC_IRIS_OPEN:		bAuxiliary = 'T'; bFunction = 'E'; break;
		case CCC_IRIS_CLOSE:	bAuxiliary = 'T'; bFunction = 'C'; break;
		// Zoom Commands
		case CCC_ZOOM_IN:		bAuxiliary = 'T'; bFunction = 'I'; wSpeedX /= 2; break;
		case CCC_ZOOM_OUT:		bAuxiliary = 'T'; bFunction = 'O'; wSpeedX /= 2; break;
		// Pan Commands
		case CCC_PAN_RIGHT:		bAuxiliary = 'T'; bFunction = 'R'; break;
		case CCC_PAN_LEFT:		bAuxiliary = 'T'; bFunction = 'L'; break;
		case CCC_TURN_180:		bAuxiliary = 'X'; bFunction = 'U'; break;
		// Tilt Commands
		case CCC_TILT_UP:		bAuxiliary = 'T'; bFunction = 'U'; break;
		case CCC_TILT_DOWN:		bAuxiliary = 'T'; bFunction = 'D'; break;
		// Move Commands
		case CCC_MOVE_LEFT_DOWN:	bAuxiliary = 'T'; bFunction = 'L'; bFunctionEx = 'D'; break;
		case CCC_MOVE_LEFT_UP:		bAuxiliary = 'T'; bFunction = 'L'; bFunctionEx = 'U'; break;
		case CCC_MOVE_RIGHT_DOWN:	bAuxiliary = 'T'; bFunction = 'R'; bFunctionEx = 'D'; break;
		case CCC_MOVE_RIGHT_UP:		bAuxiliary = 'T'; bFunction = 'R'; bFunctionEx = 'U'; break;
		// Stop Commands
		case CCC_PAN_STOP: case CCC_TILT_STOP: 
		case CCC_IRIS_STOP: case CCC_FOCUS_STOP: case CCC_ZOOM_STOP:
		case CCC_MOVE_STOP:  bFunction = '*'; break;	// stop all
		case CCC_IRIS_AUTO:	 bAuxiliary = 'X'; bFunction = 'C'; break;	// manual iris
//		case CCC_IRIS_AUTO:	 bAuxiliary = 'X'; bFunction = 'O'; break;	// manual iris
		case CCC_FOCUS_AUTO: break;
		default:
			bTelemetryCommand = FALSE;
			break;
	}
	if (bTelemetryCommand)
	{
		if (bAuxiliary)
		{
			pNewCommand->Add(bAuxiliary);
		}
		if (bFunction)
		{
			pNewCommand->Add(bFunction);
			if (wSpeedX)
			{
				WORD wCamSpeed = 5 + wSpeedX * 5;
 				sprintf(szText, "%02d", wCamSpeed); // 1,..,10 -> 10, 15,.., 55
				for (i=0; szText[i] != 0; i++)	pNewCommand->Add(szText[i]); // Speed
			}
		}

		if (bFunctionEx)
		{
			pNewCommand->Add(bFunctionEx);
			if (!wSpeedY)
			{
				// half angle speed ?
				wSpeedY = wSpeedX / 2;
				if (wSpeedY < 1) wSpeedY = 1;
			}

			if (wSpeedY)
			{
				WORD wCamSpeed = 5 + wSpeedY * 5; // 1,..,10 -> 10, 15,.., 55
 				sprintf(szText, "%02d", wCamSpeed); 
				for (i=0; szText[i] != 0; i++)	pNewCommand->Add(szText[i]); // Speed
			}
		}
	}
	else
	{
		WORD wPresetNumber = 0;
		BOOL bPresetCmd = TRUE;
		switch (cmd)
		{
			case CCC_POS_HOME:  wPresetNumber = 0; break;
			case CCC_POS_1:		wPresetNumber = 1; break;
			case CCC_POS_2:		wPresetNumber = 2; break;
			case CCC_POS_3:		wPresetNumber = 3; break;
			case CCC_POS_4:		wPresetNumber = 4; break;
			case CCC_POS_5:		wPresetNumber = 5; break;
			case CCC_POS_6:		wPresetNumber = 6; break;
			case CCC_POS_7:		wPresetNumber = 7; break;
			case CCC_POS_8:		wPresetNumber = 8; break;
			case CCC_POS_9:		wPresetNumber = 9; break;
			case CCC_POS_X:		wPresetNumber = (BYTE)HIWORD(dwValue); break;
			default: bPresetCmd = FALSE; break;
		}

		if (bPresetCmd)
		{
			switch (LOWORD(dwValue))
			{
				case 0: bFunction = 'L'; break;	// go to preset
				case 1: bFunction = 'P';break;	// set preset
				case 2: bAuxiliary = 'X'; bFunction = 'E'; break;	// clear preset
			}
			if (bAuxiliary)
			{
				pNewCommand->Add(bAuxiliary);
			}
			if (bFunction)
			{
				pNewCommand->Add(bFunction);
 				sprintf(szText, "%03d", wPresetNumber);  // preset Number
				for (i=0; szText[i] != 0; i++)	pNewCommand->Add(szText[i]);
// not nessessary, default settings are: 10 seconds dwell time, speed 3, patrol path 1 enabled, others disabled
//				if (bFunction == 'P')
//				{
//					sprintf(szText, "%03d%d", 5, 2);  // Dwell Time, Patrol Speed
//					for (i=0; szText[i] != 0; i++)	pNewCommand->Add(szText[i]);
//				}
			}
		}
		else
		{
			if (CCC_AUXOUT == cmd)
			{
				pNewCommand->Add('X');	// auxiliary command
				bFunction = HIWORD(dwValue) ? '2' : '3'; // Output Number 1 On, Off
				pNewCommand->Add(bFunction);
			}
			else if (CCC_CONFIG == cmd)
			{
				switch (dwValue)
				{
					case PTZF_CONFIG_PANASONIC_ON:		bFunction = 'O'; bFunctionEx = 'N'; break;
					case PTZF_CONFIG_PANASONIC_OFF:		bFunction = 'O'; bFunctionEx = 'F'; break;
					case PTZF_CONFIG_PANASONIC_UP:		bFunction = 'T'; bFunctionEx = 'U';	break;
					case PTZF_CONFIG_PANASONIC_DOWN:	bFunction = 'T'; bFunctionEx = 'D'; break;
					case PTZF_CONFIG_PANASONIC_LEFT:	bFunction = 'T'; bFunctionEx = 'L'; break;
					case PTZF_CONFIG_PANASONIC_RIGHT:	bFunction = 'T'; bFunctionEx = 'R'; break;
					case PTZF_CONFIG_PANASONIC_SET1:	bFunction = 'O'; bFunctionEx = 'W'; break; // Enter
					case PTZF_CONFIG_PANASONIC_SET2:	bFunction = 'O'; bFunctionEx = 'X'; break; // escape
					case PTZF_CONFIG_SET3:				bFunction = 'O'; bFunctionEx = 'Z';	break; // plus
					case PTZF_CONFIG_SET4:				bFunction = 'O'; bFunctionEx = 'Y'; break; // minus
					case PTZF_CONFIG_PANASONIC_RESET:   bFunction = 'O'; bFunctionEx = 'R'; break; // reset display
					case PTZF_CONFIG_PANASONIC_ALLRESET:
						break;
				}
				if (bFunction)
				{
					pNewCommand->Add(bFunction);
				}
				if (bFunctionEx)
				{
					pNewCommand->Add(bFunctionEx);
				}
			}
		}
	}

	pNewCommand->Add(bStop);
	pNewCommand->m_bRepeatAllowed = bRepeat;
	m_arrayCommand.SafeAdd(pNewCommand);
	TryToSendCommand(FALSE);

	m_cccLast = cmd;
	return nReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::OnReceivedData(LPBYTE pBuffer, DWORD dwLen)
{
	DWORD	dw = 0;
	BYTE	byte, bSTX = 0, bETX = 0;
	int		nCount = 0;
	switch (GetType())
	{
		case CCT_PELCO_D:
			bSTX = 0xff;
			nCount = 0;
			break;
		case CCT_PELCO_P:
			bSTX = 0xa0;
			bETX = 0xaf;
			break;
		case CCT_BBV_RS422:
			bSTX = 0xb0;
			bETX = 0xbf;
			break;
		case CCT_SENSORMATIC:
			bSTX = 0x00;
			nCount = 0;
			break;
		case CCT_CBC_ZCNAF27:
			bSTX = 0xc5;
			nCount = 6;
			break;
		case CCT_VIDEOTEC:
			bSTX = 0x00;
			nCount = 0;
			break;
		case CCT_BEWATOR:
			bSTX = 0x00;
			nCount = 0;
			break;
	}
	while (pBuffer && dw<dwLen)
	{
		if (m_byaReceived.GetSize() >= 60)
		{
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			// Fehler, zu viele Zeichen
			CString sMsg;
			WK_TRACE_ERROR(_T("ControlRecord %s COM%i Daten Len %i Buffer-Overrun, no ETX\n"),
							NameOfEnum(GetType()), GetCOMNumber(), m_byaReceived.GetSize());
			sMsg.Format( _T("Buffer-Overrun|COM%i"), GetCOMNumber() );
			WK_STAT_PEAK(_T("BufferOverrun"), 1, sMsg);
			m_byaReceived.RemoveAll();
		}

		byte = pBuffer[dw];
		// Wenn ein Startzeichen kommt, fangen die Daten erst richtig an
		if (bSTX && byte == bSTX)
		{
			ClearAllData();
			m_byaReceived.Add(byte); // add for easier handling
			m_nReceiveCount = nCount;
		}
		// Wenn ein Endezeichen kommt, ist ein Datensatz komplett
		else if (  (bETX && byte == bETX)
				|| (m_nReceiveCount && m_byaReceived.GetSize() == m_nReceiveCount))
		{
			m_byaReceived.Add(byte); // add for easier handling
			CheckData();
			m_nReceiveCount = 0;
		}
		else
		{
			m_byaReceived.Add(byte); 
		}
		dw++;
	} // while
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::OnTimeOutTransparent()
{
	m_csSendCommand.Lock();
	if (m_eStatus == WAITING_FOR_ACK)
	{
		CurrentCommandFinished(FALSE);
	}
	else
	{
		// valid state if nothing to do
	}
	m_csSendCommand.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::CurrentCommandFinished(BOOL bForceNextOne)
{
	DEBUG_TRACE(_T("ControlRecord %s COM=%d CurrentCommandFinished\n"), NameOfEnum(GetType()), GetCOMNumber());
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
void CControlRecordUni1::CheckData()
{
	int i, iLen = m_byaReceived.GetSize();
	// Check for valid destination (=our source) address
	if (iLen > 1)
	{
		TRACE(_T("Received(:"));
		for (i=0; i<iLen; i++)
		{
			TRACE(_T("%02x:"), m_byaReceived.GetAtFast(i));
		}
		TRACE(_T(")\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordUni1::TryToSendCommand(BOOL bForceRepeat)
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
			if (theApp.m_bTraceData)
			{
				theApp.TraceData(GetCOMNumber(), (void*)pData, nSize, FALSE);
			}
			Write((void*)pData, nSize);
			m_eStatus = WAITING_FOR_ACK;
		}
		else
		{
			WK_TRACE_WARNING(_T("ControlRecord %s COM=%d ; SendCommand without data\n"), 
				NameOfEnum(GetType()), GetCOMNumber());
		}
	}
}
