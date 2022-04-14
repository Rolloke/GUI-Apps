// SDISocket.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "SDISocket.h"
#include "SDIRS232.h"
#include "SDITestDoc.h"

// CSDISocket

CSDISocket::CSDISocket()
{
	m_pDocument = NULL;
}

CSDISocket::~CSDISocket()
{
	m_SendData.SafeDeleteAll();
}
void CSDISocket::SetDocument(CSDITestDoc* pDoc)
{
	m_pDocument = pDoc;
}


// CSDISocket member functions

void CSDISocket::OnSend(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_SendData.GetSize())
	{
		CByteArray* pByteArray = m_SendData.GetAt(0);
		if (   m_pDocument->m_bTraceRS232DataAscii
			|| m_pDocument->m_bTraceRS232DataHex)
		{
			CSDIRS232 tracer(0,m_pDocument->m_bTraceRS232DataAscii,m_pDocument->m_bTraceRS232DataHex,FALSE);
			tracer.TraceData(pByteArray->GetData(),pByteArray->GetSize(),FALSE);
		}
		DWORD dwSend = Send(pByteArray->GetData(),pByteArray->GetSize());
		if (dwSend == (DWORD)pByteArray->GetSize())
		{
			TRACE(_T("sent %d Bytes\n"),dwSend);
			m_SendData.RemoveAt(0);
			WK_DELETE(pByteArray);
		}
		else
		{
			int e = WSAGetLastError();
			WK_TRACE(_T("cannot send data %s\n"),GetLastErrorString(e));
		}
	}
	else
	{
		TRACE(_T("nothing to send\n"));
	}



	CAsyncSocket::OnSend(nErrorCode);
}
////////////////////////////////////////////
void CSDISocket::AddSendData(const CByteArray& bytes)
{
	CByteArray* pByteArray = new CByteArray;
	pByteArray->Append(bytes);
	m_SendData.Add(pByteArray);
}

void CSDISocket::OnConnect(int nErrorCode)
{
	TRACE(_T("CSDISocket::OnConnect\n"));

	CAsyncSocket::OnConnect(nErrorCode);
}
