/* GlobalReplace: m_idProcess --> m_idProcessMacro */
/* GlobalReplace: InputToOutput --> Activation */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: input.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/input.h $
// CHECKIN:		$Date: 17.07.06 16:40 $
// VERSION:		$Revision: 77 $
// LAST CHANGE:	$Modtime: 17.07.06 16:34 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __INPUT_H_
#define __INPUT_H_

#include "Activation.h"

class CProcess;
class CSecTimer;
class CIPCOutputServer;
class CIPCInputServer;
class CProcessScheduler;

// CActivation Struktur           
// byState
#define IO_NOSTATE			-1
#define IO_OK				0
#define IO_ALARM			1




////////////////////////////////////////////////////////////////////////////
// Definitionen für Input Struktur           
//
// Status-word(bitmask) der Struktur
#define I_STATE_DEFAULT		0x000C		//Default Free und Edge Positiv
#define I_STATE_CURRENT		0x0001		//Bit 0
#define I_STATE_ALARM		0x0002		//Bit 1
#define I_STATE_EDGE		0x0004		//Bit 2
#define I_STATE_NEEDS_CONFIRMATION 0x0008		//Bit 3
#define I_STATE_IS_COMM_PORT 0x0010		//Bit 4

//////////////////////////////////////////////////////////////////////
// Inputstruktur (class)
class CInput
{
	// construction / destruction
public:
	CInput(CIPCInputServer* pParent, WORD wSubID);
	~CInput();

	// attributes
public:
	inline BOOL	GetTempDeactivateAllowed() const;
	inline BOOL	GetEdge() const;
	inline BOOL	IsActive() const;
	inline BOOL IsUsedAsLockForTimer() const;
	inline BOOL IsMD() const;
	inline BOOL IsSDI() const;
	inline int  GetNumActions() const;
	inline CIPCInputServer* GetGroup();

	inline const CString&	GetName() const;
	inline const CSecID		GetID() const;
	inline const CSecID		GetIDActivate() const;
	inline		 CActivation&	GetActivation(int n) const;
	inline		 CIPCFields&	GetFields();
	inline const CSystemTimeSpans&	GetAlarmCallDeactivationSpans() const;

		   BOOL  IsInAlarmOffSpan();

	// settings
public:
	inline void SetName(const CString &s);
	inline void SetTempDeactivateAllowed(BOOL bTempDeactivateAllowed);
		   void SetActive(BOOL bIsActive, BOOL bStartProcesses);
	inline void	SetIDActivate(CSecID id);
		   void SetFields(const CIPCFields& fields);
		   void AddFields(const CIPCFields& fields);
		   void CopyFields(CIPCFields& fields);
		   void AddAction(CActivation* pNewActivation);
		   void SetAlarmOffSpans(int iNumRecords, 
								 const CSystemTimeSpan data[],
								 const CString& sUser,
								 const CString& sHost);
		   void DeleteFirstFields(int iNumFields, LPCTSTR szName);

	// operations
public:
	void LoadAlarmOffSpans(CWK_Profile& wkp,const CString& sSection);
	void SaveAlarmOffSpans(CWK_Profile& wkp,const CString& sSection);
	void RestartAlarmProcesses();
	void CreateMDOKActivation();
	BOOL RecalcAlarmOffSpans(const CSystemTime& st);
	void InformClientsAlarmOffSpans();

	//
	void ProcessTerminated(CProcess *pProcess);
	//
	void AddLockTimer(CSecTimer *pTimer);

	//
	void StartInitialProcesses();
	void StartMissingProcesses(WORD wOutputGroupID,
							   const CSecTimer *pTimer=NULL);
	// Implementation
protected:
	void StartAlarmProcesses();
	void StartMissingAlarmCalls();
	void StartOKProcesses();

	void CleanUpLockProcesses();
	void StartLockProcesses();
	void StopOKProcesses();
	void StopAlarmProcesses();

	void WriteAlarmOffSpanProtocol(const CString& sUser,
								   const CString& sHost);

	void IncrementKeepAlive();
	void DecrementKeepAlive();

	CProcessScheduler* GetScheduler(CActivation& activation);

public:	// public data
	WORD		wState;			// flags only!, flanke, freigabe usw.

	// data member
private:
	//
	CIPCInputServer* m_pParent;
	CActivationArray m_actions;
	CSecID			 m_id;			// InputID
	CString			 m_sName;
	CIPCFields		 m_Fields;
	CSecID			 m_idActivate;		// SECID_ACTIVE_XXX or TimerID
	BOOL			 m_bIsActive; // Alarm =TRUE / OK = FALSE
	//
	volatile int	 m_iNumKeepAliveProcesses;
	CCriticalSection m_csNumKeepAlive;

	// collection of associated timer, which use this input as lock
	CSecTimerArray m_lockTimers;

	// MNO alarm call deactivation spans
	BOOL				m_bTempDeactivateAllowed;
	BOOL				m_bAlarmOff;
	BOOL				m_bAlarmOffIn15Min;
	CSystemTimeSpans	m_AlarmCallDeactivationSpans;
	BOOL				m_bMD;
	BOOL				m_bSDI;

	BOOL				m_bNoMDRestart;
public:
	static BOOL m_bTraceAlarms;
};
////////////////////////////////////////////////////////////
inline BOOL CInput::GetEdge() const
{
	return (wState & I_STATE_EDGE)!=0;
}
////////////////////////////////////////////////////////////
inline BOOL CInput::GetTempDeactivateAllowed() const
{
	return m_bTempDeactivateAllowed;
}
////////////////////////////////////////////////////////////
inline BOOL CInput::IsActive() const
{
	return m_bIsActive;
}
////////////////////////////////////////////////////////////
inline const CString & CInput::GetName() const
{
	return m_sName;
}
////////////////////////////////////////////////////////////
inline const CSecID CInput::GetID() const
{
	return m_id;
}
////////////////////////////////////////////////////////////
inline const CSecID CInput::GetIDActivate() const
{
	return m_idActivate;
}
////////////////////////////////////////////////////////////
inline CIPCFields& CInput::GetFields()
{
	return m_Fields;
}
////////////////////////////////////////////////////////////
inline void CInput::SetIDActivate(CSecID id)
{
	m_idActivate = id;
}
////////////////////////////////////////////////////////////
inline void CInput::SetTempDeactivateAllowed(BOOL bTempDeactivateAllowed)
{
	m_bTempDeactivateAllowed = bTempDeactivateAllowed;
}
////////////////////////////////////////////////////////////
inline int CInput::GetNumActions() const
{
	return m_actions.GetSize();
}
////////////////////////////////////////////////////////////
inline void CInput::SetName(const CString &s)
{
	m_sName = s;
}
////////////////////////////////////////////////////////////
inline CActivation & CInput::GetActivation(int n) const
{
	return *m_actions[n];
}
////////////////////////////////////////////////////////////
inline BOOL CInput::IsUsedAsLockForTimer() const
{
	return (m_lockTimers.GetSize()!=0);
}
////////////////////////////////////////////////////////////
inline const CSystemTimeSpans& CInput::GetAlarmCallDeactivationSpans() const
{
	return m_AlarmCallDeactivationSpans;
}
////////////////////////////////////////////////////////////
inline BOOL CInput::IsMD() const
{
	return m_bMD;
}
////////////////////////////////////////////////////////////
inline BOOL CInput::IsSDI() const
{
	return m_bSDI;
}
////////////////////////////////////////////////////////////
inline CIPCInputServer* CInput::GetGroup()
{
	return m_pParent;
}
////////////////////////////////////////////////////////////
#endif
