// JVIEW.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include "CJpeg.h"

#ifndef _DEBUG
#undef OutputDebugString
#define OutputDebugString(x)
#endif

#define MAX_LOADSTRING 100

#define HEIGHT(x) (x.bottom-x.top)
#define WIDTH(x) (x.right-x.left)

#define RECT_WIDTH(rc) (rc.right - rc.left)
#define RECT_HEIGHT(rc) (rc.bottom - rc.top)

// Global Variables:
HWND hWnd = NULL;
HINSTANCE hInst = NULL;// current instance
TCHAR szTitle[MAX_LOADSTRING];// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];// The title bar text
TCHAR szPathname[_MAX_PATH];
TCHAR szHtmlTitle[MAX_LOADSTRING];	// The Html-Title

BYTE* g_pBuffer = NULL;
DWORD g_dwBufferLen = 0;
RECT  g_rcZoom = {0,0,0,0};
CJpeg jpeg;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void OnFileOpen();
void LoadJpegFile();
void OnDraw(HDC hDC, BOOL bPrint);
void OnFilePrint();
void OnEditCopy();
void OnLButtonDown();
void OnRButtonDown();
void ZoomDisplayWindow(const POINT &point, BOOL bIn);
BOOL ScrollDisplayWindow();
BOOL LoadHtmlFile();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     /*lpCmdLine*/,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_JVIEW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_JVIEW);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if (g_pBuffer)
	{
		delete []g_pBuffer;
		g_pBuffer = NULL;
	}
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_JVIEW);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_JVIEW;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED|WS_SYSMENU,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void OnFileOpen()
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = "JPEG Files\0*.jpg\0\0";
	ofn.Flags = OFN_HIDEREADONLY;
	ofn.lpstrFile = szPathname;
	ofn.nMaxFile = _MAX_PATH;
	if (GetOpenFileName(&ofn))
	{
		OutputDebugString(szPathname);
		OutputDebugString("\n");
		LoadJpegFile();
		LoadHtmlFile();
	}
}
/////////////////////////////////////////////////////////////
void LoadJpegFile()
{
	WIN32_FIND_DATA find;

	HANDLE hFind = FindFirstFile(szPathname,&find);

	if (hFind!=INVALID_HANDLE_VALUE)
	{
		g_dwBufferLen = find.nFileSizeLow;

		HANDLE hFile = CreateFile(szPathname,
								  GENERIC_READ,
								  FILE_SHARE_READ,
								  NULL,
								  OPEN_EXISTING,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);
		if (hFile)
		{
			DWORD dwRead = 0;
			g_pBuffer = new BYTE[g_dwBufferLen];

			if (ReadFile(hFile,g_pBuffer,g_dwBufferLen,&dwRead,NULL))
			{
				// decode it
				if (jpeg.DecodeJpegFromMemory(g_pBuffer,dwRead))
				{
					OutputDebugString("decoded\n");
					SIZE s = jpeg.GetImageDims();
					RECT cr,fr;
					GetWindowRect(hWnd,&fr);
					GetClientRect(hWnd,&cr);
					s.cx += (fr.right-fr.left)-(cr.right-cr.left);
					s.cy += (fr.bottom-fr.top)-(cr.bottom-cr.top);
					SetWindowPos(hWnd,NULL,0,0,s.cx,s.cy,SWP_NOMOVE);
					InvalidateRect(hWnd,NULL,TRUE);
				}
				else
				{
					OutputDebugString("not decoded\n");
				}
			}
			else
			{
				delete []g_pBuffer;
				g_pBuffer = NULL;
			}

			CloseHandle(hFile);
		}
		FindClose(hFind);
	}
}
/////////////////////////////////////////////////////////////
void OnDraw(HDC hDC, BOOL bPrint)
{
	RECT rt;
	GetClientRect(hWnd, &rt);
	if (g_pBuffer)
	{
		jpeg.SetBackgroundColor(RGB(0,128,0));
		jpeg.SetTextColor(RGB(200,200,200));
		jpeg.SetOSDText1(0,0, szHtmlTitle);
		jpeg.OnDraw(hDC,rt,g_rcZoom, bPrint);
	}
}
/////////////////////////////////////////////////////////////
void OnFilePrint()
{
	if (g_pBuffer)
	{
		PRINTDLG pd;
		ZeroMemory(&pd,sizeof(pd));
		pd.lStructSize = sizeof(pd);
		pd.hInstance = hInst;
		pd.hwndOwner = hWnd;
		pd.Flags = PD_RETURNDC;//|PD_RETURNDEFAULT;
		
		if (PrintDlg(&pd))
		{
			HDC hDCPrint = pd.hDC;
			if (hDCPrint)
			{
	//			theApp.SetPrinterOriantation(dcPrint, dlg.GetDeviceName(), DMORIENT_LANDSCAPE);
				
				DOCINFO di;
				ZeroMemory(&di, sizeof(DOCINFO));
				di.cbSize = sizeof(DOCINFO);
				di.lpszDocName = szPathname;

				if (SP_ERROR != StartDoc(hDCPrint,&di))
				{
					if (0<StartPage(hDCPrint))
					{
						RECT rect;
						rect.left = 0; 
						rect.top = 0;
						rect.right = GetDeviceCaps(hDCPrint,HORZRES);
						rect.bottom = GetDeviceCaps(hDCPrint,VERTRES);
						DPtoLP(hDCPrint,(LPPOINT)&rect,2);
					
						SIZE TextSize;
						GetTextExtentPoint32(hDCPrint, szHtmlTitle, strlen(szHtmlTitle), &TextSize);
						int nOff = TextSize.cy;
						TextOut(hDCPrint, rect.left+nOff, rect.top + nOff, szHtmlTitle, strlen(szHtmlTitle));

						RECT printRect;
						printRect.left = rect.left + nOff;
						printRect.top = rect.top + 3 * nOff;

						SIZE ImageSize = jpeg.GetImageDims();

						if (HEIGHT(rect) > WIDTH(rect))
						{
							// Portrait
							printRect.right = rect.right - nOff;
							printRect.bottom = printRect.top + WIDTH(printRect) * ImageSize.cy / ImageSize.cx;
						}
						else
						{
							// Landscape
							printRect.bottom = rect.bottom - nOff;
							printRect.right = printRect.left + HEIGHT(printRect) * ImageSize.cx / ImageSize.cy;
						}

						jpeg.SetOSDText1(0,0,"");
						jpeg.OnDraw(hDCPrint, printRect, g_rcZoom, TRUE);

						EndPage(hDCPrint);
					}
					else
					{
						OutputDebugString("cannot start page\n");
					}
					
					EndDoc(hDCPrint);
				}
				else
				{
					OutputDebugString("cannot start doc\n");
				}

				DeleteDC(hDCPrint);
			}
			else
			{
				OutputDebugString("cannot get printer dc\n");
			}
		}
		else
		{
			OutputDebugString("cannot get printer defaults\n");
		}
	}
}
/////////////////////////////////////////////////////////////
void OnEditCopy()
{
	jpeg.Copy(hWnd);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_FILE_OPEN:
				   OnFileOpen();
				   break;
				case ID_FILE_PRINT:
				   OnFilePrint();
				   break;
				case ID_EDIT_COPY:
					OnEditCopy();
					break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			OnDraw(hdc,FALSE);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown();
			break;
		case WM_RBUTTONDOWN:
			OnRButtonDown();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void OnLButtonDown() 
{
	if (!ScrollDisplayWindow())
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		ZoomDisplayWindow(point, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void OnRButtonDown() 
{
	if (!(IsRectEmpty(&g_rcZoom)))
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		ZoomDisplayWindow(point,FALSE);
	}
}

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
void ZoomDisplayWindow(const POINT &point, BOOL bIn)
{
	if (jpeg.IsValid())
	{
		RECT rcVideo,rcTrack,rcZoom;
		int		nX1, nY1, nX2, nY2;
		SIZE s = jpeg.GetImageDims();

		GetClientRect(hWnd, &rcVideo);
		if (IsRectEmpty(&g_rcZoom))
		{
			g_rcZoom.left	= 0;
			g_rcZoom.top	= 0;
			g_rcZoom.right	= s.cx;
			g_rcZoom.bottom	= s.cy;
		}

		if (bIn)
		{
			rcTrack.left	= point.x - RECT_WIDTH(rcVideo)  / 4;
			rcTrack.top		= point.y - RECT_HEIGHT(rcVideo) / 4;
			rcTrack.right	= point.x + RECT_WIDTH(rcVideo)  / 4;
			rcTrack.bottom	= point.y + RECT_HEIGHT(rcVideo) / 4;

			int nXOff = 0;
			int nYOff = 0;

			// Liegt das Rechteck auﬂerhalb des sichtbaren Bereiches?
			if (rcTrack.left < rcVideo.left)
				nXOff = rcVideo.left - rcTrack.left;
			else
				nXOff = 0;

			if (rcTrack.top < rcVideo.top)
				nYOff = rcVideo.top - rcTrack.top;
			else
				nYOff = 0;

			OffsetRect(&rcTrack, nXOff, nYOff);
	
			if (rcTrack.right > rcVideo.right)
				nXOff = rcTrack.right - rcVideo.right;
			else
				nXOff = 0;

			if (rcTrack.bottom > rcVideo.bottom)
				nYOff = rcTrack.bottom - rcVideo.bottom;
			else
				nYOff = 0;
			OffsetRect(&rcTrack, -nXOff, -nYOff);

			rcZoom.left		= rcTrack.left	- rcVideo.left;
			rcZoom.right	= rcTrack.right	- rcVideo.left;
			rcZoom.top		= rcTrack.top	- rcVideo.top;
			rcZoom.bottom	= rcTrack.bottom- rcVideo.top;
			
			// Normalize Rect
			if (rcZoom.left > rcZoom.right)
			{
				int nLeft = rcZoom.left;
				rcZoom.left = rcZoom.right;
				rcZoom.right= nLeft;
			}
			if (rcZoom.top > rcZoom.bottom)
			{
				int nTop = rcZoom.top;
				rcZoom.top = rcZoom.bottom;
				rcZoom.bottom= nTop;
			}

			RECT rcTemp = {0,0, RECT_WIDTH(rcVideo), RECT_HEIGHT(rcVideo)};
			IntersectRect(&rcZoom, &rcZoom, &rcTemp); 
			nX1 = g_rcZoom.left + rcZoom.left		* RECT_WIDTH(g_rcZoom)  / RECT_WIDTH(rcVideo);
			nY1	= g_rcZoom.top  + rcZoom.top		* RECT_HEIGHT(g_rcZoom) / RECT_HEIGHT(rcVideo); 
			nX2	= g_rcZoom.left + rcZoom.right		* RECT_WIDTH(g_rcZoom)  / RECT_WIDTH(rcVideo);
			nY2	= g_rcZoom.top  + rcZoom.bottom		* RECT_HEIGHT(g_rcZoom) / RECT_HEIGHT(rcVideo); 
		
			// Den maximalen Zoomfaktor begrenzen
			if (s.cx / RECT_WIDTH(g_rcZoom) < 5)
			{				
				g_rcZoom.left	= nX1;
				g_rcZoom.top	= nY1;
				g_rcZoom.right	= nX2;
				g_rcZoom.bottom	= nY2;
			}
			else
				MessageBeep(0);
		}
		else
		{
			ZeroMemory(&g_rcZoom, sizeof(g_rcZoom));
		}

		char sBuffer[255];
		sprintf(sBuffer, "l=%d t=%d r=%d b=%d\n", g_rcZoom.left, g_rcZoom.top, g_rcZoom.right, g_rcZoom.bottom);
		OutputDebugString(sBuffer);
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL ScrollDisplayWindow()
{
	BOOL bReturn = FALSE;

	if (jpeg.IsValid())
	{
		POINT StartPoint;					 
		POINT CurrentPoint;					 
		RECT  rcSavedZoom;
		RECT  rcImage;

		rcSavedZoom = g_rcZoom;

		SIZE size = jpeg.GetImageDims(); 
		rcImage.left	= 0;
		rcImage.top		= 0;
		rcImage.right	= rcImage.left	+ size.cx;
		rcImage.bottom	= rcImage.top	+ size.cy;

		ShowCursor(FALSE);
		GetCursorPos(&StartPoint);
		CurrentPoint = StartPoint;

		while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000)) 
		{
			GetCursorPos(&CurrentPoint);

			g_rcZoom = rcSavedZoom;
			g_rcZoom.left		+= StartPoint.x - CurrentPoint.x;
			g_rcZoom.right	+= StartPoint.x - CurrentPoint.x;
			g_rcZoom.top		+= StartPoint.y - CurrentPoint.y;
			g_rcZoom.bottom	+= StartPoint.y - CurrentPoint.y;
			
			if (g_rcZoom.left<0)
			{
				g_rcZoom.right += abs(g_rcZoom.left);
				g_rcZoom.left	= 0;
			}
			if (g_rcZoom.top<0)
			{
				g_rcZoom.bottom += abs(g_rcZoom.top);
				g_rcZoom.top = 0;
			}
			if (g_rcZoom.right>rcImage.right)
			{
				g_rcZoom.left -= (g_rcZoom.right-rcImage.right);
				g_rcZoom.right = rcImage.right;
			}
			if (g_rcZoom.bottom>rcImage.bottom)
			{
				g_rcZoom.top -= (g_rcZoom.bottom-rcImage.bottom);
				g_rcZoom.bottom  = rcImage.bottom;
			}

			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
		}
		ShowCursor(TRUE);
		bReturn = ((StartPoint.x != CurrentPoint.x) || (StartPoint.y != CurrentPoint.y));
	}
	return bReturn;
}

BOOL LoadHtmlFile()
{
	BOOL bResult		= FALSE;

	if (strlen(szPathname) <= 0)
		return FALSE;

	char* pszHtmlFileName = new char[strlen(szPathname)+2];
	strcpy(pszHtmlFileName, szPathname);

	// Ersetze .jpg durch .html
	char* pDest = strrchr(pszHtmlFileName, '.');
	if (pDest != NULL)
	{
		int nI = pDest - pszHtmlFileName;
		if (nI > 0)
			strcpy(&pszHtmlFileName[nI], ".html\0");
	}

	WIN32_FIND_DATA find;

	HANDLE hFind = FindFirstFile(pszHtmlFileName,&find);

	if (hFind!=INVALID_HANDLE_VALUE)
	{
		DWORD dwBufferLen = find.nFileSizeLow;

		HANDLE hFile = CreateFile(pszHtmlFileName,
								  GENERIC_READ,
								  FILE_SHARE_READ,
								  NULL,
								  OPEN_EXISTING,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);
		if (hFile)
		{
			DWORD dwRead = 0;
			BYTE* pBuffer = new BYTE[dwBufferLen];

			if (ReadFile(hFile,pBuffer,dwBufferLen,&dwRead,NULL))
			{
				char *pStart = strstr((const char*)pBuffer, "<title>");
				if (pStart)
				{
					char* pEnd = strstr(pStart, "</title>");
					*pEnd = '\0';

					strcpy(&szHtmlTitle[0], pStart+7);
					bResult = TRUE;
				}		
			}

			delete []pBuffer;
			pBuffer = NULL;

			CloseHandle(hFile);
		}
		FindClose(hFind);
	}

	delete []pszHtmlFileName;

	return bResult;
}
