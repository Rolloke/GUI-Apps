// IPCControlAudioUnit.h: interface for the CIPCAudioVision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCAUDIOVISION_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_)
#define AFX_CIPCAUDIOVISION_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CIPCMediaClient.h>
#define WM_NOTIFY_WINDOW  (WM_USER + 1)
#define WM_NOTIFY_WINDOWO (WM_USER + 2)
#define WM_NOTIFY_WINDOWI (WM_USER + 3)

class CServer;
class CIPCMediaRecordArray;
class CIPCAudioVisionList;
class CVisionDoc;

class CIPCAudioVision : public CIPCMediaClient  
{
	friend class CAudioPage;
public:
	// Construction / Destruction
	CIPCAudioVision(LPCTSTR shmName, CServer*pServer, CSecID id);
	CIPCAudioVision(LPCTSTR shmName, CVisionDoc*pServer, BOOL bAsMaster=FALSE);
	virtual ~CIPCAudioVision();
	// Connection
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	// Info, Values, States
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmHardware(CSecID secID, int errorCode, DWORD dwF1, DWORD dwF2, const CIPCExtraMemory *pEM);
	virtual void OnConfirmValues(CSecID, DWORD, DWORD, DWORD, const CIPCExtraMemory*);
	virtual void OnConfirmInfoMedia(WORD, int, int, const CIPCMediaRecordArray&);
	// Data Exchange
	virtual void OnIndicateMediaData(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwUserData);
	virtual void OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);
	void DoRequestMediaDecoding(const CIPCMediaSampleRecord& media);

	// called, when a record is added to the array
	virtual void OnAddRecord(const CIPCMediaRecord& record);
	// called, when a record is updated to the array
	virtual void OnUpdateRecord(const CIPCMediaRecord& record);
	// called, when a record is deleted from the array
	virtual void OnDeleteRecord(const CIPCMediaRecord& record);

	void DoRequestStopMediaDecoding(CSecID);
	void RequestConfig(CSecID secID);

	void SetInputChannel(CSecID secID, int);
	void SetStereoChannel(CSecID secID, int);
	void SetInputSensitivity(CSecID secID, int);
	void SetOutputVolume(CSecID secID, int);
	void SetThreshold(CSecID secID, int);
	
	void StartRecord(CSecID secID, bool bLocal, DWORD dwUserData);
	void StopRecord(CSecID secID);
	void StopPlay(CSecID secID);
	
	void SetNotifyWindow(CWnd*pWnd, int nMsg=WM_NOTIFY_WINDOW) { m_pWndNotify = pWnd;m_nNotifyMessage = nMsg; };
	void SetServer(const CServer*pS) {m_pServer = (CServer*)pS;};
		
	void AddDestination(CIPCAudioVision*pDest, CSecID secID);
	const CIPCAudioVision* GetDestination(int) const;
	BOOL HasDestinations();
	BOOL IsDestination()	{ return m_pOwnerOfMe != NULL;};
	void RemoveDestinations();
	void RemoveDestination(CIPCAudioVision*);

	int					GetInputChannel()		const {return m_nInputChannel;};
	int					GetStereoChannel()		const {return m_nStereoChannel;};
	int					GetInputSensitivity()	const {return m_nInputSensitivity;};
	int					GetThreshold()			const {return m_nThreshold;};
	int					GetOutputVolume()		const {return m_nOutputVolume;};
	const CStringArray*	GetInputChannels()		const {return (const CStringArray*)m_psaInputChannels;};
	CString             GetNamedInput(int nIndex, DWORD &dwChannel) const;
	int					GetNamedInputs()		const {return m_dwaNamedInputs.GetSize();};
	CSecID				GetSecID()				const {return m_SecID;};
	CServer*			GetServer()				const {return m_pServer;};
	CCriticalSection*   GetCSAudio()				  {return &m_csAudio;};
	CSecID				GetDefaultOutputID()	const {return m_idDefaultOutput;};
	CSecID				GetDefaultInputID()		const {return m_idDefaultInput;};
	int					GetNotifyMsg()			const {return m_nNotifyMessage;};
	void				ResetSentSamples()            { m_nSentSamples = 0; };

private:
	void InitClassMembers();
	// no implementation, to avoid implicit generation!
	CIPCAudioVision(const CIPCAudioVision& src);		
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCAudioVision& src);	

	CServer*		m_pServer;
	CVisionDoc*		m_pDocument;
	CSecID			m_SecID;
	CSecID			m_RecordSecID;
	CWnd*			m_pWndNotify;
	int				m_nNotifyMessage;
	CSecID			m_idDefaultInput;
	CSecID			m_idDefaultOutput;
	
	int				m_nOutputVolume;
	int				m_nInputSensitivity;
	int				m_nThreshold;
	int				m_nInputChannel;
	int				m_nStereoChannel;
	int             m_nSentSamples;

	CStringArray*	m_psaInputChannels;
	CStringArray	m_saNamedInputs;
	CDWordArray		m_dwaNamedInputs;
	CIPCMediaRecordArray*m_pMediaRecordArray;
	CCriticalSection	m_csAudio;

	CIPCAudioVision*     m_pOwnerOfMe;
	CIPCAudioVisionList* m_pDestinations;
protected:
	void UpdateFlags(CSecID, DWORD);
};

typedef CIPCAudioVision *CIPCAudioVisionPtr;
WK_PTR_ARRAY_CS(CIPCAudioVisionList, CIPCAudioVisionPtr, AudioControl);

#endif // !defined(AFX_CIPCAUDIOVISION_H__9CB656E2_92CC_426B_9FC2_00EA400772A4__INCLUDED_)
