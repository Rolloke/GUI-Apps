// IPCControlAudioUnit.h: interface for the CIPCAudioRecherche class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCCONTROLAUDIOUNIT_H__C4E9D784_0279_47DC_B038_2CD865993DC6__INCLUDED_)
#define AFX_IPCCONTROLAUDIOUNIT_H__C4E9D784_0279_47DC_B038_2CD865993DC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCMediaClient.h"
#include "Server.h"
#include "RechercheDoc.h"

class CRechercheDoc;
class CDisplayWindow;

class CIPCAudioRecherche : public CIPCMediaClient  
{
public:

	CIPCAudioRecherche(LPCTSTR shmName, CRechercheDoc* pDoc, BOOL bAsMaster=FALSE);
	virtual ~CIPCAudioRecherche();

	// called, when a record is added to the array
	virtual void OnAddRecord(const CIPCMediaRecord& record);
	// called, when a record is updated to the array
	virtual void OnUpdateRecord(const CIPCMediaRecord& record);
	// called, when a record is deleted from the array
	virtual void OnDeleteRecord(const CIPCMediaRecord& record);
	
	//Connection
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();

	// Info, Values, States
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmHardware(CSecID secID, int errorCode, DWORD dwF1, DWORD dwF2, const CIPCExtraMemory *pEM);
	virtual void OnConfirmValues(CSecID, DWORD dwCmd, DWORD dwID, DWORD dwValue  , const CIPCExtraMemory*pData);
	virtual void OnConfirmInfoMedia(WORD, int, int, const CIPCMediaRecordArray&);
	BOOL DoNotSendSamples();

	// Requests
	void DoRequestMediaDecoding(const CIPCMediaSampleRecord& media);
	void DoRequestStopMediaDecoding();

	// Operations
	void ResetSentSamples() {m_nSentSamples = 0;}
	void SetActiveDisplayWindow(CDisplayWindow*pActive);

	// Parameters
	int             GetSentSamples() {return m_nSentSamples;}
	CDisplayWindow* GetActiveDisplayWindow() {return m_pActiveDisplayWindow;}
	BOOL            CanDecode(); 
	CSecID          GetDefaultOutputID();

	static CString GetCompressionType(CIPCMediaSampleRecord*);

private:
	int					m_nSentSamples;
	bool              m_bDecoding;
	CIPCMediaRecord*  m_pDefaultOutput;
	CRechercheDoc*		m_pRechercheDoc;
	CDisplayWindow*   m_pActiveDisplayWindow;
	static CDllModule gm_ComDSDll;
protected:
	void UpdateFlags(DWORD dwFlags);
};

#endif // !defined(AFX_IPCCONTROLAUDIOUNIT_H__C4E9D784_0279_47DC_B038_2CD865993DC6__INCLUDED_)
