// IPCDatabaseTest.h: interface for the CIPCDatabaseTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATABASETEST_H__31A97A67_C9FC_4CB8_9BCF_834ACA0F5776__INCLUDED_)
#define AFX_IPCDATABASETEST_H__31A97A67_C9FC_4CB8_9BCF_834ACA0F5776__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCDatabaseTest : public CIPCDatabase  
{
public:
	CIPCDatabaseTest(const char *shmName);
	virtual ~CIPCDatabaseTest();

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

#endif // !defined(AFX_IPCDATABASETEST_H__31A97A67_C9FC_4CB8_9BCF_834ACA0F5776__INCLUDED_)
