// RecordLandauDtp.h: interface for the CRecordLandauDtp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDLANDAUDTP_H__4C4ED9F5_B77F_11D3_BAA9_00400531137E__INCLUDED_)
#define AFX_RECORDLANDAUDTP_H__4C4ED9F5_B77F_11D3_BAA9_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDIControlRecord.h"

class CRecordLandauDtp : public CSDIControlRecord  
{
protected:
// Status der gerade bearbeiteten Daten
enum eStatus
{
	STATUS_STX,
	STATUS_INTERFACE,
	STATUS_DATE,
	STATUS_TIME,
	STATUS_TAN,
	STATUS_WSID,
	STATUS_BANKCODE,
	STATUS_ACCOUNT,
	STATUS_VALUE,
	STATUS_CAMERA,
	STATUS_CRC,
	STATUS_OK,
	STATUS_FAILED,
};

public:
	CRecordLandauDtp(SDIControlType type,
								   int iCom,
								   BOOL bTraceAscii,
								   BOOL bTraceHex,
								   BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordLandauDtp();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	InitializeResponses();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();

// access

// implementation
protected:
	void	OnStatusSTX(BYTE byte);
	void	OnStatusInterface();
	void	OnStatusDate();
	void	OnStatusTime();
	void	OnStatusTAN();
	void	OnStatusWSID();
	void	OnStatusBankCode();
	void	OnStatusAccount();
	void	OnStatusValue();
	void	OnStatusCamera();
	void	OnStatusCRC(BYTE byte);

	void	TransmitDataToSecurity();
	void	ClearAllData();
	BOOL	IsInterfaceOK();
	BOOL	IsCameraOK();
	BOOL	IsCameraExplicit();

private:
	void	IndicateAlarm(WORD wAlarm);
	void	UpdateAlarm(WORD wAlarm);
	void	UpdateInterfaceAlarms();
	void	RefreshDataForSend(BOOL bUpdateOnly);
	void	SetStandardDataMembers();

// data
protected:
	eStatus		m_eStatus;
	int			m_iInterfaceActual;	// 0 - 3, -1 invalid
	int			m_iInterfaceLast;	// remember last interface no. for update
	int			m_iCamera;			// 0 update, 1 portrait, 2 money, -1 invalid
	BOOL		m_bUpdateOnly;
	CIPCFields	m_DataFieldArrayForInterface[5];	// access via m_iInterfaceActual(0-3)
	CIPCFields	m_DataFieldArrayToSend;				// this will be send
	BOOL		m_bPreviousCameraAlarmUpdate;
};
#endif // !defined(AFX_RECORDLANDAUDTP_H__4C4ED9F5_B77F_11D3_BAA9_00400531137E__INCLUDED_)
