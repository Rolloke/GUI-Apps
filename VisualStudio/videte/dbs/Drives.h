/* GlobalReplace: CDVRDrive --> CIPCDrive */
// Drives.h: interface for the CDrives class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVES_H__13AF4796_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_DRIVES_H__13AF4796_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDrives : public CIPCDrives
{
	// construction / destruction
public:
	CDrives();
	virtual ~CDrives();

public:
	CIPCDrive* GetFreeDrive();
	CIPCDrive* GetDriveEx(_TCHAR cDriveLetter);
	
	static BOOL IsRO(const CString& sRoot);
	static BOOL IsEmpty(const CString& sRoot);
	static BOOL IsRW(const CString& sRoot);

	static BOOL DoRWToRO(CIPCDrive* pDrive);
	static BOOL DoROToRW(CIPCDrive* pDrive);
};

#endif // !defined(AFX_DRIVES_H__13AF4796_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
