// CameraControlRecordBaxall.h: interface for the CControlRecordBaxall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLRECORDBAXALL_H__F2E8615E_EE7C_4A72_B88C_0DE4CE3B18D5__INCLUDED_)
#define AFX_CONTROLRECORDBAXALL_H__F2E8615E_EE7C_4A72_B88C_0DE4CE3B18D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlRecord.h"

class CControlRecordBaxall : public CControlRecord  
{
enum enumSTATUS
{
	READY,
	WAITING_FOR_ACK
};

public:
	CControlRecordBaxall(int iCom, BOOL bTraceHex);
	virtual ~CControlRecordBaxall();

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
	BOOL	CheckForValidBaxallIDs(DWORD dwCamID);
	BOOL	GetBaxallIDs(CSecID idCamSec, BYTE& byDestID, BYTE& byCamID);
	CCommandRecord*	FillHeader(BYTE byDestID);
	void	FillCameraCmd(BYTE byDestID, BYTE byCam);
	void	FillPresetCmd(BYTE byDestID, CameraControlCmd cmd);
	void	FillTelemetrySetCmd(BYTE byDestID);
	void	FillTelemetryCmd(BYTE byDestID, CameraControlCmd byCmd, BYTE bySpeedX, BYTE bySpeedY);
	void	FillStatusResponse(BYTE byDestID);
	void	FillCRCandFooterPAD(CCommandRecord* pCommand);
	void	CheckData();
	BOOL	CheckLengthAndCRC();
	void	DataCorrupt(int iIndex);
	void	CurrentCommandFinished(BOOL bForceNextOne);
	void	OnACK();
	void	OnNAK();
	void	OnStatusRequest(BYTE byDestID);
#ifdef _DEBUG
	void	DecodeData();
	void	SendACK(BYTE byDestID, BYTE bySource);
#endif
	
// data
private:
	enumSTATUS			m_eStatus;
	BOOL				m_bDecreaseBaxallAddCode;
	BYTE				m_bySourceID;
	BYTE				m_bySendErrorCounter;
#ifdef _DEBUG
	BOOL				m_bTelemetrySet;
#endif
};

#endif // !defined(AFX_CONTROLRECORDBAXALL_H__F2E8615E_EE7C_4A72_B88C_0DE4CE3B18D5__INCLUDED_)
