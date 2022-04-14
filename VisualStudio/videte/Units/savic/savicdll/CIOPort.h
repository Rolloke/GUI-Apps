/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cioport.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/cioport.h $
// CHECKIN:		$Date: 24.06.02 13:14 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 24.06.02 12:03 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CIOPORT_H__ 
#define __CIOPORT_H__

#include "CIo.h"

class CIoPort : public CIo
{
public:
	CIoPort(DWORD dwIOBase);
	virtual ~CIoPort();

protected:
	virtual BYTE	Input8(DWORD dwIOAddr) const;						// 8Bit  Port-Lesezugriff
	virtual WORD	Input16(DWORD dwIOAddr) const;						// 16Bit Port-Lesezugriff
	virtual DWORD	Input32(DWORD dwIOAddr) const;						// 32Bit Port-Lesezugriff
	virtual void	Output8(DWORD dwIOAddr, BYTE byDatabyte) const;		// 8Bit	 Port-Schreibzugriff
	virtual void	Output16(DWORD dwIOAddr, WORD wDatabyte) const;		// 16Bit Port-Schreibzugriff
	virtual void	Output32(DWORD dwIOAddr, DWORD dwDatabyte) const;	// 32Bit Port-Schreibzugriff
private:

};												

#endif // __CIOPORT_H__
