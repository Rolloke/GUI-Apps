// RecordSniDosCom.h: interface for the CRecordSniDosCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDSNIDOSCOM_H__BA9C9461_2F96_11D3_9D19_002018816EA4__INCLUDED_)
#define AFX_RECORDSNIDOSCOM_H__BA9C9461_2F96_11D3_9D19_002018816EA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIControlRecord.h"

class CRecordSniDosCom : public CSDIControlRecord  
{
// Status der gerade bearbeiteten Daten
// wie Start, Datenlaenge, Ansteuerungspunkt etc.
enum eStatus
{
	STATUS_UNKNOWN=-1,
	STATUS_EARLY_TDR=0,
	STATUS_EARLY_TDR_1=1,
	STATUS_EARLY_TDR_2=2,
	STATUS_TRANSAKTIONSANTWORT_1=3,
	STATUS_AUTORISIERUNG=4,
	STATUS_AUTORISIERUNG_1=5,
	STATUS_AUTORISIERUNG_2=6,
	STATUS_TRANSAKTIONSANTWORT_2=7,
	STATUS_NORMAL_STATUS=8,
	STATUS_NORMAL_STATUS_1=9,
	STATUS_NORMAL_STATUS_2=10,
	STATUS_TRANSAKTIONSANTWORT_3=11,
};

// Construction
public:
	CRecordSniDosCom(SDIControlType type,
					  int iCom,
					  BOOL bTraceAscii,
					  BOOL bTraceHex,
					  BOOL bTraceEvents);

// Overrides
public:
	virtual	~CRecordSniDosCom();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();
protected:
private:

// implementation
public:
protected:
	void	CheckLine();
	void	CheckData();
	void	CheckDataEarlyTdr();
	void	CheckDataEarlyTdr1();
	void	CheckDataEarlyTdr2();
	void	CheckDataTransaktionsantwort1();
	void	CheckDataAutorisierung();
	void	CheckDataAutorisierung1();
	void	CheckDataAutorisierung2();
	void	CheckDataTransaktionsantwort2();
	void	CheckDataNormalStatus();
	void	CheckDataNormalStatus1();
	void	CheckDataNormalStatus2();
	void	CheckDataTransaktionsantwort3();
	void	TransmitDataToSecurity(WORD wAlarm);
private:

// variables
public:
protected:
private:
	eStatus		m_eStatus;				// Status der gerade empfangenen Daten
};

#endif // !defined(AFX_RECORDSNIDOSCOM_H__BA9C9461_2F96_11D3_9D19_002018816EA4__INCLUDED_)
