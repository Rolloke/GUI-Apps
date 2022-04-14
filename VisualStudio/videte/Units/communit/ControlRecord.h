// ControlRecord.h: interface for the CControlRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLRECORD_H__CE72882A_5C6E_4ECC_B250_D22E55B8AFEE__INCLUDED_)
#define AFX_CONTROLRECORD_H__CE72882A_5C6E_4ECC_B250_D22E55B8AFEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CameraCommandRecord.h"

#include "WKClasses\UnhandledException.h"

/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
class CCommandRecord : public CByteArray  
{
public:
	CCommandRecord(BOOL bRepeat = FALSE) {m_bRepeatAllowed = bRepeat;}
	BOOL	m_bRepeatAllowed;
	inline  BYTE	GetAtFast(int i);
	static	BOOL	IsTelemetryCommand(CameraControlCmd cmd);
	static  BOOL	IsPositionCmd(CameraControlCmd cmd);
};
/////////////////////////////////////////////////////////////////////////////
inline BYTE CCommandRecord::GetAtFast(int i)
{
	ASSERT(IsInArray(i, m_nSize));
	return m_pData[i];
}
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY_CS(CCommandArray, CCommandRecord*, CCommandArrayCS)
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
class CControlRecord : public CWK_RS232  
{
public:
	CControlRecord(	CameraControlType type,
					int iCom,
					BOOL bTraceAscii,
					BOOL bTraceHex,
					BOOL bTraceEvents
					);
	virtual ~CControlRecord();

// access
public:
	inline	CameraControlType	GetType() const;
			BOOL				HasSecID(CSecID secID);

// overrides
public:
	virtual BOOL	Create() PURE_VIRTUAL(0)
	virtual void	Destroy();
	virtual void	InitialiseResponses();
	virtual BOOL	Add(CSecID secID, DWORD camID);
	virtual void	OnNewCommand(CCameraCommandRecord* pCCommandR) PURE_VIRTUAL(;)
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);

// implementation
public:
	BOOL	ReadPortValues(DWORD& dwBaudRate, BYTE& byDataBits, BYTE& byParity, BYTE& byStopBits);
	CString GetRegPath();
protected:
	void	ClearAllData();
	static DWORD CheckBaudRate(DWORD dwBaudRate);
// data
protected:
	CDWordArray		m_secIDs;
	CDWordArray		m_camIDs;
//	CMapPtrToPtr	m_mapSecID_CamID;

	CCommandArrayCS		m_arrayCommand;
	CCommandRecord*		m_pCurrentCommand;
	CCriticalSection	m_csSendCommand;

	CCommandRecord	m_byaReceived;		
	CCommandRecord	m_byaStatusResponse;

private:
	CameraControlType	m_Type;
};
/////////////////////////////////////////////////////////////////////////////
inline CameraControlType CControlRecord::GetType() const
{
	return m_Type;
}
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CControlRecordArray, CControlRecord*)
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CONTROLRECORD_H__CE72882A_5C6E_4ECC_B250_D22E55B8AFEE__INCLUDED_)
