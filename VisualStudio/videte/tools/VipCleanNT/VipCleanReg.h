// VipCleanReg.h: interface for the CVipCleanReg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIPCLEANREG_H__668C1243_FFCA_11D3_A9D7_004005A26A3B__INCLUDED_)
#define AFX_VIPCLEANREG_H__668C1243_FFCA_11D3_A9D7_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SECTION_DVRT SOFTWARE_KEY _T("\\") WK_PROFILE_ROOT

#define HKLM_ENV_TEMP		_T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment")
#define ENV_TEMP			_T("TEMP")

class CVipCleanReg  
{
public:
	BOOL DelKey(CString SubKey);
	BOOL GetDisplaySettings(DWORD &dwHeigth,DWORD &dwwidth,DWORD &colour, DWORD &dwHZ);
	void SetNTLoaderTimeout(int iTimeout);
	int GetNTLoaderTimeout();
	BOOL RemoveTempDir();
	BOOL CreateTempDir();
	BOOL CheckTemp();
	BOOL DelDir();
	BOOL DelFiles(CString sdir);
	CString GetKey(CString KeyAdress, CString KeyName, DWORD Regtype=REG_SZ,HKEY MainKey = HKEY_LOCAL_MACHINE);
	BOOL SetKey(CString KeyAdress,CString KeyName,BYTE* pValue, DWORD dwlen, DWORD Regtype=REG_SZ);
	BOOL SetInt(const CString &KeyAddress,const CString &KeyName, int nValue);
	int GetInt(const CString &KeyAddress,const CString &KeyName, int nDefault);

	CVipCleanReg();
	virtual ~CVipCleanReg();


};

#endif // !defined(AFX_VIPCLEANREG_H__668C1243_FFCA_11D3_A9D7_004005A26A3B__INCLUDED_)
