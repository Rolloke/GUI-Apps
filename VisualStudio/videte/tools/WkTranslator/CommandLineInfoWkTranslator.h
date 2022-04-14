// CommandLineInfoWkTranslator.h: interface for the CCommandLineInfoWkTranslator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDLINEINFOWKTRANSLATOR_H__A56CCBB6_32CC_11D4_BAD9_00400531137E__INCLUDED_)
#define AFX_COMMANDLINEINFOWKTRANSLATOR_H__A56CCBB6_32CC_11D4_BAD9_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCommandLineInfoWkTranslator : public CCommandLineInfo  
{
public:
	CCommandLineInfoWkTranslator();
	virtual ~CCommandLineInfoWkTranslator();

	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
};

#endif // !defined(AFX_COMMANDLINEINFOWKTRANSLATOR_H__A56CCBB6_32CC_11D4_BAD9_00400531137E__INCLUDED_)
