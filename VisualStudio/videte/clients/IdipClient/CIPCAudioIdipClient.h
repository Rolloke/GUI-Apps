// CIPCAudioIdipClient.h: interface for the CIPCAudioIdipClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCAudioIdipClient_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_)
#define AFX_CIPCAudioIdipClient_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CIPCMediaClient.h>

class CServer;
class CIPCMediaRecordArray;
class CIPCAudioIdipClientListCS;
class CIdipClientDoc;
class CWndPlay;

class CIPCAudioIdipClient : public CIPCMediaClient , public CObject
{
	DECLARE_DYNAMIC(CIPCAudioIdipClient)
public:
	// Construction / Destruction
	CIPCAudioIdipClient(LPCTSTR shmName, CServer*pServer, CSecID id, WORD wCodePage);
	CIPCAudioIdipClient(LPCTSTR shmName, CIdipClientDoc*pServer, BOOL bAsMaster=FALSE);
	virtual ~CIPCAudioIdipClient();
	// Connection
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	// Info, Values, States
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmHardware(CSecID secID, int errorCode, DWORD dwF1, DWORD dwF2, const CIPCExtraMemory *pEM);
	virtual void OnConfirmValues(CSecID, DWORD, DWORD, DWORD, const CIPCExtraMemory*);
	virtual void OnConfirmInfoMedia(WORD, int, int, const CIPCMediaRecordArray&);
	// Data Exchange
	virtual void OnIndicateMediaData(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwUserData);
	virtual void OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);
	void DoRequestMediaDecoding(const CIPCMediaSampleRecord& media);
	void DoRequestStopMediaDecoding(CSecID);
	void DoRequestStopMediaDecoding();

	// called, when a record is added to the array
	virtual void OnAddRecord(const CIPCMediaRecord& record);
	// called, when a record is updated to the array
	virtual void OnUpdateRecord(const CIPCMediaRecord& record);
	// called, when a record is deleted from the array
	virtual void OnDeleteRecord(const CIPCMediaRecord& record);

	// Attributes
	void RequestConfig(CSecID secID);
	void SetInputChannel(CSecID secID, int);
	void SetStereoChannel(CSecID secID, int);
	void SetInputSensitivity(CSecID secID, int);
	void SetOutputVolume(CSecID secID, int);
	void SetThreshold(CSecID secID, int);

	void SetNotifyWindow(CWnd*pWnd, WORD wMsg=WM_NOTIFY_WINDOW) { m_pWndNotify = pWnd; m_wNotifyMessage = wMsg; };
	void SetServerID(WORD wServerID) {m_wServerID = wServerID;};

	int					GetInputChannel()		const {return m_nInputChannel;};
	int					GetStereoChannel()		const {return m_nStereoChannel;};
	int					GetInputSensitivity()	const {return m_nInputSensitivity;};
	int					GetThreshold()			const {return m_nThreshold;};
	int					GetOutputVolume()		const {return m_nOutputVolume;};
	const CStringArray*	GetInputChannels()		const {return (const CStringArray*)m_psaInputChannels;};
	CString             GetNamedInput(int nIndex, DWORD &dwChannel) const;
	int					GetNamedInputs()		const {return m_dwaNamedInputs.GetSize();};
	CSecID				GetSecID()				const {return m_SecID;};
	WORD				GetServerID()			const {return m_wServerID;};
	CCriticalSection*   GetCSAudio()				  {return &m_csAudio;};
	CSecID				GetDefaultOutputID()	const {return m_idDefaultOutput;};
	CSecID				GetDefaultInputID()		const {return m_idDefaultInput;};
	WORD				GetNotifyMsg()			const {return m_wNotifyMessage;};
	const CIPCAudioIdipClient* GetDestination(int) const;
	BOOL				HasDestinations();
	BOOL				IsDestination()	{ return m_pOwnerOfMe != NULL;};
	CWndPlay*			GetWndPlayActive() {return m_pWndPlayActive;}
	BOOL				DoNotSendSamples();
	BOOL				CanDecode(); 
	static CString		GetCompressionType(CIPCMediaSampleRecord*);
	
	// Operations
	void StartRecord(CSecID secID, bool bLocal, DWORD dwUserData);
	void StopRecord(CSecID secID);
	void StopPlay(CSecID secID);
	
	void AddDestination(CIPCAudioIdipClient*pDest, CSecID secID);
	void RemoveDestinations();
	void RemoveDestination(CIPCAudioIdipClient*);
	void ResetSentSamples()            { m_nSentSamples = 0; };
	void SetWndPlayActive(CWndPlay*pActive, CSecID idDestination);

private:
	void InitClassMembers();
	// no implementation, to avoid implicit generation!
	CIPCAudioIdipClient(const CIPCAudioIdipClient& src);		
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCAudioIdipClient& src);	

	CIdipClientDoc*	m_pDocument;
	WORD			m_wServerID;
	WORD			m_wNotifyMessage;
	CWnd*			m_pWndNotify;
	CSecID			m_SecID;
	CSecID			m_RecordSecID;
	CSecID			m_idDefaultInput;
	CSecID			m_idDefaultOutput;
	CWndPlay*		m_pWndPlayActive;
	
	int				m_nOutputVolume;
	int				m_nInputSensitivity;
	int				m_nThreshold;
	int				m_nInputChannel;
	int				m_nStereoChannel;
	int             m_nSentSamples;

	CStringArray*	m_psaInputChannels;
	CStringArray	m_saNamedInputs;
	CDWordArray		m_dwaNamedInputs;
	CCriticalSection	m_csAudio;

	CIPCAudioIdipClient*     m_pOwnerOfMe;
	CIPCAudioIdipClientListCS* m_pDestinations;

protected:
	void UpdateFlags(CSecID, DWORD);
};

//////////////////////////////////////////////////////////////////////////

class CAudioUpdateHint : public CObject
{
	DECLARE_DYNAMIC(CAudioUpdateHint)	
public:
	CAudioUpdateHint(CIPCAudioIdipClient*, DWORD dwFlags);
	CIPCAudioIdipClient*GetAudio() { return m_pAudio; };
	DWORD GetFlags() { return m_dwFlags; };
	
private:
	CAudioUpdateHint();
	CIPCAudioIdipClient*m_pAudio;
	DWORD m_dwFlags;
};

//////////////////////////////////////////////////////////////////////////

typedef CIPCAudioIdipClient *CIPCAudioIdipClientPtr;
WK_PTR_ARRAY_CS(CIPCAudioIdipClientList, CIPCAudioIdipClientPtr, CIPCAudioIdipClientListCS);

#endif // !defined(AFX_CIPCAudioIdipClient_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_)
