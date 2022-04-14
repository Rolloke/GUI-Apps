// IPCDatabaseProcess.h: interface for the CIPCDatabaseProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATABASEPROCESS_H__43C9E4A0_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_IPCDATABASEPROCESS_H__43C9E4A0_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCDatabaseProcess : public CIPCDatabase  
{
public:
	CIPCDatabaseProcess(LPCTSTR shmName);
	virtual ~CIPCDatabaseProcess();

public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();

	virtual	void OnConfirmNewSavePicture(WORD wArchivNr,CSecID camID);
	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);

	// disconnection
	virtual void	OnRequestDisconnect();
};

#endif // !defined(AFX_IPCDATABASEPROCESS_H__43C9E4A0_8B84_11D3_99EB_004005A19028__INCLUDED_)
