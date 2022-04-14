/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControl.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcServerControl.h $
// CHECKIN:		$Date: 14.04.05 12:26 $
// VERSION:		$Revision: 55 $
// LAST CHANGE:	$Modtime: 14.04.05 12:26 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CIPCServerControl_H_
#define _CIPCServerControl_H_

#include "Cipc.h"
#include "ConnectionRecord.h"

// 

//  ServerControlGroup | kind of requested connection
enum ServerControlGroup 
{
	// invalid connection type
	SC_NONE=0,
	// Input connection type
	SC_INPUT=1,
	// Output connection type
	SC_OUTPUT=2,
	// Database connection type
	SC_DATABASE=3,
	// Alarm connection type
	SC_CLIENT=4,
	// Reset connection type
	SC_RESET_SERVER=5,
	// Launcher file update connection type
	SC_FILE_UPDATE=6,
	// Data carrier connections for CD or DVD burning
	SC_DATA_CARRIER=7,
	// Audio connection type
	SC_AUDIO=8
};
/////////////////////////////////////////////////////////////
/*
 CIPCServerControl | Control class to manage connections
with dynamic shared memory names. Used in server applications
to accept new clients
*/
class AFX_EXT_CLASS CIPCServerControl : public CIPC
{
	// construction / destruction:
public:
	//!ic! constructor
	CIPCServerControl(LPCTSTR shmName, BOOL asMaster );
	//!ic! destructor
    virtual ~CIPCServerControl();

   // operations:
public:
//////////////////////////////////////////////////////////////////////////
	// Input connections:
	//!ic! NYD
			void DoRequestInputConnection(const CConnectionRecord &c);
	//!ic! NYD
	virtual void OnRequestInputConnection(const CConnectionRecord &c);
	//!ic! NYD
			void DoConfirmInputConnection(CSecID assignedID,
										  LPCTSTR shmName,
										  const CString& sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , DWORD dwCodePage
#endif
										  );
	//!ic! NYD
	virtual void OnConfirmInputConnection(CSecID assignedID,
										  const CString& sShmName,
										  const CString& sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , WORD wCodePage
#endif
										  );
//////////////////////////////////////////////////////////////////////////
	// Output connections:
	//!ic! NYD
			void DoRequestOutputConnection(const CConnectionRecord &c);
	//!ic! NYD
	virtual void OnRequestOutputConnection(const CConnectionRecord &c);
	//!ic! NYD
			void DoConfirmOutputConnection(CSecID assignedID,
										   LPCTSTR shmName,
										   const CString& sVersion,
										   DWORD dwOptions
#ifdef _UNICODE
											, DWORD dwCodePage
#endif
											);

			//!ic! NYD
	virtual void OnConfirmOutputConnection(CSecID assignedID,
										   const CString& shmName,
										   const CString& sVersion,
										   DWORD dwOptions
#ifdef _UNICODE
										   , WORD wCodePage
#endif
													);
//////////////////////////////////////////////////////////////////////////
	// Database connections:
	//!ic! NYD
			void DoRequestDataBaseConnection(const CConnectionRecord &c);
	//!ic! NYD
	virtual void OnRequestDataBaseConnection(const CConnectionRecord &c);
	//!ic! NYD
			void DoConfirmDataBaseConnection(CSecID assignedID,
											 LPCTSTR shmName,
										     const CString& sVersion,
											 DWORD dwOptions
#ifdef _UNICODE
											 , DWORD dwCodePage
#endif
											 );
			//!ic! NYD
	virtual void OnConfirmDataBaseConnection(CSecID assignedID,
											 const CString& shmName,
											 const CString& sVersion,
											 DWORD dwOptions
#ifdef _UNICODE
											 , WORD wCodePage
#endif
											 );
//////////////////////////////////////////////////////////////////////////
	// Reset connections:
	//!ic! NYD
			void DoRequestServerReset(const CConnectionRecord &c);
	//!ic! NYD
	virtual void OnRequestServerReset(const CConnectionRecord &c);
	//!ic! NYD
			void DoConfirmServerReset();
	//!ic! NYD
	virtual void OnConfirmServerReset();

	// Alarm connection:
	//!ic! NYD
			void DoRequestAlarmConnection(const CConnectionRecord &c);
	//!ic! NYD
	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);

	//!ic! NYD
			void DoConfirmAlarmConnection(const CString &shmNameInput,
										  const CString &shmNameOutput,
										  const CString &sBitrate,
										  const CString &sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , DWORD dwCodePage
#endif
										  );

	//!ic! NYD
	virtual	void OnConfirmAlarmConnection(const CString &shmNameInput,
										  const CString &shmNameOutput,
										  const CString &sBitrate,
										  const CString &sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , WORD wCodePage
#endif
										  );
//////////////////////////////////////////////////////////////////////////
	// file update:
	// Destination: 0=AppDir, 1=Windows, 2=Windows/System, 3=Root
	//				4=use sFileName
	//!ic! NYD
			void DoRequestFileUpdate(int iDestination,
									 const CString &sFileName,
									 const void *pData,
									 DWORD dwDataLen,
									 BOOL bNeedsReboot);
	//!ic! NYD
	virtual void OnRequestFileUpdate(int iDestination,
									 const CString &sFileName,
									 const void *pData,
									 DWORD dwDataLen,
									 BOOL bNeedsReboot);
	//!ic! NYD
			void DoConfirmFileUpdate(const CString &sFileName);
	//!ic! NYD
	virtual void OnConfirmFileUpdate(const CString &sFileName);

//////////////////////////////////////////////////////////////////////////
	// DataCarrier connection:
	//!ic! NYD
			void DoRequestDCConnection(const CConnectionRecord &c);
	//!ic! NYD
	virtual void OnRequestDCConnection(const CConnectionRecord &c);

	//!ic! NYD
			void DoConfirmDCConnection(const CString &shmNameDC,
									   const CString &sVersion,
									   DWORD dwOptions
#ifdef _UNICODE
										, DWORD dwCodePage
#endif
										);
	//!ic! NYD
	virtual	void OnConfirmDCConnection(const CString &shmNameDC,
							 		   const CString &sVersion,
									   DWORD dwOptions
#ifdef _UNICODE
									   , WORD wCodePage
#endif
									   );
//////////////////////////////////////////////////////////////////////////
	// Audio connections:
			void DoRequestAudioConnection(const CConnectionRecord &c);
	virtual void OnRequestAudioConnection(const CConnectionRecord &c);
			void DoConfirmAudioConnection(CSecID assignedID,
										   LPCTSTR shmName,
										   const CString& sVersion,
										   DWORD dwOptions
#ifdef _UNICODE
											, DWORD dwCodePage
#endif
											);

	virtual void OnConfirmAudioConnection(CSecID assignedID,
										   const CString& shmName,
										   const CString& sVersion,
										   DWORD dwOptions
#ifdef _UNICODE
										   , WORD wCodePage
#endif
											);
//////////////////////////////////////////////////////////////////////////
protected:
	virtual BOOL HandleCmd(DWORD dwCmd,
						   DWORD dwParam1, 
						   DWORD dwParam2,
						   DWORD dwParam3, 
						   DWORD dwParam4,
						   const CIPCExtraMemory *pExtraMem);
private:
	// no implementation, to avoid implicit generation!
	CIPCServerControl(const CIPCServerControl& src);
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCServerControl& src);	
};

#define SRV_CONTROL_REQUEST_OUTPUT_CONNECTION	1002
// dwParam1 outputOptions SCO_.XXX
// extraMemory remoteAddress
#define SRV_CONTROL_CONFIRM_OUTPUT_CONNECTION	1003
// dwParam1 assigned groupID
// dwParam2 selected options SCO_XXX
// extraMem shared memory name

#define SRV_CONTROL_REQUEST_INPUT_CONNECTION	1004
// dwParam1 outputOptions SCO_.XXX
// extraMemory remoteAddress
#define SRV_CONTROL_CONFIRM_INPUT_CONNECTION	1005
// dwParam1 assigned groupID
// dwParam2 selected options SCO_XXX
// extraMem shared memory name
#define SRV_CONTROL_REQUEST_DB_CONNECTION		1006
#define SRV_CONTROL_CONFIRM_DB_CONNECTION		1007

#define SRV_CONTROL_REQUEST_SERVER_RESET		1008
#define SRV_CONTROL_CONFIRM_SERVER_RESET		1009

#define SRV_CONTROL_REQUEST_ALARM_CONNECTION	1010
#define SRV_CONTROL_CONFIRM_ALARM_CONNECTION	1011

// special cmds reserved for LinkUnits
#define SRV_CONTROL_REQUEST_PIPE				1012
#define SRV_CONTROL_CONFIRM_PIPE				1013
#define SRV_CONTROL_REQUEST_REMOVE_PIPE			1014
#define SRV_CONTROL_CONFIRM_REMOVE_PIPE			1015

//
#define SRV_CONTROL_CANCEL_CONNECTION			1016

//
#define SRV_CONTROL_REQUEST_DC_CONNECTION		1017
#define SRV_CONTROL_CONFIRM_DC_CONNECTION		1018

#define SRV_CONTROL_REQUEST_AUDIO_CONNECTION	1019
#define SRV_CONTROL_CONFIRM_AUDIO_CONNECTION	1020

// options for input connections:

// options for output connections:
#define SCO_WANT_RELAYS				0x00000001
#define SCO_WANT_CAMERAS_BW			0x00000002
#define SCO_WANT_CAMERAS_COLOR		0x00000004
#define SCO_WANT_DECOMPRESS_BW		0x00000008
#define SCO_WANT_DECOMPRESS_COLOR	0x00000010
#define SCO_IS_DV					0x00000020
#define SCO_JPEG_AS_H263			0x00000040
#define SCO_NO_STREAM				0x00000080
#define SCO_MULTI_CAMERA			0x00000100
#define SCO_CAN_GOP					0x00000200


#endif
