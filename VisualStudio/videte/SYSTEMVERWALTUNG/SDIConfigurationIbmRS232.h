/* GlobalReplace: CSDIConfigurationRS232 --> CSDIConfigurationIbmRS232 */
// SDIConfigurationRS232.h : header file
//
#if !defined(AFX_SDICONFIGURATIONRS232_H__ACE7ED43_4C7D_11D2_AFEF_00C095EC2267__INCLUDED_)
#define AFX_SDICONFIGURATIONRS232_H__ACE7ED43_4C7D_11D2_AFEF_00C095EC2267__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationRS232.h"

class CSDIConfigurationDialog;

// Status der gerade bearbeiteten Daten
enum eStatusIBM
{
	IBM_OK,
	IBM_STX,
//	IBM_UNKNOWN,
	IBM_LENGTH,
//	IBM_FORMAT,
//	IBM_WSID,
//	IBM_DATE_WS,
//	IBM_TIME_WS,
//	IBM_TAN_WS,
//	IBM_PHOTO_STEP,
//	IBM_STATE_WS,
	IBM_DATA,
	IBM_ETX,
	IBM_CRC,
};

#define IBM_MSG_HEADER_LEN	28

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationIbmRS232
class CSDIConfigurationIbmRS232 : public CSDIConfigurationRS232
{
public:
	CSDIConfigurationIbmRS232(CSDIConfigurationDialog* pParent,
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

	eStatusIBM			m_eStatus;			// Status des gerade bearbeiteten Datums
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGURATIONRS232_H__ACE7ED43_4C7D_11D2_AFEF_00C095EC2267__INCLUDED_)
