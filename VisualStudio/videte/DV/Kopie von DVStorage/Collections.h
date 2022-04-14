/* GlobalReplace: CDVRDrive --> CIPCDrive */
/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
// Collections.h: interface for the CCollections class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CollectionS_H__13AF4794_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_CollectionS_H__13AF4794_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Collection.h"

/////////////////////////////////////////////////////////////////////////////
typedef CCollection* CCollectionPtr;
WK_PTR_ARRAY_CS(CCollectionArray,CCollectionPtr,CCollectionArrayCS);
/////////////////////////////////////////////////////////////////////////////
class CCollections : public CCollectionArrayCS
{
	// construction / destruction
public:
	CCollections();
	virtual ~CCollections();

	// attributes
public:
	CCollection*  GetCollectionByCameraNr(BYTE bNr);
	CCollection*  GetCollection(WORD wNr);
	CCollection*  GetSafeRingCollection(WORD wNr);
	DWORD	  GetMB();
	DWORD	  GetSequenceSizeInMB();
	int		  GetAlarmPercent();
	CIPCInt64 GetUsedBytes();
	int		  GetNrOfSequences();
	int		  GetNrOfRingArchives();
	int		  GetNrOfAlarmArchives();

	DWORD	  GetAlarmAndPreAlarmDefinedMB();
	DWORD	  GetAlarmAndPreAlarmUsedMB();

	CTape*	  GetNextSequence(WORD wArchivNr,WORD wSequenceNr);

	// operations
public:
	void Init(CWK_Profile& wkp);
	void Trace();
	void ScanBackupDrive(CIPCDrive* pDrive, BOOL bIndicateToClients);
	BOOL DeleteCollection(WORD wCollectionNr);
	void ScanDatabaseDrive(CIPCDrive* pDrive);
	void DeleteDatabaseDrive(const CString& sRootString);
	void DeleteOldestRingSequence();
	void CheckBackupDirectories();
	void CloseAll();
	void SafeAdd(CCollection*);

	// implementation
private:
	BOOL Load(CWK_Profile& wkProfile);
	void ScanDatabase();
	void ScanBackupDirectory(const CString& sBackupDir, CIPCDrive* pDrive, BOOL bIndicateToClients);
	CCollection* CreateBackupCollection(const CString& sBackupDir, WORD wOriginalNr);
	CCollection* GetNextArchive(WORD wArchiveNr);
	static int __cdecl CCollectionsComp(const void *p1, const void *p2);

private:
	DWORD			m_dwDirectoryCluster;
	DWORD			m_dwSequenceSizeInMB;
	CCollection*	m_pLastGetCollection;
	CStringArray	m_saBackupDirectories;
	DWORD			m_dwTickLastClose;
};

#endif // !defined(AFX_CollectionS_H__13AF4794_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
