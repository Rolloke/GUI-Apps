
#include "StdCipc.h"

#ifndef NO_WK_DONGLE

#include "WK_Dongle.h"
#include "WK_DebugOptions.h"
#include "wkclasses\WK_Trace.h"
#include "wkclasses\WK_Profile.h"
#include "wkclasses\WK_RunTimeError.h"
#include "wkclasses\wk_util.h"
#include "wkclasses\wk_file.h"
#include "wkclasses\rsa.h"
#include "wkclasses\wk_wincpp.h"

#else
// <NO_WK_DONGLE>
#define WK_TRACE g_pDebug->Trace
#define WK_TRACE_ERROR g_pDebug->Trace
// </NO_WK_DONGLE>
#endif

#define WK_WITH_DONGLE 1

#if WK_WITH_DONGLE
#include "FastApi.h"
#include "HLApi_c.h"
#endif


#define MAGIC_NUMBER 234567
static _TCHAR szMagicNumber[] = _T("MagicNumber");
static _TCHAR *szTheDongleSection=_T("Dongle");

#define WK_VERSION_NUMBER 3600
const _TCHAR *szTheHardlockMutex = _T("HardlockMutex");
const _TCHAR *szTheHardlockMemory = _T("HardlockMemory");	// system wide memory
const DWORD dwTheHardlockMemoryLength = 200;
// the shared memory format:
// 0..127 is the plain eep Data
// 128 version byte
// 129..129+3	dwHardwareState
const int iOffsetVersion = 128;
const int iOffsetHardwareState = 129;
//
const int iEEPOffsetVersionMinor=5;
const int iEEPOffsetVersionMajor=3;

// Dongle Moduladresse
WORD CWK_Dongle::m_wTheModulAddress = 2849;

#define WK_MODULE_ADDRESS  2849
#define DLR_MODULE_ADDRESS 10114

// Ausgangsschlüssel ( nicht kodiert ), 8 chars "ISTechno"
const BYTE g_szReferenceKey1[] = { 0x49, 0x53, 0x54, 0x65, 0x63, 0x68, 0x6e, 0x6f };
// Überprüfungsschlüssel ( kodiert )	//Das Ergebnis der Verschlüsselung von RefKey
const BYTE g_szVerificationKey1[] = { 0x4c, 0xac, 0x87, 0x8f, 0xeb, 0x2a, 0x1b, 0x66 };

// valid OEM codes
// 1 IST
// 2 Garny  // 20000621 gf removed as option
// 3 NCR
// 4 AKUBIS
//////////////////////////////////////////////////////////////

// static helper functions to reduce strings
static void EEPIndexError(int byteIndex)
{
	WK_TRACE_ERROR(_T("EEP data index error %d\n"),byteIndex);
}
//////////////////////////////////////////////////////////////
static void OptionalMessage(LPCTSTR szValue, int byValue)
{
	WK_TRACE(_T("Optional %s=%d\n"), szValue, byValue);
}
//////////////////////////////////////////////////////////////
void CWK_Dongle::InitWK_Dongle()
{
	m_bIsValid = FALSE;
	m_wDongleFormat=0;
	m_dwHardlockState = NO_DONGLE;	// OOPS better init value ?
	
	m_bMajorVersion = 0;
	m_bMinorVersion = 0;

	// product
	CString sModule;
	GetModuleFileName(NULL,sModule.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	sModule.ReleaseBuffer();
	if (!sModule.IsEmpty())
	{
		m_sVersion = WK_GetFileVersion(sModule);
	}

	m_productCode = IPC_FREE_STYLE;	// OOPS
	m_wOEMCode = OEM_IDIP;	// NOT YET 0, for simplicity
	//
	m_bIsTransmitter= FALSE;
	m_bIsReceiver= FALSE;
	m_bIsLocal= FALSE;
	// base
	m_bRunLauncher= FALSE;
	m_bRunSupervisor= FALSE;
	m_bRunExplorer= FALSE;
	m_bRunServer= FALSE;

	// database
	m_bRunDatabaseClient= FALSE;
	m_bRunDatabaseServer= FALSE;
	m_bRunCDRWriter = FALSE;

	// codecs
	m_bRunCoCoUnit= FALSE;
	m_bRunMiCoUnit= FALSE;
	m_bRunCoCoUnitPCI = FALSE;
	m_bRunMiCoUnitPCI = FALSE;

	// link
	m_bRunISDNUnit= FALSE;
	m_bRunSocketUnit= FALSE;

	// boxes
	m_bRunPTUnit= FALSE;
	m_bRunVCSUnit= FALSE;	

	// misc units
	m_bRunAKUUnit= FALSE;
	m_bRunCommUnit= TRUE;
	m_bRunGAUnit= FALSE;
	m_bRunSDIUnit= FALSE;
	m_bRunSimUnit= FALSE;

	// misc
	m_bRunSecAnalyzer= FALSE;
	m_bRunUpdateHandler= FALSE;
	m_bRunServicePack=FALSE;

	m_bRunCoCoTest = FALSE;
	m_bRunMiCoTest = FALSE;
	m_bRunMiCoUnit2= FALSE;

	// for future use
	m_bRunMiCoUnit3 = FALSE;
	m_bRunMiCoUnit4 = FALSE;
	m_bRunJaCobUnit1= FALSE;
	m_bRunJaCobUnit2= FALSE;
	m_bRunJaCobUnit3= FALSE;
	m_bRunJaCobUnit4= FALSE;

	m_bRunGemosUnit = FALSE;
	m_bRunICPCONUnit= FALSE;
	m_bRunUSBCameraUnit= FALSE;

	m_bRunSDIConfig = FALSE;
	m_bRunHTTP= FALSE;
	m_bRunICPCONClient= FALSE;
	m_bRunClient4= FALSE;
	m_bRunClient5= FALSE;
	m_bRunClient6= FALSE;
	m_bRunClient7= FALSE;
	m_bRunClient8= FALSE;
	m_bRunClient9= FALSE;
	m_bRunClient10= FALSE;

	m_bRunSaVicUnit1= FALSE;
	m_bRunSaVicUnit2= FALSE;
	m_bRunSaVicUnit3= FALSE;
	m_bRunSaVicUnit4= FALSE;
	m_bRunAudioUnit= FALSE;
	m_bRunQUnit= FALSE;
	m_bRunIPCameraUnit= FALSE;
	m_bRunProgram8= FALSE;
	m_bRunProgram9= FALSE;
	m_bRunProgram10= FALSE;

	m_bRunISDNUnit2= FALSE;
	m_bRunPTUnit2 = FALSE;
	m_bRunSMSUnit= FALSE;
	m_bRunTOBSUnit= FALSE;
	m_bRunEMailUnit= FALSE;
	m_bRunFAXUnit= FALSE;
	m_bRunLinkUnit7= FALSE;
	m_bRunLinkUnit8= FALSE;
	m_bRunLinkUnit9= FALSE;
	m_bRunLinkUnit10= FALSE;

	// <><><><> functionalities <><><><>
	m_bAllowSoftDecodeCoCo= FALSE;
	m_bAllowSoftDecodeMiCo= FALSE;
	m_bAllowHardDecodeCoCo= FALSE;
	m_bAllowHardDecodeMiCo= FALSE;

	m_bAllowLocalProcesses=FALSE;
	// for future use
	// CAVEAT 1..10 are TRUE by default (in ProductView)
	m_bAllowSDICashSystems = FALSE;
	m_bAllowSDIAccessControl = FALSE;
	m_bAllowISDN2BChannels = TRUE;
	m_bAllowTimers= TRUE;
	m_bAllowMultiMonitor= TRUE;
	m_bAllowHTMLmaps= TRUE;
	m_bAllowSDIParkmanagment= FALSE;
	m_bAllowFunctionality8= FALSE;
	m_bAllowFunctionality9= FALSE;
	m_bAllowFunctionality10= FALSE;

	m_bAllowHTMLNotification = FALSE;
	m_bAllowHTMLInformation = FALSE;
	m_bAllowRouting = FALSE;
	m_bAllowSDIIndustrialEquipment = FALSE;
	m_bAllowSoftDecodePT = FALSE;
	m_bAllowMotionDetection= FALSE;
	m_bAllowAlarmOffSpans= FALSE;
	m_bIsDemo= FALSE;
	m_bRestrictedTo8CamerasPerBoard= FALSE;
	m_bAllowFunctionality20 = FALSE;

#ifndef _HARDLOCK
	m_bRunTashaUnit1 = FALSE;
	m_bRunTashaUnit2 = FALSE;
	m_bRunTashaUnit3 = FALSE;
	m_bRunTashaUnit4 = FALSE;
	m_iNrOfHosts = 5;
	m_iNrOfCameras = 0;
	m_iNrOfIPCameras = 0;
#endif
	//
	m_bExeOkay=FALSE;
	m_bHardwareDongleFound = FALSE;

	m_bAllowInternalAccess=FALSE;
	m_bAllowEEPWrite=FALSE;
}
//////////////////////////////////////////////////////////////
static BYTE * CreateSharedMemory(HANDLE & hExtraMemory, BOOL & bFound)
{
#ifndef NO_WK_DONGLE
	void *pExtraMemory = NULL;
	int iNumRetries = 0;
	DWORD dwMapFlag = FILE_MAP_READ | FILE_MAP_WRITE;
	
	bFound = FALSE;

	// first try to find existing one
	hExtraMemory = OpenFileMapping(FILE_MAP_READ,
										FALSE,	// inherit
									szTheHardlockMemory
									);
	if (hExtraMemory==NULL) 
	{
		// WK_TRACE(_T("Creating hardlock mem\n"));
		// if not found create it
		hExtraMemory = CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, 
									(LPSECURITY_ATTRIBUTES)NULL, 
									PAGE_READWRITE, 0, 
									dwTheHardlockMemoryLength,
									szTheHardlockMemory
									);
	} 
	else 
	{
		// WK_TRACE(_T("Found hardlock mem\n"));
		bFound = TRUE;
	}
	if (hExtraMemory)
	{
		while (pExtraMemory==NULL && iNumRetries<10) 
		{
			if (bFound)
			{
				dwMapFlag = FILE_MAP_READ;
			}
			else
			{
				dwMapFlag = FILE_MAP_READ | FILE_MAP_WRITE;
			}
			pExtraMemory = MapViewOfFile(
								hExtraMemory, 
								dwMapFlag,
								0, 0,	// offsets
								dwTheHardlockMemoryLength
								);
			iNumRetries++;
			if (pExtraMemory==NULL)
			{
				WK_TRACE(_T("Hardlock: FAILED to map extra, error %s\n"),
					LPCTSTR(GetLastErrorString())
					);
				Sleep(50);
			} 
			else
			{
				// fine
				// pExtraMemory points to the 'global' shared memory
			}
		}	// end of MFC retry loop
	} 
	else 
	{
		// could not create file mapping handle
	}

	return (BYTE *)pExtraMemory;
#else
	return NULL;
#endif
}
//////////////////////////////////////////////////////////////
#ifndef NO_WK_DONGLE
static void FreeSharedMemory(HANDLE & hExtraMemory, BYTE *pSharedMem)
{
	if (pSharedMem) {
		UnmapViewOfFile(pSharedMem);
	}
	if (hExtraMemory) {
		WK_CLOSE_HANDLE(hExtraMemory);
	}
}
#endif
////////////////////////////////////////////////////////////////////
void CWK_Dongle::Read(BOOL bFillSharedMemory)
{
#ifdef _HARDLOCK
	ReadHardwareDongle();
	if (!IsValid())
#endif
	{
		ReadCryptoDongle();
	}
	ReadSecondCryptoDongle();
	if (bFillSharedMemory)
	{
		FillSharedMemory();
	}
}
////////////////////////////////////////////////////////////////////
void CWK_Dongle::ReadDongle(BOOL bIngoreSharedMemory)
{
#ifndef NO_WK_DONGLE
	CMutex mutex(FALSE,szTheHardlockMutex);
	CSingleLock lock(&mutex);
	if (bIngoreSharedMemory==FALSE && lock.Lock(5000)) 
	{
		// lock successful
		// check for existance of shared memory
		BOOL bFound=FALSE;	// TRUE is the shared memory is already found
		m_pSharedMemory = CreateSharedMemory(m_hSharedMemory,bFound);
		if (m_pSharedMemory) 
		{
			if (bFound) 
			{
				// if found read from shared memory
				ReadSharedMemory();
			} 
			else 
			{
				// else read hardwareDongle, create and fill shared memory
				// CAVEAT oreder is important, ReadHardwareDongle fills
				// the shared memory on the fly
				Read(TRUE);
			}
		} 
		else 
		{
			// shared memory not found/created
			Read(FALSE);
		}

		lock.Unlock();
	} 
	else 
	{
		if (bIngoreSharedMemory==FALSE) 
		{
			WK_TRACE_ERROR(_T("Failed to access HardlockMemory\n"));
		}
		// lock failed, or shared memory is not used
		Read(FALSE);
	}
	if (GetProductCode() == IPC_DTS_RECEIVER)
	{
		CString sModule;
		GetModuleFileName(NULL,sModule.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sModule.ReleaseBuffer();
		if (!sModule.IsEmpty())
		{
			m_sVersion = WK_GetFileVersion(sModule);
		}
	}
#else
	ReadHardwareDongle();
#endif
	// OLD try to read softdongle.cfg
}

//////////////////////////////////////////////////////////////
CWK_Dongle::CWK_Dongle(BOOL bIngoreSharedMemory)
{
	m_hSharedMemory = NULL;
	m_pSharedMemory = NULL;
	InitWK_Dongle();
	ReadDongle(bIngoreSharedMemory);
}

//////////////////////////////////////////////////////////////
// Benutzt den Softdongle (Nur für das HHLA-Projekt gedacht!!)
CWK_Dongle::CWK_Dongle(const CString &sFileName)
{
	m_hSharedMemory = NULL;
	m_pSharedMemory = NULL;

	InitWK_Dongle();

	CMutex mutex(FALSE,szTheHardlockMutex);
	CSingleLock lock(&mutex);
	if (lock.Lock(5000)) 
	{
		// lock successful
		// check for existance of shared memory
		BOOL bFound=FALSE;	// TRUE is the shared memory is already found
		m_pSharedMemory = CreateSharedMemory(m_hSharedMemory,bFound);

		if (m_pSharedMemory) 
		{
			ReadSoftwareDongle(sFileName);	// fills all members
			if (!bFound)
			{
				FillSharedMemory();
			}
		} 
		else 
		{
			// shared memory not found/created
			WK_TRACE(_T("CreateSharedMemory failed\n"));
		}

		lock.Unlock();
	}
}
//////////////////////////////////////////////////////////////
BOOL CWK_Dongle::RunByApplicationID(WK_ApplicationId appId)
{
#ifndef NO_WK_DONGLE
	BOOL bRun=FALSE;

	switch (appId) {
		case WAI_LAUNCHER: bRun = RunLauncher(); break;
		case WAI_SUPERVISOR: bRun = RunSupervisor(); break;
		case WAI_SECURITY_SERVER: bRun = RunServer(); break;
		case WAI_DATABASE_SERVER: bRun = RunDatabaseServer(); break;
		case WAI_AC_DC: bRun = RunCDRWriter(); break;
		case WAI_EXPLORER: bRun = RunExplorer(); break;
		case WAI_ICPCON_CLIENT: bRun = RunICPCONClient(); break;
		case WAI_HTTP: bRun = RunHTTP(); break;
		case WAI_DATABASE_CLIENT: bRun = RunDatabaseClient(); break;
		case WAI_SIMUNIT: bRun = RunSimUnit(); break;
		case WAI_GAUNIT: bRun = RunGAUnit(); break;
		case WAI_COMMUNIT: bRun = RunCommUnit(); break;
		case WAI_COCOUNIT: bRun = RunCoCoUnit(); break;
		case WAI_MICOUNIT: bRun = RunMiCoUnit(); break;
		case WAI_AKUUNIT: bRun = RunAKUUnit(); break;
		case WAI_SDIUNIT: bRun = RunSDIUnit(); break;
		case WAI_ISDN_UNIT: bRun = RunISDNUnit(); break;
		case WAI_SOCKET_UNIT: bRun = RunSocketUnit(); break;
		case WAI_VCS_UNIT: bRun = RunVCSUnit(); break;
		case WAI_SMS_UNIT: bRun = RunSMSUnit(); break;
		case WAI_GEMOS_UNIT: bRun = RunGemosUnit(); break;
		case WAI_PRESENCE_UNIT: bRun = RunPTUnit(); break;
		case WAI_TOBS_UNIT: bRun = RunTOBSUnit(); break;
		case WAI_EMAIL_UNIT: bRun = RunEMailUnit(); break;
		case WAI_FAX_UNIT: bRun = RunFAXUnit(); break;
		case WAI_UPDATE_HANDLER: bRun = RunUpdateHandler(); break;
		case WAI_ANALYZER: bRun = RunSecAnalyzer(); break;
		case WAI_SDICONFIG : bRun = RunSDIConfig(); break;
		// second instances
		case WAI_MICOUNIT_2: bRun = RunMiCoUnit2(); break;
		case WAI_MICOUNIT_3: bRun = RunMiCoUnit3(); break;
		case WAI_MICOUNIT_4: bRun = RunMiCoUnit4(); break;
		case WAI_ISDN_UNIT_2: bRun = RunISDNUnit2(); break;
		case WAI_PRESENCE_UNIT_2: bRun = RunPTUnit2(); break;

		case WAI_JACOBUNIT_1: bRun = RunJaCobUnit1(); break;
		case WAI_JACOBUNIT_2: bRun = RunJaCobUnit2(); break;
		case WAI_JACOBUNIT_3: bRun = RunJaCobUnit3(); break;
		case WAI_JACOBUNIT_4: bRun = RunJaCobUnit4(); break;

		case WAI_SAVICUNIT_1: bRun = RunSaVicUnit1(); break;
		case WAI_SAVICUNIT_2: bRun = RunSaVicUnit2(); break;
		case WAI_SAVICUNIT_3: bRun = RunSaVicUnit3(); break;
		case WAI_SAVICUNIT_4: bRun = RunSaVicUnit4(); break;

#ifndef _HARDLOCK
		case WAI_TASHAUNIT_1: bRun = RunTashaUnit1(); break;
		case WAI_TASHAUNIT_2: bRun = RunTashaUnit2(); break;
		case WAI_TASHAUNIT_3: bRun = RunTashaUnit3(); break;
		case WAI_TASHAUNIT_4: bRun = RunTashaUnit4(); break;
#endif

		case WAI_AUDIOUNIT_1: bRun = RunAudioUnit(); break;
		case WAI_AUDIOUNIT_2: bRun = RunAudioUnit(); break;
		case WAI_AUDIOUNIT_3: bRun = RunAudioUnit(); break;
		case WAI_AUDIOUNIT_4: bRun = RunAudioUnit(); break;
		case WAI_ICPCON_UNIT: bRun = RunICPCONUnit(); break;
		
		case WAI_USBCAMUNIT_1: bRun = RunUSBCameraUnit(); break;
		case WAI_USBCAMUNIT_2: bRun = RunUSBCameraUnit(); break;
		case WAI_USBCAMUNIT_3: bRun = RunUSBCameraUnit(); break;
		case WAI_USBCAMUNIT_4: bRun = RunUSBCameraUnit(); break;
			
		case WAI_DV_REMOTE_CD: bRun = TRUE; break;
		case WAI_DV_WATCH_CD:  bRun = TRUE; break;
		case WAI_DV_STARTER: bRun = TRUE; break;
		case WAI_YUTA_UNIT: bRun = TRUE; break;
		case WAI_EWF_CONTROL:    bRun = TRUE; break;
		case WAI_VIDETE_CHECK_DISK:    bRun = TRUE; break;
		case WAI_QUNIT:    bRun = RunQUnit(); break;
		case WAI_HEALTH_CONTROL:    bRun = TRUE; break;

		default:
			WK_TRACE_ERROR(_T("Invalid application id 0x%x %s\n"),(DWORD)appId,NameOfEnum(appId));
	};		// end of switch over appId

	return bRun;
#else
	return FALSE;
#endif
}
//////////////////////////////////////////////////////////////
CWK_Dongle::CWK_Dongle(WK_ApplicationId appId, BOOL bIngoreSharedMemory)
{
	m_hSharedMemory = NULL;
	m_pSharedMemory = NULL;
	// first default constructor actions
	InitWK_Dongle();
	ReadDongle(bIngoreSharedMemory);

	if (m_bIsValid) 
	{
		// then check the exe permission
		m_bExeOkay = RunByApplicationID(appId);

		if (m_bExeOkay==FALSE) 
		{
			WK_TRACE_ERROR(_T("No permission in dongle found %s\n"),NameOfEnum(appId));
			CWK_RunTimeError rte(
				appId,
				REL_CANNOT_RUN,
				RTE_DONGLE,
				NULL,
				1	// subcode exe not allowed
				);
			rte.Send();
		}
	} 
	else 
	{
		WK_TRACE_ERROR(_T("Dongle not found\n"));
		// invalid dongle or dongle not found
		CWK_RunTimeError rte(
				appId,
				REL_CANNOT_RUN,
				RTE_DONGLE,
				NULL,
				3	// dongle not found
				);
		rte.Send();
	}
}
//////////////////////////////////////////////////////////////
CWK_Dongle::~CWK_Dongle()
{
	// already closed after ReadHardwareDongle
#ifndef NO_WK_DONGLE
	FreeSharedMemory(m_hSharedMemory,m_pSharedMemory);
#endif
}
//////////////////////////////////////////////////////////////
BOOL CWK_Dongle::IsValid() const
{
	// NOT YET
	return m_bIsValid;
}
//////////////////////////////////////////////////////////////
void CWK_Dongle::ReadSoftwareDongle(LPCTSTR szFileToUse)
{
#ifndef NO_WK_DONGLE
	CWKDebugOptions dongleOptions;
	CString sDebugFile;
	if (szFileToUse)
	{
		sDebugFile=szFileToUse;
	}
	else
	{
		sDebugFile = _T("c:\\SoftDongle.cfg");
	}
	
	// check for given path or C:\SoftDongle.cfg first, then in current directory
	BOOL bFoundFile = dongleOptions.ReadFromFile(sDebugFile);
	if (bFoundFile==FALSE
		&& szFileToUse==NULL)
	{	// do not try default name, if name is specified

		sDebugFile = _T("SoftDongle.cfg");
		bFoundFile = dongleOptions.ReadFromFile(sDebugFile);
	}

	if (bFoundFile) 
	{
		m_wDongleFormat=1;
		// product
		DWORD dwMagic = dongleOptions.GetValue(szMagicNumber, 0);
		if (dwMagic!=MAGIC_NUMBER) 
		{
			WK_TRACE_ERROR(_T("Invalid SoftDongle.cfg, (MagicNumber wrong)\n"));
			InitWK_Dongle();
			return;	// <<< EXIT >>>
		}

		m_productCode = (InternalProductCode) dongleOptions.GetValue(_T("ProductCode"), IPC_FREE_STYLE);
//		WK_TRACE(_T("reading Product code %d from %s\n"),(int)m_productCode,sDebugFile);
		// NOT YET range check
		m_wOEMCode = (WORD)dongleOptions.GetValue(_T("OEMCode"), 1);
//		WK_TRACE(_T("reading OEM code %d from %s\n"),(int)m_wOEMCode,sDebugFile);

		// call the member function via x()
		// convert to string #x
		#define TRANSFER(x) m_b##x = dongleOptions.GetValue(_T(#x), m_b##x);
		#include "AllDongleTransfers.h"
		#undef TRANSFER
#ifndef _HARDLOCK
		m_iNrOfHosts = dongleOptions.GetValue(_T("NrOfHosts"),0);
		m_iNrOfCameras = dongleOptions.GetValue(_T("NrOfCameras"),0);
		m_iNrOfIPCameras = dongleOptions.GetValue(_T("NrOfIPCameras"),0);
#endif


		// special for SDI options, which are TRUE as default
		m_bAllowSDICashSystems |= dongleOptions.GetValue(_T("AllowFunctionality1"),FALSE);
		m_bAllowSDIAccessControl |= dongleOptions.GetValue(_T("AllowFunctionality2"),FALSE);

		CalcProductFields();	// set IsTransmitter()....

		m_bIsValid = TRUE;
		m_dwHardlockState = STATUS_OK;
	} 
	else 
	{
		// not found
		if (szFileToUse) 
		{
			WK_TRACE_ERROR(_T("CFG File '%s' not found\n"), szFileToUse);
		}
	}

#endif
}
//////////////////////////////////////////////////////////////
void CWK_Dongle::ReadSoftwareDongle(CFile& file)
{
#ifndef NO_WK_DONGLE
	CWKDebugOptions dongleOptions;

	// check for given path or C:\SoftDongle.cfg first, then in current directory
	BOOL bFoundFile = dongleOptions.ReadFromFile(file);
	if (bFoundFile) 
	{
		m_wDongleFormat=1;
		// product
		DWORD dwMagic = dongleOptions.GetValue(szMagicNumber, 0);
		if (dwMagic!=MAGIC_NUMBER) 
		{
			WK_TRACE_ERROR(_T("Invalid SoftDongle.cfg, (MagicNumber wrong)\n"));
			InitWK_Dongle();
			return;	// <<< EXIT >>>
		}

		m_productCode = (InternalProductCode) dongleOptions.GetValue(_T("ProductCode"), IPC_FREE_STYLE);
		//		WK_TRACE(_T("reading Product code %d from %s\n"),(int)m_productCode,sDebugFile);
		// NOT YET range check
		m_wOEMCode = (WORD)dongleOptions.GetValue(_T("OEMCode"), 1);
		//		WK_TRACE(_T("reading OEM code %d from %s\n"),(int)m_wOEMCode,sDebugFile);

		// call the member function via x()
		// convert to string #x
#define TRANSFER(x) m_b##x = dongleOptions.GetValue(_T(#x), m_b##x);
#include "AllDongleTransfers.h"
#undef TRANSFER
#ifndef _HARDLOCK
		m_iNrOfHosts = dongleOptions.GetValue(_T("NrOfHosts"),0);
		m_iNrOfCameras = dongleOptions.GetValue(_T("NrOfCameras"),0);
		m_iNrOfIPCameras = dongleOptions.GetValue(_T("NrOfIPCameras"),0);
#endif

		// special for SDI options, which are TRUE as default
		m_bAllowSDICashSystems |= dongleOptions.GetValue(_T("AllowFunctionality1"),FALSE);
		m_bAllowSDIAccessControl |= dongleOptions.GetValue(_T("AllowFunctionality2"),FALSE);

		CalcProductFields();	// set IsTransmitter()....

		m_bIsValid = TRUE;
		m_dwHardlockState = STATUS_OK;
	} 
#endif
}
//////////////////////////////////////////////////////////////
void CWK_Dongle::ReadCryptoDongle()
{
	CString sWorkingDirectory,sPath,sTempDirectory;

	GetModuleFileName(NULL,sPath.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();

	int p;

	sWorkingDirectory = sPath;
	p = sWorkingDirectory.ReverseFind('\\');
	if (p!=-1)
	{
		sWorkingDirectory = sWorkingDirectory.Left(p);
	}
	else
	{
		::GetCurrentDirectory(_MAX_PATH,sWorkingDirectory.GetBufferSetLength(_MAX_PATH));
		sWorkingDirectory.ReleaseBuffer();
	}

	CString sDongleCry = sWorkingDirectory + _T("\\dongle.cry");

	// Für DEBUG-Versionen, um nicht überall eine dongle.cry haben zu müssen!!!
#ifdef _DEBUG
	if (!DoesFileExist(sDongleCry))
	{
		sDongleCry.Empty();
		sDongleCry = _T("c:\\security\\dongle.cry");
		if (!DoesFileExist(sDongleCry))
			sDongleCry.Empty();
	}
#endif
	if (!sDongleCry.IsEmpty())
	{
		// decrypt the dongle file to dcf
		CRSA rsa;
		CMemFile dest;
		if (rsa.IsValid())
		{
			if (rsa.Decode(sDongleCry,dest))
			{
				ReadSoftwareDongle(dest);
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot decode Crypto Dongle\n"));
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot init RSA for Crypto Dongle\n"));
		}
	}
	else
	{
		WK_TRACE(_T("no crypto dongle found\n"));
	}
}
//////////////////////////////////////////////////////////////
void CWK_Dongle::ReadSecondCryptoDongle()
{
	CString sWorkingDirectory;
	CString sPath;

	GetModuleFileName(NULL,sPath.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();
	int p;

	sWorkingDirectory = sPath;
	p = sWorkingDirectory.ReverseFind('\\');
	if (p!=-1)
	{
		sWorkingDirectory = sWorkingDirectory.Left(p);
	}
	else
	{
		::GetCurrentDirectory(_MAX_PATH,sWorkingDirectory.GetBufferSetLength(_MAX_PATH));
		sWorkingDirectory.ReleaseBuffer();
	}
	
	CString sDongle2Cry = sWorkingDirectory + _T("\\dongle2.cry");

	// Für DEBUG-Versionen, um nicht überall eine dongle.cry haben zu müssen!!!
#ifdef _DEBUG
	if (!DoesFileExist(sDongle2Cry))
	{
		sDongle2Cry.Empty();
		sDongle2Cry = _T("c:\\security\\dongle2.cry");
		if (!DoesFileExist(sDongle2Cry))
			sDongle2Cry.Empty();
	}
#endif

	if (!sDongle2Cry.IsEmpty())
	{
		// decrypt the dongle file to dcf
		CRSA rsa;
		CMemFile dest;
		if (rsa.IsValid())
		{
			if (rsa.Decode(sDongle2Cry,dest))
			{
				CWKDebugOptions dongleOptions;
				dongleOptions.ReadFromFile(dest);
				DWORD dwMagic = dongleOptions.GetValue(szMagicNumber, 0);
				if (dwMagic == MAGIC_NUMBER) 
				{
					// call the member function via x()
					// convert to string #x
					#define TRANSFER(x) m_b##x |= dongleOptions.GetValue(_T(#x),FALSE); 
					// CAVEAT all Transfers as ONE , so all will get the SAME
					#include "AllDongleTransfers.h"
					#undef TRANSFER
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////
#ifdef _HARDLOCK
void CWK_Dongle::ReadHardwareDongle()
{
#if WK_WITH_DONGLE

	BOOL bHardlockDriverInstalled = FALSE;
	COsVersionInfo os;
	CString sSystemDirectory,sDriverPath;

	GetSystemDirectory(sSystemDirectory.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	sSystemDirectory.ReleaseBuffer();


	if (os.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		sDriverPath = sSystemDirectory + _T("\\drivers\\hardlock.sys");
	}
	else
	{
		sDriverPath = sSystemDirectory + _T("\\hardlock.vxd");
	}
	bHardlockDriverInstalled = DoesFileExist(sDriverPath);

	if (bHardlockDriverInstalled)
	{
		m_dwHardlockState = HL_LOGIN(WK_MODULE_ADDRESS,
			LOCAL_DEVICE,
			(BYTE *)g_szReferenceKey1,
			(BYTE *)g_szVerificationKey1);
	}
	
	if (m_dwHardlockState == STATUS_OK)
	{
		m_wTheModulAddress = WK_MODULE_ADDRESS;
		WK_TRACE(_T("dongle found\n"));
	}

	// wDongleState = HL_AVAIL(); NOT YET, just one login
	if( m_dwHardlockState == STATUS_OK ) 
	{
		m_bHardwareDongleFound = TRUE;

		BYTE eepData[128];
		
		int bitIndex=0;
		int byteIndex=0;

		// reset eepData
		for (byteIndex=0;byteIndex<128;byteIndex++)
		{
			eepData[byteIndex]=0;
		}

		m_dwHardlockState = HL_READBL(eepData);
		if (m_dwHardlockState==STATUS_OK) 
		{
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// CAVEAT this has to stay in sync with the ReadHardwareDongle in the lib
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			byteIndex=0;
			if (eepData[byteIndex++] != 0xAF
				|| eepData[byteIndex++] != 0xFE
				)
			{
				WK_TRACE_ERROR(_T("Dongle ,invalid magic header %x%x\n"),
						eepData[0] ,
						eepData[1] 
				);
				m_bIsValid=FALSE;
				HL_LOGOUT();			
				m_bHardwareDongleFound =FALSE;
			} 
			else 
			{	// magic header found !
				m_bIsValid=TRUE;
				m_bHardwareDongleFound = TRUE;
				m_wDongleFormat = eepData[byteIndex++];

				int iTmp=0;
				int iTmp2=0;
				// CAVEAt += give a lot of data loss compiler crap
				// thus instead of a+=(WORD)... a=(WORD)(a+....)
				// better use iTmp

				// major part
				iTmp = ((WORD)eepData[byteIndex++])<<8;
				iTmp += eepData[byteIndex++];
				m_bMajorVersion = (BYTE)iTmp;
				iTmp *= 1000;

				// minor part
				iTmp2 = ((WORD)eepData[byteIndex++])<<8;
				iTmp2 += eepData[byteIndex++];
				m_bMinorVersion = (BYTE)iTmp2;

				iTmp = ((WORD)eepData[byteIndex++])<<8;
				iTmp += eepData[byteIndex++];
				m_wOEMCode = (WORD) iTmp;

				iTmp = ((WORD)eepData[byteIndex++])<<8;
				iTmp += eepData[byteIndex++];
				m_productCode = (InternalProductCode) iTmp;

			// call the member function via x()
			// convert to string #x
#define TRANSFER(x) \
					m_b##x = (eepData[byteIndex] & (1 << bitIndex))!=0; \
					bitIndex++; \
					if (bitIndex>7) { \
						bitIndex=0; \
						byteIndex++; \
						if (byteIndex>96) { \
							EEPIndexError(byteIndex); \
							byteIndex=0; \
						} \
					}

#include "AllDongleTransfers.h"

#undef TRANSFER
			}	// end of correct magic header

			HL_LOGOUT();
		
			///////////////////////////////////////////////
			{	// begin of secondary
				// local variants of m_XXX
				DWORD dwHardlockState = HL_LOGIN((WORD)(m_wTheModulAddress+1),
												(WORD)LOCAL_DEVICE,
												(BYTE *)g_szReferenceKey1,		  
												(BYTE *)g_szVerificationKey1
												);

	// wDongleState = HL_AVAIL(); NOT YET, just one login
	if( dwHardlockState == STATUS_OK ) 
	{
		WK_TRACE(_T("Reading secondary hardlock....\n"));
		BYTE eepData[128];
		
		int bitIndex=0;
		int byteIndex=0;

		for (byteIndex=0;byteIndex<128;byteIndex++) 
		{
			eepData[byteIndex]=0;
		}

		dwHardlockState = HL_READBL(eepData);
		if (dwHardlockState==STATUS_OK) 
		{
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// CAVEAT this has to stay in sync with the ReadHardwareDongle in the lib
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			byteIndex=0;
			if (eepData[byteIndex++] != 0xAF
				|| eepData[byteIndex++] != 0xFE
				)
			{
				WK_TRACE_ERROR(_T("Dongle ,invalid magic header %x%x\n"),
						eepData[0] ,
						eepData[1] 
				);
				HL_LOGOUT();			
			}
			else
			{	// magic header found !
				// UNUSED WORD wDongleFormat = eepData[byteIndex++];

				int iTmp=0;
				// CAVEAt += give a lot of data loss compiler crap
				// thus instead of a+=(WORD)... a=(WORD)(a+....)
				// better use iTmp

				// major part
				iTmp = ((WORD)eepData[byteIndex++])<<8;
				iTmp += eepData[byteIndex++];
				iTmp *= 1000;

				// minor part
				iTmp += ((WORD)eepData[byteIndex++])<<8;
				iTmp += eepData[byteIndex++];
//				WORD wVersionNumber = (WORD) iTmp;

				iTmp = ((WORD)eepData[byteIndex++])<<8;
				iTmp += eepData[byteIndex++];
				// UNUSED WORD wOEMCode = (WORD) iTmp;

				iTmp = ((WORD)eepData[byteIndex++])<<8;
				iTmp += eepData[byteIndex++];
				// UNUSED InternalProductCode productCode = (InternalProductCode) iTmp;

			// call the member function via x()
			// convert to string #x
			// CAVEAT the secondary dongle has to modifiy the shared memory!
#define TRANSFER(x) \
				if (m_b##x==FALSE && (eepData[byteIndex] & (1 << bitIndex))!=0) { \
					m_b##x |= 1; \
					OptionalMessage(_T(#x),((eepData[byteIndex] & (1 << bitIndex))!=0)); \
					if (m_pSharedMemory) { \
						m_pSharedMemory[byteIndex] |= (1 << bitIndex); \
					}	\
				} \
					bitIndex++; \
					if (bitIndex>7) { \
						bitIndex=0; \
						byteIndex++; \
						if (byteIndex>96) { \
							EEPIndexError(byteIndex); \
							byteIndex=0; \
						} \
					}

#include "AllDongleTransfers.h"

#undef TRANSFER
		}	// end of correct magic header, secondary hardlock

		HL_LOGOUT();

	}
	else
	{
		// could not read second memory block
		WK_TRACE(_T("Could not read secondary memory %s\n"),NameOfDongleState(dwHardlockState));
	}

	} else {
		// no secondary found, no need for a message
	}
			}	// end of secondary
			///////////////////////////////////
			CalcProductFields();
		}
		else 
		{
			WK_TRACE_ERROR(_T("Could not read dongle memory: %s\n"),
				NameOfDongleState(m_dwHardlockState)
				);
			m_bIsValid=FALSE;
			HL_LOGOUT();			
			m_bHardwareDongleFound =FALSE;
		}

		UpdateVersionNumber();	// calc the version number depending from the BuildNumber
	} 
	else 
	{
		TRACE(_T("NO HardwareDongle found [%s]\n"), NameOfDongleState(m_dwHardlockState));
		m_bHardwareDongleFound = FALSE;
		m_bIsValid=FALSE;
	}
#endif
}
#endif
////////////////////////////////////////////////////////////////////
void CWK_Dongle::CalcProductFields()
{
	switch (m_wOEMCode) 
	{
		case OEM_IDIP:	
		case OEM_SANTEC:
		case OEM_VDR:
		case OEM_PROTECTION_ONE:
		case OEM_DRESEARCH:
		case OEM_DTS:
		case OEM_SIEMENS:
		case OEM_ALARMCOM:
		case OEM_AKUBIS:	// NOT YET own stuff
			switch (m_productCode) 
			{
			case IPC_FREE_STYLE:	// OOPS
				m_bIsLocal=FALSE;
				m_bIsTransmitter=TRUE;
				m_bIsReceiver=FALSE;
				break;
			case IPC_STORAGE:
			case IPC_STORAGE_PLUS:
				m_bIsLocal=TRUE;
				m_bIsTransmitter=FALSE;
				m_bIsReceiver=FALSE;
				break;
	
			case IPC_STORAGE_TRANSMITTER:
			case IPC_TOBS_TO3K:
				m_bIsLocal=FALSE;
				m_bIsTransmitter=FALSE;
				m_bIsReceiver=TRUE;
				break;
			case IPC_INSPICIO:
			case IPC_DTS_RECEIVER:
			case IPC_DTS_IP_RECEIVER:
			case IPC_INSPECTUS:
				m_bIsLocal=FALSE;
				m_bIsTransmitter=FALSE;
				m_bIsReceiver=TRUE;
				break;
			case IPC_DTS:
				m_bIsLocal=TRUE;
				m_bIsTransmitter=TRUE;
				m_bIsReceiver=FALSE;
				break;
			case IPC_TOBS_RECEIVER:
				m_bIsLocal=FALSE;
				m_bIsTransmitter=FALSE;
				m_bIsReceiver=TRUE;
				break;
			case IPC_VIDEO_MASTER:
				m_bIsLocal=TRUE;
				m_bIsTransmitter=FALSE;
				m_bIsReceiver=FALSE;
				break;
			case IPC_ATM_SURVEILENCE:
				m_bIsLocal=TRUE;
				m_bIsTransmitter=FALSE;
				m_bIsReceiver=FALSE;
				break;
			case IPC_IDIP_4:
			case IPC_IDIP_8:
			case IPC_IDIP_16:
			case IPC_IDIP_24:
			case IPC_IDIP_32:
			case IPC_IDIP_PROJECT:
			case IPC_IDIP_XL_4:
			case IPC_IDIP_XL_8:
			case IPC_IDIP_XL_16:
			case IPC_IDIP_XL_24:
			case IPC_IDIP_XL_32:
			case IPC_IDIP_ML:
			case IPC_IDIP_IP:
				m_bIsLocal=TRUE;
				m_bIsTransmitter=TRUE;
				m_bIsReceiver=FALSE;
				break;
			case IPC_TO3K_DEV:
				m_bIsLocal = FALSE;
				m_bIsTransmitter = TRUE;
				m_bIsReceiver = FALSE;
			default:
				m_bIsLocal=TRUE;
				m_bIsTransmitter=FALSE;
				m_bIsReceiver=FALSE;
				WK_TRACE_ERROR(_T("Invalid product code %d\n"),m_productCode);
			}	// end of productLine;
			break;
		default:
			WK_TRACE_ERROR(_T("Unsupported OEM %s(%d( in dongle\n"),
				NameOfEnum((OemCode)m_wOEMCode),
				m_wOEMCode
				);
	}	// end of OEM switch
}
////////////////////////////////////////////////////////////////////
const _TCHAR *CWK_Dongle::NameOfDongleState(DWORD dwState)
{
	switch (dwState) 
	{
	case  STATUS_OK: return _T("Okay"); break;
	case  NOT_INIT: return _T("Not initialized"); break;
	case  ALREADY_INIT: return _T("Already initialized"); break;
	case  UNKNOWN_DONGLE: return _T("Unknown dongle"); break;
	case  UNKNOWN_FUNCTION: return _T("Unknown function"); break;
	case  HLS_FULL: return _T("Server full"); break;
	case  NO_DONGLE: return _T("No dongle"); break;
	case  NETWORK_ERROR: return _T("Network error"); break;
	case  NO_ACCESS: return _T("No access"); break;        
	case  INVALID_PARAM: return _T("Invalid parameter"); break;        
	case  VERSION_MISMATCH: return _T("Version mismatch"); break;        
	case  DOS_ALLOC_ERROR: return _T("DOS allocation error"); break;        
	case  CANNOT_OPEN_DRIVER: return _T("Can not open driver"); break;        
	case  INVALID_ENV: return _T("Invalid environment"); break;        
	case  DYNALINK_FAILED: return _T("DynaLink failed"); break;        
	case  INVALID_LIC: return _T("Invalid licence"); break;        
	case  NO_LICENSE: return _T("No licence"); break;        
	case  TOO_MANY_USERS: return _T("Too many users"); break;
	case  SELECT_DOWN: return _T("Printer not online"); break;
	}	// end of switch
	return _T("Invalid state");	
}
////////////////////////////////////////////////////////////////////
CString CWK_Dongle::GetVersionString() const
{
	return m_sVersion;
}
/////////////////////////////////////////////////////////////////////////
void CWK_Dongle::LoadFromCFG(const CString &sCFGFile)
{
	ReadSoftwareDongle(sCFGFile);
	// no return value, do an IsValid() check afterwards
}
/////////////////////////////////////////////////////////////////////////
BOOL CWK_Dongle::Load(CWK_Profile& wkp)
{
	m_bIsValid = wkp.GetInt(szTheDongleSection, _T("DongleFound"), FALSE);

	if (IsValid()) 
	{
		m_sVersion = wkp.GetString(_T("Version"), _T("Number"), _T("4.2"));
		m_productCode = (InternalProductCode)wkp.GetInt(szTheDongleSection,
			_T("ProductCode"), IPC_MIN_CODE);
		m_wOEMCode = (WORD)wkp.GetInt(szTheDongleSection, _T("OEMCode"), (WORD)OEM_INVALID);

		// CAVEAT, struct alignment is important here !!!
		// # is the preprocessor tokenizer /  "stringizing" operator

	#define TRANSFER(x) m_b##x = wkp.GetInt(szTheDongleSection,_T(#x),FALSE);

	// CAVEAT all Transfers as ONE, so all will get the SAME
	#include "AllDongleTransfers.h"

	TRANSFER(IsTransmitter);
	TRANSFER(IsReceiver);
	TRANSFER(IsLocal);
	#undef TRANSFER
#ifndef _HARDLOCK
	m_iNrOfHosts = wkp.GetInt(szTheDongleSection,_T("NrOfHosts"),0);
	m_iNrOfCameras = wkp.GetInt(szTheDongleSection,_T("NrOfCameras"),0);
	m_iNrOfIPCameras = wkp.GetInt(szTheDongleSection,_T("NrOfIPCameras"),0);
#endif

	} else {
		// invalid dongle
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////
BOOL CWK_Dongle::Save(CWK_Profile& wkp) const
{
	// UNUSED BOOL bDeleteOkay = 	
	wkp.DeleteSection(szTheDongleSection);
	/* NOT YET
	// OOPS seems always to be FALSE
	if (bDeleteOkay==FALSE) {

		WK_TRACE_ERROR(_T("Could not clear the registry section %s\n"),szTheDongleSection);
	}
	*/

	BOOL bValue=FALSE;

	// UNUSED BOOL bIsValid = IsValid();

	wkp.WriteInt(szTheDongleSection, _T("DongleFound"), (int) IsValid());

	if (IsValid()) {
		wkp.WriteString(szTheDongleSection, _T("DongleState"),	_T("Okay"));

		/*
		wkp.WriteInt(szTheDongleSection,_T("VersionNumber"),m_wVersionNumber);
		wkp.WriteInt(szTheDongleSection,_T("MajorVersionNumber"),GetMajorVersionNumber());
		wkp.WriteInt(szTheDongleSection,_T("MinorVersionNumber"),GetMinorVersionNumber());
		*/

		wkp.WriteInt(szTheDongleSection, _T("ProductCode"), GetProductCode());
		wkp.WriteInt(szTheDongleSection, _T("OEMCode"),     (WORD)GetOemCode());

		// CAVEAT, struct alignment is important here !!!
		// # is the preprocessor tokenizer /  "stringizing" operator

	#define TRANSFER(x) \
		bValue = x(); \
		wkp.WriteInt(szTheDongleSection,_T(#x),bValue);

	// CAVEAT all Transfers as ONE, so all will get the SAME
#include "AllDongleTransfers.h"

			TRANSFER(IsTransmitter);
			TRANSFER(IsReceiver);
			TRANSFER(IsLocal);
#undef TRANSFER
#ifndef _HARDLOCK
		wkp.WriteInt(szTheDongleSection,_T("NrOfHosts"),m_iNrOfHosts);
		wkp.WriteInt(szTheDongleSection,_T("NrOfCameras"),m_iNrOfCameras);
		wkp.WriteInt(szTheDongleSection,_T("NrOfIPCameras"),m_iNrOfIPCameras);
#endif

	} else {
		// invalid dongle
		wkp.WriteString(szTheDongleSection, _T("DongleState"),
				CWK_Dongle::NameOfDongleState( GetHardlockState() )
		);
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////
void CWK_Dongle::FillSharedMemory()
{
#ifndef NO_WK_DONGLE
	// 0..127 is the plain eep date
	// 128 Version id
	// 129..129+4 is dwState
	// ..200 for future use

	PRE_NOT_NULL(m_pSharedMemory);

	// NOT YET any validations
	int bitIndex=0;
	int byteIndex=0;

	// Affe
	m_pSharedMemory[byteIndex++] = 0xAF;
	m_pSharedMemory[byteIndex++] = 0xFE;

	// Dongleformat
	m_pSharedMemory[byteIndex++] = (BYTE)m_wDongleFormat;

	// Version
	m_pSharedMemory[byteIndex++] = 4; 
	m_pSharedMemory[byteIndex++] = 0;
	m_pSharedMemory[byteIndex++] = 0;
	m_pSharedMemory[byteIndex++] = 0;
	
	// OEM-Code
	m_pSharedMemory[byteIndex++] = HIBYTE(m_wOEMCode);
	m_pSharedMemory[byteIndex++] = LOBYTE(m_wOEMCode);

	// Product-Code
	m_pSharedMemory[byteIndex++] = HIBYTE(m_productCode);
	m_pSharedMemory[byteIndex++] = LOBYTE(m_productCode);
//	WK_TRACE(_T("FillSharedMemory %d\n"),(int)m_productCode);

#define TRANSFER(x) \
	if (m_b##x) \
	{ \
		m_pSharedMemory[byteIndex] |= (1 << bitIndex); \
	} \
	bitIndex++; \
	if (bitIndex>7) \
	{ \
		bitIndex=0; \
		byteIndex++; \
		if (byteIndex>96) \
		{ \
			EEPIndexError(byteIndex); \
			byteIndex=0; \
		} \
	}

#include "AllDongleTransfers.h"

	// write Nr Of Hosts in 2 bytes
#ifndef _HARDLOCK
	WORD wNrOfHosts = (WORD)m_iNrOfHosts;
	byteIndex++;
	if (byteIndex>96)
	{
		EEPIndexError(byteIndex);
		byteIndex=0;
	}
	m_pSharedMemory[byteIndex] = LOBYTE(wNrOfHosts);
	byteIndex++;
	if (byteIndex>96)
	{
		EEPIndexError(byteIndex);
		byteIndex=0;
	}
	m_pSharedMemory[byteIndex] = HIBYTE(wNrOfHosts);

	BYTE bNrOfCameras = (BYTE)m_iNrOfCameras;
	byteIndex++;
	if (byteIndex>96)
	{
		EEPIndexError(byteIndex);
		byteIndex=0;
	}
	m_pSharedMemory[byteIndex] = bNrOfCameras;

	BYTE bNrOfIPCameras = (BYTE)m_iNrOfIPCameras;
	byteIndex++;
	if (byteIndex>96)
	{
		EEPIndexError(byteIndex);
		byteIndex=0;
	}
	m_pSharedMemory[byteIndex] = bNrOfIPCameras;

#endif

#undef TRANSFER

	// some ReadHardwareDongle has been made before
	m_pSharedMemory[iOffsetVersion]			= 0x10;
	m_pSharedMemory[iOffsetHardwareState]	= (BYTE)(m_dwHardlockState & 255);
	m_pSharedMemory[iOffsetHardwareState+1] = (BYTE)((m_dwHardlockState>>8) & 255);
	m_pSharedMemory[iOffsetHardwareState+2] = (BYTE)((m_dwHardlockState>>16) & 255);
	m_pSharedMemory[iOffsetHardwareState+3] = (BYTE)((m_dwHardlockState>>24) & 255);
#endif
}
////////////////////////////////////////////////////////////////////
void CWK_Dongle::ReadSharedMemory()
{
#ifndef NO_WK_DONGLE

	if (m_pSharedMemory==NULL) 
	{
		WK_TRACE_ERROR(_T("Could not read hardlock shared memory\n"));
		Read(TRUE);
		return;	// <<< EXIT >>>
	}
	const BYTE *eepData = (const BYTE *)m_pSharedMemory;

	// UNUSED BYTE bySharedMemoryVersion = eepData[iOffsetVersion];	// NOT YET used

	m_dwHardlockState =    eepData[iOffsetHardwareState]
						| (eepData[iOffsetHardwareState+1]<<8)
						| (eepData[iOffsetHardwareState+2]<<8)
						| (eepData[iOffsetHardwareState+3]<<8);

	if (m_dwHardlockState == STATUS_OK ) {
		m_bHardwareDongleFound = TRUE;
		
		int bitIndex=0;
		int byteIndex=0;

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// CAVEAT this has to stay in sync with the ReadHardwareDongle in the lib
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		byteIndex=0;
		if (eepData[byteIndex++] != 0xAF
			|| eepData[byteIndex++] != 0xFE
			)
		{
			WK_TRACE_ERROR(_T("Dongle ,invalid magic header %x%x\n"),
					eepData[0] ,
					eepData[1] 
			);
			m_bIsValid=FALSE;
			m_bHardwareDongleFound =FALSE;
		} else {	// magic header found !
			m_bIsValid=TRUE;
			m_bHardwareDongleFound = TRUE;
			m_wDongleFormat = eepData[byteIndex++];

			int iTmp=0;
			// CAVEAt += give a lot of data loss compiler crap
			// thus instead of a+=(WORD)... a=(WORD)(a+....)
			// better use iTmp

			// major part
			iTmp = ((WORD)eepData[byteIndex++])<<8;
			iTmp += eepData[byteIndex++];
			iTmp *= 1000;

			// minor part
			iTmp += ((WORD)eepData[byteIndex++])<<8;
			iTmp += eepData[byteIndex++];
			// UNUSED WORD wVersionNumber = (WORD) iTmp;

			iTmp = ((WORD)eepData[byteIndex++])<<8;
			iTmp += eepData[byteIndex++];
			m_wOEMCode = (WORD) iTmp;

			iTmp = ((WORD)eepData[byteIndex++])<<8;
			iTmp += eepData[byteIndex++];
			m_productCode = (InternalProductCode) iTmp;
//			WK_TRACE(_T("ReadSharedMemory %d\n"), (int)m_productCode);

		// call the member function via x()
		// convert to string #x
#define TRANSFER(x) \
				m_b##x = (eepData[byteIndex] & (1 << bitIndex))!=0; \
				bitIndex++; \
				if (bitIndex>7) { \
					bitIndex=0; \
					byteIndex++; \
					if (byteIndex>96) { \
						EEPIndexError(byteIndex); \
						byteIndex=0; \
					} \
				}

#include "AllDongleTransfers.h"
#undef TRANSFER
			// write Nr Of Hosts in 2 bytes
#ifndef _HARDLOCK
			BYTE bLowNrOfHosts,bHighNrOfHosts;
			byteIndex++;
			bLowNrOfHosts = eepData[byteIndex];
			byteIndex++;
			bHighNrOfHosts = eepData[byteIndex];
			m_iNrOfHosts = MAKEWORD(bLowNrOfHosts,bHighNrOfHosts);

			BYTE bNrOfCameras;
			byteIndex++;
			bNrOfCameras = eepData[byteIndex];
			m_iNrOfCameras = bNrOfCameras;

			BYTE bNrOfIPCameras;
			byteIndex++;
			bNrOfIPCameras = eepData[byteIndex];
			m_iNrOfIPCameras = bNrOfIPCameras;
#endif


		}	// end of correct magic header
		CalcProductFields();
	} 
	else 
	{
		WK_TRACE_ERROR(_T("Read dongle shared memory not STATUS_OK\n"));
	}
// </NO_WK_DONGLE>
#endif
	
}	// end of ReadSharedMemory
#ifdef _HARDLOCK
//////////////////////////////////////////////////////
char *CWK_Dongle::Crypt(const char* pData, DWORD dwDataLen,
						BOOL & bOkay,
						DWORD & dwResultLen)
{
#ifndef NO_WK_DONGLE
	char *pResult = NULL;
	bOkay = FALSE;
	dwResultLen = 0;

	if (pData==NULL || dwDataLen==0)
	{
		return NULL;	// <<< EXIT >>>
	}

	CMutex mutex(FALSE,szTheHardlockMutex);
	CSingleLock lock(&mutex);
	if (lock.Lock(5000))
	{
		DWORD dwHardlockState = HL_LOGIN( 
			m_wTheModulAddress,
			LOCAL_DEVICE,
			(BYTE *)g_szReferenceKey1,
			(BYTE *)g_szVerificationKey1
		);
		if( dwHardlockState == STATUS_OK )
		{
			// leading magic byte
			// following DWORD contains the data len
			DWORD dwFullLen = dwDataLen+4+1;

			// the HL_CODE() has 8 byte alignment!
			if (dwFullLen%8)
			{
				dwFullLen += (8- (dwFullLen % 8));
			}
#ifdef _UNICODE
//			ASSERT(FALSE);
#endif			
			dwResultLen = dwFullLen;
			pResult = new char[dwFullLen];
			FillMemory(pResult,dwFullLen,0);

			pResult[0]= (BYTE)0xAB;	// magic byte
			pResult[1] = LOBYTE(LOWORD(dwDataLen));
			pResult[2] = HIBYTE(LOWORD(dwDataLen));
			pResult[3] = LOBYTE(HIWORD(dwDataLen));
			pResult[4] = HIBYTE(HIWORD(dwDataLen));
			// and the data itself
			CopyMemory(pResult+5,pData,dwDataLen);

			// crypt the data in 8192 chunks
			bOkay = TRUE;
			for (int i=0; i<(int)dwFullLen && bOkay;i += 8192)
			{
				WORD wLen;
				if ((i % 8192)==0)
				{
					wLen = (WORD)(i%8192);
				} 
				else
				{
					wLen = 8192;
				}
				dwHardlockState = HL_CODE(pResult + i,wLen);

				if( dwHardlockState == STATUS_OK )
				{
					bOkay = TRUE;
				}
				else 
				{
					WK_TRACE_ERROR(_T("Crypt error\n"));
					bOkay = FALSE;
				}
			} // end of crypt loop
		}
		else 
		{
			WK_TRACE_ERROR(_T("Failed to access hardlock.\n"));
		}
		HL_LOGOUT();			
		lock.Unlock();
	}
	else 
	{
		WK_TRACE_ERROR(_T("Failed to lock HardLock\n"));
	}

	return pResult;
#else
	//<NO_WK_DONGLE>
	return NULL;
	//</NO_WK_DONGLE>
#endif
}
////////////////////////////////////////////////////////////////////
char *CWK_Dongle::Decrypt(const char* pData, DWORD dwDataLen, 
						  BOOL &bOkay,
						  DWORD &dwResultLen)
{
#ifndef NO_WK_DONGLE
	char *pResult = NULL;
	char *pTmp = NULL;

	bOkay = FALSE;
	
	if (pData==NULL || dwDataLen==0)
	{
		return NULL;	// <<< EXIT >>>
	}

	CMutex mutex(FALSE,szTheHardlockMutex);
	CSingleLock lock(&mutex);
	if (lock.Lock(5000))
	{
		DWORD dwHardlockState = HL_LOGIN( 
			m_wTheModulAddress,
			LOCAL_DEVICE,
			(BYTE *)g_szReferenceKey1,
			(BYTE *)g_szVerificationKey1
		);
		if( dwHardlockState == STATUS_OK )
		{
			// the HL_CODE is modulo 8!
			// leading magic byte
			// following DWORD contains the data len
			DWORD dwFullLen = dwDataLen;
			if (dwDataLen%8)
			{
				// input is the result from ::Crypt so it has to be 8 byte aligned
				WK_TRACE_ERROR(_T("Invalid decrypt data\n"));
			}
#ifdef _UNICODE
//			ASSERT(FALSE);
#endif			
			dwResultLen = dwFullLen;
			pTmp = new char[dwFullLen];
			CopyMemory(pTmp,pData,dwFullLen);

			// decrypt the data in 8192 chunks
			bOkay = TRUE;
			for (int i=0; i<(int)dwFullLen && bOkay;i += 8192)
			{
				WORD wLen;
				if ((i % 8192)==0)
				{
					wLen = (WORD)(i%8192);
				}
				else
				{
					wLen = 8192;
				}
				
				dwHardlockState = HL_CODE(pTmp + i,wLen);
				
				if( dwHardlockState == STATUS_OK )
				{
					bOkay = TRUE;
				}
				else 
				{
					bOkay = FALSE;
					WK_TRACE_ERROR(_T("Decode error\n"));
				}
			}	// end of decrypt loop
		} 
		else 
		{
			WK_TRACE_ERROR(_T("Failed to access hardlock.\n"));
		}
		HL_LOGOUT();			
		lock.Unlock();
	}
	else
	{
		WK_TRACE_ERROR(_T("Failed to lock HardLock\n"));
	}

	if (bOkay)
	{
		// read the real data length from the decrypted data
		// and restore the real data, not aligned to 8 byte boundary
		// NOT YET check magic byte 0xAB
		dwResultLen = 
			pTmp[1]
			| (pTmp[2]<<8)
			| (pTmp[3]<<16)
			| (pTmp[4]<<24);
		if (dwResultLen <= dwDataLen)
		{
			pResult = new char[dwResultLen];
			CopyMemory(pResult,pTmp+5,dwResultLen);
		}
		else
		{
			WK_TRACE_ERROR(_T("Decrypt length error\n"));
		}
	}
	else 
	{
		WK_TRACE_ERROR(_T("Unable to adjust length\n"));
	}

	WK_DELETE_ARRAY(pTmp);

	return pResult;
#else
	// <NO_WK_DONGLE>
	return NULL;
	// </NO_WK_DONGLE>
#endif
}
#endif
////////////////////////////////////////////////////////////////////
const _TCHAR* NameOfEnum(CWK_Dongle::OemCode x)
{
	switch (x) 
	{
		NAME_OF_ENUM(CWK_Dongle::OEM_IDIP);
		NAME_OF_ENUM(CWK_Dongle::OEM_SANTEC);
		NAME_OF_ENUM(CWK_Dongle::OEM_AKUBIS);
		NAME_OF_ENUM(CWK_Dongle::OEM_VDR);
		NAME_OF_ENUM(CWK_Dongle::OEM_DRESEARCH);
		NAME_OF_ENUM(CWK_Dongle::OEM_DTS);
		NAME_OF_ENUM(CWK_Dongle::OEM_PROTECTION_ONE);
		NAME_OF_ENUM(CWK_Dongle::OEM_SIEMENS);
		NAME_OF_ENUM(CWK_Dongle::OEM_ALARMCOM);
		default:
			return _T("INVALID OEM");
	}
}
////////////////////////////////////////////////////////////////////
// the version number stored in the dongle, might not
// be the real installed version
// vinfo.pvi contains the BuildNumber in the first line
// thus if vinfo.pvi is found the version number is calculated 
// depending from the BuildNumber
// return TRUE if vinfo.pvi was found
BOOL CWK_Dongle::UpdateVersionNumber()
{
	// the code to read vinfo.pvi is copied from ProductVersion, sigh
	BOOL bOkay = FALSE;

	CString sDatabase;//(_T("vinfo.pvi"));
	_TCHAR szModule[_MAX_PATH];
	int p;

	GetModuleFileName(NULL,szModule,sizeof(szModule));
	sDatabase = szModule;
	p = sDatabase.ReverseFind('\\');
	sDatabase = sDatabase.Left(p+1);
	sDatabase += _T("vinfo.pvi");

#ifdef _UNICODE
	CStdioFileU fDatabase;
#else
	CStdioFile fDatabase;
#endif

	if (fDatabase.Open(sDatabase,CFile::modeRead)) {
		bOkay = TRUE;
		CString sHead;
		// UNUSED DWORD dwCheckSum=0;
		// version buildNumber number of records

		TRY
		{
			fDatabase.ReadString(sHead );
			int iVersion=0,iBuild=0,iNumber=0;
			int iNumRead=0;
			iNumRead=_stscanf(sHead,_T("%d %d %d\n"),&iVersion,&iBuild,&iNumber);
			if (iNumRead!=3)
			{
				// invalid header
				WK_TRACE_ERROR(_T("Invalid header in %s\n"), LPCTSTR(sDatabase));
				bOkay = FALSE;
			}

			fDatabase.Close();
		}
		WK_CATCH(_T("vinfo.pvi reading failed"))
	}
	else
	{
#ifndef _DEBUG	
		_TCHAR szDirName[MAX_PATH+1];
		szDirName[0]=0;
		GetCurrentDirectory(MAX_PATH,szDirName);
		CString sTmpDir(szDirName);
		sTmpDir.MakeLower();
		// only complain in non-Project directories
		if (sTmpDir.Find(_T("project"))==-1)
		{
			WK_TRACE_ERROR(_T("Did not find file '%s' in '%s'\n"),
				LPCTSTR(sDatabase),
				szDirName
				);
		}
#endif
		bOkay = FALSE;
	}

	// adjust the shared memory, sigh that's ugly
	if (m_pSharedMemory)
	{
		// order is highbyte lowbytes see ReadHardware
		m_pSharedMemory[iEEPOffsetVersionMinor]=(BYTE)((WK_VERSION_NUMBER % 1000)>>8);
		m_pSharedMemory[iEEPOffsetVersionMinor+1]=(BYTE)((WK_VERSION_NUMBER % 1000)&255);
		m_pSharedMemory[iEEPOffsetVersionMajor]=(BYTE)((WK_VERSION_NUMBER / 1000)>>8);
		m_pSharedMemory[iEEPOffsetVersionMajor+1]=(BYTE)((WK_VERSION_NUMBER / 1000)&255);
	}

	return bOkay;
}

