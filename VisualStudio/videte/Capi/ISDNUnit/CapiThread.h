
#ifndef _CCapiThread_H
#define _CCapiThread_H

#include <afxwin.h>

class CDataSocket;
class SocketUnitDoc;

class CISDNConnection;
class CHiddenCapiWnd;

class CCapiThread : public CWinThread
{
	friend class CHiddenCapiWnd;
public:
	CCapiThread();
	virtual int Run();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount );
	//
public:
	BOOL	m_bInitDone;
	BOOL	m_bTerminate;
	CHiddenCapiWnd *m_pCapiWnd;
	CISDNConnection *m_pConnection;
	//
	int m_iNormalOnIdleCount;
};

#endif