// Definiert einige Typedefinitionen und Funktionsnamen um, sodaﬂ 
// die Konvertierung von 16Bit auf 32Bit leichter wird.

#ifdef WIN32
	#include <io.h>
	#define huge
	#define far
	#define _far
	#define __far
	#define __export 			__declspec( dllexport )
	#define _fmemcpy(a,b,c) 	memcpy(a,b,c)
	#define _fmemset(a,b,c) 	memset(a,b,c)
	#define _fstrcpy(a,b)   	strcpy(a,b)
	#define _fstrcat(a,b)   	strcat(a,b)
	#define _hwrite(a,b,c)  	write(a,b,c) 
	#define inp(a)				_inp(a)
	#define outp(a,b)			_outp(a,b)
	#define GCW_HBRBACKGROUND	GCL_HBRBACKGROUND
	#define GCW_HICON			GCL_HICON  

	#define GWW_ID GWL_ID
	#define GET_WM_COMMAND_CMD(wp, lp) 	HIWORD(wp)
	#define GET_WM_COMMAND_ID(wp, lp) 	LOWORD(wp)
	#define GET_WM_COMMAND_HWND(wp, lp) ((HWND)lp)
 	#define GET_THUMBPOSITION(wp, lp)   ((int)(short)HIWORD(wParam))
#else
	#define DLL_PROCESS_ATTACH	0
	#define DLL_PROCESS_DETACH	1
	#define DLL_THREAD_ATTACH	2
	#define DLL_THREAD_DETACH	3

	#define GET_WM_COMMAND_CMD(wp, lp) 	LOWORD(lp)
	#define GET_WM_COMMAND_ID(wp, lp) 	(wp)
	#define GET_WM_COMMAND_HWND(wp, lp) ((HWND)HIWORD(lp))
    #define GET_THUMBPOSITION(wp, lp)   ((int)(short)LOWORD(lParam))
#endif

