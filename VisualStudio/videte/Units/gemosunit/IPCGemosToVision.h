// IPCGemosToVision.h: interface for the CIPCGemosToVision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCGEMOSTOVISION_H__F619054D_56F5_11D3_998C_004005A19028__INCLUDED_)
#define AFX_IPCGEMOSTOVISION_H__F619054D_56F5_11D3_998C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum{On, Off, Enabled, Disabled}GemosInputState;

class CIPCGemosToVision : public CIPC  
{
	// construction / destruction
public:
	CIPCGemosToVision(LPCTSTR szShmName);
	virtual ~CIPCGemosToVision();

	// Overrides
public:
	virtual void OnRequestDisconnect();
	virtual void OnRequestGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   DWORD dwServerData);
	virtual void OnConfirmGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);
	virtual void OnConfirmSetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);
};

#endif // !defined(AFX_IPCGEMOSTOVISION_H__F619054D_56F5_11D3_998C_004005A19028__INCLUDED_)
