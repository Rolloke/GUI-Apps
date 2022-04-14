/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: ciomemory.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/ciomemory.h $
// CHECKIN:		$Date: 24.06.02 13:14 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 24.06.02 12:03 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CIOMEMORY_H__ 
#define __CIOMEMORY_H__

#include "CIo.h"

class CIoMemory	: public CIo
{
public:
	CIoMemory(DWORD dwIOBase);
	virtual ~CIoMemory();

protected:
	virtual BYTE	Input8(DWORD wIOAddr) const;						// 8Bit  Memory-Lesezugriff
	virtual WORD	Input16(DWORD wIOAddr) const;						// 16Bit Memory-Lesezugriff
	virtual DWORD	Input32(DWORD wIOAddr) const;						// 32Bit Memory-Lesezugriff
	virtual void	Output8(DWORD wIOAddr, BYTE byDatabyte) const;		// 8Bit	 Memory-Schreibzugriff
	virtual void	Output16(DWORD wIOAddr, WORD wDatabyte) const; 		// 16Bit Memory-Schreibzugriff
	virtual void	Output32(DWORD wIOAddr, DWORD dwDatabyte) const;	// 32Bit Memory-Schreibzugriff

private:
};												

#endif // __CIOMEMORY_H__
