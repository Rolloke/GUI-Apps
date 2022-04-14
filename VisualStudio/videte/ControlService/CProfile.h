// CProfile.h: interface for the CProfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPROFILE_H__0F655F58_546E_11D3_8DB1_004005A11E32__INCLUDED_)
#define AFX_CPROFILE_H__0F655F58_546E_11D3_8DB1_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProfile  
{
public:
	CProfile(const CString &sSection);
	virtual ~CProfile();

	// read a BOOL flag from the profile, or return default if not found.
	virtual BOOL ReadProfileFlag(const CString &sEntry, BOOL bDefault);

	 // write a boolean value to the registry, if it is not the
	 // same as the default or the value already there
	virtual void WriteProfileFlag(const CString &sEntry, BOOL bValue);

	 // read a UINT from the profile, or return default if not found.
	virtual DWORD ReadProfileInt(const CString &sEntry, DWORD dwDefault);

	 // write a UINT to the profile, if it is not the same as the default or the value already there
	virtual void WriteProfileInt(const CString &sEntry, DWORD dwValue);

	 // read a string from the profile into pResult. result is number of bytes written into pResult
	virtual DWORD ReadProfileString(const CString &sEntry, const CString &sDefault, CString &sResult);

	 // write a string to the profile
	virtual void WriteProfileString(const CString &sEntry, const CString &sData);
		    
	 // read binary values from the profile into pResult.result is number of bytes written into pResult
	virtual DWORD ReadProfileBinary(const CString &sEntry, LPCVOID pDefault, LPVOID pResult, DWORD dwSize);

	 // write binary data to the profile
	virtual void WriteProfileBinary(const CString &sEntry, LPCVOID pData, DWORD dwData);  

private:
	CString	m_sSection;
	HKEY 	m_hRootKey;
};

#endif // !defined(AFX_CPROFILE_H__0F655F58_546E_11D3_8DB1_004005A11E32__INCLUDED_)
