/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordStarInterface.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordStarInterface.h $
// CHECKIN:		$Date: 21.12.05 15:29 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#pragma once
#include "sdicontrolrecord.h"

class CStarInterface;

class CRecordStarInterface : public CSDIControlRecord
{
	// construction
public:
	CRecordStarInterface(SDIControlType type,
						int iCom,
						BOOL bTraceAscii,
						BOOL bTraceHex,
						BOOL bTraceEvents);
	virtual ~CRecordStarInterface();

	// Overrides
public:
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
	virtual BOOL	IsValid() const;
	virtual void	SwitchToConfigMode(const CString& sPath, BOOL bReceive);

	// from CSDIWindow
	void OnCardEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR ReaderIndex, LPCTSTR IDNumber, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus, LPCTSTR FunctionKey);
	void OnReceiveDataStarinterfacectrl(short nIndex, LPCTSTR ReceiveString);
	void OnSendDataStarinterfacectrl(short nIndex, LPCTSTR SendString);

	// Overrides
protected:
	virtual void	InitializeResponses();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();

	// Implementation
protected:
	CStarInterface* GetStarInterfaceCtrl();
	CSDIStarDevices m_StarDevices;
	BOOL			m_bComOpen;
};
