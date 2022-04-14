// LangDllInfo.h: interface for the CLangDllInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LANGDLLINFO_H__BDE86773_1C37_11D4_BACF_00400531137E__INCLUDED_)
#define AFX_LANGDLLINFO_H__BDE86773_1C37_11D4_BACF_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLangDllInfo  
{
public:
	CLangDllInfo(const CString&	sDll,
				 const CString& sLanguage,
				 const CString& sAbbreviation,
				 const CString& sCodePage,
				 DWORD dwBits);
	virtual ~CLangDllInfo();

	inline const CString&	GetDll();
	inline const CString&	GetLanguage();
	inline const CString&	GetAbbreviation();
	inline const CString&	GetCodePage();
	inline DWORD GetCodePageBits();

protected:
	CString	m_sDll;
	CString	m_sLanguage;
	CString	m_sAbbreviation;
	CString	m_sCodePage;
	DWORD    m_dwCodePageBits;
};
//////////////////////////////////////////////////////////////////////
const CString& CLangDllInfo::GetDll()
{
	return m_sDll;
}
//////////////////////////////////////////////////////////////////////
const CString& CLangDllInfo::GetLanguage()
{
	return m_sLanguage;
}
//////////////////////////////////////////////////////////////////////
const CString& CLangDllInfo::GetAbbreviation()
{
	return m_sAbbreviation;
}
//////////////////////////////////////////////////////////////////////
const CString& CLangDllInfo::GetCodePage()
{
	return m_sCodePage;
}
DWORD CLangDllInfo::GetCodePageBits()
{
	return m_dwCodePageBits;
}
/////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CLangDllInfoArrayBase, CLangDllInfo*);
//////////////////////////////////////////////////////////////////////
class CLangDllInfoArray : public CLangDllInfoArrayBase
{
};
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_LANGDLLINFO_H__BDE86773_1C37_11D4_BACF_00400531137E__INCLUDED_)
