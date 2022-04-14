// SDIConfigurationRS232NCTDiva.h: interface for the CSDIConfigurationRS232NCTDiva class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONRS232NCTDIVA_H__83B37859_02E6_4B05_9AAE_7C27580373B2__INCLUDED_)
#define AFX_SDICONFIGURATIONRS232NCTDIVA_H__83B37859_02E6_4B05_9AAE_7C27580373B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationRS232.h"

class CSDIConfigurationRS232NCTDiva : public CSDIConfigurationRS232  
{
public:
	CSDIConfigurationRS232NCTDiva(CSDIConfigurationDialog* pParent,
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
	void	CheckData();
};

#endif // !defined(AFX_SDICONFIGURATIONRS232NCTDIVA_H__83B37859_02E6_4B05_9AAE_7C27580373B2__INCLUDED_)
