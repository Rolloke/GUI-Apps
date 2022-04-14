// CIPCOutputVision.h: interface for the CIPCOutputVision class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOUTPUTVISION_H
#define _CIPCOUTPUTVISION_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CServer;

#include "PictureLengthStatistic.h"

class CIPCOutputVision : public CIPCOutputClient  
{
	// construction / destruction
public:
	CIPCOutputVision(LPCTSTR shmName, CServer* pServer, CSecID id);
	virtual ~CIPCOutputVision();

	// attributes
public:
	inline CServer* GetServer() const;
	inline DWORD	GetBitrate() const;
		   int		GetMaxMicoFPS();
		   BOOL		IsRelayPTZ(CSecID id);
		   BOOL		IsPTZCamera(CSecID camID);
			BOOL		IsDirectShow(CSecID camID);
		   CSecID   GetCamID(WORD wCamSubID);
//ML 24.9.99 Begin Insertation{
		   CSecID   GetCamIDFromNr(WORD wCamNr);
		   CSecID   GetRelayIDFromNr(WORD wRelayNr);
		   WORD		GetNrFromCamID(CSecID camID);
		   WORD		GetNrFromRelayID(CSecID relayID);

		   virtual void OnConfirmSetRelay(CSecID relayID, BOOL bClosed);

//ML 24.9.99 End Insertation}

	// operations
public:
		   void	DeactivateCamera(CSecID id,BOOL bForceCoCoStop=FALSE);
		   void	RequestCSB(CSecID id);
		   void	RequestCameraControl(CameraControlCmd cmd, BOOL bClose);
		   BOOL ReferenceComplete();
		   void StopJpegEncoding();
		   void StopH263Encoding();
		   void RestartJpegEncoding();
	
		   void RequestPictures(CSecID id);
		   void UnrequestPictures(CSecID id,BOOL bForceCoCoStop=FALSE);

	// Implementation
public:
	virtual BOOL	Run(DWORD dwTimeOut);	// default is GetTimeoutCmdReceive()

	// Overridables
protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual void OnConfirmBitrate(DWORD dwBitrate);
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[]);
	virtual void OnAddRecord(const CIPCOutputRecord & pRec);
	virtual void OnUpdateRecord(const CIPCOutputRecord & pRec);
	virtual void OnDeleteRecord(const CIPCOutputRecord & pRec);
	virtual void OnRequestDisconnect();
	virtual void OnRequestSelectCamera(CSecID camID);
	virtual void OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);

	// picture data confirmations
	virtual void OnConfirmJpegEncoding(	const CIPCPictureRecord &pict,DWORD dwUserID);
	virtual void OnConfirmEncodedVideo(const CIPCPictureRecord &pict, DWORD dwUserData);
	virtual void OnIndicateH263Data(const CIPCPictureRecord &pict, DWORD dwJobData);

	// delay sync confirmation
	virtual void OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend, 
							   DWORD dwType,DWORD dwUserID);
	// value confirms
	virtual void OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);
	virtual void OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);

	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage);
	// MD Mask code
	virtual	void OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	virtual	void OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID);
	//
	// Implementation
protected:
	void SearchRelayPTZ();
	Compression GetCocoCompression(DWORD dwBitrate, int iQuality);
	Compression GetMicoCompression(int iQuality, Resolution res);
	Compression GetMicoCompression2(DWORD dwSize,Resolution res);

	// data member
protected:
	CServer*	m_pServer;
	CSecID		m_ID;
	BOOL		m_bGotInfo;
	DWORD		m_dwTime;
	DWORD		m_dwBitrate;

	CSecID		m_RelayPTZ[16];
	CPictureLengthStatistic* m_pPictureLengthStatistic;
	CSecID		m_idActiveCam;
};
/////////////////////////////////////////////////////////////////////
inline CServer* CIPCOutputVision::GetServer() const
{
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////
inline DWORD CIPCOutputVision::GetBitrate() const
{
	return m_dwBitrate;
}

#endif // !defined(AFX_CIPCOUTPUTVISION_H__BC7F5CC7_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_)
