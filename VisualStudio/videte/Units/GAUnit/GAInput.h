// GAInput.h
// Author : Uwe Freiwald

// Definition of CGAInput
// 

#ifndef _CGAInput_H_
#define _CGAInput_H_

class CGAUnitApp;

class CGAInput : public CIPCInput
{
	// construction / destruction
public:
	CGAInput(const char *shmName);
	~CGAInput();
	
	// member variables access
	WORD	GetGroupID() const {return m_wGroupID;};
	void	IndicateAlarm(DWORD dwMask, LPCSTR lpszAlarmString);
	void	UpdateAlarm(WORD wSubID, LPCSTR lpszAlarmString);

	// overridables
public:	
	// requests
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	

	virtual void OnRequestAlarmState(WORD wGroupID);

	virtual void OnRequestDisconnect();
	// private data
private:
	WORD		 m_wGroupID;
	DWORD		 m_OldMask;
};
#endif	// _CGAInput_H_