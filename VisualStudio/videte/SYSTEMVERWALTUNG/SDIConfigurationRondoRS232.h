// SDIConfigurationRondoRS232.h: interface for the CSDIConfigurationRondoRS232 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONRONDORS232_H__AB98AC67_5EA2_11D3_BA71_00400531137E__INCLUDED_)
#define AFX_SDICONFIGURATIONRONDORS232_H__AB98AC67_5EA2_11D3_BA71_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationRS232.h"

#define RONDO_MSG_HEADER_LEN	15

class CSDIConfigurationRondoRS232 : public CSDIConfigurationRS232  
{
// Status der gerade bearbeiteten Daten
enum eStatus
{
	STATUS_STX,
	STATUS_COMMAND,
	STATUS_SEPARATOR_1,
	STATUS_DATE,
	STATUS_TIME,
	STATUS_SEPARATOR_2,
	STATUS_DATA,
	STATUS_ETX,
	STATUS_CRC,
	STATUS_OK,
};
public:
	CSDIConfigurationRondoRS232(CSDIConfigurationDialog* pParent, 
													 int iCom,
													 BOOL bTraceAscii = TRUE,
													 BOOL bTraceHex = FALSE,
													 BOOL bTraceEvents = FALSE);
	virtual ~CSDIConfigurationRondoRS232();

// Attributes
public:

// Operations
public:

// Overrides
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);

// Implementation
public:
protected:
	void	InitializeResponses();

	eStatus		m_eStatus;			// Status des gerade bearbeiteten Datums
};

#endif // !defined(AFX_SDICONFIGURATIONRONDORS232_H__AB98AC67_5EA2_11D3_BA71_00400531137E__INCLUDED_)
