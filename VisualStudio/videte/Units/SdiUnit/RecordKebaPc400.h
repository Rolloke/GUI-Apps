// RecordKebaPc400.h: interface for the CRecordKebaPc400 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDKEBAPC400_H__F450D861_FC88_11D2_BA00_00400531137E__INCLUDED_)
#define AFX_RECORDKEBAPC400_H__F450D861_FC88_11D2_BA00_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIControlRecord.h"

class CRecordKebaPc400 : public CSDIControlRecord  
{
protected:
// Typ des Kommandos
enum eType
{
	TYPE_UNKNOWN,
	TYPE_CREDIT,
	TYPE_EC,
};

// Status der gerade bearbeiteten Daten
enum eStatus
{
	STATUS_CARD_READER,
	STATUS_WEEKDAY,
	STATUS_DATE,
	STATUS_TIME,
	STATUS_SEPERATOR,
	STATUS_BANKCODE,
	STATUS_SEPERATOR_ACCOUNT,
	STATUS_ACCOUNT,
	STATUS_CREDIT,
	STATUS_LF,
	STATUS_OK,
	STATUS_FAILED,
// only for pc2000
	STATUS_CONFIG_START,
	STATUS_CONFIG_END,
};

public:
	CRecordKebaPc400(SDIControlType type,
								 int iCom,
								 BOOL bTraceAscii,
								 BOOL bTraceHex,
								 BOOL bTraceEvents);

// Overrides
public:
	virtual	~CRecordKebaPc400();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	InitializeResponses();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();

// implementation
public:
protected:
	void	CheckAndStoreReadData(LPBYTE lpB, DWORD dwBufferLen);
	void	TransmitDataToSecurity();
	void	OnStatusKennung();
	void	OnStatusWochentag();
	void	OnStatusDatum();
	void	OnStatusZeit();
	void	OnStatusTrenner();
	void	OnStatusBLZ();
	void	OnStatusTrennerKonto();
	void	OnStatusKonto();
	void	OnStatusKredit();
	void	OnStatusLF(BYTE byte);
	void	IncreaseStatus();
	BOOL	IsKennung();
	BOOL	CheckForLF();
	BOOL	IncreaseFailureCounter();

// variables
public:
protected:
	eType		m_eTyp;
	eStatus		m_eStatus;
	int			m_iFailureCounter;
	WORD		m_wAlarm;
private:
};

#endif // !defined(AFX_RECORDKEBAPC400_H__F450D861_FC88_11D2_BA00_00400531137E__INCLUDED_)
