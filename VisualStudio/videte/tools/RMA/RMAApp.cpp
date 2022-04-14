// RMAApp.cpp: implementation of the CRMAApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RMAApp.h"
#include "RMADatabase.h"


static char szData[] = "c:\\data\\rma.dbf";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRMAApp::CRMAApp()
{
}
//////////////////////////////////////////////////////////////////////
CRMAApp::~CRMAApp()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CRMAApp::InitInstance()
{
	WriteHTTPHeader();

	if (!AfxSocketInit())
	{
		return FALSE;
	}

	OpenTAG("html");
	OpenTAG("body");

	m_Environment.ToTable();

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
int CRMAApp::Run()
{
	CString command = m_Environment.GetValue("command");
#ifdef _DEBUG
	command = "NewEntry";
#endif
	if (0 == command.CompareNoCase("NewEntry"))
	{
		NewEntry();
	}
	else if (0 == command.CompareNoCase("About"))
	{
		About();
	}
	else if (0 == command.CompareNoCase("DisplayAll"))
	{
		DisplayAll();
	}
	else
	{
		printf("no command specified\n");
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
int CRMAApp::ExitInstance()
{
	CloseTAG("body");
	CloseTAG("html");
	return 0;
}
//////////////////////////////////////////////////////////////////////
void CRMAApp::WriteHTTPHeader()
{
	// HTTP Header
	printf("Content-Type: text/html\n");
	printf("Pragma: no-cache\n");
	printf("Expires: Tue, 24 Dec 1996 00:00:00 GMT\n");
	printf("\n");
}
//////////////////////////////////////////////////////////////////////
void CRMAApp::NewEntry()
{
	CRMADatabase db;
	int r;

	r = db.m_data4.open(db.m_cb,szData);
	if (e4fileFind == r)
	{
		db.m_cb.errorSet(0);
		r = db.m_data4.create(db.m_cb,szData,db.m_fieldinfo);
	}
	if (r4success == r)
	{
		CString sBody;
		CString sRMANr;

		if (db.m_data4.recCount()>0)
		{
			db.m_data4.bottom();
		}

		Field4 fRMANr;

		r = fRMANr.init(db.m_data4,"RMANr");
		if (r4success == r)
		{

			sRMANr = fRMANr.str();

			int iYear,iMonth,iNr;

			iYear = iMonth = iNr = 0;

			sscanf(sRMANr,"%04d%02d%04d",&iYear,&iMonth,&iNr);

			CTime t = CTime::GetCurrentTime();
			int y;
			y = t.GetYear();
			if (   (iYear != y)
				|| (iMonth != t.GetMonth())
				)
			{
				iNr = 1;
				iYear = y;
				iMonth = t.GetMonth();
			}
			else
			{
				iNr++;
			}

			sRMANr.Format("%04d%02d%04d",iYear,iMonth,iNr);

			// append new item
			r = db.m_data4.appendStart();
			if (r == r4success)
			{
				db.m_data4.blank();

				for (int i=1;i<=db.m_data4.numFields();i++)
				{
					Field4 f;
					r = f.init(db.m_data4,i);
					if (r == r4success)
					{
						f.assign(m_Environment.GetValue(f.name()));
						sBody += CString(f.name()) + " = " + f.str() + "\n";
					}
					else
					{
						printf("code base error %d\n",r);
						db.m_cb.errorSet(0);
					}
				}
				Field4 fRMANr;
				r = fRMANr.init(db.m_data4,"RMANr");
				if (r4success == r)
				{
					fRMANr.assign(sRMANr);
				}
				else
				{
					printf("code base error RMANr init %d\n",r);
					db.m_cb.errorSet(0);
				}
			}
			else
			{
				printf("code base error appendStart %d\n",r);
				db.m_cb.errorSet(0);
			}

			r = db.m_data4.append();
			if (r4success !=r)
			{
				printf("code base error append %d\n",r);
				db.m_cb.errorSet(0);
			}
		}
		else
		{
			printf("code base error RMANr init %d\n",r);
			db.m_cb.errorSet(0);
		}

		db.m_data4.close();

		printf("Ihr Anliegen wurde unter der RMA Nr. %s registriert.<br>\n",sRMANr);
		printf("Sie erhalten demnächst die Rücksendeunterlagen.\n",sRMANr);

//#ifdef _DEBUG
		CEmail email;
		CString sSubject;
		sSubject.Format("RMA: %s",sRMANr);
		email.SendMail("odin","uf@videte.com","uf@videte.com",sSubject,sBody);
//#endif
	}
	else
	{
		printf("code base error %d\n",r);
		db.m_cb.errorSet(0);
	}
}
////////////////////////////////////////////////////////////////
void CRMAApp::DisplayAll()
{
	CRMADatabase db;
	int r;

	r = db.m_data4.open(db.m_cb,szData);
	if (r4success == r)
	{
		if (db.m_data4.recCount()>0)
		{
			CTAG table("table border=\"1\"");
			// title
			for (int rc = db.m_data4.top();rc!=r4eof;rc=db.m_data4.skip())
			{
				// row
				CTAG tr("tr");
				for (int i=1;i<db.m_data4.numFields();i++)
				{
					CTAG td("td");
					Field4 f;
					r = f.init(db.m_data4,i);
					if (r == r4success)
					{
						CString s(f.str());
						s.TrimLeft();
						s.TrimRight();
						if (s.IsEmpty())
						{
							s = "-";
						}
						printf(s);
					}
					else
					{
						printf("-");
					}
				}

			}
		}
		else
		{
			printf("Es sind keine Datensätze vorhanden.\n");
		}
		db.m_data4.close();
	}
	else
	{
	}
}
////////////////////////////////////////////////////////////////
void CRMAApp::About()
{
	printf("RMA CGI<br>\n");
	printf("copyright (c) 2000 Videte IT GmbH<br>\n");
	printf("Schauenburger Str. 116<br>\n");
	printf("24118 Kiel Germany<br>\n");
}