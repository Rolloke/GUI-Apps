/* GlobalReplace: MyLookUp --> CWK_DNS::LookUp */

// SocketUtil.cpp

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "SocketUtil.h"

BOOL CWK_DNS::m_bUseDNS = FALSE;
BOOL CWK_DNS::m_bDoCacheKnownNames = TRUE;
BOOL CWK_DNS::m_bDoCacheUnknownNames = TRUE;

CWK_LookUpPairs CWK_DNS::m_knownNames;
CWK_LookUpPairs CWK_DNS::m_unknownNames;

void CWK_DNS::AddKnown(const CString &sIn, const CString &sOut)
{
	if (CWK_DNS::m_bDoCacheKnownNames)
	{
		m_knownNames.Lock();
		int ix = m_knownNames.Index(sIn);
		if (ix != -1 ) 
		{
			const CWK_LookUpPair & onePair = * m_knownNames[ix];
			// double check
			if (onePair.GetInput()==sIn) 
			{
				if (onePair.GetLookUpResult()!=sOut)
				{
					WK_TRACE(_T("Lookup mismatch '%s'!='%s'\n"), LPCTSTR(onePair.GetLookUpResult()), LPCTSTR(sOut));
				}
			}	// end of name found
		} 
		else
		{
			CWK_LookUpPair *pNewPair = new CWK_LookUpPair(sIn, sOut);
			m_knownNames.Add(pNewPair);
		}
		m_knownNames.Unlock();
	}
}

void CWK_DNS::AddUnknown(const CString &sIn)
{
	CString sOut(_T("?"));
	sOut += sIn;
	sOut += _T("?");

	if (CWK_DNS::m_bDoCacheUnknownNames) {
		m_unknownNames.Lock();
		int ix = m_unknownNames.Index( sIn );

		if (ix == -1 ) {
			CWK_LookUpPair *pNewPair = new CWK_LookUpPair(sIn,sOut);
			m_unknownNames.Add(pNewPair);
		} else {
			// NOT YET double check on _T("?")
		}
		m_unknownNames.Unlock();
	}

}

void CWK_DNS::Reset()	// also useful for Cleanup
{
	CWK_DNS::m_knownNames.SafeDeleteAll();
	CWK_DNS::m_unknownNames.SafeDeleteAll();
}


CString CWK_DNS::LookUp(const CString &sIn)
{
	if (CWK_DNS::m_bUseDNS==FALSE)
	{
		return sIn;	// <<< EXIT >>>
	}
	CString sOut;

	int ix=-1;
	if (CWK_DNS::m_bDoCacheKnownNames) 
	{
		CWK_DNS::m_knownNames.Lock();
		ix = CWK_DNS::m_knownNames.Index(sIn);
		if (ix != -1 )
		{
			const CWK_LookUpPair & onePair = * CWK_DNS::m_knownNames[ix];
			TRACE(_T("Cached known   %s\n"),sIn);
			sOut = onePair.GetLookUpResult();
		}	
		CWK_DNS::m_knownNames.Unlock();
	}
	if (sOut.IsEmpty() && CWK_DNS::m_bDoCacheUnknownNames)
	{
		CWK_DNS::m_unknownNames.Lock();
		ix = CWK_DNS::m_unknownNames.Index(sIn);
		if (ix != -1 ) 
		{
			const CWK_LookUpPair & onePair = * CWK_DNS::m_unknownNames[ix];
			TRACE(_T("Cached unknown %s\n"),sIn);
			sOut = onePair.GetLookUpResult();
		}
		CWK_DNS::m_unknownNames.Unlock();
	}

	if (sOut.GetLength()) 
	{
		return sOut;
	}

	struct hostent FAR *pHost = NULL;

	DWORD x;
	CWK_String sAdr = sIn;
	x = inet_addr(sAdr);

	if (x!=INADDR_NONE) 
	{	// dotted IP number try lookup
		pHost = gethostbyaddr ((const char FAR *)&x, 4, PF_INET);
		if (pHost==NULL) 
		{
			TRACE(_T("byaddr failed reason %d\n"),WSAGetLastError());
		}
	} 
	else
	{
		// no dotted IP GetHostByAddr
		pHost = gethostbyname(sAdr);
	}
	if (pHost)
	{
		sOut = pHost->h_name;
		CWK_DNS::AddKnown(sIn,sOut);
		const char * pOneAlias = pHost->h_aliases[0];
		int a=1;
		while (pOneAlias)
		{
			TRACE(_T("alias %s\n"),(LPCTSTR)CWK_String(pOneAlias));
			pOneAlias = pHost->h_aliases[a];
			a++;	// index muss doch hochgezaehlt werden! gf
		}
	}
	else
	{
		sOut = _T("?");
		sOut += sIn;
		sOut += _T("?");

		TRACE (_T("Lookup Failed for '%s'\n"), LPCTSTR(sIn));

		CWK_DNS::AddUnknown(sIn);
	}
	TRACE(_T("MyLookup from '%s'-->'%s'\n"), LPCTSTR(sIn), LPCTSTR(sOut));
	return sOut;
}	// end of CWK_DNS::LookUp




extern LPCTSTR NameOfEnum(IPNameType x)
{
	switch (x) 
	{
	case INT_UNKNOWN: return _T("INT_UNKNOWN"); break;
	case INT_DOTTED_IP_NUMBER: return _T("INT_DOTTED_IP_NUMBER"); break;
	case INT_DOTTED_IP_NUMBER_WITH_STAR: return _T("INT_DOTTED_IP_NUMBER_WITH_STAR"); break;
	case INT_SINGLE_HOST_NAME: return _T("INT_SINGLE_HOST_NAME"); break;
	case INT_FULL_HOST_NAME: return _T("INT_FULL_HOST_NAME"); break;
	case INT_FULL_HOST_NAME_WITH_STAR: return _T("INT_FULL_HOST_NAME_WITH_STAR"); break;
	default:
		return (_T("INT_INVALID_NAME_TYPE"));
	}
}


IPNameType CWK_DNS::CalcIPNameType(const CString &sNameIn, CString &sCorrected)
{
	sCorrected = sNameIn;	// NOT YET used
	IPNameType result = INT_UNKNOWN;
	// first check Digits and dots only
	CString sTest;

	int ixStar = sNameIn.Find(_T("*"));
	int ixDot = sNameIn.Find(_T("."));

	if (ixStar == -1) {
		sTest = sNameIn.SpanIncluding(_T("1234567890."));
		if (sTest==sNameIn) {
			// check for three dots NOT YET
			result = INT_DOTTED_IP_NUMBER;
		} else {	// not only digits and dots
			if (ixDot == -1) {	// no dot[s]
				result = INT_SINGLE_HOST_NAME;
			} else {	// incluldes dot[s]
				result = INT_FULL_HOST_NAME;
			}
		}
	} else {	// cases for * included
		sTest = sNameIn.SpanIncluding(_T("1234567890.*"));
		if (sTest==sNameIn) {
			// only digits, dots and stars
			// NOT YET check for trailing star[s]
			result = INT_DOTTED_IP_NUMBER_WITH_STAR;
		} else {	// not only digits, dots and star[s]	
			if (ixDot == -1) {	// no dot[s]
				// OOPS SINGLE STAR ?
			} else {	// incluldes dot[s]
				result = INT_FULL_HOST_NAME_WITH_STAR;
			}
		}
	}

	return result;
}

CString WSAGetLastErrorString()
{
	DWORD dwError = WSAGetLastError();
	CString sResult = GetLastErrorString(dwError);

	return sResult;
}