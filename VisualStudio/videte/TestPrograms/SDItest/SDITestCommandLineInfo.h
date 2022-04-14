// SDITestCommandLineInfo.h: interface for the CSDITestCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDITESTCOMMANDLINEINFO_H__29372916_71C3_11D3_BA81_00400531137E__INCLUDED_)
#define AFX_SDITESTCOMMANDLINEINFO_H__29372916_71C3_11D3_BA81_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSDITestCommandLineInfo : public CCommandLineInfo  
{
public:
	CSDITestCommandLineInfo();
	virtual ~CSDITestCommandLineInfo();

	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

public:
	BOOL	m_bAutoStart;
};

#endif // !defined(AFX_SDITESTCOMMANDLINEINFO_H__29372916_71C3_11D3_BA81_00400531137E__INCLUDED_)
