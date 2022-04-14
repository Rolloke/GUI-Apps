// NewsLetterApp.cpp: implementation of the CNewsLetterApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewsLetterApp.h"
#include "NewsLetterDatabase.h"


static char iso0_31[]  = "         \t\n  \r                  ";
static char iso32_64[] = " !\"#$%&'()*+,-./0123456789:;<=>?@";
static char iso65_127[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefgahijklmnopqrstuvwxyz{|}~ ";
static char iso128_159[] = "                               ";
static char iso160_191[] = " ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿";
static char iso192_221[] = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝ";
static char iso222_255[] = "Þßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";

static char szPath[] = "c:\\data";
void Test()
{
	CString m_sInput = "%20 %21 %28 %29 %40 %41 %5B %7E %DF %FC";
	CString iso,inputcopy;
	iso = iso0_31;
	iso += iso32_64;
	iso += iso65_127;
	iso += iso128_159;
	iso += iso160_191;
	iso += iso192_221;
	iso += iso222_255;

	inputcopy.GetBufferSetLength(m_sInput.GetLength());

	int i,a;
	for (i=0,a=0;i<m_sInput.GetLength();i++)
	{
		if (m_sInput[i] == '%')
		{
			// next 2 chars are Hex ISO
			if (i+2<m_sInput.GetLength())
			{
				CString sHex = m_sInput[++i];
				sHex += m_sInput[++i];
				int iHex=0;
				if (1==sscanf(sHex,"%02x",&iHex))
				{
					inputcopy.SetAt(a++,iso[iHex]);
				}
			}
		}
		else
		{
			inputcopy.SetAt(a++,m_sInput[i]);
		}
	}
	if (a<i)
	{
		inputcopy.SetAt(a++,'\0');
	}
	inputcopy.ReleaseBuffer();
	TRACE("<%s> --> <%s>\n",m_sInput,inputcopy);
}
//////////////////////////////////////////////////////////////////////
void PrintTableCell(Field4& f)
{
	CTAG td("td");
	CString s(f.str());
	s.TrimLeft();
	s.TrimRight();
	if (s.IsEmpty())
	{
		s = "-";
	}
	printf(s);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNewsLetterApp::CNewsLetterApp()
{
}
//////////////////////////////////////////////////////////////////////
CNewsLetterApp::~CNewsLetterApp()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CNewsLetterApp::InitInstance()
{
	InitDebugger("Newsletter.log",WAI_INVALID);
	if (!AfxSocketInit())
	{
		return FALSE;
	}
	CString sCommandLine(m_lpCmdLine);

	if (0 == sCommandLine.CompareNoCase("SendMail"))
	{
		if (m_Environment.m_sSERVER_PROTOCOL.IsEmpty())
		{
			// we are definitly no CGI
			SendMail();
		}
		return FALSE;
	}
	else
	{
		// CGI Part		
		WriteHTTPHeader();
		
		OpenTAG("html");
		
		WriteHead();
		
		OpenTAG("body");
		
//		m_Environment.ToTable();
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
int CNewsLetterApp::Run()
{
	CString command = m_Environment.GetQueryValue("command");

	if (command.IsEmpty())
	{
		if (m_Environment.m_saQueries.GetSize()>0)
			command = m_Environment.m_saQueries.GetAt(0);
	}

#ifdef _DEBUG
	command = "DisplayList";
#endif
	if (0 == command.CompareNoCase("NewEntry"))
	{
		NewEntry();
	}
	else if (0 == command.CompareNoCase("NewDepartment"))
	{
		NewDepartment();
	}
	else if (0 == command.CompareNoCase("NewEmployee"))
	{
		NewEmployee();
	}
	else if (0 == command.CompareNoCase("ShowDepartments"))
	{
		ShowDepartments();
	}
	else if (0 == command.CompareNoCase("ShowEmployees"))
	{
		ShowEmployees();
	}
	else if (0 == command.CompareNoCase("Create"))
	{
		Create();
	}
	else if (0 == command.CompareNoCase("About"))
	{
		About();
	}
	else if (0 == command.CompareNoCase("DisplayAll"))
	{
		DisplayAll();
	}
	else if (0 == command.CompareNoCase("DisplayToday"))
	{
		DisplayToday();
	}
	else if (0 == command.CompareNoCase("DisplayDate"))
	{
		DisplayDate();
	}
	else if (0 == command.CompareNoCase("DisplayCalendar"))
	{
		DisplayCalendar();
	}
	else if (0 == command.CompareNoCase("DisplayList"))
	{
		DisplayList();
	}
	else
	{
		printf("no command specified\n");
	}
	CloseTAG("body");
	CloseTAG("html");
	return 0;
}
//////////////////////////////////////////////////////////////////////
int CNewsLetterApp::ExitInstance()
{
	CloseDebugger();
	return 0;
}
//////////////////////////////////////////////////////////////////////
void CNewsLetterApp::WriteHead()
{
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">\n");
	printf("<meta http-equiv=\"Content-Language\" content=\"de\">\n");
	printf("<title>Videte IT Newsletter</title>\n");
	printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"../../../default.css\">\n");
	printf("<link rel=\"stylesheet\" href=\"../default.css\" type=\"text/css\">\n");
	printf("</head>\n");
}
//////////////////////////////////////////////////////////////////////
void CNewsLetterApp::WriteHTTPHeader()
{
	// HTTP Header
#ifndef _DEBUG
	SYSTEMTIME t;
	GetSystemTime(&t);
	CString sDate;

	GetDateFormat(MAKELCID(MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US),SORT_DEFAULT),
				  0,
				  &t,
				  "ddd',' dd MMM yyyy",
				  sDate.GetBufferSetLength(_MAX_PATH),
				  _MAX_PATH);
	sDate.ReleaseBuffer();

	printf("Content-Type: text/html\n");
	printf("Pragma: no-cache\n");
	printf("Expires: %s %02d:%02d:%02d GMT\n",sDate,t.wHour,t.wMinute,t.wSecond);
	printf("\n");
#endif
}
//////////////////////////////////////////////////////////////////////
void CNewsLetterApp::NewEntry()
{
	CNewsLetterDatabase db;
	CSystemTime t;
	t.GetLocalTime();
	CString sYear;
	sYear.Format("%d",t.wYear);

	if (db.Open(szPath,sYear))
	{
		CString sText,sSubject,sEmployee,sDepartment;
		sText = m_Environment.GetInputValue("text");
		sSubject = m_Environment.GetInputValue("subject");
		sEmployee = m_Environment.GetInputValue("employee");

		if (   sText.IsEmpty()
			|| sSubject.IsEmpty()
			|| sEmployee.IsEmpty())
		{

			printf("<!--webbot BOT=\"GeneratedScript\" PREVIEW=\" \" startspan -->\n");
			printf("<script Language=\"JavaScript\">\n");
			printf("<!--\n");
			printf("function FrontPage_Form1_Validator(theForm)\n");
			printf("{\n");

			printf("if (theForm.subject.value == \"\")\n");
			printf("{\n");
			printf("\talert(\"Geben Sie einen Wert in das Feld Betreff ein.\");\n");
			printf("\ttheForm.subject.focus();\n");
			printf("\treturn (false);\n");
			printf("}\n");

			printf("if (theForm.subject.value.length < 1)\n");
			printf("{\n");
			printf("\talert(\"Geben Sie mindestens 1 Zeichen in das Feld Betreff ein.\");\n");
			printf("\ttheForm.subject.focus();\n");
			printf("\treturn (false);\n");
			printf("}\n");

			printf("if (theForm.subject.value.length > 32)\n");
			printf("{\n");
			printf("\talert(\"Geben Sie höchstens 32 Zeichen in das Feld Betreff ein.\");\n");
			printf("\ttheForm.subject.focus();\n");
			printf("\treturn (false);\n");
			printf("}\n");

			printf("if (theForm.text.value == \"\")\n");
			printf("{\n");
			printf("\talert(\"Geben Sie einen Wert in das Feld Text ein.\");\n");
			printf("\ttheForm.text.focus();\n");
			printf("\treturn (false);\n");
			printf("}\n");

			printf("if (theForm.text.value.length < 1)\n");
			printf("{\n");
			printf("\talert(\"Geben Sie mindestens 1 Zeichen in das Feld Text ein.\");\n");
			printf("\ttheForm.text.focus();\n");
			printf("\treturn (false);\n");
			printf("}\n");

			printf("if (theForm.text.value.length > 254)\n");
			printf("{\n");
			printf("\talert(\"Geben Sie höchstens 254 Zeichen in das Feld Text ein.\");\n");
			printf("\ttheForm.text.focus();\n");
			printf("\treturn (false);\n");
			printf("}\n");
			printf("return (true);\n");
			printf("}\n");
			printf("//-->\n");
			printf("</script>\n");
			printf("<!--webbot BOT=\"GeneratedScript\" endspan -->\n");

			printf("<form method=\"POST\" action=\"newsletter.exe?NewEntry\"\n");
			printf("onsubmit=\"return FrontPage_Form1_Validator(this)\"\n");
			printf("name=\"NewEntry\">\n");
			printf("<p align=\"left\">Name des Mitarbeiters\n");
			printf("</p>\n");
			printf("<p align=\"left\"><select size=\"1\" name=\"employee\">\n");
			CStringArray saEmployees;
			db.GetEmployees(saEmployees);
			for (int i=0;i<saEmployees.GetSize();i++)
			{
				printf("<option>%s</option>\n",saEmployees.GetAt(i));
			}
			printf("</select>\n");

			printf("</p>\n");
			printf(" <p align=\"left\">Betreff:\n");
			printf("</p>\n");
			printf("<p align=\"left\">\n");
			printf("<!--webbot bot=\"Validation\" S-Data-Type=\"String\" B-Value-Required=\"TRUE\" I-Minimum-Length=\"1\" I-Maximum-Length=\"32\" -->\n");
			printf("<input type=\"text\" name=\"subject\" size=\"32\" maxlength=\"32\">\n");
			
			printf("<p align=\"left\">Text:\n");
			printf("</p>\n");
			printf("<p align=\"left\">\n");
			printf("<!--webbot bot=\"Validation\" B-Value-Required=\"TRUE\" I-Minimum-Length=\"1\" I-Maximum-Length=\"254\" -->\n");
			printf("<textarea rows=\"6\" name=\"text\" cols=\"71\"></textarea>\n");
			printf("</p>\n");

			printf("<p align=\"center\">\n");
			printf("<input type=\"submit\" value=\"Abschicken\" name=\"B1\">\n");
			printf("<input type=\"reset\" value=\"Zurücksetzen\" name=\"B2\">\n");
			printf("</p>\n");
			
			printf("</form>\n");
		}
		else
		{
			// TODO insert news
			if (db.Append(sSubject,sText,sEmployee))
			{
				printf("%s: %s von %s hinzugefügt.\n",sSubject,sText,sEmployee);
			}
			else
			{
				printf("%s: %s von %s konnte nicht hinzugefügt werden.\n",sSubject,sText,sEmployee);
			}
		}

		db.Close();
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::DisplayToday()
{
	CNewsLetterDatabase db;
	CSystemTime t;
	t.GetLocalTime();
	CString sYear;
	sYear.Format("%d",t.wYear);

	if (db.Open(szPath,sYear))
	{
		db.DisplayToday();
		db.Close();
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::DisplayDate()
{
	CNewsLetterDatabase db;
	CSystemTime t;
	t.GetLocalTime();
	CString sYear;
	sYear.Format("%d",t.wYear);

	if (db.Open(szPath,sYear))
	{
		CString sDate;
		sDate = m_Environment.GetQueryValue("date");
		if (sDate.IsEmpty())
		{
			db.DisplayToday();
		}
		else
		{
			db.DisplayDate(sDate);
		}
		db.Close();
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::DisplayAll()
{
	CNewsLetterDatabase db;
	CSystemTime t;
	t.GetLocalTime();
	CString sYear;
	sYear.Format("%d",t.wYear);

	if (db.Open(szPath,sYear))
	{
		db.DisplayAll();
		db.Close();
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::About()
{
	printf("NewsLetter CGI<br>\n");
	printf("copyright (c) 2002 Videte IT GmbH<br>\n");
	printf("Westring 431<br>\n");
	printf("24118 Kiel Germany<br>\n");
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::Create()
{
	CString sREMOTE_ADDR = m_Environment.m_sREMOTE_ADDR;
	if (sREMOTE_ADDR == "192.168.0.101")
	{
		CNewsLetterDatabase db;
		CSystemTime t;
		t.GetLocalTime();
		CString sYear;
		sYear.Format("%d",t.wYear);

		if (db.Create(szPath,sYear))
		{
			printf("Database %s successfull created/opened\n",sYear);
		}
		else
		{
			printf("Database %s create/open failed\n",sYear);
		}
	}
	else
	{
		printf("Remote Address %s not allowed to create database.\n",sREMOTE_ADDR);
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::NewDepartment()
{
	CString sREMOTE_ADDR = m_Environment.m_sREMOTE_ADDR;
	if (sREMOTE_ADDR == "192.168.0.101")
	{
		CNewsLetterDatabase db;
		CSystemTime t;
		t.GetLocalTime();
		CString sYear;
		sYear.Format("%d",t.wYear);
		
		if (db.Open(szPath,sYear))
		{
			CString sID,sDepartment;
			sDepartment = m_Environment.GetInputValue("department");
			if (db.AddDepartment(sDepartment,sID))
			{
				printf("Abteilung '%s' mit Nummer '%s' erfolgreich hinzugefügt.\n",sDepartment,sID);
			}
			else
			{
				printf("Abteilung %s nicht hinzugefügt.\n",sDepartment);
			}
			printf("<br>");
			db.DisplayDepartments();
			db.Close();
		}
	}
	else
	{
		printf("Remote Address %s Zugriff verweigert.\n",sREMOTE_ADDR);
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::NewEmployee()
{
	CString sREMOTE_ADDR = m_Environment.m_sREMOTE_ADDR;
	if (sREMOTE_ADDR == "192.168.0.101")
	{
		CNewsLetterDatabase db;
		CSystemTime t;
		t.GetLocalTime();
		CString sYear;
		sYear.Format("%d",t.wYear);
		
		if (db.Open(szPath,sYear))
		{
			CString sDepartment,sEmployee;
			sDepartment = m_Environment.GetInputValue("department");
			sEmployee = m_Environment.GetInputValue("employee");
			if (   sDepartment.IsEmpty() 
				|| sEmployee.IsEmpty())
			{
				printf("<!--webbot BOT=\"GeneratedScript\" PREVIEW=\" \" startspan -->\n");
				printf("<script Language=\"JavaScript\">\n");
				printf("<!--\n");
				printf("function FrontPage_Form1_Validator(theForm)\n");
				printf("{\n");

				printf("if (theForm.Employee.value == \"\")\n");
				printf("{\n");
				printf("alert(\"Geben Sie einen Wert in das Feld \\\"Employee\\\" ein.\");\n");
				printf("theForm.Employee.focus();\n");
				printf("return (false);\n");
				printf("}\n");

				printf("if (theForm.Employee.value.length > 32)\n");
				printf("{\n");
				printf("alert(\"Geben Sie höchstens 32 Zeichen in das Feld \\\"Employee\\\" ein.\");\n");
				printf("theForm.Employee.focus();\n");
				printf("return (false);\n");
				printf("}\n");
				printf("return (true);\n");
				printf("}\n");
				printf("//--></script><!--webbot BOT=\"GeneratedScript\" endspan -->\n");

				db.DisplayEmployees();

				printf("<form method=\"POST\" action=\"newsletter.exe?NewEmployee\" onsubmit=\"return FrontPage_Form1_Validator(this)\" name=\"NewEmployee\">\n");
				printf("<p align=\"left\">Name des Mitarbeiters\n");
				printf("</p>\n");
				printf("<p align=\"left\">\n");
				printf("<!--webbot bot=\"Validation\" S-Data-Type=\"String\" B-Value-Required=\"TRUE\" I-Maximum-Length=\"32\" -->\n");
				printf("<input type=\"text\" name=\"Employee\" size=\"20\" maxlength=\"32\">\n");
				printf("</p>\n");
				printf("<p align=\"left\">Abteilung:\n");
				printf("</p>\n");
				printf("<p align=\"left\"><select size=\"1\" name=\"Department\">\n");

				CStringArray saDepartments;
				db.GetDepartments(saDepartments);
				for (int i=0;i<saDepartments.GetSize();i++)
				{
					printf("<option>%s</option>\n",saDepartments.GetAt(i));
				}
				
				printf("</select>\n");
				printf("</p>\n");
				printf("<p align=\"center\">\n");
				printf("<input type=\"submit\" value=\"Abschicken\" name=\"B1\">\n");
				printf("<input type=\"reset\" value=\"Zurücksetzen\" name=\"B2\">\n");
				printf("</p>\n");
				printf("</form>\n");
			}
			else
			{
				CString sD_ID,sNr;
				if (db.AddDepartment(sDepartment,sD_ID))
				{
					if (db.AddEmployee(sEmployee,sD_ID,sNr))
					{
						printf("Mitarbeiter '%s' mit Nummer '%s' erfolgreich hinzugefügt.\n",sEmployee,sNr);
					}
					else
					{
						printf("Mitarbeiter %s nicht hinzugefügt.\n",sEmployee);
					}
				}
				else
				{
				}
				printf("<br>");
				db.DisplayEmployees();
			}
			db.Close();
		}
	}
	else
	{
		printf("Remote Address %s Zugriff verweigert.\n",sREMOTE_ADDR);
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::ShowDepartments()
{
	CNewsLetterDatabase db;
	CSystemTime t;
	t.GetLocalTime();
	CString sYear;
	sYear.Format("%d",t.wYear);
	
	if (db.Open(szPath,sYear))
	{
		db.DisplayDepartments();
		db.Close();
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::ShowEmployees()
{
	CNewsLetterDatabase db;
	CSystemTime t;
	t.GetLocalTime();
	CString sYear;
	sYear.Format("%d",t.wYear);
	
	if (db.Open(szPath,sYear))
	{
		db.DisplayEmployees();
		db.Close();
	}
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::SendMail()
{
	CEmail email;
	CSystemTime t;
	t.GetLocalTime();

	CString sSubject;
	sSubject.Format("Videte IT Interner Newsletter vom %s\n",t.GetDate());
	CString sBody;

	sBody.Format("http://lucky/newsletter/cgi-bin/NewsLetter.exe?DisplayDate&date=%s",t.GetDBDate());

	email.SendMail("odin","uf@videte.com","alle@videte.com",sSubject,sBody);
}
static char szDays[7][3] = {"So", "Mo","Di","Mi","Do","Fr","Sa"};
////////////////////////////////////////////////////////////////
void CNewsLetterApp::DisplayCalendar()
{
	CSystemTime t;
	int iKW = 1;
	t.wDay = 1;
	t.wDayOfWeek = 2;
	t.wMonth = 1;
	t.wYear = 2002;
	t.wHour = t.wMinute = t.wSecond = t.wMilliseconds = 0;

	CTAG table("table border=\"1\" cellspacing=\"0\"");
	OpenTAG("tr");
	printf("<td>KW %02d</td>\n",iKW);
	printf("<td>-</td>\n");
	for (;t.wYear < 2003;t.IncrementTime(1,0,0,0,0))
	{
		if (t.wDayOfWeek>0 && t.wDayOfWeek<6)
		{
			printf("<td><a href=\"NewsLetter.exe?DisplayDate&date=%s\"> %s %02d.%02d. </a></td>",
				t.GetDBDate(),szDays[t.wDayOfWeek],t.wDay,t.wMonth);
		}
		if (t.wDayOfWeek == 0)
		{
			CloseTAG("tr");
			OpenTAG("tr");
			++iKW;
			iKW = iKW%53;
			if (iKW == 0)
			{
				iKW = 1;
			}
			printf("<td>KW %02d</td>\n",iKW);
		}
	}
	printf("<td>-</td>\n");
	printf("<td>-</td>\n");
	printf("<td>-</td>\n");
	CloseTAG("tr");
}
////////////////////////////////////////////////////////////////
void CNewsLetterApp::DisplayList()
{
	CNewsLetterDatabase db;
	CSystemTime t;
	t.GetLocalTime();
	CString sYear;
	sYear.Format("%d",t.wYear);

	if (db.Open(szPath,sYear))
	{
		db.DisplayList("");
		db.Close();
	}
}