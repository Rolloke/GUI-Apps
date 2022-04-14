// CMyDebug.h: interface for the CMyDebug class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMYDEBUG_H__EFDBFD94_06C2_11D3_8D23_004005A11E32__INCLUDED_)
#define AFX_CMYDEBUG_H__EFDBFD94_06C2_11D3_8D23_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyDebug : public CDebug  
{
public:
	CMyDebug();
	virtual ~CMyDebug();

	virtual void OnReceivedMessage(const CString& sError) const;
	virtual void OnReceivedWarning(const CString& sError) const;
	virtual void OnReceivedError(const CString& sError) const;
	virtual void OnReceivedStatLog(LPCTSTR szTopic, int iValue, LPCTSTR sName) const;
	virtual void OnReceivedStatPeak(LPCTSTR szTopic, int iValue, LPCTSTR sName) const;

};

#endif // !defined(AFX_CMYDEBUG_H__EFDBFD94_06C2_11D3_8D23_004005A11E32__INCLUDED_)
