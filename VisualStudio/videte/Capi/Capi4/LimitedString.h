

#ifndef _CLimitedString_H
#define _CLimitedString_H

#define S_UNLIMITED -1

#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden
#include <windows.h>

class CLimitedString 
{
public:
	CLimitedString();
	CLimitedString(const char *pText);
	CLimitedString(const wchar_t *pText);

	CLimitedString(int iMaxLen, const char *pText);
	CLimitedString(int iMaxLen, const wchar_t *pText);

	virtual ~CLimitedString();

	// access:
	inline BOOL IsEmpty() const;
	inline int	GetLength() const;
	inline operator LPCSTR() const;
	operator LPCWSTR();

	BOOL SetText(const wchar_t* pText);
	BOOL SetText(const char* pText);

	void Empty();

private:
	void	InitConstructor();

	int			m_iMaxLen;
	int			m_iLen;
	char*		m_pszText;
	wchar_t*	m_pszTextWide;

	static char		gm_szEmpty[];
	static wchar_t	gm_szEmptyWide[];
};

/////////////////////////////////////////////////
// two specialized versions, which simply call the super classes
// constructor with builtin limit
//
class CLimitedString32 : public CLimitedString
{
public:
	// simplified constructor
	CLimitedString32() : CLimitedString(32,"") { };
};

class CLimitedString64 : public CLimitedString
{
public:
	// simplified constructor
	CLimitedString64() : CLimitedString(64,"") { };
};
//////////////////////////////////////////////////
//// inlined functions                       /////
//////////////////////////////////////////////////
inline BOOL CLimitedString::IsEmpty() const
{
	return m_pszText == gm_szEmpty ? TRUE : FALSE;
}
//////////////////////////////////////////////////
inline int	CLimitedString::GetLength() const
{
	return m_iLen;
}
//////////////////////////////////////////////////
inline CLimitedString::operator LPCSTR() const
{
	return m_pszText;
}
//////////////////////////////////////////////////
#endif
