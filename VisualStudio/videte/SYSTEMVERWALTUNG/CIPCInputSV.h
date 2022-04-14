// CIPCInputSV.h: interface for the CIPCInputSV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINPUTSV_H__ABB398C3_B45B_11D2_B565_004005A19028__INCLUDED_)
#define AFX_CIPCINPUTSV_H__ABB398C3_B45B_11D2_B565_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define INPUT_CPF_USE_STRING_FORMAT		0x10000000
#define INPUT_CPF_USES_GLOBAL_FONTS		0x20000000
#define INPUT_CPF_GLOBAL_FONTS_CHANGED	0x40000000

#define INPUT_CPF_KB_LAYOUT_CREATED		0x01000000
#define INPUT_CPF_KB_LAYOUT_CHANGED		0x02000000

class CIPCInputSV : public CIPCInput  
{
public:
	CIPCInputSV(const CString& sCommand,
				BOOL bReceiving,
				LPCTSTR shmName
#ifdef _UNICODE
				,WORD wCodePage
#endif
				);

	virtual ~CIPCInputSV();


	// overrides
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual void OnConfirmGetFile(int iDestination,
								 const CString &sFileName,
								 const void *pData,
								 DWORD dwDataLen, DWORD dwCodePage);
	virtual void OnConfirmFileUpdate(const CString &sFileName);

	virtual void OnConfirmHardware(WORD wGroupID,int iErrorCode);	// dummy
	virtual void OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, 
								int iNumRecords, const CIPCInputRecord records[]);	// dummy
	virtual void OnConfirmAlarmState(WORD wGroupID,DWORD inputMask);	// dummy

	virtual	void OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2);	// dummy

public:
	CEvent	m_eventRegistry;
	CString m_sRegistry;
	CString m_sCoCoISAini;
	CString m_sCommand;
	UINT	m_nRemoteCodePage;

private:
	BOOL    m_bReceiving;
};

#endif // !defined(AFX_CIPCINPUTSV_H__ABB398C3_B45B_11D2_B565_004005A19028__INCLUDED_)
