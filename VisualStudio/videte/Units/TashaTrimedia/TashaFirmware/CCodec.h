/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CCodec.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CCodec.h $
// CHECKIN:		$Date: 13.12.01 13:27 $
// VERSION:		$Revision: 39 $
// LAST CHANGE:	$Modtime: 13.12.01 13:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CCodec_H__
#define __CCodec_H__

#include "helper.h"	// Added by ClassView
#if defined(__cplusplus)
extern "C" {
#endif				

#define MAX_DATA_PACKETS 25	 

typedef enum eMode
{
	modeFrame	= 1,
	modeField	= 2,
	modeCIF		= 3					   
} eMode;							      

#include "CAlarm.h"
#include "CRelay.h"

class CPerfMon;
class CJpeg;
class CVideoIn;
class CVideoOut;
class CMotionDetection;
class CMDPoints;
class CCodec
{
public:
	CCodec();
	virtual ~CCodec();

	void Run();

protected:

	virtual BOOL Open();
	virtual void Close();

	virtual BOOL OpenMessageChannel();
	virtual BOOL CloseMessageChannel();

	virtual BOOL CloseDataChannel();
	virtual BOOL OpenDataChannel();

	virtual BOOL ReceiveMessage(int& nMessageID, DWORD& dwParam1, DWORD& dwParam2, DWORD& dwParam3, DWORD& dwParam4);
	virtual void MessageLoop();
	virtual BOOL SendMessage(int nMessageID, DWORD dwParam1=0, DWORD dwParam2=0, DWORD dwParam3=0, DWORD dwParam4=0);

	BOOL StartCapture();
	BOOL StopCapture();

	BOOL SetInputSource();
	BOOL PollAlarm();
	BOOL DoNotifyAlarm(DWORD dwAlarmState);

	void OnCaptureNewFrame(const exVideoBuffer* pExVideoBuffer);
	
	BOOL DoReceiveNewCodecData();
	BOOL OnConfirmReceiveNewCodecData();

	BOOL OnRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnRequestGetContrast(DWORD dwUserData, WORD wSource);
	BOOL DoConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnRequestGetBrightness(DWORD dwUserData, WORD wSource);
	BOOL DoConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnRequestGetSaturation(DWORD dwUserData, WORD wSource);
	BOOL DoConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnRequestResumeCapture(); 
	BOOL DoConfirmResumeCapture(); 

	BOOL OnRequestPauseCapture(); 
	BOOL DoConfirmPauseCapture(); 

	BOOL OnRequestStopCapture(); 
	BOOL DoConfirmStopCapture(); 

	BOOL OnRequestStartCapture(); 
	BOOL DoConfirmStartCapture(); 

	BOOL OnRequestSetInputSource(DWORD dwParam1, DWORD dwParam2);
	BOOL DoConfirmSetInputSource(DWORD dwProcessID, WORD wSource);

	BOOL OnRequestGetInputSource();
	BOOL DoConfirmGetInputSource(WORD wSource);

	BOOL OnRequestScanForCameras(DWORD dwUserData);
	BOOL DoConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);

	BOOL OnRequestSetRelayState(WORD wRelayID, BOOL bOpenClose);
	BOOL DoConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);

	BOOL OnRequestGetRelayState();
	BOOL DoConfirmGetRelayState(DWORD dwRelayStateMask);

	BOOL OnRequestGetAlarmState();
	BOOL DoConfirmGetAlarmState(DWORD dwAlarmStateMask);

	BOOL OnRequestSetAlarmEdge(DWORD dwAlarmEdgeMask);
	BOOL DoConfirmSetAlarmEdge(DWORD dwAlarmEdgeMask);

	BOOL OnRequestGetAlarmEdge();
	BOOL DoConfirmGetAlarmEdge(DWORD dwAlarmEdgeMask);

	BOOL DoConfirmInitComplete();

	void DemoFilter(const exVideoBuffer* pExVideoBuffer);

	BOOL  AllocBuffers();
	UInt8* allocSz(int bufSz);

	void FreeBuffers();

	void SetDP();

	static void mmBufUpdateThread();
	static void DataTransferThread();

	static CCodec*	m_pThis;

private:
	exVideoBuffer m_genBuf[NUM_BUF_ENTRIES];
	
	CVideoIn* 			m_pVideoIn;
	CVideoOut* 			m_pVideoOut;
	CMotionDetection*	m_pMD;
	CAlarm				m_Alarm;
	CRelay				m_Relay;
	Int					m_nMmNum;

	Int					m_yFieldStride;
	Int					m_uvFieldStride;
	Int					m_nWidth;
	Int					m_nHeight;
	viYUVModes_t		m_viYUVMode;
	voYUVModes_t		m_voYUVMode;
	BOOL				m_bCaptureFrame;

	ULONG				m_taskBufferUpdate;
	ULONG				m_SemExitBufferUpdateThreadConfirm;
	ULONG				m_SemExitDataTransferThreadConfirm;
	ULONG				m_SemBufferMMReady;
	ULONG				m_SemNewDataInQueue;

	ULONG				m_taskDataTransfer;

	CInt64				m_u64FrameCounter;
	char				m_sFilePattern[256];

	CJpeg*				m_pJpeg;
	CPerfMon*			m_pPerf1;
	CPerfMon*			m_pPerf2;

	BOOL				m_bPause;
	BOOL				m_bRun;
	
	// Messagechannel
	UInt32				m_hDSP;
	AppSem_Semaphore	m_hMessageSynch;
	UInt32				m_hMessage;

	// Shared Memory
	UInt32				m_dwSMBufferSize;
	DataPacket*			m_pDataPacket;
	UInt32				m_hSMBufferHandle;
	
	// Dataqueue
	DataPacket*			m_pDataQueue[MAX_DATA_PACKETS];
	int					m_nRIndex;
	int					m_nWIndex;

	// Sourceselection
	SOURCE_SELECTION	m_ReqSrcSel1;
	SOURCE_SELECTION	m_ReqSrcSel2;

	DWORD				m_dwAlarmState;
};

#if defined(__cplusplus)
}
#endif

#endif __CCodec_H__
