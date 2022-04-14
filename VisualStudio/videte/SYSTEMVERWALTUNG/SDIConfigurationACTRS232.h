// SDIConfigurationACTRS232.h: interface for the CSDIConfigurationACTRS232 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONACTRS232_H__07767F55_9491_45F0_BAD7_1FE245FB38CC__INCLUDED_)
#define AFX_SDICONFIGURATIONACTRS232_H__07767F55_9491_45F0_BAD7_1FE245FB38CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationRS232.h"

class CSDIConfigurationACTRS232 : public CSDIConfigurationRS232  
{
public:
	CSDIConfigurationACTRS232(CSDIConfigurationDialog* pParent,
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

#endif // !defined(AFX_SDICONFIGURATIONACTRS232_H__07767F55_9491_45F0_BAD7_1FE245FB38CC__INCLUDED_)
