// KeyGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KeyGen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

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
		// TODO: code your application's behavior here.
		int i;
		int c;
		int d;
		int i1, i2, i3, i4, i5, i6, i7, iDummy, iCheck, l;
		i1=i2=i3=i4=i5=i6=i7=iDummy=iCheck=l=0;
		CString s,r,sDummy;

		CStdioFile files[10];

		for (i=0;i<10;i++)
		{
			s.Format("serial%d.csv",i);
			files[i].Open(s,CFile::modeCreate|CFile::modeReadWrite);
		}

		r = "123456789\n";
		for (i = 111111;i<999999;i++)
		{
			if (CRSA::RealIsPrim(i))
			{
				l++;
				d = i;
				c = 0;
				while (d>0)
				{
					c += (d%10);
					d/=10;
				}
				c = c%10;
				s.Format("%01d%06d",c,i);

				i1 = i/100000;
				iDummy = i-i1*100000;
				i2 = iDummy/10000;
				iDummy = i-i1*100000-i2*10000;
				i3 = iDummy/1000;
				iDummy = i-i1*100000-i2*10000-i3*1000;
				i4 = iDummy/100;
				iDummy = i-i1*100000-i2*10000-i3*1000-i4*100;
				i5 = iDummy/10;
				iDummy = i-i1*100000-i2*10000-i3*1000-i4*100-i5*10;
				i6 = iDummy;
				iCheck = 2*i1 + 3*i2 + 4*i3 + 4*i4 + 3*i5 + 2*i6;
				iCheck %= 100;
				sDummy.Format("%02d", iCheck);

				r.SetAt(0,s[0]);
				r.SetAt(1,s[4]);
				r.SetAt(2,s[3]);
				r.SetAt(3,s[2]);
				r.SetAt(4,s[6]);
				r.SetAt(5,s[1]);
				r.SetAt(6,s[5]);
				r.SetAt(7,sDummy[0]);
				r.SetAt(8,sDummy[1]);

 				files[c].WriteString(r);
			}
		}

	}

	return nRetCode;
}


