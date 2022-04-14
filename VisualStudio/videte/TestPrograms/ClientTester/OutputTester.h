/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: OutputTester.h $
// ARCHIVE:		$Archive: /Project/Tools/ClientTester/OutputTester.h $
// CHECKIN:		$Date: 12.06.97 19:22 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 12.06.97 19:14 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$

#ifndef __OUTPUTTESTER_H_
#define __OUTPUTTESTER_H_

#include "TickCount.h"
#include "CipcOutput.h"
#include "CipcOutputClient.h"

class CTestTool;

class COutputTester : public CIPCOutputClient 
{
public:
	COutputTester(CTestTool* m_pCTestTool, 
		const CString& pSMName,
		BOOL bAsMaster );
	~COutputTester();
	//
	static int m_iMicoFPS;
	static int m_iCocoFPS;
	static int m_iSWCodecVirtualFPS; 
	static int m_iSWCodecCountLow;
	static int m_iSWCodecCountMedium;
	static int m_iSWCodecCountHigh;

protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual	void OnRequestDisconnect();
	virtual void OnConfirmInfoOutputs(
		WORD wGroupID, 
		int iNumGroups, 
		int numRecords, 
		const CIPCOutputRecord records[]);
	virtual void OnAddRecord(const CIPCOutputRecord &pRec){}
	virtual void OnUpdateRecord(const CIPCOutputRecord &pRec){}
	virtual void OnDeleteRecord(const CIPCOutputRecord &pRec){}
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

private:
	CCriticalSection m_CS;
	CompressionType m_compressionType;	// NOT YET more than one
	int				m_iFramesPerSecond;
	BOOL			m_bIsSWCodec;
	int				m_iHardware;
	DWORD			m_dwVersion;
	WORD			m_wGroupID;
	DWORD			m_dwState;
	CString			m_shmName;
	// 	COMPRESSION_13,	256KBit/s	12fps
	// 	COMPRESSION_18	128KBit/s	12fps
	Compression		m_Compression;	// enum
	Resolution		m_Resolution;	// enum
	WORD			m_wNumPictures;
	DWORD			m_dwBitrateScale;
	DWORD			m_dwUserData;
	int				m_iLoop;
	CTickCounter	m_TickCounter[10]; 
	double          m_dRes_time;
	double          m_dRes_len;
	BOOL			m_bIsServer;
	BOOL			m_bInfo;

	BOOL m_bDoStatLogs;

public:	// lazy public members, set in OnConfirmHardware
	BOOL m_bCanSWCompress, m_bCanBWDecompress;
	BOOL m_bCanColorCompress, m_bCanColorDecompress;
	BOOL m_bCanOverlay;
};

#endif

