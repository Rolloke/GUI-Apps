// IPCOutputDVClient.h: interface for the CIPCOutputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCOUTPUTDVCLIENT_H__6B5BBEE0_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_IPCOUTPUTDVCLIENT_H__6B5BBEE0_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServer;

class CIPCOutputDVClient : public CIPCOutputClient  
{
public:
	CIPCOutputDVClient(LPCTSTR shmName, CServer* pServer);
	virtual ~CIPCOutputDVClient();

	// attributes
public:
	int		GetNrOfActiveCameras();
	int		GetNrOfCameras();
	CSecID  GetCamIDFromNr(WORD wNr);

	// overrides
public:
	virtual void OnConfirmConnection();
	// disconnection
	virtual void OnRequestDisconnect();

	// update camera and relay
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[]);
	virtual void OnAddRecord(const CIPCOutputRecord & rec);
	virtual void OnUpdateRecord(const CIPCOutputRecord & rec);
	virtual void OnDeleteRecord(const CIPCOutputRecord & rec);

	// images
	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict, DWORD dwUserID, CSecID idArchive);
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData, 
								 CSecID idArchive);

	virtual void OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual	void OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	virtual	void OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID);
	virtual void OnRequestSelectCamera(CSecID camID);
	virtual void OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);
	// data member
private:
	CServer* m_pServer;
};

#endif // !defined(AFX_IPCOUTPUTDVCLIENT_H__6B5BBEE0_9757_11D3_A870_004005A19028__INCLUDED_)
