// SDIConfigurationNcrRS232.h : header file
//
#if !defined(AFX_SDICONFIGURATIONNCRRS232_H__E2F6B1D4_85D5_11D2_B043_004005A1D890__INCLUDED_)
#define AFX_SDICONFIGURATIONNCRRS232_H__E2F6B1D4_85D5_11D2_B043_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationRS232.h"

class CSDIConfigurationDialog;

// Status der gerade bearbeiteten Daten
enum eStatusNCR
{
	NCR_OK,
	NCR_STX,
	NCR_DATA,
	NCR_ETX,
	NCR_CRC,
};

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationNcrRS232
class CSDIConfigurationNcrRS232 : public CSDIConfigurationRS232
{
// Construction
public:
	CSDIConfigurationNcrRS232(CSDIConfigurationDialog* pParent,
							  int iCom,
							  BOOL bTraceAscii = TRUE,
							  BOOL bTraceHex = FALSE,
							  BOOL bTraceEvents = FALSE);
	
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

	eStatusNCR	m_eStatus;	// Status des gerade bearbeiteten Datums
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGURATIONNCRRS232_H__E2F6B1D4_85D5_11D2_B043_004005A1D890__INCLUDED_)
