/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: UpdateRecord.h $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/UpdateRecord.h $
// CHECKIN:		$Date: 25.02.04 16:15 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 25.02.04 15:19 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _UpdateRecord_H_
#define _UpdateRecord_H_

//////////////////////////////////////////////////////////////////////////////////////

class CUpdateRecord
{
	// construction/destruction
public:
	CUpdateRecord(	int iDestination,
					const CString &sFileName,
					const void *pData,
					DWORD dwDataLen,
					BOOL bNeedsReboot);
	~CUpdateRecord();

	// access
public:
	inline CString  GetDestination() const;
	inline BOOL		NeedsReboot() const;
	inline CString	GetStringCommand() const;
	inline BOOL		IsEmpty() const;

	// operations
public:
	void SaveTemporary();
	BOOL TemporaryToOriginal();
	BOOL TemporaryTo(CString sDestination);
	void DeleteTemporary();
	BOOL MultipleUpdate();
	BOOL Setup(const CString& sCommand);

	// implementation
protected:
	BOOL Decompress();

	// private members
private:
	int		m_iDestination;
	CString m_sFileName;
	LPVOID  m_pData;
	DWORD	m_dwDataLen;
	BOOL	m_bNeedsReboot;
	CString m_sDestinationFileName;
	CString m_sTemporaryFileName;

	CString m_sStringCommand;
};
/////////////////////////////////////////////////////////////////////////////
inline CString CUpdateRecord::GetDestination() const
{
	return m_sDestinationFileName;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CUpdateRecord::NeedsReboot() const
{
	return m_bNeedsReboot;
}
/////////////////////////////////////////////////////////////////////////////
inline CString	CUpdateRecord::GetStringCommand() const
{
	return m_sStringCommand;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CUpdateRecord::IsEmpty() const
{
	return m_pData == NULL; 
}

/////////////////////////////////////////////////////////////////////////////
#endif
