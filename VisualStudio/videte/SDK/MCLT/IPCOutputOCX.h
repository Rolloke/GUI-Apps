
#pragma once
#include "cipcoutputclient.h"
#include "LivePicture.h"
#include "MCLTCtl.h"

class CLivePicture;

class CIPCOutputOCX :	public CIPCOutputClient
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
public:
	CIPCOutputOCX (CMCLTCtrl& MCLTCtrl, LPCTSTR shmName, WORD wCodePage);
	~CIPCOutputOCX(void);

	//////////////////////////////////////////////////////////////////////
	// overrides
	//////////////////////////////////////////////////////////////////////
protected:

	virtual void			OnConfirmConnection();
	virtual void			OnConfirmBitrate(DWORD dwBitrate);
	virtual void			OnAddRecord(const CIPCOutputRecord& record);
	virtual void			OnUpdateRecord(const CIPCOutputRecord & record);
	virtual void			OnConfirmInfoOutputs(WORD wGroupID, 
		int iNumGroups, 
		int numRecords, 
		const CIPCOutputRecord records[]);
	virtual void			OnConfirmSetRelay(CSecID relayID, 
		BOOL bClosed);

	virtual void			OnIndicateVideo(const CIPCPictureRecord& pict,
		DWORD dwMDX,
		DWORD dwMDY,
		DWORD dwUserData,
		CSecID idArchive);

	virtual void			OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
		DWORD dwUserID, 
		CSecID idArchive);

	//////////////////////////////////////////////////////////////////////
	// implementations
	//////////////////////////////////////////////////////////////////////
public:

	void					RequestLivePictures(CSecID sID, 
		CompressionType m_CompressionType);
	BOOL					IsLowBandwidth();

	//////////////////////////////////////////////////////////////////////
	// members
	//////////////////////////////////////////////////////////////////////
private:
	CMCLTCtrl*			m_pMCLTCtrl;	//pointer to the class which 
	//controls the live pictures
	int						m_iFrameRate;	//the framerate for showing 
	//live pictures
	DWORD					m_dwBitRate;	//the bitrate of the connection
	//to the server
};
//////////////////////////////////////////////////////////////////////