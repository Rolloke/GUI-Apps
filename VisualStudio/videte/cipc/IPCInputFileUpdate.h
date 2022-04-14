// IPCInputFileUpdate.h: interface for the CIPCInputFileUpdate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCFILEUPDATE_H__399E5091_58BB_11D4_9E1C_004005A19028__INCLUDED_)
#define AFX_IPCFILEUPDATE_H__399E5091_58BB_11D4_9E1C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCInput.h"
#include "ConnectionRecord.h"

#define FILE_ATTR_CONTINUED_TRANSMISSION 0x80000000

class AFX_EXT_CLASS CIPCInputFileUpdate : public CIPCInput
{
public:
	CIPCInputFileUpdate(const CConnectionRecord& c,
						LPCTSTR shmName, 
						BOOL asMaster);
	virtual ~CIPCInputFileUpdate();

	// attibutes
public:
	inline const CConnectionRecord &GetConnection() const;

	// overrides
public:
		//special:
	virtual void OnRequestFileUpdate(int iDestination,
									 const CString &sFileName,
									 const void *pData,
									 DWORD dwDataLen,
									 BOOL bNeedsReboot);

	virtual void OnRequestGetFile(int iDestination,
  								  const CString &sFileName);

	virtual void OnRequestResetApplication();

protected:
	void RequestGetFileStringInterface(const CString& sCommand);
	void SIGetLogicalDrives(const CString& sCommand);
	void SIGetDiskFreeSpace(char cDrive, const CString& sCommand);
	void SIGetOsVersion(const CString& sCommand);
	void SIGetSystemInfo(const CString& sCommand);
	void SIGlobalMemoryStatus(const CString& sCommand);
	void SIGetCurrentTime(const CString& sCommand);
	void SIGetSoftwareVersion(const CString& sCommand);
	void SIGetFileVersion(const CString& sFile,const CString& sCommand);
	void SIGetWWWDirectory(const CString& sDirectory,
					       const CString& sParameter, 
						   const CString& sCommand);

	void SIExportRegistry(const CString& sCommand);
	BOOL SIImportRegistry(const CString& sCommand, 
						  const void *pData,
						  DWORD dwDataLen);

	BOOL SIReset(const CString& sCommand);
	void SIGetTimeZoneInformation(const CString& sCommand);
	
	void SIDoConfirmWWWFile(const CString& sPathname);
	void SIGetproductInfo(const CString&sCommand);

	void SIEnumRegKeys(const CString&sC);
	void SIEnumRegValues(const CString&sC);
	void SIGetRegKey(const CString&sC);
	void SISetRegKey(const CString&sC);
	void SIDeleteRegKey(const CString&sC);

public:
	static BOOL m_bAllowGetRegistry;

protected:
	CConnectionRecord m_ConnectionRecord;
};
///////////////////////////////////////////////////////////////////////////////
inline const CConnectionRecord& CIPCInputFileUpdate::GetConnection() const
{
	return m_ConnectionRecord;
}

#endif // !defined(AFX_IPCFILEUPDATE_H__399E5091_58BB_11D4_9E1C_004005A19028__INCLUDED_)
