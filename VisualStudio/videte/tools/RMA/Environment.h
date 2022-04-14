// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__348AC80D_D017_11D4_9983_004005A19028__INCLUDED_)
#define AFX_ENVIRONMENT_H__348AC80D_D017_11D4_9983_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEnvironment  
{
public:
	CEnvironment();
	virtual ~CEnvironment();

public:
	void ToTable();
	CString GetValue(const CString& key);

public:
	CString m_sAUTH_TYPE;
	CString m_sComSpec;
	CString m_sGATEWAY_INTERFACE;
	CString m_sCONTENT_LENGTH;
	CString m_sCONTENT_TYPE;
	CString m_sPATH;
	CString m_sPATH_INFO;
	CString m_sPATH_TRANSLATED;
	CString m_sQUERY_STRING;
	CString m_sREMOTE_ADDR;
	CString m_sREMOTE_HOST;
	CString m_sREMOTE_USER;
	CString m_sREQUEST_METHOD;
	CString m_sSCRIPT_NAME;
	CString m_sSERVER_NAME;
	CString m_sSERVER_PROTOCOL;
	CString m_sSERVER_PORT;
	CString m_sSERVER_SOFTWARE;
	CString m_sSystemRoot;
	CString m_swindir;

	CStringArray m_StringArray;
	CMapStringToString m_mapQuery;
};

#endif // !defined(AFX_ENVIRONMENT_H__348AC80D_D017_11D4_9983_004005A19028__INCLUDED_)
