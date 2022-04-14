// RunService.h: interface for the CRunService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNSERVICE_H__35F25571_D91A_11D1_B4F3_00C095EC9EFA__INCLUDED_)
#define AFX_RUNSERVICE_H__35F25571_D91A_11D1_B4F3_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CRunService  
{
	// construction/destruction
public:
	CRunService();
	virtual ~CRunService();

	// operations
public:
	void RunServices();
	BOOL WriteRun(const CString& sKey,const CString& sValue);
	BOOL WriteRunOnce(const CString& sKey,const CString& sValue);
	BOOL ClearRun(const CString& sKey);
	BOOL ClearRunOnce(const CString& sKey);

private:
	BOOL WriteEntry(const CString& sSection, 
				   const CString& sKey, 
				   const CString& sValue);
	BOOL ClearEntry(const CString& sSection, 
					const CString& sKey);

};

#endif // !defined(AFX_RUNSERVICE_H__35F25571_D91A_11D1_B4F3_00C095EC9EFA__INCLUDED_)
