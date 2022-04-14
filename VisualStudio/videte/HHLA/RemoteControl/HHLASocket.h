#if !defined(AFX_MYSOCKET_H__4AEF1C62_8E83_11D2_8CA4_004005A11E32__INCLUDED_)
#define AFX_MYSOCKET_H__4AEF1C62_8E83_11D2_8CA4_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HHLASocket.h : header file
//
#include <afxsock.h>
#include "RemoteControlDef.h"
#include "Packet.h"

#define STX		0x02
#define ETX		0x03
#define CR  	0x0d
#define LF		0x0a

/////////////////////////////////////////////////////////////////////////////

class CPacket;
class CHHLASocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CHHLASocket();
	CHHLASocket(const CString &sIPAddr, int nPort, DWORD dwConnectTimeOut);
	virtual ~CHHLASocket();

	BOOL IsValid();
	BOOL IsConnected();
	BOOL DoConfirmPacket();
	BOOL DoRequestSendPacket(const CStringA &sPacket);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHHLASocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CHHLASocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	virtual BOOL OnReceivePacketType00(WORD wStatusCode, WORD wTimeOut, int nWorkerPort);
	virtual	BOOL OnReceivePacketType01(CLIENT_STRUCT Clients);
	virtual	BOOL OnReceivePacketType02(INDEX_STRUCT Index);
	virtual	BOOL OnReceivePacketType03(CAMERA_SET_STRUCT CameraSet);
	virtual	BOOL OnReceivePacketType04(CAMERA_PARAM_STRUCT CameraParam);
	virtual	BOOL OnReceivePacketType05(WORD wStatusCode, WORD wCamera);
	virtual BOOL OnConfirmOnline();

	CPacket	m_Packet;
private:
	void	Init();
	BOOL	OnReceivePacket(CPacket &Packet);
	BOOL	SendData(char *pMsg, DWORD dwLen);

	CString		m_sIPAddr;
	int			m_nPort;
	BOOL		m_bOK;
	BOOL		m_bConnected;
	BOOL		m_bReadingInProzess;
	CStringA	m_sInputBuffer;
	CStringA	m_sLastPacket;
	BOOL		m_bTraceHexDump;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSOCKET_H__4AEF1C62_8E83_11D2_8CA4_004005A11E32__INCLUDED_)
