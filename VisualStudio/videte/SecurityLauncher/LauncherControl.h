/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: LauncherControl.h $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/LauncherControl.h $
// CHECKIN:		$Date: 9.02.04 12:56 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 9.02.04 12:53 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CLauncherControl_H_
#define _CLauncherControl_H_


class CLauncherControl : public CIPCServerControl
{
	// construction / destruction
public:
	CLauncherControl();
	~CLauncherControl();

	// attributes
public:
	BOOL IsTimedOut();

	// overrides
public:
	virtual BOOL Run(DWORD dwTimeOut);
	virtual void OnRequestFileUpdate(int iDestination,
									 const CString &sFileName,
									 const void *pData,
									 DWORD dwDataLen,
									 BOOL bNeedsReboot);

private:
	DWORD m_dwTick;
};

#endif // _CLauncherControl_H_
