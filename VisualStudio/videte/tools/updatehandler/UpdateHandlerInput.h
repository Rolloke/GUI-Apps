/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: UpdateHandlerInput.h $
// ARCHIVE:		$Archive: /Project/Tools/UpdateHandler/UpdateHandlerInput.h $
// CHECKIN:		$Date: 3/16/06 4:44p $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 3/16/06 1:42p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _CUpdateHandlerInput_H_
#define _CUpdateHandlerInput_H_

/////////////////////////////////////////////////////////////////////////////
class CDirContent;
/////////////////////////////////////////////////////////////////////////////
class CUpdateHandlerInput : public CIPCInput
{
	// construction / destruction
public:
	CUpdateHandlerInput(LPCTSTR shmName, CSecID id
#ifdef _UNICODE
		, WORD wCodePage
#endif
		);
	~CUpdateHandlerInput();

	// access
public:
	inline const DWORD GetBitrate() const;
	const CDirContent* GetDirContent(const CString& sDir, POSITION &posFound) const;
	void	DeleteDirContent(CDirContent*);

public:
	BOOL  RemoveFileFromDirContent(const CString& sFile);
	inline const CString& GetHost() const;
	void  SetHost(const CString& sHost);

	// CIPC overrides
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual void OnConfirmBitrate(DWORD dwBitrate);

	virtual void OnConfirmGetFile(
				int iDestination,
				const CString &sFileName,
				const void *pData,
				DWORD dwDataLen, DWORD dwCodePage);
	virtual void OnConfirmFileUpdate(const CString &sFileName);

	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage);

	// private data members
private:
	CSecID			m_ID;
	DWORD			m_dwBitrate;

	CPtrList		m_DirectoryInfo;
	CString			m_sHost;

public:
	OSVERSIONINFO   m_OSVersionInfo;
	SYSTEM_INFO		m_SystemInfo;
	MEMORYSTATUS    m_MemoryStatus;
	CString			m_SoftwareVersion;
	CString			m_DriveInfo;
	CString			m_sCurrentTime;
	CString			m_sLastError;

	CString			m_sLastGot;
	CString			m_sLastWrittenTo;
	CString			m_sLastDeleted;
	CString			m_sLastExecuted;
	CString			m_sRemoteTemp;

	CString			m_RegInfo;
};
inline const DWORD CUpdateHandlerInput::GetBitrate() const
{
	return m_dwBitrate;
}
inline const CString& CUpdateHandlerInput::GetHost() const
{
	return m_sHost;
}

#endif	// _CUpdateHandlerInput_H_