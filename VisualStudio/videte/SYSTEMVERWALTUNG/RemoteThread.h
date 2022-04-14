// RemoteThread.h: interface for the CRemoteThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTETHREAD_H__ABB398C4_B45B_11D2_B565_004005A19028__INCLUDED_)
#define AFX_REMOTETHREAD_H__ABB398C4_B45B_11D2_B565_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRemoteDialog;
class CIPCInputSV;
class CIPCDatabaseSV;

class CRemoteThread : public CWK_Thread  
{
	// construction/destruction
public:
	CRemoteThread();
	virtual ~CRemoteThread();

	// attributes
public:
	BOOL HasSettings() const;
	BOOL HasDrives() const;

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
	
			void Cancel();

public:
	CRemoteDialog* m_pRemoteDialog;
	CString		   m_sRegistryFile;
	CString		   m_sCoCoISAini;
	UINT		   m_nRemoteCodePage;
	CIPCDrives	   m_Drives;
	BOOL		   m_bSend;

private:
	CIPCInputSV*	m_pCIPCInputSV;
	CIPCDatabaseSV*	m_pCIPCDatabaseSV;
	BOOL			m_bCancelled;
	CIPCFetch		m_Fetch;
};

#endif // !defined(AFX_REMOTETHREAD_H__ABB398C4_B45B_11D2_B565_004005A19028__INCLUDED_)
