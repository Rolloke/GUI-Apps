/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CCodec.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/CCodec.h $
// CHECKIN:		$Date: 11.02.05 9:04 $
// VERSION:		$Revision: 104 $
// LAST CHANGE:	$Modtime: 11.02.05 8:53 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)
#define AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_

#include "..\TashaDA\DirectAccess.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TASHA_FIRMWARE	_T("TashaFirmware535.ldr")
#define EPLD_FILENAME	_T("Tasha.rbf")
#define BOOT_LOADER_533	_T("TashaBootloader533.ldr")
#define BOOT_IMAGE_533	_T("TashaFirmware533_BF%d.ldr")
#define BOOT_IMAGE_535	_T("BF535BootImage.ldr")

// Jede Message (Außer 'TMM_NEW_CODEC_DATA') werden durch jeweils einen eigenen Thread bearbeitet.
#define USE_MULTITHREADED_MESSAGE_HANDLING

class CCodec;

typedef struct
{
	int		nMessageID;
	DWORD	dwParam1;
	DWORD	dwParam2;
	DWORD	dwParam3;
	DWORD	dwParam4;
	CCodec* pCodec;
}MESSAGE;

typedef enum
{
	VideoStreamUnknown,
	VideoStreamValid,
	VideoStreamAborted
}VIDEO_STREAM_STATE;

class CCircularBuffer;
class AFX_EXT_CLASS CCodec  
{
public:
	CCodec(int nBoardID, const CString &sAppIniName);
	virtual ~CCodec();

	// Open CCodec-Object and inititialize the Tasha board
	// (Init EPLD, BF535, BF533_1....BF533_8)
	BOOL DoRequestOpenDevice();

	// Close CCodec-Object and Reset Tasha board
	void Close();
	
	BOOL SetMask(WORD wSubID, const CIPCActivityMask& mask);
	BOOL GetMask(WORD wSubID, CIPCActivityMask& mask);
	
	BOOL DoRequestNewSingleFrame(DWORD dwUserData, WORD wSource);
	BOOL DoRequestBF535Ping();
	BOOL DoRequestSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	BOOL DoRequestGetEncoderParam(DWORD dwUserData, WORD wSource, WORD wStreamID);
	BOOL DoRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoRequestSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	BOOL DoRequestSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoRequestSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	BOOL DoRequestSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoRequestSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoRequestSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	BOOL DoRequestChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue);
	BOOL DoRequestGetBrightness(DWORD dwUserData, WORD wSource);
	BOOL DoRequestGetContrast(DWORD dwUserData, WORD wSource);
	BOOL DoRequestGetSaturation(DWORD dwUserData, WORD wSource);
	
	BOOL DoRequestGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource);
	BOOL DoRequestGetMDTreshold(DWORD dwUserData, WORD wSource);
	BOOL DoRequestGetMDMaskSensitivity(DWORD dwUserData, WORD wSource);
	BOOL DoRequestGetMaskTreshold(DWORD dwUserData, WORD wSource);
	BOOL DoRequestGetMaskIncrement(DWORD dwUserData, WORD wSource);
	BOOL DoRequestGetMaskDelay(DWORD dwUserData, WORD wSource);
	BOOL DoRequestSetPermanentMask(DWORD dwUserData, WORD wSource, const MASK_STRUCT& mask, BOOL bWait);
	BOOL DoRequestClearPermanentMask(DWORD dwUserData, WORD wSource);
	BOOL DoRequestInvertPermanentMask(DWORD dwUserData, WORD wSource);
	BOOL DoRequestStartCapture(); 
	BOOL DoRequestStopCapture();   
	BOOL DoRequestPauseCapture(); 
	BOOL DoRequestResumeCapture(); 
	BOOL DoRequestScanForCameras(DWORD dwUserData);
	BOOL DoRequestSetRelayState(WORD wRelayID, BOOL bOpenClose);
	BOOL DoRequestGetRelayState();
	BOOL DoRequestGetAlarmState();
	BOOL DoRequestSetAlarmEdge(DWORD dwAlarmEdge);
	BOOL DoRequestGetAlarmEdge();
	BOOL DoRequestSetChannel(WORD wSource, WORD wNewChannel);
	BOOL DoRequestSetAnalogOut(DWORD dwCrosspointMask);
	BOOL DoRequestGetAnalogOut();
	BOOL DoRequestSetTerminationState(BYTE byTermMask);
	BOOL DoRequestGetTerminationState();
	BOOL DoRequestSetVideoEnableState(BOOL bState);
	BOOL DoRequestGetVideoEnableState();
	BOOL DoRequestSetCrosspointEnableState(BOOL bState);
	BOOL DoRequestGetCrosspointEnableState();
	BOOL DoRequestEnableWatchdog(int nBF533ResponseTimeOut);
	BOOL DoRequestTriggerWatchdog(int nWatchdogTimeOut);
	BOOL DoRequestSetPowerLED(BOOL bState);
	BOOL DoRequestSetResetLED(BOOL bState);
	BOOL DoRequestGetVariationRegister(WORD& wValue);
	BOOL DoRequestGetDIPState(int& nValue);
	BOOL DoRequestTermination();
	BOOL DoRequestSetPCIFrameBufferAddress(DWORD dwPCIAddr);
	BOOL DoRequestSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable);
	BOOL DoRequestGetNR(DWORD dwUserData, WORD wSource);

	BOOL TransferEPLDdata(const CString& sFileName);
	BOOL TransferBF533BootData(int nBFNr, int nMaxBootAttempts=MAX_BOOT_ATTEMPTS);
 	BOOL WriteEEPromData(const CString& sFileName);
 	BOOL ReadEEPromData(const CString& sFileName);
	void EnableFrameSync(BOOL bFrameSync);

	EncoderState  GetEncoderState(){return m_eEncoderState;}

	BOOL DoRequestWriteToEEProm(const BYTE* pData, DWORD dwLen, DWORD dwStartAddr);
	BOOL DoRequestReadFromEEProm(BYTE* pData, DWORD dwLen, DWORD dwStartAddr);
	BOOL LoadConfig(WORD wSource);
	void ReadDebugConfiguration();
	BOOL ReadDongleInformation();

protected:
	virtual BOOL OnConfirmOpenDevice(int nBoardID, int nErrors);
	virtual	BOOL OnConfirmInitComplete();

	virtual BOOL OnConfirmNewSingleFrame(DWORD dwUserData, WORD wSource);
	virtual BOOL OnConfirmBF535Ping();

	virtual BOOL OnConfirmSetMDMaskSensitivity(DWORD dwUserData,  WORD wSource, const CString &sLevel);
	virtual BOOL OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);
	virtual BOOL OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	virtual BOOL OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);

	virtual	BOOL OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	virtual	BOOL OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	virtual BOOL OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue);
	virtual BOOL OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetPermanentMask(DWORD dwUserData, WORD wSource, DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource);
	virtual BOOL OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource);
	virtual BOOL OnConfirmStartCapture(); 
 	virtual BOOL OnConfirmStopCapture(); 
	virtual BOOL OnConfirmPauseCapture(); 
	virtual BOOL OnConfirmResumeCapture(); 
	virtual BOOL OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);
	virtual BOOL OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);
	virtual BOOL OnConfirmGetRelayState(DWORD dwRelayStateMask);
 	virtual BOOL OnConfirmGetAlarmState(DWORD dwAlarmStateMask);
	virtual BOOL OnConfirmSetAlarmEdge(DWORD dwAlarmEdge);
	virtual BOOL OnConfirmGetAlarmEdge(DWORD dwAlarmEdge);
	virtual BOOL OnConfirmSetChannel(WORD wSource, WORD wNewChannel);
	virtual BOOL OnConfirmSetAnalogOut(DWORD dwCrosspointMask);
	virtual BOOL OnConfirmGetAnalogOut(DWORD dwCrosspointMask);
	virtual BOOL OnConfirmSetTerminationState(BYTE byTermMask);
	virtual BOOL OnConfirmGetTerminationState(BYTE byTermMask);
	virtual BOOL OnConfirmSetVideoEnableState(BOOL bState);
	virtual BOOL OnConfirmGetVideoEnableState(BOOL bState);
	virtual BOOL OnConfirmSetCrosspointEnableState(BOOL bState);
 	virtual BOOL OnConfirmGetCrosspointEnableState(BOOL bState);
	virtual BOOL OnConfirmEnableWatchdog(int nBF533ResponseTimeOut);
	virtual BOOL OnConfirmTriggerWatchdog(int nWatchdogTimeOut);
	virtual BOOL OnConfirmSetPowerLED(BOOL bState);
	virtual BOOL OnConfirmSetResetLED(BOOL bState);
	virtual BOOL OnConfirmGetVariationRegister(WORD wValue);
	virtual BOOL OnConfirmGetDIPState(int nValue);

	virtual BOOL OnNotifySlaveInitReady(WORD wSource);
	virtual BOOL OnConfirmTermination();
	virtual BOOL OnConfirmSetPCIFrameBufferAddress(DWORD dwPCIAddr);
	virtual BOOL OnConfirmEPLDdataTransfer(DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmTransferBootloader(DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);
	virtual BOOL OnConfirmReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);
	virtual BOOL OnConfirmSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable);
	virtual BOOL OnConfirmGetNR(DWORD dwUserData, WORD wSource, BOOL bEnable);

	virtual BOOL OnIndicationVideoStreamAborted(int nChannel);
	virtual BOOL OnIndicationVideoStreamResumed(int nChannel);
	virtual BOOL OnIndicationCameraStatusChanged(int nChannel, BOOL bState);
	virtual BOOL OnIndicationAlarmStateChanged(DWORD dwAlarmMask);
	virtual BOOL OnIndicationAdapterSelectStateChanged(DWORD dwAlarmMask);
	virtual BOOL OnIndicationPCKeysStateChanged(DWORD dwAlarmMask);
	virtual BOOL OnIndicationInformation(int nMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	virtual BOOL OnIndicationDebugInformation(DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnIndicationNewCodecData(DATA_PACKET* pDataPacket);
	virtual BOOL OnIndicationNoBF533Response(int nChannel);
	virtual BOOL OnIndicationNoBF535Response();

	virtual BOOL OnReceiveNewCodecData(int nSport, DWORD dwBufferOffset, DWORD dwPacketSize);
	virtual BOOL OnReceiveNewCodecDataEx(int nSport, DWORD dwBufferOffset, DWORD dwBufferLen);
	
	virtual void OnIndicateError(int nErrorCode);
	virtual void OnIndicateWarning(int nWarningCode);

private:
	CString GetHomeDir();
	int  SearchTashaBoard(int nBoardID);
	BOOL DoRequestEPLDdataTransfer(const BYTE* pData, DWORD dwLen);
	BOOL TransferBootloader(const CString &sFilename);
	BOOL DoRequestTransferBootloader(const BYTE* pData, DWORD dwLen);
	BOOL DoRequestBoot533(int nBFNr, const BYTE* pData, DWORD dwLen);
	void TriggerPiezo();
	BOOL ReadPermanentMaskFromRegistry(MASK_STRUCT* pPermanentMask, WORD wSource);
	BOOL SavePermanentMaskToRegistry(MASK_STRUCT* pPermanentMask, WORD wSource);
	BOOL SaveCurrentPermanentMask(MASK_STRUCT* pPermanentMask, WORD wSource);
	BOOL SaveCurrentAdaptiveMask(MASK_STRUCT* pAdaptiveMask, WORD wSource);
	BOOL GetCurrentPermanentMask(MASK_STRUCT* pPermanentMask, WORD wSource);
	BOOL GetCurrentAdaptiveMask(MASK_STRUCT* pAdaptiveMask, WORD wSource);
	BOOL CheckForEEPromUpdate(const CString& sImageName);
	void ForceNewIFrame(WORD wSource, WORD wStreamID);

	BOOL OnSendTestImage(DWORD dwUserData, WORD wSource, WORD wStreamID);

/* TODO: Sollte als friend von CEEProm deklariert werden
	BOOL DoRequestWriteToEEProm(const BYTE* pData, DWORD dwLen, DWORD dwStartAddr);
	BOOL DoRequestReadFromEEProm(BYTE* pData, DWORD dwLen, DWORD dwStartAddr);
*/
	static UINT ReadDataThread(LPVOID pData);
	static UINT MessageHandleThread(LPVOID pData);
	static UINT OpenDeviceThread(LPVOID pData);
	static UINT WaitForExternalEventThread(LPVOID pData);

	DWORD GetCheckSum(const DATA_PACKET* pPacket);
	BOOL LoadIntelHexfile(const CString &sFilename, BYTE*& pData, DWORD& dwLen);
	
	BYTE HexToInt(const CString &sS);

	LPBITMAPINFO CreateDIB(const DATA_PACKET* pPacket);

	BOOL SendMessage(int nMessageID, DWORD dwParam1=0, DWORD dwParam2=0, DWORD dwParam3=0,DWORD dwParam4=0);
	BOOL ReceiveMessage(int& nMessageID, DWORD& dwParam1, DWORD& dwParam2, DWORD& dwParam3, DWORD& dwParam4);
	BOOL CloseMessageChannel();
	BOOL OpenMessageChannel();
	static void __stdcall MessageReceiveCallback(LPVOID pData);

private:
	BOOL				m_bRun;
	CString				m_sAppIniName;
	HANDLE				m_hDSP;
	DWORD				m_dwEncodedFrames;
	WORD				m_wVariationRegister;
	BOOL				m_bEstablishMessageChannel;

	EncoderState		m_eEncoderState;
	ImageRes			m_eImageRes;
	
	// SourceSelection
	CEvent				m_evSrcSelection;
	CEvent				m_evTmmInitComplete;
	CEvent				m_evTmmTermination;
	CEvent				m_evEPLDdataTransfer;
	CEvent				m_evSetPCIFrameBufferAddress;
	CEvent				m_evEEPromDataTransfer;
	CEvent				m_evGetVariationRegister;
	CEvent				m_evBootLoader;
	CEvent				m_evBootBF533;
	CEvent				m_evSetPermanentMask;
	CEvent				m_evGetDIPState;
	
	HANDLE				m_hWatchDogTriggerEvent;
	HANDLE				m_hShutDownEvent;
	HANDLE				m_hPiezoEvent;

	static CCriticalSection	m_csSendMessage;
	static CCriticalSection m_csReceiveMessage;
	CCriticalSection	m_csBF533Boot;

	MEMSTRUCT			m_memTransferBuffer;
	MEMSTRUCT			m_memFrameBuffer;

	BYTE*				m_pFrameBuffer;

	HANDLE				m_hNewDataSemaphore;						// NewDataSemaphore Semaphore
	CWinThread*			m_pReadDataThread;							// Thread, der die einzelnen Frames aus den Daten holt			
	CWinThread*			m_pWaitForExternalEventThread;				// Thread, der auf externe Events reagiert (Watchdog, Piezo)			
	CCircularBuffer*	m_pCirBuff[CHANNEL_COUNT];					// Sammelkontainer für die eintreffenden Daten.
	BOOL				m_bFrameSync;

	DWORD				m_dwCamScanMask;							// Maske, die Auskunft gibt, welche Videoeingänge bereits geprüft wurden.
	DWORD				m_dwCamMask;								// Maske, mit den vom Video Frontend gelieferten Videostati
	DWORD				m_dwValidDataPacketMask;					// Maske der gültigen Videodaten 
	DWORD				m_dwLastValidDataPacketTC[CHANNEL_COUNT];	// Zeitpunkt der letzten gültigen Videodaten
	MASK_STRUCT			m_PermanentMask[CHANNEL_COUNT];				// Enthält die jeweils letzte erhaltene permanente Maske
	MASK_STRUCT			m_AdaptiveMask[CHANNEL_COUNT];				// Enthält die jeweils letzte erhaltene adaptive Maske
	VIDEO_STREAM_STATE  m_eVideoStreamState[CHANNEL_COUNT];			// Status des Videostreams
	DWORD				m_dwBF533RecoveryTimeout;					// Zeit in Millisekunden bis BF533-Timeoutmeldung
	DWORD				m_dwBF535RecoveryTimeout;					// Zeit in Millisekunden bis BF535-Timeoutmeldung
	DWORD				m_dwStreamRecoveryTimeout;					// Zeit in Millisekunden bis StreamAborted-Timeoutmeldung
	int					m_nPFrameCounter[CHANNEL_COUNT];			// Zählt die aufeinander folgenden P-Frames für Stream1
	int					m_nBF533BootAttempts;						// Anzahl der aufeinanderfolgenden Bootversuche eines DSPs

	int					m_nDIPState;
	int					m_nBoardID;
	
	int					m_nPendingBF535Pings;

	BOOL				m_bSilence;									// TRUE-> Alle Beeps aus
	BOOL				m_bIgnoreDIP;

	BYTE*				m_lpTestImage;
	DWORD				m_dwTestImageLen;

	// Tracemeldungen (An=TRUE / Aus=FALSE)
	BOOL	m_bTraceStartEncoder;
	BOOL	m_bTraceStopEncoder;
	BOOL	m_bTracePauseEncoder;
	BOOL	m_bTraceResumeEncoder;
	BOOL	m_bTraceSetBrightness;
	BOOL	m_bTraceSetContrast;
	BOOL	m_bTraceSetSaturation;
	BOOL	m_bTraceSetRelais;
	BOOL	m_bTraceSetAlarmEdge;
	BOOL	m_bTraceGetBrightness;
	BOOL	m_bTraceGetContrast;
	BOOL	m_bTraceGetSaturation;
	BOOL	m_bTraceGetRelais;
	BOOL	m_bTraceGetAlarmEdge;
	BOOL	m_bTraceSetMDTreshold;
	BOOL	m_bTraceSetMaskTreshold;
	BOOL	m_bTraceSetMaskIncrement;
	BOOL	m_bTraceSetMaskDelay;
	BOOL	m_bTraceSetPermanentMask;
	BOOL	m_bTraceChangePermanentMask;
	BOOL	m_bTraceClearPermanentMask;
	BOOL	m_bTraceInvertPermanentMask;
	BOOL	m_bTraceGetMDTreshold;
	BOOL	m_bTraceGetMaskTreshold;
	BOOL	m_bTraceGetMaskIncrement;
	BOOL	m_bTraceGetMaskDelay;
	BOOL	m_bTraceAlarmIndication;
	BOOL	m_bTraceAnalogOutSwitch;
	BOOL	m_bTraceSetAnalogOut;
	BOOL	m_bTraceGetAnalogOut;
	BOOL	m_bTraceSetTerminationState;
	BOOL	m_bTraceGetTerminationState;
	BOOL	m_bTraceSetVideoEnableState;
	BOOL	m_bTraceGetVideoEnableState;
	BOOL	m_bTraceSetCrosspointEnableState;
	BOOL	m_bTraceGetCrosspointEnableState;
	BOOL	m_bTraceEnableWatchdog;
	BOOL	m_bTraceTriggerWatchdog;
	BOOL	m_bTraceSetPowerLED;
 	BOOL	m_bTraceSetResetLED;
	BOOL	m_bTraceGetVariationRegister;
	BOOL	m_bTraceSetEncoderParam;
	BOOL	m_bTraceGetEncoderParam;
	BOOL	m_bTraceSPORTCheckSummError;
	BOOL	m_bTraceSPICheckSummError;
	BOOL	m_bTraceSPICommandTimeout;
	BOOL	m_bTraceSPICommandPending;
	BOOL	m_bTraceMissingFields;
	BOOL	m_bTraceReadFromEEProm;
	BOOL	m_bTraceWriteToEEProm;
	BOOL	m_bTraceBF533DebugText;
	BOOL	m_bTraceGetDIPState;
	BOOL	m_bTraceGetAlarmState;
	BOOL	m_bTraceRequestSingleFrame;
	BOOL	m_bTraceSetNR;
	BOOL	m_bTraceGetNR;

};

#endif // !defined(AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)

typedef struct _DATA_PACKET_HELPER
{
	DWORD			dwDummy;			// Die ersten Bytes der SPORT-Übertragung enthalten oft Bitfehler, daher die Dummyvariable
	DWORD			dwStartMarker;		// Magic Marker1 (Startmarkierung des Headers)
	DWORD			dwCheckSum;			// Prüfsumme über die Bilddaten.
	DWORD			dwProzessTime;		// Gibt die Ausführungszeit des BF533 in MikroSeconds an
	BOOL			bValid;				// TRUE: Bilddaten können abgeholt werden
	WORD			wSize;				// Größe der Struktur ('sizeof(DATA_PACKET)')
	DataType		eType;				// Gibt den Typ des Bildes an (Jpeg, Mpeg4, YUV422,...)	
	DataSubType		eSubType;			// Beschreibt den Bildtyp näher (I-Frame, P-Frame,....)
	WORD			wSource;			// Kameranummer
	WORD			wSizeH;				// Breite des Bildes
	WORD			wSizeV;				// Höhe des Bildes
	WORD			wBytesPerPixel;		// Bytes pro Pixel: (z.B. 2Bytes bei YUV422)
	WORD			wField;				// 0->Even Field 1->Odd Field
	BOOL			bVidPresent;		// TRUE->Video detektiert/FALSE->Video nicht detektiert.
	DWORD			dwFieldID;			// FieldCounter
	DWORD			dwProzessID;		// ProzessID für den Server
	DWORD			dwStreamID;			// Streamnummer für BF533 Video requests 
	DWORD			dwImageDataLen;		// Länge der eigentlichen Bilddaten
}DATA_PACKET_HELPER;
