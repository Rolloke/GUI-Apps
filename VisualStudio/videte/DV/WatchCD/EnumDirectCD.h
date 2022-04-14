//EnumDirectCD.h


//******************************************
//enums für WatchCD.exe
//******************************************

	enum DirectCDStatus
	{
		//Stati für CWatchCDDlg
		WCD_START						= 1,
		WCD_WAIT_FOR_CDR				= 2,
		WCD_FORMAT						= 3,
		WCD_BACKUP						= 4,
		WCD_FINALIZE					= 5,
		WCD_IS_DRIVE_LOCKED				= 6,
		WCD_WAIT_FOR_EJECT				= 7,
		WCD_WAIT_FOR_EJECT_FINISHED		= 8,
		WCD_EJECT_ERROR					= 9,
		WCD_WAIT_FOR_FILE_SYSTEM		= 10,
		WCD_FORCE_TO_EJECT				= 11,
		WCD_QUIT						= 12,
		WCD_ERROR						= 99,



		//Stati für CDirectCDStart
		START_EXEC_DIRECTCD				= 101,
		START_FIND_WELCOME_DLG			= 102,
		START_FIND_DRIVEINFO_DLG		= 103,
		START_WAIT_FOR_EMPTY_CDR		= 104,
		START_WAIT_FOR_CDR_RELEASE		= 105,
		START_FOUND_EMPTY_CDR			= 106,
		START_OPEN_FORMAT_DLG			= 107,
		START_IS_STARTED				= 108,
		START_IS_EMPTY_CDR				= 109,
		START_CLOSE_CDREADY_DLG_VERSION5= 110,
		START_ERROR						= 199,


		//Stati für CDirectCDFormat
		FORMAT_FIND_DLG_FORMAT_DISC		= 201,
		FORMAT_FIND_DLG_NAME_YOUR_DISC	= 202,
		FORMAT_SET_NEW_CDR_NAME			= 203,
		FORMAT_FIND_DLG_FORMATTING_DISC	= 204,
		FORMAT_WAIT_FOR_SUCCESS			= 205,
		FORMAT_IS_FORMATTED				= 206,
		FORMAT_ERROR					= 299,


		//Stati für CDirectCDBackup
		BACKUP_START_BACKUP				= 301,
		BACKUP_WAIT_FOR_BACKUP_COMPLETE	= 302,
		BACKUP_COMPLETE					= 303,
		BACKUP_ERROR					= 399,


		//Stati für CDirectCDFinalize
		FINALIZE_EXEC_DIRECTCD			= 401,
		FINALIZE_IS_CDR_READABLE		= 402,
		FINALIZE_FIND_FINALIZE_DLG		= 403,
		FINALIZE_SET_PROTECT_DISC		= 404,
		FINALIZE_CLICK_FINISH			= 405,
		FINALIZE_FIND_COLLECTINFO_DLG	= 406,
		FINALIZE_FIND_EJECTING_DLG		= 407,
		FINALIZE_WAIT_FOR_SUCCESS		= 408,
		FINALIZE_IS_FINALIZED			= 409,
		FINALIZE_ERROR					= 499,

	};


	enum DCD_Version
	{
		DCD_VERSION_UNKNOWN = 1000,
		DCD_VERSION_26		= 1001,
		DCD_VERSION_30		= 1002,
		DCD_VERSION_50		= 1003,
	};


	enum DirectCDErrorStatus
	{
		ERROR_NO_ERROR					= 0,

		//***************************
		//Errors from CDirectCDStart
		//***************************

		//CreateProcess konnte DirectCD nicht starten
		ERROR_DCD_NOT_STARTED						= 1,

		//DirectCD läuft nicht im Hintergrund als "Watchdog"
		ERROR_DCD_WATCHDOG_NOT_RUNNING				= 2,

		//DirectCD`s Welcome-Dialog nach Timeout nicht auf Desktop
		ERROR_TIMEOUT_WAIT_FOR_WELCOME_DLG			= 3,

		//DirectCD`s DriveInfo-Dialog nach Timeout nicht auf Desktop
		ERROR_TIMEOUT_WAIT_FOR_DRIVEINFO_DLG		= 4,

		//Es wurde keine leere CDR eingelegt, Timeout erreicht
		ERROR_TIMEOUT_WAIT_FOR_EMPTY_CDR			= 5,

		//DVClient ist nicht mehr aktiv
		ERROR_DVCLIENT_NOT_RUNNING					= 6,

		//Es wurde eine schon beschriebene oder fehlerhafte CDR eingelegt
		ERROR_FOUND_NOT_EMPTY_CDR					= 7,


		//***************************
		//Errors from CDirectCDFormat
		//***************************

		//DirectCD`s Formatier-Dialog nach Timeout nicht auf Desktop
		ERROR_TIMEOUT_WAIT_FOR_FORMAT_DISC_DLG		= 101,

		//DirectCD`s NameYourDisc-Dialog nach Timeout nicht auf Desktop
		ERROR_TIMEOUT_WAIT_FOR_NAME_YOUR_DISC_DLG	= 102,

		//DirectCD`s Formatting-Dialog nach Timeout nicht auf Desktop
		ERROR_TIMEOUT_WAIT_FOR_FORMATTING_DLG		= 103,

		//DirectCD`s FormatSuccess-Dialog nach Timeout nicht auf Desktop
		ERROR_TIMEOUT_WAIT_FOR_FORMAT_SUCCESS_DLG	= 104,

		//Nach Formatiervorgang wurden nicht alle Format-Dialoge 
		//von DirectCD nach Timeout geschlossen
		ERROR_TIMEOUT_WAIT_FOR_FORMAT_IS_FORMATTED	= 105,


		//***************************
		//Errors from CDirectCDBackup
		//***************************

		//Nach Anfrage an DVClient, ob noch Daten auf CDR kopiert werden,
		//kam 60 sek. lang keine positive Antwort
		ERROR_DVCLIENT_ABNORMAL_STOP_COPYING_DATA	= 201,


		//*****************************
		//Errors from CDirectCDFinalize
		//*****************************

		//DirectCD`s Finalize-Dialog nach Timeout nicht auf Desktop
		ERROR_TIMEOUT_WAIT_FOR_FINALIZE_DLG			= 301,
		ERROR_TIMEOUT_WAIT_FOR_COLLECT_INFO_DLG		= 302,
		ERROR_TIMEOUT_WAIT_FOR_FINALIZING_DLG		= 303,
		ERROR_TIMEOUT_SET_PROTECT_DISC				= 304,
		ERROR_FINISH_BUTTON_NOT_FOUND				= 305,
		ERROR_CDR_NOT_READY_FOR_FINALIZE			= 306,
		ERROR_TIMEOUT_FINALZE_PROCESS_NOT_STARTED	= 307,
		ERROR_TIMEOUT_FINALIZING					= 308,


		//*****************************
		//Errors from CWatchCDDlg
		//*****************************

		ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD		= 401,
	};


//***********************************************************
//Name of enums für WatchCD.exe
//***********************************************************
	inline CString NameOfEnumDirectCD(DirectCDStatus DCDStatus)
{
	switch (DCDStatus)
	{
	case WCD_START:				return "WCD_START";
	case WCD_WAIT_FOR_CDR:		return "WCD_WAIT_FOR_CDR";
	case WCD_FORMAT:			return "WCD_FORMAT";
	case WCD_BACKUP:			return "WCD_BACKUP";
	case WCD_FINALIZE:			return "WCD_FINALIZE";
	case WCD_ERROR:				return "WCD_ERROR";
	case WCD_IS_DRIVE_LOCKED:	return "WCD_IS_DRIVE_LOCKED";
	case WCD_WAIT_FOR_EJECT:	return "WCD_WAIT_FOR_EJECT";
	case WCD_WAIT_FOR_EJECT_FINISHED: return "WCD_WAIT_FOR_EJECT_FINISHED";
	case WCD_EJECT_ERROR:		return "WCD_EJECT_ERROR";
	case WCD_WAIT_FOR_FILE_SYSTEM: return "WCD_WAIT_FOR_FILE_SYSTEM";
	case WCD_FORCE_TO_EJECT:	return "WCD_FORCE_TO_EJECT";
	case WCD_QUIT:				return "WCD_QUIT";

		//Stati für CDirectCDStart
	case START_EXEC_DIRECTCD:		return "START_EXEC_DIRECTCD";
	case START_FIND_WELCOME_DLG:	return "START_FIND_WELCOME_DLG";
	case START_FIND_DRIVEINFO_DLG:	return "START_FIND_DRIVEINFO_DLG";
	case START_WAIT_FOR_EMPTY_CDR:	return "START_WAIT_FOR_EMPTY_CDR";
	case START_WAIT_FOR_CDR_RELEASE:return "START_WAIT_FOR_CDR_RELEASE";
	case START_FOUND_EMPTY_CDR:		return "START_FOUND_EMPTY_CDR";
	case START_OPEN_FORMAT_DLG:		return "START_OPEN_FORMAT_DLG";
	case START_IS_STARTED:			return "START_IS_STARTED"; 
	case START_IS_EMPTY_CDR:		return "START_IS_EMPTY_CDR";
	case START_CLOSE_CDREADY_DLG_VERSION5: return "START_CLOSE_CDREADY_DLG_VERSION5";
	case START_ERROR:				return "START_ERROR";

		//Stati für CDirectCDFormat
	case FORMAT_FIND_DLG_FORMAT_DISC:		return "FORMAT_FIND_DLG_FORMAT_DISC";
	case FORMAT_FIND_DLG_NAME_YOUR_DISC:	return "FORMAT_FIND_DLG_NAME_YOUR_DISC";
	case FORMAT_SET_NEW_CDR_NAME:			return "FORMAT_SET_NEW_CDR_NAME";
	case FORMAT_FIND_DLG_FORMATTING_DISC:	return "FORMAT_FIND_DLG_FORMATTING_DISC";
	case FORMAT_WAIT_FOR_SUCCESS:			return "FORMAT_WAIT_FOR_SUCCESS";
	case FORMAT_IS_FORMATTED:				return "FORMAT_IS_FORMATTED";
	case FORMAT_ERROR:						return "FORMAT_ERROR";

		//Stati für CDirectCDBackup
	case BACKUP_START_BACKUP:				return "BACKUP_START_BACKUP";
	case BACKUP_WAIT_FOR_BACKUP_COMPLETE:	return "BACKUP_WAIT_FOR_BACKUP_COMPLETE";
	case BACKUP_COMPLETE:					return "BACKUP_COMPLETE";
	case BACKUP_ERROR:						return "BACKUP_ERROR";


		//Stati für CDirectCDFinalize
	case FINALIZE_EXEC_DIRECTCD:		return "FINALIZE_EXEC_DIRECTCD";
	case FINALIZE_IS_CDR_READABLE:		return "FINALIZE_IS_CDR_READABLE";
	case FINALIZE_FIND_FINALIZE_DLG:	return "FINALIZE_FIND_FINALIZE_DLG";
	case FINALIZE_SET_PROTECT_DISC:		return "FINALIZE_SET_PROTECT_DISC";
	case FINALIZE_CLICK_FINISH:			return "FINALIZE_CLICK_FINISH";
	case FINALIZE_FIND_COLLECTINFO_DLG:	return "FINALIZE_FIND_COLLECTINFO_DLG";
	case FINALIZE_FIND_EJECTING_DLG:	return "FINALIZE_FIND_EJECTING_DLG";
	case FINALIZE_WAIT_FOR_SUCCESS:		return "FINALIZE_WAIT_FOR_SUCCESS";
	case FINALIZE_IS_FINALIZED:			return "FINALIZE_IS_FINALIZED";
	case FINALIZE_ERROR:				return "FINALIZE_ERROR";

	default: return "unknown DirectCDStatus";
	};
}

inline CString NameOfEnumDirectCDError(DirectCDErrorStatus DCDErrorStatus)
{
	switch (DCDErrorStatus)
	{

	case ERROR_NO_ERROR:							return "ERROR_NO_ERROR";

	//Errors from CDirectCDStart
	case ERROR_DCD_NOT_STARTED:						return "ERROR_DCD_NOT_STARTED";
	case ERROR_DCD_WATCHDOG_NOT_RUNNING:			return "ERROR_DCD_WATCHDOG_NOT_RUNNING";
	case ERROR_TIMEOUT_WAIT_FOR_WELCOME_DLG:		return "ERROR_TIMEOUT_WAIT_FOR_WELCOME_DLG";
	case ERROR_TIMEOUT_WAIT_FOR_DRIVEINFO_DLG:		return "ERROR_TIMEOUT_WAIT_FOR_DRIVEINFO_DLG";
	case ERROR_TIMEOUT_WAIT_FOR_EMPTY_CDR:			return "ERROR_TIMEOUT_WAIT_FOR_EMPTY_CDR";
	case ERROR_DVCLIENT_NOT_RUNNING:				return "ERROR_DVCLIENT_NOT_RUNNING";
	case ERROR_FOUND_NOT_EMPTY_CDR:					return "ERROR_FOUND_NOT_EMPTY_CDR";

	//Errors from CDirectCDFormat
	case ERROR_TIMEOUT_WAIT_FOR_FORMAT_DISC_DLG:	return "ERROR_TIMEOUT_WAIT_FOR_FORMAT_DISC_DLG"; 
	case ERROR_TIMEOUT_WAIT_FOR_NAME_YOUR_DISC_DLG:	return "ERROR_TIMEOUT_WAIT_FOR_NAME_YOUR_DISC_DLG"; 
	case ERROR_TIMEOUT_WAIT_FOR_FORMATTING_DLG:		return "ERROR_TIMEOUT_WAIT_FOR_FORMATTING_DLG";
	case ERROR_TIMEOUT_WAIT_FOR_FORMAT_SUCCESS_DLG:	return "ERROR_TIMEOUT_WAIT_FOR_FORMAT_SUCCESS_DLG";
	case ERROR_TIMEOUT_WAIT_FOR_FORMAT_IS_FORMATTED:return "ERROR_TIMEOUT_WAIT_FOR_FORMAT_IS_FORMATTED";

	//Errors from CDirectCDBackup
	case ERROR_DVCLIENT_ABNORMAL_STOP_COPYING_DATA:	return "ERROR_DVCLIENT_ABNORMAL_STOP_COPYING_DATA";

	//Errors from CDirectCDFinalize
	case ERROR_TIMEOUT_WAIT_FOR_FINALIZE_DLG:		return "ERROR_TIMEOUT_WAIT_FOR_FINALIZE_DLG";
	case ERROR_TIMEOUT_FINALIZING:					return "ERROR_TIMEOUT_FINALIZING";
	case ERROR_TIMEOUT_WAIT_FOR_COLLECT_INFO_DLG:	return "ERROR_TIMEOUT_WAIT_FOR_COLLECT_INFO_DLG";
	case ERROR_TIMEOUT_SET_PROTECT_DISC:			return "ERROR_TIMEOUT_SET_PROTECT_DISC";
	case ERROR_FINISH_BUTTON_NOT_FOUND:				return "ERROR_FINISH_BUTTON_NOT_FOUND";
	case ERROR_CDR_NOT_READY_FOR_FINALIZE:			return "ERROR_CDR_NOT_READY_FOR_FINALIZE";
	case ERROR_TIMEOUT_FINALZE_PROCESS_NOT_STARTED:	return "ERROR_TIMEOUT_FINALZE_PROCESS_NOT_STARTED";

	//Errors from CWatchCDDlg
	case ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD:		return "ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD";
	
	default: return "unknown DirectCDErrorStatus";
	};
}

//****************************************************************************
//Error Strings für WatchCD.exe
//****************************************************************************
inline CString GetErrorString(DirectCDErrorStatus ErrorStatus)
{
	switch(ErrorStatus)
	{

	case ERROR_NO_ERROR:
		return "No Error\n";

	//***************************
	//Errors from CDirectCDStart
	//***************************
	case ERROR_DCD_NOT_STARTED:
		return "Could not start DirectCD.\n";
	case ERROR_DCD_WATCHDOG_NOT_RUNNING:
		return "DirectCD watchdog (background program) is not running.\n";
	case ERROR_TIMEOUT_WAIT_FOR_WELCOME_DLG:
		return "Couldn`t find DirectCD`s Welcome-Dialog.\n";
	case ERROR_TIMEOUT_WAIT_FOR_DRIVEINFO_DLG:
		return "Couldn`t find DirectCD`s Drive-Information-Dialog.\n";
	case ERROR_TIMEOUT_WAIT_FOR_EMPTY_CDR:
		return "Waiting for empty CD-R timeout reached after 60 sec.\n";
	case ERROR_DVCLIENT_NOT_RUNNING:
		return "DVClient is not running.\n";
	case ERROR_FOUND_NOT_EMPTY_CDR:
		return "The inserted CD-R was not empty or defect.\n";


	//***************************
	//Errors from CDirectCDFormat
	//***************************
	case ERROR_TIMEOUT_WAIT_FOR_FORMAT_DISC_DLG:
		return "DirectCD`s Dialog 'Format Disc' not on desktop after timeout.\n";

	case ERROR_TIMEOUT_WAIT_FOR_NAME_YOUR_DISC_DLG:
		return "DirectCD`s Dialog 'Name Yout Disc' not on desktop after timeout.\n";

	case ERROR_TIMEOUT_WAIT_FOR_FORMATTING_DLG:
		return "DirectCD`s Dialog 'Formatting disc...' not on desktop after timeout.\n";

	case ERROR_TIMEOUT_WAIT_FOR_FORMAT_SUCCESS_DLG:
		return "DirectCD`s Dialog 'successfully formatted' not on desktop after timeout.\n";

	case ERROR_TIMEOUT_WAIT_FOR_FORMAT_IS_FORMATTED:
		return "DirectCD`s Dialog 'Format' not on desktop after timeout.\n";


	//***************************
	//Errors from CDirectCDBackup
	//***************************
	case ERROR_DVCLIENT_ABNORMAL_STOP_COPYING_DATA:
		return "DVClient was asked if still copying data, but gave no response after timeout.\n";


	//*****************************
	//Errors from CDirectCDFinalize
	//*****************************
	case ERROR_TIMEOUT_WAIT_FOR_FINALIZE_DLG:
		return "DirectCD`s Dialog 'Finalize' not on desktop after timeout.\n";

	case ERROR_TIMEOUT_FINALIZING:
		return "Finalize process tooks more that 5 minutes.\n";

	case ERROR_FINISH_BUTTON_NOT_FOUND:
		return "Couldn`t click finish button on Finalize Dialog.\n";

	case ERROR_CDR_NOT_READY_FOR_FINALIZE:
		return "CD-R is not ready for Finalize. DirectCD not recognize CD-R as DirectCD CD-R.\n";


	//*****************************
	//Errors from CWatchCDDlg
	//*****************************
	case ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD: 
		return "An unexpected error from DirectCD.exe occoured. Backup stoped.\n";

	default:
		return "Caseless call but No error\n";
	}
}

//****************************************************************************
//enums für den Nachrichtenaustausch zwischen WatchCD.exe und DVClient.exe
//****************************************************************************

	enum BackupStatus
	{
		BS_DCD_NOT_FOUND_EMPTY_CDR	= 200,
		BS_DCD_FOUND_NOT_EMPTY_CDR	= 201,
		BS_DCD_FORMATTING			= 202,
		BS_DCD_START_BACKUP			= 203,
		BS_DCD_COPYING_DATA			= 204,
		BS_DCD_BACKUP_FAILED		= 205,
		BS_DCD_DATA_COPIED			= 206,
		BS_DCD_FINALIZE_SUCCESS		= 207,
		BS_DCD_UNEXPECTED_ERROR		= 208,
		BS_DCD_ERROR				= 299,
	};
