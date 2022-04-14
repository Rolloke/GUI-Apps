// LangDllInfo.cpp: implementation of the CLangDllInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "LangDllInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLangDllInfo::CLangDllInfo(const CString& sDll,
						   const CString& sLanguage,
						   const CString& sAbbreviation,
						   const CString& sCodePage,
							DWORD dwBits)
{
	m_sDll           = sDll;
	m_sLanguage      = sLanguage;
	m_sAbbreviation  = sAbbreviation;
	m_sCodePage      = sCodePage;
	m_dwCodePageBits = dwBits;
}
//////////////////////////////////////////////////////////////////////
CLangDllInfo::~CLangDllInfo()
{
	m_dwCodePageBits = 0;
}
