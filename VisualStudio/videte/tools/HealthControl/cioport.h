/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cioport.h $
// ARCHIVE:		$Archive: /Project/Tools/HealthControl/HealthControl/cioport.h $
// CHECKIN:		$Date: 24.05.05 9:57 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 23.05.05 8:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CIOPORT_H__ 
#define __CIOPORT_H__
	    
#include "CIoPort.h"

typedef BYTE   (*DAREADPORT8)(DWORD dwAddr);
typedef WORD   (*DAREADPORT16)(DWORD dwAddr);
typedef DWORD  (*DAREADPORT32)(DWORD dwAddr);

typedef BOOL   (*DAWRITEPORT8)(DWORD dwAddr,  BYTE byVal);
typedef BOOL   (*DAWRITEPORT16)(DWORD dwAddr, WORD wVal);
typedef BOOL   (*DAWRITEPORT32)(DWORD dwAddr, DWORD dwVal);

typedef BOOL   (*DAISVALID)();

class CIoPort
{
public:
	CIoPort(DWORD dwIOBase);
	~CIoPort();
	
	BOOL IsValid();

	BYTE	Input8(DWORD dwIOAddr) const;						// 8Bit Port-Lesezugriff
	WORD	Input16(DWORD dwIOAddr) const;						// 16Bit Port-Lesezugriff
	DWORD	Input32(DWORD dwIOAddr) const;						// 32Bit Port-Lesezugriff
	void	Output8(DWORD dwIOAddr, BYTE byData) const;			// 8Bit Port-Schreibzugriff
	void	Output16(DWORD dwIOAddr, WORD wData) const;			// 16Bit Port-Schreibzugriff
	void	Output32(DWORD dwIOAddr, DWORD dwData) const;		// 32Bit Port-Schreibzugriff

private:
	DWORD m_dwIOBase;

	static	HINSTANCE		m_hDADll;
	static	DAREADPORT8		m_DAReadPort8;
	static	DAREADPORT16	m_DAReadPort16;
	static	DAREADPORT32	m_DAReadPort32;
	static	DAWRITEPORT8	m_DAWritePort8;
	static	DAWRITEPORT16	m_DAWritePort16;
	static	DAWRITEPORT32	m_DAWritePort32;
	static	DAISVALID		m_DAIsValid;
	
	static int		m_nInstanceCounter;
	static BOOL		m_bInitOK;
};												

#endif // __CIOPORT_H__
