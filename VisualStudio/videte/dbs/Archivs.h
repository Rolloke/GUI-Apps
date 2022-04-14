/* GlobalReplace: CDVRDrive --> CIPCDrive */
/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
// Archivs.h: interface for the CArchivs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCHIVS_H__13AF4794_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_ARCHIVS_H__13AF4794_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Archiv.h"

/////////////////////////////////////////////////////////////////////////////
typedef CArchiv* CArchivPtr;
WK_PTR_ARRAY_CS(CArchivArray,CArchivPtr,CArchivArrayCS)
/////////////////////////////////////////////////////////////////////////////
class CArchivs : public CArchivArrayCS
{
	// construction / destruction
public:
	CArchivs();
	virtual ~CArchivs();

	// attributes
public:
	CArchiv*  GetArchiv(WORD wNr);
	CArchiv*  GetSafeRingArchiv(WORD wNr);
	DWORD	  GetMB(BOOL bAlsoFixedDrives=TRUE);
	int		  GetAlarmPercent();
	CIPCInt64 GetUsedBytes();
	int		  GetNrOfSequences();

	DWORD	 GetFixedMB(CIPCDrive* pDrive);
	DWORD	 GetFixedUseMB(CIPCDrive* pDrive);

	CSequence*	  GetNextSequence(WORD wArchivNr,WORD wSequenceNr);

	int		  GetNrOfDynamicArchives();
	int		  GetNrOfFixedArchives();
	DWORD	  GetFixedSizeDefinedMB();
	DWORD	  GetFixedSizeUsedMB();
	DWORD	  GetAverageDynamicSequenceSizeMB();
	void	  DeleteOldestDynamicSequence();

	// operations
public:
	void Init(CWK_Profile& wkp);
	void Trace();
	void ScanBackup();
	void ScanBackupDrive(CIPCDrive* pDrive);
	void Finalize(BOOL bShutdown);
	BOOL DeleteArchive(WORD wArchivNr);
	void CheckBackupDirectories();
	BOOL CloseAll();
	void DeleteDatabaseDrive(const CString& sRootString);
	void ScanDatabaseDrive(CIPCDrive* pDrive);

	// implementation
private:
	BOOL Load(CWK_Profile& wkProfile);
	void ScanDatabase();
	BOOL ConvertDrive(CIPCDrive* pDrive);
	void ScanBackupDirectory(const CString& sBackupDir, CIPCDrive* pDrive);

	CArchiv*  CreateBackupArchiv(const CString& sBackupDir, WORD wOriginalNr);
	CArchiv* GetNextArchive(WORD wArchiveNr);

private:
	DWORD  m_dwDirectoryCluster;
	CArchiv*		m_pLastGetArchive;
	CStringArray	m_saBackupDirectories;
	DWORD			m_dwTickLastClose;
};

#endif // !defined(AFX_ARCHIVS_H__13AF4794_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
