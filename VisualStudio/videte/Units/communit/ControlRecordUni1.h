#pragma once
#include "controlrecord.h"


class CControlRecordUni1 : public CControlRecord
{
enum enumSTATUS
{
	READY,
	WAITING_FOR_ACK
};

public:
	CControlRecordUni1(int iCom, BOOL bTraceHex, CameraControlType camType);
	virtual ~CControlRecordUni1();

	// overrides
public:
	virtual BOOL	Create();
	virtual BOOL	Add(CSecID secID, DWORD camID);
	virtual void	OnNewCommand(CCameraCommandRecord* pCCommandR);
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual void	OnTimeOutTransparent();

// implementation
protected:
	void	TryToSendCommand(BOOL bForceRepeat);

private:
	void	CheckData();
	void	CurrentCommandFinished(BOOL bForceNextOne);
	void				CreateNewCmdPelcoD(BYTE bCamID, CCameraCommandRecord* pCCommandR);
	void				CreateNewCmdPelcoP(BYTE bCamID, CCameraCommandRecord* pCCommandR);
	void				CreateNewCmdBBVRS422(BYTE bCamID, CCameraCommandRecord* pCCommandR);
	void				CreateNewCmdSensormatic(int nCam, CCameraCommandRecord* pCCommandR);
	CameraControlCmd	CreateNewCmdCbcNaf27(int nCam, CCameraCommandRecord* pCCommandR);
	CameraControlCmd	CreateNewCmdVideotec(WORD wCamID, CCameraCommandRecord* pCCommandR);
	CameraControlCmd	CreateNewCmdBewator(int nCam, CCameraCommandRecord* pCCommandR);
	CameraControlCmd	CreateNewCmdJvcTKC(int nCam, CCameraCommandRecord* pCCommandR);
	CameraControlCmd	CreateNewCmdMeridian(int nCam, CCameraCommandRecord* pCCommandR);
	
// data
private:
	enumSTATUS			m_eStatus;
	BOOL				m_bRepeat;
	BYTE				m_bySourceID;
	BYTE				m_bySendErrorCounter;
	CDWordArray			m_Flags;
	int					m_nReceiveCount;
	CameraControlCmd	m_cccLast;
};
