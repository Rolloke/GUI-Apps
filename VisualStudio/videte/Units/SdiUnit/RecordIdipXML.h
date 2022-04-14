#pragma once
#include "sdicontrolrecord.h"

class CRecordIdipXML : public CSDIControlRecord
{
public:
	CRecordIdipXML(SDIControlType type,
					int iCom,
					int iPort,
					LPCTSTR szIPAddress,
					BOOL bTraceAscii,
					BOOL bTraceHex);
	virtual ~CRecordIdipXML();

	// Overrides
public:
	virtual BOOL Create(CWK_Profile& wkp, const CString& sSection);

protected:
	virtual void OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL CheckForNewAlarmData();
	virtual void ThreadIsStarted();
	virtual void ThreadIsStopped();

public:	
	static IDispatch* ParseNode(IDispatch*pNode, const CString &sSearch, CString &sResult);

private:
	BOOL m_bUnicode;
	CWK_String m_sXML;
};
