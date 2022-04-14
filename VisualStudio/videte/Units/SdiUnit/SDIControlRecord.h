/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIControlRecord.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIControlRecord.h $
// CHECKIN:		$Date: 20.01.06 11:44 $
// VERSION:		$Revision: 43 $
// LAST CHANGE:	$Modtime: 20.01.06 11:44 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef RecordH
#define RecordH

#include "SDIInput.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIInput;

/////////////////////////////////////////////////////////////////////////////
class CSDIControlRecord : public CWK_RS232 , public CAsyncSocket
{
// construction
public:
	CSDIControlRecord(SDIControlType type,
					  int iCom,
					  int iPort,
					  LPCTSTR szIPAddress,
					  BOOL bTraceAscii,
					  BOOL bTraceHex,
					  BOOL bTraceEvents);
	virtual ~CSDIControlRecord();

// attributes
public:
	inline SDIControlType	GetType() const;
	inline const CString&	GetComment() const;
	inline CSDIInput*		GetInput(int i=0);
	inline int				GetPortNumber() const;

// Overrides
public:
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection) PURE_VIRTUAL(0)
	virtual BOOL	IsValid() const;
	virtual void	SwitchToConfigMode(const CString& sPath, BOOL bReceive);
protected:
	virtual void	InitializeResponses();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen) PURE_VIRTUAL(;)
	virtual BOOL	CheckForNewAlarmData() PURE_VIRTUAL(0)
	virtual BOOL	UpdateAlarms();

// implementation
public:
protected:
	BOOL	CreateRecord();
	BOOL	InitializeStandardMembers();
	BOOL	OpenCom(DWORD dwBaudRate,
					BYTE byDataBits,
					BYTE byParity,
					BYTE byStopBits);
	void	ClearActualData();
	void	ClearAllData();
	void	StoreDate(eSDIDataFormat eFormat);
	void	StoreDate(CString sFieldName, eSDIDataFormat eFormat);
	void	StoreTime(eSDIDataFormat eFormat);
	void	StoreTime(CString sFieldName, eSDIDataFormat eFormat);
	void	StoreAccount();
	void	StoreBankCode();
	void	StoreTAN();
	void	StoreWorkstationID();
	void	StoreValue();
	void	StoreCurrency();
	void	StoreNewDataBaseFields(CString sFieldName);
//	void	StoreProcessID();
//	void	StoreFirstName();
//	void	StoreLastName();
//	void	StoreCustomerID();
//	void	StoreBoxID();
//	void	StoreIstBetrag();
//	void	StoreSollBetrag();
//	void	StoreDifferenzBetrag();
	void	IndicateAlarm(WORD wAlarm, int iKind=2); // iKind 1=Alarm on, 0 = Alarm off, 2 = Alarm Peak
	void	UpdateAlarm(WORD wAlarm);
	void	SendACK();
	void	SendNAK();
	void	RespondStatusRequest();
	void	Close();

private:
	void	RefreshDataForSend();
	void	SetStandardDataMembers();
	void	UpdateFormerAlarms();
	void	RealUpdateAlarm(WORD wAlarm);
	void	CheckForNonDigitsInValue();
	CString	GetCenturyFor2DigitYear(CString s2DigitYear);
	BOOL	Get2DigitMonthFromString(CString sMonthString, CString& sMonth2Digits);

// data
protected:
	SDIControlType	m_Type;
	UINT			m_iPort;
	CString			m_sIPAddress;
	CSDIInputs		m_Inputs;
	int				m_iMaxBufferLen;	// max. zugelassene Laenge fuer empfangene Daten
	CString			m_sComment;
	CString			m_sCurrency;
	CString			m_sWholeData;		// Buffer zum Einlesen der Daten
	CString			m_sSingleData;		// Buffer fue einzelne Daten
	int				m_iReceivedTotal;	// Laenge der insgesamt empfangenen Daten
	int				m_iDataLen;			// Laenge der angekuendigten Daten
	BYTE			m_byXORCheckSum;	// Checksumme der empfangenen Daten
	int				m_iLenDataActual;	// Aktuelle Laenge der zu bearbeitenden Daten
	BOOL			m_bStatusRequest;	// StatusRequest empfangen
	CByteArray		m_byaACK;
	CByteArray		m_byaNAK;
	CByteArray		m_byaStatusResponse;
	CIPCFields		m_DataFieldArrayActual;
private:
	CIPCFields		m_DataFieldArrayToSend;
	CWordArray		m_wFormerAlarmsArray;
public:
	virtual void OnReceive(int nErrorCode);
};
/////////////////////////////////////////////////////////////////////////////
inline SDIControlType CSDIControlRecord::GetType() const
{
	return m_Type;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CSDIControlRecord::GetComment() const
{
	return m_sComment;
}
/////////////////////////////////////////////////////////////////////////////
inline CSDIInput* CSDIControlRecord::GetInput(int i/*=0*/)
{
	if (0<=i&&i<m_Inputs.GetSize())
	{
		return m_Inputs.GetAt(i);
	}
	return NULL;
}
inline int CSDIControlRecord::GetPortNumber() const
{
	return m_iPort;
}

/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CSDIControlRecordArray, CSDIControlRecord*)
/////////////////////////////////////////////////////////////////////////////
#endif
