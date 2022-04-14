// IPCServerToVision.h: interface for the CIPCServerToVision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERTOVISION_H__4B4E87E3_853E_4B7D_B1EB_6CFC2659B7ED__INCLUDED_)
#define AFX_IPCSERVERTOVISION_H__4B4E87E3_853E_4B7D_B1EB_6CFC2659B7ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGuardTourProcess;
#include "process.h"

class CIPCServerToVision : public CIPC  
{
	// Construction/Destruction
public:
	CIPCServerToVision(LPCTSTR szShmName, CSecID idHost);
	virtual ~CIPCServerToVision();

	// attributes
public:
	CSecID GetDestination();

	// operations
public:
	void AddProcess(CGuardTourProcess* pProcess);
	BOOL RemoveProcess(CGuardTourProcess* pProcess);
	void RemoveProcesses();
	int  GetNrOfProcesses();

	BOOL IsFetching();
	void SetFetching(BOOL bOn);

	// Overrides
public:
	virtual BOOL Run(DWORD dwTimeOut);
	virtual void OnRequestDisconnect();
	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);
	virtual void OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData);
	virtual void OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);


private:
	CSecID		m_idHost;
	CProcesses	m_GuardTourProcesses;
	DWORD		m_dwTick;
	BOOL		m_bFetching;
};
typedef CIPCServerToVision* CIPCServerToVisionPtr;
WK_PTR_ARRAY_CS(CIPCServerToVisionArray,CIPCServerToVisionPtr,CIPCServerToVisionArraySafe)


#endif // !defined(AFX_IPCSERVERTOVISION_H__4B4E87E3_853E_4B7D_B1EB_6CFC2659B7ED__INCLUDED_)
