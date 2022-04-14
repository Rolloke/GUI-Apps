////////////////////////////////////////////////////////////////////////////
#include "LimitedString.h"
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////
char	CLimitedString::gm_szEmpty[]		= "";
wchar_t	CLimitedString::gm_szEmptyWide[]	= L"";

////////////////////////////////////////////////////////////////////////////
CLimitedString::CLimitedString()
{
	InitConstructor();
}
////////////////////////////////////////////////////////////////////////////
CLimitedString::CLimitedString(const wchar_t *pText)
{
	InitConstructor();
	SetText(pText);
}
////////////////////////////////////////////////////////////////////////////
CLimitedString::CLimitedString(const char *pText)
{
	InitConstructor();
	SetText(pText);
}
////////////////////////////////////////////////////////////////////////////
CLimitedString::CLimitedString(int iMaxLen, const wchar_t *pText)
{
	InitConstructor();
	m_iMaxLen = iMaxLen;
	SetText(pText);
}
////////////////////////////////////////////////////////////////////////////
CLimitedString::CLimitedString(int iMaxLen, const char *pText)
{
	InitConstructor();
	m_iMaxLen = iMaxLen;
	SetText(pText);
}
////////////////////////////////////////////////////////////////////////////
void CLimitedString::InitConstructor()
{
	m_iLen        = 0;
	m_iMaxLen     = S_UNLIMITED;
	m_pszText     = gm_szEmpty;
	m_pszTextWide = gm_szEmptyWide;
}
////////////////////////////////////////////////////////////////////////////
CLimitedString::~CLimitedString()
{
	Empty();
}
//////////////////////////////////////////////////
CLimitedString::operator LPCWSTR()
{
	if (m_pszTextWide == gm_szEmptyWide)
	{
		int nLen = m_iLen;
		m_pszTextWide = (wchar_t*)malloc((m_iLen+1)*sizeof(wchar_t));

		nLen = MultiByteToWideChar(CP_ACP, 0, (char*)m_pszText, nLen, m_pszTextWide, nLen);
		m_pszTextWide[nLen] = 0;
	}
	return m_pszTextWide;
}
////////////////////////////////////////////////////////////////////////////
BOOL CLimitedString::SetText(const char *pText)	// truncates if > maxLen
{

	BOOL   bLimited = FALSE;
	int    iLen     = strlen(pText);
	Empty();

	if (m_iMaxLen != S_UNLIMITED)
	{
		if (iLen > m_iMaxLen)
		{
			bLimited = TRUE;
			iLen = m_iMaxLen;
		}
	}

	m_pszText = (char*)malloc(iLen+1);
	strncpy(m_pszText, pText, iLen);
	m_pszText[iLen] = '\0';
	m_iLen    = iLen;

	return !bLimited;
}
////////////////////////////////////////////////////////////////////////////
BOOL CLimitedString::SetText(const wchar_t *pText)	// truncates if > maxLen
{

	BOOL   bLimited = FALSE;
	int    iLen     = wcslen(pText);
	Empty();

	if (m_iMaxLen != S_UNLIMITED)
	{
		if (iLen > m_iMaxLen)
		{
			bLimited = TRUE;
			iLen = m_iMaxLen;
		}
	}

	m_pszText = (char*)malloc(iLen*2+1);
	iLen = WideCharToMultiByte(CP_ACP, NULL, pText, iLen, m_pszText, iLen, NULL, NULL);
	m_pszText[iLen] = '\0';
	m_iLen    = iLen;

	return !bLimited;
}
////////////////////////////////////////////////////////////////////////////
void CLimitedString::Empty()
{
	if (m_pszText != gm_szEmpty)
	{
		free(m_pszText);
	}
	if (m_pszTextWide != gm_szEmptyWide)
	{
		free(m_pszTextWide);
	}
	m_pszText		= gm_szEmpty;
	m_pszTextWide	= gm_szEmptyWide;
	m_iLen = 0;
}
