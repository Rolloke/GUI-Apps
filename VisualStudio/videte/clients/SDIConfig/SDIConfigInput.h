/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigInput.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigInput.h $
// CHECKIN:		$Date: 1.03.99 14:29 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 1.03.99 14:29 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDICONFIGINPUT_H__6FF23102_6638_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDICONFIGINPUT_H__6FF23102_6638_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "CIPCInputClient.h"
#include "SDIConfigRecord.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIConfigDoc;
class CSDIConfigView;
class CSDIConfigRecord;

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigInput
class CSDIConfigInput : public CIPCInputClient
{
// construction / destruction
public:
	CSDIConfigInput(const char *shmName, CSecID id, CSDIConfigDoc* pDoc);
	~CSDIConfigInput();

// attributes
public:
	inline	BOOL		HasInfo();
	inline	const DWORD	GetBitrate() const;
			CString		GetCommentFromRecordArrayByGroupID(CSecID id);

// overridables
protected:
	virtual BOOL Run(DWORD dwTimeOut);	// default is GetTimeoutCmdReceive()
	virtual void OnReadChannelFound();
	virtual void OnIndicateError(DWORD dwCmd,
								 CSecID id,
								 CIPCError error,
								 int iErrorCode,
								 const CString &sErrorMessage);
	virtual void OnRequestDisconnect();
	virtual void OnConfirmConnection();
	virtual void OnConfirmBitrate(DWORD dwBitrate);
	virtual void OnConfirmInfoInputs(WORD wGroupID,
									 int iNumGroups,
									 int numRecords,
									 const CIPCInputRecord records[]);
	virtual void OnConfirmFileUpdate(const CString &sFileName);
	virtual void OnConfirmGetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnConfirmSetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnAddRecord(const CIPCInputRecord& pRec);
	virtual void OnUpdateRecord(const CIPCInputRecord& pRec);
	virtual void OnDeleteRecord(const CIPCInputRecord& pRec);

// implementation
protected:
	void				AddSDIInput(CSecID id, CString& sComment);
	void				DeleteSDIInput(CSecID id);
	CSDIConfigRecord*	FindInputInArray(CSecID id);
	BOOL				FindInputGroupInArray(WORD wGroupID);
	void				RemoveInputFromArray(CSecID id);

// data
private:
	CSecID						m_ID;
	BOOL						m_bInfo;
	DWORD						m_dwBitrate;
	DWORD						m_dwTime;
	CSDIConfigDoc*				m_pDoc;
	CSDIConfigView*				m_pView;
	CString						m_sLastError;
	CSDIConfigRecordArray_CS	m_RecordArray;		// Array der vorhandenen ControlRecords
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIConfigInput::HasInfo()
{
	return m_bInfo;
}
/////////////////////////////////////////////////////////////////////////////
inline const DWORD CSDIConfigInput::GetBitrate() const
{
	return m_dwBitrate;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGINPUT_H__6FF23102_6638_11D1_9F29_0000C036AC0D__INCLUDED_)
