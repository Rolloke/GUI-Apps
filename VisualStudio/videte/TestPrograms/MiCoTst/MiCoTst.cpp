#include <stdarg.h>
#include <string.h>
#include <direct.h> 
#include <dos.h>
#include <stdlib.h>
#include "micotst.h"
#include "micoreg.h"
#include "CBT829.h" 
#include "CBT856.h"
#include "CRelay.h"
#include "CAlarm.h"
#include "micodefs.h"
#include "cwdog.h"
#include "CZoran.h"

// QUICK_TEST = TRUE -> Die folgenden Tests werden nicht durchgeführt:
//		- Zoran.TestingIRQ
//		- Zoran.Recording
//		- Zoran.Playing
//		- Es wird nur der 1. Videoeingang überprüft.
//		- Keine Abfrage der Seriennummer,
//		- Keine differenzierte Ausgabe während des Tests
//		- Keine Protokollierung während des Tests
//		- Die Karte wird an allen 4 Basisadressen gesucht.

// Änderung von Version 1.2a auf Version 1.2b (22.04.99)
// Der Interrupt 12 wird nicht mehr getestet, da die MiCoISA-Light anscheinent Probleme damit hat.
 
//#define QUICK_TEST


CIo	g_IO;                  
                                     
void __cdecl WK_TRACE(BOOL bLog, LPCSTR lpszFormat, ...);
char sSN[128] = "0";

int main()         
{   
	WORD	wIOBase[] = {0x200, 0x280, 0x300, 0x380};
    WORD	wI;                                                
	BYTE	byID;
	BOOL	bDetect = FALSE;

#ifndef QUICK_TEST	
	_mkdir("tmp");
	_mkdir("log");
#endif
	
	for (wI = 0; wI <= 3; wI++)
	{
		byID = g_IO.Input(wIOBase[wI] | MICO_ID_OFFSET);
		if ((byID == 0x22) || (byID == 0x30))
		{
			bDetect = TRUE;
#ifdef QUICK_TEST
			TestMiCoBoard(wIOBase[wI]);
#else
			if (wIOBase[wI] != 0x380)
				TestMiCoBoard(wIOBase[wI]);
			else
				WK_TRACE(TRUE, "WARNING MiCo-Board detected at adress:0x%x\tuse another adress\n", wIOBase[wI]);
#endif
		}
	}			
	
#ifdef QUICK_TEST
	if (!bDetect)
		printf("No MiCo-Board detected\n");
#else
	if (!bDetect)
		WK_TRACE(TRUE, "No MiCo-Board detected\n");
#endif    

	return 0;
} 

//////////////////////////////////////////////////////////////////////////////////////
BOOL TestMiCoBoard(WORD wIOBase)
{
	BOOL	bError	= FALSE;
    time_t 	aclock;
    struct 	tm *newtime;

	time( &aclock );               
	newtime = localtime( &aclock );

#ifdef QUICK_TEST
	printf("MiCo Testsoftware Version 1.2b\t\t\t%s", asctime(newtime));
#else
	WK_TRACE(TRUE, "MiCo Testsoftware Version 1.2b\t\t\t%s", asctime(newtime));
    WK_TRACE(TRUE, "Please enter the SN:");
    scanf("%s", sSN);
 	
	WK_TRACE(TRUE, "MiCo-Board (SN=%s) detected at adress:0x%x\n\n", sSN, wIOBase);		
#endif 
 	ResetVideoInterface(wIOBase);

#ifdef QUICK_TEST
	printf("Testing MiCo-Board. Please wait a few minutes...\n");
#endif
	// Alle Testobjekte anlegen
	WK_TRACE(FALSE, "Testing PCD8584...\n");

	CBT829 		BT829_0(wIOBase, MICO_FE0);
	CBT829 		BT829_1(wIOBase, MICO_FE1);
	CBT856 		BT856(wIOBase);
	CRelay 		rel(wIOBase, MICO_EXTCARD0, FALSE);
	CAlarm		alarm(wIOBase, MICO_EXTCARD0);
	CWatchDog 	WatchDog(wIOBase);
	CZoran		Zoran(wIOBase);
#if (0)
	BT856.Init();
	BT856.SetBeVideoFormat(MICO_PAL_CCIR);
	while (TRUE){
		BT856.CheckIt();
	};
#endif

	//#################### First BT829 Test #######################
	WK_TRACE(FALSE, "\nTesting BT829 (1)...\n");
	if (!BT829_0.CheckIt())
		bError = TRUE;

	//#################### Second BT829 Test #######################
	WK_TRACE(FALSE, "\nTesting BT829 (2)...\n");
	if (!BT829_1.CheckIt())
		bError = TRUE;

	//#################### BT856 Test #######################
	WK_TRACE(FALSE, "\nTesting BT856...\n");
	if (!BT856.CheckIt())
		bError = TRUE;

	//#################### Alarm Test #######################
	WK_TRACE(FALSE, "\nTesting Alarmhandling...\n");
	if (!alarm.CheckIt())
		bError = TRUE;

	//#################### ZR36055 Test #######################
	WK_TRACE(FALSE, "\nTesting TestingZR36055...\n");
	if (!Zoran.TestingZR36055())
		bError = TRUE;
	
	//#################### Internal memory Test #######################
//	WK_TRACE(FALSE, "\nTesting Internal memory...\n");
//	if (!Zoran.TestingInternalMemory())
//		bError = TRUE;

	//#################### Codebuffer Test #######################
	WK_TRACE(FALSE, "\nTesting Codebuffer...\n");
	if (!Zoran.TestingCodebuffer())
		bError = TRUE;

	//#################### PD71054 Test #######################
	WK_TRACE(FALSE, "\nTesting PD71054...\n");
	WatchDog.Init(10000);
	if (!WatchDog.CheckIt())
		bError = TRUE;
	
	// MiCo auf CCIR stellen
	SetVideoFormat(wIOBase, MICO_PAL_CCIR);
	
	//#################### Init first frontend #######################
	BT829_0.Init();
	BT829_0.SetFeVideoFormat(MICO_PAL_CCIR);
    BT829_0.SetFeSourceType(MICO_FBAS, MICO_NOTCH_FILTER_AUTO);
	BT829_0.SetFeBrightness((MICO_MAX_BRIGHTNESS	- MICO_MIN_BRIGHTNESS)	/ 2);
	BT829_0.SetFeContrast((MICO_MAX_CONTRAST		- MICO_MIN_CONTRAST)	/ 2);
	BT829_0.SetFeSaturation((MICO_MAX_SATURATION	- MICO_MIN_SATURATION)	/ 2);
	BT829_0.SetFeHue((MICO_MAX_HUE					- MICO_MIN_HUE)			/ 2);

	//#################### Init second frontend #######################
	BT829_1.Init();
	BT829_1.SetFeVideoFormat(MICO_PAL_CCIR);
    BT829_1.SetFeSourceType(MICO_FBAS, MICO_NOTCH_FILTER_AUTO);
	BT829_1.SetFeBrightness((MICO_MAX_BRIGHTNESS	- MICO_MIN_BRIGHTNESS)	/ 2);
	BT829_1.SetFeContrast((MICO_MAX_CONTRAST		- MICO_MIN_CONTRAST)	/ 2);
	BT829_1.SetFeSaturation((MICO_MAX_SATURATION	- MICO_MIN_SATURATION)	/ 2);
	BT829_1.SetFeHue((MICO_MAX_HUE					- MICO_MIN_HUE)			/ 2);
	
	//#################### Init backend #######################
	BT856.Init();
	BT856.SetBeVideoFormat(MICO_PAL_CCIR);
      
	//#################### Init Zoran #######################
	Zoran.Open();
	Zoran.SetFormat(MICO_ENCODER_HIGH);
	Zoran.SetBPF(20000L);
	Zoran.SetFeVideoFormat(MICO_PAL_CCIR);
	Zoran.SetBeVideoFormat(MICO_PAL_CCIR);
    
#ifdef QUICK_TEST
	if (!SystemTest(Zoran, BT829_0, BT829_1, BT856, MICO_SOURCE0, wIOBase))
		bError = TRUE;
#else
	//#################### Interrupt Test #######################
	WORD 	wIRQArray[] = {5, 10, 11};	
	if (!Zoran.TestingIRQ(wIRQArray, 3))
		bError = TRUE;

  	for (WORD wSource = MICO_SOURCE0; wSource <= MICO_SOURCE7; wSource++)
    {
    	if (!SystemTest(Zoran, BT829_0, BT829_1, BT856, wSource, wIOBase))
    		bError = TRUE;
    }   
#endif

	//#################### Relay Test #######################
	WK_TRACE(TRUE, "\nTesting Relays...\n");
	rel.CheckIt(0);
	Sleep(500);
	rel.CheckIt(1);
	Sleep(500);
	rel.CheckIt(2);
	Sleep(500);
	rel.CheckIt(3);

#ifdef QUICK_TEST
	printf("\n-----------------------------\n");			
	if (bError)
		printf("\nOne or more Errors detected in MiCo\n");
	else
		printf("\nNo Errors detected in MiCo\n");

	printf("\npress a key to continue\n");			
#else
	WK_TRACE(TRUE, "\n-----------------------------\n");			
	if (bError)
		WK_TRACE(TRUE, "\nOne or more Errors detected in MiCo (SN=%s)\n", sSN);
	else
		WK_TRACE(TRUE, "\nNo Errors detected in MiCo (SN=%s)\n", sSN);

	WK_TRACE(FALSE, "\npress a key to continue\n");			
#endif
	WaitUntilKeyStroke();

	return bError;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL SystemTest(CZoran &Zoran, CBT829 &BT829_0, CBT829 &BT829_1, CBT856 &BT856, WORD wSource, WORD wIOBase)
{
	BOOL bRet = TRUE;
	
	//#################### Videosignal detection  (first Frontend) #######################
	Zoran.SetInputSource(MICO_FE0, MICO_EXTCARD0, 0, wSource, wSource, MICO_ENCODER_HIGH, 20000L);
	WK_TRACE(TRUE, "\n\nSwitch to Videosource %u\n", wSource+1);
	Sleep(500);
	
	WK_TRACE(FALSE, "\nSearching Videosignal at BT829 (1)...\n");
	if (!BT829_0.IsVideoPresent())
	{
		WK_TRACE(TRUE, "\tVideosignal\tnot detected from BT829 (1)\n");
		bRet = FALSE;
	}
	else
	{
		WK_TRACE(TRUE, "\tVideosignal\tdetected from BT829 (1)\n");
		 
		//#################### Video IRQ Test #######################
		WK_TRACE(FALSE, "\nPolling Video Interrupts...\n");
		if (!Zoran.PollingVideoIRQ())
			bRet = FALSE;

		// Encoding/Decoding nur am 1. Videoeingang testen
		if (wSource == MICO_SOURCE0)
		{	
			// Der 1. Encodertest erfolgt mit PAL_CCIR
			SetVideoFormat(wIOBase, MICO_PAL_CCIR);
			BT829_0.SetFeVideoFormat(MICO_PAL_CCIR);
			BT829_1.SetFeVideoFormat(MICO_PAL_CCIR);
			BT856.SetBeVideoFormat(MICO_PAL_CCIR);
			Zoran.SetFeVideoFormat(MICO_PAL_CCIR);
			Zoran.SetBeVideoFormat(MICO_PAL_CCIR);

            // Testet den Encoder erstmal ohne Daten zu speichern.
			WK_TRACE(FALSE, "\nTesting Encoding...\n");
			if (!Zoran.TestingEncoder())
				bRet = FALSE;
			else
			{
#ifndef QUICK_TEST	
				WK_TRACE(FALSE, "\nTo record some frames from Source %u and Frondend 1 press any key\n", wSource+1);
				WaitUntilKeyStroke();

				// Der 2. Encodertest erfolgt mit PAL_SQUARE
				SetVideoFormat(wIOBase, MICO_PAL_SQUARE);
				BT829_0.SetFeVideoFormat(MICO_PAL_SQUARE);
				BT829_1.SetFeVideoFormat(MICO_PAL_SQUARE);
				BT856.SetBeVideoFormat(MICO_PAL_SQUARE);
				Zoran.SetFeVideoFormat(MICO_PAL_SQUARE);
				Zoran.SetBeVideoFormat(MICO_PAL_SQUARE);
				//#################### Zoran IRQ Test #######################
				WK_TRACE(FALSE, "\nStoring Data...\n");
				if (!Zoran.Recording())
					bRet = FALSE;
				else
				{
				//#################### Zoran Decoding test #######################
					WK_TRACE(FALSE, "\nPlaying Data...\n");
					if (!Zoran.Playing())
						bRet = FALSE;
				}
#endif									
			}
		}
	}

	// Das 2. Frontend nur am 1. Videoeingang testen
	if (wSource == MICO_SOURCE0)
	{	
		//#################### Videosignal detection  (second Frontend) #######################
		WK_TRACE(FALSE, "\nSearching Videosignal at BT829 (2)...\n");
		Zoran.SetInputSource(MICO_FE1, MICO_EXTCARD0, wSource, wSource, wSource, MICO_ENCODER_HIGH, 20000L);
		Sleep(500);
		if (!BT829_1.IsVideoPresent())
		{
			WK_TRACE(TRUE, "\tVideosignal\tnot detected from BT829 (2)\n");
			bRet = FALSE;
		}
		else
		{
			WK_TRACE(TRUE, "\tVideosignal\tdetected from BT829 (2)\n");
	
			//#################### Video IRQ Test #######################
			WK_TRACE(FALSE, "\nPolling Video Interrupts...\n");  
			if (!Zoran.PollingVideoIRQ())
				bRet = FALSE;

			// Der 1. Encodertest erfolgt mit PAL_CCIR
			SetVideoFormat(wIOBase, MICO_PAL_CCIR);
			BT829_0.SetFeVideoFormat(MICO_PAL_CCIR);
			BT829_1.SetFeVideoFormat(MICO_PAL_CCIR);
			BT856.SetBeVideoFormat(MICO_PAL_CCIR);
			Zoran.SetFeVideoFormat(MICO_PAL_CCIR);
			Zoran.SetBeVideoFormat(MICO_PAL_CCIR);

            // Testet den Encoder erstmal ohne Daten zu speichern.
			WK_TRACE(FALSE, "\nTesting Encoding...\n");
			if (!Zoran.TestingEncoder())
				bRet = FALSE;
			else
			{
#ifndef QUICK_TEST
				WK_TRACE(FALSE, "\nTo record some frames from Source %u and Frondend 2 press any key\n", wSource+1);
				WaitUntilKeyStroke();
		
				// Der 2. Encodertest erfolgt mit PAL_SQUARE
				SetVideoFormat(wIOBase, MICO_PAL_SQUARE);
				BT829_0.SetFeVideoFormat(MICO_PAL_SQUARE);
				BT829_1.SetFeVideoFormat(MICO_PAL_SQUARE);
				BT856.SetBeVideoFormat(MICO_PAL_SQUARE);
				Zoran.SetFeVideoFormat(MICO_PAL_SQUARE);
				Zoran.SetBeVideoFormat(MICO_PAL_SQUARE);

				//#################### Zoran Encoding test #######################
				WK_TRACE(FALSE, "\nTesting Encoding...\n");
				if (!Zoran.Recording())
					bRet = FALSE;
				else
				{
				//#################### Zoran Decoding test #######################
					WK_TRACE(FALSE, "\nTesting Decoding...\n");
					if (!Zoran.Playing())
						bRet = FALSE;
				}
#endif // QUICK_TEST
			}
		}
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////
void ResetVideoInterface(WORD wIOBase)
{
	BYTE byReset;

	// I2C-Bus Controller reseten...
	byReset = g_IO.ReadFromLatch(wIOBase | RESET_OFFSET);
	byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_VIF_BIT);
	byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_ZR);

	g_IO.WriteToLatch(wIOBase | RESET_OFFSET, byReset);

	Sleep(10);

	byReset = (BYTE)SETBIT(byReset, PARAM_RESET_VIF_BIT);
	byReset = (BYTE)SETBIT(byReset, PARAM_RESET_ZR);

	g_IO.WriteToLatch(wIOBase | RESET_OFFSET, byReset);

	Sleep(10);
}

//////////////////////////////////////////////////////////////////////////////////////
void SetVideoFormat(WORD wIOBase, WORD wVideoFormat)
{
	// MiCo auf SQUARE/CCIR stellen
	BYTE byParam = g_IO.ReadFromLatch(wIOBase | PARAM_OFFSET);
	switch (wVideoFormat)
	{
		case MICO_PAL_SQUARE:
		case MICO_NTSC_SQUARE:
			byParam = SETBIT(byParam, PARAM_SQUARE_BIT);
			byParam = SETBIT(byParam, PARAM_SQUARE2_BIT);
			break;					  
		case MICO_PAL_CCIR:
		case MICO_NTSC_CCIR:
			byParam = CLRBIT(byParam, PARAM_SQUARE_BIT);
			byParam = CLRBIT(byParam, PARAM_SQUARE2_BIT); 			
			break;
	}
	g_IO.WriteToLatch(wIOBase | PARAM_OFFSET, byParam);
}

//////////////////////////////////////////////////////////////////////////////////////
void Sleep(int wait )
{
	DWORD dwStart = GetTickCount();
	
	while(GetTickCount() < dwStart + wait){}
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD GetTickCount()
{
	return (DWORD)((double)clock() / CLOCKS_PER_SEC * 1000);
}    

//////////////////////////////////////////////////////////////////////////////////////
void CCriticalSection::Lock()
{
	// Dummy
}

//////////////////////////////////////////////////////////////////////////////////////
void CCriticalSection::Unlock()
{
	// Dummy
}                         

//////////////////////////////////////////////////////////////////////////////////////
void __cdecl WK_TRACE(BOOL bLog, LPCSTR lpszFormat, ...)
{
	static char szString[512];
	char	*pS = NULL;
	static char sBuffer[512];
   	LPCSTR lpszLocalFormat;
	FILE *stream;

#ifdef QUICK_TEST	
	printf(".");
#else
	memset(sBuffer,0, sizeof(sBuffer));

	lpszLocalFormat = lpszFormat;

	static va_list args;
	va_start(args, lpszFormat);

	_vsnprintf(szString, sizeof(szString)-1, lpszLocalFormat, args);

	printf(szString); 

	// In einem Logfile protokollieren?
	if (bLog)
	{                         
	    int nLen = strlen(sSN);
	    
	    strcpy(sBuffer, "log\\");
	    
		if (nLen > 8)
			strcat(sBuffer, &sSN[nLen - 8]);
	    else
			strcat(sBuffer, sSN);
	    
	    strcat(sBuffer, ".log");
	
		stream = fopen(sBuffer, "a" );
		fprintf( stream, szString);
		fclose( stream );
	}
#endif // TRACE_IT
}  

//////////////////////////////////////////////////////////////////////////////////////
void WaitUntilKeyStroke()
{
	while (kbhit()){
		getch();
	};
	getch(); 
}

