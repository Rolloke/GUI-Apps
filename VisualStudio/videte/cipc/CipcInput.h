/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInput.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcInput.h $
// CHECKIN:		$Date: 19.01.06 11:22 $
// VERSION:		$Revision: 78 $
// LAST CHANGE:	$Modtime: 19.01.06 10:56 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CIPC_INPUT_H
#define __CIPC_INPUT_H

#include "Cipc.h"
#include "SecID.h"
#include "CameraControl.h"
#include "UpdateDefines.h"
#include "CipcInputRecord.h"
#include "CIPCField.h"
#include "SystemTime.h"
#include "TimeZoneInformation.h"

#define STRING_DATA_POINTER 0x80000000
/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
//  CIPCInput | Communication class for input connections.
// Input connections cover alarm indications and system
// file access functions.
// Class is used for Server/Unit communication
// and for Client/Server!
// public | CIPC
//  <c CIPC>, <c CIPCOutput>
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCInput : public CIPC
{
	//  construction / destruction
public:
	//!ic! constructor
	CIPCInput(LPCTSTR shmName, BOOL asMaster );
	//!ic! destructor
    virtual ~CIPCInput();
	
	//  Server-Unit initial protocol
public:
	//!ic! nyd
			void DoRequestHardware(WORD wGroupID);
	//!ic! nyd
	virtual void OnRequestHardware(WORD wGroupID);
	//!ic! nyd
			void DoConfirmHardware(WORD wGroupID,int iErrorCode);
	//!ic! nyd
	virtual void OnConfirmHardware(WORD wGroupID,int iErrorCode);

	//!ic! nyd
			void DoRequestReset(WORD wGroupID);	
	//!ic! nyd
	virtual void OnRequestReset(WORD wGroupID);	
	//!ic! nyd
			void DoConfirmReset(WORD wGroupID);
	//!ic! nyd
	virtual void OnConfirmReset(WORD wGroupID);

	//!ic! 1 positive, 0 negative
			void DoRequestSetEdge(WORD wGroupID,DWORD edgeMask);
	//!ic! nyd
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);
	//!ic! nyd
			void DoConfirmEdge(WORD wGroupID,DWORD edgeBitmask);
	//!ic! nyd
	virtual void OnConfirmEdge(WORD wGroupID,DWORD edgeBitmask);
	
	//!ic! 1 open, 0 closed
			void DoRequestSetFree(WORD wGroupID,DWORD openMask);
	//!ic! 1 open, 0 closed
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);
	//!ic! 1 open, 0 closed
			void DoConfirmFree(WORD wGroupID,DWORD openBitmask);
	//!ic! 1 open, 0 closed
	virtual void OnConfirmFree(WORD wGroupID,DWORD openBitmask);

	//!ic! nyd
			void DoRequestAlarmState(WORD wGroupID);
	//!ic! nyd
	virtual void OnRequestAlarmState(WORD wGroupID);
	//!ic! nyd
			void DoResponseAlarmState(WORD wGroupID);
	//!ic! nyd
	virtual void OnResponseAlarmState(WORD wGroupID);
	//!ic! 1 high, 0 low
			void DoConfirmAlarmState(WORD wGroupID,DWORD inputMask);
	//!ic! 1 high, 0 low
	virtual void OnConfirmAlarmState(WORD wGroupID,DWORD inputMask);

	//!ic! 1 high, 0 low
			void DoIndicateAlarmState(WORD wGroupID,
									  DWORD inputMask,  // 1 high, 0 low
									  DWORD changeMask, // 1 changed, 0 unchanged
									  LPCTSTR szInfoString=NULL);
	//!ic! 1 high, 0 low
	virtual void OnIndicateAlarmState(WORD wGroupID,
									  DWORD inputMask, 	// 1 high, 0 low
									  DWORD changeMask,	// 1 changed, 0 unchanged
									  LPCTSTR sInfoString);
	//!ic! alarm with fields:
			void DoIndicateAlarm(CSecID id, 
								 BOOL bAlarm,
								 const CIPCFields& fields);
	//!ic! alarm with fields
	virtual void OnIndicateAlarm(CSecID id, 
								 BOOL bAlarm,
								 int iNumRecords,
								 const CIPCField fields[]);

	//!ic! new 4.0 alarm indication with fields
			void DoIndicateAlarmFieldsState(WORD wGroupID,
									        DWORD inputMask,  // 1 high, 0 low
									        DWORD changeMask, // 1 changed, 0 unchanged
									        const CIPCFields& fields);
	//!ic! new 4.0 alarm indication with fields
	virtual void OnIndicateAlarmFieldsState(WORD wGroupID,
											DWORD inputMask, 	// 1 high, 0 low
											DWORD changeMask,	// 1 changed, 0 unchanged
											int iNumFields,
											const CIPCField fields[]);
	//!ic! new 4.0 alarm update with fields
			void DoIndicateAlarmFieldsUpdate(CSecID id, 
									         const CIPCFields& fields);
	//!ic! new 4.0 alarm update with fields
	virtual void OnIndicateAlarmFieldsUpdate(CSecID id,
									         int iNumFields,
										     const CIPCField fields[]);

	//!ic! indicate single alarm with data for md
			void DoIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1 = 0,
								   DWORD dwData2 = 0);
	//!ic! indicate single alarm with data for md
	virtual	void OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2);


	//  Client-Server:
	//!ic! client request for input information
			void DoRequestInfoInputs(WORD wGroupID);	// SECID_NO_GROUP_ID for all
	//!ic! client request for input information
	virtual void OnRequestInfoInputs(WORD wGroupID);	// SECID_NO_GROUP_ID for all
	//!ic! server confirm for input information
			void DoConfirmInfoInputs(WORD wGroupID, 
									 int iNumGroups,
									 int iNumRecords, 
									 const CIPCInputRecord records[]);
	//!ic! server confirm for input information
	virtual void OnConfirmInfoInputs(WORD wGroupID, 
									 int iNumGroups,
									 int iNumRecords, 
									 const CIPCInputRecord records[]);

	//  Client-Server file update interface:
	//!ic! Destination: 0=AppDir, 1=Windows, 2=Windows/System, 3=Root 4=use sFileName
			void DoRequestFileUpdate(int iDestination,
									 const CString &sFileName,
									 const void *pData,
									 DWORD dwDataLen,
									 BOOL bNeedsReboot);
	//!ic! Destination: 0=AppDir, 1=Windows, 2=Windows/System, 3=Root 4=use sFileName
	virtual void OnRequestFileUpdate(int iDestination,
									 const CString &sFileName,
									 const void *pData,
									 DWORD dwDataLen,
									 BOOL bNeedsReboot);
	//!ic! nyd
			void DoConfirmFileUpdate(const CString &sFileName);
	//!ic! nyd
	virtual void OnConfirmFileUpdate(const CString &sFileName);
	//!ic! nyd
			void DoRequestGetFile(int iDestination,
								  const CString &sFileName);
	//!ic! nyd
	virtual void OnRequestGetFile(int iDestination,
								  const CString &sFileName);
	//!ic! nyd
			void DoConfirmGetFile(int iDestination,
								  const CString &sFileName,
								  const void *pData,
								  DWORD dwDataLen, DWORD dwCodePage=0);
	//!ic! nyd
	virtual void OnConfirmGetFile(int iDestination,
								  const CString &sFileName,
								  const void *pData,
								  DWORD dwDataLen, DWORD dwCodePage);
	//  Client-Server-Unit RS232 Transparent Interface
	//!ic! nyd
			void DoIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr);
	//!ic! nyd
	virtual void OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr);
	//!ic! nyd
				void DoWriteCommData(CSecID id,
									 const CIPCExtraMemory &data,
									 DWORD dwBlockNr);
	//!ic! nyd
	virtual void OnWriteCommData(CSecID id,
								 const CIPCExtraMemory &data,
								 DWORD dwBlockNr);
	//!ic! nyd
			void DoConfirmWriteCommData(CSecID id, DWORD dwBlockNr);
	//!ic! nyd
	virtual void OnConfirmWriteCommData(CSecID id, DWORD dwBlockNr);

	//  Client-Server-Unit pan tilt zoom control
	//!ic! nyd
			void DoRequestCameraControl(CSecID commID, 
										CSecID camID,
										CameraControlCmd cmd,
										DWORD dwValue);
	//!ic! nyd
	virtual void OnRequestCameraControl(CSecID commID, 
										CSecID camID,
										CameraControlCmd cmd,
										DWORD dwValue);


	//  Client-Server System Time protocol
	//!ic! nyd
			void DoRequestGetSystemTime();
	//!ic! nyd
	virtual	void OnRequestGetSystemTime();
	//!ic! nyd
			void DoConfirmGetSystemTime(const CSystemTime& st);
	//!ic! nyd
	virtual	void OnConfirmGetSystemTime(const CSystemTime& st);

	//!ic! nyd
			void DoRequestSetSystemTime(const CSystemTime& st);
	//!ic! nyd
	virtual	void OnRequestSetSystemTime(const CSystemTime& st);
	//!ic! nyd
			void DoConfirmSetSystemTime();
	//!ic! nyd
	virtual	void OnConfirmSetSystemTime();

	//  Client-Server alarm confirmation
	//!ic! nyd
			void DoConfirmAlarm(CSecID id);
	//!ic! nyd
	virtual	void OnConfirmAlarm(CSecID id);

	//!ic! nyd
			void DoRequestGetAlarmOffSpans(CSecID id);
	//!ic! nyd
	virtual	void OnRequestGetAlarmOffSpans(CSecID id);
	//!ic! nyd
			void DoConfirmGetAlarmOffSpans(CSecID id, const CSystemTimeSpans& spans);
	//!ic! nyd
	virtual	void OnConfirmGetAlarmOffSpans(CSecID id, int iNumRecords, const CSystemTimeSpan data[]);

	//!ic! nyd
			void DoRequestSetAlarmOffSpans(CSecID id, const CSystemTimeSpans& spans);
	//!ic! nyd
	virtual	void OnRequestSetAlarmOffSpans(CSecID id, int iNumRecords, const CSystemTimeSpan data[]);
	//!ic! nyd
			void DoConfirmSetAlarmOffSpans(CSecID id);
	//!ic! nyd
	virtual	void OnConfirmSetAlarmOffSpans(CSecID id);

		//!ic! nyd
			void DoRequestGetTimeZoneInformation();
		//!ic! nyd
	virtual	void OnRequestGetTimeZoneInformation();
		//!ic! nyd
			void DoConfirmGetTimeZoneInformation(CTimeZoneInformation tzi);
		//!ic! nyd
	virtual	void OnConfirmGetTimeZoneInformation(CTimeZoneInformation tzi);

		//!ic! nyd
			void DoRequestSetTimeZoneInformation(CTimeZoneInformation tzi);
		//!ic! nyd
	virtual	void OnRequestSetTimeZoneInformation(CTimeZoneInformation tzi);
		//!ic! nyd
			void DoConfirmSetTimeZoneInformation();
		//!ic! nyd
	virtual	void OnConfirmSetTimeZoneInformation();

		//!ic! nyd
			void DoRequestGetTimeZoneInformations();
		//!ic! nyd
	virtual	void OnRequestGetTimeZoneInformations();
		//!ic! nyd
			void DoConfirmGetTimeZoneInformations(const CTimeZoneInformations& tzi);
		//!ic! nyd
	virtual	void OnConfirmGetTimeZoneInformations(const CTimeZoneInformations& tzi);

protected:
	// cmd scheduler, calls OnXXX
	virtual BOOL HandleCmd(DWORD dwCmd,
						   DWORD dwParam1, 
						   DWORD dwParam2,
						   DWORD dwParam3, 
						   DWORD dwParam4,
						   const CIPCExtraMemory *pExtraMem);

private:
	// no implementation, to avoid implicit generation!
	CIPCInput(const CIPCInput& src);
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCInput& src);	
};

/////////////////////////////////////////////////////////////////////////////
/////// Setup Dialog ////////
#define INP_REQ_SETUP				1
		// dwParam1: GroupID
#define INP_CONF_SETUP				2
		// dwParam1: GroupID

#define INP_REQ_INFO				3
		// dwParam1 wGroupID, SECID_NO_ID for all
#define INP_CONF_INFO				4

/////////////////////////////////////////////////////////////////////////////
/////// Reset Inputmodule ////////
#define INP_REQ_RESET				5
		// dwParam1: GroupID
#define INP_CONF_RESET				6
		// dwParam1: GroupID

/////////////////////////////////////////////////////////////////////////////
/////// Set EDGE ////////
#define INP_REQ_SETEDGE				7
		// dwParam1: GroupID
		// dwParam2: Edge-Bitmask.  1: pos. edge, 0: neg. edge 
#define INP_CONF_EDGE				8
		// dwParam1: GroupID
		// dwParam2: Edge-Bitmask.  1: pos. edge, 0: neg. edge 

/////////////////////////////////////////////////////////////////////////////
/////// Open or close input channel ////////
#define INP_REQ_SETFREE				9
		// dwParam1: GroupID
		// dwParam2: Open-Bitmask.  1: inp. open, 0: inp. closed 
#define INP_CONF_FREE				10
		// dwParam1: GroupID
		// dwParam2: Open-Bitmask.  1: inp. open, 0: inp. closed 

/////////////////////////////////////////////////////////////////////////////
//////// Get/Indicate alarm state ////////
#define INP_REQ_AL_STATE			13
		// dwParam1: GroupID
#define INP_CONF_AL_STATE			14
		// dwParam1: GroupID
		// dwParam2: Input-Bitmask.  1: high (alarm), 0: low 
		// dwParam3: Change-Bitmask. 1: input has changed, 0: no change 
#define INP_IND_AL_STATE			15
		// dwParam1: GroupID
		// dwParam2: Input-Bitmask.  1: high (alarm), 0: low 
		// dwParam3: Change-Bitmask. 1: input has changed, 0: no change 
#define INP_RESP_AL_STATE			16
		// dwParam1: GroupID

/////////////////////////////////////////////////////////////////////////////
//////// Hardware state request/indication ////////
#define INP_REQ_HARDWARE			19	
		// dwParam1: GroupID
#define INP_CONF_HARDWARE			20	
		// dwParam1: GroupID
		// dwParam2: 0: no error, !=0: error
#define INP_INDICATE_ALARM_FIELDS	21

#define INP_REQUEST_GET_FILE		33
#define INP_CONFIRM_GET_FILE		34

#define INP_REQUEST_FILE_UPDATE		35
#define INP_CONFIRM_FILE_UPDATE		36


#define INP_INDICATE_COMM_DATA		37
#define	INP_WRITE_COMM_DATA			38
#define	INP_CONFIRM_WRITE_COMM_DATA	39

#define INP_REQUEST_CAMERA_CONTROL  40

#define INP_IND_ALARM_FIELD_STATE	41
#define INP_IND_ALARM_FIELD_UPDATE	42

#define INP_REQUEST_GET_SYSTEM_TIME	43
#define INP_CONFIRM_GET_SYSTEM_TIME	44

#define INP_REQUEST_SET_SYSTEM_TIME	45
#define INP_CONFIRM_SET_SYSTEM_TIME	46

#define INP_CONFIRM_ALARM			47

#define INP_INDICATE_ALARM_NR		48

#define INP_REQUEST_GET_ALARM_OFF_SPANS	49
#define INP_CONFIRM_GET_ALARM_OFF_SPANS	50
#define INP_REQUEST_SET_ALARM_OFF_SPANS	51
#define INP_CONFIRM_SET_ALARM_OFF_SPANS	52

#define INP_REQUEST_GET_ZIME_ZONE_INFORMATION	53
#define INP_CONFIRM_GET_ZIME_ZONE_INFORMATION	54
#define INP_REQUEST_SET_ZIME_ZONE_INFORMATION	55
#define INP_CONFIRM_SET_ZIME_ZONE_INFORMATION	56

#define INP_REQUEST_GET_ZIME_ZONE_INFORMATIONS	57
#define INP_CONFIRM_GET_ZIME_ZONE_INFORMATIONS	58

// Bitdefinitionen de virtuellen Alarme
#define BIT_VALARM_ARCHIVE_60PERCENT_FULL	0
#define BIT_VALARM_ARCHIVE_FULL				1
#define BIT_VALARM_KAMERAMISSING			2

#define BIT_VALARM_OUTGOING_CALL_TCP		3	
#define BIT_VALARM_INCOMING_CALL_TCP		4 	
#define BIT_VALARM_REJECT_TCP				5	

#define BIT_VALARM_OUTGOING_CALL_ISDN1		6
#define BIT_VALARM_INCOMING_CALL_ISDN1		7
#define BIT_VALARM_REJECT_ISDN1				8

#define BIT_VALARM_OUTGOING_CALL_PT1		9	
#define BIT_VALARM_INCOMING_CALL_PT1		10
#define BIT_VALARM_REJECT_PT1				11

#define BIT_VALARM_OUTGOING_CALL_ISDN2		12
#define BIT_VALARM_INCOMING_CALL_ISDN2		13
#define BIT_VALARM_REJECT_ISDN2				14

#define BIT_VALARM_OUTGOING_CALL_PT2		15
#define BIT_VALARM_INCOMING_CALL_PT2		16	
#define BIT_VALARM_REJECT_PT2				17
#define BIT_VALARM_CHECK_CALL_ERROR			18

#define BIT_VALARM_ALWAYS_ALARM				19
#define BIT_VALARM_CHECK_CALL_OK			20
#define BIT_VALARM_ALARM_OFF_SPAN			21

#define BIT_VALARM_EXTERNAL_DEVICE_FAILURE	22
#define BIT_VALARM_TEMPERATURE				23
#define BIT_VALARM_SMART_NOT_OK				24
#define BIT_VALARM_HARDDISK_FAILURE			25
#define BIT_VALARM_FAN_SPEED				26

#endif
