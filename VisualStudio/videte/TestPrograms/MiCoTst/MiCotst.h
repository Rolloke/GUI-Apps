#include "stdio.h"          
#include "memory.h"
#include <time.h>

#ifndef __MICOTST_H__
#define __MICOTST_H__

#pragma warning ( disable : 4135 )

class	CZoran;
class	CBT829;
class	CBT856;

typedef 		unsigned char   BYTE;
typedef 		unsigned short  WORD;
typedef 		unsigned long	DWORD;
typedef 		int			    BOOL;
typedef 		int			    HWND;
typedef 		const char  	*LPCSTR, *PCSTR;
typedef 		int 			HANDLE;

#define TRUE	1
#define FALSE	0
#define FAR		far
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

void __cdecl WK_TRACE(BOOL bLog, LPCSTR lpszFormat, ...);

void  ResetVideoInterface(WORD wIOBase);
void SetVideoFormat(WORD wIOBase, WORD wFormat);
BOOL  TestMiCoBoard(WORD wIOBase);
BOOL  SystemTest(CZoran &Zoran, CBT829 &BT829_0, CBT829 &BT829_1, CBT856 &BT856, WORD wSource, WORD wIOBase);
BOOL  TestingIRQ(WORD wIOBase);
void  InstallIRQ(WORD wIrq);
void  RemoveIRQ(WORD wIrq);
void  WaitUntilKeyStroke();
void  Sleep(int nWait);
DWORD GetTickCount();  

// Dummyklasse da DOS-Version ohne MFC kompiliert wird
class CCriticalSection
{
	public:
		void Lock();
		void Unlock();
};



#endif //