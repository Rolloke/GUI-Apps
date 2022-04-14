// Email.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Email.h"
#include "CEmail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BOOL DoesFileExist(const char* szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

int _tmain(int argc, TCHAR* argv[], TCHAR* /*envp[]*/)
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		nRetCode = 1;
	}
	else
	{
		if (argc == 6)
		{
			if (AfxSocketInit())
			{
				CEmail email;
				CString sBody;
				if (DoesFileExist(argv[5]))
				{
					CFile f;
					if (f.Open(argv[5],CFile::modeRead,NULL))
					{
						DWORD dwLen = f.GetLength();
						f.SeekToBegin();
						f.Read(sBody.GetBufferSetLength(dwLen),dwLen);
						f.Close();
						sBody.ReleaseBuffer();
					}
				}
				else
				{
					sBody = (LPCSTR)argv[5];
				}

				if (email.SendMail(argv[1],argv[2],argv[3],argv[4],sBody))
				{
					printf("sent email %s from %s to %s\n",argv[4],argv[2],argv[3]);
				}
				else
				{
					printf("cannot send email %s from %s to %s\n",argv[4],argv[2],argv[3]);
				}
			}
		}
		else
		{
			printf("usage  'email.exe <server> <sender> <receiver> <subject> <text/filepath>'\n");
			printf("sample 'email.exe odin uf@videte.com alle@videte.com Greetings ""Hello all together.""'\n");
			printf("sample 'email.exe odin uf@videte.com alle@videte.com Greetings ""c:\\winter greetings.html""'\n");
		}
	}

	return nRetCode;
}


