/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputServer.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcOutputServer.h $
// CHECKIN:		$Date: 9.06.06 15:18 $
// VERSION:		$Revision: 74 $
// LAST CHANGE:	$Modtime: 9.06.06 14:58 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __OUTPGRP_H_
#define __OUTPGRP_H_

#include "IOGroup.h"
#include "Output.h"
#include "CipcOutput.h"

class CProcessScheduler;
class CProcessSchedulerVideoBase;
class CQuad;

class CIPCOutputServer : public CIPCOutput, public CIOGroup
{
	// construction / destruction
public:
	CIPCOutputServer(LPCTSTR pName, int iNr, LPCTSTR pSMName);
	virtual ~CIPCOutputServer();

	// attributes
public:
	BOOL	IsMiCo() const;
	BOOL	IsJaCob() const;
	BOOL	IsSaVic() const;
	BOOL	IsTasha() const;
	BOOL	IsQ() const;
	BOOL	IsIP() const;

	BOOL	IsMDActive(CSecID id);
	BOOL	HasCameras() const;
	CString GetAppname() const;

	inline	CompressionType				GetCompressionType() const;
			CProcessScheduler*			GetProcessScheduler() const;
			CProcessSchedulerVideoBase*	GetProcessSchedulerVideo() const;
	inline  int							GetFramesPerSecond() const;
	inline  int							GetNrOfActiveMDDetectors() const;
	inline  int							GetNrOfActiveCameras() const;


	SecOutputType	TypeName2TypeNr(LPCTSTR p);
	// p:		Pointer auf ein ein string mit "camera", "relay" ...
	// RETURN:	Typenummer

	LPCTSTR TypeNr2TypeName(int iTypeNr);	// 			Gegenteil fon TypeName2TypeNr
	// cipc related
	//

	// operations
public:
	//
			void Load(CWK_Profile& wkp, int iNr);
			void Save(CWK_Profile& wkp);
	virtual void SetShutdownInProgress();
			BOOL Start();
			void DoActivityVideoOut(CSecID id, int iPort);
			void SwitchVideoOut(CSecID id, int iPort);

	//
	static int m_iMicoFPS;
	static int m_iSaVicFPS;
	static int m_iCocoFPS;
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
	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
									   DWORD dwUserData,
									   CSecID idArchive);

	virtual void OnConfirmJpegDecoding(WORD wGroupID,
									WORD wXSize, WORD wYSize, 
									DWORD dwUserData,
									const CIPCPictureRecord &pict
									);
	// H.263 related:
	virtual void OnConfirmStartH263Encoding(CSecID camID);
	virtual void OnIndicateH263Data(const CIPCPictureRecord &pict, 
									DWORD dwJobData,
									CSecID idArchive);
	virtual void OnConfirmStopH263Encoding(CSecID camID);
	virtual void OnConfirmH263Decoding(WORD wGroupID, DWORD dwUserData);	//
	virtual void OnConfirmSetOutputWindow (WORD wGroupID, int iPictureResult);
	
	// MPEG4
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData,
								 CSecID idArchive);


	virtual void OnConfirmGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);
	virtual void OnConfirmSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);

	//
	virtual	void OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	virtual	void OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID);
	//
	virtual void OnConfirmDumpOutputWindow(
				WORD wGroupID,
				WORD wUserID,
				int iFormat,
				const CIPCExtraMemory &bitmapData
				);
	virtual void OnConfirmSetDisplayWindow(WORD wGroupID);
	//
	virtual void OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend, DWORD dwType,DWORD dwUserID);
	//
	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);
	virtual void OnRequestSetValue(CSecID id,
		const CString &sKey,
		const CString &sValue,
		DWORD dwServerData);
private:
	CCriticalSection m_CS;
	CProcessSchedulerVideoBase* m_pProcessScheduler;
	CompressionType m_compressionType;	// NOT YET more than one
	int	m_iFramesPerSecond;
	int m_iNrOfMDDetectors;
	
	BOOL  m_bIsMiCo;
	BOOL  m_bIsJaCob;
	BOOL  m_bIsSaVic;
	BOOL  m_bIsTasha;
	BOOL  m_bIsQ;
	BOOL  m_bIsIP;
	BOOL  m_bIsDummy;
	BOOL  m_bHasCameras;
	DWORD m_dwBoardNr;
	CString m_sAppname;
	CQuad* m_pQuad;

public:	// lazy public members, set in OnConfirmHardware
	BOOL m_bCanSWCompress; 
	BOOL m_bCanBWDecompress;
	BOOL m_bCanColorCompress;
	BOOL m_bCanColorDecompress;
	BOOL m_bCanOverlay;
};

inline	CompressionType CIPCOutputServer::GetCompressionType() const
{ 
	return m_compressionType; 
}
inline int CIPCOutputServer::GetFramesPerSecond() const
{
	return m_iFramesPerSecond;
}
inline int CIPCOutputServer::GetNrOfActiveMDDetectors() const
{
	return m_iNrOfMDDetectors;
}
inline int CIPCOutputServer::GetNrOfActiveCameras() const
{
	 int iNrOfCameras=0;
	 for (int i=0;i<GetSize();i++)
	 {
		 if (GetOutputAt(i)->GetOutputType()==OUTPUT_CAMERA)
		 {
			 iNrOfCameras++;
		 }
	 }
	 return iNrOfCameras;
}


#endif

