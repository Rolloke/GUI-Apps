// BackupScanThread.h: interface for the CBackupScanThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPSCANTHREAD_H__1F93EA0F_A47A_4294_B332_C59D7BB43E1C__INCLUDED_)
#define AFX_BACKUPSCANTHREAD_H__1F93EA0F_A47A_4294_B332_C59D7BB43E1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBackupScanThread : public CWK_Thread  
{
public:
	CBackupScanThread(CIPCDrive* pDrive, BOOL bIndicateToClients);
	virtual ~CBackupScanThread();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);

private:
	CIPCDrive* m_pDrive; 
	BOOL	   m_bIndicateToClients;
};

#endif // !defined(AFX_BACKUPSCANTHREAD_H__1F93EA0F_A47A_4294_B332_C59D7BB43E1C__INCLUDED_)
