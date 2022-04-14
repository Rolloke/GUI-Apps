/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CRS232.h $
// ARCHIVE:		$Archive: /Project/Units/CommUnit/CRS232.h $
// CHECKIN:		$Date: 20.01.05 9:42 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 19.01.05 16:13 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _CRS232_H_
#define _CRS232_H_

#define	_STX_	0x02
#define	_ETX_	0x03
#define	_US_	0x1F
#define	_ACK_	0x06
#define	_NAK_	0x15
#define	_SP_	0x20

////////////////////////////////////////////////////////
UINT WatchCommProc(LPVOID pParam);
////////////////////////////////////////////////////////
class CRS232
{
	//	Construction / Destruction
public:
	CRS232(int iCom = 2);
	~CRS232();

public:
	int	    GetCOMNumber() const;
	// operations
public:
	BOOL	Open(CameraControlType eType, DWORD BaudRate = CBR_9600, BYTE ByteSize = 8,
				 BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT);
	void	Close();

	DWORD	Write(LPVOID pBuffer, DWORD dwLen);
	BOOL	IsOpen() {return m_bOpen;};

	LPVOID  GetReadBuffer() const;
	DWORD   GetReadBufferLength() const;
	void    DeleteReadBuffer();

	// implementation
protected:
	BOOL Setup();
	BOOL StartThread();
	void Read();
	// data
private:
	HANDLE		m_hCommFile;
	BOOL		m_bOpen;
	int			m_COMNumber;
	DCB			m_dcb;

	OVERLAPPED	m_OLRead;
	OVERLAPPED	m_OLWrite;
	LPVOID		m_pReadBuffer;
	DWORD		m_dwBufferLen;

	static	UINT WatchCommProc(LPVOID pParam);
};
////////////////////////////////////////////////////////////
inline LPVOID CRS232::GetReadBuffer() const
{
	return m_pReadBuffer;
}
////////////////////////////////////////////////////////////
inline DWORD CRS232::GetReadBufferLength() const
{
	return m_dwBufferLen;
}
////////////////////////////////////////////////////////////
inline int CRS232::GetCOMNumber() const
{
	return m_COMNumber;
}
////////////////////////////////////////////////////////////
#endif