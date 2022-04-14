/* GlobalReplace: CDVRDrive --> CIPCDrive */
/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
// Archiv.h: interface for the CArchiv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCHIV_H__13AF4791_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_ARCHIV_H__13AF4791_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Sequences.h"
#include "Conversion.h"
#include "SaveData.h"
#include "VDBCameraMap.h"

#define DBP_CAMERANR		"DBP_CANR"
#define DBP_RESOLUTION		"DBP_RES"
#define DBP_COMPRESSION		"DBP_COMP"
#define DBP_COMPRESSIONTYP	"DBP_COTY"
#define DBP_TYP				"DBP_TYP"

#define DBP_IFRAME_REF		"DBP_IFR"
#define DBP_FFV_REF			"DBP_FFVR"  // fast forward video reference
#define DBP_RWV_REF			"DBP_RWVR"  // rewind video reference
#define DBP_FFA_REF			"DBP_FFAR"  // fast forward audio reference
#define DBP_RWA_REF			"DBP_RWAR"  // rewind audio reference

#define DBP_DATE			"DBP_DATE"
#define DBP_TIME			"DBP_TIME"
#define DBP_MS				"DBP_MS"
#define DBP_DATA_POS		"DBP_DPOS"
#define DBP_DATA_LEN		"DBP_DLEN"
#define DBP_CHECK_SUM		"DBP_CSUM"

#define DBP_MD_X			"DBD_MD_X"
#define DBP_MD_Y			"DBD_MD_Y"

#define DVD_MD_X			"DVD_MD_X"
#define DVD_MD_Y			"DVD_MD_Y"
#define DVD_ERROR			"DVD_ERROR"

#define DBP_MD_X2			"DBD_MD_X2"
#define DBP_MD_Y2			"DBD_MD_Y2"
#define DBP_MD_X3			"DBD_MD_X3"
#define DBP_MD_Y3			"DBD_MD_Y3"
#define DBP_MD_X4			"DBD_MD_X4"
#define DBP_MD_Y4			"DBD_MD_Y4"
#define DBP_MD_X5			"DBD_MD_X5"
#define DBP_MD_Y5			"DBD_MD_Y5"
#define DBP_MD_S			"DBD_MD_S"

#define DVD_MD_X2			"DVD_MD_X2" // motion detection coordinates
#define DVD_MD_Y2			"DVD_MD_Y2"
#define DVD_MD_X3			"DVD_MD_X3"
#define DVD_MD_Y3			"DVD_MD_Y3"
#define DVD_MD_X4			"DVD_MD_X4"
#define DVD_MD_Y4			"DVD_MD_Y4"
#define DVD_MD_X5			"DVD_MD_X5"
#define DVD_MD_Y5			"DVD_MD_Y5"
#define DVD_MD_S			"DVD_MD_S"  // tracking info

#define DVR_TYP				"DVR_TYP"  // typ
#define DVR_CAMERANR		"DVR_CANR" // camera / audio nr.
#define DVR_RESOLUTION		"DVR_RES"  // resolution
#define DVR_COMPRESSION		"DVR_COMP" // compression
#define DVR_COMPRESSIONTYP	"DVR_COTY" // compression typ

#define DVR_DATE			"DVR_DATE"
#define DVR_TIME			"DVR_TIME"
#define DVR_MS				"DVR_MS"

#define DVR_DATA_POS		"DVR_DPOS" // position/offset in dat
#define DVR_DATA_LEN		"DVR_DLEN" // l
#define DVR_CHECK_SUM		"DVR_CSUM"

#define DBP_ARCHIVENR		"DBP_ARCNR"
#define DBP_INPUTNR			"DBP_INPNR"
#define DVR_VERSION			"DVR_VER"  // version

class CArchivs;

class CArchiv  
{
	// construction / destruction
public:
	CArchiv(const CArchivInfo& ai);	// normal archive
	CArchiv(const CString& sBackupDir, 
		    WORD wArchivNr, 
			WORD wOriginalNr);		// backup archive
	virtual ~CArchiv();

	// attributes
public:
	inline WORD			GetNr() const;
	inline WORD			GetOriginalNr() const;
	inline WORD			GetSafeRingFor() const;
	inline CArchiv*     GetPreAlarm() const;
	inline ArchivType	GetType() const;
	inline CString		GetName() const;
	inline CString		GetFixedDrive() const;
	inline DWORD		GetSizeMB() const;
	inline CIPCInt64	GetSizeBytes();
		   CIPCInt64	GetBytesFree();
		   int			GetNrSequences();
	inline CIPCInt64	GetSequenceSizeBytes() const;
	inline DWORD		GetMaxStorageTime();
	inline BOOL			IsDynamicSize() const;

		   CString		GetDirectory() const;

	inline CString		GetBackupDir() const;
	inline BOOL			IsBackup() const;
	inline BOOL			IsAlarm() const;
	inline BOOL			IsAlarmList() const;
	inline BOOL			IsRing() const;
	inline BOOL			IsPreAlarm() const;
	inline BOOL			IsSearch() const;


	CSequence* GetOldestSequence();
	CSequence* GetSequenceIndex(int i);
	CSequence* GetSequenceID(WORD wID);
	CSequence* GetNextSequence(WORD wSequenceNr);
    CIPCInt64  GetSizeBytes(const CSystemTime& stStart,
							 const CSystemTime& stEnd,
							 CIPCInt64& i64Sum,
							 DWORD dwMaximumInMB,
							 BOOL& bCancelled);

	// operations
public:
	void SetName(const CString sName);
	void SetSize(DWORD dwSize);

	void InitPreAlarm(CArchivs& archives);
	void Scan(const CString& sDir, CIPCDrive* pDrive, const CVDBSequenceMap& map);
	void AdjustSize();
	void AdjustFree();
	BOOL Delete();
	BOOL DeleteAll();
	BOOL DeleteOldestSequence();
	BOOL DeleteSequence(WORD wSequence);
	BOOL DeleteAllOlder(const CSystemTime& st);
	void Finalize(BOOL bShutDown);
	void SetSizeInMB(DWORD dwNewSize);

	BOOL ConvertSequence(const CConversion& conversion);
	BOOL SaveData(CSaveData& data, BOOL& bNewSequenceCreated);
	BOOL Verify(BOOL& bCancel);
	void Trace();
	CString Format();

	inline CCriticalSection* GetSequencesCS();
	void Lock(LPCTSTR sFkt/*=NULL*/);
	void Unlock();

	void NewDirectory(const CString& sDir);
	void DeleteDirectory(const CString& sDir);

	void SetCameraName(DWORD dwID,const CString& sName);
	BOOL GetCameraName(DWORD dwID,CString& sName);
	void LoadCameraMap(const CString& sPathname);
	BOOL SaveCameraMap(const CString& sPathname);
	void DoConfirmCameraNames(CIPCDatabaseServer* pCIPC,DWORD dwUserData);
	void DeleteDatabaseDrive(const CString& sRootString);
	void DeleteDatabaseDirectory(const CString& sDir);
	void MoveSequences(CArchiv* pArchiv);

	// implementation
protected:
	void	   Init();
	void	   InitSequenceDebit();
	void	   InitFieldsIdip();
	void	   InitFieldsDTS();

	BOOL	   IsFull();
	BOOL	   Is60PercentFull();
	CSequence* CheckNewSequence(BOOL bSuspect, const CSaveData& data,WORD& wPrevSequenceNr,DWORD& dwNrOfRecords, BOOL& bNewSequenceCreated);
	CSequence* GetNewSequence(BOOL bSuspect, CSequence* pSourceSequence = NULL, BOOL bThrow=FALSE);
	CString	   InitialFormat();

	void SendAlarmArchiveFull();
	void SendAlarmArchive60();

	// private data member
private:
	// definition data
	CSecID		m_idArchiv;
	CSecID		m_idHost;
	WORD		m_wOriginalNr;
	CString		m_sName;
	ArchivType	m_Typ;
	BOOL		m_bSearch;
	DWORD		m_dwSizeMB;
	DWORD		m_iDirectoryCluster;
	WORD		m_wSafeRingFor;
	CString		m_sFixedDrive;
	DWORD		m_dwMaxStorageTime;
	BOOL		m_bDynamicSize;
	BOOL		m_bDoFieldCheck;
	volatile BOOL	m_bMoved;
	volatile BOOL	m_bMoving;

	CString		m_sDirectory;
	CString		m_sBackupDir;
	DWORD		m_dwNrOfSequencesDebit;

	CIPCInt64	m_iSizeBytes;
	CIPCInt64	m_iSequenceSizeBytes;
	BOOL		m_bAlarmFullSend;
	BOOL		m_bAlarm60Send;

	CSequences			m_Sequences;
	CString				m_sFormat;
	CCriticalSection	m_csCB;

	CVDBCameraMap	   m_mapCamera;
	CCriticalSection   m_csCameraMap;
	CString			   m_sCameraMapPathname;

	CArchiv*		   m_pPreAlarm;

public:
	Code4		m_cb;
	Field4info	m_FieldInfo;

public:
	static CStringA m_sTYP;
	static CStringA m_sDATE;
	static CStringA m_sTIME;
	static CStringA m_sMS;

	static CStringA m_sCANR;
	static CStringA m_sRES;
	static CStringA m_sCOMP;
	static CStringA m_sCOTY;
	
	static CStringA m_sDPOS;
	static CStringA m_sDLEN;
	static CStringA m_sCSUM;

	static CStringA m_sMD_X;
	static CStringA m_sMD_Y;
	static CStringA m_sMD_X2;
	static CStringA m_sMD_Y2;
	static CStringA m_sMD_X3;
	static CStringA m_sMD_Y3;
	static CStringA m_sMD_X4;
	static CStringA m_sMD_Y4;
	static CStringA m_sMD_X5;
	static CStringA m_sMD_Y5;
	static CStringA m_sMD_S;

	static CString  m_sRootName;
	static CString  m_sBackupName;
	static CString  m_sBackupInfoFilename;

	static CString m_sDbd;
};
//////////////////////////////////////////////////////////////////////
inline WORD	CArchiv::GetNr() const
{
	return m_idArchiv.GetSubID();
}
//////////////////////////////////////////////////////////////////////
inline WORD	CArchiv::GetOriginalNr() const
{
	return m_wOriginalNr;
}
//////////////////////////////////////////////////////////////////////
inline WORD	CArchiv::GetSafeRingFor() const
{
	return m_wSafeRingFor;
}
//////////////////////////////////////////////////////////////////////
inline CArchiv* CArchiv::GetPreAlarm() const
{
	return m_pPreAlarm;
}
//////////////////////////////////////////////////////////////////////
inline CString CArchiv::GetName() const
{
	return m_sName;
}
//////////////////////////////////////////////////////////////////////
inline ArchivType CArchiv::GetType() const
{
	return m_Typ;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CArchiv::GetSizeMB() const
{
	return m_dwSizeMB;
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64 CArchiv::GetSizeBytes()
{
	CIPCInt64 r = m_Sequences.GetSizeBytes();
	r += CIPCInt64(m_iDirectoryCluster); 
	return r;
}
//////////////////////////////////////////////////////////////////////
inline CString CArchiv::GetFixedDrive() const
{
	return m_sFixedDrive;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CArchiv::IsBackup() const
{
	return m_Typ == BACKUP_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CArchiv::IsAlarm() const
{
	return m_Typ == ALARM_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CArchiv::IsRing() const
{
	return m_Typ == RING_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CArchiv::IsAlarmList() const
{
	return m_Typ == ALARM_LIST_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CArchiv::IsPreAlarm() const
{
	return m_Typ == SICHERUNGS_RING_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CArchiv::IsSearch() const
{
	return m_bSearch;
}
//////////////////////////////////////////////////////////////////////
inline CString CArchiv::GetBackupDir() const
{
	return m_sBackupDir;
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64 CArchiv::GetSequenceSizeBytes() const
{
	return m_iSequenceSizeBytes;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CArchiv::GetMaxStorageTime()
{
	return m_dwMaxStorageTime;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CArchiv::IsDynamicSize() const
{
	return m_bDynamicSize;
}
inline CCriticalSection* CArchiv::GetSequencesCS()
{
	return &m_Sequences.m_cs;
}

#endif // !defined(AFX_ARCHIV_H__13AF4791_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
