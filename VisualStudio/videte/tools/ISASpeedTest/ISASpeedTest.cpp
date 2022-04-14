/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ISASpeedTest
// FILE:		$Workfile: ISASpeedTest.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ISASpeedTest/ISASpeedTest.cpp $
// CHECKIN:		$Date: 5.08.98 9:54 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 5.08.98 9:54 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "CProfile.h"

#define ZR_CB_DATA_OFFSET		0x00
#define MICO_ID_OFFSET			0x17  

BOOL TestISASpeed(WORD wIOBase);

int main()
{
	WORD 	wIOBaseArray[] = {0x200, 0x280, 0x300, 0x380};
    WORD	wI		= 0;                                                
	BYTE	byID	= 0;
	WORD	wIOBase = 0;

	for (wI = 0; wI <= 3; wI++)
	{
		byID = _inp(wIOBaseArray[wI] | MICO_ID_OFFSET);
		if ((byID == 0x22) || (byID == 0x30))
		{
			wIOBase = wIOBaseArray[wI];
			break;
		}
	}			

	if (wIOBase == 0)
	{
		printf("No Mico detected\n");
	}
	else
	{
		printf("Mico detected at adress 0x%x\n", wIOBase);
		TestISASpeed(wIOBase);
	}

	printf("press a key to continue\n");
	getch();
  
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL TestISASpeed(WORD wIOBase)
{
	char	*lpBuffer	= NULL;
	WORD	wCBData		= 0;
	WORD	wDataLen	= 0;
	DWORD	dwCount		= 0;
	LARGE_INTEGER	DeltaTime = {0};
	
	DWORD	dwBPS		= 0;
	CProfile prof("ISABusSpeed");

	wDataLen = 50000;
	lpBuffer = new char[wDataLen];
	dwCount	 = 1000;

	if (!lpBuffer)
	{
		printf("Allocation memory error\n");
		return FALSE;
	}

	// Bilddaten in den Jpeg-Buffer kopieren
	wCBData = wIOBase | ZR_CB_DATA_OFFSET;
	wDataLen >>= 1;	// 16 Bit Zugriff

	printf("Testing	ISA-Bus speed...\n");
	Sleep(1000);

	// Hier wird gemessen...
	_asm cli
	prof.Start();
	for (DWORD dwI = 0; dwI < dwCount; dwI++)
	{		  
		_asm{
 			  movzx     ecx, wDataLen
			  mov       dx,  wCBData
			  mov       edi, lpBuffer
			  cld			 

			  test      cx,cx
			  jz        LAB_END
			  rep		insw
			LAB_END:
		}
	}
	prof.Stop();														  
	_asm sti
	DeltaTime = prof.GetTimeDiff();

	delete lpBuffer;
	lpBuffer = NULL;

	if (DeltaTime.QuadPart != 0)
	{
		dwBPS = (DWORD)(2000.0 * ((double)dwCount * (double)wDataLen) / (double)(DeltaTime.QuadPart));
		printf("Speed=%lu  KByte/s\n", dwBPS);
	}

	return TRUE;
}


