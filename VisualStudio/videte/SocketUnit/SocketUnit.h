// SocketUnit.h : main header file for the SOCKETUNIT application
//

#ifndef _CSocketUnitApp_H
#define _CSocketUnitApp_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define REGKEY_SOCKETUNIT	_T("SocketUnit")
#define REGKEY_ALLOW		_T("SocketUnit\\Allow")
#define REGKEY_DENY			_T("SocketUnit\\Deny")


#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitApp:
// See SocketUnit.cpp for the implementation of this class
//

class CSocketUnitApp : public CWinApp
{
public:
	CSocketUnitApp();
	virtual ~CSocketUnitApp();
	// acess:
	inline BOOL IsBeepOnIncomingEnabled() const;
	inline BOOL IsListeningEnabled() const;
	inline BOOL GetAllowOutgoingCalls() const;
	inline BOOL IsDemo() const;
	inline BOOL IsDTSTransmitter() const;

	inline DWORD   GetSelectedBitrate() const;
	inline CString GetIPAdress() const;
	BOOL	GetStatistic(double& dSentMBperSec, double& dReceiveMBperSec, int& iSendQueue);

	BOOL m_bTraceSync;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif
	inline void	SetSentBytes(int nBytes);
	inline void	SetReceivedBytes(int nBytes);

	//{{AFX_MSG(CSocketUnitApp)
	afx_msg void OnAppAbout();
	afx_msg void OnCloseAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ReadRegistrySettings();
	void WriteRegistrySettings();
	BOOL DoLoginDialog();

private:
	//
	BOOL m_bDoBeepOnIncoming;
	BOOL m_bDoListen;
	BOOL m_bAllowOutgoingCalls;

	CStringArray	m_saAllow;
	BOOL			m_bAllowAnyHost;
	CStringArray	m_saDeny;
	BOOL			m_bDenyNoHost;

	DWORD			m_dwSelectedBitrate;
	BOOL			m_bIsDemo;
	BOOL			m_bIsDTSTransmitter;
	CString			m_sIPAddress;
	CStatistics*	m_pSendRate;
	CStatistics*	m_pReceiveRate;
};

extern CSocketUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////////////
inline CSocketUnitApp * MyGetApp()
{
	return (CSocketUnitApp *)AfxGetApp();
}

/////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSocketUnitApp::IsBeepOnIncomingEnabled() const
{
	return m_bDoBeepOnIncoming;
}
/////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSocketUnitApp::IsListeningEnabled() const
{
	return m_bDoListen;
}
/////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSocketUnitApp::GetAllowOutgoingCalls() const
{
	return m_bAllowOutgoingCalls;
}

/////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSocketUnitApp::GetSelectedBitrate() const
{
	return m_dwSelectedBitrate;
}
/////////////////////////////////////////////////////////////////////////////////////
inline CString CSocketUnitApp::GetIPAdress() const
{
	return m_sIPAddress;
}
/////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSocketUnitApp::IsDemo() const
{
	return m_bIsDemo;
}
/////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSocketUnitApp::IsDTSTransmitter() const
{
	return m_bIsDTSTransmitter;
}
/////////////////////////////////////////////////////////////////////////////////////
inline void CSocketUnitApp::SetSentBytes(int nBytes)
{
	if (m_pSendRate)
	{
		m_pSendRate->AddValue(nBytes);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
inline void CSocketUnitApp::SetReceivedBytes(int nBytes)
{
	if (m_pReceiveRate)
	{
		m_pReceiveRate->AddValue(nBytes);
	}
}

#endif

