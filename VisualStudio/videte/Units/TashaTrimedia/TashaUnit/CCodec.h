// CCodec.h: interface for the CCodec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)
#define AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TASHA_FIRMWARE "..\\TashaFirmware\\TashaFirmware.out"

class CTashaUnitDlg;
class CCodec  
{
public:
	CCodec(CTashaUnitDlg* pWnd, int DSPNumber = 0);
	virtual ~CCodec();

	BOOL Open(const CString& sImagePath);
	void Close();

	BOOL DoRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestGetBrightness(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);

	BOOL DoRequestGetContrast(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	
	BOOL DoRequestGetSaturation(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	
	BOOL DoRequestStartCapture(); 
	BOOL OnConfirmStartCapture(); 

	BOOL DoRequestStopCapture(); 
	BOOL OnConfirmStopCapture(); 

	BOOL DoRequestPauseCapture(); 
	BOOL OnConfirmPauseCapture(); 

	BOOL DoRequestResumeCapture(); 
	BOOL OnConfirmResumeCapture(); 

	BOOL DoRequestSetInputSource(SOURCE_SELECTION &SrcSel1, SOURCE_SELECTION &SrcSel2);
	BOOL OnConfirmSetInputSource(WORD wSource, DWORD dwProcessID);

	BOOL DoRequestGetInputSource();
	BOOL OnConfirmGetInputSource(WORD wSource, DWORD dwProcessID);

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

	BOOL OnConfirmInitComplete();

	EncoderState  GetEncoderState(){return m_eEncoderState;}

protected:
	BOOL CloseDataChannel();
	BOOL OpenDataChannel();

	BOOL OnReceiveNewCodecData();
	BOOL DoConfirmReceiveNewCodecData();

	BOOL SendMessage(int nMessageID, DWORD dwParam1=0, DWORD dwParam2=0, DWORD dwParam3=0,DWORD dwParam4=0);
	BOOL ReceiveMessage(int& nMessageID, DWORD& dwParam1, DWORD& dwParam2, DWORD& dwParam3, DWORD& dwParam4);
	BOOL CloseMessageChannel();
	BOOL OpenMessageChannel();
	static UINT MessageLoop(LPVOID pData);

	BOOL CanStart(){return ((m_eEncoderState == eEncoderStateOff) || (m_eEncoderState == eEncoderStateUnvalid));}
	BOOL CanStop(){return ((m_eEncoderState == eEncoderStateOn) || (m_eEncoderState == eEncoderStatePause) || (m_eEncoderState == eEncoderStateUnvalid));}
	BOOL CanPause(){return (m_eEncoderState == eEncoderStateOn);}
	BOOL CanResume(){return (m_eEncoderState == eEncoderStatePause);}

private:
	int					m_nDSPNumber;
	UInt32				m_hDSP;
	UInt32				m_nCRTHandle;
	HANDLE				m_hExitEvent;
	CTashaUnitDlg*		m_pWnd;
	DWORD				m_dwEncodedFrames;
	CString				m_sJpegPath;

	EncoderState		m_eEncoderState;
	ImageRes			m_eImageRes;
	
		// Messagechanel
	HANDLE				m_hMessageSynch;
	BOOL				m_bRun;
	UInt32				m_hMessage;	
	CWinThread*			m_pMessageLoopThread;

	// Shared Memory Data packet
	UInt32				m_dwSMBufferSize;
	UInt32				m_hSMBufferHandle;
	DataPacket*			m_pDataPacket;

	// SourceSelection
	CEvent				m_evSrcSelection;
	CEvent				m_evTmmInitComplete;
};

#endif // !defined(AFX_CCODEC_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)
