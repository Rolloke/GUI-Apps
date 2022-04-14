/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CameraControlRecord.cpp $
// ARCHIVE:		$Archive: /Project/Units/CommUnit/CameraControlRecord.cpp $
// CHECKIN:		$Date: 20.10.05 13:28 $
// VERSION:		$Revision: 56 $
// LAST CHANGE:	$Modtime: 20.10.05 13:26 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "CommUnit.h"
#include "CameraControlRecord.h"
#include "ControlRecordUni1.h"
#include "CameraCommandRecord.h"

// Positioning SENSO SAMPLE COM2 
// wrote 7 byte(s) 02 02 03 36 00 36 00 
// wrote 7 byte(s) 02 02 03 28 00 28 00 
/////////////////////////////////////////////////////////////////////////////
static BYTE byteSensorMatic[] = {0x4C, 0x4B, 0x4D, 0x4E, 0x4F, 0x50, 0x54, 0x55, 0x56,
							  0x43, 0x49, 0x4A, 0x52, 0x51, 0x53 };
/////////////////////////////////////////////////////////////////////////////
static BYTE bytePanasonic[][3] = {{'3','2','8'}, // 0 pan
								  {'3','2','C'}, // 1
								  {'3','2','4'}, // 2
								  {'3','2','A'}, // 3 tilt
								  {'3','2','E'}, // 4
								  {'3','2','4'}, // 5
								  {'2','2','E'}, // 6 focus
								  {'2','2','A'}, // 7
								  {'2','2','4'}, // 8
								  {'2','1','2'}, // 9 iris
								  {'2','1','3'}, //10
								  {'2','1','4'}, //11
								  {'2','2','8'}, //12 zoom
								  {'2','2','C'}, //13 
								  {'2','2','4'}, //14
								  {'3','2','9'}, //15 left up
								  {'3','2','B'}, //16 right up
								  {'3','2','F'}, //17 left down
								  {'3','2','D'}, //18 right down
								  {'3','2','4'}, //19 stop
								};
/////////////////////////////////////////////////////////////////////////////
static BYTE byteElbex[][2] = {	  {0x30,0x00}, // 0 pan left
								  {0x28,0x00}, // 1
								  {0x20,0x00}, // 2
								  {0x22,0x00}, // 3 tilt up
								  {0x24,0x00}, // 4
								  {0x20,0x00}, // 5
								  {0x20,0x20}, // 6 focus far
								  {0x20,0x40}, // 7
								  {0x20,0x00}, // 8
								  {0x20,0x08}, // 9 iris open
								  {0x20,0x10}, //10
								  {0x20,0x00}, //11
								  {0x21,0x00}, //12 zoom in
								  {0x20,0x80}, //13
								  {0x20,0x00}, //14
								  {0x32,0x00}, //15 Move left up
								  {0x2a,0x00}, //16 Move right up
								  {0x34,0x00}, //17 Move left down
								  {0x2c,0x00}, //18 Move right down
								  {0x20,0x00}, //19 Move stop
							 };
/////////////////////////////////////////////////////////////////////////////
static BYTE bytePieper[][2] = { {'L','1'},	// pan left
								{'R','1'},	// pan right
								{'L','0'},	// pan stop
								{'H','1'},	// tilt up
								{'T','1'},	// tilt down
								{'H','0'},	// tilt stop
								{'F','1'},	// focus far
								{'F','3'},	// focus near
								{'F','0'},	// focus stop
								{0,0},		// iris open not supported!
								{0,0},	// iris close not supported!
								{0,0},	// iris stop not supported!
								{'Z','1'},	// zoom in
								{'Z','3'},	// zoom out
								{'Z','0'},	// zoom stop
							   };
/////////////////////////////////////////////////////////////////////////////
static BYTE byteVidev[][3] = { {'S','0','2'},	// pan left
							   {'S','0','1'},// pan right
							   {'S','0','3'},// pan stop
							   {'N','0','1'}, // tilt up
							   {'N','0','2'},
							   {'N','0','3'},
							   {'F','0','1'}, // focus far
							   {'F','0','2'},
							   {'F','0','3'},
							   {'I','0','1'}, // iris open not supported !!
							   {'I','0','2'},
							   {'I','0','3'},
							   {'Z','0','1'}, // zoom in
							   {'Z','0','2'},
							   {'Z','0','3'},
							 };
/////////////////////////////////////////////////////////////////////////////
static BYTE byteCanon[][2] =  { {0x01,0x03},	// 0 pan left
								{0x02,0x03},	// 1 pan right
								{0x03,0x03},	// 2 pan stop
								{0x03,0x01},	// 3 tilt up
								{0x03,0x02},	// 4 tilt down
								{0x03,0x03},	// 5 tilt stop
								{0x08,0x02},	// 6 focus far
								{0x08,0x03},	// 7 focus near
								{0x08,0x00},	// 8 focus stop
								{0x0B,0x02},	// 9 iris open 
								{0x0B,0x03},	//10 iris close 
								{0x0B,0x00},	//11 iris stop 
								{0x07,0x02},	//12 zoom in
								{0x07,0x03},	//13 zoom out
								{0x07,0x00},	//14 zoom stop
								{0x01,0x01},	//15 Move left up
								{0x02,0x01},	//16 Move right up
								{0x01,0x02},	//17 Move left down
								{0x02,0x02},	//18 Move right down
								{0x03,0x03},	//19 Move stop
							   };
/////////////////////////////////////////////////////////////////////////////
static BYTE byteVCL[] =  {  {0x00},	// dummy for easier array access
							{0x4C},	// pan left
							{0x52},	// pan right
							{0x6C},	// pan stop
							{0x55},	// tilt up
							{0x4E},	// tilt down
							{0x6E},	// tilt stop
							{0x3D},	// focus far
							{0x3C},	// focus near
							{0x2C},	// focus stop
							{0x3E},	// iris open 
							{0x3F},	// iris close 
							{0x2E},	// iris stop 
							{0x3A},	// zoom in
							{0x3B},	// zoom out
							{0x2A},	// zoom stop
						 };
/////////////////////////////////////////////////////////////////////////////
CCameraControlRecord::CCameraControlRecord()
{
	m_pCom = NULL;
	m_bRun = FALSE;
	m_iBufferLength = 0;
	m_iRepeatTime = 1000;
	m_bRepeat = TRUE;
	m_pRepeatThread = NULL;
	m_pRepeatCmd = NULL;
	m_type = CCT_UNKNOWN;

	int i;
	for (i=0;i<_MAX_CAMERACOMMAND;i++)
	{
		m_CommandBuffer[i] = 0;
	}

	m_byMaxComLastCameraAddress = 0x00;	// invalid camera address
//	m_byBaxallLastCameraAddress = 0x00;	// invalid camera address
}
/////////////////////////////////////////////////////////////////////////////
CCameraControlRecord::~CCameraControlRecord()
{
	m_bRun = FALSE;
	WK_DELETE(m_pCom);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraControlRecord::Create(CameraControlType type, int iCom)
{
#ifdef _DEBUG
	if (m_type != CCT_UNKNOWN)
	{
		 ASSERT(m_type == type);
	}
#endif
	m_type = type;

	WK_TRACE(_T("CameraControl Typ=%s COM=%d\n"),NameOfEnum(m_type),iCom);

	m_pCom = new CRS232(iCom);

	switch (m_type)
	{
		case CCT_SENSORMATIC_TOUCHTRACKER:
			m_iBufferLength = 7;
			m_pCom->Open(m_type);
			m_pRepeatThread = NULL;
			break;
		case CCT_PANASONIC_WV_RM70:
			m_iBufferLength = 13;
			m_pCom->Open(m_type);
			if (m_pCom->IsOpen())
			{
				m_bRun = TRUE;
				m_bRepeat = FALSE;
				m_iRepeatTime = 900;
				m_pRepeatThread = AfxBeginThread(RepeatProc,(LPVOID)this);
			}
			break;
		case CCT_ERNITEC_BDR_510:
			m_iBufferLength = 6;
			m_iRepeatTime = 900;
			m_pCom->Open(m_type, CBR_2400);
			if (m_pCom->IsOpen())
			{
				m_bRun = TRUE;
				m_bRepeat = FALSE;
				m_pRepeatThread = AfxBeginThread(RepeatProc,(LPVOID)this);
			}
			break;
		case CCT_ELBEX_97_6:
			m_iBufferLength = 4;
			m_iRepeatTime = 20;
			m_pCom->Open(m_type, CBR_4800);
			if (m_pCom->IsOpen())
			{
				m_bRun = TRUE;
				m_bRepeat = FALSE;
				m_pRepeatThread = AfxBeginThread(RepeatProc,(LPVOID)this);
			}
			break;
		case CCT_PIEPER_KMS_850S:
			m_iBufferLength = 3;
			m_pCom->Open(m_type, CBR_4800,7,EVENPARITY,ONESTOPBIT);
			m_pRepeatThread = NULL;
			break;
		case CCT_VIDEV_EC160:
			m_iBufferLength = 7;
			m_pCom->Open(m_type, CBR_19200,8,NOPARITY,TWOSTOPBITS);
			m_pRepeatThread = NULL;
			break;
		case CCT_CANON_VISCA:
			m_iBufferLength = 9;
			m_pCom->Open(m_type);
			m_pRepeatThread = NULL;
			{
				// Clear command buffers in device
				// and stop instruction being executed
				m_CommandBuffer[0] = 0x88;
				m_CommandBuffer[1] = 0x01;
				m_CommandBuffer[2] = 0x00;
				m_CommandBuffer[3] = 0x01;
				m_CommandBuffer[4] = 0xFF;
				m_iBufferLength = 5;
				SendCommand();
				// give device some time to execute clear command
				Sleep(100);
				// set addresses for devices
				m_CommandBuffer[0] = 0x88;
				m_CommandBuffer[1] = 0x30;
				m_CommandBuffer[2] = 0x01;
				m_CommandBuffer[3] = 0xFF;
				m_iBufferLength = 4;
				SendCommand();
			}
			break;
		case CCT_VCL_CAMERA:
			m_iBufferLength = 3;
			m_iRepeatTime = 1000;
			m_pCom->Open(m_type, CBR_9600,8,NOPARITY,TWOSTOPBITS);
			if (m_pCom->IsOpen())
			{
				m_bRun = TRUE;
				m_bRepeat = FALSE;
				m_pRepeatThread = AfxBeginThread(RepeatProc,(LPVOID)this);
			}
			break;
		case CCT_VCL_MAXCOM:
			m_iBufferLength = 3;
			m_iRepeatTime = 1000;
			m_pCom->Open(m_type, CBR_9600,8,NOPARITY,TWOSTOPBITS);
			m_bRun = FALSE;
			m_bRepeat = FALSE;
			m_pRepeatThread = NULL;
			break;
/*		case CCT_FASTRAX:
			m_iBufferLength = 7;
			m_iRepeatTime = 1000;
			m_pCom->Open(m_type, CBR_9600,8,NOPARITY,ONESTOPBIT);
			if (m_pCom->IsOpen())
			{
				m_bRun = TRUE;
				m_bRepeat = FALSE;
				m_pRepeatThread = AfxBeginThread(RepeatProc,(LPVOID)this);
			}
			break;
		case CCT_BAXALL:
//			WK_TRACE(_T("CCT_BAXALL not yet implemented\n"));
			m_iBufferLength = 32;
			m_iRepeatTime = 1000;
			m_pCom->Open(m_type, CBR_9600,8,NOPARITY,ONESTOPBIT);
			m_bRun = FALSE;
			m_bRepeat = FALSE;
			m_pRepeatThread = NULL;
			break;
*/		case CCT_RS232_TRANSPARENT:
		{
			// RS232 transparent
			m_pCom->Open(m_type);
		}	break;
		default:
			// do nothing, m_pCom is not opened anyway
			WK_TRACE_ERROR(_T("Unknown CameraControl Typ=%s COM=%d\n"),NameOfEnum(m_type),iCom);
			break;
	}

	return m_pCom->IsOpen();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraControlRecord::Add(CameraControlType type, CSecID secID, DWORD camID)
{
	BOOL bOK = FALSE;
#ifdef _DEBUG
	if (m_type != CCT_UNKNOWN)
	{
		 ASSERT(m_type == type);
	}
#endif
	m_type = type;
	switch(type)
	{
		case CCT_SENSORMATIC_TOUCHTRACKER:
			bOK = TRUE;
			break;
		case CCT_PANASONIC_WV_RM70:
			if (IsBetween(camID, 1, 63))
			{
				bOK = TRUE;
			}
			break;
		case CCT_ERNITEC_BDR_510:
			if (IsBetween(camID, 1, 99))
			{
				bOK = TRUE;
			}
			break;
		case CCT_ELBEX_97_6:
			if (IsBetween(camID, 1, 999))
			{
				bOK = TRUE;
			}
			break;
		case CCT_PIEPER_KMS_850S:
//			if (IsBetween(camID, , ))	// TODO! RKE: Range
			{
				bOK = TRUE;
			}
			break;
		case CCT_VIDEV_EC160:
//			if (IsBetween(camID, , ))	// TODO! RKE: Range
			{
				bOK = TRUE;
			}
			break;
		case CCT_CANON_VISCA:
			if (IsBetween(camID, 1, 3))
			{
				bOK = TRUE;
			}
			break;
		case CCT_VCL_CAMERA:
		case CCT_VCL_MAXCOM:
			if (IsBetween(camID, 1, 128))
			{
				bOK = TRUE;
			}
			break;
		case CCT_FASTRAX:
			if (IsBetween(camID, 1, 100))
			{
				bOK = TRUE;
			}
			break;
	}
	if (bOK)
	{
		m_secIDs.Add(secID.GetID());
		m_camIDs.Add(camID);
		m_Flags.Add(0);
	}
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraControlRecord::HasSecID(CSecID secID)
{
	int i,c;

	c = m_secIDs.GetSize();
	for (i=0;i<c;i++)
	{
		if (secID == m_secIDs[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::Destroy()
{
	m_bRun = FALSE;
	m_bRepeat = FALSE;
	m_pCom->Close();
	if (m_pRepeatThread)
	{
		Sleep(m_iRepeatTime+100);
		m_pRepeatThread = NULL;
	}
	WK_DELETE(m_pCom);
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::OnNewCommand(CCameraCommandRecord* pCCommandR)
{
	if (theApp.m_bTraceCommand)
	{
		WK_TRACE(_T("OnNewCommand %s, %s, %lx, %d\n"),
				 NameOfEnum(m_type),
				 NameOfEnum(pCCommandR->GetCommand()),
				 pCCommandR->GetCamID().GetID(),
				 pCCommandR->GetValue());
	}
	CameraControlCmd nReturn = CCC_INVALID;

	switch (m_type)
	{
		case CCT_SENSORMATIC_TOUCHTRACKER:
			CommandBufferSensormaticTouchTracker(pCCommandR);
			break;
		case CCT_PANASONIC_WV_RM70:
			nReturn = CommandBufferPanasonicWV_RM70(pCCommandR);
			break;
		case CCT_ERNITEC_BDR_510:
			CommandBufferErnitecBDR510(pCCommandR);
			break;
		case CCT_ELBEX_97_6:
			CommandBufferElbex(pCCommandR);
			break;
		case CCT_PIEPER_KMS_850S:
			CommandBufferPieper(pCCommandR);
			break;
		case CCT_VIDEV_EC160:
			CommandBufferVidev(pCCommandR);
			break;
		case CCT_CANON_VISCA:
			CommandBufferCanon(pCCommandR);
			break;
		case CCT_VCL_CAMERA:
			CommandBufferVCLCamera(pCCommandR);
			break;
		case CCT_VCL_MAXCOM:
			CommandBufferVCLMaxCom(pCCommandR);
			break;
		case CCT_FASTRAX:
			CommandBufferFastrax(pCCommandR);
			break;
		default:
			WK_TRACE_ERROR(_T("OnNewCommand NOT OVERWRITTEN CameraControl Typ=%s COM=%d\n"),NameOfEnum(m_type),m_pCom->GetCOMNumber());
			ASSERT(0);
			break;
	}
	if (   m_type != CCT_VCL_MAXCOM
		&& m_type != CCT_BAXALL
		)
	{
		SendCommand();
	}
	if (nReturn != CCC_INVALID)
	{
		Sleep(100);
		CCameraCommandRecord *pccr = new CCameraCommandRecord(pCCommandR->GetCommID(), pCCommandR->GetCamID(), nReturn, pCCommandR->GetValue());
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CAMERA_COMMAND, (LPARAM)pccr);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::SendCommand()
{
	m_pCom->Write((void*)&m_CommandBuffer,m_iBufferLength);
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferSensormaticTouchTracker(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
//	DWORD dwValue = pCCommandR->GetValue();
	BYTE	bCommand = byteSensorMatic[((int)cmd)-1];
	WORD    wCheckSum;
	DWORD   dwSpeed = min(10,pCCommandR->GetValue());

	// start symbol
	m_CommandBuffer[0] = _STX_;
	// data length
	m_CommandBuffer[1] = 0x02;
	// end symbol
	m_CommandBuffer[2] = _ETX_;

	if (CCommandRecord::IsPositionCmd(cmd)> 0)
	{
		switch (cmd)
		{
		case CCC_POS_1:
			m_CommandBuffer[3] = 0x36;
			break;
		case CCC_POS_2:
			m_CommandBuffer[3] = 0x37;
			break;
		case CCC_POS_3:
			m_CommandBuffer[3] = 0x33;
			break;
		case CCC_POS_4:
			m_CommandBuffer[3] = 0x39;
			break;
		case CCC_POS_5:
			m_CommandBuffer[3] = 0x38;
			break;
		case CCC_POS_6:
			m_CommandBuffer[3] = 0x34;
			break;
		case CCC_POS_7:
			m_CommandBuffer[3] = 0x3A;
			break;
		case CCC_POS_8:
			m_CommandBuffer[3] = 0x3B;
			break;
		case CCC_POS_9:
			m_CommandBuffer[3] = 0x2E;
			break;
		}
		// check sum
		m_CommandBuffer[4] = 0x00;
		wCheckSum = (WORD)(m_CommandBuffer[3] + m_CommandBuffer[4]);
		m_CommandBuffer[5] = LOBYTE(wCheckSum);
		m_CommandBuffer[6] = HIBYTE(wCheckSum);
		SendCommand();

		m_CommandBuffer[3] = 0x28;
		m_CommandBuffer[4] = 0x00;
	}
	else
	{
		// data
		m_CommandBuffer[3] = bCommand;
		m_CommandBuffer[4] = (BYTE)(dwSpeed * 12);
	}
	// check sum
	wCheckSum = (WORD)(m_CommandBuffer[3] + m_CommandBuffer[4]);
	m_CommandBuffer[5] = LOBYTE(wCheckSum);
	m_CommandBuffer[6] = HIBYTE(wCheckSum);
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd CCameraControlRecord::CommandBufferPanasonicWV_RM70(CCameraCommandRecord* pCCommandR)
{
	CAutoCritSec acs(&m_csRepeat);
	CameraControlCmd cmd = pCCommandR->GetCommand(), cmdReturn = CCC_INVALID;
	DWORD dwValue = pCCommandR->GetValue();
	int c = ((int)cmd)-1;
	int b = 0;
	if (c > 14)
	{
		c -= 14;
	}

	if (cmd != 2001)	//	vari speed is part of the repeateable commands
	{
		m_bRepeat = FALSE;
		if (   m_pRepeatCmd					// previous cmd was PTZ...
			&& IsBetween(c, 0, 19)			// new cmd is PTZ...
			&& bytePanasonic[c][2] != '4'	// but no stop cmd
			&& m_pRepeatCmd->GetCommand() != cmd)
		{
			CCameraCommandRecord ccr1(pCCommandR->GetCommID(), pCCommandR->GetCamID(), CCC_PAN_STOP, 0);
			CommandBufferPanasonicWV_RM70(&ccr1);
			SendCommand();
			Sleep(10);
		}
		WK_DELETE(m_pRepeatCmd);
	}
	// start symbol
	m_CommandBuffer[b+0] = _STX_;

	// extract the cam id
	int i;
	BOOL bFound = FALSE;
	WORD id = 0x0001;

	for (i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		id = (WORD)m_camIDs[i];
		if (id!=0)
		{
			CString sF;
			sF.Format(_T("%02d"),id);

			if (sF.GetLength()==2)
			{
				m_CommandBuffer[b+1] = 'A';
				m_CommandBuffer[b+2] = 'D';
				m_CommandBuffer[b+3] = LOBYTE(sF[0]);
				m_CommandBuffer[b+4] = LOBYTE(sF[1]);
				m_CommandBuffer[b+5] = ';';
				b = 5;
				m_iBufferLength = 18;
			}
		}
	}

	// command
/*	if (cmd==CCC_CONFIG)
	{
		if (dwValue==PTZF_CONFIG_PANASONIC_ON)
		{
			m_CommandBuffer[b+1] = 'M';
			m_CommandBuffer[b+2] = 'S';
			m_CommandBuffer[b+3] = 'U';
			m_CommandBuffer[b+4] = ':';
			m_CommandBuffer[b+5] = '1';
			m_CommandBuffer[b+6] = _ETX_;
			m_iBufferLength = b+7;
			return;
		}
		else if (dwValue == PTZF_CONFIG_PANASONIC_OFF)
		{
			m_CommandBuffer[b+1] = 'M';
			m_CommandBuffer[b+2] = 'S';
			m_CommandBuffer[b+3] = 'U';
			m_CommandBuffer[b+4] = ':';
			m_CommandBuffer[b+5] = '0';
			m_CommandBuffer[b+6] = _ETX_;
			m_iBufferLength = b+7;
			return;
		}
		else
		{
			m_CommandBuffer[b+1] = 'D';
			m_CommandBuffer[b+2] = 'C';
			m_CommandBuffer[b+3] = 'R';
			m_CommandBuffer[b+4] = ':';
			switch (dwValue)
			{
			case PTZF_CONFIG_PANASONIC_UP:
				m_CommandBuffer[b+5] = 'U';
				break;
			case PTZF_CONFIG_PANASONIC_DOWN:
				m_CommandBuffer[b+5] = 'D';
				break;
			case PTZF_CONFIG_PANASONIC_LEFT:
				m_CommandBuffer[b+5] = 'L';
				break;
			case PTZF_CONFIG_PANASONIC_RIGHT:
				m_CommandBuffer[b+5] = 'R';
				break;
			case PTZF_CONFIG_PANASONIC_SET1:
				m_CommandBuffer[b+5] = 'A';
				break;
			case PTZF_CONFIG_PANASONIC_SET2:
				m_CommandBuffer[b+5] = 'B';
				break;
			}
			m_CommandBuffer[b+6] = _ETX_;
			m_iBufferLength = b+7;
			return;
		}
	}*/
	// command
	m_CommandBuffer[b+1] = 'G';
	m_CommandBuffer[b+2] = 'C';
	m_CommandBuffer[b+3] = '7';
	m_CommandBuffer[b+4] = ':';

	BOOL bPostionCmd = CCommandRecord::IsPositionCmd(cmd);
	if (bPostionCmd > 0)
	{
		if (dwValue == 1)				// setup start
		{
			m_CommandBuffer[b+5] = '2';
			m_CommandBuffer[b+6] = '0';
			m_CommandBuffer[b+7] = '2';
			m_CommandBuffer[b+8] = '1';
			m_CommandBuffer[b+9] = '5';
			m_CommandBuffer[b+10] = '4';
			m_CommandBuffer[b+11] = '0';
			m_CommandBuffer[b+12] = _ETX_;
			SendCommand();				// go to select position
		}
		else
		{
			m_CommandBuffer[b+5] = '2';
			m_CommandBuffer[b+6] = '0';
			m_CommandBuffer[b+7] = '2';
			m_CommandBuffer[b+8] = '1';
			m_CommandBuffer[b+9] = '4';
			m_CommandBuffer[b+10] = '0';
			m_CommandBuffer[b+11] = '0';
			m_CommandBuffer[b+12] = _ETX_;
			SendCommand();				// go to select position
		}
	}
	else if (cmd == 2001)				// vari speed
	{
		dwValue = MulDiv(dwValue, 8, 10)-1;
		m_CommandBuffer[b+5] = '2';
		m_CommandBuffer[b+6] = '0';
		m_CommandBuffer[b+7] = '2';
		m_CommandBuffer[b+8] = '2';
		m_CommandBuffer[b+9] = (BYTE)('0' + dwValue);
		m_CommandBuffer[b+10] = (BYTE)('0' + dwValue);
		m_CommandBuffer[b+11] = '0';
		m_CommandBuffer[b+12] = _ETX_;
		return cmdReturn;				// no more commands (CCC_INVALID)
	}
	else if (cmd == 2000)				// setup end
	{
		m_CommandBuffer[b+5] = '2';
		m_CommandBuffer[b+6] = '0';
		m_CommandBuffer[b+7] = '2';
		m_CommandBuffer[b+8] = '1';
		m_CommandBuffer[b+9] = '5';
		m_CommandBuffer[b+10] = '4';
		m_CommandBuffer[b+11] = '3';
		m_CommandBuffer[b+12] = _ETX_;
		return cmdReturn;				// no more commands (CCC_INVALID)
	}
	else if (cmd > 1000)				// position set done
	{
		m_CommandBuffer[b+5] = '2';
		m_CommandBuffer[b+6] = '0';
		m_CommandBuffer[b+7] = '2';
		m_CommandBuffer[b+8] = '1';
		m_CommandBuffer[b+9] = '5';
		m_CommandBuffer[b+10] = '4';
		m_CommandBuffer[b+11] = '2';
		m_CommandBuffer[b+12] = _ETX_;
		return (CameraControlCmd)2000;	// go to setup end
	}

	// command
	if (cmd==CCC_CONFIG)
	{
		m_CommandBuffer[b+5] = '0';
		m_CommandBuffer[b+6] = '0';
		m_CommandBuffer[b+7] = '2';
		m_CommandBuffer[b+8] = '1';
		m_CommandBuffer[b+9] = '9';
		m_CommandBuffer[b+10] = '4';
	}
	else
	{
		m_CommandBuffer[b+5] = '2';
		m_CommandBuffer[b+6] = '0';
		m_CommandBuffer[b+7] = '2';
	}

	if (bPostionCmd)					// select position 1,...,9, home
	{
		if (bPostionCmd == -1)			// home 
		{
			m_CommandBuffer[b+8]  = '1';
			m_CommandBuffer[b+9]  = '4';
			m_CommandBuffer[b+10] = '1';
			m_CommandBuffer[b+11] = '0';
		}
		else
		{
			m_CommandBuffer[b+8] = '2';
			m_CommandBuffer[b+9] = '0';
			m_CommandBuffer[b+10] = (BYTE)('0' + (bPostionCmd-1));
			m_CommandBuffer[b+11] = '0';
		}
		if (dwValue == 1)				// go to position set done
		{
			cmdReturn = (CameraControlCmd)(bPostionCmd + 1000);
		}
	}
	else if (cmd==CCC_CONFIG)
	{
		switch (dwValue)
		{
		case PTZF_CONFIG_PANASONIC_ON:
			m_CommandBuffer[b+11] = '0';
			break;
		case PTZF_CONFIG_PANASONIC_OFF:
			m_CommandBuffer[b+11] = '1';
			break;
		case PTZF_CONFIG_PANASONIC_UP:
			m_CommandBuffer[b+11] = '2';
			break;
		case PTZF_CONFIG_PANASONIC_DOWN:
			m_CommandBuffer[b+11] = '4';
			break;
		case PTZF_CONFIG_PANASONIC_LEFT:
			m_CommandBuffer[b+11] = '5';
			break;
		case PTZF_CONFIG_PANASONIC_RIGHT:
			m_CommandBuffer[b+11] = '3';
			break;
		case PTZF_CONFIG_PANASONIC_SET1:
			m_CommandBuffer[b+11] = 'A';
			break;
		case PTZF_CONFIG_PANASONIC_SET2:
			m_CommandBuffer[b+11] = 'B';
			break;
		}
	}
	else if (IsBetween(c, 0, 19))						// pan, tilt, zoom, iris, focus
	{
		m_CommandBuffer[b+8] = '1';
		m_CommandBuffer[b+9] = bytePanasonic[c][0];
		m_CommandBuffer[b+10] = bytePanasonic[c][1];
		m_CommandBuffer[b+11] = bytePanasonic[c][2];
		if (m_CommandBuffer[b+11] != '4')				// stop cmd
		{
			if (!IsBetween(c, 6, 14))					// no zoom, iris, focus		
			{
				if (theApp.GetControlFunctions(pCCommandR->GetCamID()) & PTZF_SPEED)
				{
					m_CommandBuffer[b+10] = '6';		// variable pan tilt speed
					cmdReturn = (CameraControlCmd)2001;	// vari speed
				}
			}

			m_pRepeatCmd = new CCameraCommandRecord(pCCommandR->GetCommID(), pCCommandR->GetCamID(), pCCommandR->GetCommand(), pCCommandR->GetValue());
			m_bRepeat = TRUE;
		}
	}
	// end symbol
	m_CommandBuffer[b+12] = _ETX_;
	return cmdReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferElbex(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	CAutoCritSec acs(&m_csRepeat);
	int c = ((int)cmd)-1;
	int i;
	BOOL bFound = FALSE, bMoveCmd = FALSE;
	WORD id = 0x03FF;
	if (IsBetween(cmd, CCC_MOVE_LEFT_UP, CCC_MOVE_STOP))
	{
		c = (int)(cmd-CCC_MOVE_LEFT_UP)+15;
		bMoveCmd = TRUE;
	}

	for (i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		id = (WORD)(m_camIDs[i] & 0x03FF);
	}

	id = (WORD)(id << 3);

	m_CommandBuffer[0] = LOBYTE(id);//0xFE;
	m_CommandBuffer[1] = HIBYTE(id);//0x1F;
	m_CommandBuffer[2] = byteElbex[c][1];
	m_CommandBuffer[3] = byteElbex[c][0];
	if ( (cmd==CCC_PAN_STOP) || 
		 (cmd==CCC_TILT_STOP) || 
		 (cmd==CCC_IRIS_STOP) || 
		 (cmd==CCC_ZOOM_STOP) || 
		 (cmd==CCC_FOCUS_STOP)||
		 bMoveCmd)
	{
		SendCommand();
		m_bRepeat = FALSE; // don't send the stop command twice
	}
	else
	{
		m_bRepeat = TRUE;
	}
}
#define ERNI_AUTO_IRIS  0x0001
#define ERNI_AUTO_FOCUS 0x0002
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferErnitecBDR510(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	BOOL bSendCmdImmediately = FALSE;
	DWORD dwValue = pCCommandR->GetValue();
	WORD  wFlags = 0;
	int i, nCam = -1;
	for (i=0; i<m_secIDs.GetSize(); i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			nCam = i;
			break;
		}
	}

	BYTE cmdBuffer3 = m_CommandBuffer[3];
	BYTE cmdBuffer4 = m_CommandBuffer[4];

	BOOL bTelemetryCommand = CCommandRecord::IsTelemetryCommand(cmd);

	if (nCam != -1)
	{
		wFlags = HIWORD(m_Flags[nCam]);
		if (   bTelemetryCommand == TRUE 
			&& LOWORD(dwValue) != LOWORD(m_Flags[nCam]))
		{	// send a speed cmd before
			DWORD dwSpeeds= MAKELONG(dwValue, dwValue);
			CCameraCommandRecord ccr(pCCommandR->GetCommID(), pCCommandR->GetCamID(), (CameraControlCmd)1000, dwSpeeds);
			CommandBufferErnitecBDR510(&ccr);
			m_Flags.SetAt(nCam, MAKELONG(LOWORD(dwValue), wFlags));
		}
	}

	if (bTelemetryCommand)
	{
		m_CommandBuffer[3] = 0;	// Flags ertmal löschen
		m_CommandBuffer[4] = 0;
		if (wFlags & ERNI_AUTO_IRIS)
		{
			m_CommandBuffer[4] |=  0x03;
		}
		if (wFlags & ERNI_AUTO_FOCUS)
		{
			m_CommandBuffer[3] |=  0xC0;
		}
	}

	CAutoCritSec acs(&m_csRepeat);

	m_CommandBuffer[0] = _STX_;		// StartSymbol
	m_CommandBuffer[1] = 0xFF;		// default address	
	m_CommandBuffer[2] = 0x01;		// command PTZ

	if (nCam != -1)
	{
		m_CommandBuffer[1] = (BYTE)m_camIDs[nCam]-1; // camera specific address	
	}

	m_bRepeat = TRUE;

	switch (cmd)
	{
		case CCC_PAN_LEFT:			m_CommandBuffer[3] |= 0x02; break;
		case CCC_PAN_RIGHT:			m_CommandBuffer[3] |= 0x01; break;

		case CCC_TILT_UP:			m_CommandBuffer[3] |= 0x04; break;
		case CCC_TILT_DOWN:			m_CommandBuffer[3] |= 0x08; break;

		case CCC_MOVE_LEFT_DOWN:	m_CommandBuffer[3] |= 0x0A; break;
		case CCC_MOVE_RIGHT_DOWN:	m_CommandBuffer[3] |= 0x09; break;
		case CCC_MOVE_LEFT_UP:		m_CommandBuffer[3] |= 0x06; break;
		case CCC_MOVE_RIGHT_UP:		m_CommandBuffer[3] |= 0x05; break;

		case CCC_TILT_STOP:
		case CCC_PAN_STOP:
		case CCC_MOVE_STOP:			m_CommandBuffer[3] &= ~0x0F; m_bRepeat = FALSE; break;

		case CCC_FOCUS_NEAR:		m_CommandBuffer[3] &= ~0xC0; m_CommandBuffer[3] |= 0x40; break;
		case CCC_FOCUS_FAR:			m_CommandBuffer[3] &= ~0xC0; m_CommandBuffer[3] |= 0x80; break;
		case CCC_FOCUS_AUTO:		m_CommandBuffer[3] |=  0xC0; wFlags |=  ERNI_AUTO_FOCUS; m_bRepeat = FALSE; break;
		case CCC_FOCUS_STOP:		m_CommandBuffer[3] &= ~0xC0; wFlags &= ~ERNI_AUTO_FOCUS; m_bRepeat = FALSE; break;

		case CCC_IRIS_OPEN:			m_CommandBuffer[4] &= ~0x03; m_CommandBuffer[4] |= 0x01; break;
		case CCC_IRIS_CLOSE:		m_CommandBuffer[4] &= ~0x03; m_CommandBuffer[4] |= 0x02; break;
		case CCC_IRIS_AUTO:			m_CommandBuffer[4] |=  0x03; wFlags |=  ERNI_AUTO_IRIS;  m_bRepeat = FALSE; break;
		case CCC_IRIS_STOP:			m_CommandBuffer[4] &= ~0x03; wFlags &= ~ERNI_AUTO_IRIS;  m_bRepeat = FALSE; break;

		case CCC_ZOOM_IN:			m_CommandBuffer[3] |= 0x20; break;
		case CCC_ZOOM_OUT:			m_CommandBuffer[3] |= 0x10; break;
		case CCC_ZOOM_STOP:			m_CommandBuffer[3] &= ~0x30; m_bRepeat = FALSE; break;

		case CCC_AUXOUT:
			m_CommandBuffer[2] = 13;	// command AUX Relais
			m_CommandBuffer[3] = (BYTE)LOWORD(pCCommandR->GetValue());
			m_CommandBuffer[4] = (BYTE)HIWORD(pCCommandR->GetValue());
			bSendCmdImmediately = TRUE;
		    break;

		case CCC_POS_1: case CCC_POS_2: case CCC_POS_3: case CCC_POS_4: case CCC_POS_5: case CCC_POS_6: // 16,..,21
		case CCC_POS_7: case CCC_POS_8: case CCC_POS_9: // 23,.., 25
		case CCC_POS_X:
			if      (cmd <= CCC_POS_6)	m_CommandBuffer[3] = (BYTE)(cmd - CCC_POS_1 + 1);
			else if (cmd <= CCC_POS_9)	m_CommandBuffer[3] = (BYTE)(cmd - CCC_POS_1 + 2);
			else						m_CommandBuffer[3] = (BYTE)HIWORD(pCCommandR->GetValue());
			switch (LOWORD(pCCommandR->GetValue()))
			{
				case 0: m_CommandBuffer[2] = 2; break;	// command Call Preset
				case 1: m_CommandBuffer[2] = 5;	break;	// command Save Preset
			}
			bSendCmdImmediately = TRUE;
			break;
/*
		case CCC_IRIS_AUTO:
		{
			BYTE nAutoIrisIndex =HIBYTE(LOWORD(m_Flags[nCam]));
			m_CommandBuffer[2] = 16;	// command Camera Setup
			m_CommandBuffer[3] = 7;
			switch(nAutoIrisIndex++)
			{
				case 0: m_CommandBuffer[4] = 0x00; break;
				case 1: m_CommandBuffer[4] = 0x80; break;
				case 2: m_CommandBuffer[4] = 0xf0; break;
				case 3: m_CommandBuffer[4] = 0x08; break;
				case 4: m_CommandBuffer[4] = 0x88; break;
				case 5: m_CommandBuffer[4] = 0xf8; break;
				case 6: m_CommandBuffer[4] = 0x0f; break;
				case 7: m_CommandBuffer[4] = 0x8f; break;
				case 8: m_CommandBuffer[4] = 0xff; nAutoIrisIndex = 0; break;
			}
			m_Flags[nCam] &= 0x0000FF00;
			m_Flags[nCam] |= (nAutoIrisIndex << 8);
		} break;
		case CCC_AUXOUT:
		{
			m_CommandBuffer[2] = 10;	// command Latch AUX
			DWORD dwBit = 0x00000002 << LOWORD(pCCommandR->GetValue());
			if (HIWORD(pCCommandR->GetValue()))
			{
				m_Flags[nCam] |= dwBit;
			}
			else
			{
				m_Flags[nCam] &= ~dwBit;
			}
			m_CommandBuffer[3] = (BYTE)m_Flags[nCam];
		} break;
*/
		default:
		if (cmd == 1000)	// Speed Cmd
		{
			m_CommandBuffer[2] = 14;	// command Pan/Tilt Speed
			m_CommandBuffer[3] = (BYTE)(LOWORD(pCCommandR->GetValue()) * 21 + 45); // pan speed:  1 - 10 >> 25 - 250
			m_CommandBuffer[4] = (BYTE)(HIWORD(pCCommandR->GetValue()) * 21 + 45); // tilt speed: 1 - 10 >> 25 - 250
			bSendCmdImmediately = TRUE;
		}break;
	}

	// calculate checksum
	m_CommandBuffer[5] = 0;
	for (i=0;i<5;i++)
	{
		m_CommandBuffer[5] = (BYTE)(m_CommandBuffer[5] + m_CommandBuffer[i]);		// checksum
	}

	if (!m_bRepeat)
	{
		acs.Unlock();
		SendCommand();
	}
	else if (bSendCmdImmediately)
	{
		acs.Unlock();
		SendCommand();
		m_CommandBuffer[3] = cmdBuffer3;
		m_CommandBuffer[4] = cmdBuffer4;
		m_bRepeat = FALSE;
	}
	else if (nCam != -1)
	{
		m_Flags.SetAt(nCam, MAKELONG(LOWORD(m_Flags[nCam]), wFlags));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferPieper(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
//	DWORD dwValue = pCCommandR->GetValue();
	int i = ((int)cmd)-1;

	if ( (cmd==CCC_IRIS_OPEN) ||
		 (cmd==CCC_IRIS_CLOSE) ||
		 (cmd==CCC_IRIS_STOP))
	{
		return;
	}
	m_CommandBuffer[1] = bytePieper[i][1];
	if (m_CommandBuffer[1]!='0')
	{
		m_CommandBuffer[0] = bytePieper[i][0];
	}
	m_CommandBuffer[2] = 0x0D; // cariage return
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferVidev(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
//	DWORD dwValue = pCCommandR->GetValue();
	int c = ((int)cmd)-1;

	if ( (cmd==CCC_IRIS_OPEN) ||
		 (cmd==CCC_IRIS_CLOSE) ||
		 (cmd==CCC_IRIS_STOP))
	{
		// iris command are not supported by Videv EC160
		return;
	}

	int i;
	BOOL bFound = FALSE;
	BYTE id = 0xFF;
	char sID[] = "FF ";

	for (i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		id = (BYTE)(m_camIDs[i]);
		sprintf(sID, "%02x", id);
//		sID.Format(_T("%02x"),id);
	}
	

	m_CommandBuffer[0] = _STX_;
	m_CommandBuffer[1] = sID[0];
//	m_CommandBuffer[1] = (BYTE)(id & 0x0f);			// lower nibble
	m_CommandBuffer[2] = sID[1];
//	m_CommandBuffer[2] = (BYTE)((id >> 1) & 0x0f);	// upper nibble
	m_CommandBuffer[3] = byteVidev[c][0];
	m_CommandBuffer[4] = byteVidev[c][1];
	m_CommandBuffer[5] = byteVidev[c][2];
	m_CommandBuffer[6] = _ETX_;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferCanon(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	int c = ((int)cmd)-1;
	int i;
	BOOL bFound = FALSE, bMoveCmd = FALSE;;
	BYTE id = 0x80;
	DWORD dwValue = min(10,pCCommandR->GetValue());
	if (IsBetween(cmd, CCC_MOVE_LEFT_UP, CCC_MOVE_STOP))
	{
		c = (int)(cmd-CCC_MOVE_LEFT_UP)+15;
		bMoveCmd = TRUE;
	}

	for (i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		id = (BYTE)(m_camIDs[i] | 0x80);
	}

	m_CommandBuffer[0] = id;	// address
	m_CommandBuffer[1] = 0x01;  

	if ( (cmd==CCC_IRIS_OPEN)  ||
		 (cmd==CCC_IRIS_CLOSE) ||
		 (cmd==CCC_IRIS_STOP)  ||
		 (cmd==CCC_FOCUS_FAR)  ||
		 (cmd==CCC_FOCUS_NEAR) ||
		 (cmd==CCC_FOCUS_STOP) ||
		 (cmd==CCC_ZOOM_IN)    ||
		 (cmd==CCC_ZOOM_OUT)   ||
		 (cmd==CCC_ZOOM_STOP))
	{
		m_CommandBuffer[2] = 0x04;
		m_CommandBuffer[3] = byteCanon[c][0];
		m_CommandBuffer[4] = byteCanon[c][1];
		m_CommandBuffer[5] = 0xFF;
		m_iBufferLength = 6;
	}
	else if ((cmd==CCC_PAN_LEFT)  ||
			 (cmd==CCC_PAN_RIGHT) ||
			 (cmd==CCC_PAN_STOP)  ||
			 (cmd==CCC_TILT_UP)   ||
			 (cmd==CCC_TILT_DOWN) ||
			 (cmd==CCC_TILT_STOP) ||
			 bMoveCmd)
	{
		m_CommandBuffer[2] = 0x06;
		m_CommandBuffer[3] = 0x01;

		if ((cmd==CCC_PAN_LEFT)  ||
			(cmd==CCC_PAN_RIGHT) )
		{
			m_CommandBuffer[4] = (BYTE)MulDiv(dwValue, 0x18, 10);
//			m_CommandBuffer[4] = (BYTE)((0x18 * dwValue) / 10);  // pan speed 01 to 18
			m_CommandBuffer[5] = 0x00;  // tilt speed 01 to 14
		}
		else if ((cmd==CCC_TILT_UP)  ||
			(cmd==CCC_TILT_DOWN) )
		{
			m_CommandBuffer[4] = 0x00;  // pan speed 01 to 18
			m_CommandBuffer[5] = (BYTE)MulDiv(dwValue, 0x14, 10);  // tilt speed 01 to 14
//			m_CommandBuffer[5] = (BYTE)((0x14 * dwValue) / 10);  // tilt speed 01 to 14
		}
		else
		{
			m_CommandBuffer[4] = 0x00;
			m_CommandBuffer[5] = 0x00;
		}

		m_CommandBuffer[6] = byteCanon[c][0];
		m_CommandBuffer[7] = byteCanon[c][1];

		m_CommandBuffer[8] = 0xFF;
		m_iBufferLength = 9;
	}
	else if ( (cmd==CCC_POS_1) ||
			  (cmd==CCC_POS_2) ||
			  (cmd==CCC_POS_3) ||
			  (cmd==CCC_POS_4) ||
			  (cmd==CCC_POS_5) ||
			  (cmd==CCC_POS_6) )
	{
		m_CommandBuffer[0] = id;
		m_CommandBuffer[1] = 0x01;
		m_CommandBuffer[2] = 0x04;
		m_CommandBuffer[3] = 0x3F;
		if (dwValue == 0)
		{
			m_CommandBuffer[4] = 0x02;
		}
		else
		{
			m_CommandBuffer[4] = 0x01;
		}
		m_CommandBuffer[5] = (BYTE)((c-(int)CCC_POS_1)+1);
		m_CommandBuffer[6] = 0xFF;
		m_iBufferLength = 7;
	}
	else if (cmd==CCC_POS_HOME)
	{
		m_CommandBuffer[0] = id;
		m_CommandBuffer[1] = 0x01;
		m_CommandBuffer[2] = 0x06;
		m_CommandBuffer[3] = 0x04;
		m_CommandBuffer[4] = 0xFF;
		m_iBufferLength = 5;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferVCLCamera(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD dwValue = pCCommandR->GetValue();
	int i;
	BOOL bFound = FALSE;
	BYTE id = 0x80;
	DWORD   dwSpeed = min(10,pCCommandR->GetValue());

	m_bRepeat = FALSE; // only some commands have to be repeated

	for (i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		id = (BYTE)(m_camIDs[i] + 0x7F); // CameraID 1 - 128

		m_CommandBuffer[0] = id;	// address

		if ( (cmd==CCC_PAN_LEFT)   ||
			 (cmd==CCC_PAN_RIGHT)  ||
			 (cmd==CCC_TILT_UP)    ||
			 (cmd==CCC_TILT_DOWN)
			)
		{
			m_CommandBuffer[1] = byteVCL[cmd];
			m_CommandBuffer[2] = (BYTE)MulDiv(dwSpeed, 0x7F, 10);  // pan speed 01 to 7F
//			m_CommandBuffer[2] = (BYTE)((0x7F * dwSpeed) / 10);  // pan speed 01 to 7F
			m_iBufferLength = 3;
			m_bRepeat = TRUE;
		}
		else if (   (cmd==CCC_PAN_STOP)
				 || (cmd==CCC_TILT_STOP)
				 || (cmd==CCC_IRIS_OPEN)
				 || (cmd==CCC_IRIS_CLOSE)
				 || (cmd==CCC_IRIS_STOP)
				 || (cmd==CCC_FOCUS_FAR)
				 || (cmd==CCC_FOCUS_NEAR)
				 || (cmd==CCC_FOCUS_STOP)
				 || (cmd==CCC_ZOOM_IN)
				 || (cmd==CCC_ZOOM_OUT)
				 || (cmd==CCC_ZOOM_STOP)
			)
		{
			m_CommandBuffer[1] = byteVCL[cmd];
			m_iBufferLength = 2;
		}
		else if ( (cmd==CCC_POS_1) ||
				  (cmd==CCC_POS_2) ||
				  (cmd==CCC_POS_3) ||
				  (cmd==CCC_POS_4) ||
				  (cmd==CCC_POS_5) ||
				  (cmd==CCC_POS_6)
				)
		{
			BOOL bPreset = (BOOL)dwValue;
			if (bPreset)
			{
				m_CommandBuffer[1] = 0x47;
			}
			else
			{
				m_CommandBuffer[1] = 0x42;
			}
			m_CommandBuffer[2] = (BYTE)((cmd-(int)CCC_POS_1)+1);
			m_iBufferLength = 3;
		}
		else if ( (cmd==CCC_POS_7) ||
				  (cmd==CCC_POS_8) ||
				  (cmd==CCC_POS_9)
				)
		{
			BOOL bPreset = (BOOL)dwValue;
			if (bPreset)
			{
				m_CommandBuffer[1] = 0x47;
			}
			else
			{
				m_CommandBuffer[1] = 0x42;
			}
			m_CommandBuffer[2] = (BYTE)((cmd-(int)CCC_POS_7)+7);
			m_iBufferLength = 3;
		}
		else if (cmd==CCC_POS_HOME)
		{
			BOOL bPreset = (BOOL)dwValue;
			if (bPreset)
			{
				m_CommandBuffer[1] = 0x47;
			}
			else
			{
				m_CommandBuffer[1] = 0x42;
			}
			m_CommandBuffer[2] = 0x00;
			m_iBufferLength = 3;
		}
		else if (cmd==CCC_FOCUS_AUTO)
		{
			BOOL bOn = (BOOL)dwValue;
			if (bOn)
			{
				m_CommandBuffer[1] = 0x41;
			}
			else
			{
				m_CommandBuffer[1] = 0x61;
			}
			m_iBufferLength = 2;
		}
		else if (cmd==CCC_IRIS_AUTO)
		{
			BOOL bOn = (BOOL)dwValue;
			if (bOn)
			{
				m_CommandBuffer[1] = 0x4D;
			}
			else
			{
				m_CommandBuffer[1] = 0x6D;
			}
			m_iBufferLength = 2;
		}
		else if (cmd==CCC_TURN_180)
		{
			m_CommandBuffer[1] = 0x54;
			m_iBufferLength = 2;
		}
		else
		{
			// No valid command, set address invalid
			m_CommandBuffer[0] = 0;	// address
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferVCLMaxCom(CCameraCommandRecord* pCCommandR)
{
	CameraControlCmd cmd = pCCommandR->GetCommand();
	DWORD dwValue = pCCommandR->GetValue();
	BOOL bFound = FALSE;
	BYTE id = 0x00;
	DWORD   dwSpeed = min(10,pCCommandR->GetValue());
	BYTE bySpeed = (BYTE)MulDiv(dwSpeed, 0x7F, 10);
//	BYTE bySpeed = (BYTE)((0x7F * dwSpeed) / 10);

	m_bRepeat = FALSE; // only some commands have to be repeated

	for (int i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			id = (BYTE)(m_camIDs[i]); // CameraID 1 - 128
			break;
		}
	}
	if (bFound)
	{
		m_csMaxCom.Lock();	// a command has to be inserted completely before it can be read out!
		if (m_byMaxComLastCameraAddress != id)
		{
			m_byMaxComLastCameraAddress = id;
			CString sAddress;
			sAddress.Format(_T("%d"), id);
			BYTE byDigit;
			LPCTSTR szAddress = sAddress;
			for (i=0; i<sAddress.GetLength() ; i++)
			{
				byDigit = (BYTE)szAddress[i];
				m_byaVCLMaxCom.Add(byDigit);	// each digit pressed...
				m_byaVCLMaxCom.Add((BYTE)(byDigit-0x10));	// ...and released
			}
			m_byaVCLMaxCom.Add(0x43);	// camera pressed...
			m_byaVCLMaxCom.Add(0x63);	// ...and released
		}

		if (   (cmd==CCC_PAN_LEFT)
		    || (cmd==CCC_PAN_RIGHT)
		    || (cmd==CCC_TILT_UP)
		    || (cmd==CCC_TILT_DOWN)
		   )
		{
			m_byaVCLMaxCom.Add(byteVCL[cmd]);
			m_byaVCLMaxCom.Add(bySpeed);	// speed parameter
		}
		else if (   (cmd==CCC_PAN_STOP)
				 || (cmd==CCC_TILT_STOP)
				 || (cmd==CCC_IRIS_OPEN)
				 || (cmd==CCC_IRIS_CLOSE)
				 || (cmd==CCC_IRIS_STOP)
				 || (cmd==CCC_FOCUS_FAR)
				 || (cmd==CCC_FOCUS_NEAR)
				 || (cmd==CCC_FOCUS_STOP)
				 || (cmd==CCC_ZOOM_IN)
				 || (cmd==CCC_ZOOM_OUT)
				 || (cmd==CCC_ZOOM_STOP)
			)
		{
			m_byaVCLMaxCom.Add(byteVCL[cmd]);
		}
		else if (cmd==CCC_POS_HOME)
		{
			BOOL bPreset = (BOOL)dwValue;
			if (bPreset)
			{
				m_byaVCLMaxCom.Add(0x42);	// preset pressed
				m_byaVCLMaxCom.Add(0x30);	// digit pressed (0-9)
				m_byaVCLMaxCom.Add(0x62);	// preset released
			}
			else
			{
				m_byaVCLMaxCom.Add(0x42);	// digit pressed (0-9)
				m_byaVCLMaxCom.Add(0x30);	// preset pressed
				m_byaVCLMaxCom.Add(0x62);	// preset released
			}
		}
		else if ( (cmd==CCC_POS_1)	  ||
				  (cmd==CCC_POS_2) ||
				  (cmd==CCC_POS_3) ||
				  (cmd==CCC_POS_4) ||
				  (cmd==CCC_POS_5) ||
				  (cmd==CCC_POS_6)
				)
		{
			BYTE byte = (BYTE)((cmd-(int)CCC_POS_1) + 0x31);
			BOOL bPreset = (BOOL)dwValue;
			if (bPreset)
			{
				m_byaVCLMaxCom.Add(0x42);	// preset pressed
				m_byaVCLMaxCom.Add(byte);	// digit pressed (0-9)
				m_byaVCLMaxCom.Add(0x62);	// preset released
			}
			else
			{
				m_byaVCLMaxCom.Add(byte);	// digit pressed (0-9)
				m_byaVCLMaxCom.Add(0x42);	// preset pressed
				m_byaVCLMaxCom.Add(0x62);	// preset released
			}
		}
		else if ( (cmd==CCC_POS_7) ||
				  (cmd==CCC_POS_8) ||
				  (cmd==CCC_POS_9)
				)
		{
			BYTE byte = (BYTE)((cmd-(int)CCC_POS_7) + 0x37);
			BOOL bPreset = (BOOL)dwValue;
			if (bPreset)
			{
				m_byaVCLMaxCom.Add(0x42);	// preset pressed
				m_byaVCLMaxCom.Add(byte);	// digit pressed (0-9)
				m_byaVCLMaxCom.Add(0x62);	// preset released
			}
			else
			{
				m_byaVCLMaxCom.Add(byte);	// digit pressed (0-9)
				m_byaVCLMaxCom.Add(0x42);	// preset pressed
				m_byaVCLMaxCom.Add(0x62);	// preset released
			}
		}
		else if (cmd==CCC_FOCUS_AUTO)
		{
			m_byaVCLMaxCom.Add(0x50);
			m_byaVCLMaxCom.Add(0x70);
		}
		else if (cmd==CCC_IRIS_AUTO)
		{
			m_byaVCLMaxCom.Add(0x4D);
			m_byaVCLMaxCom.Add(0x6D);
		}
		else if (cmd==CCC_TURN_180)
		{
			m_byaVCLMaxCom.Add(0x07);
			m_byaVCLMaxCom.Add(0x17);
		}
		m_csMaxCom.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::CommandBufferFastrax(CCameraCommandRecord* pCCommandR)
{
	BOOL bFound = FALSE;
	BYTE id = 0x00;

	for (int i=0;i<m_secIDs.GetSize();i++)
	{
		if (pCCommandR->GetCamID()==m_secIDs[i])
		{
			// found
			bFound = TRUE;
			id = (BYTE)(m_camIDs[i]); // CameraID 1 - 100
			break;
		}
	}
	if (bFound)
	{
		m_bRepeat = FALSE; // only some commands have to be repeated
		CameraControlCmd cmd = pCCommandR->GetCommand();
		DWORD dwValue = pCCommandR->GetValue();
		// some special values from dwValue
		BOOL bPreset = (BOOL)dwValue;
//		TRACE(_T("FastraxII dwValue %u\n"), dwValue);
		DWORD dwSpeed = min(10, dwValue);	// 1 - 10
		BYTE bySpeed = (BYTE)(((7.0 * (float)dwSpeed) / 10.0) + 0.5);	// 1 - 7
//		TRACE(_T("FastraxII bySpeed %u\n"), bySpeed);

		m_CommandBuffer[0] = 0xA5;	// Header
		m_CommandBuffer[1] = id;	// id	
		m_CommandBuffer[2] = 0x10;	// for lens control with Pan/ Tilt commands, functions are special values
		m_CommandBuffer[3] = 0x0D;	// stop zoom as default value for pan/tilt commands
		m_CommandBuffer[4] = 0x88;	// stop pan & tilt as default value for focus/iris/zoom commands

		switch (cmd)
		{
			case CCC_PAN_LEFT:
				m_CommandBuffer[4] = (BYTE)(0x88 - (bySpeed<<4));
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_PAN_RIGHT:	
				m_CommandBuffer[4] = (BYTE)(0x88 + (bySpeed<<4));
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_PAN_STOP:
				// use default
				break;
			case CCC_TILT_UP:	
				m_CommandBuffer[4] = (BYTE)(0x88 + (bySpeed));
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_TILT_DOWN:	
				m_CommandBuffer[4] = (BYTE)(0x88 - (bySpeed));
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_TILT_STOP:	
				// use default
				break;
			case CCC_FOCUS_NEAR:
				m_CommandBuffer[3] = 0x03;
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_FOCUS_FAR:
				m_CommandBuffer[3] = 0x04;
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_FOCUS_STOP:
				// use default
				break;
			case CCC_IRIS_OPEN:
				m_CommandBuffer[3] = 0x06;
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_IRIS_CLOSE:
				m_CommandBuffer[3] = 0x05;
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_IRIS_STOP:
				// use default
				break;
			case CCC_ZOOM_IN:
				m_CommandBuffer[3] = 0x0E;
				m_bRepeat = TRUE; // only some commands have to be repeated
				break;
			case CCC_ZOOM_OUT:
				m_CommandBuffer[3] = 0x0C;
				m_bRepeat = TRUE; // only some commands have to be repeated
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
				if (bPreset)
				{
					m_CommandBuffer[2] = 0x25; // 0x27 does not work at second time
				}
				else
				{
					m_CommandBuffer[2] = 0x11;
				}
				m_CommandBuffer[3] = 0x80;
				m_CommandBuffer[4] = (BYTE)(0x80 + (cmd - CCC_POS_1) +1);
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
					m_CommandBuffer[2] = 0x1D;
					m_CommandBuffer[3] = 0x80;
					m_CommandBuffer[4] = 0x80;
				}
				break;
			case CCC_POS_7:
			case CCC_POS_8:
			case CCC_POS_9:
				if (bPreset)
				{
					m_CommandBuffer[2] = 0x25; // 0x27 does not work at second time
				}
				else
				{
					m_CommandBuffer[2] = 0x11;
				}
				m_CommandBuffer[3] = 0x80;
				m_CommandBuffer[4] = (BYTE)(0x80 + (cmd - CCC_POS_1));
				break;
		}

		// End
		m_CommandBuffer[5] = 0x55;
		// calculate checksum
		m_CommandBuffer[6] = 0;
		for (i=0 ; i<6 ; i++)
		{
			m_CommandBuffer[6] = (BYTE)(m_CommandBuffer[6] + m_CommandBuffer[i]);		// checksum
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CCameraControlRecord::ReceivedDataVCLMaxCom(LPVOID pBuffer,DWORD dwLen)
{
	if (pBuffer)
	{
		BYTE* pByte = (BYTE*)pBuffer;
		BYTE byte;
		for (DWORD dw=0 ; dw<dwLen ; dw++)
		{
			byte = pByte[dw];
			if ((byte & 0x80) == 0x80)	// is it a valid command
			{
				if ((byte & 0x0f) == 0x00)	// request for data
				{
					m_csMaxCom.Lock();
					if (m_byaVCLMaxCom.GetSize() != 0)
					{
						m_CommandBuffer[0] = m_byaVCLMaxCom.GetAt(0);
						m_byaVCLMaxCom.RemoveAt(0);
					}
					else
					{
						m_CommandBuffer[0] = 0x00;
					}
					m_csMaxCom.Unlock();
					m_iBufferLength = 1;
					SendCommand();
				}
				else if ((byte & 0x05) == 0x05)	// request for software reference
				{
					m_CommandBuffer[0] = 0x4B; // 'K';
					m_CommandBuffer[0] = 0x54; // 'T';
					m_CommandBuffer[0] = 0x31; // '1';
					m_CommandBuffer[0] = 0x2E; // '.';
					m_CommandBuffer[0] = 0x30; // '0';
					m_iBufferLength = 5;
					SendCommand();
				}
			}
			else
			{	// ignore it
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
UINT CCameraControlRecord::RepeatProc(LPVOID pParam)
{
	CCameraControlRecord* pCameraControlRecord = (CCameraControlRecord*)pParam;

	while (pCameraControlRecord->m_bRun)
	{
		Sleep(pCameraControlRecord->m_iRepeatTime);

		if (pCameraControlRecord->m_bRepeat && pCameraControlRecord->m_bRun)
		{
			CAutoCritSec acs(&pCameraControlRecord->m_csRepeat);
			if (pCameraControlRecord->m_pRepeatCmd)
			{
				CCameraCommandRecord *pCCR = pCameraControlRecord->m_pRepeatCmd;
				CCameraCommandRecord *pccr = new CCameraCommandRecord(pCCR->GetCommID(), pCCR->GetCamID(), pCCR->GetCommand(), pCCR->GetValue());
				AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CAMERA_COMMAND, (LPARAM)pccr);
			}
			else
			{
				pCameraControlRecord->SendCommand();
			}
		}
	}
	return 2;
}
