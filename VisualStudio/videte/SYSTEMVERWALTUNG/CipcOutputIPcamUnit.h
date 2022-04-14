/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcOutputIPcamUnit.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/CipcOutputIPcamUnit.h $
// CHECKIN:		$Date: 30.08.06 10:36 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 30.08.06 10:04 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CipcOutput.h"

class CCameraParameterPage;

class CConfirmValue
{
public:
	CConfirmValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);

	CSecID	m_id;
	CString	m_sKey;
	CString	m_sValue;
	DWORD	m_dwUserData;
};

#define ONCONFIRMGETVALUE 1
#define ONCONFIRMSETVALUE 2

class CIPCOutputIPcamUnit : public CIPCOutput
{
public:
	CIPCOutputIPcamUnit(CCameraParameterPage* pMainWnd, 
					   LPCTSTR shmName, BOOL bAsMaster=FALSE);
	~CIPCOutputIPcamUnit();
	// Attributes
	BOOL IsConnected();
	inline WORD GetGroupID() const;

	// Operations
	BOOL CheckGroupID(WORD wGid, LPCTSTR sFunction);

	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmGetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData);
	virtual void OnConfirmSetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData);

   // RequestXXX
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestDisconnect();

	// Connection
private:
	CCameraParameterPage*	m_pParamDlg;
	WORD					m_wGroupID;
};

inline WORD CIPCOutputIPcamUnit::GetGroupID() const
{
	return m_wGroupID;
}
