// CIPCDatabaseIdipClientLocal.h: interface for the CIPCDatabaseIdipClientLocal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDATABASE_IDIPCLIENT_LOCAL_H__7DA22864_AE9E_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_CIPCDATABASE_IDIPCLIENT_LOCAL_H__7DA22864_AE9E_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CIdipClientDoc;

class CIPCDatabaseIdipClientLocal : public CIPCDatabaseClient  
{
public:
	CIPCDatabaseIdipClientLocal(CIdipClientDoc* pDoc , LPCTSTR szShmName);
	~CIPCDatabaseIdipClientLocal();

public:
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual void OnConfirmFieldInfo(int iNumRecords, const CIPCField data[]);
	virtual void OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	virtual void OnIndicateNewSequence(const CIPCSequenceRecord& data, WORD  wPrevSequenceNr, DWORD dwNrOfRecords);

private:
	CIdipClientDoc* m_pDoc;
};

#endif // !defined(AFX_CIPCDATABASE_IDIPCLIENT_LOCAL_H__7DA22864_AE9E_11D1_8C1B_00C095EC9EFA__INCLUDED_)
