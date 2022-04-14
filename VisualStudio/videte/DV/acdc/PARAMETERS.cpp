// PARAMETERS.cpp: implementation of the PARAMETERS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "PARAMETERS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PARAMETERS::PARAMETERS()
{
	InitParameters(true);
}

PARAMETERS::~PARAMETERS()
{
#ifdef _UNICODE
	FreeAnsiStrings();
#endif
}

void PARAMETERS::InitParameters(bool bFirst/* =false */)
{
	WK_TRACE(_T("reset parameter list %d\n"), bFirst);
	// Set this flag to prevent destruction of strings because we
	// don't own them.

#ifdef _UNICODE
	if (!bFirst)
	{
		FreeAnsiStrings();
	}
#endif
	
	m_FileList.Clear();
	m_FileList.m_bOwnData = false;

	m_Command = COMMAND_UNKNOWN;
	m_psDriveName = NULL;
	m_bReal = false;
	m_bTAO = false;
	m_bUnderRunProt = false;
	m_psArtist = NULL;
	m_psTitle = NULL;
	m_iSpeed = 0;			// 0 is maximum speed
	m_bCDExtra = false;
	m_eEraseMode = NEROAPI_ERASE_QUICK;
	m_bEnableAbort = false;
	m_bSpeedTest = false;
	m_bCloseSession = false;
	m_iSessionToImport = -1;
	m_bDetectNonEmptyCDRW = false;
	m_bCDText = false;
	m_bUseRockridge = false;
	m_bCreateIsoFS = false;
	m_bCreateUdfFS = false;
	m_bImportRockridge = false;
	m_bImportIsoOnly = false;
	m_bPreferRockridge = false;
	m_bDisableEject = false;
	m_bVerify = false;
	m_bDVD = false;
	m_bAudioSelected = false;
	m_bISOSelected = false;
	
	m_psVolumeName = NULL;
	m_bJoliet = true;
	m_bMode2 = false;
	m_bVolumeNameSupplied = false;

	m_BurnType = BURNTYPE_UNKNOWN;

	m_psImageFilename = NULL;

	m_iNumTracks = 0;

	m_bProcessedParameterFile = false;

	m_bWriteNeroErrorLog = true;
	m_psErrorLogFilename = NULL;
	m_bNeroLogTimestamp = false;
	m_bForceEraseCDRW = false;
#ifdef _UNICODE
	for (int i=0; i<99; i++)
	{
		m_Tracks[i].m_psFilename = NULL;
	}
#endif
}


int PARAMETERS::GetFileListSize() const
{
	return m_FileList.GetSize();
}

const CSimpleStringArray& PARAMETERS::GetFileList() const
{
	return m_FileList;
}

CSimpleStringArray& PARAMETERS::GetFileList()
{
	return m_FileList;
}

LPCSTR PARAMETERS::GetErrorLogName() const
{
	return m_psErrorLogFilename;
}

COMMAND_LINE_ERRORS PARAMETERS::SetErrorLogName(int& argc, CStringArray& saParams)
{
	COMMAND_LINE_ERRORS ecl = SetBurnType(BURNTYPE_IMAGE);
	if (CLE_NO_ERROR != ecl)
	{
		return ecl;
	}


	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_ERROR_LOG_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}

	argc++;

#ifdef _UNICODE
	CWK_String str(saParams.GetAt(argc));
	WK_DELETE(m_psErrorLogFilename);
	m_psErrorLogFilename = str.DetachAnsiString();
#else
	m_psErrorLogFilename = (LPSTR)LPCSTR(saParams.GetAt(argc));
#endif
	return CLE_NO_ERROR;
}

COMMAND_LINE_ERRORS PARAMETERS::SetWriteErrorLog()
{
	m_bWriteNeroErrorLog = false;
	return CLE_NO_ERROR;
}

bool PARAMETERS::GetWriteErrorLog() const
{
	return m_bWriteNeroErrorLog;
}

COMMAND_LINE_ERRORS PARAMETERS::SetProcessedParameterFile()
{
	if (COMMAND_UNKNOWN != m_Command)
	{
		return CLE_MULTIPLE_COMMANDS_SPECIFIED;
	}

	m_bProcessedParameterFile = true;
	return CLE_NO_ERROR;
}

bool PARAMETERS::GetProcessedParameterFile()
{
	return m_bProcessedParameterFile;
}

const TRACK PARAMETERS::GetTrack(int iTrack) const
{
  return m_Tracks[iTrack];
}

const bool PARAMETERS::StillTrackSpaceLeft() const
{
	if (m_iNumTracks >= sizeof (m_Tracks)/sizeof (m_Tracks[0]))
	{
		return false;
	}
	else
	{
		return true;
	}
}

TRACK& PARAMETERS::GetTrack (int iTrack)
{
	return m_Tracks[iTrack];
}

const int PARAMETERS::GetTrackNumber(int iTrack) const
{
	return m_Tracks[iTrack].m_iTrack;
}

const LPCSTR PARAMETERS::GetTrackFileName (int iTrack) const
{
	return m_Tracks[iTrack].m_psFilename;
}

const NERO_TRACKMODE_TYPE PARAMETERS::GetTrackMode(int iTrack) const
{
	return m_Tracks[iTrack].m_Mode;
}

COMMAND_LINE_ERRORS PARAMETERS::SetTrackFileName(int iTrack, LPCTSTR psName)
{
#ifdef _UNICODE
	CWK_String str(psName);
	WK_DELETE(m_Tracks[iTrack].m_psFilename);
	m_Tracks[iTrack].m_psFilename = str.DetachAnsiString();
#else
	m_Tracks[iTrack].m_psFilename = (LPSTR)psName;
#endif
	return CLE_NO_ERROR;
}

const int PARAMETERS::GetNumberOfTracks() const
{
	return m_iNumTracks;
}

COMMAND_LINE_ERRORS PARAMETERS::SetTrackMode(int iTrack, NERO_TRACKMODE_TYPE eType)
{
	m_Tracks[iTrack].m_Mode = eType;
	return CLE_NO_ERROR;
}

COMMAND_LINE_ERRORS PARAMETERS::IncrementNumberOfTracks()
{
	m_iNumTracks++;
	return CLE_NO_ERROR;
}

COMMAND_LINE_ERRORS PARAMETERS::SetTrackNumber(int iTrack, int iNumber)
{
	m_Tracks[iTrack].m_iTrack = iNumber;
	return CLE_NO_ERROR;
}

const NEROAPI_CDRW_ERASE_MODE PARAMETERS::GetEraseMode() const
{
	return m_eEraseMode;
}

COMMAND_LINE_ERRORS PARAMETERS::SetEraseMode(NEROAPI_CDRW_ERASE_MODE eMode)
{
	if (GetEraseMode() == eMode)
	{
		return CLE_MULTIPLE_COMMANDS_SPECIFIED;
	}

	m_eEraseMode = eMode;
	return CLE_NO_ERROR;
}

COMMAND_LINE_ERRORS PARAMETERS::SetImageFileName(LPCTSTR psName)
{
#ifdef _UNICODE
	CWK_String str(psName);
	WK_DELETE(m_psImageFilename);
	m_psImageFilename = str.DetachAnsiString();
#else
	m_psImageFilename = (LPSTR)psName;
#endif	
	return CLE_NO_ERROR;
}

const LPCSTR PARAMETERS::GetImageFileName() const
{
	return m_psImageFilename;
}

COMMAND_LINE_ERRORS PARAMETERS::SetBurnType(BURNTYPE Type)
{
	// The Burn type cannot be set twice
	// BURNTYPE_ISOAUDIO is an exception to this rule.
	// However, settings for ISO/Audio and mixed more are handled
	// in SetAudioSelected and SetISOSelected directly.
	// So we do not have to worry about that here.
	// Another exception is BURNTYPE_FREESTYLE, where every
	// track can be written in a different mode.
	// So it is legal to provide that type more than once.

	if (BURNTYPE_UNKNOWN != GetBurnType())
	{
		if (BURNTYPE_FREESTYLE != GetBurnType())
		{
			// A burn type other than freestyle has been set already. 

			return CLE_BURN_TYPE_ALREADY_SPECIFIED;
		}
		else
		{
			// The burn type set before is freestyle.
			// If the new type is not BURNTYPE_FREESTYLE
			// we need to return an error.

			if (BURNTYPE_FREESTYLE != Type)
			{
				// Illegal combination of types

				return CLE_BURN_TYPE_ALREADY_SPECIFIED;
			}
		}
	}

	// Burn type has not been specified yet, so just set it

	m_BurnType = Type;
	return CLE_NO_ERROR;
}

const BURNTYPE PARAMETERS::GetBurnType() const
{
	return m_BurnType;
}

const COMMAND PARAMETERS::GetCommand() const
{
	return m_Command;
}

COMMAND_LINE_ERRORS PARAMETERS::SetCommand (COMMAND Command)
{
	if (GetCommand() == COMMAND_UNKNOWN)
	{
		m_Command = Command;
	}
	else
	{
		return CLE_MULTIPLE_COMMANDS_SPECIFIED;
	}

	return CLE_NO_ERROR;
}

const LPCSTR PARAMETERS::GetDriveName() const
{
	return m_psDriveName;
}

COMMAND_LINE_ERRORS PARAMETERS::SetDriveName (int& argc, CStringArray& saParams)
{
	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_DRIVE_NAME_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}

	argc++;

#ifdef _UNICODE
	CWK_String str(saParams.GetAt(argc));
	WK_DELETE(m_psDriveName);
	m_psDriveName = str.DetachAnsiString();
#else
	m_psDriveName = (LPSTR)LPCSTR(saParams.GetAt(argc));
#endif
	return CLE_NO_ERROR;
}	

const bool PARAMETERS::GetUseReal() const
{
	return m_bReal;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseReal ()
{
	m_bReal = true;
	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetUseTAO() const
{
	return m_bTAO;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseTAO ()
{
	m_bTAO = true;
	return CLE_NO_ERROR;
}	


const bool PARAMETERS::GetUseUnderRunProt() const
{
	return m_bUnderRunProt;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseUnderRunProt ()
{
	m_bUnderRunProt = true;
	return CLE_NO_ERROR;
}	
	

const LPCSTR PARAMETERS::GetArtist() const
{
	return m_psArtist;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetArtist (int& argc, CStringArray& saParams)
{
	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_ARTIST_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}
	int iParamCount = argc;
	int iSize = saParams.GetSize();
	if (iParamCount+1 >= iSize)
	{
		return CLE_MISSING_ARTIST_PARAMETER;
	}

	argc++;

#ifdef _UNICODE
	CWK_String str(saParams.GetAt(argc));
	WK_DELETE(m_psArtist);
	m_psArtist = str.DetachAnsiString();
#else
	m_psArtist = (LPSTR)LPCSTR(saParams.GetAt(argc));
#endif
	return CLE_NO_ERROR;
}	
	

const LPCSTR PARAMETERS::GetTitle() const
{
	return m_psTitle;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetTitle (int& argc, CStringArray& saParams)
{
	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_TITLE_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}

	argc++;

#ifdef _UNICODE
	CWK_String str(saParams.GetAt(argc));
	WK_DELETE(m_psTitle);
	m_psTitle = str.DetachAnsiString();
#else
	m_psTitle = (LPSTR)LPCSTR(saParams.GetAt(argc));
#endif
	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetAudioSelected() const
{
	return m_bAudioSelected;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetAudioSelected ()
{
	// Audio can only be used in combination with ISO

	if (BURNTYPE_UNKNOWN != GetBurnType())
	{
		if (false == GetISOSelected())
		{
			return CLE_BURN_TYPE_ALREADY_SPECIFIED;
		}
	}

	m_bAudioSelected = true;
	m_BurnType = BURNTYPE_ISOAUDIO;

	return CLE_NO_ERROR;
}	


const int PARAMETERS::GetSpeed() const
{
	return m_iSpeed;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetSpeed (int& argc, CStringArray& saParams)
{
	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_SPEED_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}

	argc++;

	CString sSpeed = saParams.GetAt(argc);
	m_iSpeed = _ttoi(sSpeed);

	// check if conversion went right

	if (GetSpeed() < 0)
	{
		return CLE_INVALID_SPEED_PARAMETER;
	}

	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetUseCDExtra() const
{
	return m_bCDExtra;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetCDExtra ()
{
	m_bCDExtra = true;
	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetEnableAbort() const
{
	return m_bEnableAbort;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetEnableAbort ()
{
	m_bEnableAbort = true;
	return CLE_NO_ERROR;
}	

const bool PARAMETERS::GetUseSpeedTest() const
{
	return m_bSpeedTest;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetSpeedTest ()
{
	m_bSpeedTest = true;
	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetCloseSession() const
{
	return m_bCloseSession;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetCloseSession ()
{
	m_bCloseSession = true;
	return CLE_NO_ERROR;
}	
	

const int PARAMETERS::GetSessionToImport() const
{
	return m_iSessionToImport;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetSessionToImport (int& argc, CStringArray& saParams)
{
	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_IMPORT_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}

	argc++;

	CString sSession = saParams.GetAt(argc);

	m_iSessionToImport = _ttoi(sSession);

	if (GetSessionToImport() < 0)
	{
		return CLE_INVALID_IMPORT_PARAMETER;
	}

	return CLE_NO_ERROR;
}	

const bool PARAMETERS::GetDetectNonEmptyCDRW() const
{
	return m_bDetectNonEmptyCDRW;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetDetectNonEmptyCDRW ()
{
	m_bDetectNonEmptyCDRW = true;
	return CLE_NO_ERROR;
}	


const bool PARAMETERS::GetUseCDText() const
{
	return m_bCDText;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseCDText ()
{
	m_bCDText = true;
	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetUseRockridge() const
{
	return m_bUseRockridge;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseRockridge ()
{
	m_bUseRockridge = true;
	return CLE_NO_ERROR;
}	

const bool PARAMETERS::GetCreateIsoFs() const
{
	return m_bCreateIsoFS;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetCreateIsoFs ()
{
	m_bCreateIsoFS = true;
	return CLE_NO_ERROR;
}	


const bool PARAMETERS::GetCreateUdfFS() const
{
	return m_bCreateUdfFS;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetCreateUdfFs ()
{
	m_bCreateUdfFS = true;
	return CLE_NO_ERROR;
}	


const bool PARAMETERS::GetImportRockridge() const
{
	return m_bImportRockridge;

}	

COMMAND_LINE_ERRORS PARAMETERS::SetImportRockridge ()
{
	m_bImportRockridge = true;
	return CLE_NO_ERROR;
}	


const bool PARAMETERS::GetImportIsoOnly() const
{
	return m_bImportIsoOnly;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetImportIsoOnly ()
{
	m_bImportIsoOnly = true;
	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetPreferRockRidge() const
{
	return m_bPreferRockridge;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetPreferRockridge ()
{
	m_bPreferRockridge = true;
	return CLE_NO_ERROR;
}	


const bool PARAMETERS::GetDisableEject() const
{
	return m_bDisableEject;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetDisableEject ()
{
	m_bDisableEject = true;
	return CLE_NO_ERROR;
}	
	

const bool PARAMETERS::GetVerify() const
{
	return m_bVerify;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetVerify ()
{
	m_bVerify = true;
	return CLE_NO_ERROR;
}	
	
const bool PARAMETERS::GetISOSelected() const
{
	return m_bISOSelected;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetISOSelected (int& argc, CStringArray& saParams)
{
	// ISO can be used in combination with 
	// any other burn type except BURNTYPE_IMAGE.

	if (BURNTYPE_UNKNOWN != GetBurnType())
	{
		// If ISO has been set already return an error

		if (true == GetISOSelected())
		{
			return CLE_BURN_TYPE_ALREADY_SPECIFIED;
		}
		else
		{
			// The other illegal type in this case
			// is BURNTYPE_IMAGE.
			if (BURNTYPE_IMAGE == GetBurnType())
			{
				return CLE_BURN_TYPE_ALREADY_SPECIFIED;
			}
		}
	}
	else
	{
		// No burn type set before

		m_BurnType = BURNTYPE_ISOAUDIO;
	}
	
	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_ISO_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}

	argc++;
	
	COMMAND_LINE_ERRORS ecl = SetVolumeName(saParams.GetAt(argc));
	if (CLE_NO_ERROR != ecl)
	{
		return ecl;
	}
	
	ecl = SetVolumeNameSupplied();
	if (CLE_NO_ERROR != ecl)
	{
		return ecl;
	}

	m_bISOSelected = true;
	return CLE_NO_ERROR;
}	

const LPCSTR PARAMETERS::GetVolumeName() const
{
	return m_psVolumeName;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetVolumeName (CString psName)
{
#ifdef _UNICODE
	CWK_String str(psName);
	WK_DELETE(m_psVolumeName);
	m_psVolumeName = str.DetachAnsiString();
#else
	m_psVolumeName = (LPSTR)LPCSTR(psName);
#endif
	return CLE_NO_ERROR;
}	
	
const bool PARAMETERS::GetUseJoliet() const
{
	return m_bJoliet;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseJoliet ()
{
	m_bJoliet = false;
	return CLE_NO_ERROR;
}	
	
const bool PARAMETERS::GetUseMode2() const
{
	return m_bMode2;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseMode2 ()
{
	m_bMode2 = true;
	return CLE_NO_ERROR;
}	
	
const bool PARAMETERS::GetVolumeNameSupplied() const
{
	return m_bVolumeNameSupplied;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetVolumeNameSupplied ()
{
	m_bVolumeNameSupplied = true;
	return CLE_NO_ERROR;
}	

const bool PARAMETERS::GetUseDVD() const
{
	return m_bDVD;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetUseDVD ()
{
	m_bDVD = true;
	return CLE_NO_ERROR;
}	

COMMAND_LINE_ERRORS PARAMETERS::SetImageBurnType(int &argc, CStringArray& saParams)
{
	COMMAND_LINE_ERRORS ecl = SetBurnType(BURNTYPE_IMAGE);
	if (CLE_NO_ERROR != ecl)
	{
		return ecl;
	}

	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_IMAGE_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}
	
	argc++;

	return SetImageFileName(saParams.GetAt(argc));
}

COMMAND_LINE_ERRORS PARAMETERS::SetFreestyleBurnType(int &argc, CStringArray& saParams)
{
	// make sure that the burn type has not been
	// specified already

	if (GetBurnType() != BURNTYPE_UNKNOWN && GetBurnType() != BURNTYPE_FREESTYLE)
	{
		return CLE_BURN_TYPE_ALREADY_SPECIFIED;
	}

	// set the track mode

	COMMAND_LINE_ERRORS ecl = CLE_NO_ERROR;

	if (!_tcsicmp (saParams.GetAt(argc), _T("--freestyle_mode1")))
	{
		ecl = SetTrackMode(GetNumberOfTracks(), NERO_TRACKMODE_MODE1);
	}
	else if (!_tcsicmp (saParams.GetAt(argc), _T("--freestyle_mode2")))
	{
		ecl = SetTrackMode(GetNumberOfTracks(), NERO_TRACKMODE_MODE2_FORM1);
	}
	else if (!_tcsicmp (saParams.GetAt(argc), _T("--freestyle_audio")))
	{
		ecl = SetTrackMode(GetNumberOfTracks(), NERO_TRACKMODE_AUDIO);
	}

	if (CLE_NO_ERROR != ecl)
	{
		return ecl;
	}

	// the file name must be specified after the track mode
	//next param don`t has to be start with "--"
	COMMAND_LINE_ERRORS cle = CLE_MISSING_IMAGE_PARAMETER;
	COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

	if(cleResult != CLE_NO_ERROR)
	{
		//return the command line error
		return cleResult;
	}

	// make sure that we can add another track
	if (StillTrackSpaceLeft() == false)
	{
		return CLE_MAXIMUM_NUMBER_OF_TRACKS_REACHED;
	}

	// Get the track filename
	argc++;

	ecl = SetTrackFileName(GetNumberOfTracks(), saParams.GetAt(argc));
	if (CLE_NO_ERROR != ecl)
	{
		return ecl;
	}

	ecl = IncrementNumberOfTracks();
	if (CLE_NO_ERROR != ecl)
	{
		return ecl;
	}

	// finally, set the burn type
	return SetBurnType(BURNTYPE_FREESTYLE);
}

COMMAND_LINE_ERRORS PARAMETERS::AddAudioReadTrack(int &argc, CStringArray& saParams)
{
	bool bOK = false;

	if (_tcslen (saParams.GetAt(argc)) <= 4 &&
		_T('0') <= (saParams.GetAt(argc)).GetAt(2) && (saParams.GetAt(argc)).GetAt(2) <= _T('9') &&
		_T('0') <= (saParams.GetAt(argc)).GetAt(3) && (saParams.GetAt(argc)).GetAt(3) <= _T('9'))
	{

		if (StillTrackSpaceLeft() == false)
		{
			return CLE_MAXIMUM_NUMBER_OF_TRACKS_REACHED;
		}

		CString sNumberOfTracks = (saParams.GetAt(argc)).GetAt(2);
		
		COMMAND_LINE_ERRORS ecl = SetTrackNumber(GetNumberOfTracks(), _ttoi (sNumberOfTracks));
		
		if (GetTrackNumber(GetNumberOfTracks()) > 0)
		{

			//next param don`t has to be start with "--"
			COMMAND_LINE_ERRORS cle = CLE_MISSING_TRACK_PARAMETER;
			COMMAND_LINE_ERRORS cleResult = CheckNextParamIsParam(argc, saParams, cle);

			if(cleResult != CLE_NO_ERROR)
			{
				//return the command line error
				return cleResult;
			}			

			argc++;

			ecl = SetTrackFileName(GetNumberOfTracks(), saParams.GetAt(argc));
			if (CLE_NO_ERROR != ecl)
			{
				return ecl;
			}
			ecl = IncrementNumberOfTracks();
			if (CLE_NO_ERROR != ecl)
			{
				return ecl;
			}
			bOK = true;
		}
	}

	if (!bOK)
	{
		return CLE_INVALID_PARAMETER;
	}

	return CLE_NO_ERROR;
}

COMMAND_LINE_ERRORS PARAMETERS::AddAudioIsoFreestyleWriteTrack(int &argc, CStringArray& saParams)
{
	if (false == GetVolumeNameSupplied() && GetBurnType() != BURNTYPE_FREESTYLE)
	{
		// If volume name was not yet supplied, the parameter is an audio file.

		if (StillTrackSpaceLeft() == false)
		{
			return CLE_MAXIMUM_NUMBER_OF_TRACKS_REACHED;
		}

		COMMAND_LINE_ERRORS ecl = SetTrackFileName(GetNumberOfTracks(), saParams.GetAt(argc));
		if (CLE_NO_ERROR != ecl)
		{
			return ecl;
		}

		ecl = IncrementNumberOfTracks();
		if (CLE_NO_ERROR != ecl)
		{
			return ecl;
		}
	}
	else
	{
		// This is a data file so simply add it to the back of the array.
#ifdef _UNICODE
		CWK_String sFileToAdd(saParams.GetAt(argc));
		GetFileList().Add (sFileToAdd.DetachAnsiString());
#else
		CString sFileToAdd = saParams.GetAt(argc);
		GetFileList().Add ((char*)(const char*)sFileToAdd);
#endif
	}

	return CLE_NO_ERROR;
}

const bool PARAMETERS::GetNeroLogTimestamp() const
{
	return m_bNeroLogTimestamp;
}

COMMAND_LINE_ERRORS PARAMETERS::SetNeroLogTimestamp()
{
	m_bNeroLogTimestamp = true;
	return CLE_NO_ERROR;
}

COMMAND_LINE_ERRORS PARAMETERS::SetForceEraseCDRW()
{
	m_bForceEraseCDRW = TRUE;
	return CLE_NO_ERROR;
}

const bool PARAMETERS::GetForceEraseCDRW() const
{
	return m_bForceEraseCDRW;
}

//check if the next parameter in parameterlist is a parameter started with "--"
COMMAND_LINE_ERRORS PARAMETERS::CheckNextParamIsParam(int& argc, CStringArray& saParams, 
													  COMMAND_LINE_ERRORS cle)
{
	int iCurrentParam	= argc;
	int iSize			= saParams.GetSize();	
	CString sIsParam;
	if (iCurrentParam+1 >= iSize)
	{
		return cle;
	}
	else
	{
		int iLength = saParams.GetAt(iCurrentParam+1).GetLength();
		//next param don`t has to start with "--"
		if(iLength == 1)
		{
			sIsParam =  saParams.GetAt(iCurrentParam+1).GetAt(0);
		}
		else if(iLength >= 2)
		{
			sIsParam =  saParams.GetAt(iCurrentParam+1).GetAt(0);
			sIsParam += saParams.GetAt(iCurrentParam+1).GetAt(1);
		}

		
		//only parameters can start with "--"
		if(sIsParam == _T("--") || sIsParam == _T("-"))
		{
			return cle;
		}
	}

	return CLE_NO_ERROR;
}

#ifdef _UNICODE
void PARAMETERS::FreeAnsiStrings()
{
	WK_DELETE(m_psErrorLogFilename);
	WK_DELETE(m_psDriveName);
	WK_DELETE(m_psArtist);
	WK_DELETE(m_psTitle);
	WK_DELETE(m_psImageFilename);
	int i, nCount;
	for (i=0; i<99; i++)
	{
		WK_DELETE(m_Tracks[i].m_psFilename);
	}
	nCount = m_FileList.GetSize();
	for (i=0; i<nCount; i++)
	{
		delete m_FileList.vect[i];
	}
}
#endif
