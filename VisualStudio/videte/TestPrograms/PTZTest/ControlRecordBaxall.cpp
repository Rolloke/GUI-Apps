// ControlRecordBaxall.cpp: implementation of the CControlRecordBaxall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PTZTest.h"
#include "ControlRecordBaxall.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
#define BAXALL_MODIFIER_CODE	(BYTE)0x08
#define BAXALL_ADD_CODE			(BYTE)(BAXALL_MODIFIER_CODE + 0x01)

#define BAXALL_ACK				(BYTE)0x41
#define BAXALL_NAK				(BYTE)0x42
#define BAXALL_STATUS_RESPONSE	(BYTE)0x43

//////////////////////////////////////////////////////////////////////
CControlRecordBaxall::CControlRecordBaxall(int iCom, BOOL bTraceHex)
	: CControlRecord(iCom, FALSE, bTraceHex, FALSE)
{
	m_bDecreaseBaxallAddCode = FALSE;
	m_bySourceID = 0x89;	// Source ID, default ZTX6 9
//	RefreshTimeOut(200);
}
//////////////////////////////////////////////////////////////////////
CControlRecordBaxall::~CControlRecordBaxall()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecordBaxall::Create()
{
	WK_TRACE("ControlRecordBaxall Create COM=%d\n", GetCOMNumber());
	InitialiseResponses();
	return Open(CBR_9600,8,NOPARITY,ONESTOPBIT);;
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::InitialiseResponses()
{
	m_byArrayBaxall.RemoveAll();
	m_byArrayBaxall.Add(0xAA);			// Header PAD
	m_byArrayBaxall.Add(0xAA);			// Header PAD
	m_byArrayBaxall.Add(0x02);			// Start symbol
	m_byArrayBaxall.Add(0x81);			// Destination ID, default keyboard 1
	m_byArrayBaxall.Add(m_bySourceID);	// Source ID
	m_byArrayBaxall.Add(0x0A);			// Status/ACK/NAK
	m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);
	m_byArrayBaxall.Add(BAXALL_ADD_CODE + 4);	// Length
	m_byArrayBaxall.Add(0xA1);			// device type ZTX6
	m_byArrayBaxall.Add(0x41);			// default ACK
	m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);	// following 2 LED bytes zero
	m_byArrayBaxall.Add(BAXALL_ADD_CODE);
	m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);
	m_byArrayBaxall.Add(BAXALL_ADD_CODE);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillHeader(BYTE byDestID)
{
	m_byArrayBaxall.RemoveAll();
	m_byArrayBaxall.Add(0xAA);			// Header PAD
	m_byArrayBaxall.Add(0xAA);			// Header PAD
	m_byArrayBaxall.Add(0x02);			// Start symbol
	m_byArrayBaxall.Add(byDestID);		// Destination ID
	m_byArrayBaxall.Add(m_bySourceID);	// Source ID
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillAckNakStatus(BYTE byResponse)
{
	m_byArrayBaxall.Add(0x0A);					// Status/ACK/NAK
	m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);
	m_byArrayBaxall.Add(BAXALL_ADD_CODE + 4);	// Length
	m_byArrayBaxall.Add(0xA1);					// device type ZTX6
	m_byArrayBaxall.Add(byResponse);			// default ACK
	m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);	// following 2 LED bytes zero
	m_byArrayBaxall.Add(BAXALL_ADD_CODE);
	m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);
	m_byArrayBaxall.Add(BAXALL_ADD_CODE);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::FillCRCandFooterPAD()
{
	// checksum, sum up from source id (index 4) to end of data
	WORD wCRC = 0;
	BYTE byte = 0;
	for (int i=4 ; i<m_byArrayBaxall.GetSize() ; i++)
	{
		byte = m_byArrayBaxall.GetAt(i);
		if (byte == BAXALL_MODIFIER_CODE)
		{
			// ignore and take next reduced
			i++;
			byte  = m_byArrayBaxall.GetAt(i);
			byte = (BYTE)(byte - BAXALL_ADD_CODE);
			wCRC = (BYTE)(wCRC + byte);
		}
		else
		{
			wCRC = (BYTE)(wCRC + byte);
		}
	}
	// now set the CRC bytes appropriate
	BYTE byCRC = LOBYTE(wCRC);
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byCRC <= BAXALL_MODIFIER_CODE)
	{
		m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);
		m_byArrayBaxall.Add((BYTE)(byCRC + BAXALL_ADD_CODE));
	}
	else
	{
		m_byArrayBaxall.Add(byCRC);
	}
	byCRC = HIBYTE(wCRC);
	// if value <= BAXALL_MODIFIER_CODE send as 2-Byte (BAXALL_MODIFIER_CODE and value + BAXALL_ADD_CODE
	if (byCRC <= BAXALL_MODIFIER_CODE)
	{
		m_byArrayBaxall.Add(BAXALL_MODIFIER_CODE);
		m_byArrayBaxall.Add((BYTE)(byCRC + BAXALL_ADD_CODE));
	}
	else
	{
		m_byArrayBaxall.Add(byCRC);
	}
	// end of data and footer PAD
	m_byArrayBaxall.Add(0x03);
	m_byArrayBaxall.Add(0xAA);
	m_byArrayBaxall.Add(0x55);
	m_byArrayBaxall.Add(0x00);
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
			WK_TRACE_ERROR("COM%i Daten Len %i Buffer-Overrun, no ETX\n",
								GetCOMNumber(), m_byaReceived.GetSize());
			sMsg.Format( "Buffer-Overrun|COM%i", GetCOMNumber() );
			WK_STAT_PEAK("BufferOverrun", 1, sMsg);
			m_byaReceived.RemoveAll();
		}

		byte = pBuffer[dw];
		// Wenn ein Startzeichen kommt, fangen die Daten erst richtig an
		if (byte == 0x02)
		{
			ClearAllData();
			m_bDecreaseBaxallAddCode = FALSE;
			m_byaReceived.Add(byte); // add for easier handling
		}
		// Wenn ein Endezeichen kommt, ist ein Datensatz komplett
		else if (byte == 0x03)
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
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::CheckData()
{
	int iLen = m_byaReceived.GetSize();
	BYTE byte;
	// Check for valid destination (=our source) address
	if (iLen > 1)
	{
		byte = m_byaReceived.GetAt(1);
		if (byte == m_bySourceID)
		{
			if (CheckLengthAndCRC())
			{
				// Check for command
				if (iLen > 3)
				{
					byte = m_byaReceived.GetAt(3);
					if (byte == 0x0a)	// ACK/Status
					{
						// Check for Status request
						if (iLen > 6)
						{
							byte = m_byaReceived.GetAt(6);
							if (byte == 0x81)	// Status request
							{
								SendStatusResponse(m_byaReceived.GetAt(2));
							}
						}
						else
						{
							DataCorrupt(6);
						}
					}
					else // Any command, send ACK/NAK
					{
						int iResponse = theApp.GetResponse();
						if (iResponse == RESPONSE_ACK)
						{
							SendACK(m_byaReceived.GetAt(2));
						}
						else if (iResponse == RESPONSE_NAK)
						{
							SendNAK(m_byaReceived.GetAt(2));
						}
						else
						{ // ignore, no response
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
			WK_TRACE_WARNING("ControlRecordBaxall COM=%d ; Data not for us %02x<>%02x\n",
									GetCOMNumber(), byte, m_bySourceID);
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
		BYTE byLenT = m_byaReceived.GetAt(4);
		BYTE byLenR = (BYTE)(iLen - 8);
		if (byLenT == byLenR)
		{
			// Check for CRC
			int iIndexLastCrcByte = byLenT + 6;
			if (iLen > iIndexLastCrcByte)
			{
				WORD wCRC_T = m_byaReceived.GetAt(iIndexLastCrcByte);
				wCRC_T = (WORD)(wCRC_T << 8);
				wCRC_T = (WORD)(wCRC_T + m_byaReceived.GetAt(iIndexLastCrcByte-1));
				WORD wCRC_R = 0;
				int iLastDataIndex = iIndexLastCrcByte - 2;
				for (int i=2 ; i<=iLastDataIndex ; i++)
				{
					wCRC_R = (WORD)(wCRC_R + m_byaReceived.GetAt(i));
				}
				if (wCRC_R == wCRC_R)
				{
					bReturn = TRUE;
				}
				else
				{
					WK_TRACE_WARNING("ControlRecordBaxall COM=%d ; Invalid CRC %04x<>%04x\n",
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
			WK_TRACE_WARNING("ControlRecordBaxall COM=%d ; Length invalid %02x<>%02x\n",
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
	WK_TRACE_WARNING("ControlRecordBaxall COM=%d ; Data corrupt for index %i\n",
							GetCOMNumber(), iIndex);
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::SendACK(BYTE byDestID)
{
	FillHeader(byDestID);
	FillAckNakStatus(BAXALL_ACK);
	FillCRCandFooterPAD();
	Send();
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::SendNAK(BYTE byDestID)
{
	FillHeader(byDestID);
	FillAckNakStatus(BAXALL_NAK);
	FillCRCandFooterPAD();
	Send();
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::SendStatusResponse(BYTE byDestID)
{
	FillHeader(byDestID);
	FillAckNakStatus(BAXALL_STATUS_RESPONSE);
	FillCRCandFooterPAD();
	Send();
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecordBaxall::Send()
{
	if (m_byArrayBaxall.GetSize())
	{
		Write(m_byArrayBaxall.GetData(), m_byArrayBaxall.GetSize());
	}
	else
	{
		WK_TRACE_WARNING("ControlRecordBaxall COM=%d ; SendCommand without data\n",
							GetCOMNumber());
	}
}
