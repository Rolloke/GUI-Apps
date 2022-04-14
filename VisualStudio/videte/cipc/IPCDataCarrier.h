// IPCDataCarrier.h: interface for the CIPCDataCarrier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATACARRIER_H__75341FBC_4E8F_4A06_9A0F_8C77F60AF6E2__INCLUDED_)
#define AFX_IPCDATACARRIER_H__75341FBC_4E8F_4A06_9A0F_8C77F60AF6E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cipc.h"
#include "cipcdrives.h"

//Flags for DoRequestSession()
#define CD_WRITE_ISO		1	//write CD in ISO format
#define	CD_WRITE_VCD		2	//write CD in VideoCD format
#define	CD_WRITE_SVCD		3	//write CD in SuperVideoCD format
#define	DVD_WRITE_ISO		4	//write DVD in ISO format

#define DC_ERROR_UNKNOWN_ERROR			100 //unknown error
#define DC_ERROR_NO_ERROR				101 //status ok, no errors
#define DC_ERROR_NO_BURN_SOFTWARE		102	//no burn software available 
#define DC_ERROR_NO_MEDIUM				103 //no medium found in drive
#define DC_ERROR_DRIVE_NOT_FOUND		104 //specified drive not found
#define DC_ERROR_BURN_PROCESS_FAILED	105 //burn process failed
#define DC_ERROR_NO_CDRW_SUPPORTED		106 //drive not support CDRW
#define DC_ERROR_ERASE_CDRW_FAILED		107 //erase CD-RW failed
#define DC_ERROR_FILE_NOT_FOUND			108 //file(s) to burn not found
#define DC_ERROR_USER_ABORT				109 //user aborted process
#define DC_ERROR_SESSION_IS_RUNNING		110 //requested session is already running

class AFX_EXT_CLASS CIPCDataCarrier : public CIPC  
{
	//  construction / destruction
public:
	//!ic! constructor
	CIPCDataCarrier(LPCTSTR shmName, BOOL asMaster);
	//!ic! destructor
    virtual ~CIPCDataCarrier();

	//  Client - Server protocol
public:
	// volume info
			void DoRequestVolumeInfos();
	virtual void OnRequestVolumeInfos();
			void DoConfirmVolumeInfos(const CIPCDrives& drives);
	virtual	void OnConfirmVolumeInfos(int iNumDrives,
									  const CIPCDrive drives[]);

	// session
			void DoRequestSession(DWORD dwSessionID,
								  const CString& sVolume,
								  const CStringArray& sPathnames,
								  DWORD dwFlags);
	virtual	void OnRequestSession(DWORD dwSessionID,
								  const CString& sVolume,
								  const CStringArray& sPathnames,
								  DWORD dwFlags);

			void DoIndicateSessionProgress(DWORD dwSessionID, int iProgress);
	virtual	void OnIndicateSessionProgress(DWORD dwSessionID, int iProgress);

			void DoConfirmSession(DWORD dwSessionID);
	virtual	void OnConfirmSession(DWORD dwSessionID);

			void DoRequestCancelSession(DWORD dwSessionID);
	virtual	void OnRequestCancelSession(DWORD dwSessionID);
			void DoConfirmCancelSession(DWORD dwSessionID);
	virtual	void OnConfirmCancelSession(DWORD dwSessionID);

			void DoRequestDeleteVolume(const CString& sVolume);
	virtual	void OnRequestDeleteVolume(const CString& sVolume);
			void DoConfirmDeleteVolume(const CString& sVolume);
	virtual	void OnConfirmDeleteVolume(const CString& sVolume);

protected:
	// cmd scheduler, calls OnXXX
	virtual BOOL HandleCmd(DWORD dwCmd,
						   DWORD dwParam1, 
						   DWORD dwParam2,
						   DWORD dwParam3, 
						   DWORD dwParam4,
						   const CIPCExtraMemory *pExtraMem);
};

#define CIPC_DC_REQUEST_VOLUME_INFOS		500
#define CIPC_DC_CONFIRM_VOLUME_INFOS		501
#define CIPC_DC_REQUEST_SESSION				502
#define CIPC_DC_CONFIRM_SESSION				503
#define CIPC_DC_INDICATE_SESSION_PROGRESS	504
#define CIPC_DC_REQUEST_CANCEL_SESSION		505
#define CIPC_DC_CONFIRM_CANCEL_SESSION		506
#define CIPC_DC_REQUEST_DELETE_VOLUME		507
#define CIPC_DC_CONFIRM_DELETE_VOLUME		508

#endif // !defined(AFX_IPCDATACARRIER_H__75341FBC_4E8F_4A06_9A0F_8C77F60AF6E2__INCLUDED_)
