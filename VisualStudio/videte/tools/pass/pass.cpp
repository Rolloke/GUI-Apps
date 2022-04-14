// pass.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "pass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

char Transform(char c)
{
	switch (c)
	{
	case '0':
		return 'q';
	case '1':
		return 'w';
	case '2':
		return 'e';
	case '3':
		return 'r';
	case '4':
		return 't';
	case '5':
		return 'z';
	case '6':
		return 'u';
	case '7':
		return 'i';
	case '8':
		return 'o';
	case '9':
		return 'p';
	case 'A':
		return 'a';
	case 'B':
		return 's';
	case 'C':
		return 'd';
	case 'D':
		return 'f';
	case 'E':
		return 'g';
	case 'F':
		return 'h';
	}
	return ' ';
}
CString TransPassword(CString s)
{
	CString sP = s;
	sP.MakeUpper();
	for (int i=0;i<sP.GetLength();i++)
	{
		sP.SetAt(i,Transform(sP[i]));
	}
	return sP;
}
//////////////////////////////////////////////////////////////////////
void WriteHTTPHeader()
{
	// HTTP Header
	printf("Content-Type: text/html\n");
	printf("Pragma: no-cache\n");
	printf("Expires: Tue, 24 Dec 1996 00:00:00 GMT\n");
	printf("\n");
}

void PrintTimezoneRow(const CString& sTimezonename,int offset)
{
	TIME_ZONE_INFORMATION tzi;
	SYSTEMTIME utc;
	SYSTEMTIME local;
	FILETIME ft;
//	LARGE_INTEGER li;

	GetTimeZoneInformation(&tzi);
	GetSystemTime(&utc);
	tzi.Bias = -offset*60;
	SystemTimeToTzSpecificLocalTime(&tzi,&utc,&local);
	SystemTimeToFileTime(&local,&ft);

/*
	li.HighPart = ft.dwHighDateTime;
	li.LowPart = ft.dwLowDateTime;
	li.QuadPart += (__int64)10000000 * (__int64)3600 * (__int64)offset;
	ft.dwHighDateTime = li.HighPart;
	ft.dwLowDateTime = li.LowPart;
*/

	DWORD dw = 0;
	CString sP,sTime,sDWORD,sPassword;
	CTime ct(local);

	dw = (DWORD)ct.GetTime();
	sDWORD.Format("%08lx",dw);
	sPassword = TransPassword(sDWORD);
	sTime.Format("%02d.%02d.%04d %02d:%02d:%02d",ct.GetDay(),ct.GetMonth(),
		ct.GetYear(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
	printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",sTimezonename,sTime,sPassword);
}
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		CEnvironment environment;

/*		
		WriteHTTPHeader();
		
		printf("<html>\n");
		printf("<link rel=\"STYLESHEET\" type=\"text/css\" href=\"../../style.css\">\n");
		printf("<body>\n");*/

		printf("<table border=\"0\">\n");

		printf("<tr><td>Time Zone</td><td>Current Local Time</td><td>Password for 30min</td></tr>\n");

		PrintTimezoneRow("(GMT) Greenwich Mean Time : Dublin, Edinburgh, Lisbon, London",0);
		PrintTimezoneRow("(GMT+01:00) Brussels, Berlin, Bern, Rome, Stockholm, Vienna",1);
		PrintTimezoneRow("(GMT+01:00) Bratislava, Budapest, Ljubljana, Prague, Warsaw",1);
		PrintTimezoneRow("(GMT+01:00) Amsterdam, Copenhagen, Madrid, Paris, Vilnius",1);
		PrintTimezoneRow("(GMT+02:00) Helsinki, Riga, Tallinn, Athen, Istanbul, Minsk, Israel",2);
		PrintTimezoneRow("(GMT+03:00) Moscow, St. Petersburg, Volgograd",3);
		PrintTimezoneRow("(GMT+03:00) Baghdad, Kuwait, Riyadh",3);
		PrintTimezoneRow("(GMT+04:00) Abu Dhabi, Muscat",3);
		
		printf("</table>\n");
/*
		printf("</body>\n");
		printf("</html>\n");*/
	}

	return nRetCode;
}



