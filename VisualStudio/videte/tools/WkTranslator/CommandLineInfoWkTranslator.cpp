// CommandLineInfoWkTranslator.cpp: implementation of the CCommandLineInfoWkTranslator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandLineInfoWkTranslator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCommandLineInfoWkTranslator::CCommandLineInfoWkTranslator()
{
}
//////////////////////////////////////////////////////////////////////
CCommandLineInfoWkTranslator::~CCommandLineInfoWkTranslator()
{
}
//////////////////////////////////////////////////////////////////////
void CCommandLineInfoWkTranslator::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	CString sParam = lpszParam;
	if (!bFlag)
	{
		if (sParam.GetLength() == 3)
		{
			CStringArray result;
			CString sPattern = _T("*") + sParam +_T(".lng");
			Translator_SearchFiles(result, _T(""), sPattern, FALSE);
			if (result.GetSize() > 0)
			{
				sParam = result.GetAt(0);
			}
			else
			{
				sParam.Empty();
			}
		}
	}
	if (sParam.GetLength())
	{
		CCommandLineInfo::ParseParam(sParam, bFlag, bLast);
	}
}
