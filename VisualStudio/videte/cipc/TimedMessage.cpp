/* GlobalReplace: @[mM][dD] --> Function: */
/* GlobalReplace: @[aA][lL][sS][oO] -->  */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: TimedMessage.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/TimedMessage.cpp $
// CHECKIN:		$Date: 26.09.03 15:51 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 26.09.03 15:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"

#include "TimedMessage.h"
#include "Cipc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
{CTimedMessage Overview|Overview,CTimedMessage}
 {members|CTimedMessage}

CTimedMessage allows to restrict trace messaages to certain
times. A timestamp is remembered and the next message is
only printed if a spcified delta time has elapsed.

 The time is checked AND stored internaly in
{CTimedMessage::OmitMessage}, 
{CTimedMessage::OmitMessageWithData}.
*/

/*
  {overview|Overview,CTimedMessage}
*/

/*Function: allows to reset the internal state, the next message is
printed independent from the last message.
*/
void CTimedMessage::Reset()
{
	m_bAlreadyHadMessage = FALSE;
}

/*Function: allows to specify the the time between two messages 
(in mili seconds). Might need a call to {CTimedMessage::Reset}
*/
void CTimedMessage::SetDelta(DWORD dwDelta)
{
	m_dwDelta = dwDelta;
}

/*Function: 
Calls WK_TRACE under the following conditions:
 it is the first message
 if the last message was before more than the specified miliseconds

 To perform the check a call to @CW{GetTickCount()} is made.

 {CTimedMessage::StatMessage}
*/
void __cdecl CTimedMessage::Message(LPCTSTR lpszFormat, ...)
{
	if ( OmitMessage()==FALSE ) {
		m_tickLastMessage = GetTickCount();
		m_bAlreadyHadMessage = TRUE;

		const DWORD MAX_MESSAGE_LEN=2000;

		_TCHAR *szString = new _TCHAR[MAX_MESSAGE_LEN];
		szString[0]=0;

		va_list args;
		va_start(args, lpszFormat);

		_vsntprintf(szString, MAX_MESSAGE_LEN-1, lpszFormat, args);
		WK_TRACE(LPCTSTR(szString));
		WK_DELETE_ARRAY(szString);
	}
}
/*Function: See {CTimedMessage::Message}.
*/


/*Function: 
Calls equivalent WK_STAT_LOG, see {CTimedMessage::Message}.
*/
void CTimedMessage::StatMessage(LPCTSTR szTopic, int iValue, LPCTSTR sName)
{
	if ( OmitMessage()==FALSE )
	{
		m_tickLastMessage = GetTickCount();
		m_bAlreadyHadMessage = TRUE;
		WK_STAT_LOG(szTopic, iValue, sName);
	}
}
/*Function:
Sets the internal stored userData. Does NOT perform
{CTimedMessage::Reset}.

 {CTimedMessage::OmitMessageWithData}
*/
void CTimedMessage::SetUserData(DWORD dwUserData)
{
	m_dwUserData=dwUserData;
}
