
#ifndef _CWK_String_H_
#define _CWK_String_H_

#include "wk.h"
#include "WK_Template.h"

class CWK_String : public CString
{
public:
	inline CWK_String();
	inline CWK_String(const CString &sSrc);
	inline CWK_String(const char *szText);
	//
	inline BOOL Contains(const CString &sPattern) const;
};

// cast/copy constructor
inline CWK_String::CWK_String(const char *szText) : CString(szText)
{
	// no extra data
}
inline CWK_String::CWK_String() : CString()
{
	// no extra data
}


// cast/copy constructor
inline CWK_String::CWK_String(const CString &sSrc) : CString(sSrc)
{
	// no extra data
}

inline BOOL CWK_String::Contains(const CString &sPattern) const
{
	return (Find(sPattern) != -1);
}

typedef CWK_String *CWK_StringPtr;

WK_PTR_ARRAY_CS(CWK_StringArrayPlain,CWK_StringPtr,CWK_StringArray);

class CProtorStringArray : public CWK_StringArray
{
	public:
	inline BOOL AlreadyHas(const CString &sPattern);
	inline int RemoveByPattern(const CString &sPattern);
};

inline BOOL CProtorStringArray::AlreadyHas(const CString &sPattern)
{
	for (int i=0;i<GetSize();i++) {
		if (GetAt(i)->Contains(sPattern)) {
			return TRUE;
		}
	}

	return FALSE;
}

inline int CProtorStringArray::RemoveByPattern(const CString &sPattern)
{
	int iNumRemoved=0;
	for (int i=0;i<GetSize();i++) {
		if (GetAt(i)->Contains(sPattern)) {
			CWK_String *pTmp = GetAt(i);
			RemoveAt(i);
			i--; // adjust loop
			WK_DELETE(pTmp);

			iNumRemoved++;
		}
	}
	return iNumRemoved;
}
#endif
