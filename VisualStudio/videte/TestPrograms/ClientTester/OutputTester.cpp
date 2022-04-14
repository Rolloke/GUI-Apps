///////////////////////////////////////////////////////////////
// FILE   :	$Workfile: OutputTester.cpp $
// ARCHIVE:	$Archive: /Project/Tools/ClientTester/OutputTester.cpp $
// CHECKIN:	$Date: 12.06.97 23:05 $
// VERSION:	$Revision: 6 $
// LAST   :	$Modtime: 12.06.97 23:04 $
// AUTHOR :	$Author: Hedu $
//        : $Nokeywords:$

#include "stdafx.h"

#include "TickCount.h"
#include "TestTool.h"
#include "OutputTester.h"
#include "PictureRecord.h"
#include "PictureDef.h"
#include "util.h"

#include "ClientPicTest.h"

#include "CipcOutputClient.h"
#include "CipcExtraMemory.h"
#include "WK_Names.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef WK_MASTER
	#define WK_MASTER TRUE
	#define WK_SLAVE  FALSE
#endif

#define CONNECT 0
#define INDICAT 1


int COutputTester::m_iMicoFPS = 16;				// also set in CSec3App::Reset!
int COutputTester::m_iCocoFPS = 12;				// also set in CSec3App::Reset!
int COutputTester::m_iSWCodecVirtualFPS = 16;	// also set in CSec3App::Reset!
int COutputTester::m_iSWCodecCountLow = 2;		// also set in CSec3App::Reset!
int COutputTester::m_iSWCodecCountMedium = 3;	// also set in CSec3App::Reset!
int COutputTester::m_iSWCodecCountHigh= 4;		// also set in CSec3App::Reset!


/////////////////////////////////////////////////////////////////////////////
// COutputTester
/////////////////////////////////////////////////////////////////////////////
/*@TOPIC{COutputTester Overview|COutputTester,Overview}
@RELATED @LINK{members|COutputTester},
*/
/*
@MLIST @RELATED @LINK{COutputTesterClient}
*/

/*@MHEAD{constructor:}*/
/*@MD NYD */
COutputTester::COutputTester(
	CTestTool* l_pCTestTool, 
	const CString& pSMName,
	BOOL bAsMaster )
	: CIPCOutputClient(pSMName, bAsMaster)//, CIOGroup(pSMName)
{
//	m_bIsSWCodec          = FALSE;
	// OLD the following values are set in OnConfirmHardware
	// NEW 0603997 calc from ShmName 
	m_bCanSWCompress      = FALSE;
	m_bCanBWDecompress    = FALSE;
	m_bCanColorCompress   = FALSE;
	m_bCanColorDecompress = FALSE;
	m_bCanOverlay         = FALSE;
	// picture data
	m_compressionType	= COMPRESSION_UNKNOWN;
	m_iFramesPerSecond	=  0;
	m_iHardware			= -1;
	m_dwVersion			=  0;
	m_wGroupID          = 0x1000;
	m_dwState			= 0L;
	// these are brand new keys
//	m_shmName			= l_pCTestTool->GetShmName();
	m_Compression		= l_pCTestTool->GetCompression();	
	m_Resolution		= l_pCTestTool->GetResolution();
	m_wNumPictures		= l_pCTestTool->GetNumPictures();
	m_dwBitrateScale	= l_pCTestTool->GetBitrateScale();
	m_iLoop				= l_pCTestTool->GetLoop();
	m_bIsServer			= l_pCTestTool->GetIsServer();
	m_dRes_time			= 0.0;
	m_dRes_len			= 0.0;
	m_bInfo				= FALSE;

	// NEW HEDU
	m_bDoStatLogs	= FALSE;
	if (l_pCTestTool) {
		m_bDoStatLogs		= l_pCTestTool->GetDoStatLogs();
	}

	CString sShm(pSMName);
	sShm = l_pCTestTool->GetShmName();
	if (sShm==SM_SWCODEC_OUTPUT) 
	{
		m_compressionType  = COMPRESSION_JPEG;
		m_iFramesPerSecond = m_iSWCodecVirtualFPS;
		m_bIsSWCodec       = TRUE;
		m_bCanSWCompress   = TRUE;
		m_bCanBWDecompress = TRUE;
	} 
	else if (  sShm==SM_COCO_OUTPUT_CAM0
			|| sShm==SM_COCO_OUTPUT_CAM1
			|| sShm==SM_COCO_OUTPUT_CAM2
			|| sShm==SM_COCO_OUTPUT_CAM3) 
	{					    
		m_compressionType     = COMPRESSION_H261;
		// static member set via ServerDebug.cfg
		m_iFramesPerSecond    = m_iCocoFPS;	
		m_bCanColorCompress   = TRUE;
		m_bCanColorDecompress = TRUE;
		m_bCanOverlay         = TRUE;
	} 
	else if (sShm==SM_MICO_OUTPUT_CAMERAS) 
	{
		m_compressionType   = COMPRESSION_COLORJPEG;
		// static member set via ServerDebug.cfg
		m_iFramesPerSecond  = m_iMicoFPS;
		m_bCanColorCompress = TRUE;
	} 
	else 
	{
		m_compressionType = COMPRESSION_UNKNOWN;
	}
	m_iHardware = -1;
}

/*@MD NYD */
COutputTester::~COutputTester()
{
 StopThread();
}

/**funcref ***************************************************
 * class   : COutputTester 
 * name    : OnReadChannelFound(
 * input   : );	 
 *         : 
 * output  : nil
 *         :
 * purpose : initial connection
 *         :
 * uses    : WK_TRACE(...);
 *         : DoRequestConnection();
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 09.04.1997
 *         : 23.05.1997	self tcp: check added
 *************************************************************/
/*
@MHEAD{connection:}
@MLIST The initial connection is made in the following order:
@MD
*/
void COutputTester::OnReadChannelFound()
{
	WK_TRACE("OnReadChannelFound\n");
	WK_TRACE("\
		Compression  = %d\n\
		Resolution   = %d\n\
		NumPictures  = %d\n\
		BitrateScale = %d\n\
		Loop         = %d\n",
		m_Compression,
		m_Resolution,
		m_wNumPictures,
		m_dwBitrateScale,
		m_iLoop
		);
	DoRequestConnection();
}

/*@MD NYD */
void COutputTester::OnConfirmConnection()
{
	WK_TRACE("OnConfirmConnection:\n");
	CString sMsg;
	sMsg.Format("OutputUnit=%s",GetShmName());
	if ( m_bDoStatLogs ) {
		WK_STAT_LOG("Unit",1,sMsg);
	}

	if(m_bIsServer)
	{
	 CIPC::OnConfirmConnection();
	 DoRequestSetGroupID(m_wGroupID);
	}
	else
	{
	 WK_TRACE("Do->RequestInfoOutputs:\n");
	 CIPC::OnConfirmConnection(); /// ??????????????
 	 DoRequestInfoOutputs(SECID_NO_GROUPID);	// alle groups
	}
}

void COutputTester::OnConfirmInfoOutputs(
	WORD wGroupID, 
	int iNumGroups, 
	int numRecords, 
	const CIPCOutputRecord records[])
{
 CIPCOutputClient::OnConfirmInfoOutputs(wGroupID,iNumGroups,numRecords,records);
 WK_TRACE("OnConfirmInfoOutputs:\n GroupID=%x\n m_wGroupID=%x\n",
	 wGroupID,m_wGroupID);
 m_bInfo=TRUE;
 m_wGroupID=wGroupID;

 CSecID camID;
 // such die kamera!
	for(int i = 0; 
		i < numRecords 
		&& camID.GetID()==SECID_NO_ID;
		i++)
	{
	 const CIPCOutputRecord & oneOutput = records[i];
		if (   (oneOutput.IsCamera())
			&& (oneOutput.CameraDoesJpeg()==FALSE)
			&& (oneOutput.CameraHasColor()==TRUE) )
		{
			camID= oneOutput.GetID();
		}
	}
  	 WK_TRACE("Do->camID=%x\n",camID);

 {
// 	 WK_TRACE("Do->TheBoggyWoggy:\n");
	 m_CS.Lock();
	 m_dwState              = 0L;
	 Resolution			res = m_Resolution;
	 Compression	   comp = m_Compression;
	 WORD	   wNumPictures = m_wNumPictures;
	 DWORD	 dwBitrateScale = m_dwBitrateScale;
	 DWORD	     dwUserData = 0L;
	 m_CS.Unlock();

/*
	 if (GetCompressionType()==COMPRESSION_H261) 
	 {	// make sure encoding is stopped
		DoRequestStopMpegEncoding(camID);
		WK_TRACE("DidHeRequestStopMpegEncoding?:\nyes, he did, didn't he?\n");
	 }
*/
/*	 2. fetch zum local server
	 CRect rect(10, 10, 704, 576 );
	 DoRequestSetDisplayWindow( m_wGroupID, rect );
	 WK_TRACE("DidHeRequestSetDisplayWindow?:\nyes, he did, didn't he?\n");
	 DoRequestSetOutputWindow ( m_wGroupID, rect );
	 WK_TRACE("DidHeRequestSetOutputWindow?:\nyes, he did, didn't he?\n");
*/
//	 for(int i = 0; i < m_iLoop; i++)
	 {
//	  DoRequestEncodedVideo(	
	  DoRequestStartMpegEncoding(
		camID, 
		res,
		comp,
//		wNumPictures,
		dwBitrateScale,
		100//dwUserData
		);
	  m_TickCounter[INDICAT].StartCount();
	  WK_TRACE("DidHeRequestEncodedVideo?\nyes, he did, didn't he?\n");
	 }

 }
}

/*@MD NYD */
/*
void COutputTester::OnConfirmSetGroupID(WORD wGroupID)
{
 DoRequestHardware(m_wGroupID);
}
*/
/*@MD NYD */
/*
void COutputTester::OnConfirmHardware(
		WORD wGroupID,
		int iErrorCode,
		BOOL bCanSWCompress,
		BOOL bCanBWDecompress,
		BOOL bCanColorCompress,
		BOOL bCanColorDecompress,
		BOOL bCanOverlay )
{
	ASSERT(wGroupID==m_wGroupID);
	m_CS.Lock();
	m_iHardware           = iErrorCode;
	// picture data	  
	m_bCanSWCompress      = bCanSWCompress;
	m_bCanBWDecompress    = bCanBWDecompress;
	m_bCanColorCompress   = bCanColorCompress;
	m_bCanColorDecompress = bCanColorDecompress;
	m_bCanOverlay         = bCanOverlay;
	m_CS.Unlock();

	WK_TRACE("OnConfirmHardware: \n");

	if (m_iHardware==0) 
	{
		DoRequestReset(m_wGroupID);
	}
	
}
*/
/*@MHEAD{disconnect:}*/
/*@MD NYD */
void COutputTester::OnRequestDisconnect()
{
	CString sMsg;
	sMsg.Format("OutputUnit|%s",GetShmName());
	if ( m_bDoStatLogs ) {
		WK_STAT_LOG("Unit",0,sMsg);
	}
	m_iHardware = -1;

}

/*--------------------------@MHEAD{jpeg:}*/
/*@MD NYD */
void COutputTester::OnConfirmJpegEncoding(
		const CIPCPictureRecord &pict,
		DWORD dwUserData )
{
}

/*@MD NYD */
void COutputTester::OnConfirmJpegDecoding(WORD wGroupID,
		WORD wXSize, WORD wYSize, 
		DWORD dwUserData,
		const CIPCPictureRecord &pict )
{
}

/*--------------------------@MHEAD{local video (bitmap):}*/
/*@MD NYD */
void COutputTester::OnIndicateLocalVideoData(
		CSecID camID,
		WORD wXSize, WORD wYSize, 
		const CIPCPictureRecord &pict )
{
}

/*@MD NYD */
void COutputTester::OnConfirmStopMpegEncoding(CSecID camID)
{
	WK_TRACE("MPEG stopped for %x\n",camID.GetID());
}

/*@MD NYD */
/*
void COutputTester::OnConfirmMpegDecoding(
		WORD wGroupID, 
		DWORD dwUserData )
{
}
*/
/*--------------------------@MHEAD{bitmap from overlay:}*/
/*@MD NYD */
/*
void COutputTester::OnConfirmDumpOutputWindow(
		WORD wGroupID,
		WORD wUserID,
		int iFormat,
		const CIPCExtraMemory &bitmapData )
{
}
*/
/*--------------------------@MHEAD{CIPC value:}*/
/*@MD NYD */
/*
void COutputTester::OnConfirmGetValue(
		CSecID id, // might be used as group ID only
		const CString &sKey,
		const CString &sValue,
		DWORD dwServerData )
{
}
*/
/*@MD NYD */
/*
void COutputTester::OnConfirmSync(
		DWORD dwTickConfirm, 
		DWORD dwTickSend, 
		DWORD dwType,
		DWORD dwUserID )
{
	// NOT YET DROPPPED
}
*/
////////////////////////////////////
/*@MHEAD{unsorted:}*/
/*@MD NYD */
void COutputTester::OnConfirmStartMpegEncoding(CSecID camID)
{
	// NOT YET
}

/*@MD NYD */
/*
void COutputTester::OnConfirmSetOutputWindow (
		WORD wGroupID, int iPictureResult)
{
	// NOT YET
}
*/
/*@MD NYD */
/*
void COutputTester::OnConfirmSetDisplayWindow(WORD wGroupID)
{
	// NOT YET
}
*/
/*@MD NYD */
/*
void COutputTester::OnConfirmReset(WORD wGroupID)
{
	m_CS.Lock();
	m_dwState               = 0L;
	m_CS.Unlock();
	Resolution			res = m_Resolution;
	Compression		   comp = m_Compression;
	WORD	   wNumPictures = m_wNumPictures;
	DWORD	 dwBitrateScale = m_dwBitrateScale;
	DWORD	     dwUserData = 0L;
    CSecID camID(m_wGroupID,0);

	if (m_compressionType==COMPRESSION_H261) 
	{	// make sure encoding is stopped
		DoRequestStopMpegEncoding(camID);
	}
//	DoRequestCurrentState(m_wGroupID);
	
	// overlay:
	CRect rect(10, 10, 704, 576 );
	DoRequestSetDisplayWindow( m_wGroupID, rect );
	DoRequestSetOutputWindow ( m_wGroupID, rect );

	for(int i = 0; i < m_iLoop; i++)
	{
	 DoRequestEncodedVideo(	
		camID, 
		res,
		comp,
		wNumPictures,
		dwBitrateScale,
		dwUserData
		);
	 m_TickCounter[INDICAT].StartCount();
	}
}
*/
/*--------------------------@MHEAD{mpeg:}*/
/*@MD NYD */
void COutputTester::OnIndicateMpegData(
		const CIPCPictureRecord &pict, 
		DWORD dwJobData )
{
 static int           iAccu_time = 0;
 static int            iAccu_len = 0;
 static int iNumPicturesReceived = 0; 
 static int             iNumPics = 0; 
 iNumPicturesReceived++;

 	if ( m_bDoStatLogs ) {
		WK_STAT_LOG("Reset",pict.GetDataLength(),"PicData");
	}

 m_TickCounter[INDICAT].StopCount();
 
 double l_ticks= (double)m_TickCounter[INDICAT].GetDeltaTicks();
/*
 WK_TRACE("Picture[%3d] cam%2d Len %d\n",
	iNumPicturesReceived,
	pict.GetCamID().GetSubID(),
	pict.GetDataLength()
	);
*/
 iAccu_time += l_ticks;
 iAccu_len  += pict.GetDataLength();
 iNumPics++;

 if(iAccu_time >= 1000)
 {
  m_dRes_len  += (double)iAccu_len;
  m_dRes_time += (double)iAccu_time;

  if(iAccu_time)
  {
   WK_TRACE("iAccu_len = %6d [bits] in 1[s] numPic/s=%d\n",
		iAccu_len * 8000/iAccu_time, 
		iNumPics
		);
  }
  iAccu_len  = 0;
  iAccu_time = 0;
  iNumPics   = 0;
 }
 else
 {
/*
   WK_TRACE("\niAccu_len = %d [bits] in %d [ms]\n",
		iAccu_len * 8,
		iAccu_time
		);
*/
 }

 if(iNumPicturesReceived >= m_wNumPictures * m_iLoop )
 {
	if(m_dRes_time)
	{
	 WK_TRACE("\nm_dRes_len = %d [bits] in 1[s] nach %d stück\n",
		(int)(m_dRes_len * 8000.0/m_dRes_time),
		iNumPicturesReceived );
	}
	DoRequestStopMpegEncoding(pict.GetCamID());

	DelayedDelete();	// make sure we not called a secdond time
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
	// GetMain()->PostMessage(WM_CLOSE);
 }
 m_TickCounter[INDICAT].StartCount();
}
