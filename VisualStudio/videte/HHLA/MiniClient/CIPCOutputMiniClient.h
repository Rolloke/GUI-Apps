// CIPCOutputMiniClient.h: interface for the CIPCOutputMiniClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCOUTPUTMINICLIENT_H__AEE943A5_8A89_11D2_8CA3_004005A11E32__INCLUDED_)
#define AFX_CIPCOUTPUTMINICLIENT_H__AEE943A5_8A89_11D2_8CA3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CipcOutputClient.h"

class CMainFrame;
class CIPCOutputMiniClient : public CIPCOutputClient  
{
public:
	CIPCOutputMiniClient(CMainFrame *pMainFrame, LPCTSTR shmName);
	virtual ~CIPCOutputMiniClient();
	BOOL RequestPictures(WORD wSource);
	BOOL SetCameraParameter(WORD wSource, int nBrightness, int nContrast, int nSaturation);
	BOOL RequestCameraParameter(WORD wSource);

protected:

	virtual void OnAddRecord(const CIPCOutputRecord &pRec);
	virtual void OnUpdateRecord(const CIPCOutputRecord &pRec);
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[]);
	virtual void OnDeleteRecord(const CIPCOutputRecord &pRec);
	virtual void OnRequestDisconnect();

	// picture data confirmations
	virtual void OnConfirmJpegEncoding(	const CIPCPictureRecord &pict,DWORD dwUserID, CSecID idArchive);

	virtual void OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);
	virtual void OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);

private:
	CMainFrame	*m_pMainFrame;	
	int			m_nNumberOfOutputs;
};

#endif // !defined(AFX_CIPCOUTPUTMINICLIENT_H__AEE943A5_8A89_11D2_8CA3_004005A11E32__INCLUDED_)
