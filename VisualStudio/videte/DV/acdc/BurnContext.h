// BurnContext.h: interface for the BurnContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BURNCONTEXT_H__99CE58E4_BB17_46F5_8421_7A1111B0E52A__INCLUDED_)
#define AFX_BURNCONTEXT_H__99CE58E4_BB17_46F5_8421_7A1111B0E52A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ExitCode.h"
#include "parameters.h"	// Added by ClassView
#include "NeroBurn.h"
#include "AcdcDlg.h"

#define MAX_DVD_CAPACITY_IN_MB					4483
#define MAX_DVD_DOUBLE_LAYER_CAPACITY_IN_MB		8152
#define MAX_CD_CAPACITY_IN_MB					702
#define UNKNOWN_MEDIA_TYPE _T("Medientype: unknown")
#define RESERVE_SPACE_FOR_READER_SOFTWARE		12 

class CNeroBurn;
class CAcdcDlg;
class CBurnContext  
{

//functions
public:
	CBurnContext(PARAMETERS* params, CNeroBurn* pNeroBurn);
	~CBurnContext ();

	//given Nero functions
	//********************
	EXITCODE OpenDevice (const PARAMETERS & params);
	EXITCODE NeroLoad (void);
	EXITCODE InitNeroAPI (void);
	EXITCODE GetAvailableDrives (void);
	EXITCODE Exit (EXITCODE code);

	//MainCommands: located in MainCommands.cpp
	EXITCODE CommandCDInfo (const PARAMETERS & params);
	EXITCODE CommandEject (const PARAMETERS & params);
	EXITCODE CommandErase (const PARAMETERS & params);
	EXITCODE CommandListDrives (const PARAMETERS & params);
	EXITCODE CommandRead (const PARAMETERS & params);
	DWORD	 CommandVersion (CString& sVersion);
	EXITCODE CommandWrite (const PARAMETERS & params);

	//WriteCommands: located in WriteCommands.cpp
	EXITCODE WriteFreestyle (const PARAMETERS & params);
	EXITCODE WriteImage (const PARAMETERS & params);
	EXITCODE WriteIsoAudio (const PARAMETERS & params);
	void	 WriteNeroErrorLog (const PARAMETERS & params);
	EXITCODE WriteVideoCD (const PARAMETERS & params);


	//new functions
	//*************
	void		NeroAPIFree();	
	PARAMETERS* GetParams();
	void		SetCurrentProgress(int iProgress);
	static void	AddMessageLine(CString sMessage, int iNeroWaitCDType = -1);
	void		ResetParams();
	void		SetAbortFlag(BOOL bFlag = TRUE);
	BOOL		GetAbortFlag();
	MediumInfo	GetMediumInfo();
	ULONGLONG	GetMediumFreeCapacityInBytes();
	ULONGLONG	GetMediumUsedCapacityInBytes();
	void		SetCDRWEraseSuccess(BOOL bSuccess);
	BOOL		GetCDRWEraseSuccess();
	void		SetCDRWErasingTime(DWORD sSeconds); //set the erasing time in seconds for erasing a not empty cdrw
	void		SetCDRWAbortFlag(BOOL bFlag);
	BOOL		GetCDRWAbortFlag();
	BOOL		IsNeroInitialized();	
	BOOL		GetUsedCapacityRecursiv(CString sStartDir, ULONGLONG &ullTotalBytes);
	BOOL		IsOldNeroCmdVersion(); //TRUE, wenn zu Grunde liegende NeroCmd Version 5.5.8.2 ist
									   //FALSE, wenn Version 5.5.10.15

	//CheckMediaType: located in CheckMediaType.cpp	
	//*********************************************
	BOOL		CheckMediaCapacities(CString sDriveLetter);	//checks free and used capacities of the medium
	CString		CheckOldMediumType();	//when using NeroAPI version less than 5.5.9.10.

				//when using NeroAPI version  5.5.9.10. or higher
	CString		CheckNewMediaType(NERO_MEDIA_TYPE MediaType);	
	
protected:
	//given Nero functions
	//********************

	EXITCODE TranslateNeroToExitCode (NEROAPI_BURN_ERROR err);
	DWORD	 GetBurnFlags (const PARAMETERS & params);

	//Iso functions: located in IsoTrack.cpp
	EXITCODE GetIsoTrack (const PARAMETERS & params, CNeroIsoTrack ** ppIsoTrack, NERO_ISO_ITEM ** ppItem);
	void	 DeleteIsoItemTree (NERO_ISO_ITEM * pItem);
	EXITCODE CreateIsoTree (LPCSTR psFilename, NERO_ISO_ITEM ** ppItem, int iLevel = 0);

	//Callback functions: located in NeroCallbacks.cpp
	static BOOL  NERO_CALLBACK_ATTR AbortedCallback (void *pUserData);
	static BOOL  NERO_CALLBACK_ATTR IdleCallback (void *pUserData);
	static BOOL  NERO_CALLBACK_ATTR ProgressCallback (void *pUserData, DWORD dwProgressInPercent);
	static void  NERO_CALLBACK_ATTR AddLogLine (void *pUserData, NERO_TEXT_TYPE type, const char *text);
	static void  NERO_CALLBACK_ATTR SetPhaseCallback (void *pUserData, const char *text);
	static void  NERO_CALLBACK_ATTR SetMajorPhaseCallback (void *pUserData, NERO_MAJOR_PHASE phase, void* reserved);
	static void	 NERO_CALLBACK_ATTR DisableAbortCallback (void *pUserData, BOOL enableAbort);
	static DWORD NERO_CALLBACK_ATTR WriteIOCallback (void *pUserData, BYTE *pBuffer, DWORD dwLen);
	static BOOL  NERO_CALLBACK_ATTR EOFCallback (void *pUserData);
	static BOOL  NERO_CALLBACK_ATTR ErrorCallback (void *pUserData);
	static DWORD NERO_CALLBACK_ATTR ReadIOCallback (void *pUserData, BYTE *pBuffer, DWORD dwLen);
	static		 NeroUserDlgInOut NERO_CALLBACK_ATTR UserDialog (void * pUserData, NeroUserDlgInOut type, void *data);

	//new functions
	//*************
	void GetLastErrorLogLine();
	void ResetCurrentProgress();
	BOOL SetExtraWriteBuffer();	

private:

	//functions for checking mediums capacity and mediatype, located in CCheckMediaType()
	//*********************************************************************************
	void		InitCheckMediaType();	//initialize all members for checking media type
	CString		OnMedia_CDROM();		//things to do when medium is a CDROM
	CString		OnMedia_DVD_ROM();		//things to do when medium is a DVD_ROM
	CString		OnMedia_CDR();			//things to do when medium is a CD-R
	CString		OnMedia_DVD_M_R();		//things to do when medium is a DVD-R
	CString		OnMedia_DVD_P_R();		//things to do when medium is a DVD+R
	CString		OnMedia_CDRW();			//things to do when medium is a CDRW
	CString		OnMedia_DVD_PW_RW(NERO_MEDIA_TYPE MediaType);
										//things to do when medium is a DVD+RW or DVD-RW
	CString		OnMedia_CD();			//things to do when medium is a CD (CD-R / CD-RW)
	CString		OnMedia_DVD_M();		//things to do when medium is a DVD- (DVD-R / DVD-RW)
	CString		OnMedia_DVD_P();		//things to do when medium is a DVD+ (DVD+R / DVD+RW)

	CString		OnMedia_DVD_P_R9();		//things to do when medium is a DVD-R9 double layer

	

	//given Nero members
	//******************
protected:

	NERO_DEVICEHANDLE		m_NeroDeviceHandle;
	NERO_SCSI_DEVICE_INFOS*	m_NeroDeviceInfos;
	NERO_CD_INFO *			m_NeroCDInfo;
	void *					m_pCDStamp;
	bool					m_bNeroInitialized;
	static NERO_SETTINGS	s_NeroSettings;
	static NERO_PROGRESS	s_NeroProgress;

	//new members
	//***********
public:

	CNeroBurn*  m_pNeroBurn;		//NeroBurn handles all NeroAPI calls and Errors

private:

	BOOL		m_bAborted;			//abort flag which is check by NeroAPI continously during NeroAPI
									//functions. If TRUE, the current NeroAPI function will be terminated

	PARAMETERS* m_params;			//pointer to the parameters object, which holds all 
									//nesseccary parameters for NeroAPI burning processes

	char*		m_pNeroLastError;	//holds NeroAPI's last error
	int			m_iCurrentProgress;//current burning progress in percent
	BOOL		m_bDVDSecondProgress;//DVD burning time twice from 0% to 100%, this is for 2. half
	MediumInfo	m_MediumInfo;		//information about the inserted medium (CD-r, CD-RW, ect.)
	BOOL		m_bCDRWEraseSuccess;//TRUE, if CDRW was successfully erased

	//members for checking mediums capacity and mediatype, located in CCheckMediaType()
	//*********************************************************************************
	ULONGLONG	m_ullMediumFreeCapacityInBytes;	//free Bytes on inserted medium
	ULONGLONG	m_ullMediumUsedCapacityInBytes;	//used Bytes on inserted medium
	ULONGLONG	m_ullOneMegaByte;				//one MB (1024*1024) in ULL
	ULONGLONG	m_ullBytesPerBlock;				//bytes per block on data CD/DVD (2048)
	ULONGLONG	m_ullMaxCapacityDVD;			//max capacity of a dvd (4,7 GB)
	ULONGLONG	m_ullMaxCapacityDVDDoubleLayer;	//max capacity of a dvd double layer (8,5 GB)
	ULONGLONG	m_ullMaxCapacityCD;				//max capacity of a cd (700MB)
	ULONGLONG	m_ullMaxDVDBlocks;				//max number of blocks on a DVD
	DWORD		m_dwNumOfTracks;				//number of track on RW-mediums
	BOOL		m_bCheckMediaCapacity;			//if TRUE getting mediums capacity was successful

	BOOL	m_bCDRWAbortFlag;	//if TRUE, cd-rw deleting profress has to stop after deleting
								//the further burn process will be canceled
	BOOL	m_bCDRWEraseStart;	//TRUE, if an erase process (CDRW) is currently running
};



#endif // !defined(AFX_BURNCONTEXT_H__99CE58E4_BB17_46F5_8421_7A1111B0E52A__INCLUDED_)


