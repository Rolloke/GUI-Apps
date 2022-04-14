// GaRS232.h: interface for the CGaRS232 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GARS232_H__AF7B26A3_D13C_11D2_B9BC_00400531137E__INCLUDED_)
#define AFX_GARS232_H__AF7B26A3_D13C_11D2_B9BC_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
#define	_STX_	0x02
#define	_ETX_	0x03
#define	_US_	0x1F
#define	_ACK_	0x06
#define	_NAK_	0x15
#define	_SP_	0x20
*/

class CGaRS232 : public CWK_RS232  
{
public:
	CGaRS232(int iCom, BOOL bTraceAscii, BOOL bTraceHex, BOOL bTraceEvents);
	virtual ~CGaRS232();

// Overrides
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
};

#endif // !defined(AFX_GARS232_H__AF7B26A3_D13C_11D2_B9BC_00400531137E__INCLUDED_)
