/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WK_Dongle.h $
// ARCHIVE:		$Archive: /Project/CIPC/WK_Dongle.h $
// CHECKIN:		$Date: 26.07.06 11:05 $
// VERSION:		$Revision: 123 $
// LAST CHANGE:	$Modtime: 26.07.06 10:59 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
#ifndef __CWK_Dongle_H
#define __CWK_Dongle_H

#ifndef NO_WK_DONGLE

#include "wkclasses\WK_RunTimeError.h"
#include "wkclasses\WK_Profile.h"

#else
// <NO_WK_DONGLE>
// CAVEAT direct type declaration, only for the NO_WK_DONGLE part!
typedef WK_ApplicationId int;

enum WK_RunTimeError
{
		RTE_INVALID = 0
};
// </NO_WK_DONGLE>
#endif

#if _MFC_VER < 0x700
	#define _HARDLOCK
#endif

/////////////////////////////////////////////////////////////////////////////
// InternalProductCode | The product code for the software
// CAVEAT: Is stored in dongle file, never change values!
enum InternalProductCode 
{
	// invalid value
	IPC_MIN_CODE			= 0,
	// free style product
	IPC_FREE_STYLE			= 0,
	// idip recorder, MiCo or JaCob
	IPC_STORAGE				= 1,
	// idip recorder, MiCo or JaCob plus
	IPC_STORAGE_PLUS		= 2,
	// idip recorder, MiCo or JaCob , TCP or ISDN
	IPC_STORAGE_TRANSMITTER	= 4,
	// idip recorder and livecam receiver, H.263 and JPEG
	IPC_INSPICIO=8,
	// idip receiver for all
	IPC_INSPECTUS	= 12,
	// 4 jacobs
	IPC_VIDEO_MASTER		= 13,
	// for atm's in banks
	IPC_ATM_SURVEILENCE		= 14,
	// for TeleObserver only receiver
	IPC_TOBS_RECEIVER		= 15,
	// for DTS only receiver
	IPC_DTS_RECEIVER		= 16,
	// for DTS transmitter
	IPC_DTS					= 17,
	// for DTS IP receiver
	IPC_DTS_IP_RECEIVER		= 18,
	// for TO 3000 
	IPC_TOBS_TO3K			= 19,
	// for idip 8
	IPC_IDIP_8				= 20,
	// for idip 16
	IPC_IDIP_16				= 21,
	// for idip 32
	IPC_IDIP_32				= 22,
	// for idip project
	IPC_IDIP_PROJECT		= 23,
	// for idip 4
	IPC_IDIP_4				= 24,
	// for TeleObserver 3000 Device
	IPC_TO3K_DEV			= 25,
	// for idip 24
	IPC_IDIP_24				= 26,

	// for idip XL 4
	IPC_IDIP_XL_4			= 27,
	// for idip XL 8
	IPC_IDIP_XL_8			= 28,
	// for idip XL 16
	IPC_IDIP_XL_16			= 29,
	// for idip XL 24
	IPC_IDIP_XL_24			= 30,
	// for idip XL 32
	IPC_IDIP_XL_32			= 31,

	// for idip ML
	IPC_IDIP_ML				= 32,
	// for idip IP
	IPC_IDIP_IP				= 33,

	// CAVEAT: Is stored in dongle file, never change values!
	IPC_MAX_CODE			= 33,

	// VideoSave products
	IPC_VS_MINI_SB			= 1,
	IPC_VS_MINI_ORUEA		= 2,
	IPC_VS_MINI_SB_ORUEA	= 3,
	IPC_VS_SB8				= 4,
	IPC_VS_ORUEA8			= 5,
	IPC_VS_SB_ORUEA8		= 6,
	IPC_VS_AWP				= 7,
	IPC_VS_AWP_CONFIG		= 8,

	IPC_VS_MAX_CODE			= 8
};

//////////////////////////////////////////////////////////////
/*  CWK_Dongle | The class to define several software
products on one base. Each module and some functionalities
must be allowed by the dongle. The first system wide instance copies
all dongle values into a shared memory area as cache. All
other dongle instances in all processes will read from
this cache. A software dongle is available too.
*/
class AFX_EXT_CLASS CWK_Dongle 
{
public:
	// enumeration of software OEM's in CWK_Dongle scope!
	// CAVEAT: Is saved in oem.ini for read-only software, do not change values!
	enum OemCode 
	{
		// invalid OEM code
		OEM_INVALID=0,
		// the idip OEM
		OEM_IDIP=1,
//		OEM_GARNY=2,   // never use OEM value 2 again
		// Santec OEM, formerly unused German Parcel OEM
		OEM_SANTEC=3,
		// Akubis OEM for Deutsche Telekom
		OEM_AKUBIS=4,
		// VDR OEM for Alarmcom DTS receiver software
		OEM_VDR=5,
		// DResearch OEM for idip and DTS receiver software
		OEM_DRESEARCH=6,
		// DTS OEM for DTS receiver software
		OEM_DTS=7,
		// ProtectionOne OEM for DTS and DTS receiver software
		OEM_PROTECTION_ONE=8,
		// Siemens OEM for Vidis Mobile software
		OEM_SIEMENS=9,
		// Alarmcom OEM for idip software
		OEM_ALARMCOM=10,
		OEM_INVALID_MAX=11
	// CAVEAT: Is saved in oem.ini for read-only software, do not change values!
	// CAVEAT: Also edit NameOfEnum(CWK_Dongle::OemCode code)
	};

	//  construction/destruction
public:
	//!ic! constructor I
	CWK_Dongle(BOOL bIngoreSharedMemory=FALSE);
	//!ic! constructor II checks for exe automatically
	CWK_Dongle(WK_ApplicationId appId, BOOL bIngoreSharedMemory=FALSE);
	//!ic! constructor III from file
	CWK_Dongle(const CString &sFileName);
	//!ic! destructor
	virtual ~CWK_Dongle();

	//  attributes
public:
		   BOOL IsValid() const;
	inline DWORD GetHardlockState() const;
	// <><><><> product <><><><>
	CString GetVersionString() const;

	inline BYTE GetMajorVersion() const;
	inline BYTE GetMinorVersion() const;

	//
	inline InternalProductCode GetProductCode() const;
	// OEM Code
	inline OemCode GetOemCode() const;
	//
	inline BOOL IsTransmitter() const;
	inline BOOL IsReceiver() const;
	inline BOOL IsLocal() const;

	// base
	inline BOOL RunLauncher() const;
	inline BOOL RunSupervisor() const;
	inline BOOL RunExplorer() const;
	inline BOOL RunServer() const;

	// database
	inline BOOL RunDatabaseClient() const;
	inline BOOL RunDatabaseServer() const;
	inline BOOL RunCDRWriter() const;

	// codecs
	inline BOOL RunCoCoUnit() const;
	inline BOOL RunMiCoUnit() const;
	inline BOOL RunCoCoUnitPCI() const;
	inline BOOL RunMiCoUnitPCI() const;
	inline BOOL RunAnyCameraUnit() const;

	// links
	inline BOOL RunAnyLinkUnit() const;
	inline BOOL RunISDNUnit() const;
	inline BOOL RunSocketUnit() const;

	// boxes
	inline BOOL RunAnyBoxUnit() const;
	inline BOOL RunVCSUnit() const;
	inline BOOL RunPTUnit() const;

	// misc units
	inline BOOL RunAKUUnit() const;
	inline BOOL RunCommUnit() const;
	inline BOOL RunGAUnit() const;
	inline BOOL RunSDIUnit() const;
	inline BOOL RunSimUnit() const;
	inline BOOL RunAnyRelaisUnit() const;

	// misc
	inline BOOL RunSecAnalyzer() const;
	inline BOOL RunUpdateHandler() const;
	inline BOOL RunServicePack() const;
	//
	inline BOOL RunCoCoTest() const;
	inline BOOL RunMiCoTest() const;
	
	// already in use (most of them)
	inline BOOL RunMiCoUnit2() const;
	inline BOOL RunMiCoUnit3() const;
	inline BOOL RunMiCoUnit4() const;

	inline BOOL RunJaCobUnit1() const;
	inline BOOL RunJaCobUnit2() const;
	inline BOOL RunJaCobUnit3() const;
	inline BOOL RunJaCobUnit4() const;

#ifndef _HARDLOCK
	inline BOOL RunTashaUnit1() const;
	inline BOOL RunTashaUnit2() const;
	inline BOOL RunTashaUnit3() const;
	inline BOOL RunTashaUnit4() const;
#endif

	inline BOOL RunGemosUnit() const;
	inline BOOL RunICPCONUnit() const;
	inline BOOL RunUSBCameraUnit() const;

	inline BOOL RunSDIConfig() const;
	inline BOOL RunHTTP() const;
	inline BOOL RunICPCONClient() const;
	inline BOOL RunClient4() const;
	inline BOOL RunClient5() const;
	inline BOOL RunClient6() const;
	inline BOOL RunClient7() const;
	inline BOOL RunClient8() const;
	inline BOOL RunClient9() const;
	inline BOOL RunClient10() const;

	inline BOOL RunISDNUnit2() const;
	inline BOOL RunPTUnit2() const;
	inline BOOL RunSMSUnit() const;
	inline BOOL RunTOBSUnit() const;
	inline BOOL RunEMailUnit() const;
	inline BOOL RunFAXUnit() const;
	inline BOOL RunLinkUnit7() const;
	inline BOOL RunLinkUnit8() const;
	inline BOOL RunLinkUnit9() const;
	inline BOOL RunLinkUnit10() const;

	inline BOOL RunSaVicUnit1() const;
	inline BOOL RunSaVicUnit2() const;
	inline BOOL RunSaVicUnit3() const;
	inline BOOL RunSaVicUnit4() const;
	inline BOOL RunAudioUnit() const;
	inline BOOL RunQUnit() const;
	inline BOOL RunIPCameraUnit() const;
	inline BOOL RunProgram8() const;
	inline BOOL RunProgram9() const;
	inline BOOL RunProgram10() const;
	
	// functionalities
	inline BOOL AllowSoftDecodeCoCo() const;
	inline BOOL AllowSoftDecodeMiCo() const;
	inline BOOL AllowHardDecodeCoCo() const;
	inline BOOL AllowHardDecodeMiCo() const;

	inline BOOL AllowLocalProcesses() const;
	// for future use
	// CAVEAT 1..10 are TRUE by default (in ProductView)
	inline BOOL AllowSDICashSystems() const;
	inline BOOL AllowSDIAccessControl() const;
	inline BOOL AllowISDN2BChannels() const;
	inline BOOL AllowTimers() const;
	inline BOOL AllowMultiMonitor() const;
	inline BOOL AllowHTMLmaps() const;
	inline BOOL AllowSDIParkmanagment() const;
	inline BOOL AllowFunctionality8() const;
	inline BOOL AllowFunctionality9() const;
	inline BOOL AllowFunctionality10() const;

	inline BOOL AllowHTMLNotification() const;
	inline BOOL AllowHTMLInformation() const;
	inline BOOL AllowRouting() const;
	inline BOOL AllowSDIIndustrialEquipment() const;
	inline BOOL AllowSoftDecodePT() const;
	inline BOOL AllowMotionDetection() const;
	inline BOOL AllowAlarmOffSpans() const;
	inline BOOL IsDemo() const;
	inline BOOL RestrictedTo8CamerasPerBoard() const;
	inline BOOL AllowFunctionality20() const;
	//
	inline BOOL AllowInternalAccess() const;
	inline BOOL AllowEEPWrite() const;
	//
#ifndef _HARDLOCK
	inline int  GetNrOfHosts() const;
	inline int  GetNrOfCameras() const;
	inline int  GetNrOfIPCameras() const;
#endif


	// generic
	BOOL RunByApplicationID(WK_ApplicationId appId);
	inline BOOL IsExeOkay() const;	// only useable if the appId contructor is used
	//
	static const _TCHAR* NameOfDongleState(DWORD dwState);
	
	//
	void LoadFromCFG(const CString &sCFGFile);	// for ProductView

	BOOL Load(CWK_Profile& wkp);
	BOOL Save(CWK_Profile& wkp) const;

protected:
	void InitWK_Dongle();
	void ReadDongle(BOOL bIngoreSharedMemory=FALSE);
	void CalcProductFields();

private:
	void ReadSoftwareDongle(LPCTSTR szFileToUse=NULL);
	void ReadSoftwareDongle(CFile& file);
#ifdef _HARDLOCK
	void ReadHardwareDongle();
#endif
	void ReadCryptoDongle();
	void ReadSecondCryptoDongle();
	void Read(BOOL bFillSharedMemory);
	//
	void FillSharedMemory();
	void ReadSharedMemory();
	//
	BOOL UpdateVersionNumber();	// returns TRUE if vinfo.pvi was found
	//
	// RETURN: the [de]crypted data or NULL
#ifdef _HARDLOCK
	static char *Crypt(const char *pData, DWORD dwDataLen, 
						BOOL &bOkay, DWORD & dwResultLen
						);
	static char *Decrypt(const char *pData, DWORD dwDataLen,
						BOOL &bOkay, DWORD & dwResultLen
						);
#endif

protected:
	// data
	BOOL m_bIsValid;
	// product
	CString m_sVersion;
	//
	WORD m_wOEMCode;
	//
	InternalProductCode m_productCode;

	BOOL m_bIsTransmitter;
	BOOL m_bIsReceiver;
	BOOL m_bIsLocal;

	//
	BOOL m_bExeOkay;	// only set if the constructor with appId is used
	BOOL m_bHardwareDongleFound;

	BYTE m_bMajorVersion;
	BYTE m_bMinorVersion;

	// Dongle flags as burned in hardware dongle
	// CAVEAT: keep order unchanged!
	// base
	BOOL m_bRunLauncher;
	BOOL m_bRunSupervisor;
	BOOL m_bRunExplorer;
	BOOL m_bRunServer;

	// database
	BOOL m_bRunDatabaseClient;
	BOOL m_bRunDatabaseServer;
	BOOL m_bRunCDRWriter;

	// codecs
	BOOL m_bRunCoCoUnit;
	BOOL m_bRunMiCoUnit;
	BOOL m_bRunCoCoUnitPCI;
	BOOL m_bRunMiCoUnitPCI;

	// link
	BOOL m_bRunISDNUnit;
	BOOL m_bRunSocketUnit;

	// boxes
	BOOL m_bRunVCSUnit;
	BOOL m_bRunPTUnit;

	// misc units
	BOOL m_bRunAKUUnit;

	BOOL m_bRunCommUnit;
	BOOL m_bRunGAUnit;
	BOOL m_bRunSDIUnit;
	BOOL m_bRunSimUnit;

	// misc
	BOOL m_bRunSecAnalyzer;
	BOOL m_bRunUpdateHandler;
	BOOL m_bRunServicePack;

	BOOL m_bRunCoCoTest;
	BOOL m_bRunMiCoTest;

	// for future use
	BOOL m_bRunMiCoUnit2;
	BOOL m_bRunMiCoUnit3;
	BOOL m_bRunMiCoUnit4;
	BOOL m_bRunJaCobUnit1;
	BOOL m_bRunJaCobUnit2;
	BOOL m_bRunJaCobUnit3;
	BOOL m_bRunJaCobUnit4;

	BOOL m_bRunGemosUnit;
	BOOL m_bRunICPCONUnit;
	BOOL m_bRunUSBCameraUnit;


	BOOL m_bRunSDIConfig;
	BOOL m_bRunHTTP;
	BOOL m_bRunICPCONClient;
	BOOL m_bRunClient4;
	BOOL m_bRunClient5;
	BOOL m_bRunClient6;
	BOOL m_bRunClient7;
	BOOL m_bRunClient8;
	BOOL m_bRunClient9;
	BOOL m_bRunClient10;

	BOOL m_bRunSaVicUnit1;
	BOOL m_bRunSaVicUnit2;
	BOOL m_bRunSaVicUnit3;
	BOOL m_bRunSaVicUnit4;
	BOOL m_bRunAudioUnit;
	BOOL m_bRunQUnit;
	BOOL m_bRunIPCameraUnit;
	BOOL m_bRunProgram8;
	BOOL m_bRunProgram9;
	BOOL m_bRunProgram10;

	BOOL m_bRunISDNUnit2;
	BOOL m_bRunPTUnit2;
	BOOL m_bRunSMSUnit;
	BOOL m_bRunTOBSUnit;
	BOOL m_bRunEMailUnit;
	BOOL m_bRunFAXUnit;
	BOOL m_bRunLinkUnit7;
	BOOL m_bRunLinkUnit8;
	BOOL m_bRunLinkUnit9;
	BOOL m_bRunLinkUnit10;

	// <><><><> functionalities <><><><>
	BOOL m_bAllowSoftDecodeCoCo;
	BOOL m_bAllowSoftDecodeMiCo;
	BOOL m_bAllowHardDecodeCoCo;
	BOOL m_bAllowHardDecodeMiCo;

	BOOL m_bAllowLocalProcesses;
	// for future use
	BOOL m_bAllowSDICashSystems;
	BOOL m_bAllowSDIAccessControl;
	BOOL m_bAllowISDN2BChannels;
	BOOL m_bAllowTimers;
	BOOL m_bAllowMultiMonitor;
	BOOL m_bAllowHTMLmaps;
	BOOL m_bAllowSDIParkmanagment;
	BOOL m_bAllowFunctionality8;
	BOOL m_bAllowFunctionality9;
	BOOL m_bAllowFunctionality10;

	BOOL m_bAllowHTMLNotification;
	BOOL m_bAllowHTMLInformation;
	BOOL m_bAllowRouting;
	BOOL m_bAllowSDIIndustrialEquipment;
	BOOL m_bAllowSoftDecodePT;
	BOOL m_bAllowMotionDetection;
	BOOL m_bAllowAlarmOffSpans;
	BOOL m_bIsDemo;
	BOOL m_bRestrictedTo8CamerasPerBoard;
	BOOL m_bAllowFunctionality20;

	BOOL m_bAllowInternalAccess;
	BOOL m_bAllowEEPWrite;
	// End of dongle flags as burned in hardware dongle
	// CAVEAT: keep above order unchanged!
	// Add new flags below!

#ifndef _HARDLOCK
	BOOL m_bRunTashaUnit1;
	BOOL m_bRunTashaUnit2;
	BOOL m_bRunTashaUnit3;
	BOOL m_bRunTashaUnit4;
	int  m_iNrOfHosts;
	int  m_iNrOfCameras;
	int  m_iNrOfIPCameras;
#endif

//	static Dongle data
private:
	// Dongle Moduladresse
	static WORD m_wTheModulAddress;

	HANDLE m_hSharedMemory;
	BYTE *m_pSharedMemory;

	DWORD m_dwHardlockState;
	WORD m_wDongleFormat;
};	// end of CWK_Dongle

//////////////////////////////////////////////////////////////
// inlined functions
// <><><><> product <><><><>
inline CWK_Dongle::OemCode CWK_Dongle::GetOemCode() const
{
	return (OemCode) m_wOEMCode;
}
//////////////////////////////////////////////////////////////
inline BYTE CWK_Dongle::GetMajorVersion() const
{
	return m_bMajorVersion;
}
//////////////////////////////////////////////////////////////
inline BYTE CWK_Dongle::GetMinorVersion() const
{
	return m_bMinorVersion;
}
//////////////////////////////////////////////////////////////
inline BOOL CWK_Dongle::IsTransmitter() const
{
	return m_bIsTransmitter;
}
inline BOOL CWK_Dongle::IsReceiver() const
{
	return m_bIsReceiver;
}
inline BOOL CWK_Dongle::IsLocal() const
{
	return m_bIsLocal;
}
//////////////////////////////////////////////////////////////
// <><><><> modules (.exe) <><><><>
inline BOOL CWK_Dongle::RunLauncher() const
{
	return m_bRunLauncher;
}
inline BOOL CWK_Dongle::RunSupervisor() const
{
	return m_bRunSupervisor;
}
inline BOOL CWK_Dongle::RunExplorer() const
{
	return m_bRunExplorer;
}
inline BOOL CWK_Dongle::RunServer() const
{
	return m_bRunServer;
}
//////////////////////////////////////////////////////////////
inline BOOL CWK_Dongle::RunDatabaseClient() const
{
	return m_bRunDatabaseClient;
}
inline BOOL CWK_Dongle::RunDatabaseServer() const
{
	return m_bRunDatabaseServer;
}
inline BOOL CWK_Dongle::RunCDRWriter() const
{
	return m_bRunCDRWriter;
}
//////////////////////////////////////////////////////////////
inline BOOL CWK_Dongle::RunAKUUnit() const
{
	return m_bRunAKUUnit;
}
inline BOOL CWK_Dongle::RunCoCoUnit() const
{
	return m_bRunCoCoUnit;
}
inline BOOL CWK_Dongle::RunCommUnit() const
{
	return m_bRunCommUnit;
}
inline BOOL CWK_Dongle::RunGAUnit() const
{
	return m_bRunGAUnit;
}
inline BOOL CWK_Dongle::RunMiCoUnit() const
{
	return m_bRunMiCoUnit;
}
inline BOOL CWK_Dongle::RunSDIUnit() const
{
	return m_bRunSDIUnit;
}
inline BOOL CWK_Dongle::RunSimUnit() const
{
	return m_bRunSimUnit;
}
//////////////////////////////////////////////////////////////
inline BOOL CWK_Dongle::RunISDNUnit() const
{
	return m_bRunISDNUnit;
}
inline BOOL CWK_Dongle::RunSocketUnit() const
{
	return m_bRunSocketUnit;
}
inline BOOL CWK_Dongle::RunVCSUnit() const
{
	return m_bRunVCSUnit;
}

inline BOOL CWK_Dongle::RunPTUnit() const
{
	return m_bRunPTUnit;
}
inline BOOL CWK_Dongle::RunSecAnalyzer() const
{
	return m_bRunSecAnalyzer;
}
inline BOOL CWK_Dongle::RunUpdateHandler() const
{
	return m_bRunUpdateHandler;
}
inline BOOL CWK_Dongle::RunServicePack() const
{
	return m_bRunServicePack;
}

inline BOOL CWK_Dongle::RunCoCoTest() const
{
	return m_bRunCoCoTest;
}
inline BOOL CWK_Dongle::RunMiCoTest() const
{
	return m_bRunMiCoTest;
}
//////////////////////////////////////////////////////////////
// for future use
inline BOOL CWK_Dongle::RunMiCoUnit2() const
{
	return m_bRunMiCoUnit2;
}
inline BOOL CWK_Dongle::RunMiCoUnit3() const
{
	return m_bRunMiCoUnit3;
}
inline BOOL CWK_Dongle::RunMiCoUnit4() const
{
	return m_bRunMiCoUnit4;
}
inline BOOL CWK_Dongle::RunJaCobUnit1() const
{
	return m_bRunJaCobUnit1;
}
inline BOOL CWK_Dongle::RunJaCobUnit2() const
{
	return m_bRunJaCobUnit2;
}
inline BOOL CWK_Dongle::RunJaCobUnit3() const
{
	return m_bRunJaCobUnit3;
}
inline BOOL CWK_Dongle::RunJaCobUnit4() const
{
	return m_bRunJaCobUnit4;
}

inline BOOL CWK_Dongle::RunGemosUnit() const
{
	return m_bRunGemosUnit;
}
inline BOOL CWK_Dongle::RunICPCONUnit() const
{
	return m_bRunICPCONUnit;
}
inline BOOL CWK_Dongle::RunUSBCameraUnit() const
{
	return m_bRunUSBCameraUnit;
}

//////////////////////////////////////////////////////////////
inline BOOL CWK_Dongle::RunSDIConfig() const
{
	return m_bRunSDIConfig;
}
inline BOOL CWK_Dongle::RunHTTP() const
{
	return m_bRunHTTP;
}
inline BOOL CWK_Dongle::RunICPCONClient() const
{
	return m_bRunICPCONClient;
}
inline BOOL CWK_Dongle::RunClient4() const
{
	return m_bRunClient4;
}
inline BOOL CWK_Dongle::RunClient5() const
{
	return m_bRunClient5;
}
inline BOOL CWK_Dongle::RunClient6() const
{
	return m_bRunClient6;
}
inline BOOL CWK_Dongle::RunClient7() const
{
	return m_bRunClient7;
}
inline BOOL CWK_Dongle::RunClient8() const
{
	return m_bRunClient8;
}
inline BOOL CWK_Dongle::RunClient9() const
{
	return m_bRunClient9;
}
inline BOOL CWK_Dongle::RunClient10() const
{
	return m_bRunClient10;
}
//////////////////////////////////////////////////////////////
inline BOOL CWK_Dongle::RunSaVicUnit1() const
{
	return m_bRunSaVicUnit1;
}
inline BOOL CWK_Dongle::RunSaVicUnit2() const
{
	return m_bRunSaVicUnit2;
}
inline BOOL CWK_Dongle::RunSaVicUnit3() const
{
	return m_bRunSaVicUnit3;
}
inline BOOL CWK_Dongle::RunSaVicUnit4() const
{
	return m_bRunSaVicUnit4;
}
inline BOOL CWK_Dongle::RunAudioUnit() const
{
	return m_bRunAudioUnit;
}
inline BOOL CWK_Dongle::RunQUnit() const
{
	return m_bRunQUnit;
}
inline BOOL CWK_Dongle::RunIPCameraUnit() const
{
	return m_bRunIPCameraUnit;
}
inline BOOL CWK_Dongle::RunProgram8() const
{
	return m_bRunProgram8;
}
inline BOOL CWK_Dongle::RunProgram9() const
{
	return m_bRunProgram9;
}
inline BOOL CWK_Dongle::RunProgram10() const
{
	return m_bRunProgram10;
}
//////////////////////////////////////////////////////////////
inline BOOL CWK_Dongle::RunISDNUnit2() const
{
	return m_bRunISDNUnit2;
}
inline BOOL CWK_Dongle::RunPTUnit2() const
{
	return m_bRunPTUnit2;
}
inline BOOL CWK_Dongle::RunSMSUnit() const
{
	return m_bRunSMSUnit;
}
inline BOOL CWK_Dongle::RunTOBSUnit() const
{
	return m_bRunTOBSUnit;
}
inline BOOL CWK_Dongle::RunEMailUnit() const
{
	return m_bRunEMailUnit;
}
inline BOOL CWK_Dongle::RunFAXUnit() const
{
	return m_bRunFAXUnit;
}
inline BOOL CWK_Dongle::RunLinkUnit7() const
{
	return m_bRunLinkUnit7;
}
inline BOOL CWK_Dongle::RunLinkUnit8() const
{
	return m_bRunLinkUnit8;
}
inline BOOL CWK_Dongle::RunLinkUnit9() const
{
	return m_bRunLinkUnit9;
}
inline BOOL CWK_Dongle::RunLinkUnit10() const
{
	return m_bRunLinkUnit10;
}

//////////////////////////////////////////////////////////////
// functionalities
inline BOOL CWK_Dongle::AllowSoftDecodeCoCo() const
{
	return m_bAllowSoftDecodeCoCo;
}
inline BOOL CWK_Dongle::AllowSoftDecodeMiCo() const
{
	return m_bAllowSoftDecodeMiCo;
}
inline BOOL CWK_Dongle::AllowHardDecodeCoCo() const
{
	return m_bAllowHardDecodeCoCo;
}
inline BOOL CWK_Dongle::AllowHardDecodeMiCo() const
{
	return m_bAllowHardDecodeMiCo;
}
inline BOOL CWK_Dongle::AllowLocalProcesses() const
{
	return m_bAllowLocalProcesses;
}
//////////////////////////////////////////////////////////////
// for future use
inline BOOL CWK_Dongle::AllowSDICashSystems() const
{
	return m_bAllowSDICashSystems;
}
inline BOOL CWK_Dongle::AllowSDIAccessControl() const
{
	return m_bAllowSDIAccessControl;
}
inline BOOL CWK_Dongle::AllowISDN2BChannels() const
{
	return m_bAllowISDN2BChannels;
}
inline BOOL CWK_Dongle::AllowTimers() const
{
	return m_bAllowTimers;
}
inline BOOL CWK_Dongle::AllowMultiMonitor() const
{
	return m_bAllowMultiMonitor;
}
inline BOOL CWK_Dongle::AllowHTMLmaps() const
{
	return m_bAllowHTMLmaps;
}
inline BOOL CWK_Dongle::AllowSDIParkmanagment() const
{
	return m_bAllowSDIParkmanagment;
}
inline BOOL CWK_Dongle::AllowFunctionality8() const
{
	return m_bAllowFunctionality8;
}
inline BOOL CWK_Dongle::AllowFunctionality9() const
{
	return m_bAllowFunctionality9;
}
inline BOOL CWK_Dongle::AllowFunctionality10() const
{
	return m_bAllowFunctionality10;
}
inline BOOL CWK_Dongle::AllowHTMLNotification() const
{
	return m_bAllowHTMLNotification;
}
inline BOOL CWK_Dongle::AllowHTMLInformation() const
{
	return m_bAllowHTMLInformation;
}
inline BOOL CWK_Dongle::AllowRouting() const
{
	return m_bAllowRouting;
}
inline BOOL CWK_Dongle::AllowSDIIndustrialEquipment() const
{
	return m_bAllowSDIIndustrialEquipment;
}
inline BOOL CWK_Dongle::AllowSoftDecodePT() const
{
	return m_bAllowSoftDecodePT;
}
inline BOOL CWK_Dongle::AllowMotionDetection() const
{
	return m_bAllowMotionDetection;
}
inline BOOL CWK_Dongle::AllowAlarmOffSpans() const
{
	return m_bAllowAlarmOffSpans;
}
inline BOOL CWK_Dongle::IsDemo() const
{
	return m_bIsDemo;
}
inline BOOL CWK_Dongle::RestrictedTo8CamerasPerBoard() const
{
	return m_bRestrictedTo8CamerasPerBoard;
}
inline BOOL CWK_Dongle::AllowFunctionality20() const
{
	return m_bAllowFunctionality20;
}

//////////////////////////////////////////////////////////////
// Helper funktions
inline BOOL CWK_Dongle::IsExeOkay() const
{
		return m_bExeOkay;
}
inline InternalProductCode CWK_Dongle::GetProductCode() const
{
	return m_productCode;
}
inline DWORD CWK_Dongle::GetHardlockState() const
{
	return m_dwHardlockState;
}


inline BOOL CWK_Dongle::RunAnyLinkUnit() const
{
	return (RunISDNUnit() || RunSocketUnit() || RunTOBSUnit());
}
inline BOOL CWK_Dongle::RunAnyBoxUnit() const
{
    return (RunVCSUnit() || RunPTUnit() || RunTOBSUnit());
}

inline BOOL CWK_Dongle::RunAnyCameraUnit() const
{
	return    RunCoCoUnit() 
		   || RunCoCoUnitPCI()
		   || RunMiCoUnit()
		   || RunMiCoUnit2()
		   || RunMiCoUnit3()
		   || RunMiCoUnit4()
		   || RunMiCoUnitPCI()
		   || RunJaCobUnit1()
		   || RunJaCobUnit2()
		   || RunJaCobUnit3()
		   || RunJaCobUnit4()
		   || RunSaVicUnit1()
		   || RunSaVicUnit2()
		   || RunSaVicUnit3()
		   || RunSaVicUnit4()
#ifndef _HARDLOCK
			|| RunTashaUnit1()
			|| RunTashaUnit2()
			|| RunTashaUnit3()
			|| RunTashaUnit4()
			|| RunQUnit()
			|| RunIPCameraUnit()
#endif
			;
}

inline BOOL CWK_Dongle::RunAnyRelaisUnit() const
{
	return    RunCoCoUnit() 
		   || RunCoCoUnitPCI()
		   || RunMiCoUnit()
		   || RunMiCoUnit2()
		   || RunMiCoUnit3()
		   || RunMiCoUnit4()
		   || RunMiCoUnitPCI()
		   || RunJaCobUnit1()
		   || RunJaCobUnit2()
		   || RunJaCobUnit3()
		   || RunJaCobUnit4()
		   || RunSaVicUnit1()
		   || RunSaVicUnit2()
		   || RunSaVicUnit3()
		   || RunSaVicUnit4()
		   || RunAKUUnit()
		   || RunSimUnit()
		   || RunQUnit()
		   || RunIPCameraUnit();
}

inline BOOL CWK_Dongle::RunCoCoUnitPCI() const
{
	return m_bRunCoCoUnitPCI;
}
inline BOOL CWK_Dongle::RunMiCoUnitPCI() const
{
	return m_bRunMiCoUnitPCI;
}

inline BOOL CWK_Dongle::AllowInternalAccess() const
{
	return m_bAllowInternalAccess;
}
inline BOOL CWK_Dongle::AllowEEPWrite() const
{
	return m_bAllowEEPWrite;
}
#ifndef _HARDLOCK
inline BOOL CWK_Dongle::RunTashaUnit1() const
{
	return m_bRunTashaUnit1;
}
inline BOOL CWK_Dongle::RunTashaUnit2() const
{
	return m_bRunTashaUnit2;
}
inline BOOL CWK_Dongle::RunTashaUnit3() const
{
	return m_bRunTashaUnit3;
}
inline BOOL CWK_Dongle::RunTashaUnit4() const
{
	return m_bRunTashaUnit4;
}
inline int CWK_Dongle::GetNrOfHosts() const
{
	return m_iNrOfHosts;
}
inline int CWK_Dongle::GetNrOfCameras() const
{
	return m_iNrOfCameras;
}
inline int CWK_Dongle::GetNrOfIPCameras() const
{
	return m_iNrOfIPCameras;
}
#endif


AFX_EXT_CLASS LPCTSTR NameOfEnum(CWK_Dongle::OemCode code);

//////////////////////////////////////////////////////////////
#endif
