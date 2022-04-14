// SDIConfigurationMicrolockRS232.h: interface for the CSDIConfigurationMicrolockRS232 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONMICROLOCKRS232_H__005545AE_D98A_4180_9AB7_6FDAD28523C6__INCLUDED_)
#define AFX_SDICONFIGURATIONMICROLOCKRS232_H__005545AE_D98A_4180_9AB7_6FDAD28523C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationRS232.h"

class CSDIConfigurationMicrolockRS232 : public CSDIConfigurationRS232  
{
public:
	CSDIConfigurationMicrolockRS232(CSDIConfigurationDialog* pParent,
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
private:
	CString	m_sEndOfLine;
	int		m_iEndOfLineLen;
};

#endif // !defined(AFX_SDICONFIGURATIONMICROLOCKRS232_H__005545AE_D98A_4180_9AB7_6FDAD28523C6__INCLUDED_)
