/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControlClientSide.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcServerControlClientSide.h $
// CHECKIN:		$Date: 23.03.04 15:15 $
// VERSION:		$Revision: 44 $
// LAST CHANGE:	$Modtime: 22.03.04 11:01 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _CIPCServerControlClientSide_H_
#define _CIPCServerControlClientSide_H_

#include "CipcServerControl.h"
#include "CipcFetchResult.h"

class CIPCFetch;

class AFX_EXT_CLASS CIPCServerControlClientSide : public CIPCServerControl
{
	// construction / destruction
public:
	CIPCServerControlClientSide(ServerControlGroup g,
								const _TCHAR *unitName,
								const CConnectionRecord &c
								);
	// for updating files
	CIPCServerControlClientSide(LPCTSTR sShmName,
								int iDestination,
								const CString &sFileName,
								const void *pData,
								DWORD dwDataLen,
								BOOL bNeedsReboot);
    virtual ~CIPCServerControlClientSide();

	// attributes
public:
	inline const CIPCFetchResult &GetResult() const;
	inline HANDLE ResultEvent() const;

	// operations
public:
	void Cancel();
	static BOOL FetchServerReset(const CConnectionRecord &c);
	static BOOL FetchAlarmConnection(const CConnectionRecord &c);
	static BOOL StartVision();

	static BOOL FetchUpdate(int iDestination,
							const CString &sFileName,
							const void *pData,
							DWORD dwDataLen,
							BOOL bNeedsReboot);

public:	
	// overrides
	virtual void OnReadChannelFound();
	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage);
	virtual void OnConfirmConnection();
	
	virtual void OnConfirmInputConnection(CSecID assignedID,
										  const CString& sShmName,
										  const CString& sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , WORD wCodePage
#endif
										  );
	
	virtual void  OnConfirmOutputConnection(CSecID assignedID,
										  const CString& sShmName,
										  const CString& sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , WORD wCodePage
#endif
										  );
	
	virtual void OnConfirmDataBaseConnection(CSecID assignedID,
											const CString& sShmName,
										  const CString& sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , WORD wCodePage
#endif
										  );
	//  NYD
	virtual	void OnConfirmDCConnection(const CString &shmNameDC,
							 		   const CString &sVersion,
									   DWORD dwOptions
#ifdef _UNICODE
									   , WORD wCodePage
#endif
									   );
	virtual void OnConfirmServerReset();
	//
	virtual void OnConfirmAlarmConnection(const CString &shmNameInput,
										  const CString &shmNameOutput,
										  const CString &sBitrate,
										  const CString &sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , WORD wCodePage
#endif
										  );

	virtual void OnConfirmFileUpdate(const CString &s);
	
	//
	virtual void  OnConfirmAudioConnection(CSecID assignedID,
										  const CString& sShmName,
										  const CString& sVersion,
										  DWORD dwOptions
#ifdef _UNICODE
										  , WORD wCodePage
#endif
										  );

	void SetConnectTimeout(DWORD dwTimeout) { m_dwTimeout=dwTimeout; }
	void SetWriteChannelRetries(int iNumRetries) { m_iNumWriteChannelRetries = iNumRetries; }

protected:
	static void CalcRemoteNames(const CString &sRemoteIn,
								CString & sUnitName, 
								CString & sModulName,
								ServerControlGroup group);

	static void ModuleNotRunning(CIPCFetchResult &result, const CString &sModule);


protected:
	virtual BOOL OnRetryWriteChannel();

private:
	CIPCServerControlClientSide(const CIPCServerControlClientSide& src);// no implementation, to avoid implicit generation!
	void operator=(const CIPCServerControlClientSide& src);				// no implementation, to avoid implicit generation!

	void Init();
	// call data:
	ServerControlGroup m_group;
	CConnectionRecord m_connection;
	CString m_sRemote;
	DWORD m_dwOptions;
	DWORD m_dwTimeout;
	//
	CIPCFetchResult m_frResult;
	CEvent			m_evResult;
	BOOL m_bCancel;
	
	int	m_iNumWriteChannelRetries;
	//
	CString m_sAlarmShmNameInput;
	CString m_sAlarmShmNameOutput;
	CSecID	m_AlarmCamID;

	// update data
	int m_iDestination;
	CString m_sFileName;
	void *m_pData;
	DWORD m_dwDataLen;
	BOOL m_bNeedsReboot;

	friend class CIPCFetch;
};
/////////////////////////////////////////////////////////////////////////////
inline const CIPCFetchResult &CIPCServerControlClientSide::GetResult() const
{
	return m_frResult;
}
/////////////////////////////////////////////////////////////////////////////
inline HANDLE CIPCServerControlClientSide::ResultEvent() const
{
	return m_evResult;
}
/////////////////////////////////////////////////////////////////////////////
AFX_EXT_CLASS CIPCFetchResult NewFetchInputConnection(const CConnectionRecord &c,
													  int iNumWriteChannelRetries=0);
/////////////////////////////////////////////////////////////////////////////
AFX_EXT_CLASS CString FetchOutputConnection(const CConnectionRecord &c,
											DWORD &dwOutputOptionsReturned,
											CSecID &assignedID);
/////////////////////////////////////////////////////////////////////////////
AFX_EXT_CLASS CIPCFetchResult NewFetchOutputConnection(const CConnectionRecord &c,
													   int iNumWriteChannelRetries=0);

/////////////////////////////////////////////////////////////////////////////
AFX_EXT_CLASS CIPCFetchResult NewFetchDataBaseConnection(const CConnectionRecord &c,
														 int iNumWriteChannelRetries=0);
/////////////////////////////////////////////////////////////////////////////

#endif
