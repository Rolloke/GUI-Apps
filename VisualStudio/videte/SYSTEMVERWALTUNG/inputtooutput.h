/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputtooutput.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/inputtooutput.h $
// CHECKIN:		$Date: 27.04.06 20:21 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 27.04.06 20:05 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __InputToOutput_H_
#define __InputToOutput_H_

#define IO_NOSTATE			((DWORD)-1)
#define IO_OK				0
#define IO_ALARM			1

#define IO_DEFAULTPRIOR		3


class CInput;
class CInputToOutput;

//////////////////////////////////////////////////////////////////////
class CInputToOutput
{
	// construction / destruction
public:
	CInputToOutput(CInput* pInput);
	//	CInputToOutput();
	~CInputToOutput();

	// access:
public:
	inline DWORD  GetInputState();
	inline CSecID GetOutputOrMediaID();
	inline CSecID GetTimerID();
	inline CSecID GetProcessID();
	inline DWORD  GetPriority();
	inline CSecID GetArchiveID() const;
	inline CSecID GetNotificationID() const;
	inline CameraControlCmd GetPTZPreset() const;

	inline CInput* GetInput();	

	// settings
public:
	inline void SetInputState(DWORD dwState);
	inline void SetOutputID(CSecID idOutput);
	inline void SetTimerID(CSecID idTimer);
	inline void SetProcessID(CSecID idProcess);
	inline void SetPriority(DWORD dwPriority);
	inline void SetArchiveID(CSecID id);
	inline void SetNotificationID(CSecID id);
	inline void SetPTZPreset(CameraControlCmd ccc);

	// operations
	void DeleteInputToOutputFromInput();

public:
protected:
	DWORD	m_dwState;			// Eingangsstatus
	DWORD	m_dwPriority;		// Priority
	CSecID	m_idOutputOrMedia;			// Ausgangsnummer
	CSecID	m_idTimer;			// Timer, neu CSecID
	CSecID	m_idProcess;		// Prozess der gestartet werden soll
	CSecID	m_idArchive;		// Archiv
	CSecID	m_idNotification;
	CameraControlCmd m_PTZPreset;
	CInput*	m_pInput;
	BOOL	m_bMarkForDelete;

	friend class CInput;
};
////////////////////////////////////////////////////////////////////////////
inline CInput* CInputToOutput::GetInput()
{
	return m_pInput;
}
////////////////////////////////////////////////////////////////////////////
inline DWORD CInputToOutput::GetInputState()
{
	return m_dwState;
}
inline CSecID CInputToOutput::GetOutputOrMediaID()
{
	return m_idOutputOrMedia;
}
inline CSecID CInputToOutput::GetTimerID()
{
	return m_idTimer;
}
inline CSecID CInputToOutput::GetProcessID()
{
	return m_idProcess;
}
inline DWORD CInputToOutput::GetPriority()
{
	return m_dwPriority;
}
inline CameraControlCmd CInputToOutput::GetPTZPreset() const
{
	return m_PTZPreset;
}
inline void CInputToOutput::SetInputState(DWORD dwState)
{
	m_dwState = dwState;
}
inline void CInputToOutput::SetOutputID(CSecID idOutput)
{
	m_idOutputOrMedia = idOutput;
}
inline void CInputToOutput::SetTimerID(CSecID idTimer)
{
	m_idTimer = idTimer;
}
inline void CInputToOutput::SetProcessID(CSecID idProcess)
{
	m_idProcess = idProcess;
}
inline void CInputToOutput::SetPriority(DWORD dwPriority)
{
	m_dwPriority = dwPriority;
}
inline void CInputToOutput::SetPTZPreset(CameraControlCmd ccc)
{
	m_PTZPreset = ccc;
}
inline CSecID CInputToOutput::GetArchiveID() const
{
	return m_idArchive;
}

inline void CInputToOutput::SetArchiveID(CSecID id)
{
	m_idArchive = id;
}

inline CSecID CInputToOutput::GetNotificationID() const
{
	return m_idNotification;
}

inline void CInputToOutput::SetNotificationID(CSecID id)
{
	m_idNotification = id;
}

#endif
