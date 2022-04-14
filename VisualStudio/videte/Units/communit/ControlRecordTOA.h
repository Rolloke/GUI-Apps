// CameraControlRecordTOA.h: interface for the CControlRecordTOA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLRECORDTOA_H__F2E8615E_EE7C_4A72_B88C_0DE4CE3B18D5__INCLUDED_)
#define AFX_CONTROLRECORDTOA_H__F2E8615E_EE7C_4A72_B88C_0DE4CE3B18D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlRecord.h"

class CControlRecordTOA : public CControlRecord  
{
enum enumSTATUS
{
	READY,
	WAITING_FOR_ACK
};

public:
	CControlRecordTOA(int iCom, BOOL bTraceHex);
	virtual ~CControlRecordTOA();

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
	BOOL	CheckForValidTOAIDs(DWORD dwCamID);
	BOOL	GetTOAIDs(CSecID idCamSec, BYTE& byHeaderOnes, BYTE& byHundredsTens, int &iCam);
	void	FillMenuCmd(BYTE byHeaderOnes, BYTE byHundredsTens, DWORD dwValue);
	void	FillCameraCmd(BYTE byHeaderOnes, BYTE byHundredsTens, CameraControlCmd byCmd, DWORD dwValue, int iCam);
	CCommandRecord*		FillHeader(BYTE byHeaderOnes, BYTE byHundredsTens);
	void	FillTelemetryCmd(CCommandRecord* pNewCommand, CameraControlCmd cmd, DWORD dwValue);
	void	FillPresetCmd(CCommandRecord* pNewCommand, CameraControlCmd cmd, DWORD dwValue, int iCam);
	void	FillParity(CCommandRecord* pCommand);
	void	CheckData();
	BOOL	CheckLengthAndParity();
	void	DataCorrupt(int iIndex);
	void	CurrentCommandFinished(BOOL bForceNextOne);
	void	OnACK();
	void	OnNAK();
	BYTE	GetMenuCommand(DWORD dwValue, BOOL bRepeat);
#ifdef _DEBUG
	void	DecodeData();
#endif
	
// data
private:
	enumSTATUS	m_eStatus;
	BYTE		m_bySendErrorCounter;
	CDWordArray			m_Flags;
#ifdef _DEBUG
	BOOL		m_bTelemetrySet;
#endif
};

#endif // !defined(AFX_CONTROLRECORDTOA_H__F2E8615E_EE7C_4A72_B88C_0DE4CE3B18D5__INCLUDED_)
