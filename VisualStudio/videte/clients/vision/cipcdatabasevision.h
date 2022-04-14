// CIPCDataBaseVision.h: interface for the CIPCDataBaseVision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDATABASEVISION_H__7DA22864_AE9E_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_CIPCDATABASEVISION_H__7DA22864_AE9E_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CVisionDoc;

class CIPCDataBaseVision : public CIPCDatabaseClient  
{
public:
	CIPCDataBaseVision(CVisionDoc* pDoc , LPCTSTR shmName);
	~CIPCDataBaseVision();

public:
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual void OnIndicateRemoveFile(const CIPCArchivFileRecord &data) {}
	virtual void OnIndicateArchivFile(int iNumRecords, const CIPCArchivFileRecord data[]) {}
	virtual void OnIndicateFileClosed(const CIPCArchivFileRecord &data) {}
	virtual void OnIndicateQueryResult(DWORD dwUserID, const CIPCPictureRecord &pictData){}
	virtual void OnIndicateBackupFile(int iNumRecords, const CIPCBackupArchivFileRecord data[]) {}
	virtual void OnIndicateBackupFileClosed(const CIPCBackupArchivFileRecord &data) {}
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);

private:
	CVisionDoc* m_pDoc;
};

#endif // !defined(AFX_CIPCDATABASEVISION_H__7DA22864_AE9E_11D1_8C1B_00C095EC9EFA__INCLUDED_)
