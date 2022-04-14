// NewsLetterDatabase.h: interface for the CNewsLetterDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NewsLetterDATABASE_H__D52C8E83_D0C4_11D4_9983_004005A19028__INCLUDED_)
#define AFX_NewsLetterDATABASE_H__D52C8E83_D0C4_11D4_9983_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNewsLetterDatabase  
{
	// construction / destruction
public:
	CNewsLetterDatabase();
	virtual ~CNewsLetterDatabase();

	// attributes


	// operations
public:
	BOOL Create(const CString& sPath, const CString& sPrefix);
	BOOL Open(const CString& sPath, const CString& sPrefix);
	BOOL Append(const CString& sSubject, const CString& sNews, const CString& sEmployee);
	BOOL Close();

	BOOL DisplayAll();
	BOOL DisplayToday();
	BOOL DisplayDate(const CString& sDBDate);
	BOOL DisplayDepartments();
	BOOL DisplayEmployees();
	BOOL AddDepartment(const CString& sDepartment, CString& sID);
	BOOL AddEmployee(const CString& sName, const CString& sD_ID, CString& sNr);

	BOOL GetDepartments(CStringArray& saDepartments);
	BOOL GetEmployees(CStringArray& saEmployees);
	BOOL GetDepartment(const CString& sEmployee, CString& sD_ID);
	BOOL GetEmployee(const CString& sEmployee, CString& sNr);
	BOOL DisplayList(const CString& sQuery);

protected:
	BOOL DisplayTable(const CString& sQuery);
	BOOL GetDepartmentIDs(CStringArray& saIDs);

	// data member
private:
	Code4		m_c4NewsLetter;
	
	Data4		m_d4News;
	Data4		m_d4Employees;
	Data4		m_d4Departments;
	
	Relate4set  m_r4Set;
	Relate4		m_r4News2Employees;
	Relate4		m_r4Employees2Departments;
};

#endif // !defined(AFX_NewsLetterDATABASE_H__D52C8E83_D0C4_11D4_9983_004005A19028__INCLUDED_)
