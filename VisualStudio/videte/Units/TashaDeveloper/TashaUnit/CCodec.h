// CCodec.h: interface for the CCodec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)
#define AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TASHA_FIRMWARE "TashaFirmware535.ldr"

class CCircularBuffer;
class CTashaUnitDlg;
class CWK_PerfMon;
class CCodec  
{
public:
	CCodec(CTashaUnitDlg* pWnd, int DSPNumber, const CString &sAppIniName);
	virtual ~CCodec();

	BOOL Open(const CString& sImagePath);
	void Close();

	BOOL DoRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue);
	BOOL OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue);

	BOOL DoRequestGetBrightness(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestGetContrast(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	
	BOOL DoRequestGetSaturation(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestGetMDTreshold(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestGetMaskTreshold(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestGetMaskIncrement(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestGetMaskDelay(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetPermanentMask(DWORD dwUserData, WORD wSource, const BYTE* pData, DWORD dwLen, BOOL bWait);
	BOOL OnConfirmSetPermanentMask(DWORD dwUserData, WORD wSource, DWORD dwAddr, DWORD dwLen);

	BOOL DoRequestClearPermanentMask(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource);

	BOOL DoRequestInvertPermanentMask(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource);
	
	BOOL DoRequestStartCapture(); 
	BOOL OnConfirmStartCapture(); 

	BOOL DoRequestStopCapture(); 
	BOOL OnConfirmStopCapture(); 

	BOOL DoRequestPauseCapture(); 
	BOOL OnConfirmPauseCapture(); 

	BOOL DoRequestResumeCapture(); 
	BOOL OnConfirmResumeCapture(); 

	BOOL DoRequestScanForCameras(DWORD dwUserData);
	BOOL OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);

	BOOL DoRequestSetRelayState(WORD wRelayID, BOOL bOpenClose);
	BOOL OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);

	BOOL DoRequestGetRelayState();
	BOOL OnConfirmGetRelayState(DWORD dwRelayStateMask);

	BOOL DoRequestGetAlarmState();
	BOOL OnConfirmGetAlarmState(DWORD dwAlarmStateMask);
	
	BOOL DoRequestSetAlarmEdge(DWORD dwAlarmEdge);
	BOOL OnConfirmSetAlarmEdge(DWORD dwAlarmEdge);

	BOOL DoRequestGetAlarmEdge();
	BOOL OnConfirmGetAlarmEdge(DWORD dwAlarmEdge);

	BOOL DoRequestSetChannel(WORD wSource, WORD wNewChannel);
	BOOL OnConfirmSetChannel(WORD wSource, WORD wNewChannel);

	BOOL OnConfirmInitComplete();
	BOOL OnNotifySlaveInitReady(WORD wSource);

	BOOL DoRequestTermination();
	BOOL OnConfirmTermination();

	BOOL DoRequestSetPCIFrameBufferAddress(DWORD dwPCIAddr);
	BOOL OnConfirmSetPCIFrameBufferAddress(DWORD dwPCIAddr);

	BOOL TransferEPLDdata(const CString& sFileName);
	BOOL DoRequestEPLDdataTransfer(const BYTE* pData, DWORD dwLen);
	BOOL OnConfirmEPLDdataTransfer(DWORD dwAddr, DWORD dwLen);

	BOOL TransferBF533BootData(int nBFNr, const CString &sFilename);
	BOOL DoRequestBoot533(int nBFNr, const BYTE* pData, DWORD dwLen);
	BOOL OnConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen);

	BOOL WriteEEPromData(const CString& sFileName);
	BOOL DoRequestWriteToEEProm(const BYTE* pData, DWORD dwLen, DWORD dwStartAddr);
	BOOL OnConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);

	BOOL ReadEEPromData(const CString& sFileName);
	BOOL DoRequestReadFromEEProm(BYTE* pData, DWORD dwLen, DWORD dwStartAddr);
	BOOL OnConfirmReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);

	BOOL OnIndicationAlarm(DWORD dwAlarmMask);
	BOOL OnIndicationInformation(int nMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);

	DWORD GetCheckSum(const DATA_PACKET* pPacket);

	EncoderState  GetEncoderState(){return m_eEncoderState;}

	void EnableFrameSync(BOOL bFrameSync);
	static UINT ReadDataThread(LPVOID pData);

	BOOL LoadIntelHexfile(const CString &sFilename, BYTE*& pData, DWORD& dwLen);
	BOOL LoadConfig(WORD wSource);
	BOOL SaveConfig(WORD wSource);
	BOOL ReadPermanentMaskFromRegistry(MASK_STRUCT* pPermanentMask, WORD wSource);
	BOOL SavePermanentMaskToRegistry(MASK_STRUCT* pPermanentMask, WORD wSource);


protected:
	BOOL OnReceiveNewCodecData(int nSport, DWORD dwBufferOffset, DWORD dwPacketSize);
	BOOL OnReceiveNewCodecDataEx(int nSport, DWORD dwBufferOffset, DWORD dwBufferLen);

	BOOL DoConfirmReceiveNewCodecData();

	BOOL SendMessage(int nMessageID, DWORD dwParam1=0, DWORD dwParam2=0, DWORD dwParam3=0,DWORD dwParam4=0);
	BOOL ReceiveMessage(int& nMessageID, DWORD& dwParam1, DWORD& dwParam2, DWORD& dwParam3, DWORD& dwParam4);
	BOOL CloseMessageChannel();
	BOOL OpenMessageChannel();
	static void __stdcall MessageReceiveCallback(LPVOID pData);

	BOOL CanStart(){return ((m_eEncoderState == eEncoderStateOff) || (m_eEncoderState == eEncoderStateUnvalid));}
	BOOL CanStop(){return ((m_eEncoderState == eEncoderStateOn) || (m_eEncoderState == eEncoderStatePause) || (m_eEncoderState == eEncoderStateUnvalid));}
	BOOL CanPause(){return (m_eEncoderState == eEncoderStateOn);}
	BOOL CanResume(){return (m_eEncoderState == eEncoderStatePause);}

	LPBITMAPINFO CreateDIB(const DATA_PACKET* pPacket);

	BYTE HexToInt(const CString &sS);
	void ReadDebugConfiguration();

public:
	BOOL				m_bRun;

private:
	CString				m_sAppIniName;
	int					m_nDSPNumber;
	HANDLE				m_hDSP;
	CTashaUnitDlg*		m_pWnd;
	DWORD				m_dwEncodedFrames;
	CString				m_sJpegPath;
	BOOL				m_bEstablishMessageChannel;

	EncoderState		m_eEncoderState;
	ImageRes			m_eImageRes;
	
	// SourceSelection
	CEvent				m_evSrcSelection;
	CEvent				m_evTmmInitComplete;
	CEvent				m_evTmmTermination;
	CEvent				m_evEPLDdataTransfer;
	CEvent				m_evBootBF533;
	CEvent				m_evSetPCIFrameBufferAddress;
	CEvent				m_evEEPromDataTransfer;
	CEvent				m_evSetPermanentMask;

	static CCriticalSection	m_csMessageHandling;
	CWK_PerfMon*		m_pPerf1;
	CWK_PerfMon*		m_pPerf2;
	CWK_PerfMon*		m_pPerf3;

	MEMSTRUCT			mem_TransferBuffer;
	MEMSTRUCT			mem_FrameBuffer;
	DATA_PACKET*		m_pDataPacket;

	BYTE*				m_pFrameBuffer;

	HANDLE				m_hFieldReadySemaphore;				// Field Encoded Semaphore
	CWinThread*			m_pReadDataThread;			
	CCircularBuffer*	m_pCirBuff[CHANNEL_COUNT];
	BOOL				m_bFrameSync;

	DWORD				m_dwCamMask;					// Gibt an welche Kamera Daten liefert
	int					m_nVidPres[CHANNEL_COUNT];		// Ein Wert von '0' bedeutet. Keine Daten auf diesem Kanal

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
};

#endif // !defined(AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)
