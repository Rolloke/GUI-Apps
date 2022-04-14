#if !defined(AFX_LANGDLL_H__111983A7_1C34_11D4_BACF_00490532157E__INCLUDED_)
#define AFX_LANGDLL_H__111983A7_1C34_11D4_BACF_00490532157E__INCLUDED_

//////////////////////////////////////////////////////////////////////
typedef LPCTSTR (APIENTRY* GET_LANGUAGE_NAME_PTR)(void);
typedef LPCTSTR (APIENTRY* GET_LANGUAGE_ABBREVIATION_PTR)(void);
typedef LPCTSTR (APIENTRY* GET_CODE_PAGE_PTR)(void);
typedef DWORD   (APIENTRY* GET_CODE_PAGE_BITS_PTR)(void);

extern "C" AFX_EXT_API LPCTSTR GetLanguageName(void);
extern "C" AFX_EXT_API LPCTSTR GetLanguageAbbreviation(void);
extern "C" AFX_EXT_API LPCTSTR GetCodePage(void);
extern "C" AFX_EXT_API DWORD   GetCodePageBits(void);

#endif // !defined(AFX_LANGDLL_H__111983A7_1C34_11D4_BACF_00490532157E__INCLUDED_)
