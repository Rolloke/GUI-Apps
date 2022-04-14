// IPCControlAudioUnit.h: interface for the CIPCControlAudioUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCCONTROLAUDIOUNIT_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_)
#define AFX_IPCCONTROLAUDIOUNIT_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAudioPage;

class CIPCControlAudioUnit : public CIPCMedia  
{
	friend class CAudioPage;
public:
	CIPCControlAudioUnit(LPCTSTR, BOOL, CAudioPage*);
	virtual ~CIPCControlAudioUnit();

	virtual void OnConfirmHardware(CSecID secID, int errorCode, DWORD dwF1, DWORD dwF2, const CIPCExtraMemory *pEM);
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmValues(CSecID, DWORD, DWORD, DWORD, const CIPCExtraMemory*);
	virtual void OnRequestDisconnect();
	virtual void OnConfirmSetGroupID(WORD wGroup);

	CSecID GetSecID() {return m_SecID;}

private:
	// no implementation, to avoid implicit generation!
	CIPCControlAudioUnit(const CIPCControlAudioUnit& src);		
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCControlAudioUnit& src);	

	CAudioPage  *m_pDlg;
	CSecID         m_SecID;
};

#endif // !defined(AFX_IPCCONTROLAUDIOUNIT_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_)
