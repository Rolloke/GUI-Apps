// CIPCOutputServerCall.h: interface for the CIPCOutputServerCall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCOUTPUTSERVERCALL_H__648371E3_FA43_11D2_B5D9_004005A19028__INCLUDED_)
#define AFX_CIPCOUTPUTSERVERCALL_H__648371E3_FA43_11D2_B5D9_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMultipleCallProcess;

class CIPCOutputServerCall : public CIPCOutputClient  
{
public:
	CIPCOutputServerCall(CMultipleCallProcess* pProcess,LPCTSTR shmName);
	virtual ~CIPCOutputServerCall();

	// attributes
public:
	inline BOOL IsReady();

	// operations
public:

	// overrides
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[]);

	virtual void OnRequestDisconnect();
	virtual void OnAddRecord(const CIPCOutputRecord &pRec);
	virtual void OnUpdateRecord(const CIPCOutputRecord &pRec);
	virtual void OnDeleteRecord(const CIPCOutputRecord &pRec);
	virtual void OverrideError();

	// data member
private:
	CMultipleCallProcess* m_pProcess;
	BOOL				  m_bIsReady;
	CString				  m_sName;
};

inline BOOL CIPCOutputServerCall::IsReady()
{
	return m_bIsReady;
}

#endif // !defined(AFX_CIPCOUTPUTSERVERCALL_H__648371E3_FA43_11D2_B5D9_004005A19028__INCLUDED_)
