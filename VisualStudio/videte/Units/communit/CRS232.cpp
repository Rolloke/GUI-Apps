// CRS232.cpp

#include "stdafx.h"
#include "CommUnit.h"
#include "CRS232.h"
#include "WKClasses/wk_wincpp.h"
#include "ControlRecordUni1.h"

#define COM_BUFFER 4096

extern CCommUnitApp theApp;

//////////////////////////////////////////////////////////////////////
CRS232::CRS232(int iCom)
{
	m_hCommFile = NULL;
	m_bOpen = FALSE;
	m_COMNumber = iCom;
	memset(&m_dcb,0,sizeof(DCB));
	m_dcb.DCBlength = sizeof(DCB);

	m_pReadBuffer = NULL;
	m_dwBufferLen = 0;

	m_OLRead.Offset = 0;
	m_OLRead.OffsetHigh = 0;
	m_OLRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_OLWrite.Offset = 0;
	m_OLWrite.OffsetHigh = 0;
	m_OLWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
}
//////////////////////////////////////////////////////////////////////
CRS232::~CRS232()
{

	if (IsOpen())
		Close();

    WK_CLOSE_HANDLE(m_OLRead.hEvent);
    WK_CLOSE_HANDLE(m_OLWrite.hEvent);
	DeleteReadBuffer();
}
/////////////////////////////////////////////////////////////////////////////
void CRS232::DeleteReadBuffer()
{
	if (m_pReadBuffer)
	{
		free(m_pReadBuffer);
		m_pReadBuffer = NULL;
	}
	m_dwBufferLen = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRS232::Open(CameraControlType eType, DWORD BaudRate/* = CBR_9600*/,
				  BYTE ByteSize/* = 8*/,
				  BYTE Parity/* = NOPARITY*/,
				  BYTE StopBits/* = ONESTOPBIT*/)
{
	CString sCom;
	CString sError;

	if (m_hCommFile)
		return FALSE;

    // open COMM device
	sCom.Format(_T("COM%d"),m_COMNumber);

	CControlRecordUni1 cr(m_COMNumber, FALSE, eType);
	cr.ReadPortValues(BaudRate, ByteSize, Parity, StopBits);

	COsVersionInfo osInfo;
	if ( osInfo.IsNT() ) {
		if ( 9 < m_COMNumber ) {
			sCom.Format(_T("\\\\.\\COM%d"),m_COMNumber);
		}
	}
	m_hCommFile = CreateFile(	sCom,
								GENERIC_READ | GENERIC_WRITE,
								0,    // exclusive access
								NULL, // no security attrs
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // overlapped I/O
								NULL );
	
	if (m_hCommFile == (HANDLE)-1 )
	{
		sError = GetLastErrorString();
		CString sMessage;
		sMessage.LoadString(IDP_NO_CONNECTION);
		sMessage += _T('\n');
		sMessage += sCom;
		sMessage += _T('\n');
		sMessage += sError;
		AfxMessageBox(sMessage,MB_ICONSTOP|MB_OK);
		m_hCommFile = NULL;
		WK_TRACE_ERROR(sMessage);
		return FALSE;
	}

	GetCommState(m_hCommFile, &m_dcb);
    m_dcb.BaudRate = BaudRate;
    m_dcb.ByteSize = ByteSize;
    m_dcb.Parity = Parity;
    m_dcb.StopBits = StopBits;

	if (eType == CCT_RS232_TRANSPARENT)
	{
		CWK_Profile wkp;
		CString sReg = cr.GetRegPath();
		CString sValue = _T("EvtChar");
		int nEvtChar = wkp.GetInt(sReg, sValue, -1);
		if (nEvtChar == -1)
		{
			nEvtChar = 0;
			wkp.WriteInt(sReg, sValue, nEvtChar);
		}
		if (nEvtChar)
		{
			WK_TRACE(_T("%s:%d\n"), sValue, nEvtChar);
		}
		m_dcb.EvtChar = (char) nEvtChar;
	}

	CString sP,sSB;
	switch (Parity)
	{
		case EVENPARITY: sP = _T("EVENPARITY");break;
		case MARKPARITY: sP = _T("MARKPARITY");break;
		case NOPARITY: sP = _T("NOPARITY");break;
		case ODDPARITY: sP = _T("ODDPARITY");break;
	}
	switch (StopBits)
	{
		case ONESTOPBIT: sSB = _T("1 StopBit"); break;
		case ONE5STOPBITS: sSB = _T("1,5 StopBits"); break;
		case TWOSTOPBITS: sSB = _T("2 StopBits"); break;
	}
	WK_TRACE(_T("open %s,%dBaud,%dBit,%s,%s\n"),
			(LPCTSTR)sCom,BaudRate,ByteSize,(LPCTSTR)sP,(LPCTSTR)sSB);

	if (!Setup())
	{
		return FALSE;
	}

	if (!StartThread())
	{
		return FALSE;
	}

	m_bOpen = TRUE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRS232::Setup()
{
	COMMTIMEOUTS  CommTimeOuts ;

    if (!m_hCommFile)
	    return FALSE;

	// get any early notifications
	if (!SetCommMask(m_hCommFile, EV_RXCHAR))
		return FALSE;
	
	// setup device buffers
	if (!SetupComm(m_hCommFile, COM_BUFFER, COM_BUFFER))
		return FALSE;

	// purge any information in the buffer
	PurgeComm(m_hCommFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
    CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0 ;
    CommTimeOuts.WriteTotalTimeoutConstant = 1000 ;
    SetCommTimeouts(m_hCommFile, &CommTimeOuts) ;

    return SetCommState(m_hCommFile, &m_dcb) ;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRS232::StartThread()
{
	AfxBeginThread(WatchCommProc,(LPVOID)this);
	return TRUE;
}
////////////////////////////////////////////////////////////////////
void CRS232::Read()
{
	BOOL    fRS ;
	COMSTAT ComStat ;
	DWORD   dwEF;
	DWORD   dwL;
	DWORD   dwE;

	if (!m_bOpen)
		return;

	// only try to read number of bytes in queue 
	ClearCommError(m_hCommFile, &dwEF, &ComStat ) ;
	dwL = ComStat.cbInQue;

	if (dwL==0)
		return;

	DeleteReadBuffer();
	m_pReadBuffer = malloc(dwL);
	m_dwBufferLen = dwL;

	fRS = ReadFile(m_hCommFile,m_pReadBuffer,dwL,&dwL,&m_OLRead);

	if (!fRS)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// We have to wait for read to complete. 
			// This function will timeout according to the
			// CommTimeOuts.ReadTotalTimeoutConstant variable
			// Every time it times out, check for port errors
			while(!GetOverlappedResult( m_hCommFile, &m_OLRead, &dwL, TRUE ))
			{
				dwE = GetLastError();
				if (dwE == ERROR_IO_INCOMPLETE)
					// normal result if not finished
					continue;
				else
				{
					// an error occurred, try to recover
					ClearCommError(m_hCommFile, &dwEF, &ComStat);
					break;
				}
			}
		}

		DeleteReadBuffer();
	}
	else
	{
		// post that we read data to main thread
		if (theApp.m_bTraceData)
		{
			theApp.TraceData(m_COMNumber,m_pReadBuffer,m_dwBufferLen);
		}
		theApp.OnReadData(m_COMNumber);
	}
}
/////////////////////////////////////////////////////////////////////////////
DWORD CRS232::Write(LPVOID pBuffer, DWORD dwLen)
{
	if ((m_bOpen) && (dwLen>0))
	{
		DWORD dwL = dwLen;

		if (WriteFile(m_hCommFile,pBuffer,dwLen,&dwL,&m_OLWrite))
		{
			if (theApp.m_bTraceData)
			{
				theApp.TraceData(m_COMNumber,pBuffer,dwLen,FALSE);
			}
			return dwL;
		}
		else
		{
			DWORD   dwErrorFlags;
			DWORD	dwLE = GetLastError();
			BOOL	bWriteStatus = FALSE;
			COMSTAT ComStat;

			if (dwLE == ERROR_IO_PENDING)
			{
				// We have to wait for write to complete. 
				// This function will timeout according to the
				// CommTimeOuts.WriteTotalTimeoutConstant variable
				// Every time it times out, check for port errors
				while (FALSE==(bWriteStatus=GetOverlappedResult(m_hCommFile,&m_OLWrite,&dwLen,TRUE))) 
				{
					dwLE = GetLastError();
					if (dwLE == ERROR_IO_INCOMPLETE) {
						// normal result if not finished
						continue;
					}
					else 
					{
						// an error occurred, try to recover
						ClearCommError(m_hCommFile, &dwErrorFlags, &ComStat);
						break;
					}
				}
				if (bWriteStatus && theApp.m_bTraceData)
				{
					theApp.TraceData(m_COMNumber,pBuffer,dwLen,FALSE);
				}
			}
			else
			{
				// OOPS Anderer Fehler
				// an error occurred, try to recover
				ClearCommError(m_hCommFile, &dwErrorFlags, &ComStat);
				WK_TRACE(_T("cannot write data to COM%d %s\n"),m_COMNumber,GetLastErrorString(dwLE));
			}
		}
	}

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CRS232::Close()
{
	if (!m_hCommFile)
		return;

	m_bOpen = FALSE;

	SetCommMask(m_hCommFile, 0);

	// drop DTR
	EscapeCommFunction(m_hCommFile, CLRDTR ) ;

	// purge any outstanding reads/writes and close device handle
	PurgeComm(m_hCommFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	CloseHandle(m_hCommFile) ;
	m_hCommFile = NULL;
	WK_TRACE(_T("close COM%d\n"),m_COMNumber);
	Sleep(100);
}
/////////////////////////////////////////////////////////////////////////////
UINT CRS232::WatchCommProc(LPVOID pParam)
{
	CRS232* pCom = (CRS232*)pParam;
	DWORD       dwEvtMask ;
	HANDLE		hEvent;

	// create I/O event used for overlapped read
	hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	if (!SetCommMask(pCom->m_hCommFile, pCom->m_dcb.EvtChar != 0 ? EV_RXFLAG : EV_RXCHAR))
	{
		return FALSE;
	}

	while (pCom->m_bOpen)
	{
		dwEvtMask = 0 ;

		WaitCommEvent(pCom->m_hCommFile, &dwEvtMask, NULL );

		if (  ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
		    ||((dwEvtMask & EV_RXFLAG) == EV_RXFLAG))
		{
			pCom->Read();
		}
	}

    CloseHandle(hEvent);

	return 1;
}
