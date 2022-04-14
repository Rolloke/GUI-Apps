// Drives.h: interface for the CIPCDrives class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDRIVES_H__13AF4796_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_CIPCDRIVES_H__13AF4796_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCDrive.h"
#include "wk_template.h"
/////////////////////////////////////////////////////////////////////////////
typedef CIPCDrive* CIPCDrivePtr;
WK_PTR_ARRAY_CS(CIPCDriveArray,CIPCDrivePtr,CIPCDriveArrayCS)

class CIPC;
class CIPCExtraMemory;
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCDrives : public CIPCDriveArrayCS
{
	// construction / destruction
public:
	CIPCDrives();
	virtual ~CIPCDrives();

	// attributes
public:
	DWORD		GetTotalHDSizeMB();
	DWORD		GetMB();
	DWORD		GetAvailableMB();
	DWORD		GetUsedMB();
	CIPCInt64	GetUsedBytes();
	DWORD		GetFreeMB();
	const CString& GetFailedDrives() const;

	// operations
public:
	void FromArray(int iNumDrives, const CIPCDrive drives[]);
	void Init(CWK_Profile& wkProfile);
	void CheckSpace();
	
	CIPCDrive* GetDrive(char c);
	CIPCDrive* GetDrive(const CString& sRoot);

	void Trace();
	void Save(CWK_Profile& wkProfile);

	// bubble code
public:
	CIPCExtraMemory* BubbleFromDrives(CIPC *pCipc) const;
	static CIPCDrive* DrivesFromBubble(int iNumRecords,
									   const CIPCExtraMemory *pExtraMem);
	// implementation
private:
	BOOL Load(CWK_Profile& wkProfile);
	BOOL CheckForOld30RegFormat(CWK_Profile& wkp);
// Data
	CString m_sFailedDrives;
};

#endif // !defined(AFX_DRIVES_H__13AF4796_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
