// RMAApp.h: interface for the CRMAApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RMAAPP_H__348AC807_D017_11D4_9983_004005A19028__INCLUDED_)
#define AFX_RMAAPP_H__348AC807_D017_11D4_9983_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRMAApp : public CWinApp
{
public:
	CRMAApp();
	virtual ~CRMAApp();

public:
	BOOL InitInstance();
	int  Run();
	int  ExitInstance();

protected:
	void NewEntry();
	void About();
	void DisplayAll();
	void WriteHTTPHeader();

private:
	CEnvironment m_Environment;
};

#endif // !defined(AFX_RMAAPP_H__348AC807_D017_11D4_9983_004005A19028__INCLUDED_)
