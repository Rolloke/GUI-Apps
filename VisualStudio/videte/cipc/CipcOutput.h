/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutput.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcOutput.h $
// CHECKIN:		$Date: 18.05.04 10:44 $
// VERSION:		$Revision: 100 $
// LAST CHANGE:	$Modtime: 18.05.04 10:36 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __CIPC_OUTPUT_H
#define __CIPC_OUTPUT_H

#include "Cipc.h"
#include "PictureDef.h"
#include "SystemTime.h"
#include "IPCActivityMask.h"

class CIPCPictureRecord;
class CIPCOutputRecord;
class CIPCExtraMemory;
class CRect;		// forward declaration
class CIPCOutput;	// forward declaration

enum OverlayDisplayMode {
	ODM_INVALID = 0,
	ODM_LOCAL_VIDEO = 1,
	ODM_DECODE = 2,
	ODM_ENCODE = 3
};

enum ClientDisplayResource {
	CDR_INVALID			= 0,
	CDR_OVERLAY			= 1,
	CDR_DECOMPRESS_H263	= 2
};


/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
/*  CIPCOutput | 
Communication class for output connections.
Output connections cover any camera and image functionality and relay
output functions. Class is used for Server-Unit communication
and for Client-Server!
<nl>
Unit-Server Connection Start:
Server is always MASTER
Unit is always SLAVE
<nl>
<nl>
		MASTER/Server				SLAVE/Unit<nl>
<nl>
		DoRequestConnection			OnRequestConnection<nl>
		OnConfirmConnection			DoConfirmConnection<nl>
		DoRequestHardware			OnRequestHardware<nl>
		OnConfirmHardware			DoConfirmHardware<nl>
		DoRequestReset				OnRequestReset<nl>
									DoConfirmReset<nl>
<nl>
Client-Server Connection Start
<nl>
The Client has always to fetch a communication usind
CIPCFetch class. Result is a shared memory name
to create a new communication class instance.
<nl>
Server is always MASTER
Client is always SLAVE
<nl>
<nl>
		SLAVE/Client				MASTER/Server<nl>
<nl>
		DoRequestConnection			OnRequestConnection<nl>
		OnConfirmConnection			DoConfirmConnection<nl>
		DoRequestInfo				OnRequestInfo<nl>
		OnConfirmInfo				DoConfirmInfo<nl>
*/
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCOutput : public CIPC
{
	//  construction / destruction
public:
	//!ic! constructor
	CIPCOutput(LPCTSTR shmName, BOOL asMaster );
	//!ic! destructor
    virtual ~CIPCOutput();

	//  Client-Server Information
public:
	//!ic! NYD
	void DoRequestInfoOutputs(WORD wGroupID);	// SECID_NO_GROUP_ID for all
	//!ic! NYD
	void DoConfirmInfoOutputs(WORD wGroupID, int iNumGroups, 
								int iNumRecords, 
								const CIPCOutputRecord records[]);
	//!ic! NYD
	virtual void OnRequestInfoOutputs(WORD wGroupID);	// SECID_NO_GROUP_ID for all
	//!ic! NYD
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, 
								int iNumRecords, 
								const CIPCOutputRecord records[]
								);
	//  Server-Unit Reset:
	//!ic! NYD
			void DoRequestReset(WORD wGroupID);
	//!ic! NYD
			void DoConfirmReset(WORD wGroupID);
	//!ic! NYD
	virtual void OnRequestReset(WORD wGroupID);
	//!ic! NYD
	virtual void OnConfirmReset(WORD wGroupID);
	
	//  Server-Unit Hardware:
	//!ic! NYD
			void DoRequestHardware(WORD wGroupID);
	//!ic! NYD
			void DoConfirmHardware(WORD wGroupID, 
									int iErrorcode,
									BOOL bCanSWCompress,
									BOOL bCanSWDecompress,
									BOOL bCanColorCompress,
									BOOL bCanColorDecompress,
									BOOL bCanOverlay
									);
	//!ic! NYD
	virtual void OnRequestHardware(WORD wGroupID);
	//!ic! NYD
	virtual void OnConfirmHardware(WORD wGroupID,	
									int errorCode,	// 0==okay, !=0 some error
									BOOL bCanSWCompress,
									BOOL bCanSWDecompress,
									BOOL bCanColorCompress,
									BOOL bCanColorDecompress,
									BOOL bCanOverlay
									);
	//  Server-Unit Current State:
	//!ic! NYD
			void DoRequestCurrentState(WORD wGroupID);
	//!ic! NYD
			void DoConfirmCurrentState(WORD wGroupID, DWORD dwStateBitmask);
	//!ic! NYD
	virtual void OnRequestCurrentState(WORD wGroupID);
	//!ic! NYD
	virtual void OnConfirmCurrentState(WORD wGroupID, DWORD stateMask);
	
	//  Client-Server-Unit set relay:
	//!ic! NYD
			void DoRequestSetRelay(CSecID relayID, BOOL bClosed);
	//!ic! NYD
	virtual void OnRequestSetRelay(CSecID relayID, BOOL bClosed);
	//!ic! NYD
			void DoConfirmSetRelay(CSecID relayID, BOOL bClosed);
	//!ic! NYD
	virtual void OnConfirmSetRelay(CSecID relayID, BOOL bClosed);
	
	//  Client-Server common video
	//!ic! used for Presence PT-200 client request for video data
			void DoRequestEncodedVideo(CSecID camID, 
									   Resolution  res,
									   Compression comp,
								       WORD	wNumPictures,
									   DWORD	dwBitrate,
									   DWORD	dwUserData=0,
									   BOOL bDoSleep = TRUE);
	//!ic! server side overwrite to recognize client video request
	virtual void OnRequestEncodedVideo(CSecID camID, 
									   Resolution  res,
									   Compression comp,
									   WORD	wNumPictures,
									   DWORD	dwBitrate,
									   DWORD	dwUserData=0);
	//!ic! NYD
			void DoConfirmEncodedVideo(const CIPCPictureRecord &pict,
									   DWORD dwUserData,
									   CSecID idArchive);
	//!ic! NYD
	virtual void OnConfirmEncodedVideo(const CIPCPictureRecord &pict,
									   DWORD dwUserData,
									   CSecID idArchive);
	
	//!ic! NYD
			void DoRequestNewJpegEncoding(CSecID camID,
										  Resolution res, 
										  Compression comp,
										  int iNumPictures,
										  DWORD	dwUserId=0,
										  BOOL bDoSleep = TRUE);
	//!ic! NYD
	virtual void OnRequestNewJpegEncoding(CSecID camID, 
										  Resolution res, 
										  Compression comp,
										  int iNumPictures,
										  DWORD	dwUserID);
	//  Server-Unit video request
	//!ic! NYD
			void DoRequestUnitVideo(WORD wGroupID,
								    BYTE byCam, 
								    BYTE byNextCam,
								    Resolution res,
								    Compression comp,
									CompressionType ct,
									int iNumPictures,
									DWORD dwUserId=0,
									BOOL bDoSleep = TRUE);
	//!ic! NYD
	virtual void OnRequestUnitVideo(WORD wGroupID,
								    BYTE byCam, 
								    BYTE byNextCam,
								    Resolution res,
								    Compression comp,
									CompressionType ct,
								    int iNumPictures,
								    DWORD dwUserID);
			void DoRequestStartVideo(CSecID id,			// camera nr.
									 Resolution res,		// resolution
									 Compression comp,		// image size or bitrate
									 CompressionType ct,	// JPEG, YUV, MPEG4 ...
									 int iFPS,				// frames per second
									 int iIFrameInterval,   // for MPEG4 I-Frame interval, 1=I-Frame only
									 DWORD dwUserData		// unique server id
									 );
	virtual	void OnRequestStartVideo(CSecID id,			// camera nr.
									 Resolution res,		// resolution
									 Compression comp,		// image size or bitrate
									 CompressionType ct,	// JPEG, YUV, MPEG4 ...
									 int iFPS,
									 int iIFrameInterval, 
									 DWORD dwUserData		// unique server id
									);
			void DoIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData,
								 CSecID idArchive);
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData,
								 CSecID idArchive);
			void DoRequestStopVideo(CSecID camID,
									DWORD dwUserData);
	virtual void OnRequestStopVideo(CSecID camID,
									DWORD dwUserData);

	//!ic! NYD
			void DoConfirmJpegEncoding(const CIPCPictureRecord &pict,
										DWORD dwUserData,
										CSecID idArchive,
										BOOL bDoSleep=TRUE);
	//!ic! NYD
	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
								   	   DWORD dwUserData,
									   CSecID idArchive);

	//  BW decompress (old pre 3.0)
	//!ic! NYD
			void DoRequestJpegDecodingExtra(WORD wGroupID, 
											Resolution res, 
											CIPCExtraMemoryPtr &ptrRef,
											DWORD dwUserData);

	//!ic! NYD	(old pre 3.0)
	virtual void OnRequestJpegDecoding(WORD wGroupID, 
									   Resolution res,
									   const CIPCExtraMemory &data,
									   DWORD dwUserData);
	//!ic! NYD	(old pre 3.0)
			void DoConfirmJpegDecoding(WORD wGroupID,
									   WORD wXSize, 
									   WORD wYSize,
									   DWORD dwUserData,
									   const CIPCPictureRecord &pict);	

	//!ic! NYD	(old pre 3.0)
	virtual void OnConfirmJpegDecoding(WORD wGroupID,
									   WORD wXSize, 
									   WORD wYSize, 
									   DWORD dwUserData,
									   const CIPCPictureRecord &pict);


	//  Client-Server-Unit Megra Overlay:
	//!ic! NYD
			void DoRequestSetDisplayWindow(WORD wGroupID, const CRect &rect);
	//!ic! NYD
	virtual void OnRequestSetDisplayWindow(WORD wGroupID, const CRect &rect);
	//!ic! NYD
			void DoConfirmSetDisplayWindow(WORD wGroupID);
	//!ic! NYD
	virtual void OnConfirmSetDisplayWindow(WORD wGroupID);

	//!ic! NYD
			void DoRequestSetOutputWindow (WORD wGroupID, 
											const CRect &rect, 
											OverlayDisplayMode odm=ODM_LOCAL_VIDEO
											);
	//!ic! NYD
	virtual void OnRequestSetOutputWindow (WORD wGroupID, const CRect &rect, OverlayDisplayMode odm);
	//!ic! NYD
			void DoConfirmSetOutputWindow (WORD wGroupID, int iError);
	//!ic! NYD
	virtual void OnConfirmSetOutputWindow (WORD wGroupID, int iPictureResult);
	//!ic! NYD
			void DoIndicateClientActive(BOOL bClientIsActive, WORD wGroupID);
	//!ic! NYD
	virtual void OnIndicateClientActive(BOOL bClientIsActive, WORD wGroupID);

	//  Client-Server-Unit local video (pre 4.0):
	//!ic! NYD
			void DoRequestLocalVideo(CSecID camID, 
									 Resolution res,
									 const CRect &rect,
									 BOOL bActive);
	//!ic! NYD
	virtual void OnRequestLocalVideo(CSecID camID, 
									 Resolution res,
									 const CRect &rect,
									 BOOL bActive);
	//!ic! NYD
				void DoConfirmLocalVideo(CSecID camID, 
										 Resolution res,
										 const CRect &rect,
										 BOOL bActive,
										 BOOL bIsOverlay);
	//!ic! NYD
	virtual void OnConfirmLocalVideo(CSecID camID, 
									 Resolution res,
									 const CRect &rect,
									 BOOL bActive,
									 BOOL bIsOverlay);
	//  Client-Server-Unit local video picture data:
	//!ic! NYD
			void DoIndicateLocalVideoData(CSecID camID,
										  WORD wXSize, 
										  WORD wYSize, 
										  const CIPCPictureRecord &pict);
	//!ic! NYD
	virtual void OnIndicateLocalVideoData(CSecID camID, 
									WORD wXSize, WORD wYSize, 
									const CIPCPictureRecord &pict
									);

	//  Client-Server-Unit MPEG/H.263 encoding:
	//!ic! NYD
			void DoRequestStartH263Encoding(CSecID camID, 
											Resolution res, 
											Compression comp,
											DWORD dwBitrate=64000,
											DWORD dwRecordTime=0);
	//!ic! NYD
	virtual void OnRequestStartH263Encoding(CSecID camID, 
											Resolution res, 
											Compression comp,
											DWORD dwBitrate,
											DWORD dwRecordTime);
	//!ic! NYD
			void DoConfirmStartH263Encoding(CSecID camID);
	//!ic! NYD
	virtual void OnConfirmStartH263Encoding(CSecID camID);
	//!ic! NYD
			void DoIndicateH263Data(const CIPCPictureRecord &pict, 
									DWORD dwJobData,
									CSecID idArchive);
	//!ic! NYD
	virtual void OnIndicateH263Data(const CIPCPictureRecord &pict, 
									DWORD dwJobData,
									CSecID idArchive);
	//!ic! NYD
			void DoRequestStopH263Encoding(CSecID camID);
	//!ic! NYD
	virtual void OnRequestStopH263Encoding(CSecID camID);
	//!ic! NYD
			void DoConfirmStopH263Encoding(CSecID camID);
	//!ic! NYD
	virtual void OnConfirmStopH263Encoding(CSecID camID);

	//  Client-Server-Unit MPEG/H.263 decoding:
	//!ic! NYD
			void DoRequestH263Decoding(WORD wGroupID,
									   const CRect &rect, 
									   const CIPCPictureRecord &pict,
									   DWORD dwUserData);
	//!ic! NYD
	virtual void OnRequestH263Decoding(WORD wGroupID,
									   const CRect &rect,
									   const CIPCPictureRecord &pict,
									   DWORD dwUserData);
	//!ic! NYD
			void DoConfirmH263Decoding(WORD wGroupID, DWORD dwUserData);
	//!ic! NYD
	virtual void OnConfirmH263Decoding(WORD wGroupID, DWORD dwUserData);
	//!ic! NYD
			void DoRequestStopH263Decoding(WORD wGroupID, const CRect &rect);
	//!ic! NYD
	virtual void OnRequestStopH263Decoding(WORD wGroupID, const CRect &rect);
	//!ic! NYD
			void DoRequestDumpOutputWindow(WORD wGroupID,
										   WORD wUserID,
										   const CRect &outputRect,
										   int iFormat=0);
	//!ic! NYD
	virtual void OnRequestDumpOutputWindow(WORD wGroupID,
										   WORD wUserID,
										   const CRect &outputRect,
										   int iFormat);
	
	//!ic! NYD
			void DoConfirmDumpOutputWindow(WORD wGroupID,
										   WORD wUserID,
										   int iFormat,
										   const CIPCExtraMemory &bitmapData);
			
	//!ic! NYD
	virtual void OnConfirmDumpOutputWindow(WORD wGroupID,
										   WORD wUserID,
										   int iFormat,
										   const CIPCExtraMemory &bitmapData);
	/////////////
	// CAVEAT special type 1234 passes queues as fast as possible
	// at least cipc queues, link units not yet
	//  Client-Server synchronisation
	//!ic! NYD
			void DoRequestSync(DWORD dwTickSend, DWORD dwType, DWORD dwUserID);
	//!ic! NYD
	virtual void OnRequestSync(DWORD dwTickSend, DWORD dwType,DWORD dwUserID);
	//!ic! NYD
			void DoConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend, 
							   DWORD dwType, DWORD dwUserID);
	//!ic! NYD
	virtual void OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend, 
				   DWORD dwType,DWORD dwUserID);

	//  Server-Unit idle pictures
	//!ic! NYD
			void DoRequestIdlePictures(int iNumPics);
	//!ic! NYD
	virtual void OnRequestIdlePictures(int iNumPics);

	//  Client-Server camera selection:
	//!ic! NYD
			void DoRequestSelectCamera(CSecID camID);
	//!ic! NYD
	virtual void OnRequestSelectCamera(CSecID camID);
	//!ic! NYD
			void DoIndicateClientChanged(ClientDisplayResource cdr);
	//!ic! NYD
	virtual	void OnIndicateClientChanged(ClientDisplayResource cdr);

	//!ic! NYD
			void DoRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID);
	//!ic! NYD
	virtual void OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID);
	//!ic! NYD
			void DoConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	//!ic! NYD
	virtual	void OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);

	//!ic! NYD
			void DoRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	//!ic! NYD
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	//!ic! NYD
			void DoConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID);
	//!ic! NYD
	virtual	void OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID);

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
	CIPCOutput(const CIPCOutput& src);		
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCOutput& src);	
};
/////////////////////////////////////////////////////////////////////////////
// One outputgroup has max. 32 outputs.
// All Param. are 0 unless otherwise described

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
//////// Setup Dialog ////////
#define OUTP_REQ_SETUP				101
// Called to show Setup Dialog. 
// Respond:	OUTP_CONF_SETUP
#define OUTP_CONF_SETUP				102
// Confirms that Dialog has been shown

#define OUTP_REQ_INFO				103
#define OUTP_CONF_INFO				104
/////////////////////////////////////////////////////////////////////////////
//////// Reset Inputmodule ////////
#define OUTP_REQ_RESET				105
// dwParam1 iGroupID
#define OUTP_CONF_RESET				106
// Confirms a Reset
// dwParam1: wGroupID
// dwParam2: iErrorCode: 0, hardware OK, !=0 hardware error
// dwParam3: bitmask with information about capabilities:
// 1 = canBWCompress
// 2 = canBWDecompress
// 4 = canColorCompress
// 8 = canColorDecompress
//16 = canOverlay
//
#define PICT_REQ_JPEG_ENCODING 301

#define PICT_CONF_JPEG_ENCODING 302

#define PICT_REQ_WND_DISPLAY	308
// dwParam1:	(lo,hi) left, top				  
// dwParam2:	(lo,hi) right, bottom
#define PICT_CONF_WND_DISPLAY	309

#define PICT_REQ_WND_OUTP		310						
// dwParam1:	Camera system-number
// dwParam2:	(lo,hi) oben links
// dwParam3:	(lo,hi) unten rechts
#define PICT_CONF_WND_OUTP		311
// dwParam1:	Camera system-number
// dwParam2:	PICTURE_ERROR, PICTURE_OVERLAY, PICTURE_BITMAP  


// OLD	#define PICTURE_ERROR			1000	// Fehler ist aufgetreten.
// OLD 	#define PICTURE_OVERLAY			1001	// Das Bild wird mit Overlay angezeigt
// OLD 	#define PICTURE_BITMAP			1002	// Das zurückgelieferte Bild ist ein Bitmap
// OLD 	#define PICTURE_NO_PICTURE		1003	// Modul kann kein Eigenbild liefern

#define PICT_REQ_DECOMPRESS		318
// dwParam1:	Picture Type: PICTURE_HIGH, PICTURE_MID, PICTURE_LOW	
// dwParam2:	CameraSystemNumber
// dwParam3:	Size of Compressed Picture.
#define PICT_CONF_DECOMPRESS	319
// dwParam1:	PICTURE_OVERLAY, ignore dwParam1 and dwParam2.  
//				PICTURE_BITMAP, dwParam1 and dwParam2 are valid.
//				PICTURE_ERROR, Error, ignore dwParam1, dwParam2 and dwParam4
// dwParam2:	X-Size of Picture in Cipc-Extra-Memory 
// dwParam3:	Y-Size of Picture in Cipc-Extra-Memory 
// dwParam4:	OutputWindowNumber.  The same number as in OUTP_REQ_WND_OUTP
	#define DECOMPRESS_ERROR	1000
	#define DECOMPRESS_OVERLAY	1001
	#define DECOMPRESS_BITMAP	1002

// for params see according functions calls
#define PICT_REQ_START_H263		330
#define PICT_CONF_START_H263	331
// no response, DoIndicateError instead
#define PICT_REQ_STOP_H263		333
#define PICT_CONF_STOP_H263		334
#define PICT_INDICATE_H263_DATA	335
#define PICT_REQ_DISPLAY_H263	336
#define PICT_CONF_DISPLAY_H263	337
//
// END of picture defines
/////////////////////////////////////////////////////////////////////////////
//////// Select Output ////////
#define OUTP_REQUEST_SET_RELAY			107
#define OUTP_CONFIRM_SET_RELAY			108

/////////////////////////////////////////////////////////////////////////////
//////// Current state ////////
#define OUTP_REQ_CUR_STATE			110
// Respond: OUTP_CONF_CUR_STATE
#define OUTP_CONF_CUR_STATE			111
// dwParam1:	Output-state Bitmask
// dwParam2:	Output-state Bitmask for changed outputs

/////////////////////////////////////////////////////////////////////////////
//////// Hardware state request ////////
#define OUTP_REQ_HARDWARE			114	
// Get Hardware Status
// Respond: OUTP_CONF_HARDWARE
#define OUTP_CONF_HARDWARE			115	
// dwParam1:	0: no error, !=0: error

/////////////////////////////////////////////////////////////////////////////
#define OUTP_REQ_CLOSE_MODULE		118
// Stop module execution
#define OUTP_CONF_CLOSE_MODULE		119

#define OUTP_REQUEST_LOCAL_VIDEO	120
#define OUTP_CONFIRM_LOCAL_VIDEO	121
#define OUTP_INDICATE_LOCAL_VIDEO_DATA 122

#define OUTP_REQUEST_STOP_H263_DECODING 124
#define OUTP_CONFIRM_STOP_H263_DECODING 125

#define OUTP_REQUEST_LOCAL_OVERLAY	126

#define OUTP_CONFIRM_JPEG_ENCODING_NEW 127
#define OUTP_CONFIRM_JPEG_DECODING_NEW 128
#define OUTP_INDICATE_LOCAL_VIDEO_DATA_NEW 129

#define CIPC_OUTPUT_REQUEST_DUMP_OUTPUT_WINDOW 130
#define CIPC_OUTPUT_CONFIRM_DUMP_OUTPUT_WINDOW 131

#define CIPC_REQUEST_SYNC 132
#define CIPC_CONFIRM_SYNC 133

#define OUTP_INDICATE_CLIENT_ACTIVE		134
// send to client
#define OUTP_REQUEST_SELECT_CAMERA		135

#define OUTP_REQUEST_ENCODED_VIDEO		136
#define OUTP_INDICATE_CLIENT_CHANGED	137
#define OUTP_CONFIRM_ENCODED_VIDEO		138

#define OUTP_REQUEST_IDLE_PICTURES		139

#define PICT_REQ_UNIT_VIDEO				338

#define CIPC_OUTPUT_REQUEST_GET_MASK	140
#define CIPC_OUTPUT_CONFIRM_GET_MASK	141
#define CIPC_OUTPUT_REQUEST_SET_MASK	142
#define CIPC_OUTPUT_CONFIRM_SET_MASK	143

#define CIPC_OUTPUT_REQUEST_START_VIDEO 144
#define CIPC_OUTPUT_REQUEST_STOP_VIDEO	145
#define CIPC_OUTPUT_INDICATE_VIDEO		146

/////////////////////////////////////////////////////////////////////////////
// Possible Output-Activations:
enum SecOutputType {
	OUTPUT_OFF = -1,
	OUTPUT_CAMERA = 0,
	OUTPUT_RELAY = 1,
	OUTPUT_AUDIO =2
};

#endif
