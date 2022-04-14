// SDIConfigurationRS232.h : header file
//
#if !defined(AFX_SDICONFIGURATIONRS232_H__E2F6B1D3_85D5_11D2_B043_004005A1D890__INCLUDED_)
#define AFX_SDICONFIGURATIONRS232_H__E2F6B1D3_85D5_11D2_B043_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSDIConfigurationDialog;


/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationRS232
class CSDIConfigurationRS232 : public CWK_RS232
{
// Construction
public:
	CSDIConfigurationRS232(CSDIConfigurationDialog* pParent,
						   int iCom,
						   BOOL bTraceAscii = TRUE,
						   BOOL bTraceHex = FALSE,
						   BOOL bTraceEvents = FALSE);

	virtual ~CSDIConfigurationRS232();

// Attributes
public:

// Operations
public:
	CString	GetAndRemoveData();

// Overrides
	virtual void	OnDataComplete();
	virtual void	OnTimeOutTransparent();

// Implementation
public:
	void	StartReadInData();
	void	StopReadInData();
	void	AddData(const CString& sData);
protected:
	void	ClearActualData();
	void	ClearAllData();
	void	SendACK();
	void	SendNAK();
	void	RespondStatusRequest();

	CSDIConfigurationDialog* m_pSDIConfigurationDialog;
	BOOL				m_bReadInData;		// Einlesen der Daten
	CString				m_sBuffer;			// Buffer zum Einlesen der Daten
	CString				m_sDataComplete;	// kompletter Datansatz (inkl. Header)
	int					m_iReceivedTotal;	// Laenge der insgesamt empfangenen Daten
	int					m_iDataLen;			// Laenge der angekuendigten Daten
	BYTE				m_byXORCheckSum;	// Checksumme der empfangenen Daten
	int					m_iLenDataActual;	// Aktuelle Laenge der zu bearbeitenden Daten
	BOOL				m_bStatusRequest;	// StatusRequest empfangen
	CByteArray			m_byaACK;
	CByteArray			m_byaNAK;
	CByteArray			m_byaStatusResponse;
	CCriticalSection	m_CS;
private:
	CStringArray		m_sDataArray;		// Array fuer die eingelesenen Daten
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGURATIONRS232_H__E2F6B1D3_85D5_11D2_B043_004005A1D890__INCLUDED_)
