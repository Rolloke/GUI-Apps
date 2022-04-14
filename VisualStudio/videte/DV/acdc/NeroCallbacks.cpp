/******************************************************************************
|*	This file keeps all Nero callback functions. These functions tells the 
|*	main program ACDC.exe which function the NeroAPI currently is in. 
|*	When NeroAPI executes a function NeroAPI also have the focus. With these
|*	callback functions the mein application ACDC.exe gets the focus back.
|*
|*	The pUserData member of the NERO_CALLBACK structure is supposed to hold a
|*	pointer to the calling object in a C++ environment. 
|*
|*	implemented callback functions:
|*		-	AbortedCallback			-	check the current process is supposed to be terminated
|*		-	IdleCallback			-	will be called continuously during a burn process
|*		-	ProgressCallback		-	how much of the current process has been completed
|*		-	AddLogLine				-	textual information about certain states
|*		-	SetPhaseCallback		-	textual information about the current phase of burning
|*		-	SetMajorPhaseCallback	-	which phase of the burn process NeroAPI is currently in
|*		-	DisableAbortCallback	-	can the burn process can be interrupted or not
|*		-	WriteIOCallback			-	is used when PCM data is being read from CD
|*		-	EOFCallback				-	is used when PCM data is being read from CD and the 
|*										end of file is reached.
|*		-	ErrorCallback			-	is used when PCM data is being read from CD and  
|*						  			    an error is occured.
|*		-	ReadIOCallback			-	will used when PCM data is writen to CD
|*		-	UserDialog				-	let the user make a choice, or tell that he had finished a task
|*
|*	PROGRAM: NeroCallbacks.cpp
|*
******************************************************************************/


#include "stdafx.h"
#include "BurnContext.h"

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : AbortedCallback 
 Description   : This function is used by the NeroAPI to check whether the current process is
				 supposed to be terminated.

 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)

 Result type:  TRUE = terminate function (BOOL)
			  FALSE = do nothing
 Author: TKR
 created: July, 05 2002
 <TITLE AbortedCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/

BOOL NERO_CALLBACK_ATTR CBurnContext::AbortedCallback (void *pUserData)
{
	return ((CBurnContext*)pUserData)->m_bAborted;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : IdleCallback 
 Description   : This function will be called continuously during a burn process. 
				 If the user aborts this process, mbAborted becomes true 
				 and the API will be told to stop.

 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)

 Result type:  TRUE = stop function (BOOL)
			  FALSE = do nothing
 Author: TKR
 created: July, 05 2002
 <TITLE IdleCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
BOOL NERO_CALLBACK_ATTR CBurnContext::IdleCallback (void *pUserData)
{
	static MSG msg;
	while (!((CBurnContext*)pUserData)->m_bAborted && ::PeekMessage(&msg,NULL,NULL,NULL,PM_NOREMOVE))
	{

		if (!AfxGetThread()->PumpMessage())
		{
			break;
		}
	}
	return ((CBurnContext*)pUserData)->m_bAborted;
}


/*********************************************************************************************
 Class		   : CBurnContext
 Function      : ProgressCallback 
 Description   : This function will provide information on how much of the 
				 current process has been completed.
				 We use this information for display in a progress bar.

 Parameters:  
  pUserData:		   (O):  pointer to the calling object in a C++ environment  (void*)
  dwProgressInPercent: (O):	 progress in percent	

 Result type:  TRUE = stop function (BOOL)
			  FALSE = do nothing (DWORD)
 Author: TKR
 created: July, 05 2002
 <TITLE ProgressCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
BOOL NERO_CALLBACK_ATTR CBurnContext::ProgressCallback (void *pUserData, DWORD dwProgressInPercent)
{
	//inform mainapp about current progress in percent
	CBurnContext* pBurnContext = (CBurnContext*)pUserData;
	pBurnContext->SetCurrentProgress((int)dwProgressInPercent);
	return ((CBurnContext*)pUserData)->m_bAborted;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : AddLogLine 
 Description   : This function provides textual information about certain states that might be
				 important for the application.

 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)
  type     : (O):  type of text message (NERO_TEXT_TYPE)
  text     : (O):  value of text message (const char*)	

 Result type:  --
 Author: TKR
 created: July, 05 2002
 <TITLE AddLogLine >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
void NERO_CALLBACK_ATTR CBurnContext::AddLogLine (void *pUserData, NERO_TEXT_TYPE type, const char *text)
{
	char * header;
	const char *start;

	//Type des Eintrags, der vor jede Textausgabe  gestellt wird
	switch (type)
	{
		case NERO_TEXT_INFO:        // informative text
			header = "[i]";
			break;
		case NERO_TEXT_STOP:        // some operation stopped prematurely
			header = "[#]";
			break;
		case NERO_TEXT_EXCLAMATION: // important information 
			header = "[!]";
			break;
		case NERO_TEXT_QUESTION:    // a question which requires an answer
			header = "[?]";
			break;
		case NERO_TEXT_DRIVE:		// a message concerning a CD-ROM drive or recorder
			header = "[-]";
			break;
		default:
			header = "";
	}

	//Step through the message text, considering newline characters
	//and inserting a line break every 76 characters if the line is longer
	start = text;
	while (*start)
	{

		//search for newline NL(LF) and set a pointer to the 
		//next newline character. If no newline is found end becomes NULL.
		char *end = strchr (start, '\n');

		//Determine the length of the string part to be printed.
		//If a newline character was found the length is the difference between end and start
		//Otherwise there is no newline between the current position of start in the string
		//and the end of the string. So the length can be determined by a simple call to strlen.
		int len;
		if (NULL != end)
		{
			len = end - start;
		}
		else
		{
			len = strlen (start);
		}

		//We also make sure that no more than 76 characters are printed
		//no matter how long the current string part really is.
		if (len > 76)
		{
			len = 76;
		}
		
		CString sMessage;
#ifdef _UNICODE
		CWK_String sHeader = header;
		const CWK_String sStart = start;
		//sMessage.Format(_T("%s %s\n"), sHeader, sStart);
		sMessage = sHeader + _T(": ") + sStart;
#else
		sMessage.Format("%s %s\n", header, start);
#endif

		if(sMessage.Left(8) == _T(" Der Pro"))
		{
			TRACE(_T(""));
		}
		AddMessageLine(sMessage);

		//Shift the start pointer right by the amount of bytes just printed.
		start += len;


		//If newline characters were found start has to be set to the next character
		//If end contains NULL this means that either no newlines were found or
		//the end of the string has been reached.
		if (NULL != end)
		{
			++start;
		}
	}
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : SetPhaseCallback 
 Description   : This function provides textual information about the current phase of the burning
				 process.

 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)
  text     : (O):  value of text message (const char*)	

 Result type:  --
 Author: TKR
 created: July, 05 2002
 <TITLE SetPhaseCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
void NERO_CALLBACK_ATTR CBurnContext::SetPhaseCallback (void *pUserData, const char *text)
{
	AddMessageLine(text);
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : SetMajorPhaseCallback 
 Description   : Let the application know which phase of the burn process 
				 NeroAPI is currently in.
 
 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)
  phase    : (O):  name of the current process (NERO_MAJOR_PHASE)
  reserved : (O):  reserved (void*)

 Result type:  --
 Author: TKR
 created: July, 05 2002
 <TITLE SetMajorPhaseCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
void NERO_CALLBACK_ATTR CBurnContext::SetMajorPhaseCallback (void *pUserData, NERO_MAJOR_PHASE phase, void* reserved)
{
	if(phase == NERO_PHASE_START_WRITE)
	{
		//init burn progress with -1, that means start tracing progress of burning 
		((CBurnContext*)pUserData)->SetCurrentProgress((int)-1);
	}

	if(phase == NERO_PHASE_DONE_WRITE)
	{
		WK_TRACE(_T("brennen komplett\n"));
	}
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : DisableAbortCallback 
 Description   : Tells the main program whether the burn process can be interrupted or not.
 
 Parameters:  
  pUserData	 : (O):  pointer to the calling object in a C++ environment  (void*)
  enableAbort: (O):  TRUE = process can be interrupted (BOOL)

 Result type:  --
 Author: TKR
 created: July, 05 2002
 <TITLE DisableAbortCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
void NERO_CALLBACK_ATTR CBurnContext::DisableAbortCallback (void *pUserData, BOOL enableAbort)
{
	if (FALSE == enableAbort)
	{
		WK_TRACE(_T("[i] The current process cannot be interrupted\n"));
	}
	else
	{
		WK_TRACE(_T("[i] The process can be interrupted again\n"));
	}
}


/*********************************************************************************************
 Class		   : CBurnContext
 Function      : WriteIOCallback 
 Description   : These IO callback is necessary to perform operations with NeroAPI
				 that don't deal with files directly. We use them when dealing with 
				 the PCM format. 
				 WriteIOCallback will used when PCM data is being read from CD.
				 PCM = uncompressed .wav file
 
 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)
  pBuffer  : (O):  pointer to buffer which holds data from CD (BYTE*)
  dwLen	   : (O):  length of the data to store (DWORD)

 Result type:  --
 Author: TKR
 created: July, 05 2002
 <TITLE WriteIOCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
DWORD NERO_CALLBACK_ATTR CBurnContext::WriteIOCallback (void *pUserData, BYTE *pBuffer, DWORD dwLen)
{
	WK_TRACE(_T("----- WriteIOCallback \n"));
	return fwrite (pBuffer, 1, dwLen, (FILE *)pUserData);
}


/*********************************************************************************************
 Class		   : CBurnContext
 Function      : EOFCallback 
 Description   : These IO callback is necessary to perform operations with NeroAPI
				 that don't deal with files directly. We use them when dealing with 
				 the PCM format. 
				 EOFCallback will used when PCM data is being read from CD and the 
				 end of file is reached.
				 PCM = uncompressed .wav file
 
 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)

 Result type:  TRUE = end of file is reached (BOOL)
 Author: TKR
 created: July, 05 2002
 <TITLE EOFCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
BOOL NERO_CALLBACK_ATTR CBurnContext::EOFCallback (void *pUserData)
{
	WK_TRACE(_T("----- EOFCallback \n"));
	return feof ((FILE *)pUserData);
}


/*********************************************************************************************
 Class		   : CBurnContext
 Function      : ErrorCallback 
 Description   : These IO callback is necessary to perform operations with NeroAPI
				 that don't deal with files directly. We use them when dealing with 
				 the PCM format. 
				 ErrorCallback will used when PCM data is being read from CD and  
				 an error is occured.
				 PCM = uncompressed .wav file
 
 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)

 Result type:  TRUE = an error is occured (BOOL)
 Author: TKR
 created: July, 05 2002
 <TITLE ErrorCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
BOOL NERO_CALLBACK_ATTR CBurnContext::ErrorCallback (void *pUserData)
{
	WK_TRACE(_T("----- ErrorCallback \n"));
	return ferror ((FILE *)pUserData);
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : ReadIOCallback 
 Description   : These IO callback is necessary to perform operations with NeroAPI
				 that don't deal with files directly. We use them when dealing with 
				 the PCM format. 
				 WriteIOCallback will used when PCM data is writen to CD.
				 PCM = uncompressed .wav file

 Parameters:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)
  pBuffer  : (O):  pointer to buffer which holds data for the CD (BYTE*)
  dwLen	   : (O):  length of the data to store (DWORD)

 Result type:   number of full items actually read (DWORD)
 Author: TKR
 created: July, 05 2002
 <TITLE ReadIOCallback >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
DWORD NERO_CALLBACK_ATTR CBurnContext::ReadIOCallback (void *pUserData, BYTE *pBuffer, DWORD dwLen)
{
	WK_TRACE(_T("----- ReadIOCallback \n"));
	return fread (pBuffer, 1, dwLen, (FILE *)pUserData);
}


/*********************************************************************************************
 Klasse		: CBurnContext
 Funktion	: UserDialog
 Zweck		: The UserDialog() callback function is designed to let the user make a
			  choice, or tell the NeroAPI that the user had finished a task, 
			  which the NeroAPI required him to perform. 
			  Because in ACDC.exe is no need to ask the user some questions, we will decide
			  what to do without asking questions.

 Parameter:  
  pUserData: (O):  pointer to the calling object in a C++ environment  (void*)
  type     : (O):  type of text message (NeroUserDlgInOut)
  data     : (O):  value of text message (void*)	

 Result type:   choice of NeroAPI questions (NeroUserDlgInOut)
 Author: TKR
 created: July, 05 2002
 <TITLE UserDialog >
 <GROUP Nero-Callback>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Callback>
*********************************************************************************************/
NeroUserDlgInOut NERO_CALLBACK_ATTR CBurnContext::UserDialog (void* pUserData, NeroUserDlgInOut type, void *data)
{
	CString sMessage;
	BOOL bAbortProcess = FALSE;
	switch (type)
	{
		case DLG_AUTO_INSERT:
			//determined return value:	DLG_RETURN_CONTINUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Auto Insert Notification is turned on in the system configuration.
			//This may cause serious problems while burning: your CD might be damaged,
			//or the system might hang up.
			//Nero is able to burn CDs with Auto Insert Notification turned on if all
			//necessary drivers are installed.
			//You can do the following:
			//1. DLG_RETURN_INSTALL_DRIVER	- Install IO driver which temporarily disables auto insert. 
			//								  Note: this only works if the additional argument 
			//								  for the callback is not NULL, otherwise it should 
			//								  not be offered to the user.
			//2. DLG_RETURN_OFF_RESTART		- Turn off Auto Insert Notification and restart Windows
			//3. DLG_RETURN_EXIT			- Exit Nero						
			//4. DLG_RETURN_CONTINUE		- Continue at your own risk		
			//------------------
			sMessage = "Auto Insert Notification is turned on in the system configuration.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_CONTINUE;


		case DLG_DISCONNECT_RESTART:
			//determined return value:	DLG_RETURN_ON_RESTART 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Disconnect is turned off in the system configuration.
			//This may cause serious problems while burning: 
			//your might be damaged, or the system might hang up.
			//You can do the following:
			//1. DLG_RETURN_ON_RESTART	- Turn on disconnect and restart windows				
			//2. DLG_RETURN_RESTART		- Don't change disconnect option and restart windows
			//------------------
			sMessage = "Disconnect is turned off in the system configuration.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_ON_RESTART;

			
		case DLG_DISCONNECT:
			//determined return value:	DLG_RETURN_CONTINUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Disconnect is turned off in the system configuration.
			//This may cause serious problems while burning: 
			//your might be damaged, or the system might hang up.
			//You can do the following:
			//1. DLG_RETURN_ON_RESTART	- Turn on disconnect and restart windows				
			//2. DLG_RETURN_RESTART		- Don't change disconnect option and restart windows
			//3. DLG_RETURN_CONTINUE	- Continue at your own risk		
			//------------------
			sMessage = "Continue at your own risk.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_CONTINUE;

			
		case DLG_AUTO_INSERT_RESTART:
			sMessage = "Auto Insert Notification is now OFF. You should restart Windows.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_EXIT;

		case DLG_RESTART:
			sMessage = "Please restart Windows now.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_EXIT;

		case DLG_SETTINGS_RESTART:
			//determined return value:	DLG_RETURN_CONTINUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Nero detected some modifications of your PC system configuration
			//and needs to modify some settings. Please restart your PC to make
			//the changes become effective.
			//You can do the following:
			//1. DLG_RETURN_RESTART		- Don't change disconnect option and restart windows
			//2. DLG_RETURN_CONTINUE	- Continue at your own risk	
			//------------------
			sMessage = "Nero detected some modifications of your PC system configuration.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_CONTINUE;

		case DLG_OVERBURN:
			//determined return value:	DLG_RETURN_TRUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Sorry, this compilation contains too much data to fit on the CD
			//with respect to the normal CD capacity. Do you want to try
			//overburn writing at your own risk (this might cause read
			//errors at the end of the CD or might even damage your recorder)?
			//Note: It is also possible, that SCSI/Atapi errors occur at the end
			//of the simulation or burning. Even in this case there is a certain
			//chance, that the CD is readable.
			//You can do the following:
			//1. DLG_RETURN_TRUE	- Don't change disconnect option and restart windows
			//2. DLG_RETURN_FALSE	- Continue at your own risk	
			//------------------
			sMessage = "This compilation contains too much data.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_TRUE;

		case DLG_COPY_QUALITY_LOSS:
			//determined return value:	DLG_RETURN_TRUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Disc must be written as 'track at once' ('disc at once' not
			//supported or not able to write such a disc). This means
			//there might be some minor quality loss (e.g. lost audio index
			//or different pause between audio tracks). Do you want to
			//proceed anyway?
			//You can do the following:
			//1. DLG_RETURN_TRUE	- Yes
			//2. DLG_RETURN_FALSE	- No	
			//------------------
			sMessage = "Disc must be written as 'track at once'.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_TRUE;

		case DLG_COPY_FULLRISK: 
			//determined return value:	DLG_RETURN_TRUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Disc must be written as 'track at once' ('disc at once' not
			//supported or not able to write such a disc).  Unfortunately
			//your image has a special format which can most likely only
			//be written correctly with 'disc at once'. But you may
			//PROCEED AT YOUR OWN RISK. Do you want to continue burning?
			//You can do the following:
			//1. DLG_RETURN_TRUE	- Yes
			//2. DLG_RETURN_FALSE	- No
			//------------------
			sMessage = "Disc must be written as 'track at once'.\n";
			AddMessageLine(sMessage);
			return DLG_RETURN_TRUE;

		case DLG_AUDIO_PROBLEMS:

			if (((DWORD)data) & AUP_NOTRACK_FOUND)
			{
				sMessage = "No tracks given.\n";
				AddMessageLine(sMessage);
				return DLG_RETURN_EXIT;
			}
			if (((DWORD)data) == AUP_MEGA_FATAL)
			{
				sMessage = "fatal internal problem.\n";
				AddMessageLine(sMessage);
				return DLG_RETURN_EXIT;
			}

			//determined return value:	DLG_RETURN_TRUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Nero has detected that the following audio properties are not
			//supported by your recorder in the current write mode and need
			//to be modified:
			
			if (((DWORD)data) & AUP_FIRST_TR_PAUSE) 
				sMessage = "medium specification allows only 2-3 Seconds pause for the first track.\n";

			if (((DWORD)data) & AUP_PAUSE_SETTINGS) 
				sMessage = "Pause length between the tracks not supported.\n";

			if (((DWORD)data) & AUP_INDEX_SETTINGS)
				sMessage = "Additional index positions not supported.\n";

			if (((DWORD)data) & AUP_ISRC_SETTINGS)
				sMessage = "ISRC codes not supported.\n";

			if (((DWORD)data) & AUP_COPYPROT_SETTINGS)
				sMessage = "Copy protection bit not supported.\n";

			AddMessageLine(sMessage);

			//Nero can change the audio track settings for you to fit the
			//capabilities of your recorder. This might cause some audio
			//properties to be lost. Do you want Nero to change the track
			//settings for you?
			//You can do the following:
			//1. DLG_RETURN_TRUE	- Yes
			//2. DLG_RETURN_FALSE	- No
			//------------------
			sMessage = _T("Nero can change the audio track settings for you.\n");
			AddMessageLine(sMessage);
			return DLG_RETURN_TRUE;

		case DLG_WAITCD:
			{
			//This dialog type differs slightly from the other ones:
			//it should pop up a message and return immediately while still showing
			//the message, so that the API can test for the expected CD in the meantime.
			//During this time, the NERO_IDLE_CALLBACK will be called to give the
			//application a chance to update its display and to test for user abort.
			//The API might call DLG_WAITCD several times to change the text.
			//The text depends on the "data" argument that is passed to the
			//NERO_USER_DIALOG callback. It is the enumeration NERO_WAITCD_TYPE.

			//abort if space on CD/DVD is too small or CD/DVD type is unsopported
			NERO_WAITCD_TYPE NeroWaitCD = (NERO_WAITCD_TYPE) (int)data;
			if(    NeroWaitCD == NERO_WAITCD_NOTENOUGHSPACE
				|| NeroWaitCD == NERO_WAITCD_MEDIUM_UNSUPPORTED)
			{
				bAbortProcess = TRUE;
			}
			
			//NeroGetLocalizedWaitCDTexts() ist erst in NeroCmd 5.5.10.15 verfügbar
			//in der alten NeroCmd Version 5.5.8.2 muß NeroGetWaitCDTexts() verwendet werden

			BOOL bAtLeast = ((CBurnContext*)pUserData)->IsOldNeroCmdVersion();

			if(bAtLeast)
			{
				sMessage = NeroGetLocalizedWaitCDTexts ((NERO_WAITCD_TYPE) (int)data);

			}
			else
			{
				sMessage = NeroGetWaitCDTexts ((NERO_WAITCD_TYPE) (int)data);
			}

			AddMessageLine(sMessage, (int)data);
			}
			break;

		case DLG_WAITCD_REMINDER:
			//It is time to remind the user of inserting the CD: play a jingle, flash the screen, etc.
			//Called only once after a certain amount of time of no CD being inserted.


			if(!((CBurnContext*)pUserData)->GetCDRWEraseSuccess())
			{
				bAbortProcess = TRUE;
			}
			sMessage = _T("No empty medium was inserted, abort process.\n");
			AddMessageLine(sMessage);
			break;

		case DLG_WAITCD_DONE:
			// nothing to be done in the text interface,
			// should close dialog box in a GUI
			if(!((CBurnContext*)pUserData)->GetAbortFlag())
			{
				sMessage = _T("medium found in drive.\n");
			}
			else
			{
				sMessage = _T("process aborted.\n");
				//reset abort flag
//				TRACE(_T(_T("UserDialog: DLG_WAITCD_DONE reset abort flag\n")));
//				((CBurnContext*)pUserData)->SetAbortFlag(FALSE);
			}

			AddMessageLine(sMessage);
			break;

		case DLG_FILESEL_IMAGE:
			//determined return value:	DLG_RETURN_TRUE 
			//all possible choices are listed below as a comment
			
			//Nero Information:
			//-----------------
			//Ask the user the path of the file which will be generated by the Image Recorder.
			//The "data" argument points on a 256 bytes buffer that has to be filled with the image path
			//You can do the following:
			//1. DLG_RETURN_TRUE	- save image
			//2. DLG_RETURN_EXIT	- stop the burn process
			//-----------------

			//we don`t need to save an image file
			sMessage = _T("Enter file name to save image to:\n");
			AddMessageLine(sMessage);
			return DLG_RETURN_TRUE;

		case DLG_BURNIMAGE_CANCEL:
			//Tell that there is not enough space on disk to produce this image
			sMessage = _T("There is not enough space available to burn the image.\n");
			bAbortProcess = TRUE;
			AddMessageLine(sMessage);
			break;

		case DLG_NON_EMPTY_CDRW:
		{
			//Nero Information:
			//-----------------
			//Tell the user that the CDRW is not empty
			//Starting from NeroAPI 5.5.3.0, the "data" argument contains the device handle from the recorder
			//Will be called only if the NBF_DETECT_EMPTY_CDRW flags is given to the NeroBurn function
			//You can do the following:
			//1. DLG_RETURN_EXIT		- stop the burn process
			//2. DLG_RETURN_CONTINUE	- continue the burn process
			//3. DLG_RETURN_RESTART		- ask the user for an other CD
			//-----------------

			// Do not ask the user if the --force_erase_cdrw
			// command line parameter has been provided
			// pUserData holds a pointer to CBurnContext which we set 
			// when initializing NeroAPI.
			// We need this pointer to obtain settings and handles
			// from the CBurnContext class.


			//TKR - normaly we don`t need to check if the parameter --force_erase_cdrw was given
			//because we will erase a not empty CD-RW each time.
			//We ask the user after calling --CDInfo if a not empty CD-RW should be erased or not
			//We keep this original code only because of the compatibility for later uses
			NeroUserDlgInOut res;
			if (true == ((CBurnContext*)pUserData)->m_params->GetForceEraseCDRW())
			{
				sMessage = _T("This RW-medium is not empty. Erasing RW-medium.\n");
				res = DLG_RETURN_CONTINUE;
			}
			else
			{
				sMessage = _T("Cannot erase RW-medium, flag is missing.\n");
				res = DLG_RETURN_EXIT;
			}

			AddMessageLine(sMessage);

			if (DLG_RETURN_CONTINUE == res)
			{
				// Erase the CDRW
				NEROAPI_CDRW_ERASE_MODE mode = NEROAPI_ERASE_QUICK;

				int time = NeroGetCDRWErasingTime(((CBurnContext*)pUserData)->m_NeroDeviceHandle,mode);

				if (time == -1)
				{
					sMessage = _T("No medium inserted.\n");
					AddMessageLine(sMessage);
					bAbortProcess = TRUE;
				}
				else if (time == -2)
				{
					sMessage = _T("This recorder does not support RW-mediums.\n");
					AddMessageLine(sMessage);
					bAbortProcess = TRUE;
				}

				sMessage.Format(_T("Erasing CDRW. This will take approx. %d seconds.\n"),time);
				((CBurnContext*)pUserData)->SetCDRWErasingTime(time);

				AddMessageLine(sMessage);

				TRACE(_T("*** Erase started \n"));

				int err = NeroEraseCDRW(((CBurnContext*)pUserData)->m_NeroDeviceHandle,mode);
				if (err)
				{
					//TODO TKR error hochmelden
					sMessage = _T("Error erasing the RW-medium.\n");
					AddMessageLine(sMessage);
					bAbortProcess = TRUE;
					res = DLG_RETURN_EXIT; //erzwingt abbruch des backups
				}
				else
				{
					((CBurnContext*)pUserData)->SetCDRWEraseSuccess(TRUE);
					bAbortProcess = ((CBurnContext*)pUserData)->GetCDRWAbortFlag();
					TRACE(_T("*** Erase stopped \n"));
				}
			}
			else
			{
				bAbortProcess = TRUE;
			}
		
			return res;
		}
	}

	if(bAbortProcess)
	{
		//abort function
		((CBurnContext*)pUserData)->SetAbortFlag();
	}

	return DLG_RETURN_EXIT;
}


