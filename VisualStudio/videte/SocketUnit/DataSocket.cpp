// clntsock.cpp : implementation of the CDataSocket class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include <stddef.h>
#include "DataSocket.h"
#include "SocketUnitDoc.h"
#include "Cipc.h"
#include "CipcPipeSocket.h"
#include "CipcExtraMemory.h"
#include "CIPCOutput.h" 
#include "SocketUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
void CDataSocket::AssertValid() const
{
	CSocket::AssertValid();
}

void CDataSocket::Dump(CDumpContext& dc) const
{
	CSocket::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

//static int iDataCounter=0;

void CDataSocket::Init()
{
	m_bReceivedClose		= FALSE;

	m_wId1					= 0;
	m_wId2					= 0;
	m_dwMemory				= GetTickCount();
	m_iCounter				= 0;
	m_bCanSend				= FALSE;
	m_bConnected			= TRUE;

	m_iMaxReceiveBufferSize	= TCP_BUFFER_SIZE;
	m_iReceiveBufferSize	= 0;
	m_iSendBufferSize		= 0;
	m_iSendErrorCounter		= 0;

	m_pMaxReceiveBuffer		= new BYTE[TCP_BUFFER_SIZE];
	m_pReceiveBuffer		= NULL;
 	m_pConnection			= NULL;
	m_pPipe					= NULL;

	m_iSizeOfDataNextToRead = 7 * sizeof(DWORD);
	m_bDataIsCMDRecord      = TRUE;

	ZeroMemory((BYTE*)m_dwCMDRecordArray,7 * sizeof(DWORD));
	m_bFirstCMDReceived = FALSE;
	m_iRecvLowWatermark		= 0;

	m_dwSendBufferSize = 128*1024;
	m_pSendBuffer = new BYTE[m_dwSendBufferSize];
}

/////////////////////////////////////////////////////////////////////////////

CDataSocket::CDataSocket(CConnectionThread *pConnection)
{
	Init();
	m_pConnection = pConnection;
}

/////////////////////////////////////////////////////////////////////////////

CDataSocket::CDataSocket(CIPCPipeSocket *pPipe, CConnectionThread *pThread)
{
	Init();

	m_pPipe		  = pPipe;
	m_pConnection = pThread;
}

/////////////////////////////////////////////////////////////////////////////

void CDataSocket::OnClose(int nErrorCode)
{
	m_bCanSend = FALSE;
	WK_TRACE(_T("OnClose %s %d\n"),m_sStoredRemoteName,nErrorCode);
	if (   m_pConnection 
		&& m_pConnection->GetDoc()) 
	{
		m_pConnection->OnClose(nErrorCode);
	}
	Close();
	CSocket::OnClose(nErrorCode);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDataSocket::DoSend(	WORD wLocalId, WORD wRemoteId,
							DWORD dwCmd,
							DWORD dwParam1, DWORD dwParam2,
							DWORD dwParam3, DWORD dwParam4,
							const CIPCExtraMemory* pExtraMem
						)
{
	/*
	if (m_bAlreadyClosed) 
	{
		WK_TRACE(_T("DoSend but AlreadyClosed\n"));
		return FALSE;	// <<< EXIT>>>
	}
	*/
	if (m_bReceivedClose)
	{
		WK_TRACE(_T("Can't send %s after close\n"),CIPC::NameOfCmd(dwCmd));
		return FALSE;
	}
	
	int   numSend      = 0;

	DWORD dwShouldSend = 0;
	DWORD dwData[7];

	dwData[0] = ((DWORD)wLocalId)<<16|(wRemoteId);
	dwData[1] = dwCmd;
	dwData[2] = dwParam1;
	dwData[3] = dwParam2;
	dwData[4] = dwParam3;
	dwData[5] = dwParam4;
	dwData[6] = 0;

	if (pExtraMem)
	{
		dwShouldSend = pExtraMem->GetLength()+7*4;
		dwData[6] = pExtraMem->GetLength();
		if (dwShouldSend>m_dwSendBufferSize)
		{
			// data too large make a simple copy
			// maybe more than 1 MB setup file
			BYTE *pCopy = new BYTE[7*4+pExtraMem->GetLength()];
			// copy dwData
			::CopyMemory(pCopy,dwData,7*4);	
			// copy extra mem
			::CopyMemory(pCopy+7*4,pExtraMem->GetAddress(),pExtraMem->GetLength());
			numSend      = Send(pCopy, dwShouldSend);
			delete pCopy;
		}
		else
		{
			// copy dwData
			::CopyMemory(m_pSendBuffer,dwData,7*4);	
			// copy extra mem
			::CopyMemory(m_pSendBuffer+7*4,pExtraMem->GetAddress(),pExtraMem->GetLength());
			numSend = Send(m_pSendBuffer, dwShouldSend);
		}
	}
	else
	{
		// no extra data
		dwShouldSend = sizeof(DWORD)*7;
		numSend		 = Send(dwData,dwShouldSend);
	}
	if (numSend == SOCKET_ERROR)
	{
		int iSocketError = WSAGetLastError();
		if (   iSocketError == WSAECONNRESET
			|| iSocketError == WSAENETRESET
			|| iSocketError == WSAECONNABORTED
			|| iSocketError == WSAENETDOWN)
		{
			m_pConnection->DoTerminate();
		}
		else
		{
			WK_TRACE_ERROR(_T("Socket Error in send %d, %s %s\n"),iSocketError,GetLastErrorString(iSocketError),m_sStoredRemoteName);
		}
	}
	else if ((DWORD)numSend != dwShouldSend) 
	{
		theApp.SetSentBytes(numSend);
		WK_TRACE_ERROR(_T("SendError, only %d from %d\n"),numSend,dwShouldSend);
	}
	else
	{
		theApp.SetSentBytes(numSend);
		// OK
	}

	return ((DWORD)numSend==dwShouldSend);
}
/////////////////////////////////////////////////////////////////////////////
void CDataSocket::OnSend(int nErrorCode)
{
	DWORD dwSNDBUF = 0;
	DWORD dwRCVBUF = 0;
	int iRCVTIMEO = 0;
	int iSNDTIMEO = 0;
	BOOL bTCPNoDelay = FALSE;
	int iLen = 4;

	GetSockOpt(TCP_NODELAY,&bTCPNoDelay,&iLen);

	GetSockOpt(SO_SNDBUF,&dwSNDBUF,&iLen);
	GetSockOpt(SO_RCVBUF,&dwRCVBUF,&iLen);

	GetSockOpt(SO_RCVTIMEO,&iRCVTIMEO,&iLen);
	GetSockOpt(SO_SNDTIMEO,&iSNDTIMEO,&iLen);

	WK_TRACE(_T("OnSend %s SNDBUF=%d, RCVBUF=%d, TCP_NODELAY=%d, SO_RCVTIMEO=%d SO_SNDTIMEO=%d\n"),
		m_sStoredRemoteName,dwSNDBUF,dwRCVBUF,bTCPNoDelay,iRCVTIMEO,iSNDTIMEO);

	m_pConnection->OnSend();

	m_bCanSend = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDataSocket::DoSend()
{
	BOOL bSendOkay = TRUE;

	if (   (m_pConnection->m_sendQueue.GetCount()>0)
		  && bSendOkay
		  && !IsBlocking())
	{
		CSocketCmdRecord *pRecordToSend = m_pConnection->m_sendQueue.SafeGetAndRemoveHead();
		
		if (pRecordToSend)
		{
	
			bSendOkay = DoSend(	pRecordToSend->m_wLocalId, 
								pRecordToSend->m_wRemoteId,
								pRecordToSend->m_dwCmd,
								pRecordToSend->m_dwParam1,
								pRecordToSend->m_dwParam2,
								pRecordToSend->m_dwParam3,
								pRecordToSend->m_dwParam4,
								pRecordToSend->m_pExtraMem
								);

			if (bSendOkay)
			{
				m_iSendErrorCounter = 0;
				WK_DELETE(pRecordToSend); 
				m_iCounter=0;
			}
			else
			{
				if (pRecordToSend->m_iRetryCount<=10)
				{
					pRecordToSend->m_iRetryCount++;
					m_pConnection->AddToSendQueue(pRecordToSend,FALSE);
					pRecordToSend = NULL;
				}
				else
				{
					//Record konnte definitiv nicht versendet werden
					m_iSendErrorCounter++;
					WK_TRACE(_T("OnSend Record cannot send command %s. deleted\n"),
						CIPC::NameOfCmd(pRecordToSend->m_dwCmd));
					WK_DELETE(pRecordToSend);
				}
			}
		}
		
		if (m_iSendErrorCounter > 10 && m_bCanSend)
		{
			WK_TRACE(_T("OnSend, zuviele Fehler, Verbindung wird geschlossen!\n"));
  			m_pConnection->DoTerminate();
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDataSocket::OnReceive(int nErrorCode)
{
	if (nErrorCode)
	{
		m_pConnection->DoTerminate();
		CSocket::OnReceive(nErrorCode);
		return;
	}

	int iDataRead=0;

	if (m_iReceiveBufferSize < m_iSizeOfDataNextToRead)
	{
						   //nach ablauf von timeout wird socket aus blockierender 
						   //funktion recv oder recfrom geholt
		iDataRead = Receive((char*)m_pMaxReceiveBuffer,
							m_iMaxReceiveBufferSize);
///		TRACE(_T("OnReceive %d read %d max\n"),iDataRead,m_iMaxReceiveBufferSize);
		if(iDataRead > 0)
		{
			m_pConnection->ResetSync();
			AddToReceiveBuffer(m_pMaxReceiveBuffer,iDataRead);
		}
		else if(iDataRead == SOCKET_ERROR || iDataRead==0)
		{
			if(GetLastError() != WSAEWOULDBLOCK)
			{
				WK_TRACE(_T("OnReceive SOCKET_ERROR, possible during disconnect: %s!\n"), GetLastErrorString());
				m_pConnection->DoTerminate();
				CSocket::OnReceive(nErrorCode);
				return;
			}
			else
			{
				CSocket::OnReceive(nErrorCode);
				Sleep(10);
				return;
			}

		} 

	}

	while (m_iReceiveBufferSize >= m_iSizeOfDataNextToRead)
	{
		HandleBuffer();
	}
	
	CSocket::OnReceive(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
// CSocket Implementation

CDataSocket::~CDataSocket()
{
  	WK_DELETE(m_pReceiveBuffer);
	WK_DELETE(m_pMaxReceiveBuffer);
	WK_DELETE_ARRAY(m_pSendBuffer);
}
/////////////////////////////////////////////////////////////////////////////

CString CDataSocket::GetPipeTypeString() const
{
	CString sPipeType(_T("InvalidPipe"));

	if (m_pConnection) 
	{
		sPipeType=UserNameOfEnum(m_pConnection->GetCIPCType());
	}

	return sPipeType;
}
/////////////////////////////////////////////////////////////////////////////
void CDataSocket::HandleBuffer()
{
   	int   iSevenDWORDs    = 7 * sizeof(DWORD);
	BOOL  bDataComplete   = FALSE;
	DWORD dwDataLen       = 0;

	BYTE *pData = RemoveFromReceiveBuffer(m_iSizeOfDataNextToRead);

	if(!pData)
		return;

	if(m_bDataIsCMDRecord && m_iSizeOfDataNextToRead==iSevenDWORDs) //CMDRecord wird erwartet
	{
		for( int n=0; n<7 ; n++ )
		{
			m_dwCMDRecordArray[n] = *( (DWORD*) (pData + (n * sizeof(DWORD))));
		}
		WK_DELETE(pData);
		
		m_wId1 = (WORD) (m_dwCMDRecordArray[0]>>16);
		m_wId2 = (WORD) (m_dwCMDRecordArray[0] & 65535);

		if(m_dwCMDRecordArray[6] > 0) //Datenlänge evtl. anhängender Daten
		{
			// keep dwDataLen reasonable 20 MB max
			// DynamischeBuffer Anpassung
			if (m_dwCMDRecordArray[6] > 20*1024*1024)
			{
				WK_TRACE_ERROR(_T("Unecpected size of data %d, truncated\n"),m_dwCMDRecordArray[6]);
				m_dwCMDRecordArray[6] = 20*1024*1024;
			}

			if(m_iMaxReceiveBufferSize < (int)m_dwCMDRecordArray[6])
			{
				//m_iMaxReceiveBufferSize=m_dwCMDRecordArray[6]+iSevenDWORDs;
				//SetReceiveBufferSize(m_iMaxReceiveBufferSize);
				//m_iMaxReceiveBufferSize = GetReceiveBufferSize();
				//WK_DELETE(m_pMaxReceiveBuffer);
				//m_pMaxReceiveBuffer = new BYTE[m_iMaxReceiveBufferSize];
			}
			m_iSizeOfDataNextToRead = m_dwCMDRecordArray[6];
			bDataComplete           = FALSE;
			m_bDataIsCMDRecord      = FALSE;
		}
		else//keine anhängende Daten vorhanden
		{
			m_bDataIsCMDRecord      = TRUE;
			m_iSizeOfDataNextToRead = iSevenDWORDs;
			bDataComplete           = TRUE;
			dwDataLen               = 0;
		}
	}
	else   //anhängende Daten empfangen
	{
		dwDataLen          = m_iSizeOfDataNextToRead;
		m_bDataIsCMDRecord = TRUE;
		bDataComplete      = TRUE;
	}
	if(bDataComplete)
	{
		ConnectionThreadState state = CTS_CREATED;	// dummy
		if (m_pConnection)
		{
			state = m_pConnection->GetState();
		}
		else
		{
			WK_TRACE_ERROR(_T("Don't have CConnectionThread link!?\n"));
		}
		// action depends on the ConnectionThreads state
		switch (state)
		{
			case CTS_CONNECTED_ACTIVE:
			case CTS_CONNECTED_PASSIVE:
			case CTS_CONNECTED_ACTIVE_UNCONFIRMED:
			case CTS_CONNECTED_PASSIVE_CONFIRMED:
			case CTS_CONNECTED:
			{
				m_pConnection->HandleCmd(
										  m_wId1,
										  m_wId2,		
										  dwDataLen, 
										  pData,
										  m_dwCMDRecordArray[1],
										  m_dwCMDRecordArray[2],
										  m_dwCMDRecordArray[3],
										  m_dwCMDRecordArray[4],
										  m_dwCMDRecordArray[5]
										 );

				ZeroMemory((BYTE*)m_dwCMDRecordArray,7 * sizeof(DWORD));
				m_iSizeOfDataNextToRead = iSevenDWORDs;
				m_bDataIsCMDRecord		= TRUE;
				WK_DELETE(pData);
			}
			break;
	
			default:
			WK_TRACE_ERROR(_T("received data in state %d\n"),
							state	// NOT YET NameOfEnum
							);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

BYTE* CDataSocket::RemoveFromReceiveBuffer(int iSize)
{
	BYTE *pHelp=NULL;

	if(iSize==0)
	{
		return NULL;
	}

	BYTE *pRet = new BYTE[iSize];
	
	if(m_iReceiveBufferSize > iSize)
	{
		pHelp= new BYTE[m_iReceiveBufferSize-iSize];
		CopyMemory(pHelp,m_pReceiveBuffer+iSize,m_iReceiveBufferSize-iSize);
		CopyMemory(pRet,m_pReceiveBuffer,iSize);
	}
	else if(m_iReceiveBufferSize == iSize)
	{
		CopyMemory(pRet,m_pReceiveBuffer,iSize);
	}
	else
	{
		return NULL;
	}
	m_iReceiveBufferSize = m_iReceiveBufferSize - iSize;

	WK_DELETE(m_pReceiveBuffer);
	m_pReceiveBuffer = pHelp;

	return pRet;
}

/////////////////////////////////////////////////////////////////////////////

void CDataSocket::AddToReceiveBuffer(BYTE* pBuffer, int iBufferSize)
{
	BYTE *pHelp;
	
	if(iBufferSize==0)
	{
		return;
	}

	theApp.SetReceivedBytes(iBufferSize);

	pHelp = new BYTE[m_iReceiveBufferSize+iBufferSize];

	if(m_iReceiveBufferSize)	
	{
		CopyMemory(pHelp,m_pReceiveBuffer,m_iReceiveBufferSize);
		CopyMemory(pHelp+m_iReceiveBufferSize,pBuffer,iBufferSize);
		WK_DELETE(m_pReceiveBuffer);
	}
	else
	{	
   		CopyMemory(pHelp,pBuffer,iBufferSize);
	}
	
	m_iReceiveBufferSize = m_iReceiveBufferSize + iBufferSize;
	m_pReceiveBuffer=pHelp;
}
/////////////////////////////////////////////////////////////////////////////
CIPCPipeSocket* CDataSocket::GetCIPCPipeSocket()
{
	return m_pPipe;
}
/////////////////////////////////////////////////////////////////////////////
void CDataSocket::SetCIPCPipeSocket(CIPCPipeSocket *pPipe)
{
	m_pPipe = pPipe;
}
/////////////////////////////////////////////////////////////
void CDataSocket::SetSocketOpts()
{
	DWORD dwBufferLen = TCP_BUFFER_SIZE;
	SetSockOpt(SO_RCVBUF,&dwBufferLen,4);
	SetSockOpt(SO_SNDBUF,&dwBufferLen,4);
	BOOL bNoDelay = TRUE;
	SetSockOpt(TCP_NODELAY,&bNoDelay,4);
	int iTimeOut = 500;
	SetSockOpt(SO_RCVTIMEO,&iTimeOut,4);
	SetSockOpt(SO_SNDTIMEO,&iTimeOut,4);
}
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDataSocket, CSocket)






