/* GlobalReplace: CDVRDrive --> CIPCDrive */
// SVDoc.h : interface of the CSVDoc class
//
#ifndef _SVDOC_H
#define _SVDOC_H

/////////////////////////////////////////////////////////////////////////////
#include "OutputList.h"
#include "InputList.h"
#include "MediaList.h"
#include "ProcessList.h"
#include "SecTimer.h"

/////////////////////////////////////////////////////////////////////////////
class CSVView;

/////////////////////////////////////////////////////////////////////////////
class CSVDoc : public CDocument
{
protected: // create from serialization only
	CSVDoc();
	DECLARE_DYNCREATE(CSVDoc)

// Attributes
public:
	inline CHostArray*			GetHosts();
	inline CUserArray*			GetUsers();
	inline CPermissionArray*	GetPermissions();
	inline CInputList*			GetInputs();
	inline COutputList*			GetOutputs();
	inline CMediaList*			GetMedias();
	inline CArchivInfoArray*	GetArchivs();
	inline CProcessList*		GetProcesses();
	inline CSecTimerArray*		GetTimers();
	inline CIPCDrives*			GetDrives();
	inline CNotificationMessageArray*	GetNotifications();
	inline CIPCFields*			GetDatabaseFields();
	inline CDWordArray*			GetSDIControls();

	inline CString				GetCoCoIni();

#ifdef _UNICODE
	bool						UseBinaryFormat();
#endif
	// attributes
public:
	CWK_Profile&	GetProfile();
	CWK_Dongle*		GetDongle();
	DWORD			GetCommPortInfo();
	inline DWORD	GetFlags() const;
	inline BOOL		IsLocal() const;
	inline CString	GetHostName() const;
	
	CSVView*		GetView();

// Operations
public:
	void Connect(CSecID idHostID);
	void AddInitApp(const CString& sAppname);
	void InitServer();
	void InitApps();

	void LoadDatabaseFields();
	void LoadSDIControls();
	void CancelChangesRemoteDrives();
	void SaveChangesRemoteDrives();
	
	BOOL CheckActivationsWithProcess(CSecID id);
	void LoadDrives(BOOL bUpdate=FALSE);
	BOOL SaveChanges();

	CInputToOutput* CreateMDRecording(CSecID idCam);
	CSecID			CreateAlarmList();
	BOOL CreateDesigna(SDIControlType sdict,
					   CSecID idCam, DWORD dwBFR, DWORD dwTCC,
				       const CDWordArray& dwAlarms,const CDWordArray& dwXMLs);
	BOOL CreateSDIControls( SDIControlType sdict,
							CSecID idCam,
							DWORD dwBFR, 
							DWORD dwTCC,
							const CDWordArray& dwAlarms,
							const CDWordArray& dwXMLs,
							CSecID idRecording,
							CSecID idAlarmList,
							CSecID idSaveProcess);
	BOOL CreateDesignaAlarms(CSecID idCam, 
						   DWORD dwBFR, 
						   DWORD dwTCC,
						   const CDWordArray& dwAlarms,
						   const CString& sSection,
						   SDIControlType ct,
						   int iPortNr,
						   CSecID idRecording,
						   CSecID idAlarmList,
						   CSecID idSaveProcess,
						   BOOL bNew);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void OnIdle();
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSVDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void Load();
	void ReLoadLocal();
// Generated message map functions
protected:
	//{{AFX_MSG(CSVDoc)
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnDisconnect();
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadRegistry();
	afx_msg void OnUpdateFileLoadRegistry(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAndClose();
	afx_msg void OnUpdateFileSaveAndClose(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL CopyTrivialListToRegistry(const CString &sFile);

private:
	CWK_Profile*			  m_pProfile;
	CWK_Dongle*				  m_pDongle;
	DWORD					  m_dwFlags;
	CSecID					  m_idHost;
	CString					  m_sHost;

	CHostArray				  m_Hosts;
	CUserArray				  m_Users;
	CPermissionArray		  m_Permissions;
	CInputList				  m_Inputs;
	COutputList				  m_Outputs;
	CMediaList				  m_Medias;
	CArchivInfoArray		  m_Archivs;
	CProcessList			  m_Processes;
	CSecTimerArray			  m_Timers;
	CNotificationMessageArray m_Notifications;
	CIPCDrives				  m_Drives;
	CIPCDrives				  m_RemoteDrives;
	CIPCFields				  m_DatabaseFields;
	CDWordArray				  m_SDIControls;

	CString					  m_sCoCoIni;
	CString					  m_sRegistryFile;
	UINT					  m_nRemoteCodePage;
	BOOL					  m_bIsLoadedFile;

	CStringArray	m_saInitApps;
public:
};
/////////////////////////////////////////////////////////////////////////////
inline CHostArray* CSVDoc::GetHosts()		 
{
	return &m_Hosts;
}
/////////////////////////////////////////////////////////////////////////////
inline CUserArray* CSVDoc::GetUsers()		 
{
	return &m_Users;
}
/////////////////////////////////////////////////////////////////////////////
inline CPermissionArray* CSVDoc::GetPermissions() 
{
	return &m_Permissions;
}
/////////////////////////////////////////////////////////////////////////////
inline CInputList* CSVDoc::GetInputs()		 
{
	return &m_Inputs;
}
/////////////////////////////////////////////////////////////////////////////
inline COutputList* CSVDoc::GetOutputs()	 
{
	return &m_Outputs;
}
/////////////////////////////////////////////////////////////////////////////
inline CMediaList* CSVDoc::GetMedias()	 
{
	return &m_Medias;
}
/////////////////////////////////////////////////////////////////////////////
inline CArchivInfoArray* CSVDoc::GetArchivs()	 
{
	return &m_Archivs;
}
/////////////////////////////////////////////////////////////////////////////
inline CProcessList* CSVDoc::GetProcesses()
{
	return &m_Processes;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecTimerArray * CSVDoc::GetTimers()
{
	return &m_Timers;
}
/////////////////////////////////////////////////////////////////////////////
inline CNotificationMessageArray* CSVDoc::GetNotifications()
{
	return &m_Notifications;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCFields* CSVDoc::GetDatabaseFields()
{
	return &m_DatabaseFields;
}
/////////////////////////////////////////////////////////////////////////////
inline CDWordArray*	CSVDoc::GetSDIControls()
{
	return &m_SDIControls;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCDrives* CSVDoc::GetDrives()
{
	return &m_Drives;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CSVDoc::GetCoCoIni()
{
	return m_sCoCoIni;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSVDoc::GetFlags() const
{
	return m_dwFlags;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CSVDoc::IsLocal() const
{
	return m_idHost == SECID_LOCAL_HOST;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CSVDoc::GetHostName() const
{
	return m_sHost;
}
/////////////////////////////////////////////////////////////////////////////

#endif
/////////////////////////////////////////////////////////////////////////////
