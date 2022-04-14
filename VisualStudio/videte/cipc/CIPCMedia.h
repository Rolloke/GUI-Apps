/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCMedia.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCMedia.h $
// CHECKIN:		$Date: 8.03.05 12:39 $
// VERSION:		$Revision: 55 $
// LAST CHANGE:	$Modtime: 8.03.05 12:39 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CIPC_MEDIA_H
#define __CIPC_MEDIA_H

#include "Cipc.h"
#include "SystemTime.h"
#include "CipcMediaRecord.h"

class CIPCMediaSampleRecord;
class CIPCExtraMemory;

// OnRequestHardware Flags
#define MEDIA_CAN_AUDIO_INPUT  0x00000001	// Audio Input Device available
#define MEDIA_CAN_AUDIO_OUTPUT 0x00000002	// Audio Output Device available
#define MEDIA_CAN_VIDEO_INPUT  0x00000004	// Video Input Device available
#define MEDIA_CAN_VIDEO_OUTPUT 0x00000008	// Video Output Device available

#define MEDIA_IS_AUDIO         _T("Audio")
#define MEDIA_IS_VIDEO         _T("Video")

// OnRequestHardware RequestID´s
#define MEDIA_IDR_VIDEOINPUTDEVICES		1	// retrieves Video Input Devices
#define MEDIA_IDR_LEGACY_AM_FILTERS		2	// retrieves Direct Show Filters
#define MEDIA_IDR_VIDEOCOMPRESSORS		3	// retrieves Video Compressor Filters
#define MEDIA_IDR_AUDIOCOMPRESSORS		4	// retrieves Audio Compressor Filters
#define MEDIA_IDR_AUDIOINPUTDEVICES		5	// retrieves Audio Input Devices
#define MEDIA_IDR_AUDIO_RENDERERS		6	// retrieves Audio Renderer Devices
#define MEDIA_IDR_MIDI_RENDERERS		7	// retrieves Midi Render Devices
#define MEDIA_IDR_EXTERNAL_RENDERERS	8	// retrieves External Render Devices
#define MEDIA_IDR_DEVICE_CONTROL		9	// retrieves Control Devices
#define MEDIA_IDR_ACTIVE_MOVIE			10	// retrieves Activ Movie Filters
#define MEDIA_IDR_DVD_HW_DECODERS		11	// retrieves Hardware Decoder Filters
#define MEDIA_IDR_FILTER_MAPPER			12	// retrieves Filter Mapper device 1
#define MEDIA_IDR_FILTER_MAPPER2		13	// retrieves Filter Mapper device 2

#define MEDIA_IDR_INPUT_PINS			30	// Input Pins of the Capture Device
#define MEDIA_IDR_TEST_CONFIG			31	// Tests the Configuration

// DoRequestStartMediaEncoding parameter
#define MEDIA_ENC_NEW_SEGMENT			0x00000001	// Start a new segment for encoding
#define MEDIA_ENC_ONLY_INITIALIZE		0x00000002	// Only initialize Filter but do not start
#define MEDIA_ENC_NO_COMPRESSION 		0x00000004	// Start without compression filter
#define MEDIA_ENC_LOCAL_PLAY			0x00000008	// the data is rendered locally in the device
#define MEDIA_ENC_DELIVER_ALWAYS		0x00000010	// the data is delivered independently from threshold
#define MEDIA_ENC_DONT_SHOW_STATE		0x00000020	// Starts audio detection whithout showing the record state
#define MEDIA_ENC_DONT_CHANGE_STATES	0x00000040	// Restarts the Encoder without changing any states
#define MEDIA_ENC_CLIENT_REQUEST		0x00010000	// Request from Client

// DoRequestStartMediaDecoding parameter
#define MEDIA_DEC_CONFIRM_RECEIVING		0x00000001	// Confirms received samples

// Set and Get Values
#define MEDIA_SET_VALUE					0x80000000	// Set the Value
#define MEDIA_GET_VALUE					0x40000000	// Get the Value
#define MEDIA_ERROR_OCCURRED			0x20000000	// Error indication
#define MEDIA_SAVE_IN_REGISTRY			0x10000000  // Saves the set value into registry
#define MEDIA_COMMAND_VALUE(cmd)		(0x0fffffff & cmd) // Masks the command value

// Audio Cmds							Values					Type
#define MEDIA_ENABLE				1	// (FALSE, TRUE)		(DWORD)
#define MEDIA_MONO					2	// (FALSE, TRUE)		(DWORD)
#define MEDIA_RECORDING_LEVEL		3	// (-1, 0,...,1000)		(DWORD) -1: Automatic gain !
#define MEDIA_BALANCE				4	// (-1000,...,1000)		(DWORD)
#define MEDIA_LOUDNESS				5	// (0,...,1000)			(DWORD)
#define MEDIA_TREBLE				6	// (-1000,...,1000)		(DWORD)
#define MEDIA_BASS					7	// (-1000,...,1000)		(DWORD)
#define MEDIA_VOLUME				8	// (0,...,1000)			(DWORD)
#define MEDIA_VOLUME_LEVEL			9	// (0,...,100)			(DWORD)
#define MEDIA_START_VOLUME_LEVEL	10	// (FALSE, TRUE)		(DWORD)
#define MEDIA_THRESHOLD				11	// (0,...,100)			(DWORD)
#define MEDIA_DEVICE_NAME			12	//						(CString)
#define MEDIA_BYTES_PER_SECOND		13	//						(DWORD)
#define MEDIA_AUDIO_QUALITYS		14	// 						(CStringArray)
#define MEDIA_INPUT_PIN_NAME		15	//						(CString)
#define MEDIA_SAMPLE_FREQUENCY		16	// (0,1,2,3)			(DWORD)
#define MEDIA_STATE					17	// (binary Flags)		(DWORD)
#define MEDIA_STEREO_CHANNEL		18	// (0,1)				(DWORD)
#define MEDIA_START_LOCAL_REPLAY	19	// (FALSE, TRUE)		(BOOL)
#define MEDIA_DISCONNECT			20	// (TRUE)				(BOOL)

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////

class CIPCMediaSampleRecord;
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCMedia : public CIPC
{
	// construction / destruction
public:
	// constructor
	CIPCMedia(LPCTSTR shmName, BOOL asMaster );
	// destructor
    virtual ~CIPCMedia();

	//!ic! Unit-Server Information
public:
	//!ic! Media Reset:
			void DoRequestReset(WORD wGroupID);
	virtual void OnRequestReset(WORD wGroupID);
			void DoConfirmReset(WORD wGroupID);
	virtual void OnConfirmReset(WORD wGroupID);
	
	//!ic! Media Hardware:

			void DoRequestHardware(CSecID SecID, DWORD dwIDRequest);
	virtual void OnRequestHardware(CSecID SecID, DWORD dwIDRequest);
			void DoConfirmHardware(CSecID SecID,
				int iErrorcode       = 0,
				DWORD dwFlags        = 0,
				DWORD dwIDRequest    = 0,
				CIPCExtraMemory *pEM = NULL);
	virtual void OnConfirmHardware(CSecID SecID, int errorCode, DWORD dwFlags, DWORD dwIDRequest, const CIPCExtraMemory *pEM);
	
	//!ic! Client-Server Information
public:
			void DoRequestInfoMedia(WORD wGroupID);
	virtual void OnRequestInfoMedia(WORD wGroupID);
			void DoConfirmInfoMedia(WORD wGroupID, 
								    int iNumGroups, 
								    int iNumRecords, 
								    const CIPCMediaRecordArray& records);
	virtual	void OnConfirmInfoMedia(WORD wGroupID, 
								    int iNumGroups, 
								    int iNumRecords, 
								    const CIPCMediaRecordArray& records);


	//!ic! Media Encoding
public:
			void DoRequestStartMediaEncoding(CSecID sourceID, DWORD dwFlags, DWORD dwUserData);
	virtual void OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData);
			void DoIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID mediaID, DWORD dwUserData);
	virtual void OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID mediaID, DWORD dwUserData);
			void DoRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags);
	virtual void OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags);
	
	//!ic! Media Decoding
public:
			void DoRequestMediaDecoding(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwFlags);
	virtual void OnRequestMediaDecoding(const CIPCMediaSampleRecord&, CSecID mediaID, DWORD dwFlags);
			void DoRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);
	virtual void OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);

	//!ic! Media Values:
public:
			void DoRequestValues(  CSecID, DWORD dwCmd, DWORD dwID, DWORD dwValue=0, const CIPCExtraMemory*pData=NULL);
	virtual void OnRequestValues(CSecID, DWORD dwCmd, DWORD dwID, DWORD dwValue  , const CIPCExtraMemory*pData);
			void DoConfirmValues(  CSecID, DWORD dwCmd, DWORD dwID, DWORD dwValue=0, const CIPCExtraMemory*pData=NULL);
	virtual void OnConfirmValues(CSecID, DWORD dwCmd, DWORD dwID, DWORD dwValue  , const CIPCExtraMemory*pData);

protected:
	// cmd schedudler, calls OnXXX
	virtual BOOL HandleCmd(DWORD dwCmd,
						   DWORD dwParam1, 
						   DWORD dwParam2,
						   DWORD dwParam3, 
						   DWORD dwParam4,
						   const CIPCExtraMemory *pExtraMem);
private:
	// no implementation, to avoid implicit generation!
	CIPCMedia(const CIPCMedia& src);		
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCMedia& src);	
};
/////////////////////////////////////////////////////////////////////////////

#define CIPC_MEDIA_FIRST                 2000

#define CIPC_MEDIA_REQUESTRESET          2000	// Request for reset
#define CIPC_MEDIA_CONFIRMRESET          2001	// confirms teh reset request
#define CIPC_MEDIA_REQUESTHARDWARE       2002	// Request hardware config
#define CIPC_MEDIA_CONFIRMHARDWARE       2003	// confirms hardware config request
#define CIPC_MEDIA_REQUESTSTART_ENCODING 2004	// Requests to start encoding
#define CIPC_MEDIA_REQUESTSTOP_ENCODING  2005	// Requests to stop encoding
#define CIPC_MEDIA_INDICATE_DATA         2006	// Indicates data from the device
#define CIPC_MEDIA_REQUEST_DECODING      2007	// Requests to decode media data
#define CIPC_MEDIA_REQUESTSTOP_DECODING  2008	// Requests to stop decoding
#define CIPC_MEDIA_REQUEST_VALUES        2009	// Requests to set or get values
#define CIPC_MEDIA_CONFIRM_VALUES        2010	// Confirms the request for values
#define CIPC_MEDIA_REQUEST_INFO		     2011	// Requests info for Media inputs and outputs
#define CIPC_MEDIA_CONFIRM_INFO	        2012	// Confirms the request for Media inputs and outputs

#define CIPC_MEDIA_LAST                  2012

/////////////////////////////////////////////////////////////////////////////
// Connection Start:
//
//			MASTER						SLAVE
//		OUTP_REQ_CONNECTION		->	
//								<-	OUTP_CONF_CONNECTION	
//		OUTP_REQ_HARDWARE		->	
//								<-	OUTP_CONF_HARDWARE
//		INP_REQ_RESET			->
//								<-	INP_CONF_RESET
//				
/////////////////////////////////////////////////////////////////////////////

#endif // __CIPC_MEDIA_H
