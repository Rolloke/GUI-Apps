// WorkSocket.h: interface for the CWorkSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKSOCKET_H__7F4E4642_A479_11D2_A9C3_004005A11E32__INCLUDED_)
#define AFX_WORKSOCKET_H__7F4E4642_A479_11D2_A9C3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HHLASocket.h"

class CRemoteControlDlg;
class CWorkSocket : public CHHLASocket  
{
public:
	CWorkSocket(CRemoteControlDlg* pWnd, const CString &sAppID, const CString &sIPAddr, int nPort, DWORD dwConnectTimeOut);
	virtual ~CWorkSocket();

	BOOL DoConfirmOnline();
	BOOL DoConfirmGetCameraParameter(WORD wCamera, int nBrightness, int nContrast, int nSaturation);
	BOOL CameraMissing(WORD wSource, BOOL bFlag);

protected:
	virtual BOOL OnReceivePacketType00(WORD wStatusCode, WORD wTimeOut, int nWorkerPort);
	virtual	BOOL OnReceivePacketType01(CLIENT_STRUCT Clients);
	virtual	BOOL OnReceivePacketType02(INDEX_STRUCT Index);
	virtual	BOOL OnReceivePacketType03(CAMERA_SET_STRUCT CameraSet);
	virtual	BOOL OnReceivePacketType04(CAMERA_PARAM_STRUCT CameraParam);
	virtual	BOOL OnReceivePacketType05(WORD wStatusCode, WORD wCamera);
	virtual BOOL OnConfirmOnline();
	virtual void OnClose(int nErrorCode);
	virtual void MinimizeAllClients();

protected:
	BOOL				m_bWaitForEchoRequest;
	CRemoteControlDlg	*m_pWnd;
	CString				m_sAppID;
	WORD				m_wCameraSet[5];	// Kameraset
	int					m_nCamSetNr;		// Counter von 0...4	
	int					m_nMaxClients;		// Anzahl der Clients 
};

#endif // !defined(AFX_WORKSOCKET_H__7F4E4642_A479_11D2_A9C3_004005A11E32__INCLUDED_)
