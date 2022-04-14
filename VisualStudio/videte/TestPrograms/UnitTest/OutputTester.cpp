///////////////////////////////////////////////////////////////
// FILE   :	$Workfile: OutputTester.cpp $
// ARCHIVE:	$Archive: /Project/Tools/UnitTest/OutputTester.cpp $
// CHECKIN:	$Date: 25.04.97 20:14 $
// VERSION:	$Revision: 5 $
// LAST   :	$Modtime: 25.04.97 20:14 $
// AUTHOR :	$Author: Hedu $
//        : $Nokeywords:$

#include "stdafx.h"
#include "TickCount.h"
#include "TestTool.h"
#include "OutputTester.h"
#include "PictureRecord.h"
#include "PictureDef.h"
#include "util.h"

#include "CipcExtraMemory.h"
#include "WK_Names.h"

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
		const CString& pSMName )
		: CIPCOutput(pSMName, WK_MASTER)
{
	m_bIsSWCodec          = FALSE;
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
	m_iLoop				= l_pCTestTool->GetLoop();			//5
	m_dRes_time			= 0.0;
	m_dRes_len			= 0.0;

	CString sShm(pSMName);
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

/*@MHEAD{destructor:}*/
/*@MD NYD */
void COutputTester::SetShutdownInProgress()
{
}


/*@MD NYD */
COutputTester::~COutputTester()
{
	StopThread();
//	WK_DELETE(m_pProcessScheduler);
//	DeleteAllOutputs();
}

/*@MHEAD{timeouts:}*/
/*@MD NYD */
BOOL COutputTester::OnTimeoutCmdReceive()
{
	if (GetIPCState()==CIPC_STATE_READ_FOUND) 
	{
		// OLD DoRequestConnection();
	} 
	else if (GetIPCState()==CIPC_STATE_CONNECTED) 
	{
		if (GetNumTimeoutsCmdReceive()*GetTimeoutCmdReceive()>3*60*1000) 
		{
			DoRequestCurrentState(m_wGroupID);
		}
	} 
	else 
	{
	}
 return TRUE;	// OOPS
}

/*@MD NYD */
BOOL COutputTester::OnTimeoutWrite(DWORD dwCmd)
{
	// NOt YET make use of dwCmd
#if 1
	if (GetIPCState()!=CIPC_STATE_CONNECTED) 
	{
		// Sleep(1000);	// wait a second and try to reconnect
		// DoRequestConnection();
		return FALSE;
	} 
	else 
	{
		if (GetNumTimeoutsWrite()*GetTimeoutCmdReceive()>15000) 
		{
			TRACE("%s:Timeout %d times, giving up, trying to reconnect...\n",GetShmName(),GetNumTimeoutsWrite());
			m_iHardware = -1;
			// DoRequestConnection();
		} 
		else 
		{
			// retry
			// DoRequestCurrentState(m_wGroupID);
		}
	}
#endif

	return TRUE;	// retry
}


// s:		Output-type as string [camera, isdn, audio, relay]
// RETURN:	Output-type.
SecOutputType COutputTester::TypeName2TypeNr(const char *s)
{
	if (_stricmp(s, "relay")==0) 
	{
		return OUTPUT_RELAY;
	} 
	else if (_stricmp(s, "audio")==0) 
	{
	 	return OUTPUT_AUDIO;
	} 
	else if (_stricmp(s,"camera")==0) 
	{
		return OUTPUT_CAMERA;
	} 
	else if (_stricmp(s,"off")==0) 
	{
		return OUTPUT_OFF;
	} 
	else 
	{
		TRACE("TypeName2TypeNr:Invalid OutputType:%s!\n",s);
		return OUTPUT_OFF;
	}
}

// iTypeNr:	TypeNummer [P_CAMERA usw ]
const char *COutputTester::TypeNr2TypeName(int iTypeNr)
{
	if (iTypeNr==OUTPUT_CAMERA)
	{	
		return "camera";
	}
	else if (iTypeNr==OUTPUT_RELAY)	
	{	
		return "relay";
	}
	else if (iTypeNr==OUTPUT_AUDIO)	
	{	
		return "audio";
	}
	else if (iTypeNr==OUTPUT_OFF)	
	{	
		return "off";
	}
//	else // in any case

 return "invalid output";
}

/*@MHEAD{connection:}*/
/*@MLIST The initial connection is made in the following order:*/
/*@MD NYD */
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
	sMsg.Format("OutputUnit|%s",GetShmName());
	WK_STAT_LOG("Unit",1,sMsg);
	CIPC::OnConfirmConnection();

//	DoRequestVersionInfo(GetGroupID());
	DoRequestSetGroupID(m_wGroupID);
//	pApp->UpdateStateInfo();
}

/*@MDT{optional} NYD */
void COutputTester::OnConfirmVersionInfo(
	WORD wGroupID, 
	DWORD dwVersion)
{
	m_dwVersion = dwVersion;
	WK_TRACE("%s has version %d\n",GetShmName(),dwVersion);
}

/*@MD NYD */
void COutputTester::OnConfirmSetGroupID(WORD wGroupID)
{
/*
	for (int i=0;i<GetSize();i++) 
	{
		OUTPUT *pOutput=GetOutputAtWritable(i);
		pOutput->m_bRelayClosed = FALSE;
	}
*/
	DoRequestHardware(m_wGroupID);
}

/*@MD NYD */
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

	if (m_iHardware==0) 
	{
		DoRequestReset(m_wGroupID);
	}
	
}

/*@MHEAD{disconnect:}*/
/*@MD NYD */
void COutputTester::OnRequestDisconnect()
{
	CString sMsg;
	sMsg.Format("OutputUnit|%s",GetShmName());
	WK_STAT_LOG("Unit",0,sMsg);
	m_iHardware = -1;

}

/*@MHEAD{relay:}*/
/*@MD NYD */
void COutputTester::OnConfirmSetRelay(
		CSecID relayID, 
		BOOL bClosed )
{
	WORD ix = relayID.GetSubID();
	m_CS.Lock();
	// OOPS m_dwState ?
	if (bClosed) 
	{
		m_dwState |= (1L<<relayID.GetSubID());
	} 
	else 
	{
		m_dwState &= ~(1L<<relayID.GetSubID());
	}
	m_CS.Unlock();

}

/*@MHEAD{state:}*/
/*@MD NYD */
void COutputTester::OnConfirmCurrentState(WORD wGroupID, DWORD stateMask)
{
	m_CS.Lock();
	m_dwState = stateMask;	// Bitmask-Status der Ausgänge
	m_CS.Unlock();
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
void COutputTester::OnConfirmMpegDecoding(
		WORD wGroupID, 
		DWORD dwUserData )
{
}

/*--------------------------@MHEAD{bitmap from overlay:}*/
/*@MD NYD */
void COutputTester::OnConfirmDumpOutputWindow(
		WORD wGroupID,
		WORD wUserID,
		int iFormat,
		const CIPCExtraMemory &bitmapData )
{
}

/*--------------------------@MHEAD{CIPC value:}*/
/*@MD NYD */
void COutputTester::OnConfirmGetValue(
		CSecID id, // might be used as group ID only
		const CString &sKey,
		const CString &sValue,
		DWORD dwServerData )
{
}

/*@MD NYD */
void COutputTester::OnConfirmSync(
		DWORD dwTickConfirm, 
		DWORD dwTickSend, 
		DWORD dwType,
		DWORD dwUserID )
{
	// NOT YET DROPPPED
}

////////////////////////////////////
/*--------------------------@MHEAD{unsorted:}*/
/*@MD NYD */
void COutputTester::OnConfirmStartMpegEncoding(CSecID camID)
{
	// NOT YET
}

/*@MD NYD */
void COutputTester::OnConfirmSetOutputWindow (
		WORD wGroupID, int iPictureResult)
{
	// NOT YET
}

/*@MD NYD */
void COutputTester::OnConfirmSetDisplayWindow(WORD wGroupID)
{
	// NOT YET
}

/*@MD NYD */
void COutputTester::OnConfirmSetUp(WORD wGroupID)
{
	// OOPS
}

/*@MD NYD */
void COutputTester::OnWaitFailed(DWORD dwCmd)
{
	// NOT YET
}

// SWCodecScale
/*@MD */
int COutputTester::SWCodecUpScale(
		int iNormalFPS,
		Resolution res) const
{
	int iFPS;
	switch (res) 
	{
		case RESOLUTION_HIGH:
			iFPS = iNormalFPS * m_iSWCodecCountHigh;
			break;
		case RESOLUTION_MID:
			iFPS = iNormalFPS * m_iSWCodecCountMedium;
			break;
		case RESOLUTION_LOW:
			iFPS = iNormalFPS * m_iSWCodecCountLow;
			break;
		default:
			iFPS = iNormalFPS;
	}
	return iFPS;
}
// SWCodecScale
/*@MD */
int COutputTester::SWCodecDownScale(
		int iScaledFPS, 
		Resolution res) const
{
	int iFPS;
	if (iScaledFPS) 
	{
		switch (res) 
		{
			case RESOLUTION_HIGH:
				iFPS = iScaledFPS / m_iSWCodecCountHigh;
				break;
			case RESOLUTION_MID:
				iFPS = iScaledFPS / m_iSWCodecCountMedium;
				break;
			case RESOLUTION_LOW:
				iFPS = iScaledFPS / m_iSWCodecCountLow;
				break;
			default:
				iFPS = iScaledFPS;
		}
	} 
	else 
	{
		// OOPS 0 pics !?
	}	

	return iFPS;
}

/*@MD NYD */
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

	if (GetCompressionType()==COMPRESSION_H261) 
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

/*--------------------------@MHEAD{mpeg:}*/
/*@MD NYD */
#define PRINT_ONE_LINE f.Write(sOneLine,sOneLine.GetLength());
void COutputTester::OnIndicateMpegData(
		const CIPCPictureRecord &pict, 
		DWORD dwJobData )
{
 static int           iAccu_time = 0;
 static int            iAccu_len = 0;
 static int iNumPicturesReceived = 0; // NOT YET member
 static int             iNumPics = 0; 
 iNumPicturesReceived++;

#if 0
 // HEDU dump IFrame
 static BOOL bDoPrintFullPictures=TRUE;
 static int iNumFullPictures=0;
 if (bDoPrintFullPictures
	 && pict.GetPictureType()==SPT_FULL_PICTURE) {
	 CString sFilename;
	 iNumFullPictures++;
	 sFilename.Format("C:\\Log\\ipic%03d.dat",iNumFullPictures);
	 CFile f;
	 if (f.Open(sFilename, CFile::modeCreate | CFile::modeWrite)) {
		CString sOneLine;
		CString sNewLine="\n\t";
		CString sOneByte;
		// print header
		sOneLine="\n#include \"stdafx.h\"\n";
		PRINT_ONE_LINE;
		sOneLine="\n#include \"PictureDef.h\"\n";
		PRINT_ONE_LINE;
		sOneLine="\n#include \"PictureRecord.h\"\n";
		PRINT_ONE_LINE;

		sOneLine="\nextern CIPCPictureRecord *CreateEmptyPicture(const CIPC *pCipc);\n\n\n";
		PRINT_ONE_LINE;

		sOneLine.Format("static DWORD dwEmptyIFrameLen = %d;\n",pict.GetDataLength());
		PRINT_ONE_LINE;
		sOneLine.Format("static BYTE pEmptyIFrame[] = { // len %d",pict.GetDataLength());
		PRINT_ONE_LINE;
		// print data
		for (int i=0;i<pict.GetDataLength();i++) {
			if (i==0 || i % 10==0) {
				f.Write(sNewLine,sNewLine.GetLength());
			}
			sOneByte.Format("0x%x,",pict.GetData()[i]);
			f.Write(sOneByte,sOneByte.GetLength());
		}
		 // print footer
		sOneLine="\n}; // end of pEmptyIFrame \n\n";
		PRINT_ONE_LINE;
		sOneLine="\nCIPCPictureRecord *CreateEmptyPicture(const CIPC *pCipc)\n";
		PRINT_ONE_LINE;
 		sOneLine="{\n";
		PRINT_ONE_LINE;
 		sOneLine="CIPCPictureRecord *pPic;\n";
		PRINT_ONE_LINE;
		sOneLine="pPic = new CIPCPictureRecord(\n";
		sOneLine +="\t\tpCipc,\n\t\tpEmptyIFrame,\n\t\tdwEmptyIFrameLen,\n";
		sOneLine += "\t\tSECID_NO_ID,\n";
		PRINT_ONE_LINE;
 		sOneLine.Format("\t\t%s,\n",NameOfEnum(pict.GetResolution()));
		PRINT_ONE_LINE;
 		sOneLine.Format("\t\t%s,\n",NameOfEnum(pict.GetCompression()));
		PRINT_ONE_LINE;
		sOneLine.Format("\t\t%s,\n",NameOfEnum(pict.GetCompressionType()));
		PRINT_ONE_LINE;
		sOneLine="\t\tCTime::GetCurrentTime(),\n";
		sOneLine += "\t\t0, NULL, 0, SPT_FULL_PICTURE, 0 );\n";
		PRINT_ONE_LINE;

		sOneLine="return pPic;\n} // end of CreateEmptyPicture\n";
		PRINT_ONE_LINE;

		f.Close();
	 } else {
		 // could not open file NOT YET
	 }
 }
#endif
 
 m_TickCounter[INDICAT].StopCount();

 WK_STAT_LOG("Reset",pict.GetDataLength(),"PicData");

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
  if(iAccu_time)
  {
   WK_TRACE("iAccu_len = %6d [bits] in 1[s] numPic/s=%d\n",
		iAccu_len * 8000/iAccu_time, 
		iNumPics
		);
  }
  m_dRes_len  += (double)iAccu_len;
  m_dRes_time += (double)iAccu_time;
  iAccu_len    = 0;
  iAccu_time   = 0;
  iNumPics     = 0;
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
	WK_TRACE("data_len = %d [bits] in 1[s] nach %d bildern\n",
		(int)(m_dRes_len * 8000.0/m_dRes_time),
		iNumPicturesReceived );
    AfxGetMainWnd()->PostMessage(WM_CLOSE);
 }
 m_TickCounter[INDICAT].StartCount();
}

