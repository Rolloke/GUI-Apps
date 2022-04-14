// ControlRecordFastrax.h: interface for the CControlRecordFastrax class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLRECORDFASTRAX_H__8C9B469B_7092_4911_9E2F_98C647F2F017__INCLUDED_)
#define AFX_CONTROLRECORDFASTRAX_H__8C9B469B_7092_4911_9E2F_98C647F2F017__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlRecord.h"

class CControlRecordFastrax : public CControlRecord  
{
enum enumStatusSend
{
	READY,
	WAITING_FOR_ANSWER_STATUS,
	WAITING_FOR_ANSWER_POSITION
};
enum enumStatusReceive
{
	ANSWER_NONE,
	ANSWER_STATUS,
	ANSWER_POSITION
};

public:
	CControlRecordFastrax(int iCom, BOOL bTraceHex);
	virtual ~CControlRecordFastrax();

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
	BOOL	CurrentCommandFinished(BOOL bAcknowledged);
private:
	BOOL	OpenCom();
	CCommandRecord*	CreateCommandRecord(BYTE byID);
	CameraControlCmd	FillCameraCmd(CCommandRecord* pCommand, CameraControlCmd cmd, DWORD dwValue);
	void	FillSecondPresetCmd(CCommandRecord* pCommand, CameraControlCmd cmd);
	void	FillCRC(CCommandRecord* pCommand);
	void	CheckData();
	BOOL	CheckCRC();

// data
private:
	BOOL				m_bBidirectional;
	DWORD				m_dwRepeatTime;
	BOOL				m_bOpenCloseRS232;

	enumStatusSend		m_eStatusSend;
	enumStatusReceive	m_eStatusReceive;
	int					m_iBytesToReceive;
	int					m_iBytesReceived;
};

#endif // !defined(AFX_CONTROLRECORDFASTRAX_H__8C9B469B_7092_4911_9E2F_98C647F2F017__INCLUDED_)
