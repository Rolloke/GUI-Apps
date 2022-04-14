// MCIDevice.h: interface for the CMCIDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MCIDEVICE_H__0488AD43_483A_11D4_8602_004005A26A3B__INCLUDED_)
#define AFX_MCIDEVICE_H__0488AD43_483A_11D4_8602_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mmsystem.h"



class CMCIDevice  
{
public:
	void iocontrol();
	BOOL DeviceIsReady();
	BOOL MediaInserted();
	DWORD CloseCdromDevice();
	BOOL CloseCDCaddy();
	BOOL OpenCDCaddy();
	void SetMainWnd(CWnd* pWnd);
	BOOL OpenCdromDevice(CString sDriveletter="");
	MCIDEVICEID GetID();
	CMCIDevice();
	virtual ~CMCIDevice();

private:
	BOOL m_bDeviceClosed;
	CString m_sDriveletter;
	CWnd* m_pMainWnd;
	MCIDEVICEID m_DeviceID;
};

#endif // !defined(AFX_MCIDEVICE_H__0488AD43_483A_11D4_8602_004005A26A3B__INCLUDED_)
