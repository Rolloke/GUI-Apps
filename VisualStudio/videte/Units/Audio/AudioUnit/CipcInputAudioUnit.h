/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: CipcInputAudioUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Audio/AudioUnit/CipcInputAudioUnit.h $
// CHECKIN:		$Date: 20.02.04 16:17 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 20.02.04 15:39 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputAudioUnit_H_
#define _CIPCInputAudioUnit_H_

#include "CipcInput.h"

class CAudioUnitDlg;

class CIPCInputAudioUnit : public CIPCInput
{
friend class CICPI7000Module;
public:
	CIPCInputAudioUnit(CAudioUnitDlg* pDlg, LPCTSTR shmName);
	virtual ~CIPCInputAudioUnit();
	
	BOOL IsValid();

   // RequestXXX
 	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	virtual void OnRequestDisconnect();
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

	void SetAudioAlarm(BOOL);
	bool IsAudioDetectorActive() { return m_dwFree & 0x00000001 ? true : false; };
	bool DoReset() { return m_bReset;}
	void ConfirmReset();

protected:

private:
	CAudioUnitDlg		*m_pDlg;
	WORD					m_wGroupID;
	DWORD             m_dwEdgeMask;
	DWORD             m_dwFree;
	DWORD             m_dwAlarm;
	bool              m_bReset;
};

#endif	// _CIPCInputAudioUnit_H_
