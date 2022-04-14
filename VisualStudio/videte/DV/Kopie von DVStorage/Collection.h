//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLECTION_H)
#define AFX_COLLECTION_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Sequences.h"
#include "ImageData.h"

#define DVR_VERSION			"DVR_VER"  // version
#define DVR_CAMERANR		"DVR_CANR" // camera / audio nr.
#define DVR_RESOLUTION		"DVR_RES"  // resolution
#define DVR_COMPRESSION		"DVR_COMP" // compression
#define DVR_COMPRESSIONTYP	"DVR_COTY" // compression typ
#define DVR_TYP				"DVR_TYP"  // typ

#define DVR_DATE			"DVR_DATE" // datum
#define DVR_TIME			"DVR_TIME" // uhrzeit
#define DVR_MS				"DVR_MS"   // millisekunden

#define DVR_DATA_POS		"DVR_DPOS" // position/offset in dat
#define DVR_DATA_LEN		"DVR_DLEN" // l
#define DVR_CHECK_SUM		"DVR_CSUM"
#define DBP_DATE			"DBP_DATE"
#define DBP_TIME			"DBP_TIME"

#define DBP_IFRAME_REF		"DBP_IFR"	// I Frame reference
#define DBP_FFV_REF			"DBP_FFVR"  // fast forward video reference
#define DBP_RWV_REF			"DBP_RWVR"  // rewind video reference
#define DBP_FFA_REF			"DBP_FFAR"  // fast forward audio reference
#define DBP_RWA_REF			"DBP_RWAR"  // rewind audio reference

#define DBD_MD_X			"DBD_MD_X"	// motion detection coordinates
#define DBD_MD_Y			"DBD_MD_Y"
#define DVD_MD_X			"DVD_MD_X"
#define DVD_MD_Y			"DVD_MD_Y"
#define DVD_MD_S			"DVD_MD_S"  // tracking info
#define DVD_ERROR			"DVD_ERROR"

#define DVD_MD_X2			"DVD_MD_X2" // motion detection coordinates
#define DVD_MD_Y2			"DVD_MD_Y2"
#define DVD_MD_X3			"DVD_MD_X3"
#define DVD_MD_Y3			"DVD_MD_Y3"
#define DVD_MD_X4			"DVD_MD_X4"
#define DVD_MD_Y4			"DVD_MD_Y4"
#define DVD_MD_X5			"DVD_MD_X5"
#define DVD_MD_Y5			"DVD_MD_Y5"

#define DVR_MAX_QUERY_RESULTS "DVR_MAX_QUERY_RESULTS"

class CCollection : public CSequences //, public CWK_Thread
{
	// construction / destruction
public:
	CCollection(const CArchivInfo& ai);
	CCollection(const CString& sBackupDir, WORD wCollectionNr);
	virtual ~CCollection();

	// attributes
public:
	inline WORD			GetNr() const;
	inline WORD			GetSafeRingFor() const;
	inline ArchivType	GetType() const;
	inline CString		GetName() const;
	inline DWORD		GetSizeMB();
	inline DWORD		GetTapeSizeBytes() const;

		   CString		GetFolder() const;
	inline CString		GetBackupDir() const;

	inline BOOL			IsAlarm() const;
	inline BOOL			IsAlarmList() const;
	inline BOOL			IsRing() const;
	inline BOOL			IsPreAlarm() const;
	inline BOOL			IsBackup() const;

		   DWORD	    GetNumberOfPictures();
		   CIPCInt64	GetSizeBytes();
		   CIPCInt64	GetSizeBytes(const CSystemTime& stStart,
									 const CSystemTime& stEnd,
									 CIPCInt64& i64Sum,
									 DWORD dwMaximumInMB,
									 BOOL& bCancelled);

	// attributes
public:
	CTape* GetFirstTape(CIPCDrive* pDrive);
	CTape* GetFirstSequence(const CSystemTime& s);
	CTape* GetTapeIndex(int i);
	CTape* GetTapeID(WORD wID);
	CTape* GetOldestTape();
	CTape* GetNextSequence(WORD wSequenceNr);
	void	SetNr(WORD wID) {m_idCollection.Set(SECID_NO_GROUPID, wID);}
	inline CCriticalSection*GetCS();

	// operations
public:
	void	   SetName(const CString sName);
	void	   SetSize(DWORD dwSize);
	void	   AddScannedTape(CTape* pTape);
	CTape*	   AddNewTape(BOOL bSuspect, CIPCDrive* pDrive);
	BOOL	   RemoveTape(WORD wID);

	void Scan(const CString& sDir, CIPCDrive* pDrive, BOOL bBackup, BOOL bIndicate,const CVDBSequenceMap& map);
	void DeleteDatabaseDrive(const CString& sRootString);
	void DeleteDatabaseDirectory(const CString& sDir);
	void AdjustSize();
	void AdjustFree();
	BOOL Delete();
	BOOL DeleteAll();
	void CloseAll();
	BOOL DeleteAllOlder(const CSystemTime& st);
	BOOL DeleteTape(WORD wTape);
	BOOL DeleteOldestTape();
	void SetFirstSequenceAfterRestart();

	void Trace();
	CString Format();

	void NewFolder(const CString& sDir);
	void DeleteFolder(const CString& sDir);

	// storage thread
	BOOL Store(const CImageData& data, BOOL& bNewSequenceCreated);
	BOOL Verify(BOOL& bCancel);

	// implementation
protected:
	void	   Init();
	void	   InitTapeDebit();
	void	   InitFields();

	BOOL	   IsFull();
	BOOL	   Is60PercentFull();
	CTape*	   CheckNewTape(BOOL bSuspect, 
							BOOL& bNewSequenceCreated, 
							const CImageData& data,
							WORD& wPrevSequenceNr,
							DWORD& dwNrOfRecords);
	CTape*	   GetNewTape(BOOL bSuspect, CTape* pSourceTape = NULL);
	void	   MoveTapes(CCollection* pCollection);
	CString	   InitialFormat();

	WORD	   GetNewID();

	void SendAlarmFull();
	void SendAlarm60();

	// private data member
private:
	// definition data
	CSecID		m_idCollection;
	CString		m_sName;
	ArchivType	m_Typ;
	BOOL		m_bSearch;
	DWORD		m_dwSizeMB;
	DWORD		m_iFolderCluster;
	WORD		m_wSafeRingFor;
	CString		m_sFolder;
	DWORD		m_dwNrOfTapesDebit;
	BOOL		m_bDoFieldCheck;
	BOOL		m_bMoved;
	DWORD		m_dwLastMoveTick;
	BOOL		m_bFullSend;
	BOOL		m_b60Send;
	BOOL		m_bFirstSequenceAfterRestart;

	CIPCInt64	m_iSizeBytes;
	DWORD		m_iTapeSizeBytes;
	CString		m_sBackupDir;

	CString				m_sFormat;
	CCriticalSection	m_csCollection;
	CCriticalSection	m_csCB;

	WORD		m_wTapeID;

/*
	CImageDataQueue	m_ImageDataQueue;
	DWORD			m_dwMaxInQueue;
*/

public:
	Code4		m_CodeBase;
	Field4info	m_FieldInfo;
};
//////////////////////////////////////////////////////////////////////
inline WORD	CCollection::GetNr() const
{
	return m_idCollection.GetSubID();
}
//////////////////////////////////////////////////////////////////////
inline WORD	CCollection::GetSafeRingFor() const
{
	return m_wSafeRingFor;
}
//////////////////////////////////////////////////////////////////////
inline CString CCollection::GetName() const
{
	return m_sName;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCollection::GetSizeMB()
{
	return m_dwSizeMB;
}
//////////////////////////////////////////////////////////////////////
inline ArchivType CCollection::GetType() const
{
	return m_Typ;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CCollection::IsAlarm() const
{
	return m_Typ == ALARM_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CCollection::IsAlarmList() const
{
	return m_Typ == ALARM_LIST_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CCollection::IsRing() const
{
	return m_Typ == RING_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CCollection::IsPreAlarm() const
{
	return m_Typ == SICHERUNGS_RING_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CCollection::IsBackup() const
{
	return m_Typ == BACKUP_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline CString CCollection::GetBackupDir() const
{
	return m_sBackupDir;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCollection::GetTapeSizeBytes() const
{
	return m_iTapeSizeBytes;
}
//////////////////////////////////////////////////////////////////////
inline CCriticalSection*CCollection::GetCS()
{
	return &m_csCollection;
}

#endif // !defined(AFX_Collection_H__13AF4791_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
