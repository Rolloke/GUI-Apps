/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cio.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/cio.h $
// CHECKIN:		$Date: 22.10.02 13:49 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 22.10.02 12:43 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CIO_H__ 
#define __CIO_H__

class CIo
{
public:
	BYTE Input(DWORD dwPort);							// 8Bit  Lesezugriff
	WORD Inputw(DWORD dwPort);							// 16Bit Lesezugriff
	DWORD Inputdw(DWORD dwPort);						// 16Bit Lesezugriff
	void Output(DWORD dwPort, BYTE byDatabyte);			// 8Bit	 Schreibzugriff
	void Outputw(DWORD dwPort, WORD wDatabyte); 		// 16Bit Schreibzugriff
	void Outputdw(DWORD dwPort, DWORD dwDatabyte); 		// 16Bit Schreibzugriff
	virtual ~CIo();
	DWORD GetBaseAddress() const {return m_dwIOBase;}

protected:
	CIo();
	CIo(DWORD dwIOBase);

	// Diese Funktionen müssen überschrieben werden.
	virtual BYTE	Input8(DWORD dwPort) const = 0;						// 8Bit  Lese-Zugriff
	virtual WORD	Input16(DWORD dwPort) const = 0;					// 16Bit Lese-Zugriff
	virtual DWORD	Input32(DWORD dwPort) const = 0;					// 16Bit Lese-Zugriff
	virtual void	Output8(DWORD dwPort, BYTE byDatabyte) const = 0;	// 8Bit	 Schreib-Zugriff
	virtual void	Output16(DWORD dwPort, WORD wDatabyte) const = 0;	// 16Bit Schreib-Zugriff
	virtual void	Output32(DWORD dwPort, DWORD dwDatabyte) const = 0;	// 16Bit Schreib-Zugriff

	DWORD m_dwIOBase;

private:
//	static CCriticalSection m_csIo;					// Synchronisationsobjekt

};												

#endif // __CIO_H__
