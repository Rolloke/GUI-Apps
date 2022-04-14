// IPCDatabaseTest.h: interface for the CIPCDatabaseTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATABASETEST_H__C13369DF_8673_11D5_9A29_004005A19028__INCLUDED_)
#define AFX_IPCDATABASETEST_H__C13369DF_8673_11D5_9A29_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDVSTestDlg;
class CIPCDatabaseTest : public CIPCDatabaseClient  
{
public:
	CIPCDatabaseTest(CDVSTestDlg* pDlg, LPCTSTR shmName);
	virtual ~CIPCDatabaseTest();

public:
	inline CIPCSequenceRecords& GetSequences();

	inline CIPCFields&	GetSequenceFields();
	inline CIPCFields&	GetSequenceRecords();

public:
	virtual void OnRequestDisconnect();
	virtual void OnConfirmConnection();
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[]);
	virtual	void OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
									   const CIPCSequenceRecord data[]);
	virtual	void OnConfirmBackupByTime(WORD wUserData,
									   DWORD dwID,
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD wFlags,
									   CIPCInt64 i64Size);
	virtual	void OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID);
	virtual void OnIndicateNewSequence(const CIPCSequenceRecord& data);
	virtual void OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	virtual	void OnConfirmRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 CIPCFields fields,
									 CIPCFields records);
	virtual	void OnConfirmAlarmListArchives(int iNumRecords, 
											const CIPCArchivRecord data[]);

private:
	CDVSTestDlg*		m_pDlg;
	CIPCSequenceRecords m_Sequences;

	CIPCFields			m_SequenceFields;
	CIPCFields			m_SequenceRecords;
};
inline CIPCSequenceRecords& CIPCDatabaseTest::GetSequences()
{
	return m_Sequences;
}
inline CIPCFields& CIPCDatabaseTest::GetSequenceFields()
{
	return m_SequenceFields;
}
inline CIPCFields& CIPCDatabaseTest::GetSequenceRecords()
{
	return m_SequenceRecords;
}

#endif // !defined(AFX_IPCDATABASETEST_H__C13369DF_8673_11D5_9A29_004005A19028__INCLUDED_)
