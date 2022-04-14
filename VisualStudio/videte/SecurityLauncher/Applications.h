// Applications.h: interface for the CApplications class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONS_H__BF857DA1_C7C0_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_APPLICATIONS_H__BF857DA1_C7C0_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Application.h"

class CMeszMez;

class CApplications : public CTypedPtrArray<CPtrArray,CApplication*>
{
	// construction/destruction
public:
	CApplications();
	virtual ~CApplications();

	// attributes
public:
	CApplication* GetApplication(const CString& sPath);
	CApplication* GetApplication(WK_ApplicationId applicationId);
	int  GetNrOfBChannels(WK_ApplicationId waiExcept);

	// operations
public:
	BOOL InitAppsAndDongle(CWK_Dongle* pDongle, CMeszMez* pMeszMez, int nMode, BOOL bUseUH);
	void StartAll();
	void TerminateAll();
	void DeleteAll();

	// implementation
private:
	BOOL IsFileForCocoUnit(const CString& exe);
	BOOL IsFileForMicoUnit(const CString& exe);

private:
	CWK_Dongle* m_pDongle;
	int			m_nMode;
};

#endif // !defined(AFX_APPLICATIONS_H__BF857DA1_C7C0_11D1_8C1B_00C095EC9EFA__INCLUDED_)
