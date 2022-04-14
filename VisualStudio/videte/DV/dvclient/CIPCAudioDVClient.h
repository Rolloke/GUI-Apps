// IPCControlAudioUnit.h: interface for the CIPCAudioDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCAUDIODVCLIENT_H__C4E9D784_0279_47DC_B038_2CD865993DC6__INCLUDED_)
#define AFX_CIPCAUDIODVCLIENT_H__C4E9D784_0279_47DC_B038_2CD865993DC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCMediaClient.h"
#include "Server.h"

class CPlayWindow;

#define INPUT_SENSITIVITY_AUTO 101
#define ERROR_VALUE -1

class CIPCAudioDVClient : public CIPCMediaClient  
{
public:

	CIPCAudioDVClient(LPCTSTR shmName, CServer* pServer, BOOL bAsMaster=FALSE);
	virtual ~CIPCAudioDVClient();

	// called, when a record is added to the array
	virtual void OnAddRecord(const CIPCMediaRecord& record);
	// called, when a record is updated to the array
	virtual void OnUpdateRecord(const CIPCMediaRecord& record);
	// called, when a record is deleted from the array
	virtual void OnDeleteRecord(const CIPCMediaRecord& record);
	
	// Connection
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();

	// Info, Values, States
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmHardware(CSecID secID, int errorCode, DWORD dwF1, DWORD dwF2, const CIPCExtraMemory *pEM);
	virtual void OnConfirmValues(CSecID, DWORD dwCmd, DWORD dwID, DWORD dwValue  , const CIPCExtraMemory*pData);
	virtual void OnConfirmInfoMedia(WORD, int, int, const CIPCMediaRecordArray&);
	virtual void OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwData);
	virtual void OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwData);

	// Requests
	void DoRequestMediaDecoding(const CIPCMediaSampleRecord& media);
	void DoRequestStopMediaDecoding();

	BOOL SetInputSensitivity(int nValue);
	BOOL SetInputThreshold(int nValue);
	BOOL SetOutputVolume(int nValue);

	BOOL SetCameraNumber(int nCam);
	BOOL SetRecordingMode(int nType);

	BOOL SwitchInput(LPCTSTR str);
	BOOL SwitchOutput(LPCTSTR str);

	// Operations
	void ResetSentSamples() {m_nSentSamples = 0;}
	void SetActiveDisplayWindow(CPlayWindow*pActive);

	// Parameters
	int             GetSentSamples() {return m_nSentSamples;}
	CPlayWindow   * GetActiveDisplayWindow() {return m_pActiveDisplayWindow;}
	BOOL            CanDecode(); 
	BOOL            DoNotSendSamples();
	CSecID          GetDefaultOutputID();
	CSecID          GetDefaultInputID();

	int             GetInputSensitivity() {return m_nInputSensitivity;}
	int             GetInputThreshold()   {return m_nThreshold;}
	int             GetOutputVolume()     {return m_nOutputVolume;}

	int             GetRecordingMode()    {return m_nRecordingMode;}
	int             GetCameraNumber()     {return m_nCameraNumber;}

	BOOL            IsInputOn()           {return m_bInputOn  == TRUE;}
	BOOL            IsOutputOn()          {return m_bOutputOn == TRUE;}

	static CString GetCompressionType(CIPCMediaSampleRecord*);

private:
	int					m_nSentSamples;
	bool              m_bDecoding;
	CIPCMediaRecord*  m_pDefaultOutput;
	CIPCMediaRecord*  m_pDefaultInput;
	CPlayWindow*      m_pActiveDisplayWindow;
	static CDllModule gm_ComDSDll;

	int				m_nOutputVolume;
	int				m_nInputSensitivity;
	int				m_nThreshold;

	int            m_nRecordingMode;
	int            m_nCameraNumber;
	
	BOOL           m_bInputOn;
	BOOL           m_bOutputOn;

	CServer*	m_pServer;
	CEvent		m_eRequest;
	static const TCHAR *gm_szType[]; 

protected:
	void UpdateFlags(DWORD dwFlags);
};

#endif // !defined(AFX_CIPCAUDIODVCLIENT_H__C4E9D784_0279_47DC_B038_2CD865993DC6__INCLUDED_)
