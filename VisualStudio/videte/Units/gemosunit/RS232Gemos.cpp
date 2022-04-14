// RS232Gemos.cpp: implementation of the CRS232Gemos class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GemosUnit.h"
#include "RS232Gemos.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRS232Gemos::CRS232Gemos()
{
	SetTraceRS232DataAscii(FALSE);
	SetTraceRS232DataHex(FALSE);
	SetTraceRS232Events(FALSE);

	CWK_Profile Prof;
	m_bTraceENQ			= (BOOL)Prof.GetInt(szSection, _T("TraceENQ"),	0);
	m_bTraceACK			= (BOOL)Prof.GetInt(szSection, _T("TraceACK"),	0);
	m_bTraceNAK			= (BOOL)Prof.GetInt(szSection, _T("TraceNAK"),	0);
	m_bTraceSYN			= (BOOL)Prof.GetInt(szSection, _T("TraceSYN"),	0);
	m_bTraceDC3			= (BOOL)Prof.GetInt(szSection, _T("TraceDC3"),	0);
	m_bTraceEOT			= (BOOL)Prof.GetInt(szSection, _T("TraceEOT"),	0);
	m_bTraceCMD			= (BOOL)Prof.GetInt(szSection, _T("TraceCMD"),	0);

	m_eState = GEMOS_STX;
	m_bCalculatedCheckSum = 0;
	m_hSYNEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hACKEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hNAKEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hDC3Event = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_nMaxENQRepeats		= 4;
	m_nMaxNAKRepeats		= 4;
	m_nMaxTimeOutRepeats	= 4;
}

//////////////////////////////////////////////////////////////////////
CRS232Gemos::~CRS232Gemos()
{
	CWK_Profile Prof;
	Prof.WriteInt(szSection, _T("TraceENQ"),			m_bTraceENQ);
	Prof.WriteInt(szSection, _T("TraceACK"),			m_bTraceACK);
	Prof.WriteInt(szSection, _T("TraceNAK"),			m_bTraceNAK);
	Prof.WriteInt(szSection, _T("TraceSYN"),			m_bTraceSYN);
	Prof.WriteInt(szSection, _T("TraceDC3"),			m_bTraceDC3);
	Prof.WriteInt(szSection, _T("TraceEOT"),			m_bTraceEOT);
	Prof.WriteInt(szSection, _T("TraceCMD"),			m_bTraceCMD);

	WK_CLOSE_HANDLE(m_hSYNEvent);
	WK_CLOSE_HANDLE(m_hACKEvent);
	WK_CLOSE_HANDLE(m_hNAKEvent);
	WK_CLOSE_HANDLE(m_hDC3Event);
}

//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	if (pData)
	{
		for (DWORD i=0;i<dwLen;i++)
		{
			OnReceivedData(pData[i]);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("pData == NULL\n"));
	}
}

//////////////////////////////////////////////////////////////////////
DWORD CRS232Gemos::WriteByte(BYTE b)
{
//	WK_TRACE(_T("writing single byte thread %08lx, %02x\n"),GetCurrentThreadId(),b);
	return Write(&b,1);
}

//////////////////////////////////////////////////////////////////////
DWORD CRS232Gemos::WriteString(const CString &sCmd)
{
	if (sCmd.GetLength())
	{
		CWK_String s(sCmd); 
		return Write((void*)(LPCSTR)s, sCmd.GetLength());
	}
	WK_TRACE(_T("try to write empty string\n"));
	return 0;
}

//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnReceivedData(BYTE data)
{
	switch (data)
	{
	case ASCII_ENQ:
		OnENQ();
		break;
	case ASCII_ACK:
		OnACK();
		break;
	case ASCII_NAK:
		OnNAK();
		break;
	case ASCII_SYN:
		OnSYN();
		break;
	case ASCII_DC3:
		OnDC3();
		break;
	case ASCII_EOT:
		OnEOT();
		break;
	default:
		OnPackage(data);
		break;
	}
}

//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnENQ()
{
	if (m_bTraceENQ)
		WK_TRACE(_T("ENQ received answering SYN\n"));
	WriteByte(ASCII_SYN);
}

//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnACK()
{
	if (m_bTraceACK)
		WK_TRACE(_T("ACK received\n"));
	if (m_hACKEvent)
	{
		SetEvent(m_hACKEvent);
	}
	else
	{
		WK_TRACE(_T("m_hACKEvent == NULL\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnNAK()
{
	if (m_bTraceNAK)
		WK_TRACE(_T("NAK received\n"));
	if (m_hNAKEvent)
	{
		SetEvent(m_hNAKEvent);
	}
	else
	{
		WK_TRACE(_T("m_hNAKEvent == NULL\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnSYN()
{
	if (m_bTraceSYN)
		WK_TRACE(_T("SYN received\n"));
	if (m_hSYNEvent)
	{
		SetEvent(m_hSYNEvent);
	}
	else
	{
		WK_TRACE(_T("m_hSYNEvent == NULL\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnDC3()
{
	if (m_bTraceDC3)
		WK_TRACE(_T("DC3 received\n"));
	if (m_hDC3Event)
	{
		SetEvent(m_hDC3Event);
	}
	else
	{
		WK_TRACE(_T("m_hDC3Event == NULL\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnEOT()
{
	if (m_bTraceEOT)
		WK_TRACE(_T("EOT received\n"));
}

//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnPackage(BYTE data)
{
	switch(m_eState)
	{
	case GEMOS_STX:
//		WK_TRACE(_T("rs232 thread is %08lx\n"),GetCurrentThreadId());
		if (ASCII_STX == data)
		{
			m_eState = GEMOS_DATA;
			m_bCalculatedCheckSum = 0;
			m_sTransmittedCheckSum.Empty();
			m_sReceiveCommand.Empty();
		}
		else
		{
			WK_TRACE_ERROR(_T("<STX>!=<%x>\n"),data);
		}
		break;
	case GEMOS_DATA:
		m_bCalculatedCheckSum ^= data;
		if (ASCII_ETX == data)
		{
			m_eState = GEMOS_BCC1;
		}
		else
		{
			m_sReceiveCommand += data;
			if (m_sReceiveCommand.GetLength()>255)
			{
				WK_TRACE_ERROR(_T("data too long\n"));
			}
		}
		break;
	case GEMOS_BCC1:
		m_sTransmittedCheckSum = (char)data;
		m_eState = GEMOS_BCC2;
		break;
	case GEMOS_BCC2:
		{
			unsigned int iTransmittedCheckSum = 0;
			m_sTransmittedCheckSum += data;
			if (1 == _stscanf(m_sTransmittedCheckSum,_T("%02x"),&iTransmittedCheckSum))
			{
				if ((unsigned int)m_bCalculatedCheckSum == iTransmittedCheckSum)
				{
					WK_TRACE(_T("checksum ok\n"));
					ProcessCommand();
					WriteByte(ASCII_ACK);
				}
				else
				{
					WK_TRACE_ERROR(_T("wrong checksum\n"));
					WriteByte(ASCII_NAK);
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot scan checksum\n"));
			}
		}
		WK_TRACE(_T("waiting for new command\n"));
		m_eState = GEMOS_STX;
		break;
	default:
		WK_TRACE_ERROR(_T("invalid RS232 state %d\n"),(int)m_eState);
		break;
	}
}

//////////////////////////////////////////////////////////////////////
void CRS232Gemos::ProcessCommand()
{
	// das aktuelle Kommando
	if (m_bTraceCMD)
		WK_TRACE(_T("processing command <%s>\n"),m_sReceiveCommand);

	// Kommando in die KommandoQueue hängen und den MainThread benachrichtigen.
	CString psCmd(m_sReceiveCommand);
	
	m_csFetchCmd.Lock();
	m_sReceiveCommandList.Add(psCmd);
	m_csFetchCmd.Unlock();

	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_CMD_RECEIVED, 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CRS232Gemos::SendCommand(const CString &sSendCommand)
{
	DWORD	dwCmdLen	= sSendCommand.GetLength();
	DWORD	dwRet		= 0;

	// Vorgegebene Maximallänge überschritten ?
	if (dwCmdLen > 255)
	{
		OnErrorReceived(_T("SendCommand: Command too long (>255)\n"));
		return FALSE;
	}
	
	// Wiederhole 'm_nMaxTimeOutRepeats'-mal bei Timeout.
	int nTimeOutCnt = 0;
	do
	{
		// Wiederhole 'm_nMaxENQRepeats'-mal, bei fehlendem SYN bzw DC3
		int nENQCnt = 0;	
		do
		{
			WriteByte(ASCII_ENQ);

			// Warte auf SYN oder DC3 bzw. Timeout
			HANDLE hEvents[2];
			hEvents[0] = m_hSYNEvent;
			hEvents[1] = m_hDC3Event;
			dwRet = WaitForMultipleObjects(2, hEvents, FALSE, 500);
		}
		while (((dwRet == WAIT_OBJECT_0+1) ||
			    (dwRet == WAIT_TIMEOUT))   &&
				(nENQCnt++ < m_nMaxENQRepeats));
		
		if (dwRet == WAIT_OBJECT_0+1)
		{
			OnErrorReceived(_T("SendCommand: Buffer full. Give up\n"));
			return FALSE;
		}
		if (dwRet == WAIT_TIMEOUT)
		{
			OnErrorReceived(_T("SendCommand: Timeout! No SYN received. Give up\n"));
			return FALSE;
		}

		// Wiederhole 'm_nMaxNAKRepeats'-mal bei erhalt von NAK
		int nNAKCnt = 0;	
		do
		{
			// Startzeichen senden
			if (WriteByte(ASCII_STX) != 1)
				OnErrorReceived(_T("SendCommand: Can't send STX\n"));

			// Kommando senden
			if (WriteString(sSendCommand) != dwCmdLen)
			{
				OnErrorReceived(_T("SendCommand: Can't send command\n"));
				return FALSE;
			}

			// Endzeichen senden
			if (WriteByte(ASCII_ETX) != 1)
			{
				WK_TRACE_ERROR(_T("Can't send ETX\n"));
			}

			// Prüfsumme bestimmen
			BYTE byBCC = 0;
			for (DWORD dwI = 0; dwI < dwCmdLen; dwI++)
				byBCC ^= (BYTE)sSendCommand.GetAt(dwI);

			byBCC ^= ASCII_ETX;				    
			CString sBCC;
			sBCC.Format(_T("%02x"), byBCC);
			
			// Prüfsumme schicken
			if (WriteString(sBCC) != (DWORD)sBCC.GetLength())
			{
				WK_TRACE_ERROR(_T("Can't send BCC\n"));
			}

			// Warte auf ACK oder NAK bzw. Timeout
			HANDLE hEvents[2];
			hEvents[0] = m_hACKEvent;
			hEvents[1] = m_hNAKEvent;
			dwRet = WaitForMultipleObjects(2, hEvents, FALSE, 200);
		}
		while ((dwRet == WAIT_OBJECT_0+1) && (nNAKCnt++ < m_nMaxNAKRepeats));
		if (dwRet == WAIT_OBJECT_0+1)
		{
			OnErrorReceived(_T("SendCommand: Timeout! No ACK received. Give up\n"));
			return FALSE;
		}
	}
	while((dwRet == WAIT_TIMEOUT) && (nTimeOutCnt++ < m_nMaxTimeOutRepeats));
	if (dwRet == WAIT_TIMEOUT)
	{
		OnErrorReceived(_T("SendCommand: Timeout! No response. Give up\n"));
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CRS232Gemos::OnErrorReceived(const CString &sErr)
{
	WK_TRACE_ERROR(sErr);
}

//////////////////////////////////////////////////////////////////////
CString CRS232Gemos::GetAndRemoveCommand()
{
	m_csFetchCmd.Lock();

	int nIndex		= m_sReceiveCommandList.GetUpperBound();
	CString psCmd	= m_sReceiveCommandList.GetAt(nIndex);
	
	m_sReceiveCommandList.RemoveAt(nIndex);

	CString  sCmd(psCmd);
	
	m_csFetchCmd.Unlock();	
	
	return sCmd;
}

//////////////////////////////////////////////////////////////////////
BOOL CRS232Gemos::TestingConnection()
{
	DWORD	dwRet		= 0;

	// Wiederhole 'm_nMaxENQRepeats'-mal, bei fehlendem SYN bzw DC3
	int nENQCnt = 0;	
	do
	{
		WriteByte(ASCII_ENQ);

		// Warte auf SYN oder DC3 bzw. Timeout
		HANDLE hEvents[2];
		hEvents[0] = m_hSYNEvent;
		hEvents[1] = m_hDC3Event;
		dwRet = WaitForMultipleObjects(2, hEvents, FALSE, 500);
	}
	while (((dwRet == WAIT_OBJECT_0+1) ||
			(dwRet == WAIT_TIMEOUT))   &&
			(nENQCnt++ < m_nMaxENQRepeats));
	
	if (dwRet == WAIT_OBJECT_0+1)
	{
		OnErrorReceived(_T("SendCommand: Buffer full. Give up\n"));
		return FALSE;
	}
	if (dwRet == WAIT_TIMEOUT)
	{
		OnErrorReceived(_T("SendCommand: Timeout! No SYN received. Give up\n"));
		return FALSE;
	}

	// EOT senden
	WriteByte(ASCII_EOT);

	return TRUE;
}
