// Backup.h: interface for the CBackup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUP_H__FCAFEC63_C017_11D2_B57C_004005A19028__INCLUDED_)
#define AFX_BACKUP_H__FCAFEC63_C017_11D2_B57C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MFC_VER < 0x0700
typedef	DWORD FILESIZE;
#else
typedef	ULONGLONG FILESIZE;
#endif

class CBackupThread;
#include "SequenceHashEntry.h"
/////////////////////////////////////////////////////////////////////
class CBackup  
{
	// construction / destruction
public:
	CBackup(DWORD dwUserData,
			DWORD dwClientID,
			const CString& sDestinationPath,
			const CString& sName,
			int iNumIDs,
			const DWORD dwIDs[]);
	CBackup(WORD wUserData,
			DWORD dwClientID,
			const CString& sDestinationPath,
			const CString& sName,
			int iNumIDs,
			const DWORD dwIDs[],
			const CSystemTime& start,
			const CSystemTime& end,
			WORD  wFlags,
			DWORD dwMaximumInMB);
	virtual ~CBackup();

	// attributes
public:
	inline WORD	   GetFlags() const;
	inline DWORD   GetMaximumInMB() const;
	inline BOOL	   Initialized() const;
	inline DWORD   GetClientID() const;
	inline DWORD   GetUserData() const;
	inline CString GetDestinationPath() const;
	inline CString GetName() const;
	inline CString GetDirectory() const;
	inline int	   GetNrOfIDs() const;
		   DWORD   GetIDNr(int iIndex) const;

	inline CSystemTime GetStartTime() const;
	inline CSystemTime GetEndTime() const;
	inline CIPCInt64   GetCopiedBytes() const;
		   BOOL		   IsBackupToRWMedium() const;
	inline CVDBSequenceMap& GetSequenceMap();

	// operations
public:
		   BOOL Initialize();
		   BOOL WasLastTapeOfCollection(WORD wCollection, WORD wTape);
		   BOOL CleanUp();
		   void WriteSequenceMap();
		   void IncreaseCopiedBytes(WORD wCollection, WORD wTape, FILESIZE dwBytes,WORD wFlags);
		   BOOL CopyFileTrace(WORD wCollection, WORD wTape, const CString& source, const CString& dest, FILESIZE dwMaxRead=-1);
		   void SetBackupThread(CBackupThread* pBackupThread);

	// implementation
protected:
	void Init(DWORD dwUserData,
			  DWORD dwClientID,
			  const CString& sDestinationPath,
			  const CString& sName,
			  int iNumIDs,
			  const DWORD dwIDs[],
			  WORD wFlags = 0,
			  DWORD dwMaximumInMB = 0);
	BOOL MyCopyFile(WORD wCollection, WORD wTape, const CString& sSource, const CString& sDest, FILESIZE dwMaxRead=-1);

	// data member
private:
	DWORD		m_dwClientID;
	DWORD		m_dwUserData;
	CString		m_sDestinationPath;
	CString		m_sName;
	CString		m_sDirectory;
	CDWordArray	m_IDs;
	CDWordArray	m_LastTapes;
	BOOL		m_bInitialized;
	BOOL		m_bTrace;

	WORD		m_wFlags;
	DWORD		m_dwMaximumInMB;
	CSystemTime m_stStart;
	CSystemTime m_stEnd;
	CIPCInt64	m_iCopiedBytes;
	DWORD		m_dwLastConfirm;
	CBackupThread* m_pBackupThread;
	CVDBSequenceMap m_SequenceMap;
};
/////////////////////////////////////////////////////////////////////
inline BOOL CBackup::Initialized() const
{
	return m_bInitialized;
}
/////////////////////////////////////////////////////////////////////
inline DWORD CBackup::GetClientID() const
{
	return m_dwClientID;
}
/////////////////////////////////////////////////////////////////////
inline DWORD CBackup::GetUserData() const
{
	return m_dwUserData;
}
/////////////////////////////////////////////////////////////////////
inline CString CBackup::GetDestinationPath() const
{
	return m_sDestinationPath;
}
/////////////////////////////////////////////////////////////////////
inline CString CBackup::GetName() const
{
	return m_sName;
}
/////////////////////////////////////////////////////////////////////
inline int CBackup::GetNrOfIDs() const
{
	return m_IDs.GetSize();
}
/////////////////////////////////////////////////////////////////////
inline CString CBackup::GetDirectory() const
{
	return m_sDirectory;
}
/////////////////////////////////////////////////////////////////////
inline WORD	CBackup::GetFlags() const
{
	return m_wFlags;
}
/////////////////////////////////////////////////////////////////////
inline CSystemTime CBackup::GetStartTime() const
{
	return m_stStart;
}
/////////////////////////////////////////////////////////////////////
inline CSystemTime CBackup::GetEndTime() const
{
	return m_stEnd;
}
/////////////////////////////////////////////////////////////////////
inline CIPCInt64 CBackup::GetCopiedBytes() const
{
	return m_iCopiedBytes;
}
/////////////////////////////////////////////////////////////////////
inline DWORD CBackup::GetMaximumInMB() const
{
	return m_dwMaximumInMB;
}
/////////////////////////////////////////////////////////////////////
inline CVDBSequenceMap& CBackup::GetSequenceMap()
{
	return m_SequenceMap;
}
/////////////////////////////////////////////////////////////////////
typedef CBackup* CBackupPtr;
WK_PTR_LIST_CS(CBackupList,CBackupPtr,CBackups);
/////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BACKUP_H__FCAFEC63_C017_11D2_B57C_004005A19028__INCLUDED_)
