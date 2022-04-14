// ControlRecordBaxall.h: interface for the CControlRecordBaxall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLRECORDBAXALL_H__4671D7FB_BE5B_4E09_A2AF_6BFA6A23ADF9__INCLUDED_)
#define AFX_CONTROLRECORDBAXALL_H__4671D7FB_BE5B_4E09_A2AF_6BFA6A23ADF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlRecord.h"

class CControlRecordBaxall : public CControlRecord  
{
public:
	CControlRecordBaxall(int iCom, BOOL bTraceHex);
	virtual ~CControlRecordBaxall();

// overrides
public:
	virtual BOOL	Create();
	virtual void	InitialiseResponses();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual void	OnTimeOutTransparent();

// implementation
protected:
private:
	void	FillHeader(BYTE byDestID);
	void	FillAckNakStatus(BYTE byResponse);
	void	FillCRCandFooterPAD();
	void	CheckData();
	BOOL	CheckLengthAndCRC();
	void	DataCorrupt(int iIndex);
	void	SendACK(BYTE byDestID);
	void	SendNAK(BYTE byDestID);
	void	SendStatusResponse(BYTE byDestID);
	void	Send();

// data
private:
	CByteArray	m_byArrayBaxall;
	BYTE		m_bySourceID;
	BOOL		m_bDecreaseBaxallAddCode;
};

#endif // !defined(AFX_CONTROLRECORDBAXALL_H__4671D7FB_BE5B_4E09_A2AF_6BFA6A23ADF9__INCLUDED_)
