// CIPCInputServerCall.h: interface for the CIPCInputServerCall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINPUTSERVERCALL_H__EF225B64_F7E8_11D2_B5D7_004005A19028__INCLUDED_)
#define AFX_CIPCINPUTSERVERCALL_H__EF225B64_F7E8_11D2_B5D7_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMultipleCallProcess;
class CIPCOutputServerCall;

class CIPCInputServerCall : public CIPCInput
{
public:
	CIPCInputServerCall(CMultipleCallProcess* pProcess,LPCTSTR shmName);
	virtual ~CIPCInputServerCall();

	// attributes
public:
	inline BOOL IsReady();
	inline BOOL IsError();

	// operations
public:
	void RequestImages();
	void RequestSetTime();

	// overrides
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	//!ic! server confirm for input information
	virtual void OnConfirmInfoInputs(WORD wGroupID, 
									 int iNumGroups,
									 int iNumRecords, 
									 const CIPCInputRecord records[]);

	virtual void OnConfirmGetFile(
				int iDestination,
				const CString &sFileName,
				const void *pData,
				DWORD dwDataLen, DWORD dwCodePage);

	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage);

	virtual	void OnConfirmSetSystemTime();
	virtual void OnRequestDisconnect();
	virtual void OverrideError();

	// implementation
protected:
	void OnConfirmFile(const CString &sFileName, const void *pData, DWORD dwDataLen);
	void OnConfirmDirectory(const CString &sFileName, const void *pData, DWORD dwDataLen);
	void CheckEnd();

	// data member
private:
	CMultipleCallProcess* m_pProcess;
	BOOL			   m_bIsReady;
	BOOL			   m_bError;
	int				   m_iNrOfFiles;
	CString			   m_sName;
	BOOL			   m_bGotActualDir;
	BOOL			   m_bGotReferenceDir;
};
////////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputServerCall::IsReady()
{
	return m_bIsReady;
}
////////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputServerCall::IsError()
{
	return m_bError;
}

#endif // !defined(AFX_CIPCINPUTSERVERCALL_H__EF225B64_F7E8_11D2_B5D7_004005A19028__INCLUDED_)
