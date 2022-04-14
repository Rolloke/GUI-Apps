// EndcontrolRegKeySave.h: interface for the CEndcontrolRegKeySave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENDCONTROLREGKEYSAVE_H__C654FF64_4655_11D3_8CBC_004005A1D890__INCLUDED_)
#define AFX_ENDCONTROLREGKEYSAVE_H__C654FF64_4655_11D3_8CBC_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEndcontrolRegKeySave  
{
public:
	CString m_sRegPathToSave;
	DWORD m_dwDataLen;
	LPBYTE m_pbData;
	DWORD m_dwType;
	DWORD m_dwValueNameLen;
	LPSTR m_pcValueName;
	CEndcontrolRegKeySave();
	virtual ~CEndcontrolRegKeySave();


};

#endif // !defined(AFX_ENDCONTROLREGKEYSAVE_H__C654FF64_4655_11D3_8CBC_004005A1D890__INCLUDED_)
