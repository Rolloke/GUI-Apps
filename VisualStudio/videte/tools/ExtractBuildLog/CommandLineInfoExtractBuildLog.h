// CommandLineInfoExtractBuildLog.h: interface for the CCommandLineInfoExtractBuildLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDLINEINFOEXTRACTBUILDLOG_H__CE2DA231_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
#define AFX_COMMANDLINEINFOEXTRACTBUILDLOG_H__CE2DA231_7526_11D5_BB45_0050BF49CEBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommandLineInfoExtractBuildLog : public CCommandLineInfo  
{
public:
	CCommandLineInfoExtractBuildLog();
	virtual ~CCommandLineInfoExtractBuildLog();

	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
};

#endif // !defined(AFX_COMMANDLINEINFOEXTRACTBUILDLOG_H__CE2DA231_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
