// CHHLA.h: interface for the CHHLA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHHLA_H__B634A781_A54B_11D2_A9C3_004005A11E32__INCLUDED_)
#define AFX_CHHLA_H__B634A781_A54B_11D2_A9C3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConnectSocket.h"
#include "WorkSocket.h"

class CConnectSocket;
class CWorkSocket;
class CHHLA  
{
public:
	CHHLA(CRemoteControlDlg *pWnd);
	virtual ~CHHLA();
	BOOL DoConnection(const CString &sIPAddr, int nPort, const CString& sAppID);
	BOOL DoConfirmOnline();
	BOOL DoConfirmGetCameraParameter(WORD wCamera, int nBrightness, int nContrast, int nSaturation);
	BOOL CameraMissing(WORD wSource, BOOL bFlag);

private:
	CConnectSocket		*m_pCSocket;
	CWorkSocket			*m_pWSocket;
	int					m_nWorkPort;
	TIMEOUT_STRUCT		m_TimeOut;
	CRemoteControlDlg	*m_pWnd;
};

#endif // !defined(AFX_CHHLA_H__B634A781_A54B_11D2_A9C3_004005A11E32__INCLUDED_)
