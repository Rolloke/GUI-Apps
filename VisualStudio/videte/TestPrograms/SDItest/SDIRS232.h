/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIRS232.h $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/SDIRS232.h $
// CHECKIN:		$Date: 21.12.05 16:50 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 21.12.05 12:58 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _SDIRS232_H_
#define _SDIRS232_H_

class CSDISocket;
////////////////////////////////////////////////////////
class CSDIRS232 : public CWK_RS232
{
	friend class CSDISocket;
	//	Construction / Destruction
public:
	CSDIRS232(int iCom,
			  BOOL bTraceAscii,
			  BOOL bTraceHex,
			  BOOL bTraceEvents);
	~CSDIRS232();

	// implementation
public:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual void	OnTimeOutTransparent();
	virtual void	SwitchToConfigMode(const CString& sPath, BOOL bReceive);

	// data
private:
};
////////////////////////////////////////////////////////////
#endif
