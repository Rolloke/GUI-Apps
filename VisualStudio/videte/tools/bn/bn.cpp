// bn.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "bn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

//////////////////////////////////////////////////////////////////////
void WriteHTTPHeader()
{
	// HTTP Header
	printf("Content-Type: text/html\n");
	printf("Pragma: no-cache\n");
	printf("Expires: Tue, 24 Dec 1996 00:00:00 GMT\n");
	printf("\n");
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

		WriteHTTPHeader();
		
		printf("<html>\n");
		printf("<body>\n");
		printf("<ul>\n");
		CString sSoftware = environment.GetQueryValue("software");
		CString sStart = environment.GetQueryValue("start");
		CString sStop = environment.GetQueryValue("stop");

		CString sFile;
		
		if (0==sSoftware.CompareNoCase("idip"))
		{
			sFile = "\\\\file01\\software\\bin\\dvrt\\vinfo.pvi";
		}
		else if (0==sSoftware.CompareNoCase("dts"))
		{
			sFile = "\\\\file01\\software\\bin\\dv\\vinfo.pvi";
		}
		CString s1;
		CFile fFile;
		int iBuildNr = 0;
		
		if (fFile.Open(sFile,CFile::modeRead))
		{
			BYTE b = 0;
			while (fFile.Read(&b,1))
			{
				s1 += b;
			}
		}
		CString sBuildNumber = s1.Mid(2,3);
		int iBuildNumber = 0;
		int iStart = 0;
		int iStop = 0;
		sscanf(sBuildNumber,"%d",&iBuildNumber);
		sscanf(sStart,"%d",&iStart);
		sscanf(sStop,"%d",&iStop);

		printf("%s %d-%d\n",sSoftware,iStart,iStop);

		printf("<ul>\n");
		for (int i=iStart;i<=iBuildNumber&&i<=iStop;i++)
		{
			printf("<li>\n");
			CString s;
			s.Format("<a href=\"http://lucky/software/buildnotes/%s/Buildnotes%d.htm\">%s Build %d</a>\n",sSoftware,i,sSoftware,i);
			printf(s);
			printf("</li>\n");
		}
		printf("</ul>\n");

		printf("</body>\n");
		printf("</html>\n");
	}

	return nRetCode;
}


