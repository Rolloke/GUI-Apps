// ControlRecord.h: interface for the CControlRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLRECORD_H__FDEEA2FC_997F_4DBE_88E0_B6CC94FF4404__INCLUDED_)
#define AFX_CONTROLRECORD_H__FDEEA2FC_997F_4DBE_88E0_B6CC94FF4404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CControlRecord : public  CWK_RS232  
{
public:
	CControlRecord(	int iCom,
					BOOL bTraceAscii,
					BOOL bTraceHex,
					BOOL bTraceEvents
					);
	virtual ~CControlRecord();

// access
public:

// overrides
public:
	virtual BOOL	Create() = 0;
	virtual void	Destroy();
	virtual void	InitialiseResponses();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);

// implementation
public:
protected:
	void	ClearAllData();
// data
protected:
	CByteArray	m_byaReceived;		
	CByteArray	m_byaACK;
	CByteArray	m_byaNAK;
	CByteArray	m_byaStatusResponse;
};
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CControlRecordArray, CControlRecord*)
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CONTROLRECORD_H__FDEEA2FC_997F_4DBE_88E0_B6CC94FF4404__INCLUDED_)
