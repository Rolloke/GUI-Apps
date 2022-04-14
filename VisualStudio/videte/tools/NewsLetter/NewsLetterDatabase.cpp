// NewsLetterDatabase.cpp: implementation of the CNewsLetterDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewsLetterDatabase.h"
#include "NewsLetterApp.h"

static char szNews[] = "NEWS";
static char szEmployees[] = "EMPLOYEES";
static char szDepartments[] = "DEPARTMENTS";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNewsLetterDatabase::CNewsLetterDatabase()
{
	m_c4NewsLetter.safety = FALSE;
	m_c4NewsLetter.errOff = TRUE;
	m_c4NewsLetter.accessMode = OPEN4DENY_RW;
	m_c4NewsLetter.optimize = OPT4OFF;
	m_c4NewsLetter.optimizeWrite = OPT4OFF;
	m_c4NewsLetter.autoOpen = TRUE;
}
CNewsLetterDatabase::~CNewsLetterDatabase()
{
	Close();
	m_c4NewsLetter.initUndo();
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::Create(const CString& sPath, const CString& sPrefix)
{
	Field4info	fiNews(m_c4NewsLetter);
	Field4info	fiEmployees(m_c4NewsLetter);
	Field4info	fiDepartments(m_c4NewsLetter);
	Tag4info	tiEmployees(m_c4NewsLetter);
	Tag4info	tiDepartments(m_c4NewsLetter);

	// fields
	fiNews.add("DATETIME",'C',14,0);
	fiNews.add("SUBJECT",'C',32,0);
	fiNews.add("TEXT",'C',254,0);
	fiNews.add("E_NR",'C',4,0);

	fiEmployees.add("NR",'C',4,0);
	fiEmployees.add("LASTNAME",'C',20,0);
	fiEmployees.add("FIRSTNAME",'C',20,0);
	fiEmployees.add("D_ID",'C',4,0);

	// tags
	fiDepartments.add("ID",'C',4,0);
	fiDepartments.add("DNAME",'C',20,0);

	tiEmployees.add("NR_TAG","NR",NULL,r4unique);
	tiDepartments.add("ID_TAG","ID",NULL,r4unique);

	if (!DoesFileExist(sPath))
	{
		CreateDirectory(sPath,NULL);
	}

	if (!DoesFileExist(sPath+'\\'+sPrefix))
	{
		CreateDirectory(sPath+'\\'+sPrefix,NULL);
	}

	if (DoesFileExist(sPath+'\\'+sPrefix))
	{
		// create the news.dbf
		CString sNews = sPath + '\\' + sPrefix + '\\' + szNews;
		int r = m_d4News.open(m_c4NewsLetter,sNews);
		if (e4fileFind == r)
		{
			m_c4NewsLetter.errorSet(0);
			r = m_d4News.create(m_c4NewsLetter,sNews,fiNews);
		}

		if (r == r4success)
		{
			// create employees.dbf
			CString sEmployees = sPath + '\\' + sPrefix + '\\' + szEmployees;
			r = m_d4Employees.open(m_c4NewsLetter,sEmployees);
			if (e4fileFind == r)
			{
				m_c4NewsLetter.errorSet();
				r = m_d4Employees.create(m_c4NewsLetter,sEmployees,fiEmployees,tiEmployees);
			}
			else if (r4success == r)
			{
				m_d4Employees.close();
			}

			if (r == r4success)
			{
				// create Departments.dbf
				CString sDepartments = sPath + '\\' + sPrefix + '\\' + szDepartments;
				r = m_d4Departments.open(m_c4NewsLetter,sDepartments);
				if (e4fileFind == r)
				{
					m_c4NewsLetter.errorSet();
					r = m_d4Departments.create(m_c4NewsLetter,sDepartments,fiDepartments,tiDepartments);
				}
				else if (r4success == r)
				{
					m_d4Departments.close();
				}
			}
		}
		else if (r4success == r)
		{
			m_d4News.close();
		}

		return r == r4success;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::Open(const CString& sPath, const CString& sPrefix)
{
	CString sEmployees = sPath + '\\' + sPrefix + '\\' + szEmployees;
	CString sDepartments = sPath + '\\' + sPrefix + '\\' + szDepartments;
	CString sNews = sPath + '\\' + sPrefix + '\\' + szNews;

	int r;

	r = m_d4News.open(m_c4NewsLetter,sNews);
	if (r == r4success)
	{
		r = m_d4Employees.open(m_c4NewsLetter,sEmployees);
		if (r == r4success)
		{
			r = m_d4Departments.open(m_c4NewsLetter,sDepartments);
			if (r == r4success)
			{
				if (r4success == m_r4Set.init(m_d4News))
				{
					Tag4 nrTag(m_d4Employees,"NR_TAG");
					r = m_r4News2Employees.init(m_r4Set,m_d4Employees,"E_NR",nrTag);
					if (r4success == r)
					{
						Tag4 idTag(m_d4Departments,"ID_TAG");
						r = m_r4Employees2Departments.init(m_r4News2Employees,m_d4Departments,"D_ID",idTag);
						if (r == r4success)
						{
							return TRUE;
						}
						else
						{
							TRACE("cannot init slave relation 2 %d\n",r);
						}
					}
					else
					{
						TRACE("cannot init slave relation 1 %d\n",r);
					}
				}
				else
				{
					TRACE("cannot init master relation\n");
				}
			}
			else
			{
				TRACE("cannot open %s\n",sDepartments);
			}
		}
		else
		{
			TRACE("cannot open %s\n",sEmployees);
		}
	}
	else
	{
		TRACE("cannot open %s\n",sNews);
	}

	return FALSE;

}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::GetEmployee(const CString& sEmployee, CString& sNr)
{
	BOOL bRet = FALSE;
	CString sE_Nr;
	// Employee OK ?
	Relate4set relation;
	CString sName(sEmployee);
	CString sQuery,sFirstName,sLastName;
	int p;

	p = sName.Find(' ');
	if (p!=-1)
	{
		sFirstName = sName.Left(p);
		sLastName  = sName.Mid(p+1);
	}

	sQuery.Format("('%s' $ LASTNAME) .AND. ('%s' $ FIRSTNAME)",sLastName,sFirstName);

	int rc = relation.init(m_d4Employees);
	if (rc == r4success)
	{
		rc = relation.querySet(sQuery);
		if (rc==r4success)
		{
			rc = relation.top();
			if (rc==r4success)
			{
				Field4 f(m_d4Employees,"NR");
				if (f.isValid())
				{
					sNr = f.str();
					bRet = TRUE;
				}
			}
			relation.free();
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::GetDepartment(const CString& sEmployee, CString& sD_ID)
{
	BOOL bRet = FALSE;
	CString sE_Nr;
	// Employee OK ?
	Relate4set relation;
	CString sName(sEmployee);
	CString sQuery,sFirstName,sLastName;
	int p;

	p = sName.Find(' ');
	if (p!=-1)
	{
		sFirstName = sName.Left(p);
		sLastName  = sName.Mid(p+1);
	}

	sQuery.Format("('%s' $ LASTNAME) .AND. ('%s' $ FIRSTNAME)",sLastName,sFirstName);

	int rc = relation.init(m_d4Employees);
	if (rc == r4success)
	{
		rc = relation.querySet(sQuery);
		if (rc==r4success)
		{
			rc = relation.top();
			if (rc==r4success)
			{
				Field4 f(m_d4Employees,"D_ID");
				if (f.isValid())
				{
					sD_ID = f.str();
					bRet = TRUE;
				}
			}
			relation.free();
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::AddDepartment(const CString& sDepartment, CString& sID)
{
	CString sD_ID;
	// department OK ?
	Relate4set relation;
	CString sQuery;

	sQuery.Format("'%s' $ DNAME",sDepartment);

	int rc = relation.init(m_d4Departments);
	if (rc == r4success)
	{
		rc = relation.querySet(sQuery);
		if (rc==r4success)
		{
			rc = relation.top();
			relation.free();
			if (rc==r4success)
			{
				Field4 did(m_d4Departments,"ID");
				sD_ID = did.str();
			}
			else
			{
				TRACE("Department not found %s\n",sDepartment);
				Tag4 idTag(m_d4Departments,"ID_TAG");
				idTag.unique(r4unique);
				
				m_d4Departments.appendStart();
				Field4 dname(m_d4Departments,"DNAME");
				Field4 did(m_d4Departments,"ID");
				dname.assign(sDepartment);
				DWORD id = 0;
				CString s;
				
				do
				{
					id++;
					s.Format("%04d",id);
					did.assign(s);
					rc = m_d4Departments.append();
				}
				while (rc == r4unique);
				sD_ID = s;
				m_c4NewsLetter.errorSet();
				m_d4Departments.flush();
			}
		}
		sID = sD_ID;
		return TRUE;
	}
	else
	{
			m_c4NewsLetter.errorSet();
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::AddEmployee(const CString& sName, const CString& sD_ID, CString& sNr)
{
	CString sE_Nr;
	// Employee OK ?
	Relate4set relation;
	CString sQuery,sFirstName,sLastName;
	int p;

	p = sName.Find(' ');
	if (p!=-1)
	{
		sFirstName = sName.Left(p);
		sLastName  = sName.Mid(p+1);
	}

	sQuery.Format("('%s' $ LASTNAME) .AND. ('%s' $ FIRSTNAME)",sLastName,sFirstName);

	int rc = relation.init(m_d4Employees);
	if (rc == r4success)
	{
		rc = relation.querySet(sQuery);
		if (rc==r4success)
		{
			rc = relation.top();
			relation.free();
			if (rc==r4success)
			{
				Field4 e_nr(m_d4Employees,"NR");
				sE_Nr = e_nr.str();
			}
			else
			{
				TRACE("Employee not found %s\n",sName);
				Tag4 nrTag(m_d4Employees,"NR_TAG");
				nrTag.unique(r4unique);
				
				m_d4Employees.appendStart();
				Field4 firstname(m_d4Employees,"FIRSTNAME");
				Field4 lastname(m_d4Employees,"LASTNAME");
				Field4 d_id(m_d4Employees,"D_ID");
				Field4 e_nr(m_d4Employees,"NR");
				firstname.assign(sFirstName);
				lastname.assign(sLastName);
				d_id.assign(sD_ID);
				DWORD id = 0;
				CString s;
				
				do
				{
					id++;
					s.Format("%04d",id);
					e_nr.assign(s);
					rc = m_d4Employees.append();
				}
				while (rc == r4unique);
				sE_Nr = s;
				m_d4Employees.flush();
				m_c4NewsLetter.errorSet();
			}
		}
		sNr = sE_Nr;
		return TRUE;
	}
	else
	{
		m_c4NewsLetter.errorSet();
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::Append(const CString& sSubject,const CString& sNews,
								 const CString& sEmployee)
{
	CString sNr;

	// department OK ?
	if (GetEmployee(sEmployee,sNr))
	{
		TRACE("employee nr is %s\n",sNr);
		// now add the news
		CSystemTime t;
		t.GetLocalTime();
		
		int rc;
		
		rc = m_d4News.appendStart();
		
		if (rc == r4success)
		{
			Field4 e_nr(m_d4News,"E_NR");
			if (e_nr.isValid())
				rc = e_nr.assign(sNr);
			else
				m_c4NewsLetter.errorSet();
		}
		if (rc == r4success)
		{
			Field4 datetime(m_d4News,"DATETIME");
			if (datetime.isValid())
				rc = datetime.assign(t.GetDBDate()+t.GetDBTime());
			else
				m_c4NewsLetter.errorSet();
		}				
		if (rc == r4success)
		{
			Field4 subject(m_d4News,"SUBJECT");
			if (subject.isValid())
				rc = subject.assign(sSubject);
			else
				m_c4NewsLetter.errorSet();
		}				
		if (rc == r4success)
		{
			Field4 news(m_d4News,"TEXT");
			if (news.isValid())
				rc = news.assign(sNews);
			else
				m_c4NewsLetter.errorSet();
		}
		if (rc == r4success)
		{
			rc = m_d4News.append();
			if (rc == r4success)
			{
				m_d4News.flush();
			}
			else
			{
				m_c4NewsLetter.errorSet();
				TRACE("News append failed %d\n",rc);
			}
		}

		return rc == r4success;
	}


	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::Close()
{
	if (m_r4Set.isValid())
	{
		m_r4Set.free();
	}

	if (m_d4Departments.isValid())
	{
		m_d4Departments.close();
	}
	if (m_d4Employees.isValid())
	{
		m_d4Employees.close();
	}
	if (m_d4News.isValid())
	{
		m_d4News.close();
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::DisplayDate(const CString& sDBDate)
{
	CString s;
	CSystemTime t;
	t.SetDBDate(sDBDate);
	{
		CTAG h1("h1");
		printf("Newsletter vom %s\n",t.GetDate());
	}
	s.Format("(DATETIME>='%s000000') .AND. (DATETIME<='%s235959')",sDBDate,sDBDate);
	return DisplayTable(s);
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::DisplayToday()
{
	CString s,d;
	CSystemTime t;
	t.GetLocalTime();
	d = t.GetDBDate();
	return DisplayDate(d);
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::DisplayAll()
{
	return DisplayTable("");
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::DisplayTable(const CString& sQuery)
{
 	if (m_r4Set.isValid())
 	{
		int rc = r4success;
		if (!sQuery.IsEmpty())
		{
			rc = m_r4Set.querySet(sQuery);
		}

		if (rc == r4success)
		{
			CTAG table("table border=\"1\" cellspacing=\"0\"");
			
			{
				// Titelzeile
				CTAG tr("tr");
				{
					CTAG td("td");
					printf("Datum und Uhrzeit");
				}
				{
					CTAG td("td");
					printf("Subjekt");
				}
				{
					CTAG td("td");
					printf("Text");
				}
				{
					CTAG td("td");
					printf("Mitarbeiter");
				}
				{
					CTAG td("td");
					printf("Abteilung");
				}
			}
			
			
			Field4 datetime(m_d4News,"DATETIME");
			Field4 subject(m_d4News,"SUBJECT");
			Field4 news(m_d4News,"TEXT");
			Field4 fname(m_d4Employees,"FIRSTNAME");
			Field4 lname(m_d4Employees,"LASTNAME");
			Field4 dname(m_d4Departments,"DNAME");
			
			for (int rc=m_r4Set.top();rc!=r4eof;rc = m_r4Set.skip())
			{
				CTAG tr("tr");
				
				// Datum und Zeit
				{
					CTAG td("td");
					CSystemTime t;
					CString s(datetime.str());
					t.SetDBDate(s.Left(8));
					t.SetDBTime(s.Right(6));
					printf(t.GetDateTime());
				}
				
				// Subject
				PrintTableCell(subject);
				
				// Text
				PrintTableCell(news);
				
				// Mitarbeiter
				{
					CTAG td("td");
					CString name;
					name = fname.str();
					name.TrimRight();
					name += ' ';
					name += lname.str();
					name.TrimRight();
					printf(name);
				}
				
				// Abteilung
				PrintTableCell(dname);
			}
		}
 	}
 	else
 	{
 		printf("Es sind keine Datensätze vorhanden.\n");
 	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::DisplayList(const CString& sQuery)
{
 	if (m_r4Set.isValid())
 	{
		int rc = r4success;
		if (!sQuery.IsEmpty())
		{
			rc = m_r4Set.querySet(sQuery);
		}

		if (rc == r4success)
		{
//			rc = m_r4Set.sortSet("DEPARTMENTS->DNAME");
			
			if (rc == r4success)
			{
				Field4 datetime(m_d4News,"DATETIME");
				Field4 subject(m_d4News,"SUBJECT");
				Field4 news(m_d4News,"TEXT");
				Field4 fname(m_d4Employees,"FIRSTNAME");
				Field4 lname(m_d4Employees,"LASTNAME");
				Field4 dname(m_d4Departments,"DNAME");

				OpenTAG("ul");
				CString sDNAME;
				BOOL bULOpen = FALSE;
				for (int rc=m_r4Set.top();rc!=r4eof;rc = m_r4Set.skip())
				{
					if (sDNAME != dname.str())
					{
						printf("<li>%s</li>\n",dname.str());
						if (bULOpen)
						{
							CloseTAG("ul");
							bULOpen = FALSE;
						}
						OpenTAG("ul");
						sDNAME = dname.str();
						bULOpen = TRUE;
					}
					printf("<li>%s: %s</li>\n",subject.str(),news.str());
				}
				if (bULOpen)
				{
					CloseTAG("ul");
					bULOpen = FALSE;
				}
				CloseTAG("ul");
			}
		}
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::GetDepartmentIDs(CStringArray& saIDs)
{
	if (m_d4Departments.isValid())
	{
		for (m_d4Departments.top(); !m_d4Departments.eof(); m_d4Departments.skip())
		{
			Field4 f(m_d4Departments,"ID");
			saIDs.Add(f.str());
		}
		return TRUE;
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::GetDepartments(CStringArray& saDepartments)
{
	if (m_d4Departments.isValid())
	{
		for (m_d4Departments.top(); !m_d4Departments.eof(); m_d4Departments.skip())
		{
			Field4 f(m_d4Departments,"DNAME");
			saDepartments.Add(f.str());
		}
		return TRUE;
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::DisplayDepartments()
{
	CTAG table("table border=\"1\" cellspacing=\"0\"");

	{
		CTAG tr("tr");
		{
			CTAG td("td");
			printf("Nr");
		}
		{
			CTAG td("td");
			printf("Abteilung");
		}
	}
	if (m_d4Departments.isValid())
	{
		Field4 name(m_d4Departments,"DNAME");
		Field4 nr(m_d4Departments,"ID");
		for (m_d4Departments.top(); !m_d4Departments.eof(); m_d4Departments.skip())
		{
			CTAG tr("tr");
			PrintTableCell(nr);
			PrintTableCell(name);
		}
		return TRUE;
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::GetEmployees(CStringArray& saEmployees)
{
	if (m_d4Employees.isValid())
	{
		Field4 fname(m_d4Employees,"FIRSTNAME");
		Field4 lname(m_d4Employees,"LASTNAME");
		for (m_d4Employees.top(); !m_d4Employees.eof(); m_d4Employees.skip())
		{
			CString name;
			name = fname.str();
			name.TrimRight();
			name += ' ';
			name += lname.str();
			name.TrimRight();
			saEmployees.Add(name);
		}
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////
BOOL CNewsLetterDatabase::DisplayEmployees()
{
	CTAG table("table border=\"1\" cellspacing=\"0\"");

	{
		CTAG tr("tr");
		{
			CTAG td("td");
			printf("Nr");
		}
		{
			CTAG td("td");
			printf("Name");
		}
	}
	if (m_d4Employees.isValid())
	{
		Field4 fname(m_d4Employees,"FIRSTNAME");
		Field4 lname(m_d4Employees,"LASTNAME");
		Field4 nr(m_d4Employees,"NR");
		
		for (m_d4Employees.top(); !m_d4Employees.eof(); m_d4Employees.skip())
		{
			CTAG tr("tr");
			PrintTableCell(nr);
			{
				CTAG td("td");
				CString name;
				name = fname.str();
				name.TrimRight();
				name += ' ';
				name += lname.str();
				name.TrimRight();
				printf(name);
			}
		}
		return TRUE;
	}
	else
	{
		printf("EMPLOYEES not valid\n");
	}
	return FALSE;
}
