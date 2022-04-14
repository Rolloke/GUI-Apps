/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: OutputTester.h $
// ARCHIVE:		$Archive: /Project/Tools/UnitTest/OutputTester.h $
// CHECKIN:		$Date: 23.04.97 18:09 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 14.04.97 15:48 $
// BY AUTHOR:	$Author: Hajo $
// $Nokeywords:$

/* GlobalReplace: bCanSWDecompress --> bCanBWDecompress */
/* GlobalReplace: DisplayMpeg --> MpegDecoding */
/* GlobalReplace: OnConfirmStartMpeg --> OnConfirmStartMpegEncoding */
#ifndef __OUTPUTTESTER_H_
#define __OUTPUTTESTER_H_

//#include "IOGroup.h"
#include "TickCount.h"
#include "CipcOutput.h"

class CTestTool;

class COutputTester : public CIPCOutput//, public CIOGroup
{
public:
	COutputTester(CTestTool* m_pCTestTool, const CString& pSMName);
	~COutputTester();
	virtual void SetShutdownInProgress();

	SecOutputType	TypeName2TypeNr(const char *p);
	// p:		Pointer auf ein ein string mit "camera", 
	// "relay" oder "audio"
	// RETURN:	Typenummer

	const char *TypeNr2TypeName(int iTypeNr);	// 	Gegenteil von TypeName2TypeNr
	// cipc related
	virtual BOOL OnTimeoutWrite(DWORD dwCmd);
	virtual BOOL OnTimeoutCmdReceive();
	virtual void OnWaitFailed(DWORD dwCmd);
	//
	inline BOOL IsSWCodec() const;
	int SWCodecUpScale(int iNormalFPS, Resolution res) const;
	int SWCodecDownScale(int iScaledFPS, Resolution res) const;
	//
	static int m_iMicoFPS;
	static int m_iCocoFPS;
	static int m_iSWCodecVirtualFPS; // 'pesudo-frames' see below and SWCodecUpScale/SWCodecDownScale
	// special for SWCodec, how many pseudo frames for each resolution
	static int m_iSWCodecCountLow;
	static int m_iSWCodecCountMedium;
	static int m_iSWCodecCountHigh;
protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual	void OnRequestDisconnect();

	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmSetUp(WORD wGroupID);
	virtual void OnConfirmReset(WORD wGroupID);
	virtual void OnConfirmSetRelay(CSecID relayID, BOOL bClosed);
	virtual void OnConfirmCurrentState(WORD wGroupID, DWORD stateMask);
	virtual void OnConfirmHardware(WORD wGroupID, int errorCode,
						BOOL bCanSWCompress,
						BOOL bCanBWDecompress,
						BOOL bCanColorCompress,
						BOOL bCanColorDecompress,
						BOOL bCanOverlay
				);	// errorCode 0==no error, else some error code
	// picture functions
	// ConfirmXXX
	virtual void OnIndicateLocalVideoData(CSecID camID, 
									WORD wXSize, WORD wYSize,
									const CIPCPictureRecord &pict);
	// JPEG related
	virtual void OnConfirmJpegEncoding(
									const CIPCPictureRecord &pict,
									DWORD dwUserData
									);
	virtual void OnConfirmJpegDecoding(WORD wGroupID,
									WORD wXSize, WORD wYSize, 
									DWORD dwUserData,
									const CIPCPictureRecord &pict
									);
	// MPEG related:
	virtual void OnConfirmStartMpegEncoding(CSecID camID);
	virtual void OnIndicateMpegData(const CIPCPictureRecord &pict, DWORD dwJobData);
	virtual void OnConfirmStopMpegEncoding(CSecID camID);
	virtual void OnConfirmMpegDecoding(WORD wGroupID, DWORD dwUserData);	//
	virtual void OnConfirmSetOutputWindow (WORD wGroupID, int iPictureResult);
	//
	virtual void OnConfirmGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);

	//
	virtual void OnConfirmDumpOutputWindow(
				WORD wGroupID,
				WORD wUserID,
				int iFormat,
				const CIPCExtraMemory &bitmapData
				);
	virtual void OnConfirmSetDisplayWindow(WORD wGroupID);
	//
	virtual void OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend, 
				   DWORD dwType,DWORD dwUserID);
private:
	CCriticalSection m_CS;
//	CProcessScheduler *m_pProcessScheduler;
	CompressionType m_compressionType;	// NOT YET more than one
	int				m_iFramesPerSecond;
	BOOL			m_bIsSWCodec;
	int				m_iHardware;
	DWORD			m_dwVersion;
	WORD			m_wGroupID;
	DWORD			m_dwState;
	// new ones for the coconut
	CString			m_shmName;
	// 	COMPRESSION_13,	256KBit/s	12fps
	// 	COMPRESSION_18	128KBit/s	12fps
	Compression		m_Compression;	// enum
	Resolution		m_Resolution;	// enum
	WORD			m_wNumPictures;
	DWORD			m_dwBitrateScale;
	DWORD			m_dwUserData;
	int				m_iLoop;
	CTickCounter	m_TickCounter[10]; // der schnelle hack
	double          m_dRes_time;
	double          m_dRes_len;

public:	// lazy public members, set in OnConfirmHardware
	BOOL m_bCanSWCompress, m_bCanBWDecompress;
	BOOL m_bCanColorCompress, m_bCanColorDecompress;
	BOOL m_bCanOverlay;
	inline	CompressionType GetCompressionType() const;
//	inline CProcessScheduler *GetProcessScheduler() const;
	inline int GetFramesPerSecond() const;
};
/*
inline CProcessScheduler *COutputTester::GetProcessScheduler() const
{
	return m_pProcessScheduler;
}
*/

inline	CompressionType COutputTester::GetCompressionType() const
{ 
	return m_compressionType; 
}
inline int COutputTester::GetFramesPerSecond() const
{
	return m_iFramesPerSecond;
}
inline BOOL COutputTester::IsSWCodec() const
{
	return m_bIsSWCodec;	// calculated in constructor
}

#endif

