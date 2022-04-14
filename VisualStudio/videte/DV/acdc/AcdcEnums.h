// AcdcEnums.h
// Includes all neccessary enums

//******************************************
//	enum MainFunktion
//	all MainFunctions are main commands from CIPC 
//	this functions start calls of the NeroAPI
//******************************************
	enum MainFunction
	{
		MF_START			= 0,	//start status

		MF_CD_WRITE_ISO		= 1,	//write CD in ISO format
		MF_CD_WRITE_VCD		= 2,	//write CD in VideoCD format
		MF_CD_WRITE_SVCD	= 3,	//write CD in SuperVideoCD format
		MF_EJECT			= 4,	//eject medium
		MF_CD_DRIVES		= 5,	//list all available CD-RW drives
		MF_CD_INFO			= 6,	//get information about loaded medium (CD-R/CD-RW/DVD)
		MF_COMPLETE			= 7,	//API process complete, stop thread
		MF_ABORT			= 8,	//abort current process
		MF_ERASE			= 9,	//erase CD-RW / DVD-RW

		MF_WAIT_IN_USE		= 98,	//burn thread currentl in use, only one thread at present allowed
		MF_ERRORS			= 99,	//an error ocoured

	};
	
	enum MediumInfo
	{
		MI_NODRIVE			= 0,	//drive not found
		MI_NOMEDIUM			= 1,	//no medium found
		MI_CDREADONLY		= 2,	//medium is read only
		MI_CDOK				= 3,	//medium is an empty CD and is writable
		MI_CDRWOK			= 4,	//medium is an empty CD-RW and is writable
		MI_CDRWDELETE		= 5,	//medium is a not empty CD-RW and has to be deleted first
		MI_DVDREADONLY		= 6,	//medium is read only
		MI_DVDOK			= 7,	//medium is an empty DVD and is writable
		MI_DVDRWOK			= 8,	//medium is an empty DVD-RW and is writable
		MI_DVDRWDELETE		= 9,	//medium is a not empty DVD-RW and has to be deleted first
		MI_UNKNOWN_MEDIUM	= 10,	//unknown medium, maybe cd-drive is out of order
	};


	enum BurnProducer
	{
		BP_NOPRODUCER		= 0,	//no burn software installed 
		BP_NERO				= 1,	//burn software: Nero
		BP_XP				= 2,	//burn software is WindowsXP own burn program
		BP_DIRECT_CD		= 3,    //burn software is DirectCD
	};

	enum NewMessageFrom
	{
		NM_SERVER_CONTROL_THREAD	= 1,
		NM_NERO_BURN_THREAD			= 2,
	};

#define WM_BURN_RESULT			WM_USER + 1
#define WM_BURN_CDINFO			WM_USER + 2 
#define WM_BURN_WRITEISO		WM_USER + 3
#define WM_BURN_ERROR			WM_USER + 4
#define WM_BURN_NOTIFY_DRIVE	WM_USER + 5 

#define WM_BURN_NEW_MESSAGE	WM_USER + 101

	