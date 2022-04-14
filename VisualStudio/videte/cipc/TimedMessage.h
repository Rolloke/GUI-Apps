/* GlobalReplace: @[mM][dD] --> Function: */
/* GlobalReplace: @[aA][lL][sS][oO] -->  */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: TimedMessage.h $
// ARCHIVE:		$Archive: /Project/CIPC/TimedMessage.h $
// CHECKIN:		$Date: 26.09.03 15:51 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 26.09.03 15:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _CTimedMessage_H
#define _CTimedMessage_H


class AFX_EXT_CLASS CTimedMessage
{
public:
	inline CTimedMessage(DWORD dwDeltaMiliSeconds=5000);
	//
	inline void FakeMessage();
	// output:
	void __cdecl Message(LPCTSTR lpszFormat, ...);
	//
	void StatMessage(LPCTSTR szTopic,int iValue, LPCTSTR sName);
	// control:
	void SetDelta(DWORD dwDeltaMiliSeconds);
	void SetUserData(DWORD dwUserData);
	void Reset();
	// check:
	inline BOOL OmitMessage();
	inline BOOL OmitMessageWithData(DWORD dwUserData);
	// access:
	inline BOOL AlreadyHadMessage() const;
	inline DWORD GetDelta() const;
	inline DWORD GetLastTick() const;
	inline int	GetNumOmitted() const;
	inline DWORD GetUserData() const;
private:
	DWORD	m_dwDelta;
	DWORD	m_tickLastMessage;
	BOOL	m_bAlreadyHadMessage;
	DWORD	m_dwUserData;
};

/*
USAGE:
	if (bOkay==FALSE) { // some error condition
		m_tmInvalidHost.Message("some error\n");	// timed message
	}
*/

/*Function: constructor with default delta of 5 seconds.
*/
inline CTimedMessage::CTimedMessage(DWORD dwDelta)
{
	m_dwDelta = dwDelta;
	m_tickLastMessage = 0;
	m_bAlreadyHadMessage = FALSE;
	m_dwUserData = 0;
}
/*Function: returns TRUE, is the message should be suppressed. 
If not the the current time (GetTickCount()) is stored
internaly as the time for the last message.
*/
inline BOOL CTimedMessage::OmitMessage()
{
	if (m_bAlreadyHadMessage && m_dwDelta!=0) 
	{
		if (GetTimeSpan(m_tickLastMessage) < m_dwDelta) 
		{
			// less than dwDelta since last message
			// omit this message
			return TRUE;	// EXIT
		}
	}
	m_tickLastMessage = GetTickCount();
	m_bAlreadyHadMessage=TRUE;
	return FALSE;
}
/*Function:
Like {CTimedMessage::OmitMessage} but a with @CW{dwUserData}
as additional condition. That is, if @CW{dwUserData} changed from
the last message, the message is printed, igrnoring the time limit

 Automatically calls {CTimedMessage::SetUserData}

 {CTimedMessage::OmitMessage},
{CTimedMessage::SetUserData}
*/
inline BOOL CTimedMessage::OmitMessageWithData(DWORD dwUserData)
{
	if (m_bAlreadyHadMessage && m_dwDelta!=0
		&& dwUserData==m_dwUserData) 
	{
		if (GetTimeSpan(m_tickLastMessage) < m_dwDelta) {
			// less than dwDelta since last message
			// omit this message
			return TRUE;	// EXIT
		} else {
			m_tickLastMessage = GetTickCount();
			m_bAlreadyHadMessage=TRUE;
			return FALSE;
		}
	} else {
		// automagically update userdata and m_bAlreadyHadMessage
		m_dwUserData = dwUserData;
		m_tickLastMessage = GetTickCount();
		m_bAlreadyHadMessage=TRUE;
		return FALSE;
	}
}


/*Function: nomes est omen*/
inline DWORD CTimedMessage::GetDelta() const
{
	return m_dwDelta;
}
/*Function: nomes est omen*/
inline DWORD CTimedMessage::GetLastTick() const
{
	return m_tickLastMessage;
}
/*Function: nomes est omen*/
inline BOOL CTimedMessage::AlreadyHadMessage() const
{
	return m_bAlreadyHadMessage;
}
/*Function: nomes est omen*/
inline DWORD CTimedMessage::GetUserData() const
{
	return m_dwUserData;
}

inline void CTimedMessage::FakeMessage()
{
	m_bAlreadyHadMessage=TRUE;
}

#endif
