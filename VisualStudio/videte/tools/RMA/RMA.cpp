// RMA.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "RMAApp.h"

CRMAApp theApp;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if (AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
	{
		if (theApp.InitApplication())
		{
			if (theApp.InitInstance())
			{
				theApp.Run();
			}
			return theApp.ExitInstance();
		}
	}
	return FALSE;
}





