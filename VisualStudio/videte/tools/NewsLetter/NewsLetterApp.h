// NewsLetterApp.h: interface for the CNewsLetterApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NewsLetterAPP_H__348AC807_D017_11D4_9983_004005A19028__INCLUDED_)
#define AFX_NewsLetterAPP_H__348AC807_D017_11D4_9983_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CNewsLetterApp : public CWinApp
{
public:
	CNewsLetterApp();
	virtual ~CNewsLetterApp();

public:
	BOOL InitInstance();
	int  Run();
	int  ExitInstance();

protected:
	void About();
	void Create();
	void NewEntry();
	void NewDepartment();
	void NewEmployee();
	void ShowDepartments();
	void ShowEmployees();
	void DisplayAll();
	void DisplayToday();
	void DisplayDate();
	void DisplayCalendar();
	void DisplayList();
	void WriteHead();
	void WriteHTTPHeader();
	void SendMail();

private:
	CEnvironment m_Environment;
};

void PrintTableCell(Field4& f);

#endif // !defined(AFX_NewsLetterAPP_H__348AC807_D017_11D4_9983_004005A19028__INCLUDED_)
