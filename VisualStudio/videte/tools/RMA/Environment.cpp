// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RMA.h"
#include "Environment.h"

#define MY_ENV(x) m_s##x = getenv(#x);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnvironment::CEnvironment()
{
	MY_ENV(AUTH_TYPE);
	MY_ENV(ComSpec);
	MY_ENV(GATEWAY_INTERFACE);
	MY_ENV(CONTENT_LENGTH);
	MY_ENV(CONTENT_TYPE);
	MY_ENV(PATH);
	MY_ENV(PATH_INFO);
	MY_ENV(PATH_TRANSLATED);
	MY_ENV(QUERY_STRING);
	MY_ENV(REMOTE_ADDR);
	MY_ENV(REMOTE_HOST);
	MY_ENV(REMOTE_USER);
	MY_ENV(REQUEST_METHOD);
	MY_ENV(SCRIPT_NAME);
	MY_ENV(SERVER_NAME);
	MY_ENV(SERVER_PROTOCOL);
	MY_ENV(SERVER_PORT);
	MY_ENV(SERVER_SOFTWARE);
	MY_ENV(SystemRoot);
	MY_ENV(windir);

#ifdef _DEBUG
	m_sQUERY_STRING = "Command=NewEntrie&Company=&City=&PostalCode=&Street=&Country=&Fon=&Fax=&Name=&EMail=&Serial=&Type=VDR+402-4D&Date=&BillNr=&Error=";
#endif
	if (!m_sQUERY_STRING.IsEmpty())
	{
		SplitString(m_sQUERY_STRING,m_StringArray,"&");
		StringArray2Map(m_StringArray,m_mapQuery,'=');
	}
}
//////////////////////////////////////////////////////////////////////
CEnvironment::~CEnvironment()
{

}
//////////////////////////////////////////////////////////////////////
#define TABLE_ROW(x) printf("<tr><td>%s</td><td>%s</td><tr>\n",#x,m_s##x);
void CEnvironment::ToTable()
{
	OpenTAG("h1");
	printf("Environment");
	CloseTAG("h1");
	OpenTAG("table border=\"1\"");
	TABLE_ROW(AUTH_TYPE);
	TABLE_ROW(ComSpec);
	TABLE_ROW(GATEWAY_INTERFACE);
	TABLE_ROW(CONTENT_LENGTH);
	TABLE_ROW(CONTENT_TYPE);
	TABLE_ROW(PATH);
	TABLE_ROW(PATH_INFO);
	TABLE_ROW(PATH_TRANSLATED);
	TABLE_ROW(QUERY_STRING);
	TABLE_ROW(REMOTE_ADDR);
	TABLE_ROW(REMOTE_HOST);
	TABLE_ROW(REMOTE_USER);
	TABLE_ROW(REQUEST_METHOD);
	TABLE_ROW(SCRIPT_NAME);
	TABLE_ROW(SERVER_NAME);
	TABLE_ROW(SERVER_PROTOCOL);
	TABLE_ROW(SERVER_PORT);
	TABLE_ROW(SERVER_SOFTWARE);
	TABLE_ROW(SystemRoot);
	TABLE_ROW(windir);
	CloseTAG("table");

	OpenTAG("h1");
	printf("Query Array");
	CloseTAG("h1");
	OpenTAG("table border=\"1\"");
	for (int i=0;i<m_StringArray.GetSize();i++)
	{
		printf("<tr><td>%i</td><td>%s</td><tr>\n",i,m_StringArray.GetAt(i));
	}
	CloseTAG("table");

	OpenTAG("h1");
	printf("Query MAP %i Entries",m_mapQuery.GetCount());
	CloseTAG("h1");
	OpenTAG("table border=\"1\"");
	CString key,value;
	POSITION pos = m_mapQuery.GetStartPosition();
	while (pos)
	{
		printf("<tr><td>%s</td><td>%s</td><tr>\n",key,value);
		m_mapQuery.GetNextAssoc(pos,key,value);
	}
	CloseTAG("table");
}
//////////////////////////////////////////////////////////////////////
CString CEnvironment::GetValue(const CString& key)
{
	CString value;
	CString k(key);
	k.MakeLower();
	m_mapQuery.Lookup(k,value);
	return value;
}
